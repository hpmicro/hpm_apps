/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup EcatAppl EtherCAT application
@{
*/

/**
\file applInterface.h
\author EthercatSSC@beckhoff.com
\brief Definition of the application interface functions

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 COE4: update default entry name handling in case of 16Bit characters, add CoE Read/write indication functions<br>
<br>Changes to version V5.11:<br>
V5.12 APPL1: add optional application function called from the main loop (after mbx and esm are executed)<br>
V5.12 EEPROM3: implement a store EEPROM timeout handler<br>
V5.12 EOE1: move icmp sample to the sampleappl,add EoE application interface functions<br>
V5.12 FOE1: update new interface,move the FoE sample to sampleappl,add FoE application callback functions<br>
<br>Changes to version - :<br>
V5.10.1 : Start file change log
*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/


#ifndef _APPL_INTERFACE_H_
#define _APPL_INTERFACE_H_

#include "ecat_def.h"
#include <stdlib.h>
#include <string.h>
#include "esc.h"
#include "ecatslv.h"
#include "objdef.h"
#include "ecatappl.h"




#include "foeappl.h"

#endif /*#ifndef _APPL_INTERFACE_H_*/

#if defined(_APPL_INTERFACE_) && (_APPL_INTERFACE_ == 1)
   #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Function pointer which are used to call optional functions from the application
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return   0 if successful or greater 0 in case of an error
 
 \param    wordaddr      Word address of the EEPROM data to be read
 
 \brief    This function shall copy EEPROM data to the ESC EEPROM data register (0x508:0x50F/0x50B).
 \brief    The EEPROM data starting at the specified word address and the length specified in register 0x502.6
 \brief    The data shall be copied to the ESC EEPROM buffer (ESC offset 0x508)
 \brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16 (* pAPPL_EEPROM_Read)(UINT32 wordaddr);

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return   0 if successful or greater 0 in case of an error

\param    wordaddr      Word address of the EEPROM data to be written

\brief    This function shall copy data from the ESC EEPROM data register (0x508:0x50F/0x50B) to the EEPROM memory.
\brief    The EEPROM data starting at the specified word address and the length specified with "EEPROM_WRITE_SIZE".
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16(*pAPPL_EEPROM_Write)(UINT32 wordaddr);

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return   0 if successful or greater 0 in case of an error

\brief    This function shall copy the EEPROM reload information to the ESC EEPROM data register (0x508:0x50F/0x50B).
\brief    Read the ESC data sheet for the reload information (e.g. Beckhoff IPCore ESC Datasheet section II, clause 3.45.1)
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16(*pAPPL_EEPROM_Reload)(void);

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    If EEPROM Emulation is enabled and the written data is not stored directly during 
\brief    the EEPROM Write commands to the permanent memory this function can be used to store the EEPROM data.
\brief    It is called 1000 ms after the last EEPROM access. Using this function shall only be used if it is 
\brief    not possible to store the EEPROM data directly during the EEPROM write access.
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void(*pAPPL_EEPROM_Store)(void);






/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     pName         Pointer to the name of the file (the pointer is null if the function is called due to a previous busy state)
\param     nameSize      Length of the file name (the value is 0 if the function is called due to a previous busy state)
\param     password      Password for the file read (the value is 0 if the function is called due to a previous busy state)
\param     maxBlockSize  Maximum size of a data block (copied to pData)
\param     pData         Destination pointer for the first FoE fragment
                        


\return                  block size:
                            < FOE_MAXBUSY-101    (0x7F95)
                         busy:
                            FOE_MAXBUSY-100 (0%)    (0x7FFA - 0x64)
                            ...
                            FOE_MAXBUSY (100%) (0x7FFA)
                         error:
                            ECAT_FOE_ERRCODE_NOTDEFINED (0x8000)
                            ECAT_FOE_ERRCODE_NOTFOUND (0x8001)
                            ECAT_FOE_ERRCODE_ACCESS    (0x8002)
                            ECAT_FOE_ERRCODE_DISKFULL (0x8003)
                            ECAT_FOE_ERRCODE_ILLEGAL (0x8004)
                            ECAT_FOE_ERRCODE_EXISTS    (0x8006)
                            ECAT_FOE_ERRCODE_NOUSER    (0x8007)

\brief    The function is called when a file read request was received. The Foe fragments shall always have the length of "maxBlockSize" till the last file fragment.
\brief    In case that the file size is a multiple of "maxBlockSize" 0 shall be returned after the last fragment.
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16 (*pAPPL_FoeRead)(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password, UINT16 maxBlockSize, UINT16 *pData);


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     offset        File offset which shall be transmitted next
\param     maxBlockSize  Maximum size of a data block (copied to pData)
\param     pData         Destination pointer for the next foe fragment



\return                  block size:
                            < FOE_MAXBUSY-101    (0x7F95)
                        busy:
                            FOE_MAXBUSY-100 (0%)    (0x7FFA - 0x64)
                            ...
                            FOE_MAXBUSY (100%) (0x7FFA)
                        error:
                            ECAT_FOE_ERRCODE_NOTDEFINED (0x8000)
                            ECAT_FOE_ERRCODE_NOTFOUND (0x8001)
                            ECAT_FOE_ERRCODE_ACCESS    (0x8002)
                            ECAT_FOE_ERRCODE_DISKFULL (0x8003)
                            ECAT_FOE_ERRCODE_ILLEGAL (0x8004)
                            ECAT_FOE_ERRCODE_EXISTS    (0x8006)
                            ECAT_FOE_ERRCODE_NOUSER    (0x8007)

\brief    The function is called to transmit FoE read data 2 .. n (the slave received an acknowledge on a previous accepted file read request). The Foe fragments shall always have the length of "maxBlockSize" till the last file fragment.
\brief    In case that the file size is a multiple of "maxBlockSize" 0 shall be returned after the last fragment.
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16(*pAPPL_FoeReadData)(UINT32 offset, UINT16 maxBlockSize, UINT16 *pData);


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     errorCode     Error code send by the EtherCAT master


\brief    The function is called when the master has send an FoE Abort.
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void(*pAPPL_FoeError)(UINT32 errorCode);



/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     pName         Pointer to the name of the file
\param     nameSize      Length of the file name
\param     password      Password for the file read

\return                 okay, or an error code
                        0 (okay)
                        ECAT_FOE_ERRCODE_NOTDEFINED (0x8000)
                        ECAT_FOE_ERRCODE_NOTFOUND (0x8001)
                        ECAT_FOE_ERRCODE_ACCESS    (0x8002)
                        ECAT_FOE_ERRCODE_DISKFULL (0x8003)
                        ECAT_FOE_ERRCODE_ILLEGAL (0x8004)
                        ECAT_FOE_ERRCODE_EXISTS    (0x8006)
                        ECAT_FOE_ERRCODE_NOUSER    (0x8007)

\brief    This function is called on a received FoE write request.
\brief    (no busy response shall be returned by this function. If the slave requires some time to handle the incoming data the function pAPPL_FoeData() shall return a busy)
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16 (*pAPPL_FoeWrite)(UINT16 MBXMEM * pName, UINT16 nameSize, UINT32 password);


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     pData            Received file data
\param 	   Size             Length of received file data
\param 	   bDataFollowing   TRUE if more FoE Data requests are following

\return                     okay, busy or an error code
                            0 (okay)
                            FOE_MAXBUSY-100 (0%)    (0x7FFa - 100)
                            FOE_MAXBUSY (100%) (0x7FFA)
                            ECAT_FOE_ERRCODE_NOTDEFINED (0x8000)
                            ECAT_FOE_ERRCODE_ACCESS    (0x8002)
                            ECAT_FOE_ERRCODE_DISKFULL (0x8003)
                            ECAT_FOE_ERRCODE_ILLEGAL (0x8004)
                            ECAT_FOE_ERRCODE_NOUSER    (0x8007)

\brief    This function is called on a FoE Data request
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16(*pAPPL_FoeWriteData)(UINT16 MBXMEM * pData, UINT16 Size, BOOL bDataFollowing);

/* ECATCHANGE_START(V5.13) COE4*/
/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     Index            Index of the object to be read
\param     Subindex         Subindex of the object entry to be read
\param     CompleteAccess   Indicates if the object is read with complete access

\brief    The function is called before an object/entry is read
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void(*pAPPL_CoeReadInd)(UINT16 Index, UINT8 Subindex, BOOL CompleteAccess);

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     Index            Index of the object which was written
\param     Subindex         Subindex of the object entry which was written
\param     CompleteAccess   Indicates if the object was written with complete access

\brief    The function is called after an object/entry was successfully written
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void(*pAPPL_CoeWriteInd)(UINT16 Index, UINT8 Subindex, BOOL CompleteAccess);

/* ECATCHANGE_END(V5.13) COE4*/
/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function is called by the SSC from the main loop
\brief	   The function pointer is reset in MainInit() so it shall be set afterwards
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void(*pAPPL_MainLoop)();

/*-----------------------------------------------------------------------------------------
------
------    Functions provided by the SSC to be called from the application
------
-----------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**
\return   0 if successful or greater 0 in case of an error

\brief    Initialize the generic slave stack.
\brief    This function should be called after the platform including operating system and
\brief    ESC is ready to use.
*////////////////////////////////////////////////////////////////////////////////////////
PROTO UINT16 MainInit(void);


/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function handles the low priority function like EtherCAT state machine
\brief    handling, mailbox protocols and if no synchronization is enabled also the
\brief    application.
\brief    This function shall be called cyclically from the application.
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void MainLoop(void);


/////////////////////////////////////////////////////////////////////////////////////////
/**
\param    alStatus       requested AL Status 
\param    alStatusCode   requested AL Status Code

\brief    This function shall be called by the application to trigger state transition in case
\brief    of an application error or to complete a pending transition.
\brief    If the function was called due to an error it shall be again if the error is gone.
\brief    NOTE: state requests to a higher state than the current state are not allowed.
*////////////////////////////////////////////////////////////////////////////////////////
PROTO void ECAT_StateChange(UINT8 alStatus, UINT16 alStatusCode);



#undef PROTO
