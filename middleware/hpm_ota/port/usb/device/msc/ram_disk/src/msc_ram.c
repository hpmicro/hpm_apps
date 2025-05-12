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
#include "ota_kconfig.h"

DataProcess data_process = NULL;

#define MSC_IN_EP 0x81
#define MSC_OUT_EP 0x02

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

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

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, USB_BULK_EP_MPS_FS, 0x02),
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, USB_BULK_EP_MPS_HS, 0x02),
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro MSC DEMO",           /* Product */
    "2024051701",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor msc_ram_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
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

    if (data_process)
    {
        data_process(buffer, length);
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
    usbd_desc_register(busid, &msc_ram_descriptor);
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
