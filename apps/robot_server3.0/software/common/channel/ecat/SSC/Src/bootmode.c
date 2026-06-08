/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup ESM EtherCAT State Machine
@{
*/

/**
\file bootmode.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.12

<br>Changes to version V4.20:<br>
V5.12 BOOT2: call BL_Start() from Init to Boot<br>
<br>Changes to version - :<br>
V4.20: File created
*/

/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include "ecat_def.h"


#define    _BOOTMODE_ 1
#include "bootmode.h"
#undef _BOOTMODE_

#include "ecatfoe.h"

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    State        Current state

 \brief Called from INIT to BOOT
*////////////////////////////////////////////////////////////////////////////////////////
void BL_Start( UINT8 State)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**

\brief Called in the state transition from BOOT to Init
*////////////////////////////////////////////////////////////////////////////////////////
void BL_Stop(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    password    download password

 \brief Dummy BL_StartDownload function
*////////////////////////////////////////////////////////////////////////////////////////
void BL_StartDownload(UINT32 password)
{
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pData    Data pointer
 \param    Size    Data Length


 \return    FoE error code

 \brief Dummy BL_Data function
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 BL_Data(UINT16 *pData,UINT16 Size)
{
    return 0;
}
/** @} */
