import sys, getopt
import os
import struct
import ctypes


HEAD_BACKUP_OFFSET = 16
FMR_BOOTHEAD_OFFSET = 0
FMR_SW_VERSION_OFFSET = HEAD_BACKUP_OFFSET + 0x1000 + 8
FMR_EXIP_EMPTY_OFFSET = 0x400
FMR_NOT_SW_VERSION_OFFSET = 0xC00 + 8
FMR_EXIP_SW_VERSION_OFFSET = 0x1000 + 8

def makeup_ota(type, firmwarefile, otafile):
    otafile_obj = open(otafile, 'wb+')

    firmware_obj = open(firmwarefile, 'rb')
    firmware_obj.seek(FMR_BOOTHEAD_OFFSET,0)
    firmware_binary = firmware_obj.read()

    
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

    if type == 0:
        makeup_ota(0, firm, sign_name)
        print ("pack %s -> %s Success!!!" %(firm,sign_name))
    elif type == 1:
        makeup_ota(1, firm, sign_name)
        print ("pack %s -> %s Success!!!" %(firm,sign_name))
    else:
        help()

if __name__ == '__main__':
    main(sys.argv)