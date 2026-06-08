/*
 * Copyright (c) 2024 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_YT8531C_REGS_H
#define HPM_YT8531C_REGS_H

typedef enum {
    YT8531C_BMCR                         = 0,   /* 0x0: Basic Mode Control Register */
    YT8531C_BMSR                         = 1,   /* 0x1: Basic Mode Status Register */
    YT8531C_PHYID1                       = 2,   /* 0x2: PHY Identifier Register 1 */
    YT8531C_PHYID2                       = 3,   /* 0x3: PHY Identifier Register 2 */
    YT8531C_CR                           = 0x10,
    YT8531C_SR                           = 0x11,
    YT8531C_EXAR                         = 0x1E, 
    YT8531C_EXDR                         = 0x1F, 
} YT8531C_REG_Type;

/* Bitfield definition for register: BMCR */
/*
 * RESET (RW/SC)
 *
 * 1: PHY reset
 * 0: normal operation
 * After software reset, need to delay 10ms de-assert time for chip steady.
 */
#define YT8531C_BMCR_RESET_MASK (0x8000U)
#define YT8531C_BMCR_RESET_SHIFT (15U)
#define YT8531C_BMCR_RESET_SET(x) (((uint16_t)(x) << YT8531C_BMCR_RESET_SHIFT) & YT8531C_BMCR_RESET_MASK)
#define YT8531C_BMCR_RESET_GET(x) (((uint16_t)(x) & YT8531C_BMCR_RESET_MASK) >> YT8531C_BMCR_RESET_SHIFT)

/*
 * LOOPBACK (RW)
 *
 * This bit enables loopback of transmit data nibbles TXD3:0 to the
 * receive data path.
 * 1: Enable loopback  0: Normal operation
 */
#define YT8531C_BMCR_LOOPBACK_MASK (0x4000U)
#define YT8531C_BMCR_LOOPBACK_SHIFT (14U)
#define YT8531C_BMCR_LOOPBACK_SET(x) (((uint16_t)(x) << YT8531C_BMCR_LOOPBACK_SHIFT) & YT8531C_BMCR_LOOPBACK_MASK)
#define YT8531C_BMCR_LOOPBACK_GET(x) (((uint16_t)(x) & YT8531C_BMCR_LOOPBACK_MASK) >> YT8531C_BMCR_LOOPBACK_SHIFT)

/*
 * SPEED0 (RW)
 *
 * This bit sets the network speed.
 * 1: 100Mbps  0: 10Mbps
 */
#define YT8531C_BMCR_SPEED0_MASK (0x2000U)
#define YT8531C_BMCR_SPEED0_SHIFT (13U)
#define YT8531C_BMCR_SPEED0_SET(x) (((uint16_t)(x) << YT8531C_BMCR_SPEED0_SHIFT) & YT8531C_BMCR_SPEED0_MASK)
#define YT8531C_BMCR_SPEED0_GET(x) (((uint16_t)(x) & YT8531C_BMCR_SPEED0_MASK) >> YT8531C_BMCR_SPEED0_SHIFT)

/*
 * ANE (RW)
 *
 * This bit enables/disables the NWay auto-negotiation function.
 * 1: Enable auto-negotiation; bits 0:13 and 0:8 will be ignored
 * 0: Disable auto-negotiation; bits 0:13 and 0:8 will determine the
 * link speed and the data transfer mode
 */
#define YT8531C_BMCR_ANE_MASK (0x1000U)
#define YT8531C_BMCR_ANE_SHIFT (12U)
#define YT8531C_BMCR_ANE_SET(x) (((uint16_t)(x) << YT8531C_BMCR_ANE_SHIFT) & YT8531C_BMCR_ANE_MASK)
#define YT8531C_BMCR_ANE_GET(x) (((uint16_t)(x) & YT8531C_BMCR_ANE_MASK) >> YT8531C_BMCR_ANE_SHIFT)

/*
 * PWD (RW)
 *
 * This bit turns down the power of the PHY chip
 * The MDC, MDIO is still alive for accessing the MAC.
 * 1: Power down  0: Normal operation
 */
#define YT8531C_BMCR_PWD_MASK (0x800U)
#define YT8531C_BMCR_PWD_SHIFT (11U)
#define YT8531C_BMCR_PWD_SET(x) (((uint16_t)(x) << YT8531C_BMCR_PWD_SHIFT) & YT8531C_BMCR_PWD_MASK)
#define YT8531C_BMCR_PWD_GET(x) (((uint16_t)(x) & YT8531C_BMCR_PWD_MASK) >> YT8531C_BMCR_PWD_SHIFT)

/*
 * ISOLATE (RW)
 *
 * 1: Electrically isolate the PHY from MII/RMII
 * PHY is still able to respond to MDC/MDIO.
 * 0: Normal operation
 */
#define YT8531C_BMCR_ISOLATE_MASK (0x400U)
#define YT8531C_BMCR_ISOLATE_SHIFT (10U)
#define YT8531C_BMCR_ISOLATE_SET(x) (((uint16_t)(x) << YT8531C_BMCR_ISOLATE_SHIFT) & YT8531C_BMCR_ISOLATE_MASK)
#define YT8531C_BMCR_ISOLATE_GET(x) (((uint16_t)(x) & YT8531C_BMCR_ISOLATE_MASK) >> YT8531C_BMCR_ISOLATE_SHIFT)

/*
 * RESTART_AN (RW/SC)
 *
 * This bit allows the NWay auto-negotiation function to be reset.
 * 1: Re-start auto-negotiation  0: Normal operation
 */
#define YT8531C_BMCR_RESTART_AN_MASK (0x200U)
#define YT8531C_BMCR_RESTART_AN_SHIFT (9U)
#define YT8531C_BMCR_RESTART_AN_SET(x) (((uint16_t)(x) << YT8531C_BMCR_RESTART_AN_SHIFT) & YT8531C_BMCR_RESTART_AN_MASK)
#define YT8531C_BMCR_RESTART_AN_GET(x) (((uint16_t)(x) & YT8531C_BMCR_RESTART_AN_MASK) >> YT8531C_BMCR_RESTART_AN_SHIFT)

/*
 * DUPLEX (RW)
 *
 * This bit sets the duplex mode if auto-negotiation is disabled (bit
 * 0:12=0).
 * 1: Full duplex  0: Half duplex
 */
#define YT8531C_BMCR_DUPLEX_MASK (0x100U)
#define YT8531C_BMCR_DUPLEX_SHIFT (8U)
#define YT8531C_BMCR_DUPLEX_SET(x) (((uint16_t)(x) << YT8531C_BMCR_DUPLEX_SHIFT) & YT8531C_BMCR_DUPLEX_MASK)
#define YT8531C_BMCR_DUPLEX_GET(x) (((uint16_t)(x) & YT8531C_BMCR_DUPLEX_MASK) >> YT8531C_BMCR_DUPLEX_SHIFT)

/*
 * COLLISION_TEST (RW)
 *
 * Collision Test.
 * 1: Collision test enabled
 * 0: Normal operation
 * When set, this bit will cause the COL signal to be asserted in
 * response to the TXEN assertion within 512-bit times. The COL
 * signal will be de-asserted within 4-bit times in response to the
 * TXEN de-assertion.
 */
#define YT8531C_BMCR_COLLISION_TEST_MASK (0x80U)
#define YT8531C_BMCR_COLLISION_TEST_SHIFT (7U)
#define YT8531C_BMCR_COLLISION_TEST_SET(x) (((uint16_t)(x) << YT8531C_BMCR_COLLISION_TEST_SHIFT) & YT8531C_BMCR_COLLISION_TEST_MASK)
#define YT8531C_BMCR_COLLISION_TEST_GET(x) (((uint16_t)(x) & YT8531C_BMCR_COLLISION_TEST_MASK) >> YT8531C_BMCR_COLLISION_TEST_SHIFT)

/*
 * SPEED1 (RW)
 *
 * Speed Select Bit 1.
 * Refer to bit 13.
 */
#define YT8531C_BMCR_SPEED1_MASK (0x40U)
#define YT8531C_BMCR_SPEED1_SHIFT (6U)
#define YT8531C_BMCR_SPEED1_SET(x) (((uint16_t)(x) << YT8531C_BMCR_SPEED1_SHIFT) & YT8531C_BMCR_SPEED1_MASK)
#define YT8531C_BMCR_SPEED1_GET(x) (((uint16_t)(x) & YT8531C_BMCR_SPEED1_MASK) >> YT8531C_BMCR_SPEED1_SHIFT)

/* Bitfield definition for register: BMSR */
/*
 * BASE100_T4_1 (RO)
 *
 * 1: Enable 100Base-T4 support
 * 0: Suppress 100Base-T4 support
 */
#define YT8531C_BMSR_BASE100_T4_1_MASK (0x8000U)
#define YT8531C_BMSR_BASE100_T4_1_SHIFT (15U)
#define YT8531C_BMSR_BASE100_T4_1_GET(x) (((uint16_t)(x) & YT8531C_BMSR_BASE100_T4_1_MASK) >> YT8531C_BMSR_BASE100_T4_1_SHIFT)

/*
 * BASE100_TX_FD_1 (RO)
 *
 * 1: Enable 100Base-TX full duplex support
 * 0: Suppress 100Base-TX full duplex support
 */
#define YT8531C_BMSR_BASE100_TX_FD_1_MASK (0x4000U)
#define YT8531C_BMSR_BASE100_TX_FD_1_SHIFT (14U)
#define YT8531C_BMSR_BASE100_TX_FD_1_GET(x) (((uint16_t)(x) & YT8531C_BMSR_BASE100_TX_FD_1_MASK) >> YT8531C_BMSR_BASE100_TX_FD_1_SHIFT)

/*
 * BASE100_TX_HD_1 (RO)
 *
 * 1: Enable 100Base-TX half duplex support
 * 0: Suppress 100Base-TX half duplex support
 */
#define YT8531C_BMSR_BASE100_TX_HD_1_MASK (0x2000U)
#define YT8531C_BMSR_BASE100_TX_HD_1_SHIFT (13U)
#define YT8531C_BMSR_BASE100_TX_HD_1_GET(x) (((uint16_t)(x) & YT8531C_BMSR_BASE100_TX_HD_1_MASK) >> YT8531C_BMSR_BASE100_TX_HD_1_SHIFT)

/*
 * BASE10_TX_FD (RO)
 *
 * 1: Enable 10Base-T full duplex support
 * 0: Suppress 10Base-T full duplex support
 */
#define YT8531C_BMSR_BASE10_TX_FD_MASK (0x1000U)
#define YT8531C_BMSR_BASE10_TX_FD_SHIFT (12U)
#define YT8531C_BMSR_BASE10_TX_FD_GET(x) (((uint16_t)(x) & YT8531C_BMSR_BASE10_TX_FD_MASK) >> YT8531C_BMSR_BASE10_TX_FD_SHIFT)

/*
 * BASE10_TX_HD (RO)
 *
 * 1: Enable 10Base-T half duplex support
 * 0: Suppress 10Base-T half duplex support
 */
#define YT8531C_BMSR_BASE10_TX_HD_MASK (0x800U)
#define YT8531C_BMSR_BASE10_TX_HD_SHIFT (11U)
#define YT8531C_BMSR_BASE10_TX_HD_GET(x) (((uint16_t)(x) & YT8531C_BMSR_BASE10_TX_HD_MASK) >> YT8531C_BMSR_BASE10_TX_HD_SHIFT)

/*
 * MDIO_MFPS (RO)
 *
 */
#define YT8531C_BMSR_MDIO_MFPS_MASK (0x40U)
#define YT8531C_BMSR_MDIO_MFPS_SHIFT (6U)
#define YT8531C_BMSR_MDIO_MFPS_GET(x) (((uint16_t)(x) & YT8531C_BMSR_MDIO_MFPS_MASK) >> YT8531C_BMSR_MDIO_MFPS_SHIFT)

/*
 * AUTO_NEGOTIATION_COMPLETE (RO)
 *
 * 1: Auto-negotiation process completed
 * 0: Auto-negotiation process not completed
 */
#define YT8531C_BMSR_AUTO_NEGOTIATION_COMPLETE_MASK (0x20U)
#define YT8531C_BMSR_AUTO_NEGOTIATION_COMPLETE_SHIFT (5U)
#define YT8531C_BMSR_AUTO_NEGOTIATION_COMPLETE_GET(x) (((uint16_t)(x) & YT8531C_BMSR_AUTO_NEGOTIATION_COMPLETE_MASK) >> YT8531C_BMSR_AUTO_NEGOTIATION_COMPLETE_SHIFT)

/*
 * REMOTE_FAULT (RC)
 *
 * 1: Remote fault condition detected (cleared on read)
 * 0: No remote fault condition detected
 */
#define YT8531C_BMSR_REMOTE_FAULT_MASK (0x10U)
#define YT8531C_BMSR_REMOTE_FAULT_SHIFT (4U)
#define YT8531C_BMSR_REMOTE_FAULT_GET(x) (((uint16_t)(x) & YT8531C_BMSR_REMOTE_FAULT_MASK) >> YT8531C_BMSR_REMOTE_FAULT_SHIFT)

/*
 * AUTO_NEGOTIATION_ABILITY (RO)
 *
 * 1: PHY is able to perform auto-negotiation
 * 0: PHY is not able to perform auto-negotiation
 */
#define YT8531C_BMSR_AUTO_NEGOTIATION_ABILITY_MASK (0x8U)
#define YT8531C_BMSR_AUTO_NEGOTIATION_ABILITY_SHIFT (3U)
#define YT8531C_BMSR_AUTO_NEGOTIATION_ABILITY_GET(x) (((uint16_t)(x) & YT8531C_BMSR_AUTO_NEGOTIATION_ABILITY_MASK) >> YT8531C_BMSR_AUTO_NEGOTIATION_ABILITY_SHIFT)

/*
 * LINK_STATUS (RO)
 *
 * 1: Valid link established
 * 0: No valid link established
 */
#define YT8531C_BMSR_LINK_STATUS_MASK (0x4U)
#define YT8531C_BMSR_LINK_STATUS_SHIFT (2U)
#define YT8531C_BMSR_LINK_STATUS_GET(x) (((uint16_t)(x) & YT8531C_BMSR_LINK_STATUS_MASK) >> YT8531C_BMSR_LINK_STATUS_SHIFT)

/*
 * JABBER_DETECT (RO)
 *
 * 1: Jabber condition detected
 * 0: No jabber condition detected
 */
#define YT8531C_BMSR_JABBER_DETECT_MASK (0x2U)
#define YT8531C_BMSR_JABBER_DETECT_SHIFT (1U)
#define YT8531C_BMSR_JABBER_DETECT_GET(x) (((uint16_t)(x) & YT8531C_BMSR_JABBER_DETECT_MASK) >> YT8531C_BMSR_JABBER_DETECT_SHIFT)

/*
 * EXTENDED_CAPABILITY (RO)
 *
 * 1: Extended register capable (permanently=1)
 * 0: Not extended register capable
 */
#define YT8531C_BMSR_EXTENDED_CAPABILITY_MASK (0x1U)
#define YT8531C_BMSR_EXTENDED_CAPABILITY_SHIFT (0U)
#define YT8531C_BMSR_EXTENDED_CAPABILITY_GET(x) (((uint16_t)(x) & YT8531C_BMSR_EXTENDED_CAPABILITY_MASK) >> YT8531C_BMSR_EXTENDED_CAPABILITY_SHIFT)

/* Bitfield definition for register: PHYID1 */
/*
 * OUI_MSB (RO)
 *
 * JLSemi OUI is 0x24DF10
 * 0010 0100 1101 1111 0001 0000
 * BIT1.......................................................BIT24
 * Register 2.[15:0] show bit3 to 18 of OUI
 * 1001 0011 0111 1100
 * BIT3................................BIT18
 */
#define YT8531C_PHYID1_OUI_MSB_MASK (0xFFFFU)
#define YT8531C_PHYID1_OUI_MSB_SHIFT (0U)
#define YT8531C_PHYID1_OUI_MSB_GET(x) (((uint16_t)(x) & YT8531C_PHYID1_OUI_MSB_MASK) >> YT8531C_PHYID1_OUI_MSB_SHIFT)

/* Bitfield definition for register: PHYID2 */
/*
 * OUI_LSB (RO)
 *
 * Organizationally Unique Identifier bits 19:24
 * 01 0000
 * bit19....bit24
 */
#define YT8531C_PHYID2_OUI_LSB_MASK (0xFC00U)
#define YT8531C_PHYID2_OUI_LSB_SHIFT (10U)
#define YT8531C_PHYID2_OUI_LSB_GET(x) (((uint16_t)(x) & YT8531C_PHYID2_OUI_LSB_MASK) >> YT8531C_PHYID2_OUI_LSB_SHIFT)

/*
 * MODEL_NUMBER (RO)
 *
 * Model Number
 */
#define YT8531C_PHYID2_MODEL_NUMBER_MASK (0x3F0U)
#define YT8531C_PHYID2_MODEL_NUMBER_SHIFT (4U)
#define YT8531C_PHYID2_MODEL_NUMBER_GET(x) (((uint16_t)(x) & YT8531C_PHYID2_MODEL_NUMBER_MASK) >> YT8531C_PHYID2_MODEL_NUMBER_SHIFT)

/*
 * REVISION_NUMBER (RO)
 *
 * Contact JLSemi FAEs for information on the device revision number
 */
#define YT8531C_PHYID2_REVISION_NUMBER_MASK (0xFU)
#define YT8531C_PHYID2_REVISION_NUMBER_SHIFT (0U)
#define YT8531C_PHYID2_REVISION_NUMBER_GET(x) (((uint16_t)(x) & YT8531C_PHYID2_REVISION_NUMBER_MASK) >> YT8531C_PHYID2_REVISION_NUMBER_SHIFT)

#define YT8531C_SR_LINK_STATUS_MASK (0x400U)
#define YT8531C_SR_LINK_STATUS_SHIFT (10U)
#define YT8531C_SR_LINK_STATUS_SET(x) (((uint16_t)(x) << YT8531C_SR_LINK_STATUS_SHIFT) & YT8531C_SR_LINK_STATUS_MASK)
#define YT8531C_SR_LINK_STATUS_GET(x) (((uint16_t)(x) & YT8531C_SR_LINK_STATUS_MASK) >> YT8531C_SR_LINK_STATUS_SHIFT)

#define YT8531C_SR_DUPLEX_STATUS_MASK (0x2000U)
#define YT8531C_SR_DUPLEX_STATUS_SHIFT (13U)
#define YT8531C_SR_DUPLEX_STATUS_SET(x) (((uint16_t)(x) << YT8531C_SR_DUPLEX_STATUS_SHIFT) & YT8531C_SR_DUPLEX_STATUS_MASK)
#define YT8531C_SR_DUPLEX_STATUS_GET(x) (((uint16_t)(x) & YT8531C_SR_DUPLEX_STATUS_MASK) >> YT8531C_SR_DUPLEX_STATUS_SHIFT)

#define YT8531C_SR_SPEED_MODE_MASK (0xC000U)
#define YT8531C_SR_SPEED_MODE_SHIFT (14U)
#define YT8531C_SR_SPEED_MODE_SET(x) (((uint16_t)(x) << YT8531C_SR_SPEED_MODE_SHIFT) & YT8531C_SR_SPEED_MODE_MASK)
#define YT8531C_SR_SPEED_MODE_GET(x) (((uint16_t)(x) & YT8531C_SR_SPEED_MODE_MASK) >> YT8531C_SR_SPEED_MODE_SHIFT)

#endif /* HPM_YT8531C_REGS_H */