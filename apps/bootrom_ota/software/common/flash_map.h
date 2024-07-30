#ifndef __FLASH_MAP_H
#define __FLASH_MAP_H

#define FW_OTP_SEC_IMG2_OFFSET                (2) // 2X256K = 512K
#define FW_OTA_SEC_MAX_IMG_LEN                (2) // 2X256K = 512K

#define FW_HEADER_SW_VERSION_OFFSET           (0x08)

#define FLASH_ADDR_BASE                       (0x80000000)

#define FLASH_START_ADDR                      (0 + FLASH_ADDR_BASE)                              // 0x80000000
#define FLASH_MAX_SIZE                        (0x200000)                                         // 2M

//APP1 -------------
//APP1 ALL IMG
#define FLASH_APP1_ALL_IMG_ADDR                (FLASH_START_ADDR)
#define FLASH_APP1_ALL_IMG_SIZE                ((FW_OTP_SEC_IMG2_OFFSET*256*1024))

#define FLASH_APP1_EXIP_INFO_ADDR              (FLASH_APP1_ALL_IMG_ADDR)
#define FLASH_APP1_EXIP_INFO_SIZE              (0x400)

#define FLASH_APP1_NOR_CFG_ADDR                (FLASH_APP1_EXIP_INFO_ADDR + FLASH_APP1_EXIP_INFO_SIZE)
#define FLASH_APP1_NOR_CFG_SIZE                (0xC00)

#define FLASH_APP1_BOOT_HEAD_ADDR              (FLASH_APP1_NOR_CFG_ADDR + FLASH_APP1_NOR_CFG_SIZE)
#define FLASH_APP1_BOOT_HEAD_SIZE              (0x2000)

#define FLASH_APP1_USER_IMG_ADDR               (FLASH_APP1_BOOT_HEAD_ADDR + FLASH_APP1_BOOT_HEAD_SIZE)
#define FLASH_APP1_USER_IMG_SIZE               (FLASH_APP1_ALL_IMG_SIZE-(FLASH_APP1_EXIP_INFO_SIZE+FLASH_APP1_NOR_CFG_SIZE+FLASH_APP1_BOOT_HEAD_SIZE))
//-----------------------------

//APP2 --------------------
//APP1 ALL IMG
#define FLASH_APP2_ALL_IMG_ADDR                (FLASH_START_ADDR + FLASH_APP1_ALL_IMG_SIZE)
#define FLASH_APP2_ALL_IMG_SIZE                ((FW_OTP_SEC_IMG2_OFFSET*256*1024))

#define FLASH_APP2_EXIP_INFO_ADDR              (FLASH_APP2_ALL_IMG_ADDR)
#define FLASH_APP2_EXIP_INFO_SIZE              (0x400)

#define FLASH_APP2_NOR_CFG_ADDR                (FLASH_APP2_EXIP_INFO_ADDR + FLASH_APP2_EXIP_INFO_SIZE)
#define FLASH_APP2_NOR_CFG_SIZE                (0xC00)

#define FLASH_APP2_BOOT_HEAD_ADDR              (FLASH_APP2_NOR_CFG_ADDR + FLASH_APP2_NOR_CFG_SIZE)
#define FLASH_APP2_BOOT_HEAD_SIZE              (0x2000)

#define FLASH_APP2_USER_IMG_ADDR               (FLASH_APP2_BOOT_HEAD_ADDR + FLASH_APP2_BOOT_HEAD_SIZE)
#define FLASH_APP2_USER_IMG_SIZE               (FLASH_APP2_ALL_IMG_SIZE-(FLASH_APP2_EXIP_INFO_SIZE+FLASH_APP2_NOR_CFG_SIZE+FLASH_APP2_BOOT_HEAD_SIZE))
//------------------------

//APP1 IMGINFO,checksum todo
#define FLASH_APP1_IMGINFO_ADDR                (FLASH_APP2_USER_IMG_ADDR + FLASH_APP2_USER_IMG_SIZE)
#define FLASH_APP1_IMGINFO_SIZE                (0x1000)

//APP2 IMGINFO, checksum todo
#define FLASH_APP2_IMGINFO_ADDR                (FLASH_APP1_IMGINFO_ADDR + FLASH_APP1_IMGINFO_SIZE)
#define FLASH_APP2_IMGINFO_SIZE                (0x1000)

#define FLASH_USER_COMMON_ADDR                 (FLASH_APP2_IMGINFO_ADDR + FLASH_APP2_IMGINFO_SIZE)
#define FLASH_USER_COMMON_SIZE                 (0x80000)  

// #define FLASH_NOR_CFG_ADDR       (FLASH_START_ADDR + 0x400)                          // 0x80000400
// #define FLASH_NOR_CFG_SIZE       (0xC00)

// #define FLASH_BOOT_HEAD_ADDR     (FLASH_NOR_CFG_ADDR + FLASH_NOR_CFG_SIZE)

// #define FLASH_USER_APP1_ADDR     (FLASH_BOOT_HEAD_ADDR)                              // 0x80001000
// #define FLASH_USER_APP1_SIZE     (FW_OTP_SEC_IMG2_OFFSET * 256 * 1024)               //512K

// #define FLASH_USER_APP2_ADDR     (FLASH_USER_APP1_ADDR + FLASH_USER_APP1_SIZE)       // 0x80081000
// #define FLASH_USER_APP2_SIZE     (FW_OTA_SEC_MAX_IMG_LEN * 256 * 1024)               //512K

// #define FLASH_APP1_HEADER_ADDR   (FLASH_USER_APP2_ADDR + FLASH_USER_APP2_SIZE)       // 0x80100000
// #define FLASH_APP1_HEADER_SIZE   (0x1000)                                            // 4K

// #define FLASH_APP2_HEADER_ADDR   (FLASH_APP1_HEADER_ADDR + FLASH_APP1_HEADER_SIZE)   // 0x80101000
// #define FLASH_APP2_HEADER_SIZE   (0x1000)                                            // 4K

// #define FLASH_USER_COMMON_ADDR   (FLASH_APP2_HEADER_ADDR + FLASH_APP2_HEADER_SIZE)   // 0x80102000
// #define FLASH_USER_COMMON_SIZE   (0x80000)                                           //512K

#endif //__FLASH_MAP_H