import sys, getopt
import os
import struct
import ctypes
import binascii
import hashlib 
import zlib
import calendar
import time

from gmssl import sm3, func

# #define GROUPSPLIT_BLOCKSIZE           (512)

# #define GROUPSPLIT_MAGIC_0    0x48504D2DUL /* HPM- */
# #define GROUPSPLIT_MAGIC_1    0x9E5D5157UL /* Randomly selected */
# #define GROUPSPLIT_END       0x48454E44UL /* HEND */

# typedef struct
# {
#     uint32_t magic0;
#     uint32_t magic1;
#     uint32_t index;
#     uint32_t datasize;
#     uint8_t data[492];
#     uint32_t magicend;
# } hpm_groupsplit_pack_t;

#define HPM_APP_HASH_DATA_MAXSIZE   (64)

#define HPM_APP_FILE_FLAG_MAGIC   (0xbeaf5aa5)
#define HPM_APP_FILE_TOUCH_ID     (0xa5a55a5a)

# typedef enum
# {
#     HPM_APP1   = 0,
#     HPM_APP2,
#     HPM_APPMAX
# } HPM_APP_INDEX;


# /*hpm app header structure*/
# typedef struct
# {
#     uint32_t magic;
#     uint32_t touchid;
#     uint32_t device;
#     uint32_t len;
#     uint32_t version;
#     uint8_t type;
#     uint8_t hash_enable;
#     uint8_t pwr_hash;
#     uint8_t hash_type;
#     uint32_t reserved;
#     uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];
# } hpm_app_header_t;

HPM_USB_DEVICE_FILE_NAME = "usb_device_update.upd"
HPM_MERGE_BOOTUSER_APPS_NAME = "merge_bootuser_app1.bin"

GROUPSPLIT_BLOCKSIZE    =       (512)

GROUPSPLIT_MAGIC_0      =  0x48504D2D #/* HPM- */
GROUPSPLIT_MAGIC_1      =  0x9E5D5157 #/* Randomly selected */
GROUPSPLIT_END          =  0x48454E44  #/* HEND */

HPM_APP_FILE_FLAG_MAGIC = 0xbeaf5aa5
HPM_APP_FILE_TOUCH_ID = 0xa5a55a5a

HEAD_BACKUP_OFFSET = 92

def convert_to_usbupd(file_content):
    global familyid
    datapadding = b""
    while len(datapadding) < 512 - 492 - 16 - 4:
        datapadding += b"\x00\x00\x00\x00"
    numblocks = (len(file_content) + 491) // 492
    outp = []
    for blockno in range(numblocks):
        ptr = 492 * blockno
        chunk = file_content[ptr:ptr + 492]
        hd = struct.pack(b"<IIII",
            GROUPSPLIT_MAGIC_0, GROUPSPLIT_MAGIC_1, blockno, len(chunk))
        while len(chunk) < 492:
            chunk += b"\x00"
        block = hd + chunk + struct.pack(b"<I", GROUPSPLIT_END)
        assert len(block) == 512
        outp.append(block)
    return b"".join(outp)

def makeup_udp(isusb, type, firmwarefile, otafile):
    otafile_obj = open(otafile, 'wb+')

    firmware_obj = open(firmwarefile, 'rb')
    firmware_obj.seek(HEAD_BACKUP_OFFSET,0)
    firmware_binary = firmware_obj.read()

    otafile_obj.seek(HEAD_BACKUP_OFFSET,0)
    otafile_obj.write(firmware_binary)

    fota_magic = HPM_APP_FILE_FLAG_MAGIC
    touch_id = HPM_APP_FILE_TOUCH_ID
    fota_device = 0x0048504D # "hpm"
    
    utc_time = calendar.timegm(time.gmtime())
    print("version:", utc_time)
    version = utc_time
    fota_len = len(firmware_binary)
    firm_type = 0
    hash_enable=1
    pwr_hash=1
    hash_type = type
    reserved = 0
    int_data = 0
    hash_data = ''
    print ("fota_len: ", fota_len)
    fota_head = ctypes.create_string_buffer(HEAD_BACKUP_OFFSET)
    struct.pack_into('<I', fota_head, 0, fota_magic)
    struct.pack_into('<I', fota_head, 4, touch_id)
    struct.pack_into('<I', fota_head, 8, fota_device)
    struct.pack_into('<I', fota_head, 12, fota_len)
    # H
    struct.pack_into('<I', fota_head, 16, version)
    struct.pack_into('<B', fota_head, 20, firm_type)
    struct.pack_into('<B', fota_head, 21, hash_enable)
    struct.pack_into('<B', fota_head, 22, pwr_hash)
    struct.pack_into('<B', fota_head, 23, hash_type)
    struct.pack_into('<I', fota_head, 24, reserved)

    if (type == 0):
        for val in list(firmware_binary):
            if (int_data > 4294967295):
                int_data = 0;
            int_data += val
        print ("int_data:", int_data)
        struct.pack_into('<I', fota_head, 28, int_data)
    elif (type == 1):
        for val in list(firmware_binary):
            int_data ^= val
        print ("int_data:", int_data)
        struct.pack_into('<I', fota_head, 28, int_data)
    elif (type == 2):
        # int_data = (binascii.crc32(firmware_binary) & 0xFFFFFFFF)
        int_data = zlib.crc32(firmware_binary) & 0xFFFFFFFF
        print ("int_data:", int_data)
        struct.pack_into('<I', fota_head, 28, int_data)
    elif (type == 3):
        sha1 = hashlib.sha1()
        sha1.update(firmware_binary)
        hash_data = sha1.hexdigest()
        print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(20):
            struct.pack_into('B', fota_head, 28+i, hash_arry[i])
    elif (type == 4):
        sha256 = hashlib.sha256()
        sha256.update(firmware_binary)
        hash_data = sha256.hexdigest()
        print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(32):
            struct.pack_into('B', fota_head, 28+i, hash_arry[i])
    elif (type == 5):
        hash_data = sm3.sm3_hash(func.bytes_to_list(firmware_binary))
        print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(32):
            struct.pack_into('B', fota_head, 28+i, hash_arry[i])
    else:
        print ("ERROR: no support the type:", type, "!")
        return


    otafile_obj.seek(0,0)
    otafile_obj.write(fota_head)

    otafile_obj.close()
    fota_len = os.path.getsize(otafile)

    if(isusb):
        usbdevicefile_obj = open(HPM_USB_DEVICE_FILE_NAME, 'wb+')
        signfile_obj = open(otafile, 'rb')

        signfile_obj.seek(0,0)
        signfile_binary = signfile_obj.read()
        usbdevice_binary = convert_to_usbupd(signfile_binary)
        usbdevicefile_obj.seek(0,0)
        usbdevicefile_obj.write(usbdevice_binary)
        signfile_obj.close()
        usbdevicefile_obj.close()

FLASH_ADDR_BASE       =   (0x80000000)
FLASH_START_ADDR      =   (0)
FLASH_MAX_SIZE        =   (0x400000)

FLASH_EXIP_INFO_ADDR  =   (FLASH_START_ADDR)
FLASH_EXIP_INFO_SIZE  =   (0x400)

FLASH_NORCFG_OPTION_ADDR = (FLASH_EXIP_INFO_ADDR + FLASH_EXIP_INFO_SIZE)
FLASH_NORCFG_OPTION_SIZE = (0xC00)

FLASH_BOOT_HEADER_ADDR   = (FLASH_NORCFG_OPTION_ADDR + FLASH_NORCFG_OPTION_SIZE)
FLASH_BOOT_HEADER_SIZE   = (0x2000)

FLASH_BOOT_USER_ADDR     = (FLASH_BOOT_HEADER_ADDR + FLASH_BOOT_HEADER_SIZE) 
FLASH_BOOT_USER_SIZE     = (0x40000)

FLASH_USER_APP1_ADDR     = (FLASH_BOOT_USER_ADDR + FLASH_BOOT_USER_SIZE)
FLASH_USER_APP1_SIZE     = (0x100000)

def merge_burnfile(isexip, bootuserbin, otafilebin):
    bootuser_obj = open(bootuserbin, 'rb')
    bootuser_obj.seek(0,0)
    bootuser_binary = bootuser_obj.read()

    otafile_obj = open(otafilebin, 'rb')
    otafile_obj.seek(0,0)
    otafile_binary = otafile_obj.read()

    burnfile_obj = open(HPM_MERGE_BOOTUSER_APPS_NAME, 'wb+')
    burnfile_obj.seek(0,0)

    cnt = 0
    #init full 0xff
    if(isexip) :
        while (cnt < (FLASH_USER_APP1_ADDR)):
            burnfile_obj.write(b'\xff')
            cnt += 1
    else:
        while (cnt < (FLASH_USER_APP1_ADDR - FLASH_EXIP_INFO_SIZE)):
            burnfile_obj.write(b'\xff')
            cnt += 1

    burnfile_obj.seek(0,0)
    burnfile_obj.write(bootuser_binary)

    if(isexip) :
        burnfile_obj.seek(FLASH_USER_APP1_ADDR,0)
    else:
        burnfile_obj.seek(FLASH_USER_APP1_ADDR - FLASH_EXIP_INFO_SIZE,0)
    burnfile_obj.write(otafile_binary)
    burnfile_obj.close()
    print ("merge boot and apps success! -> :merge_bootuser_app1.bin")


def help():
    print("---------help------------")
    print("一共6个参数")
    print("第一个参数：此脚本文件 pack_ota.py")
    print("第二个参数：签名type类型")
    print("           0-checksum 校验和校验")
    print("           1-xor 异或校验")
    print("           2-CRC32 校验")
    print("           3-SHA1 校验")
    print("           4-SHA256 校验")
    print("           5-SM3 校验")
    print("第三个参数：原始固件路径")
    print("第四个参数：目标OTA路径")
    print("第五个参数(可缺省)：BOOTUSER类型：0:正常固件, 1:exip加密固件")
    print("第六个参数(可缺省)：BOOTUSER固件路径")
    print("")
    print("举例：")
    print("     python pack_ota.py 0 demo.bin update_sign.bin 0 bootuser.bin")
    print("     python pack_ota.py 1 demo.bin update_sign.bin 0 bootuser.bin")
    print("     python pack_ota.py 2 demo.bin update_sign.bin 0 bootuser.bin")
    print("     python pack_ota.py 3 demo.bin update_sign.bin 0 bootuser.bin")
    print("     python pack_ota.py 4 demo.bin update_sign.bin 0 bootuser.bin")
    print("     python pack_ota.py 5 demo.bin update_sign.bin 0 bootuser.bin")
    print(" 同时会生成基于USB DEVICE的OTA包(usb_device_update.upd)")
    print(" 若包含第5/6参数，同时会生成bootuser+userapp合并的用于烧录的固件: %s " %(HPM_MERGE_BOOTUSER_APPS_NAME))
    print("-------------------------")

def main(argv):
    argc = len(argv)
    if (argc < 4):
        help()
        return
    if (int(argv[1]) > 5):
        help()
        return
    if (argc == 4) :
        print ("0:", argv[0], ",1:", argv[1], ",2:", argv[2], ",3:", argv[3])
    else:
        print ("0:", argv[0], ",1:", argv[1], ",2:", argv[2], ",3:", argv[3], ",4:", argv[4], ",5:", argv[5])

    makeup_udp(1, int(argv[1]), argv[2], argv[3])
    print ("type:%d, pack %s -> %s Success!!!" %(int(argv[1]), argv[2],argv[3]))
    print ("USB device: %s make Success!!!" %(HPM_USB_DEVICE_FILE_NAME))

    if (argc == 6) :
        merge_burnfile(int(argv[4]), argv[5], argv[3])
        print ("merge bootuser+app: %s make Success!!!" %(HPM_MERGE_BOOTUSER_APPS_NAME))

if __name__ == '__main__':
    main(sys.argv)