/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_KSZ8081_REGS_H
#define HPM_KSZ8081_REGS_H

/* Bitfield definition for register: BMCR */
/*
 * Reset (RW)
 *
 * 1: PHY reset
 * 0: Normal operation
 * Register 0 (BMCR) and register 1 (BMSR) will return to default
 * values after a software reset (set Bit15 to 1).
 * This action may change the internal PHY state and the state of the
 * physical link associated with the PHY.
 */
#define KSZ8081_BMCR_RESET_MASK  (0x8000U)
#define KSZ8081_BMCR_RESET_SHIFT (15U)
#define KSZ8081_BMCR_RESET_SET(x) (((uint32_t)(x) << KSZ8081_BMCR_RESET_SHIFT) & KSZ8081_BMCR_RESET_MASK)
#define KSZ8081_BMCR_RESET_GET(x) (((uint32_t)(x) & KSZ8081_BMCR_RESET_MASK) >> KSZ8081_BMCR_RESET_SHIFT)

/* Bitfield definition for register: PHYID1 */
/*
 * OUI_MSB (RO)
 *
 * Organizationally Unique Identifier Bit 3:18.
 * Always 0000000000011100.
 */
#define KSZ8081_PHYID1_OUI_MSB_MASK  (0xFFFFU)
#define KSZ8081_PHYID1_OUI_MSB_SHIFT (0U)
#define KSZ8081_PHYID1_OUI_MSB_GET(x) (((uint32_t)(x) & KSZ8081_PHYID1_OUI_MSB_MASK) >> KSZ8081_PHYID1_OUI_MSB_SHIFT)

/* Bitfield definition for register: PHYID2 */
/*
 * OUI_LSB (RO)
 *
 * Organizationally Unique Identifier Bit 19:24.
 * Always 110010.
 */
#define KSZ8081_PHYID2_OUI_MSB_MASK  (0xFC00U)
#define KSZ8081_PHYID2_OUI_MSB_SHIFT (10U)
#define KSZ8081_PHYID2_OUI_MSB_GET(x) (((uint32_t)(x) & KSZ8081_PHYID2_OUI_MSB_MASK) >> KSZ8081_PHYID2_OUI_MSB_SHIFT)

 //**********************

/*******************************************************************************
 * Definitions
 ******************************************************************************/

 /*! @brief PHY driver version */
#define FSL_KSZ8081_PHY_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*! @brief Defines the PHY registers. */
#define KSZ8081_PHY_BASICCONTROL_REG      0x00U /*!< The PHY basic control register. */
#define KSZ8081_PHY_BASICSTATUS_REG       0x01U /*!< The PHY basic status register. */
#define KSZ8081_PHY_ID1_REG               0x02U /*!< The PHY ID one register. */
#define KSZ8081_PHY_ID2_REG               0x03U /*!< The PHY ID two register. */
#define KSZ8081_PHY_AUTONEG_ADVERTISE_REG 0x04U /*!< The PHY auto-negotiate advertise register. */
#define KSZ8081_PHY_CONTROL1_REG          0x1EU /*!< The PHY control one register. */
#define KSZ8081_PHY_CONTROL2_REG          0x1FU /*!< The PHY control two register. */

#define KSZ8081_PHY_CONTROL_ID1 0x22U /*!< The PHY ID1*/

/*! @brief Defines the mask flag in basic control register. */
#define KSZ8081_PHY_BCTL_DUPLEX_MASK          0x0100U /*!< The PHY duplex bit mask. */
#define KSZ8081_PHY_BCTL_RESTART_AUTONEG_MASK 0x0200U /*!< The PHY restart auto negotiation mask. */
#define KSZ8081_PHY_BCTL_AUTONEG_MASK         0x1000U /*!< The PHY auto negotiation bit mask. */
#define KSZ8081_PHY_BCTL_SPEED_MASK           0x2000U /*!< The PHY speed bit mask. */
#define KSZ8081_PHY_BCTL_LOOP_MASK            0x4000U /*!< The PHY loop bit mask. */
#define KSZ8081_PHY_BCTL_RESET_MASK           0x8000U /*!< The PHY reset bit mask. */
#define KSZ8081_PHY_BCTL_SPEED_100M_MASK      0x2000U /*!< The PHY 100M speed mask. */

/*!@brief Defines the mask flag of operation mode in control two register*/
#define KSZ8081_PHY_CTL2_REMOTELOOP_MASK    0x0004U /*!< The PHY remote loopback mask. */
#define KSZ8081_PHY_CTL2_REFCLK_SELECT_MASK 0x0080U /*!< The PHY RMII reference clock select. */
#define KSZ8081_PHY_CTL1_10HALFDUPLEX_MASK  0x0001U /*!< The PHY 10M half duplex mask. */
#define KSZ8081_PHY_CTL1_100HALFDUPLEX_MASK 0x0002U /*!< The PHY 100M half duplex mask. */
#define KSZ8081_PHY_CTL1_10FULLDUPLEX_MASK  0x0005U /*!< The PHY 10M full duplex mask. */
#define KSZ8081_PHY_CTL1_100FULLDUPLEX_MASK 0x0006U /*!< The PHY 100M full duplex mask. */
#define KSZ8081_PHY_CTL1_SPEEDUPLX_MASK     0x0007U /*!< The PHY speed and duplex mask. */
#define KSZ8081_PHY_CTL1_ENERGYDETECT_MASK  0x10U   /*!< The PHY signal present on rx differential pair. */
#define KSZ8081_PHY_CTL1_LINKUP_MASK        0x100U  /*!< The PHY link up. */
#define KSZ8081_PHY_LINK_READY_MASK         (KSZ8081_PHY_CTL1_ENERGYDETECT_MASK | KSZ8081_PHY_CTL1_LINKUP_MASK)

/*! @brief Defines the mask flag in basic status register. */
#define KSZ8081_PHY_BSTATUS_LINKSTATUS_MASK  0x0004U /*!< The PHY link status mask. */
#define KSZ8081_PHY_BSTATUS_AUTONEGABLE_MASK 0x0008U /*!< The PHY auto-negotiation ability mask. */
#define KSZ8081_PHY_BSTATUS_AUTONEGCOMP_MASK 0x0020U /*!< The PHY auto-negotiation complete mask. */

/*! @brief Defines the mask flag in PHY auto-negotiation advertise register. */
#define KSZ8081_PHY_100BaseT4_ABILITY_MASK    0x200U /*!< The PHY have the T4 ability. */
#define KSZ8081_PHY_100BASETX_FULLDUPLEX_MASK 0x100U /*!< The PHY has the 100M full duplex ability.*/
#define KSZ8081_PHY_100BASETX_HALFDUPLEX_MASK 0x080U /*!< The PHY has the 100M full duplex ability.*/
#define KSZ8081_PHY_10BASETX_FULLDUPLEX_MASK  0x040U /*!< The PHY has the 10M full duplex ability.*/
#define KSZ8081_PHY_10BASETX_HALFDUPLEX_MASK  0x020U /*!< The PHY has the 10M full duplex ability.*/

#endif /* HPM_KSZ8081_REGS_H */