/**
 * Copyright (c) 2021-2024 HPMicro
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#ifndef __FULL_PORT_AS_DRV_H_
#define __FULL_PORT_AS_DRV_H_

typedef struct _FULL_PORT_PTP_Protocol_Head_
{
    union {
        uint8_t offset0;
        struct {
            uint8_t messageId: 4;
            uint8_t transportSpecific: 4;
        } offset0_bm;
    };
    union {
        uint8_t offset1;
        struct {
            uint8_t versionPTP: 4;
            uint8_t reserved0:  4;
        } offset1_bm;
    };
    uint8_t messageLen[2];
    uint8_t domainNum;
    uint8_t reserved1;
    uint8_t flags[2];
    uint8_t correctionField[8];
    uint8_t reserved2[4];
    uint8_t sourcePortIdentity[10];
    uint8_t sequenceId[0];
    uint8_t control;
    uint8_t logMean;
} FULL_PORT_PTP_Protocol_Head_st;

typedef struct _FULL_PORT_TSN_Head_
{
    union {
        uint32_t hdr0;
        struct {
            uint32_t src_port:  8;
            uint32_t reserved0: 8;
            uint32_t queue:     3;
            uint32_t utag:      3;
            uint32_t reserved1: 2;
            uint32_t fpe:       1;
            uint32_t reserved2: 3;
            uint32_t htype:     4;
        } hdr0_bm;
    };
    uint32_t cb;
    uint32_t tstamp_lo;
    uint32_t tstamp_hi;
}FULL_PORT_TSN_Head_st;

typedef struct _FULL_PORT_PTP_Based_TSN_Sync_For_TX_
{
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_Sync_For_TX_st;

typedef struct _FULL_PORT_PTP_Based_TSN_FollowUp_For_TX_
{
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_FollowUp_For_TX_st;

typedef struct _FULL_PORT_PTP_Based_DelayReq_For_TX__
{
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_DelayReq_For_TX_st;

typedef struct _FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_
{
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
    uint8_t portId;
}FULL_PORT_PTP_Based_TSN_DelayResp_For_TX_st;

typedef struct _FULL_PORT_PTP_Based_TSN_Sync_
{
    FULL_PORT_TSN_Head_st stTSNHead;
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_Sync_st;

typedef struct _FULL_PORT_PTP_Based_TSN_FollowUp_
{
    FULL_PORT_TSN_Head_st stTSNHead;
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_FollowUp_st;

typedef struct _FULL_PORT_PTP_Based_DelayReq__
{
    FULL_PORT_TSN_Head_st stTSNHead;
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
}FULL_PORT_PTP_Based_TSN_DelayReq_st;

typedef struct _FULL_PORT_PTP_Based_TSN_DelayResp_
{
    FULL_PORT_TSN_Head_st stTSNHead;
    uint8_t des_mac[6];
    uint8_t src_mac[6];
    uint8_t protocol[2];
    FULL_PORT_PTP_Protocol_Head_st stPtpProtocolHead;
    uint8_t timestamp[10];
    uint8_t portId;
}FULL_PORT_PTP_Based_TSN_DelayResp_st;


#endif // __FULL_PORT_AS_H_