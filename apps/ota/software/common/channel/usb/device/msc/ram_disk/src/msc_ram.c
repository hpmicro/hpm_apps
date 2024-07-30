/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_msc.h"
#include "msc_ram.h"
#include "hpm_groupsplit.h"
#include "hpm_flashmap.h"

DataProcess data_process = NULL;

#define MSC_IN_EP 0x81
#define MSC_OUT_EP 0x02

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

#define MSC_BLOCK_NUM (FLASH_USER_APP1_SIZE / GROUPSPLIT_BLOCKSIZE)
#define MSC_BLOCK_SIZE (GROUPSPLIT_BLOCKSIZE)

#define DIV_CEIL(_v, _d) (((_v) / (_d)) + ((_v) % (_d) ? 1 : 0))

#define FAT_ENTRY_SIZE (2)
#define FAT_ENTRIES_PER_SECTOR (MSC_BLOCK_SIZE / FAT_ENTRY_SIZE)

#define TOTAL_CLUSTERS_ROUND_UP DIV_CEIL(MSC_BLOCK_NUM, 1)
#define BPB_SECTORS_PER_FAT DIV_CEIL(TOTAL_CLUSTERS_ROUND_UP, FAT_ENTRIES_PER_SECTOR)

typedef struct
{
    uint8_t JumpInstruction[3];
    uint8_t OEMInfo[8];
    uint16_t SectorSize;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t RootDirectoryEntries;
    uint16_t TotalSectors16;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t TotalSectors32;
    uint8_t PhysicalDriveNum;
    uint8_t Reserved;
    uint8_t ExtendedBootSig;
    uint32_t VolumeSerialNumber;
    uint8_t VolumeLabel[11];
    uint8_t FilesystemIdentifier[8];
} __attribute__((packed)) FAT_BootBlock;

static FAT_BootBlock const BootBlock = {
    .JumpInstruction = {0xeb, 0x3c, 0x90},
    .OEMInfo = "HPM-OTA ",
    .SectorSize = MSC_BLOCK_SIZE,
    .SectorsPerCluster = 1,
    .ReservedSectors = 1,
    .FATCopies = 2,
    .RootDirectoryEntries = 64,
    .TotalSectors16 = MSC_BLOCK_NUM,
    .MediaDescriptor = 0xF8,
    .SectorsPerFAT = BPB_SECTORS_PER_FAT,
    .SectorsPerTrack = 1,
    .Heads = 1,
    .TotalSectors32 = 0,
    .PhysicalDriveNum = 0x80, /* to match MediaDescriptor of 0xF8 */
    .ExtendedBootSig = 0x29,
    .VolumeSerialNumber = 0x00420042,
    .VolumeLabel = "HPM-UPD",
    .FilesystemIdentifier = "FAT16   ",
};

const uint8_t msc_ram_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    /*
     * string2 descriptor
     */
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'M', 0x00,                  /* wcChar10 */
    'S', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    /*
     * string3 descriptor
     */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    /*
     * device qualifier descriptor
     */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;

    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    (void)busid;
    (void)lun;
    *block_num = MSC_BLOCK_NUM; /* Pretend having so many buffer,not has actually. */
    *block_size = MSC_BLOCK_SIZE;
    printf("block num:%d, block size:%d\r\n", *block_num, *block_size);
}

int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;
    memset(buffer, 0, length);
    if (sector == 0)
    {
        memcpy(buffer, &BootBlock, sizeof(BootBlock));
        buffer[510] = 0x55;
        buffer[511] = 0xaa;
    }
    return 0;
}

int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;
    (void)sector;
    uint32_t i;
    hpm_groupsplit_pack_t *groupsplit_t = NULL;
    if (length < sizeof(hpm_groupsplit_pack_t) || (length % sizeof(hpm_groupsplit_pack_t)) != 0)
    {
        printf("BAD!, sizeof(hpm_groupsplit_pack_t):%d\r\n", sizeof(hpm_groupsplit_pack_t));
        return -1;
    }
    for (i = 0; i < length / sizeof(hpm_groupsplit_pack_t); i++)
    {
        groupsplit_t = (hpm_groupsplit_pack_t *)buffer + (i * sizeof(hpm_groupsplit_pack_t));
        if (!is_hpm_groupsplit_block(groupsplit_t))
        {
            continue;
        }
        if (data_process)
        {
            data_process((groupsplit_t->index * sizeof(groupsplit_t->data)),  groupsplit_t->data, groupsplit_t->datasize);
        }
    }

    return 0;
}

/* function ------------------------------------------------------------------*/
/**
 * @brief            msc ram init
 * @pre              none
 * @param[in]        none
 * @retval           none
 */
struct usbd_interface intf0;

void msc_ram_init(uint8_t busid, uint32_t reg_base)
{

    if (sizeof(hpm_groupsplit_pack_t) != GROUPSPLIT_BLOCKSIZE)
    {
        printf("group split pack struct error!\r\n");
        while (1)
        {
        }
    }
    usbd_desc_register(busid, msc_ram_descriptor);
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
