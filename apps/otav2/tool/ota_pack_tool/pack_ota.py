import sys, getopt
import os
import struct
import ctypes
import binascii
import hashlib 
import zlib
import calendar
import time
import subprocess
import json
import os

from gmssl import sm3, func

FLASH_BASE_ADDR = 0x80000000
APP_IMG_OFFSET  = 0x45000

HASH_TYPE_CHECKSUM = 0
HASH_TYPE_XOR = 1
HASH_TYPE_CRC32 = 2
HASH_TYPE_SHA1 = 3
HASH_TYPE_SHA256 = 4
HASH_TYPE_SM3 = 5

OTA_TYPE_NORMAL_FIRMWARE = 0

OTA_TYPE_DIFF_FIRMWARE = 1

OTA_TYPE_LZMA_FIRMWARE = 2

# OTA_TYPE_NORMAL_FIRMWARE = 0
# OTA_TYPE_RAM_FIRMWARE = 1
# OTA_TYPE_DIFF_FLASHXIP_FIRMWARE = 2
# OTA_TYPE_DIFF_RAM_FIRMWARE = 3
# OTA_TYPE_PROFILE = 4

HEADER_INIT_VERSION =       (0x35)

HPM_USB_DEVICE_FILE_NAME = "usb_device_update.upd"

GROUPSPLIT_BLOCKSIZE    =       (512)

GROUPSPLIT_MAGIC_0      =  0x48504D2D #/* HPM- */
GROUPSPLIT_MAGIC_1      =  0x9E5D5157 #/* Randomly selected */
GROUPSPLIT_END          =  0x48454E44  #/* HEND */


HPM_APP_FILE_FLAG_MAGIC = 0xbeaf5aa5
HPM_APP_FILE_TOUCH_ID = 0xa5a55a5a
HPM_APP_DIFF_FLAG_MAGIC = 0xbeafa5a5
HPM_APP_LZMA_FLAG_MAGIC = 0xbefaa55a

HPM_APP_HEADER_SIZE = 4096
HPM_APP_GENERAL_SIZE = 92
HPM_APP_DIFF_SIZE = 152
HPM_APP_LZMA_SIZE = 79
HPM_APP_DIFF_DUMMY_SIZE = HPM_APP_HEADER_SIZE - HPM_APP_DIFF_SIZE - HPM_APP_GENERAL_SIZE
HPM_APP_LZMA_DUMMY_SIZE = HPM_APP_HEADER_SIZE - HPM_APP_LZMA_SIZE - HPM_APP_GENERAL_SIZE

# /**
#  * @brief BODY type mask 0~15
#  *        ;内容类型掩码
#  */
OTA_TYPE_BODY_MASK   =     (0x0F)
OTA_TYPE_BODY_SHIFT  =     (0)
def OTA_TYPE_BODY_SET(x):
    return (((x) << OTA_TYPE_BODY_SHIFT) & OTA_TYPE_BODY_MASK)

def OTA_TYPE_BODY_GET(x):
    return (((x) & OTA_TYPE_BODY_MASK) >> OTA_TYPE_BODY_SHIFT)

# /**
#  * @brief type RAM mask, 1 is ram, 0 is xip
#  *        ;类型RAM掩码，1是ram类型，0是flash_xip类型
#  */
OTA_TYPE_RAM_MASK    =      (0x40)
OTA_TYPE_RAM_SHIFT   =     (6)
def OTA_TYPE_RAM_SET(x):
    return (((x) << OTA_TYPE_RAM_SHIFT) & OTA_TYPE_RAM_MASK)

def OTA_TYPE_RAM_GET(x):
    return (((x) & OTA_TYPE_RAM_MASK) >> OTA_TYPE_RAM_SHIFT)

# /**
#  * @brief type exip mask, 1 is exip enable, 0 is no exip enable
#  *        ;类型EXIP 掩码, 1是使能了EXIP启用加密， 0是为启用加密
#  */
OTA_TYPE_EXIP_MASK   =     (0x80)
OTA_TYPE_EXIP_SHIFT  =     (7)
def OTA_TYPE_EXIP_SET(x):
    return (((x) << OTA_TYPE_EXIP_SHIFT) & OTA_TYPE_EXIP_MASK)

def OTA_TYPE_EXIP_GET(x):
    return (((x) & OTA_TYPE_EXIP_MASK) >> OTA_TYPE_EXIP_SHIFT)


# #pragma pack(1)
# typedef union
# {
#     struct {
#         uint32_t magic;
#         uint32_t touchid;
#         uint32_t device;
#         uint32_t len;
#         uint32_t version;
#         uint8_t type;
#         uint8_t hash_enable;
#         uint8_t pwr_hash;
#         uint8_t hash_type;
#         uint32_t start_addr;
#         uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];
#         diff_image_header_t diff_header;
#     };
#     uint8_t buffer[HPM_APP_HEADER_SIZE];
# } hpm_app_header_t;
# #pragma pack()
hpm_app_default_header = {
    'magic': [4, HPM_APP_FILE_FLAG_MAGIC],
    'touchid': [4, HPM_APP_FILE_TOUCH_ID],
    'device': [4, 0x6750],
    'len': [4, 0],
    'version': [4, 0],
    'type': [1, OTA_TYPE_NORMAL_FIRMWARE],
    'hash_enable': [1, 1],
    'pwr_hash': [1, 1],
    'hash_type': [1, 4],
    'start_addr':[4, 0],
    'hash_data':[64, []],
    'diff_header':[152, []],
    'dummy': [HPM_APP_DIFF_DUMMY_SIZE, []],
    'iv': [8, []],
    'key': [16, []]
}


# #pragma pack(1)
# typedef struct
# {
#     uint32_t magic;
#     uint8_t comp_type;
#     uint8_t reserved;
#     uint32_t old_version;
#     uint32_t old_img_len;
#     uint8_t old_hash_type;
#     uint8_t old_hash_data[HPM_APP_HASH_DATA_MAXSIZE];
#     uint32_t new_version;
#     uint32_t new_img_len;
#     uint8_t new_hash_type;
#     uint8_t new_hash_data[HPM_APP_HASH_DATA_MAXSIZE];
# } diff_image_header_t;
# #pragma pack()
hpm_diff_default_header = {
    'magic': [4, HPM_APP_DIFF_FLAG_MAGIC],
    'comp_type': [1, 0],
    'reserved': [1, 0],
    'old_version': [4, HEADER_INIT_VERSION],
    'old_img_len': [4, 0],
    'old_hash_type': [1, 4],
    'old_hash_data': [64, []],
    'new_version': [4, 0],
    'new_img_len': [4, 0],
    'new_hash_type': [1, 4],
    'new_hash_data': [64, []],
}

# typedef struct
# {
#     uint32_t magic;
#     uint8_t comp_type;
#     uint8_t reserved;
#     uint32_t version;
#     uint32_t img_len;
#     uint8_t hash_type;
#     uint8_t hash_data[HPM_APP_HASH_DATA_MAXSIZE];
# } lzma_image_header_t;

hpm_lzma_default_header = {
    'magic': [4, HPM_APP_LZMA_FLAG_MAGIC],
    'comp_type': [1, 0],
    'reserved': [1, 0],
    'version': [4, HEADER_INIT_VERSION],
    'img_len': [4, 0],
    'hash_type': [1, 4],
    'hash_data': [64, []],
}

exipinfo_jsondata = {
    "exip_image_info": {
        "kek":"75b8c7d576b6ad7b1307392ed6df6ec4",
        "mem_base_addr": "0x80000000",
        "regions": [
            [
                "0x80000000",
                "0x400",
                "00000000000000000000000000000000",
                "0000000000000000"
            ]
        ]
    },
    "output_path": "./",
    "user_data": {
        "binary_path": "./",
        "offset": ""
    }
}

EXIP_PADING_LEN_SIZE  = 0x400

DEBUG = False  # 设置为 False 以关闭打印

def debug_print(*args):
    if DEBUG:
        print(*args)

# uint32_t magic0;
# uint32_t magic1;
# uint8_t type;
# uint8_t reserved[3];
# uint32_t file_version;
# uint32_t total_len;
# uint32_t index;
# uint32_t datasize;
# uint8_t data[480];
# uint32_t magicend;

def convert_to_usbupd(type, file_version, file_content):
    global familyid
    reversed = 0
    total_len = len(file_content)
    numblocks = (len(file_content) + 479) // 480
    outp = []
    for blockno in range(numblocks):
        ptr = 480 * blockno
        chunk = file_content[ptr:ptr + 480]
        hd = struct.pack(b"<II",
            GROUPSPLIT_MAGIC_0, GROUPSPLIT_MAGIC_1)
        hd += struct.pack(b"<B", type)
        hd += struct.pack(b"<BBB", reversed, reversed, reversed)
        hd += struct.pack(b"<IIII", file_version, total_len, blockno, len(chunk))

        while len(chunk) < 480:
            chunk += b"\x00"
        block = hd + chunk + struct.pack(b"<I", GROUPSPLIT_END)
        assert len(block) == 512
        outp.append(block)
    return b"".join(outp)

def makeup_hashdata(hashtype, bin, head, offset):
    int_data = 0
    hash_arry = []
    hash_data = []
    if (hashtype == HASH_TYPE_CHECKSUM):
        for val in list(bin):
            if (int_data > 4294967295):
                int_data = 0;
            int_data += val
        debug_print ("int_data:", int_data)
        struct.pack_into('<I', head, offset, int_data)
    elif (hashtype == HASH_TYPE_XOR):
        for val in list(bin):
            int_data ^= val
        debug_print ("int_data:", int_data)
        struct.pack_into('<I', head, offset, int_data)
    elif (hashtype == HASH_TYPE_CRC32):
        int_data = zlib.crc32(bin) & 0xFFFFFFFF
        debug_print ("int_data:", int_data)
        struct.pack_into('<I', head, offset, int_data)
    elif (hashtype == HASH_TYPE_SHA1):
        sha1 = hashlib.sha1()
        sha1.update(bin)
        hash_data = sha1.hexdigest()
        debug_print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(20):
            struct.pack_into('B', head, offset+i, hash_arry[i])
    elif (hashtype == HASH_TYPE_SHA256):
        sha256 = hashlib.sha256()
        sha256.update(bin)
        hash_data = sha256.hexdigest()
        debug_print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(32):
            struct.pack_into('B', head, offset+i, hash_arry[i])
    elif (hashtype == HASH_TYPE_SM3):
        hash_data = sm3.sm3_hash(func.bytes_to_list(bin))
        debug_print ("hash_data:", hash_data, "len:", len(hash_data))
        hash_arry = bytearray.fromhex(hash_data)
        for i in range(32):
            struct.pack_into('B', head, offset+i, hash_arry[i])
    else:
        print ("ERROR: no support the type:", hashtype, "!")
    return int_data, hash_data

def makeup_headerinfo_data(headerinfo, bin):
    fota_head = ctypes.create_string_buffer(HPM_APP_HEADER_SIZE)
    size = 0
    debug_print("size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['magic'][1])
    size +=  headerinfo['magic'][0]
    debug_print("magic size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['touchid'][1])
    size +=  headerinfo['touchid'][0]
    debug_print("touchid size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['device'][1])
    size +=  headerinfo['device'][0]
    debug_print("device size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['len'][1])
    size +=  headerinfo['len'][0]
    debug_print("len size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['version'][1])
    size +=  headerinfo['version'][0]
    print("version:", headerinfo['version'][1])
    debug_print("version size:", size)
    struct.pack_into('<B', fota_head, size, headerinfo['type'][1])
    size +=  headerinfo['type'][0]
    debug_print("type size:", size)
    struct.pack_into('<B', fota_head, size, headerinfo['hash_enable'][1])
    size +=  headerinfo['hash_enable'][0]
    debug_print("hash_enable size:", size)
    struct.pack_into('<B', fota_head, size, headerinfo['pwr_hash'][1])
    size +=  headerinfo['pwr_hash'][0]
    debug_print("pwr_hash size:", size)
    struct.pack_into('<B', fota_head, size, headerinfo['hash_type'][1])
    size +=  headerinfo['hash_type'][0]
    debug_print("hash_type size:", size)
    struct.pack_into('<I', fota_head, size, headerinfo['start_addr'][1])
    size +=  headerinfo['start_addr'][0]
    debug_print("start_addr size:", size)

    int_data, hash_arry = makeup_hashdata(headerinfo['hash_type'][1], bin, fota_head, size)
    print("header hash:", int_data, ",", hash_arry)
    size +=  headerinfo['hash_data'][0]
    debug_print("hash_data size:", size)
    return size, fota_head

def makeup_diffinfo_data(hashtype, head, size, oldbin, newbin):
    diffinfo = hpm_diff_default_header
    diffinfo['old_img_len'][1] = len(oldbin)
    diffinfo['old_hash_type'][1] = hashtype

    diffinfo['new_version'][1] = calendar.timegm(time.gmtime())
    diffinfo['new_img_len'][1] = len(newbin)
    diffinfo['new_hash_type'][1] = hashtype

    print("old img version(no use):", diffinfo['old_version'][1])
    print("new img version:", diffinfo['new_version'][1])

    struct.pack_into('<I', head, size, diffinfo['magic'][1])
    size +=  diffinfo['magic'][0]
    debug_print("diff magic size:", size)
    struct.pack_into('<B', head, size, diffinfo['comp_type'][1])
    size +=  diffinfo['comp_type'][0]
    debug_print("diff comp_type size:", size)
    struct.pack_into('<B', head, size, diffinfo['reserved'][1])
    size +=  diffinfo['reserved'][0]
    debug_print("diff reserved size:", size)
    struct.pack_into('<I', head, size, diffinfo['old_version'][1])
    size +=  diffinfo['old_version'][0]
    debug_print("diff old_version size:", size)
    struct.pack_into('<I', head, size, diffinfo['old_img_len'][1])
    size +=  diffinfo['old_img_len'][0]
    debug_print("diff old_img_len size:", size)
    struct.pack_into('<B', head, size, diffinfo['old_hash_type'][1])
    size +=  diffinfo['old_hash_type'][0]
    debug_print("diff old_hash_type size:", size)
    int_data, oldhash_arr = makeup_hashdata(diffinfo['old_hash_type'][1], oldbin, head, size)
    print("old img header hash:", int_data, ",", oldhash_arr)
    size +=  diffinfo['old_hash_data'][0]
    debug_print("diff old_hash_data size:", size)
    struct.pack_into('<I', head, size, diffinfo['new_version'][1])
    size +=  diffinfo['new_version'][0]
    debug_print("diff new_version size:", size)
    struct.pack_into('<I', head, size, diffinfo['new_img_len'][1])
    size +=  diffinfo['new_img_len'][0]
    debug_print("diff new_img_len size:", size)
    struct.pack_into('<B', head, size, diffinfo['new_hash_type'][1])
    size +=  diffinfo['new_hash_type'][0]
    debug_print("diff new_hash_type size:", size)

    intdata, newhash_arr = makeup_hashdata(diffinfo['new_hash_type'][1], newbin, head, size)
    print("new img header hash:", intdata, ",", newhash_arr)
    size +=  diffinfo['new_hash_data'][0]
    debug_print("diff new_hash_data size:", size)
    if(oldhash_arr == newhash_arr):
        print("BAD !hash same, same firmware, no differential upgrade required")
        raise ValueError("hash same!")

    return size, head

def makeup_lzmainfo_data(hashtype, head, size, srcbin):
    lzmainfo = hpm_lzma_default_header
    lzmainfo['img_len'][1] = len(srcbin)
    lzmainfo['hash_type'][1] = hashtype
    lzmainfo['version'][1] = calendar.timegm(time.gmtime())

    print("lzma version:", lzmainfo['version'][1])

    struct.pack_into('<I', head, size, lzmainfo['magic'][1])
    size +=  lzmainfo['magic'][0]
    debug_print("lzma magic size:", size)
    struct.pack_into('<B', head, size, lzmainfo['comp_type'][1])
    size +=  lzmainfo['comp_type'][0]
    debug_print("lzma comp_type size:", size)
    struct.pack_into('<B', head, size, lzmainfo['reserved'][1])
    size +=  lzmainfo['reserved'][0]
    debug_print("lzma reserved size:", size)
    struct.pack_into('<I', head, size, lzmainfo['version'][1])
    size +=  lzmainfo['version'][0]
    debug_print("lzma version size:", size)
    struct.pack_into('<I', head, size, lzmainfo['img_len'][1])
    size +=  lzmainfo['img_len'][0]
    debug_print("lzma img_len size:", size)
    struct.pack_into('<B', head, size, lzmainfo['hash_type'][1])
    size +=  lzmainfo['hash_type'][0]
    debug_print("lzma hash_type size:", size)
    int_data, hash_arr = makeup_hashdata(lzmainfo['hash_type'][1], srcbin, head, size)
    print("lzma img header hash:", int_data, ",", hash_arr)
    size +=  lzmainfo['hash_data'][0]
    debug_print("lzam hash_data size:", size)

    return size, head

def exip_encode_process(iv, key, baseaddr, offset, srcdatabin):
    data_obj = open("exip_src_data.bin", 'wb+')
    data_obj.seek(0,0)
    data_obj.write(srcdatabin)
    data_obj.close()

    data_base_offset = baseaddr + offset
    current_length = len(srcdatabin)
    padding_len = (EXIP_PADING_LEN_SIZE - (current_length % EXIP_PADING_LEN_SIZE)) % EXIP_PADING_LEN_SIZE
    region_len = current_length + padding_len

    new_region = [
        hex(data_base_offset),
        hex(region_len),
        key.hex(),
        iv.hex()
    ]
    
    exipinfo_jsondata["exip_image_info"]["mem_base_addr"] = hex(baseaddr)
    exipinfo_jsondata["exip_image_info"]["regions"] = [new_region]

    exipinfo_jsondata["output_path"] = "exip_encode_data.bin"
    exipinfo_jsondata["user_data"]["binary_path"] = "exip_src_data.bin"
    exipinfo_jsondata["user_data"]["offset"] = hex(offset)

    debug_print(json.dumps(exipinfo_jsondata, indent=4))

    json_file_path = 'exip_image_info.json'
    with open(json_file_path, 'w') as json_file:
        json.dump(exipinfo_jsondata, json_file, indent=4)

    # ./img_img_util -- create_image xpi0_xip.json
    result = subprocess.run(['./win_tool/hpm_img_util.exe', '--', 'create_image', './exip_image_info.json'], capture_output=True, text=True)
        
    os.remove('exip_src_data.bin')
    os.remove('exip_image_info.json')
    debug_print("result:", result)
    if(result.returncode != 0 or result.stdout.find('success') == -1):
        raise ValueError("build exip image error!")

    exipbin_obj = open("exip_encode_data.bin", 'rb')
    exipbin_obj.seek(0,0)
    exipbin = exipbin_obj.read()
    exipbin_obj.close()
    os.remove('exip_encode_data.bin')
    exiplen = len(exipbin)
    return exiplen,exipbin

def makeup_udp(isusb, headinfo, otafile, new_firmbin, old_firmbin):
    offset = 0
    otafile_obj = open(otafile, 'wb+')
    newfile_obj = open(new_firmbin, 'rb')
    if(OTA_TYPE_RAM_GET(headinfo['type'][1]) == 1):
        offset = 0
    else:
        offset = HPM_APP_HEADER_SIZE

    newfile_obj.seek(offset,0)
    newfilesrc_binary = newfile_obj.read()
    newfile_obj.close()
    newfilesrc_len = len(newfilesrc_binary)

    if DEBUG:
        test_obj = open("src_nohead.bin", 'wb+')
        test_obj.seek(0,0)
        test_obj.write(newfilesrc_binary)
        test_obj.close()
        debug_print("newfile_len1:", newfilesrc_len)

    if(OTA_TYPE_EXIP_GET(headinfo['type'][1]) == 1):
        newfile_len,newfile_binary = exip_encode_process(headinfo['iv'][1], headinfo['key'][1], FLASH_BASE_ADDR, APP_IMG_OFFSET, newfilesrc_binary)
    else:
        newfile_binary = newfilesrc_binary
        newfile_len = newfilesrc_len

    if DEBUG:
        debug_print("newfile_len2:", newfile_len)
        test2_obj = open("exip_code_data.bin", 'wb+')
        test2_obj.seek(0,0)
        test2_obj.write(newfile_binary)
        test2_obj.close()

    if(OTA_TYPE_BODY_GET(headinfo['type'][1]) == OTA_TYPE_NORMAL_FIRMWARE):
        headinfo['len'][1] = newfile_len
        size, headdata = makeup_headerinfo_data(headinfo, newfile_binary)

        for i in range(HPM_APP_HEADER_SIZE - HPM_APP_GENERAL_SIZE):
            struct.pack_into('B', headdata, size + i, 0)
        size += HPM_APP_HEADER_SIZE - HPM_APP_GENERAL_SIZE
        debug_print("dummy size:", size)

        otafile_obj.seek(0, 0)
        otafile_obj.write(headdata)
        otafile_obj.seek(HPM_APP_HEADER_SIZE,0)
        otafile_obj.write(newfile_binary)

    elif(OTA_TYPE_BODY_GET(headinfo['type'][1]) == OTA_TYPE_DIFF_FIRMWARE):
        oldfile_obj = open(old_firmbin, 'rb')
        oldfile_obj.seek(offset, 0)
        oldfilesrc_binary = oldfile_obj.read()
        oldfile_obj.close()
        oldfilesrc_len = len(oldfilesrc_binary)

        if(OTA_TYPE_EXIP_GET(headinfo['type'][1]) == 1):
            oldfile_len, oldfile_binary = exip_encode_process(headinfo['iv'][1], headinfo['key'][1], FLASH_BASE_ADDR, APP_IMG_OFFSET, oldfilesrc_binary)
        else:
            oldfile_binary = oldfilesrc_binary
            oldfile_len = oldfilesrc_len

        newnoheader_obj = open("new_nohead_img.bin", 'wb+')
        newnoheader_obj.seek(0,0)
        newnoheader_obj.write(newfile_binary)
        newnoheader_obj.close()

        oldnoheader_obj = open("old_nohead_img.bin", 'wb+')
        oldnoheader_obj.seek(0,0)
        oldnoheader_obj.write(oldfile_binary)
        oldnoheader_obj.close()

        #run exe build patch
        # /bsdifflzma_createpatch.exe -t 0/1 -o demo.bin -n demo2.bin -p diff_build6.bin
        result = subprocess.run(['./win_tool/bsdifflzma_createpatch.exe', '-t', '0', '-o', 'old_nohead_img.bin', '-n',\
                                 'new_nohead_img.bin', '-p', 'diff_img_build.bin'], capture_output=True, text=True)
        
        os.remove('old_nohead_img.bin')
        os.remove('new_nohead_img.bin')
        if(result.returncode != 0 or result.stdout.find('success') == -1):
            raise ValueError("build difflzma create patch error!")

        difffile_obj = open("diff_img_build.bin", 'rb')
        difffile_obj.seek(0, 0)
        difffile_binary = difffile_obj.read()
        difffile_obj.close()
        os.remove('diff_img_build.bin')

        difffile_len = len(difffile_binary)
        # debug_print("diffimg:", difffile_binary)
        debug_print("difffile_len:", difffile_len)

        headinfo['len'][1] = difffile_len
        size, headdata = makeup_headerinfo_data(headinfo, difffile_binary)
        diffsize, diffheaddata = makeup_diffinfo_data(headinfo['hash_type'][1], headdata, size, oldfile_binary, newfile_binary)

        for i in range(HPM_APP_HEADER_SIZE - HPM_APP_DIFF_SIZE - HPM_APP_GENERAL_SIZE):
            struct.pack_into('B', diffheaddata, diffsize + i, 0)
        diffsize += HPM_APP_HEADER_SIZE - HPM_APP_DIFF_SIZE - HPM_APP_GENERAL_SIZE
        debug_print("diff dummy size:", diffsize)

        otafile_obj.seek(0, 0)
        otafile_obj.write(diffheaddata)
        otafile_obj.seek(HPM_APP_HEADER_SIZE,0)
        otafile_obj.write(difffile_binary)

    elif(OTA_TYPE_BODY_GET(headinfo['type'][1]) == OTA_TYPE_LZMA_FIRMWARE):
        newnoheader_obj = open("new_nohead_img.bin", 'wb+')
        newnoheader_obj.seek(0,0)
        newnoheader_obj.write(newfile_binary)
        newnoheader_obj.close()

        result = subprocess.run(['./win_tool/bsdifflzma_createpatch.exe', '-t', '1', '-o', 'new_nohead_img.bin', '-n',\
                                 'null', '-p', 'lzma_img_build.bin'], capture_output=True, text=True)

        os.remove('new_nohead_img.bin')
        if(result.returncode != 0 or result.stdout.find('success') == -1):
            raise ValueError("build lzma create patch error!")
        
        lzmafile_obj = open("lzma_img_build.bin", 'rb')
        lzmafile_obj.seek(0, 0)
        lzmafile_binary = lzmafile_obj.read()
        lzmafile_obj.close()
        os.remove('lzma_img_build.bin')

        lzmafile_len = len(lzmafile_binary)
        debug_print("lzmafile_len:", lzmafile_len)

        headinfo['len'][1] = lzmafile_len

        size, headdata = makeup_headerinfo_data(headinfo, lzmafile_binary)
        lzmasize, lzmaheaddata = makeup_lzmainfo_data(headinfo['hash_type'][1], headdata, size, newfile_binary)
        
        lzmasize += HPM_APP_HEADER_SIZE - HPM_APP_LZMA_SIZE - HPM_APP_GENERAL_SIZE
        debug_print("lzma dummy size:", lzmasize)

        otafile_obj.seek(0, 0)
        otafile_obj.write(lzmaheaddata)
        otafile_obj.seek(HPM_APP_HEADER_SIZE,0)
        otafile_obj.write(lzmafile_binary)

    else:
        raise ValueError("no support this type!")

    otafile_obj.close()
    fota_len = os.path.getsize(otafile)
    print("build len:", fota_len)

    if(isusb):
        usbdevicefile_obj = open(HPM_USB_DEVICE_FILE_NAME, 'wb+')
        signfile_obj = open(otafile, 'rb')

        signfile_obj.seek(0,0)
        signfile_binary = signfile_obj.read()
        usbdevice_binary = convert_to_usbupd(headinfo['type'][1], headinfo['version'][1], signfile_binary)
        usbdevicefile_obj.seek(0,0)
        usbdevicefile_obj.write(usbdevice_binary)
        signfile_obj.close()
        usbdevicefile_obj.close()

def getexip_keyinfo(exipkeyfile):
    with open(exipkeyfile, 'r') as file:
        data = json.load(file)
    # 提取 AES 密钥和 IV
    key_hex = data['key']
    iv_hex = data['iv']

    # 将十六进制字符串转换为字节
    key = bytes.fromhex(key_hex)
    iv = bytes.fromhex(iv_hex)
    if len(iv) != 8 or len(key) != 16:
        raise ValueError("exie aes iv key error!")
    return iv,key

def help():
    print("---------help------------")
    print("一共5个或6个参数")
    print("第一个参数：此脚本文件 pack_ota.py")
    print("第二个参数：生成固件type类型:0基于FLASH_XIP类型的通用固件, 1基于RAM类型的通用固件, 2基于FLASH_XIP类型的差分固件, 3基于RAM类型的差分固件")
    print("第三个参数：签名type类型")
    print("           0-checksum 校验和校验")
    print("           1-xor 异或校验")
    print("           2-CRC32 校验")
    print("           3-SHA1 校验")
    print("           4-SHA256 校验")
    print("           5-SM3 校验")
    print("第四个参数：目标OTA路径")
    print("第五个参数：原始固件路径")
    print("第六个参数: 差分旧固件路径")
    
    print("")
    print("举例：")
    print("     python pack_ota.py 0 0 update_sign.bin demo.bin")
    print("     python pack_ota.py 0 1 update_sign.bin demo.bin")
    print("     python pack_ota.py 0 2 update_sign.bin demo.bin")
    print("     python pack_ota.py 0 3 update_sign.bin demo.bin")
    print("     python pack_ota.py 0 4 update_sign.bin demo.bin")
    print("     python pack_ota.py 0 5 update_sign.bin demo.bin")
    print("     python pack_ota.py 2 3 update_sign.bin demo.bin old_demo.bin")
    print("     python pack_ota.py 2 4 update_sign.bin demo.bin old_demo.bin")
    print("     python pack_ota.py 2 5 update_sign.bin demo.bin old_demo.bin")
    print(" 同时会生成基于USB DEVICE的OTA包(usb_device_update.upd)")
    print(" 注意:为确保差分固件签名的消息摘要唯一性，差分固件的签名必须使用3/4/5")
    print("-------------------------")

def main(argv):
    # 根据提示输入参数
    while True:
        print("当前是先楫半导体生成升级签名固件运行脚本！")
        print("请根据提示输入，按回车结束输入！")

        while True:
            ramtype = input("选择固件运行区域类型: \
                            \n\t0:基于FLASH_XIP类型 \
                            \n\t1:基于RAM类型 \
                            \n请输入: ")
            if(ramtype.strip() == ""):
                print("输入错误，请重新输入")
            elif(int(ramtype) < 0 or int(ramtype) > 1):
                print("输入类型不支持，请重新输入")
            else:
                break

        while True:
            exipenable = input("选择固件是否启用EXIP加密: \
                            \n\t0:不启用 \
                            \n\t1:启用 \
                            \n请输入: ")
            if(exipenable.strip() == ""):
                print("输入错误，请重新输入")
            elif(int(exipenable) < 0 or int(exipenable) > 1):
                print("输入错误，请重新输入")
            else:
                break

        if int(exipenable) == 1 :
            exipkeyfile = input("请输入密匙文件名称(不输入回车默认:exip_key.json):")
            if(exipkeyfile.strip() == ""):
                exipkeyfile = "exip_key.json"
            exipiv,exipkey = getexip_keyinfo(exipkeyfile)

        while True:
            firmtype = input("选择要生成的固件类型: \
                            \n\t0:正常固件 \
                            \n\t1:差分固件 \
                            \n\t2:压缩固件 \
                            \n请输入: ")
            if(firmtype.strip() == ""):
                print("输入错误，请重新输入")
            elif(int(firmtype) < 0 or int(firmtype) > 2):
                print("输入类型不支持，请重新输入")
            else:
                break
        while True:
            hashtype = input("选择固件签名类型: \
                            \n\t0-checksum 校验和校验 \
                            \n\t1-xor 异或校验 \
                            \n\t2-CRC32 校验 \
                            \n\t3-SHA1 校验 \
                            \n\t4-SHA256 校验 \
                            \n\t5-SM3 校验 \
                            \n请输入: ")
            if(hashtype.strip() == ""):
                print("输入错误，请重新输入")
            elif(int(hashtype) < 0 or int(hashtype) > 5):
                print("输入类型不支持，请重新输入")
            elif(int(firmtype) == 1  and int(hashtype) < 3):
                print("为确保差分固件签名消息摘要唯一性，固件签名类型必须使用：3-SHA1 或 4-SHA256 或 5-SM3, 当前签名类型不可用，请重新输入")
            else:
                break

        ram_start_addr = "0"
        if(int(ramtype) == 1):
            ram_start_addr = input("请输入RAM类型运行的起始地址:(不输入回车默认:0)")
            if(ram_start_addr.strip() == ""):
                ram_start_addr = "0"

        signfile_name = input("请输入要生成的签名固件名称(不输入回车默认:update_sign.bin):")
        if(signfile_name.strip() == ""):
            signfile_name = "update_sign.bin"

        while True:
            newfile_firm = input("请输入新固件的路径名称:")
            if(newfile_firm.strip() == ""):
                print("新固件路径为空，请重新输入:")
            else:
                break

        oldfile_firm = ""
        if(int(firmtype) == 1):
            while True:
                oldfile_firm = input("请输入旧固件的路径名称:")
                if(oldfile_firm.strip() == ""):
                    print("就固件路径为空，请重新输入:")
                else:
                    break
        
        print("选择->固件运行区域类型:", ramtype, ",是否启用EXIP加密:", exipenable, 
            ",固件类型:", firmtype, ",签名类型:", hashtype, ",RAM起始地址:", ram_start_addr, \
            ",生成签名固件名称:", signfile_name, ",新原始固件路径:", newfile_firm, ",旧原始固件路径:", oldfile_firm)
        
        app_header = hpm_app_default_header
        app_header['version'][1] = calendar.timegm(time.gmtime())
        app_header['type'][1] = OTA_TYPE_BODY_SET(int(firmtype)) | OTA_TYPE_EXIP_SET((int(exipenable))) | OTA_TYPE_RAM_SET(int(ramtype))
        app_header['hash_type'][1] = int(hashtype)
        app_header['start_addr'][1] = int(ram_start_addr)
        if int(exipenable) == 1 :
            app_header['iv'][1] = exipiv
            app_header['key'][1] = exipkey

        print("type:", app_header['type'][1])

        try:
            makeup_udp(1, app_header, signfile_name, newfile_firm, oldfile_firm)
            print ("SUCCESS! firm_type:%d, hash_type:%d, packet -> %s Success!!!" %(app_header['type'][1], int(hashtype), signfile_name))
            print ("USB device: %s make Success!!!" %(HPM_USB_DEVICE_FILE_NAME))
        except (ValueError, ZeroDivisionError) as e:
            print(f"错误：{e}")
    

if __name__ == '__main__':
    main(sys.argv)