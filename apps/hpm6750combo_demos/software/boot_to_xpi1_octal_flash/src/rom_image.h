/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * @file rom_image.h
 *
 */
#ifndef ROM_IMAGE_H
#define ROM_IMAGE_H

#include <stdint.h>

/* @brief Image Header definitions */
typedef struct {
    uint8_t tag;                            /*!< Image Header tag: must be 0xBF, offset: 0x00 */
    uint8_t version;                        /*!< Image Header version, offset: 0x01 */
    uint16_t length;                        /*!< Image Header Length:, offset: 0x02 */
    union {
        struct {
            uint32_t srk_set: 4;            /*!< SRK set */
            uint32_t srk_idx: 4;            /*!< SRK index */
            uint32_t srk_revoke_mask: 4;    /*!< SRK REVOKE Mask */
            uint32_t reserved0: 4;
            uint32_t signature_type: 4;     /*!< Signature type, 1 - ECDSA-P256, 4 - SM2 */
            uint32_t blob_location: 4;      /*!< Blob location */
            uint32_t lifecycle: 4;          /*!< Life cycle */
            uint32_t reserved: 4;           /*!< Reserved for future use */
        };
        uint32_t U;
    } flags;                                /*!< Image Flags, offset: 0x04 */
    uint16_t sw_version;                    /*!< Software version, offset: 0x08 */
    uint8_t fuse_version;                   /*!< FUSE version, offset: 0x0A */
    uint8_t number_of_fw;                   /*!< Number of Firmware in this image, offset: 0x0B */
    uint16_t device_config_block_offset;    /*!< Offset of the device configuration block, offset: 0x0c */
    uint16_t signature_block_offset;        /*!< Offset of the signature configuration block, offset: 0x0e */
} boot_image_hdr_t;

#define IMAGE_HEADER_SIZE_MAX               (4096U)
#define IMAGE_HEADER_SIZE_MIN               (sizeof(boot_image_hdr_t) + sizeof(firmware_info_t))
#define SW_VERSION_INVALID                  (0xFFFFU)

#define DEVICE_CONFIG_BLOCK_OFFSET_MIN      IMAGE_HEADER_SIZE_MIN
#define SIGNATURE_CONFIG_BLOCK_OFFSET_MIN   IMAGE_HEADER_SIZE_MIN
#define IMAGE_HEADER_VERSION                (0x10U)
#define IMAGE_HEADER_TAG                    (0xBFU)
#define SIGNATURE_TYPE_NONE                 (0U)
#define SIGNATURE_TYPE_ECDSA                (1U)
#define SIGNATURE_TYPE_SM2                  (4U)
#define SIGNATURE_TYPE_MAX                  (SIGNATURE_TYPE_SM2)

#define IMAGE_SRK_SET_NONE                  (0U)
#define IMAGE_SRK_SET_HPM                   (1U)
#define IMAGE_SRK_SET_OEM                   (2U)

#define FW_BLOB_IN_SIGNATURE_BLOCK          (0U)
#define FW_BLOB_AT_FIXED_DEV_LOCATION       (1U)

/**
 * @brief firmware information structure
 */
typedef struct {
    uint32_t offset;                        /*!< Firmware Offset (relative to the Image header), offset: 0x00 */
    uint32_t size;                          /*!< Firmware size, offset : 0x04 */
    union {
        struct {
            uint32_t fw_type: 4;            /*!< Firmware type, 0 - Executable, 1 - Command Container */
            uint32_t core_id: 4;
            uint32_t hash_type: 4;          /*!< HASH type, 0 - none, 1 - SHA256, 4 - SM3 */
            uint32_t is_encrypted: 4;       /*!< Image encryption flag, 0 - Plaintext, 1 - Encrypted */
            uint32_t reserved1: 16;
        };
        uint32_t U;
    } flags;                                /*!< Firmware flags, offset: 0x08 */
    uint32_t reserved0;                     /*!< Reserved, padding word, offset: 0x0C */
    uint32_t load_addr;                     /*!< Load address, firmware destination address, offset: 0x10 */
    uint32_t reserved1;                     /*!< Reserved for future use, offset: 0x14 */
    uint32_t entry_point;                   /*!< Image entry point, offset: 0x18 */
    uint32_t reserved;                      /*!< Reserved for future use, offset: 0x1C */
    /*!< HASH digest, only 32 bytes are used, the other 32 bytes are reserved, offset: 0x20 */
    uint32_t hash[16];
    uint32_t iv[8];                         /*!< IV for encrypt image, offset : 0x60 */
} firmware_info_t;

#define FW_INFO_CNT_MAX                     (4U)
#define FW_INFO_CNT_MIN                     (1U)
/* FW type definitions */
#define FW_TYPE_EXECUTABLE                  (0U)
#define FW_TYPE_CMD_CONTAINER               (1U)

/* FW HASH type */
#define FW_HASH_TYPE_NONE                   (0U)
#define FW_HASH_TYPE_SHA256                 (1U)
#define FW_HASH_TYPE_SM3                    (4U)

/**
 * @brief Device configuration block header structure
 */
typedef struct {
    union {
        struct {
            uint8_t tag;
            uint8_t version;
            uint16_t length;
        };
        uint32_t hdr;
    };
    uint8_t content[];
} device_config_block_hdr_t;

/**
 * @brief Device configuration information header structure
 */
typedef struct {
    union {
        struct {
            uint8_t tag;
            uint8_t param;
            uint16_t length;
        };
        uint32_t hdr;
    };
    uint8_t content[];
} device_cfg_info_hdr_t;

/**
 * @brief wakeup entry information structure
 */
typedef struct {
    uint32_t start;
    uint32_t wakeup_entry;
    uint32_t length;
    uint32_t hash_type;
    uint32_t hash[8];
} wakeup_entry_info_t;

#define DEVICE_CFG_BLOCK_HDR_TAG                (0xC0U)
#define DEVICE_CFG_BLOCK_HDR_VER                (0x10U)
#define DEVICE_CFG_INFO_XPI_NOR_TAG             (0xC1U)
#define DEVICE_CFG_INFO_XPI_RAM_TAG             (0xC2U)
#define DEVICE_CFG_INFO_SDRAM_TAG               (0xC3U)
#define DEVICE_CFG_INFO_WAKEUP_ENTRY_CHK_TAG    (0xC4U)
#define DEVICE_CFG_INFO_GP_TAG                  (0xC5U)
#define DEVICE_CFG_INFO_FUSE_UNLOCK_TAG         (0xC6U)
#define DEVICE_CFG_BLOCK_SIZE_MIN               (sizeof(device_config_block_hdr_t))
#define DEVICE_CFG_BLOCK_SIZE_MAX               (IMAGE_HEADER_SIZE_MAX - IMAGE_HEADER_SIZE_MIN)

/**
 * @brief SRK table header structure
 */
typedef struct {
    uint8_t tag;                                /*!< SRK Table Header tag, offset: 0x00 */
    uint8_t version;                            /*!< SRK Table Header version, offset: 0x01 */
    uint16_t length;                            /*!< SRK Table length, offset: 0x02 */
    uint8_t content[];
} srk_table_hdr_t;

/**
 * @brief SRK item structure
 */
typedef struct {
    uint8_t tag;                                /*!< SRK Item Tag, offset: 0x00 */
    uint8_t sig_type;
    uint16_t length;                            /*!< SRK Item length, offset: 0x02 */
    uint8_t hash_type;                          /*!< SRK HASH type, offset: 0x04 */
    uint8_t curve;                              /*!< Curve, offset: 0x05 */
    uint8_t reserved1;
    uint8_t flags;                              /*!< Flags, offset: 0x07 */
    uint16_t x_len;                             /*!< length of X */
    uint16_t y_len;                             /*!< length of Y */
    uint8_t content[];
    /* x content <x_len> bytes */
    /* y content <y_len> bytes */
} srk_item_t;

#define SRK_HASH_ALG_SHA256         (1U)
#define SRK_HASH_ALG_SHA384         (2U)
#define SRK_HASH_ALG_SHA512         (3U)
#define SRK_HASH_ALG_SM3            (4U)
#define SRK_CURVE_PRIME256V1        (1U)
#define SRK_CURVE_SEC384R1          (2U)
#define SRK_CURVE_SEC521R1          (3U)
#define SRK_CURVE_SM2               (4U)
#define SRK_X_LEN_P256              (32U) /* SM2, PRIME256V1 */
#define SRK_X_LEN_P384              (48U)
#define SRK_X_LEN_P521              (66U)
#define SRK_Y_LEN_P256              (SRK_X_LEN_P256)
#define SRK_Y_LEN_P384              (SRK_X_LEN_P384)
#define SRK_Y_LEN_P521              (SRK_X_LEN_P521)
#define SRK_X_LEN_SM2               (32U)
#define SRK_Y_LEN_SM2               (SRK_X_LEN_SM2)

#define SRK_ITEM_TAG (0xE1U)
#define SRK_ITEM_SIG_TYPE_ECDSA     (0x21U)
#define SRK_ITEM_SIG_TYPE_SM2       (0x27U)
#define SRK_ITEM_HDR_SIZE           (sizeof(srk_item_t))
#define SRK_ITEM_SIZE_SM2           (SRK_ITEM_HDR_SIZE + SRK_X_LEN_SM2 + SRK_Y_LEN_SM2) /* Item Header + X + Y */
#define SRK_ITEM_SIZE_ECDSA_P256    (SRK_ITEM_HDR_SIZE + SRK_X_LEN_P256 + SRK_Y_LEN_P256) /* Item Header + X + Y */
#define SRK_ITEM_SIZE_ECDSA_P384    (SRK_ITEM_HDR_SIZE + SRK_X_LEN_P384 + SRK_Y_LEN_P384)
#define SRK_ITEM_SIZE_ECDSA_P521    (SRK_ITEM_HDR_SIZE + SRK_X_LEN_P521 + SRK_Y_LEN_P521)
#define SRK_ITEM_SIZE_MIN           MIN(SRK_ITEM_SIZE_ECDSA_P256, SRK_ITEM_SIZE_SM2)
#define SRK_ITEM_SIZE_MAX           MAX(SRK_ITEM_SIZE_ECDSA_P521, SRK_ITEM_SIZE_SM2)

#define SRK_TABLE_SIZE_MIN          (sizeof(srk_table_hdr_t) + 4U * SRK_ITEM_SIZE_MIN)
#define SRK_TABLE_SIZE_MAX          (sizeof(srk_table_hdr_t) + 4U * SRK_ITEM_SIZE_MAX)

/**
 * @brief Signature Header structure
 */
typedef struct {
    uint8_t tag;
    uint8_t version;
    uint16_t length;
    uint32_t reserved;
    /* Signature data */
    uint8_t content[];
} signature_hdr_t;

#define SIGNATURE_TAG (0xD2U)
#define SIGNATURE_ECDSA_P256_SIZE   (8U + 32U + 32U) /* Header size + r + s */
#define SIGNATURE_SM2_SIZE          (8U  + 32U  + 32U)
#define SIGNATURE_SIZE_MIN          MIN(SIGNATURE_ECDSA_P256_SIZE, SIGNATURE_SM2_SIZE)
#define SIGNATURE_SIZE_MAX          MAX(SIGNATURE_ECDSA_P256_SIZE, SIGNATURE_SM2_SIZE)

/**
 * @brief Certificate Block header structure
 */
typedef struct {
    uint8_t tag;
    uint8_t version;
    uint16_t length;
    uint16_t signature_offset;
    uint16_t reserved;
    uint8_t content[];
    /* SRK block */
    /* Signature info */
} certificate_block_hdr_t;

#define CERTIFICATE_BLOCK_TAG   (0xD3U)
#define CERTIFICATE_HDR_VER     (0x10U)
#define CERTIFICATE_SIZE_MIN    (sizeof(certificate_block_hdr_t) + SRK_ITEM_SIZE_MIN + SIGNATURE_SIZE_MIN)
#define CERTIFICATE_SIZE_MAX    (sizeof(certificate_block_hdr_t) + SRK_ITEM_SIZE_MAX + SIGNATURE_SIZE_MAX)

/**
 * @brief Firmware Blob Header structure
 */
typedef struct {
    uint8_t tag;
    uint8_t version;
    uint16_t length;
    uint8_t flags;
    uint8_t key_size;
    uint8_t algorithm;
    uint8_t mode;
    uint8_t iv[16];
    uint8_t content[];
    /*Wrapped key*/
} fw_blob_hdr_t;

#define BLOB_BLOCK_TAG              (0xD4U)
#define BLOB_HDR_VERSION            (0x10U)
#define BLOB_WRAP_KEY_SIZE_MIN      (64U) /* 128-bit IV + 128bit key 16 + 16 + 16*/
#define BLOB_WRAP_KEY_SIZE_MAX      (96U) /* 128-bit IV 256bit key, 32 + 32 + 16 */
#define BLOB_SIZE_MIN               (8U + BLOB_WRAP_KEY_SIZE_MIN)
#define BLOB_SIZE_MAX               (8U + BLOB_WRAP_KEY_SIZE_MAX)
#define BLOB_FLAG_KEY_FMK           (0x1U)
#define BLOB_FLAG_KEY_ZMK           (0x2U)
#define BLOB_FLAG_KEY_EXIP0_KEYH    (0x03U)
#define BLOB_FLAG_KEY_EXIP1_KEYH    (0x04U)
#define BLOB_FLAG_KEY_USER_KEYL     (0x03U) /* NOTE: defined since HPM600 */
#define BLOB_FLAG_KEY_USER_KEYH     (0x04U) /* NOTE: defined since HPM600 */
#define BLOB_KEY_SIZE_128BIT        (0x10U)
#define BLOB_KEY_SIZE_256BIT        (0x20U)
#define BLOB_ENC_ALG_AES            (0x33U)
#define BLOB_ENC_ALG_SM4            (0x55U)
#define BLOB_MODE_CBC               (0x11U)

/**
 * @brief Signature Block Header structure
 */
typedef struct {
    uint8_t tag;                    /*!< Signature Block Tag, offset: 0x00 */
    uint8_t version;                /*!< Signature block version, offset: 0x01 */
    uint16_t length;                /*!< Signature block size, offset: 0x02 */
    uint16_t key_id;                /*!< Key Identifier, 0 - HPM_Key, 1 - Customer Key, offset 0x04 */
    uint16_t reserved0;             /*!< Reserved for future use, offset: 0x06 */
    uint16_t srk_table_offset;      /*!< SRK Table offset, offset: 0x08 */
    uint16_t signature_offset;      /*!< Signature offset, offset: 0x0A */
    uint16_t certificate_offset;    /*!< Reserved for future use, offset: 0x0C */
    uint16_t blob_offset;           /*!< Blob offset, offset: 0x0E */
} signature_block_hdr_t;

#define SIGNATURE_BLOCK_TAG         (0xD0U)
#define SIGNATURE_BLOCK_VER         (0x10U)
#define SIGNATURE_BLOCK_HDR_SIZE    (sizeof(signature_block_hdr_t))
/* Signature block contains, Signature block header, SRK table and signature */
#define SIGNATURE_BLOCK_SIZE_MIN    (SIGNATURE_BLOCK_HDR_SIZE + ALIGN_UP(SRK_TABLE_SIZE_MIN,8) + ALIGN_UP(SIGNATURE_SIZE_MIN,8))
/* Signature block contains, Signature block header, SRK table, signature, certificate, certificate signature and blob */
#define SIGNATURE_BLOCK_SIZE_MAX    (SIGNATURE_BLOCK_SIZE_MIN + ALIGN_UP(CERTIFICATE_SIZE_MAX, 8) + ALIGN_UP(SIGNATURE_SIZE_MAX, 8) + BLOB_SIZE_MAX)

#define SRK_TABLE_HDR_TAG (0xD1U)
#define SRK_TABLE_HDR_VER (0x10)
#define SRK_ITEM_HDR_TAG  (0xE1U)
#define SIGNATURE_HDR_TAG (0xD2U)
#define SIGNATURE_HDR_VER (0x10)


#define IMAGE_SRK_SET_MAX           (IMAGE_SRK_SET_OEM)

#define CORE0_BOOT_ENTRY_FLAG       (0xc0bef1a9UL)
#define CORE1_BOOT_ENTRY_FLAG       (0xc1bef1a9UL)
#define CORE0_BOOT_ENTRY_BAD_FLG    (0xc0bebadfUL)
#define CORE1_BOOT_ENTRY_BAD_FLG    (0xc1bebadfUL)

#define SHA256_DIGEST_SIZE_IN_BYTES (32U)
#define SM3_DIGEST_SIZE_IN_BYTES    (32U)
#define HASH_DIGEST_SIZE_MAX        (64U)

/**
 * @brief Next Block Information structure
 */
typedef struct next_block_info_struct {
    uint16_t size_in_bytes;
    uint16_t is_last_blk;
    uint32_t padding[3];
    uint8_t hash[];
} next_blk_info_t;

/**
 * @brief The Command Container Header
 *        This header contains the basic information of the command container, including:
 *          - tag
 *          - version
 *          - flags
 *          - total block count
 *          - block size (exclude the header itself and the last one)
 *          - total_size of the image
 *          - descriptor
 *          - next block hash
 *        It also contains the hash of the next block
 */
typedef struct enc_cmd_container_header_struct {
    uint32_t tag;
    uint32_t version;
    uint32_t flags;
    uint8_t reserved[68];
    uint8_t descriptor[48];
} cmd_container_hdr_t;

#define CMD_CONTAINER_HDR_TAG (0x31764343) /* ASCII:CCv1 - Command Container V1 */
#define CMD_CONTAINER_VERSION (0x56010000) /* V1.0.0 */
#define CMD_CONTAINER_BLOCK_SIZE (512U)

/**
 * @brief Command Container structure for command entry
 */
typedef struct cmd_container_cmd_struct {
    uint8_t cmd_tag;
    uint8_t cmd_params;
    uint8_t cmd_misc;
    uint8_t padding;
    union {
        struct {
            uint32_t mem_id;
            uint32_t start;
            uint32_t data_bytes;
        };
        struct {
            uint32_t extra_option;
            uint32_t reserved[2];
        };
    };
    uint8_t data[];
} cmd_container_cmd_t;

#define CMD_MISC_FINAL (1U)


#endif /* ROM_IMAGE_H */
