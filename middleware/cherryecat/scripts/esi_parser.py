#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ESI(EtherCAT Slave Information) to EEPROM Binary Converter

Copyright (c) 2025, sakumisu

SPDX-License-Identifier: Apache-2.0

"""

import xml.etree.ElementTree as ET
import struct
import sys
import os
from typing import Dict, List, Tuple, Optional

class EtherCATXMLParser:
    def __init__(self):
        # 设备基本信息
        self.vendor_id = 0x00000000  # 默认厂商ID
        self.product_code = 0x00000000  # 默认产品代码
        self.revision_no = 0x00000000  # 默认版本号
        self.serial_number = 0x00000000  # 序列号
        self.device_name = ""
        self.device_type = ""

        # 邮箱配置
        self.mailbox_protocols = 0x0
        self.boot_rx_mailbox = {}
        self.boot_tx_mailbox = {}
        self.std_rx_mailbox = {}
        self.std_tx_mailbox = {}

        # 字符串表
        self.strings = []

        # 类别数据
        self.categories = []

    def parse_hex_value(self, hex_str: str) -> int:
        """解析十六进制字符串"""
        if not hex_str:
            return 0
        hex_str = hex_str.strip()
        if hex_str.startswith('#x'):
            return int(hex_str[2:], 16)
        elif hex_str.startswith('0x'):
            return int(hex_str[2:], 16)
        else:
            try:
                return int(hex_str, 16)
            except:
                return int(hex_str, 10)

    def parse_device_info(self, device_elem):
        """解析设备基本信息"""
        # 获取产品代码和版本号
        type_elem = device_elem.find('Type')
        if type_elem is not None:
            product_code = type_elem.get('ProductCode')
            if product_code:
                self.product_code = self.parse_hex_value(product_code)

            revision_no = type_elem.get('RevisionNo')
            if revision_no:
                self.revision_no = self.parse_hex_value(revision_no)

        # 获取设备名称
        name_elem = device_elem.find('Name')
        if name_elem is not None and name_elem.text:
            self.device_name = name_elem.text.strip()

        # 获取设备类型
        type_name = device_elem.find('Type/Name')
        if type_name is not None and type_name.text:
            self.device_type = type_name.text.strip()

    def parse_vendor_info(self, vendor_elem):
        """解析厂商信息"""
        vendor_id_elem = vendor_elem.find('Id')
        if vendor_id_elem is not None and vendor_id_elem.text:
            self.vendor_id = self.parse_hex_value(vendor_id_elem.text)

    def parse_mailbox_info(self, device_elem):
        """解析邮箱信息"""
        mailbox_elem = device_elem.find('.//Mailbox')
        if mailbox_elem is not None:
            # 检查支持的协议
            self.mailbox_protocols = 0

            if mailbox_elem.find('CoE') is not None:
                self.mailbox_protocols |= 0x04  # CoE
            if mailbox_elem.find('FoE') is not None:
                self.mailbox_protocols |= 0x08  # FoE
            if mailbox_elem.find('EoE') is not None:
                self.mailbox_protocols |= 0x10  # EoE
            if mailbox_elem.find('SoE') is not None:
                self.mailbox_protocols |= 0x20  # SoE

        # 从SM配置中获取邮箱地址和大小
        sm_elems = device_elem.findall('.//Sm')
        for i, sm_elem in enumerate(sm_elems):
            start_addr = self.parse_hex_value(sm_elem.get('StartAddress', '0'))
            size = self.parse_hex_value(sm_elem.get('DefaultSize', '0'))

            if i == 0:  # MBoxOut (接收)
                self.boot_rx_mailbox = {"offset": start_addr, "size": size}
                self.std_rx_mailbox = {"offset": start_addr, "size": size}
            elif i == 1:  # MBoxIn (发送)
                self.boot_tx_mailbox = {"offset": start_addr, "size": size}
                self.std_tx_mailbox = {"offset": start_addr, "size": size}

    def add_string(self, text: str) -> int:
        """添加字符串到字符串表，返回索引"""
        if not text:
            return 0

        # 检查是否已存在
        for i, existing in enumerate(self.strings):
            if existing == text:
                return i + 1

        # 添加新字符串
        self.strings.append(text)
        return len(self.strings)

    def create_strings_category(self) -> bytes:
        """创建字符串类别(Category 10)"""
        if not self.strings:
            return b''

        data = bytearray()

        # 字符串数量
        data.append(len(self.strings))

        # 每个字符串: 长度 + 内容
        for string in self.strings:
            string_bytes = string.encode('ascii', errors='replace')
            data.append(len(string_bytes))
            data.extend(string_bytes)

        # 填充到偶数长度
        if len(data) % 2:
            data.append(0)

        return bytes(data)

    def create_general_category(self) -> bytes:
        """创建通用类别(Category 30)"""
        data = bytearray()

        # Group Type String Index (2 bytes)
        group_idx = self.add_string("ECAT_Device")
        data.extend(struct.pack('<H', group_idx))

        # Image Name String Index (2 bytes)
        image_idx = self.add_string("ECAT_CIA402")
        data.extend(struct.pack('<H', image_idx))

        # Order Number String Index (2 bytes)
        order_idx = self.add_string("")
        data.extend(struct.pack('<H', order_idx))

        # Device Name String Index (2 bytes)
        name_idx = self.add_string(self.device_name)
        data.extend(struct.pack('<H', name_idx))

        # CoE Details (2 bytes) - 支持SDO, PDO配置
        coe_details = 0x0027  # Enable SDO, SDO Info, PDO Assign, PDO Config
        data.extend(struct.pack('<H', coe_details))

        # FoE Details (2 bytes)
        foe_details = 0x0000
        data.extend(struct.pack('<H', foe_details))

        # EoE Details (2 bytes)
        eoe_details = 0x0000
        data.extend(struct.pack('<H', eoe_details))

        # SoE Channels (1 byte)
        soe_channels = 0x00
        data.append(soe_channels)

        # DS402 Channels (1 byte)
        ds402_channels = 0x01
        data.append(ds402_channels)

        # SysmanClass (1 byte)
        sysman_class = 0x00
        data.append(sysman_class)

        # Flags (1 byte)
        flags = 0x01  # Enable SafeOp
        data.append(flags)

        # Current Consumption (2 bytes)
        current = 0x0000
        data.extend(struct.pack('<H', current))

        # Group Type and Image Name for 2nd device (if any)
        data.extend(struct.pack('<H', 0x0000))  # Group Type 2
        data.extend(struct.pack('<H', 0x0000))  # Image Name 2

        # Physical Memory Address (2 bytes)
        phys_addr = 0x0000
        data.extend(struct.pack('<H', phys_addr))

        # 填充到偶数长度
        if len(data) % 2:
            data.append(0)

        return bytes(data)

    def create_fmmu_category(self) -> bytes:
        """创建FMMU类别(Category 40)"""
        data = bytearray()

        # FMMU配置 - 8个FMMU
        fmmu_configs = [
            0x01,  # FMMU0: Outputs
            0x02,  # FMMU1: Inputs
            0x03,  # FMMU2: MBox State
            0x00,  # FMMU3: Unused
            0x00,  # FMMU4: Unused
            0x00,  # FMMU5: Unused
            0x00,  # FMMU6: Unused
            0x00,  # FMMU7: Unused
        ]

        for config in fmmu_configs:
            data.append(config)

        return bytes(data)

    def create_sm_category(self) -> bytes:
        """创建同步管理器类别(Category 41)"""
        data = bytearray()

        # SM配置数据结构: StartAddr(2) + Length(2) + ControlByte(1) + Enable(1)
        sm_configs = [
            # SM0: MBoxOut (接收邮箱)
            (self.boot_rx_mailbox["offset"], self.boot_rx_mailbox["size"], 0x26, 0x01),
            # SM1: MBoxIn (发送邮箱)
            (self.boot_tx_mailbox["offset"], self.boot_tx_mailbox["size"], 0x22, 0x01),
            # SM2: Process Data Output
            (0x1100, 0x0000, 0x64, 0x00),  # 长度为0表示未配置
            # SM3: Process Data Input
            (0x1400, 0x0000, 0x20, 0x00),  # 长度为0表示未配置
            # SM4-7: 未使用
            (0x0000, 0x0000, 0x00, 0x00),
            (0x0000, 0x0000, 0x00, 0x00),
            (0x0000, 0x0000, 0x00, 0x00),
            (0x0000, 0x0000, 0x00, 0x00),
        ]

        for start_addr, length, control, enable in sm_configs:
            data.extend(struct.pack('<H', start_addr))  # Start Address
            data.extend(struct.pack('<H', length))      # Length
            data.append(control)                        # Control Byte
            data.append(enable)                         # Enable

        return bytes(data)

    def create_category(self, category_type: int, data: bytes) -> bytes:
        """创建类别头部+数据"""
        header = bytearray()

        # Category Type (2 bytes)
        header.extend(struct.pack('<H', category_type))

        # Category Size in words (2 bytes)
        size_words = (len(data) + 1) // 2
        header.extend(struct.pack('<H', size_words))

        return bytes(header) + data

    def generate_eeprom(self) -> bytes:
        """生成完整的EEPROM数据，参考eeprom.h的格式"""
        eeprom_data = bytearray()

        # === EEPROM Header (固定128字节) ===

        # PDI Control (2 bytes) - 0x800C (Digital I/O + SII EEPROM)
        eeprom_data.extend(struct.pack('<H', 0x800C))

        # PDI Configuration (2 bytes) - 0x6681
        eeprom_data.extend(struct.pack('<H', 0x6681))

        # Sync Impulse Length (2 bytes)
        eeprom_data.extend(struct.pack('<H', 0x0000))

        # PDI Configuration 2 (2 bytes)
        eeprom_data.extend(struct.pack('<H', 0x0000))

        # Station Alias (2 bytes)
        eeprom_data.extend(struct.pack('<H', 0x3412))

        # Reserved (2 bytes)
        eeprom_data.extend(struct.pack('<H', 0x0000))

        # Checksum (2 bytes) - 稍后计算
        checksum_pos = len(eeprom_data)
        eeprom_data.extend(struct.pack('<H', 0x0077))  # 临时值

        # Vendor ID (4 bytes)
        eeprom_data.extend(struct.pack('<L', self.vendor_id))

        # Product Code (4 bytes)
        eeprom_data.extend(struct.pack('<L', self.product_code))

        # Revision Number (4 bytes)
        eeprom_data.extend(struct.pack('<L', self.revision_no))

        # Serial Number (4 bytes)
        eeprom_data.extend(struct.pack('<L', self.serial_number))

        # Bootstrap Mailbox Receive Offset (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.boot_rx_mailbox["offset"]))

        # Bootstrap Mailbox Receive Size (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.boot_rx_mailbox["size"]))

        # Bootstrap Mailbox Send Offset (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.boot_tx_mailbox["offset"]))

        # Bootstrap Mailbox Send Size (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.boot_tx_mailbox["size"]))

        # Standard Mailbox Receive Offset (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.std_rx_mailbox["offset"]))

        # Standard Mailbox Receive Size (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.std_rx_mailbox["size"]))

        # Standard Mailbox Send Offset (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.std_tx_mailbox["offset"]))

        # Standard Mailbox Send Size (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.std_tx_mailbox["size"]))

        # Mailbox Protocol (2 bytes)
        eeprom_data.extend(struct.pack('<H', self.mailbox_protocols))

        # Reserved bytes to reach 128 bytes header
        current_size = len(eeprom_data)
        header_size = 128
        if current_size < header_size:
            eeprom_data.extend(b'\x00' * (header_size - current_size))

        # === Categories Section ===

        # Category 10: Strings
        strings_data = self.create_strings_category()
        if strings_data:
            eeprom_data.extend(self.create_category(10, strings_data))

        # Category 30: General
        general_data = self.create_general_category()
        eeprom_data.extend(self.create_category(30, general_data))

        # Category 40: FMMU
        fmmu_data = self.create_fmmu_category()
        eeprom_data.extend(self.create_category(40, fmmu_data))

        # Category 41: SyncM
        sm_data = self.create_sm_category()
        eeprom_data.extend(self.create_category(41, sm_data))

        # End of Categories marker
        eeprom_data.extend(struct.pack('<H', 0xFFFF))
        eeprom_data.extend(struct.pack('<H', 0x0000))

        # 填充到合适的大小 (通常是2KB)
        target_size = 2048
        if len(eeprom_data) < target_size:
            eeprom_data.extend(b'\xFF' * (target_size - len(eeprom_data)))

        # 重新计算校验和 (SII头部校验和)
        checksum = 0
        # 计算前14字节的校验和，跳过校验和字段本身
        for i in range(0, 14, 2):
            if i != 12:  # 跳过校验和位置
                word = struct.unpack('<H', eeprom_data[i:i+2])[0]
                checksum += word

        checksum = (~checksum + 1) & 0xFFFF  # 2's complement

        # 更新校验和
        struct.pack_into('<H', eeprom_data, checksum_pos, checksum)

        return bytes(eeprom_data)

    def parse_xml(self, xml_file: str) -> bool:
        """解析XML文件"""
        try:
            tree = ET.parse(xml_file)
            root = tree.getroot()

            # 查找并解析厂商信息
            vendor_elem = root.find('.//Vendor')
            if vendor_elem is not None:
                self.parse_vendor_info(vendor_elem)

            # 查找并解析设备信息
            device_elem = root.find('.//Device')
            if device_elem is not None:
                self.parse_device_info(device_elem)
                self.parse_mailbox_info(device_elem)

            print(f"Parsed XML: Vendor=0x{self.vendor_id:08X}, Product=0x{self.product_code:08X}")
            print(f"Device Name: {self.device_name}")
            print(f"Mailbox RX: 0x{self.std_rx_mailbox['offset']:04X}({self.std_rx_mailbox['size']})")
            print(f"Mailbox TX: 0x{self.std_tx_mailbox['offset']:04X}({self.std_tx_mailbox['size']})")

            return True

        except Exception as e:
            print(f"Error parsing XML file: {e}")
            import traceback
            traceback.print_exc()
            return False

    def generate_c_header(self, array_name: str = "cherryecat_eepromdata") -> str:
        """生成C语言头文件格式的数组"""
        eeprom_data = self.generate_eeprom()

        lines = [
            "/*",
            f"The EEPROM data is created based on EtherCAT Slave Information (ESI) XML file.",
            f"Generated {len(eeprom_data)} bytes of EEPROM data",
            f"Vendor ID: 0x{self.vendor_id:08X}",
            f"Product Code: 0x{self.product_code:08X}",
            f"Revision: 0x{self.revision_no:08X}",
            f"Device Name: {self.device_name}",
            "*/",
            f"unsigned char {array_name}[] = {{",
        ]

        # 按16字节一行格式化数据
        for i in range(0, len(eeprom_data), 16):
            chunk = eeprom_data[i:i+16]
            hex_values = [f"0x{b:02X}" for b in chunk]
            line = ",".join(hex_values)
            if i + 16 < len(eeprom_data):
                line += ","
            lines.append(line)

        lines.append("};")

        return "\n".join(lines)

def main():
    if len(sys.argv) < 3:
        print("Usage: python esi_parse.py <input.xml> <output.bin> [output.h]")
        print("  input.xml  - EtherCAT ESI XML file")
        print("  output.bin - Output binary EEPROM file")
        print("  output.h   - Optional C header file output")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    header_file = sys.argv[3] if len(sys.argv) > 3 else None

    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found")
        sys.exit(1)

    # 创建解析器
    parser = EtherCATXMLParser()

    # 解析XML
    print(f"Parsing XML file: {input_file}")
    if not parser.parse_xml(input_file):
        print("Failed to parse XML file")
        sys.exit(1)

    # 生成EEPROM数据
    print("Generating EEPROM data...")
    eeprom_data = parser.generate_eeprom()

    # 写入二进制文件
    try:
        with open(output_file, 'wb') as f:
            f.write(eeprom_data)

        print(f"✓ Successfully converted '{input_file}' to '{output_file}'")
        print(f"✓ Generated {len(eeprom_data)} bytes of EEPROM data")
        print(f"✓ Vendor ID: 0x{parser.vendor_id:08X}")
        print(f"✓ Product Code: 0x{parser.product_code:08X}")
        print(f"✓ Revision: 0x{parser.revision_no:08X}")
        print(f"✓ Device Name: {parser.device_name}")

    except Exception as e:
        print(f"Error writing binary file: {e}")
        sys.exit(1)

    # 生成C头文件(可选)
    if header_file:
        try:
            header_content = parser.generate_c_header()
            with open(header_file, 'w') as f:
                f.write(header_content)
            print(f"✓ Generated C header file: {header_file}")
        except Exception as e:
            print(f"Error writing header file: {e}")

if __name__ == "__main__":
    main()
