/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
 * \addtogroup FoE FileTransfer over EtherCAT
 * @{
 */

/**
\file foeappl.h
\author EthercatSSC@beckhoff.com

\version 5.12

<br>Changes to version V5.01:<br>
V5.12 FOE1: update new interface,move the FoE sample to sampleappl,add FoE application callback functions<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#ifndef _FOEAPPL_H_
#define _FOEAPPL_H_

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/
/*---------------------------------------------
-    Error Codes
-----------------------------------------------*/
#define    ECAT_FOE_ERRCODE_NOTDEFINED          0x8000 /**< \brief Not defined*/
#define    ECAT_FOE_ERRCODE_NOTFOUND            0x8001 /**< \brief The file requested by an FoE upload service could not be found on the server*/
#define    ECAT_FOE_ERRCODE_ACCESS              0x8002 /**< \brief Read or write access to this file not allowed (e.g. due to local control).*/
#define    ECAT_FOE_ERRCODE_DISKFULL            0x8003 /**< \brief Disk to store file is full or memory allocation exceeded*/
#define    ECAT_FOE_ERRCODE_ILLEGAL             0x8004 /**< \brief Illegal FoE operation, e.g. service identifier invalid*/
#define    ECAT_FOE_ERRCODE_PACKENO             0x8005 /**< \brief FoE packet number invalid*/
#define    ECAT_FOE_ERRCODE_EXISTS              0x8006 /**< \brief The file which is requested to be downloaded does already exist*/
#define    ECAT_FOE_ERRCODE_NOUSER              0x8007 /**< \brief No User*/
#define    ECAT_FOE_ERRCODE_BOOTSTRAPONLY       0x8008 /**< \brief FoE only supported in Bootstrap*/
#define    ECAT_FOE_ERRCODE_NOTINBOOTSTRAP      0x8009 /**< \brief This file may not be accessed in BOOTSTRAP state*/
#define    ECAT_FOE_ERRCODE_NORIGHTS            0x800A /**< \brief Password invalid*/
#define    ECAT_FOE_ERRCODE_PROGERROR           0x800B /**< \brief Generic programming error. Should only be returned if  error reason cannot be distinguished*/
#define    ECAT_FOE_ERRCODE_INVALID_CHECKSUM    0x800C /**< \brief checksum included in the file is invalid*/
#define    ECAT_FOE_ERRCODE_INVALID_FIRMWARE    0x800D /**< \brief The hardware does not support the downloaded firmware*/
#define    ECAT_FOE_ERRCODE_NO_FILE             0x800F /**< \brief Do not use (identical with 0x8001)*/
#define    ECAT_FOE_ERRCODE_NO_FILE_HEADER      0x8010 /**< \brief Missing file header of error in file header*/
#define    ECAT_FOE_ERRCODE_FLASH_ERROR         0x8011 /**< \brief Flash cannot be accessed*/

#endif //_FOEAPPL_H_

#ifdef _FOEAPPL_
    #define PROTO
#else
    #define PROTO extern
#endif


/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
------
------    Global Functions
------
-----------------------------------------------------------------------------------------*/
PROTO    UINT16     FOE_Read(UINT16 MBXMEM * pName, UINT16 nameSize, UINT16 MBXMEM * pData, UINT32 password);
PROTO    UINT16     FOE_Write(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password);
PROTO    UINT16     FOE_Data(UINT16 MBXMEM * pData, UINT16 Size);
PROTO    UINT16     FOE_Ack(UINT32 fileOffset, UINT16 MBXMEM * pData);
PROTO    UINT16     FOE_Busy(UINT16 done, UINT32 fileOffset, UINT16 MBXMEM * pData);
PROTO    void       FOE_Error(UINT32 errorCode);

#undef PROTO
/** @}*/
