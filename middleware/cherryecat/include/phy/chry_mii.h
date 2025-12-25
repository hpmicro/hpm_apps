/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CHRY_MII_H
#define CHRY_MII_H

/* Generic MII registers. */
#define MII_BMCR          0x00 /* Basic Mode Control Register */
#define MII_BMSR          0x01 /* Basic Mode Status Register  */
#define MII_PHYSID1       0x02 /* PHY Identifier Register #1  */
#define MII_PHYSID2       0x03 /* PHY Identifier Register #2  */
#define MII_ANAR          0x04 /* Auto-Negotiation Advertisement Register   */
#define MII_ANLPAR        0x05 /* Auto-Negotiation Link Partner Ability Register */
#define MII_ANER          0x06 /* Auto-Negotiate Expansion Register          */
#define MII_ANNPTR        0x07 /* Auto-Negotiation Next Page Transmit Register */
#define MII_ANNPRR        0x08 /* Auto-Negotiation Next Page Receive Register  */
#define MII_GBCR          0x09 /* 1000Base-T Control Register          */
#define MII_GBSR          0x0a /* 1000Base-T Status Register           */
#define MII_GBESR         0x0f /* 1000Base-T Extended Status Register  */

/* Basic Mode Control Register. */
#define BMCR_RESET        (1 << 15) /* Reset to default state      */
#define BMCR_LOOPBACK     (1 << 14) /* TXD loopback bits           */
#define BMCR_SPEED100     (1 << 13) /* Select 100Mbps or 10Mbps    */
#define BMCR_ANENABLE     (1 << 12) /* Enable auto negotiation     */
#define BMCR_POWERDOWN    (1 << 11) /* Enable low power state      */
#define BMCR_ISOLATE      (1 << 10) /* Isolate data paths from MII */
#define BMCR_ANRESTART    (1 << 9)  /* Auto negotiation restart    */
#define BMCR_FULLDPLX     (1 << 8)  /* Full duplex                 */
#define BMCR_CTST         (1 << 7)  /* Collision test              */
#define BMCR_SPEED1000    (1 << 6)  /* MSB of Speed (1000)         */
#define BMCR_RESV         0x003f    /* Unused...                   */

/* Basic Mode Status Register. */
#define BMSR_100T4        (1 << 15) /* Enable 100Base-T4 support  */
#define BMSR_100FULL      (1 << 14) /* Enable 100Base-TX full duplex support  */
#define BMSR_100HALF      (1 << 13) /* Enable 100Base-TX half duplex support  */
#define BMSR_10FULL       (1 << 12) /* Enable 10Base-TX full duplex support  */
#define BMSR_10HALF       (1 << 11) /* Enable 10Base-TX half duplex support  */
#define BMSR_100HALF2     (1 << 10) /* Can do 100BASE-T2 HDX       */
#define BMSR_100FULL2     (1 << 9)  /* Can do 100BASE-T2 FDX       */
#define BMSR_ESTATEN      (1 << 8)  /* Extended Status in R15      */
#define BMSR_ANEGCOMPLETE (1 << 5)  /* Auto-negotiation complete   */
#define BMSR_REMOTEFAULT  (1 << 4)  /* Remote fault detected       */
#define BMSR_ANEGCAPABLE  (1 << 3)  /* Able to do auto-negotiation */
#define BMSR_LINKSTATUS   (1 << 2)  /* Link status                 */
#define BMSR_JCD          (1 << 1)  /* Jabber detected             */
#define BMSR_ERCAP        (1 << 0)  /* Ext-reg capability          */

/* Auto-Negotiation Advertisement Register. */
#define ANAR_NPAGE        (1 << 15) /* Next page bit               */
#define ANAR_ACK          (1 << 14) /* Link partner acknowledges reception of local node’s capability data word   */
#define ANAR_REMOTEFAULT  (1 << 13) /* Link partner is indicating a remote fault    */
#define ANAR_ASYM_PAUSE   (1 << 11) /* Try for asymetric pause     */
#define ANAR_PAUSE        (1 << 10) /* Try for pause               */
#define ANAR_100T4        (1 << 9)  /* 100Base-T4 is supported by local mode  */
#define ANAR_100FULL      (1 << 8)  /* 100Base-TX full duplex is supported by local mode */
#define ANAR_100HALF      (1 << 7)  /* 100Base-TX half duplex is supported by local mode */
#define ANAR_10FULL       (1 << 6)  /* 10Base-TX full duplex is supported by local mode */
#define ANAR_10HALF       (1 << 5)  /* 10Base-TX half duplex is supported by local mode */
#define ANAR_SLCT         0x001f    /* Selector bits               */
#define ANAR_CSMA         0x0001    /* Only selector supported     */

#define ANAR_SPEED_ALL    (ANAR_10HALF | ANAR_10FULL | \
                        ANAR_100HALF | ANAR_100FULL)

/* Auto-Negotiation Link Partner Ability Register. */
#define ANLPAR_NPAGE       (1 << 15) /* Next page bit               */
#define ANLPAR_ACK         (1 << 14) /* Link partner acknowledges reception of local node’s capability data word   */
#define ANLPAR_REMOTEFAULT (1 << 13) /* Link partner is indicating a remote fault    */
#define ANLPAR_ASYM_PAUSE  (1 << 11) /* Try for asymetric pause     */
#define ANLPAR_PAUSE       (1 << 10) /* Try for pause               */
#define ANLPAR_100T4       (1 << 9)  /* 100Base-T4 is supported by local mode  */
#define ANLPAR_100FULL     (1 << 8)  /* 100Base-TX full duplex is supported by local mode */
#define ANLPAR_100HALF     (1 << 7)  /* 100Base-TX half duplex is supported by local mode */
#define ANLPAR_10FULL      (1 << 6)  /* 10Base-TX full duplex is supported by local mode */
#define ANLPAR_10HALF      (1 << 5)  /* 10Base-TX half duplex is supported by local mode */
#define ANLPAR_SLCT        0x001f    /* Selector bits               */
#define ANLPAR_CSMA        0x0001    /* Only selector supported     */

/* 1000Base-T Control Register          */
#define GBCR_1000FULL      0x0200 /* Advertise 1000BASE-T full duplex */
#define GBCR_1000HALF      0x0100 /* Advertise 1000BASE-T half duplex */
#define GBCR_PREFER_MASTER 0x0400 /* prefer to operate as master */
#define GBCR_AS_MASTER     0x0800
#define GBCR_ENABLE_MASTER 0x1000

/* 1000Base-T Status Register           */
#define GBSR_1000MSFAIL    0x8000 /* Master/Slave resolution failure */
#define GBSR_1000MSRES     0x4000 /* Master/Slave resolution status */
#define GBSR_1000LOCALRXOK 0x2000 /* Link partner local receiver status */
#define GBSR_1000REMRXOK   0x1000 /* Link partner remote receiver status */
#define GBSR_1000FULL      0x0800 /* Link partner 1000BASE-T full duplex */
#define GBSR_1000HALF      0x0400 /* Link partner 1000BASE-T half duplex */

/* 1000Base-T Extended Status Register  */
#define GBESR_1000_XFULL   0x8000 /* Can do 1000BaseX Full       */
#define GBESR_1000_XHALF   0x4000 /* Can do 1000BaseX Half       */
#define GBESR_1000_TFULL   0x2000 /* Can do 1000BT Full          */
#define GBESR_1000_THALF   0x1000 /* Can do 1000BT Half          */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif