/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

 #ifndef _GATEWAY_APPS_H_
#define _GATEWAY_APPS_H_


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "ecatappl.h"

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/

#endif //_GATEWAY_APPS_H_

//include custom application object dictionary 
#include "gateway_appsObjects.h"
#include "gw_mq_msg.h"
#include "gw_mq.h"


#if defined(_GATEWAY_APPS_) && (_GATEWAY_APPS_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif

#define MODULEID_CAN   0x100
#define MODULEID_CANFD 0x101



PROTO void APPL_Application(void);
#if EXPLICIT_DEVICE_ID
PROTO UINT16 APPL_GetDeviceID(void);
#endif

PROTO void   APPL_AckErrorInd(UINT16 stateTrans);
PROTO UINT16 APPL_StartMailboxHandler(void);
PROTO UINT16 APPL_StopMailboxHandler(void);
PROTO UINT16 APPL_StartInputHandler(UINT16 *pIntMask);
PROTO UINT16 APPL_StopInputHandler(void);
PROTO UINT16 APPL_StartOutputHandler(void);
PROTO UINT16 APPL_StopOutputHandler(void);

PROTO UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize);
PROTO void APPL_InputMapping(UINT16* pData);
PROTO void APPL_OutputMapping(UINT16* pData);

#undef PROTO
/** @}*/

