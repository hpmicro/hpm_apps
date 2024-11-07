import sys, getopt
import os
import struct
import ctypes

# ---- flash map
FW_OTP_SEC_IMG2_OFFSET           =  (2) # 2X256K = 512K
FW_OTA_SEC_MAX_IMG_LEN           =  (2) # 2X256K = 512K

FW_HEADER_SW_VERSION_OFFSET      =  (0x08)

FLASH_ADDR_BASE                  =  (0x80000000)

FLASH_START_ADDR                 =  (0 + FLASH_ADDR_BASE)                              # 0x80000000
FLASH_MAX_SIZE                   =  (0x200000)                                         # 2M

#APP1 -------------
#APP1 ALL IMG
FLASH_APP1_ALL_IMG_ADDR          =   (FLASH_START_ADDR)
FLASH_APP1_ALL_IMG_SIZE          =   ((FW_OTP_SEC_IMG2_OFFSET*256*1024))

FLASH_APP1_EXIP_INFO_ADDR        =   (FLASH_APP1_ALL_IMG_ADDR)
FLASH_APP1_EXIP_INFO_SIZE        =   (0x400)

FLASH_APP1_NOR_CFG_ADDR          =   (FLASH_APP1_EXIP_INFO_ADDR + FLASH_APP1_EXIP_INFO_SIZE)
FLASH_APP1_NOR_CFG_SIZE          =   (0xC00)

FLASH_APP1_BOOT_HEAD_ADDR        =   (FLASH_APP1_NOR_CFG_ADDR + FLASH_APP1_NOR_CFG_SIZE)
FLASH_APP1_BOOT_HEAD_SIZE        =   (0x2000)

FLASH_APP1_USER_IMG_ADDR         =   (FLASH_APP1_BOOT_HEAD_ADDR + FLASH_APP1_BOOT_HEAD_SIZE)
FLASH_APP1_USER_IMG_SIZE         =   (FLASH_APP1_ALL_IMG_SIZE-(FLASH_APP1_EXIP_INFO_SIZE+FLASH_APP1_NOR_CFG_SIZE+FLASH_APP1_BOOT_HEAD_SIZE))
#-----------------------------

#APP2 --------------------
#APP1 ALL IMG
FLASH_APP2_ALL_IMG_ADDR          =   (FLASH_START_ADDR + FLASH_APP1_ALL_IMG_SIZE)
FLASH_APP2_ALL_IMG_SIZE          =   ((FW_OTP_SEC_IMG2_OFFSET*256*1024))

FLASH_APP2_EXIP_INFO_ADDR        =   (FLASH_APP2_ALL_IMG_ADDR)
FLASH_APP2_EXIP_INFO_SIZE        =   (0x400)

FLASH_APP2_NOR_CFG_ADDR          =   (FLASH_APP2_EXIP_INFO_ADDR + FLASH_APP2_EXIP_INFO_SIZE)
FLASH_APP2_NOR_CFG_SIZE          =   (0xC00)

FLASH_APP2_BOOT_HEAD_ADDR        =   (FLASH_APP2_NOR_CFG_ADDR + FLASH_APP2_NOR_CFG_SIZE)
FLASH_APP2_BOOT_HEAD_SIZE        =   (0x2000)

FLASH_APP2_USER_IMG_ADDR         =   (FLASH_APP2_BOOT_HEAD_ADDR + FLASH_APP2_BOOT_HEAD_SIZE)
FLASH_APP2_USER_IMG_SIZE         =   (FLASH_APP2_ALL_IMG_SIZE-(FLASH_APP2_EXIP_INFO_SIZE+FLASH_APP2_NOR_CFG_SIZE+FLASH_APP2_BOOT_HEAD_SIZE))
#------------------------

#APP1 IMGINFO,checksum todo
FLASH_APP1_IMGINFO_ADDR          =   (FLASH_APP2_USER_IMG_ADDR + FLASH_APP2_USER_IMG_SIZE)
FLASH_APP1_IMGINFO_SIZE          =   (0x1000)

#APP2 IMGINFO, checksum todo
FLASH_APP2_IMGINFO_ADDR          =   (FLASH_APP1_IMGINFO_ADDR + FLASH_APP1_IMGINFO_SIZE)
FLASH_APP2_IMGINFO_SIZE          =   (0x1000)

FLASH_USER_COMMON_ADDR           =   (FLASH_APP2_IMGINFO_ADDR + FLASH_APP2_IMGINFO_SIZE)
FLASH_USER_COMMON_SIZE           =   (0x80000) 
# flash map

HEAD_BACKUP_OFFSET = 16
FMR_BOOTHEAD_OFFSET = 0
FMR_SW_VERSION_OFFSET = HEAD_BACKUP_OFFSET + 0x1000 + 8
FMR_EXIP_EMPTY_OFFSET = 0x400
FMR_NOT_SW_VERSION_OFFSET = 0xC00 + 8
FMR_EXIP_SW_VERSION_OFFSET = 0x1000 + 8

def makeup_ota(type, firmwarefile, otafile, burnfile):
    otafile_obj = open(otafile, 'wb+')

    firmware_obj = open(firmwarefile, 'rb')
    firmware_obj.seek(FMR_BOOTHEAD_OFFSET,0)
    firmware_binary = firmware_obj.read()
    firmware_obj.close()

    
    otafile_obj.seek(HEAD_BACKUP_OFFSET,0)
    if type == 0 :
        cnt = 0
        while (cnt < FMR_EXIP_EMPTY_OFFSET):
            otafile_obj.write(b'\xff')
            cnt += 1
        otafile_obj.seek(FMR_EXIP_EMPTY_OFFSET + HEAD_BACKUP_OFFSET,0)
    
    otafile_obj.write(firmware_binary)

    fota_magic = 0xbeaf5aa5
    fota_device = 0x6750
    fota_checksum = 0
    fota_len = 0
    if type == 0:
        fota_len = os.path.getsize(firmwarefile) + FMR_EXIP_EMPTY_OFFSET -FMR_BOOTHEAD_OFFSET
    else :
        fota_len = os.path.getsize(firmwarefile)-FMR_BOOTHEAD_OFFSET
    print ("fota_len: ", fota_len)

    otafile_obj.seek(HEAD_BACKUP_OFFSET,0)
    firmware_binary = otafile_obj.read()

    for val in list(firmware_binary):
        # fota_checksum += val.encode('hex')
        # fota_checksum += ord(val)
        fota_checksum += val

    fota_head = ctypes.create_string_buffer(16)
    struct.pack_into('<I', fota_head, 0, fota_magic)
    struct.pack_into('<I', fota_head, 4, fota_device)
    struct.pack_into('<I', fota_head, 8, fota_len)
    struct.pack_into('<I', fota_head, 12, fota_checksum)

    otafile_obj.seek(0,0)
    otafile_obj.write(fota_head)

    otafile_obj.close()
    print ("fota_checksum: ", fota_checksum)
    print ("type:", type, ", sign and ota success!")
    print("Generated file :", otafile, ",SUCCESS!")

    #burn file merge
    burnfile_obj = open(burnfile, 'wb+')
    burnfile_obj.seek(0,0)
    offset = 0
    if type == 0 :
        # no exip firm, start 0x400 addr
        offset = FMR_EXIP_EMPTY_OFFSET
    else :
        offset = 0

    firmware_obj = open(firmwarefile, 'rb')
    firmware_obj.seek(FMR_BOOTHEAD_OFFSET,0)
    firmware_binary = firmware_obj.read()
    firmware_obj.close()

    cnt = 0
    #init full 0xff
    while (cnt < (FLASH_APP1_IMGINFO_ADDR + FLASH_APP1_IMGINFO_SIZE - FLASH_ADDR_BASE - offset)):
        burnfile_obj.write(b'\xff')
        cnt += 1

    burnfile_obj.seek(0,0)

    burnfile_obj.write(firmware_binary)

    burnfile_obj.seek(FLASH_APP1_IMGINFO_ADDR-FLASH_ADDR_BASE - offset,0)
    burnfile_obj.write(fota_head)
    burnfile_obj.close()
    print("merge burn file :", burnfile, ",SUCCESS!")

def help():
    print("---------help------------")
    print("一共4个参数")
    print("第一个参数：此脚本文件 pack_sign.py")
    print("第二个参数：type类型")
    print("           0-明文固件签名，生成OTA固件;")
    print("           1-加密EXIP固件签名，生成OTA固件;")
    print("第三个参数：原始固件路径")
    print("")
    print("举例：")
    print("     python pack_sign.py 0 demo.bin")
    print("     python pack_sign.py 1 demo_exip.bin")
    print("-------------------------")

def main(argv):
    argc = len(argv)
    if (argc < 3):
        help()
        return
    print ("0:", argv[0], ",1:", argv[1], ",2:", argv[2])
    
    type = int(argv[1])
    firm = argv[2]
    firm_arry = firm.split('.bin')
    sign_name = firm_arry[0]+"_sign.bin"
    merge_name = firm_arry[0]+"_full_burn.bin"

    if type == 0:
        makeup_ota(0, firm, sign_name, merge_name)
        print ("pack %s -> %s and %s Success!!!" %(firm,sign_name, merge_name))
    elif type == 1:
        makeup_ota(1, firm, sign_name, merge_name)
        print ("pack %s -> %s and %s Success!!!" %(firm,sign_name, merge_name))
    else:
        help()

if __name__ == '__main__':
    main(sys.argv)
