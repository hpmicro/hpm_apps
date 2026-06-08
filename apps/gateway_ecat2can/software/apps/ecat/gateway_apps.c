
/*
 * Copyright (c) 2025 HPMicro, All Rights Reserved
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */
#include "ecat_def.h"

#include "applInterface.h"

#define _GATEWAY_APPS_ 1
#include "gateway_apps.h"
#undef _GATEWAY_APPS_

#include "coeappl.h"
/////////////////////////////////////////////////////////////////////////////////////////
/**
\param    u32ModuleId  Module ident of the request module

\return   Pointer to the module object dictionary

\brief   This function gets the module default object dictionary 
		  (not the module specific dictionary which is mapped to the device object dictionary)
*////////////////////////////////////////////////////////////////////////////////////////
TOBJECT OBJMEM* Module_GetObjectDictionary(UINT32 u32ModuleId)
{
    UINT16 u16Counter = 0;
    UINT16 u16NumModules = SIZEOF(ModuleOds) / SIZEOF(TMODULEODREF);

    for (u16Counter = 0; u16Counter < u16NumModules; u16Counter++)
    {
        if (ModuleOds[u16Counter].u32ModuleId == u32ModuleId)
        {
            return ModuleOds[u16Counter].pObjectDictionary;
        }
    }

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param    u32ModuleId  Module ident of the request module
\return   number of objects

\brief   This functions gets the number of objects defined by the module
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 Module_GetObjectCount(UINT32 u32ModuleId)
{
    TOBJECT OBJMEM* pObject;
    UINT16 u16Counter = 0;

    pObject = Module_GetObjectDictionary(u32ModuleId);
    if (pObject != NULL)
    {
        while (pObject->Index != 0xFFFF)
        {
            u16Counter++;
            pObject++;
        }
    }

    return u16Counter;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return   0 if all module objects are added to the device object dictionary

\brief   This functions add/remove the module related objects to/from the device object dictionary based on the configured module ident list (0xF030)
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 Module_UpdateObjectDictionary()
{
    TOBJECT OBJMEM* pModuleObjEntry = NULL;
    UINT16 u16Counter = 0;
    UINT16 IndexIncrement = 0;
    UINT16 result = 0;
    LOG_I("ConfiguredModuleIdentList0xF030.u16SubIndex0:0x%x\r\n", ConfiguredModuleIdentList0xF030.u16SubIndex0);
    LOG_I("ConfiguredModuleIdentList0xF030.aEntries[0]:0x%x\r\n", ConfiguredModuleIdentList0xF030.aEntries[0]);

    for (u16Counter = 0; u16Counter < 1; u16Counter++)
    {
        UINT32 u32ModuleId = ConfiguredModuleIdentList0xF030.aEntries[u16Counter];

        /*clear module ident if the loop is out of the configured range*/
        if (u16Counter >= ConfiguredModuleIdentList0xF030.u16SubIndex0)
            u32ModuleId = 0;

        if (u32ModuleId != AssignedModules[u16Counter].u32ModuleId)
        {
            /*a new module is assigned to the current slot*/
            if (AssignedModules[u16Counter].u32ModuleId != 0)
            {
                /*Clear the assigned module objects*/
                pModuleObjEntry = AssignedModules[u16Counter].pObjectDictionary;
                while (pModuleObjEntry->Index != 0xFFFF)
                {
                    COE_RemoveDicEntry(pModuleObjEntry->Index);

                    /*free previously allocated memory*/
                    if (pModuleObjEntry->pVarPtr != NULL)
                    {
                        FREEMEM(pModuleObjEntry->pVarPtr);
                        pModuleObjEntry->pVarPtr = NULL;
                    }
                    pModuleObjEntry++;
                }//loop over the current assigned module OD

                FREEMEM(AssignedModules[u16Counter].pObjectDictionary);
            }

            AssignedModules[u16Counter].u32ModuleId = u32ModuleId;
            AssignedModules[u16Counter].pObjectDictionary = NULL;

            if (u32ModuleId != 0)
            {
                UINT16 u16NumObjects = 0;
                UINT16 u16OdSize = 0;
                u16NumObjects = Module_GetObjectCount(u32ModuleId);

                /*add 0xFFFF "object"*/
                u16OdSize = (SIZEOF(TOBJECT) * (u16NumObjects + 1));

                AssignedModules[u16Counter].pObjectDictionary = ALLOCMEM(u16OdSize);

                if (AssignedModules[u16Counter].pObjectDictionary == NULL)
                {
                    /*failed to allocate memory for module OD*/
                    AssignedModules[u16Counter].u32ModuleId = 0;

                    return 1;
                }

                pModuleObjEntry = Module_GetObjectDictionary(u32ModuleId);
                if (pModuleObjEntry == NULL)
                {
                    /*Module object dictionary not found*/
                    AssignedModules[u16Counter].u32ModuleId = 0;
                    FREEMEM(AssignedModules[u16Counter].pObjectDictionary);
                    AssignedModules[u16Counter].pObjectDictionary = NULL;

                    return 1;
                }

                MEMCPY(AssignedModules[u16Counter].pObjectDictionary, pModuleObjEntry, u16OdSize);

                pModuleObjEntry = AssignedModules[u16Counter].pObjectDictionary;
                /*loop over all module object dictionary entries and add to device object dictionary*/
                while (pModuleObjEntry->Index != 0xFFFF)
                {
                    /*Index increment is greater 0, create a new object dictionary entry*/
                    pModuleObjEntry->pPrev = NULL;
                    pModuleObjEntry->pNext = NULL;

                    if ((pModuleObjEntry->Index >= 0x1600 && pModuleObjEntry->Index <= 0x17FF)
                        || (pModuleObjEntry->Index >= 0x1A00 && pModuleObjEntry->Index <= 0x1BFF))
                    {
                        pModuleObjEntry->Index = pModuleObjEntry->Index + (u16Counter * MODULE_PDO_OBJECT_INCREMENT);
                    }
                    else if (pModuleObjEntry->Index >= 0x2000 && pModuleObjEntry->Index <= 0xBFFF)
                    {
                        pModuleObjEntry->Index = pModuleObjEntry->Index + (u16Counter * MODULE_OBJECT_INCREMENT);
                    }


                    /*allocate object data memory*/
                    if (pModuleObjEntry->pVarPtr != NULL)
                    {
                        void MBXMEM* pData = pModuleObjEntry->pVarPtr;
                        /*Get size of required object memory (Implicit compiler alignment is taken into account if OBJ_DWORD_ALIGN/OBJ_WORD_ALIGN are set)*/
                        UINT8 maxSubindex = (pModuleObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
                        OBJCONST TSDOINFOENTRYDESC OBJMEM* pEntry; 
                        UINT32 u32objsize = OBJ_GetEntryOffset(maxSubindex, pModuleObjEntry);
                        pEntry = OBJ_GetEntryDesc(pModuleObjEntry, maxSubindex);
                        u32objsize = BIT2BYTE((u32objsize + pEntry->BitLength)); 

                        pModuleObjEntry->pVarPtr = ALLOCMEM(u32objsize);

                        if (pModuleObjEntry->pVarPtr == NULL)
                        {
                            return 1;
                        }

                        /*copy initial value to new object*/
                        MEMCPY(pModuleObjEntry->pVarPtr, pData, u32objsize);

                        if ((pModuleObjEntry->Index >= 0x1600 && pModuleObjEntry->Index <= 0x17FF)
                            || (pModuleObjEntry->Index >= 0x1A00 && pModuleObjEntry->Index <= 0x1BFF))
                        {
                            UINT16 maxConfiguredSubindex = (pModuleObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT; //required to check if value for Subindex0 is valid
                            UINT32* pData = (UINT32*)(((UINT16*)pModuleObjEntry->pVarPtr) + (OBJ_GetEntryOffset(1, pModuleObjEntry) >> 4));/*Get PDO entry*/
                            UINT16 u16Idx;

                            while (maxConfiguredSubindex > 0)
                            {
                                u16Idx = (UINT16)((*pData) >> 16);

                                if (u16Idx >= 0x2000 && u16Idx <= 0xBFFF)
                                {
                                    u16Idx += (u16Counter * MODULE_OBJECT_INCREMENT);
                                    *pData = (*pData & 0x0000FFFF) | (((UINT32)u16Idx) << 16);
                                }
                                pData++;
                                maxConfiguredSubindex--;
                            }
                        }
                    }

                    if (pModuleObjEntry->Index <= 0xFFF)
                    {
                        /*skip data type/unit or enum definitions if they are already defined in the OD*/
                        OBJCONST TOBJECT OBJMEM *pObj = OBJ_GetObjectHandle(pModuleObjEntry->Index);
                        if (pObj == NULL)
                        {
                            result = COE_AddObjectToDic(pModuleObjEntry);

                            if (result != 0)
                            {
                                return result;
                            }
                        }
                    }
                    else
                    {
                        result = COE_AddObjectToDic(pModuleObjEntry);

                        if (result != 0)
                        {
                            return result;
                        }
                    }

                    if (result != 0)
                    {
                        return 1;
                    }

                    pModuleObjEntry++;
                } 
            }
        }
        else if(AssignedModules[u16Counter].pObjectDictionary != NULL)
        {
            /*verify that the enum definitions are available*/
            OBJCONST TOBJECT OBJMEM* pObj;
            pModuleObjEntry = AssignedModules[u16Counter].pObjectDictionary;
            /*loop over all module object dictionary entries and add to device object dictionary*/
            while ((pModuleObjEntry->Index != 0xFFFF) && (pModuleObjEntry->Index <= 0xFFF))
            {
                pObj = OBJ_GetObjectHandle(pModuleObjEntry->Index);
                if (pObj == NULL)
                {
                    result = COE_AddObjectToDic(pModuleObjEntry);

                    if (result != 0)
                    {
                        return result;
                    }
                }
                pModuleObjEntry++;
            }
        }
    }

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
             all general settings were checked to start the mailbox handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case this function will be called cyclically
            until a value unequal NOERROR_INWORK is returned

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                       register (0x204) when this function is succeeded. The event mask can be adapted
                       in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
           all general settings were checked to start the input handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

#if COE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, invalid mapping error

 \brief    Assign all PDO mapping to the related SM assign objects

*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_UpdateAssignObjects()
{
        LOG_I("%s\r\n",__func__);
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM* pAssignObj = OBJ_GetObjectHandle(0x1C12);
    UINT16 maxConfiguredSubindex = (pAssignObj->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
    OBJCONST TOBJECT OBJMEM* pObjEntry = COE_GetObjectDictionary();

    /*Process 0x1C12*/
    do
    {
        if (pObjEntry->Index >= 0x1600 && pObjEntry->Index <= 0x17FF)
        {
            if (PDOAssignEntryCnt >= maxConfiguredSubindex)
            {
                return ALSTATUSCODE_INVALIDOUTPUTMAPPING;;
            }
            sRxPDOassign.aEntries[PDOAssignEntryCnt] = pObjEntry->Index;
            PDOAssignEntryCnt++;
        }

        /*abort loop if max index is reached (objects are sorted in ascending order)*/
        if (pObjEntry->Index >= 0x17FF)
        {
            break;
        }

        pObjEntry = pObjEntry->pNext;
    } while (pObjEntry != NULL);

    sRxPDOassign.u16SubIndex0 = PDOAssignEntryCnt;
    
    /*clear remaining entries*/
    for (; PDOAssignEntryCnt < maxConfiguredSubindex; PDOAssignEntryCnt++)
    {
        sRxPDOassign.aEntries[PDOAssignEntryCnt] = 0;
    }


    /*Process 0x1C13*/
    pAssignObj = OBJ_GetObjectHandle(0x1C13);
    maxConfiguredSubindex = (pAssignObj->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
    PDOAssignEntryCnt = 0;

    while (pObjEntry != NULL)
    {
        if (pObjEntry->Index >= 0x1A00 && pObjEntry->Index <= 0x1BFF)
        {
            if (PDOAssignEntryCnt >= maxConfiguredSubindex)
            {
                return ALSTATUSCODE_INVALIDINPUTMAPPING;;
            }
            sTxPDOassign.aEntries[PDOAssignEntryCnt] = pObjEntry->Index;
            PDOAssignEntryCnt++;
        }

        /*abort loop if max index is reached (objects are sorted in ascending order)*/
        if (pObjEntry->Index >= 0x1BFF)
        {
            break;
        }

        pObjEntry = pObjEntry->pNext;
    } 

    sTxPDOassign.u16SubIndex0 = PDOAssignEntryCnt;

    /*clear remaining entries*/
    for (; PDOAssignEntryCnt < maxConfiguredSubindex; PDOAssignEntryCnt++)
    {
        sTxPDOassign.aEntries[PDOAssignEntryCnt] = 0;
    }

    return 0;
}
#endif // #if COE_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize)
{
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

#if COE_SUPPORTED
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM * pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
   
	result = Module_UpdateObjectDictionary();
    if (result != ALSTATUSCODE_NOERROR)
    {
        return ALSTATUSCODE_UNSPECIFIEDERROR;
    }

    result = APPL_UpdateAssignObjects();
    if (result != ALSTATUSCODE_NOERROR)
    {
        return result;
    }

#if MAX_PD_OUTPUT_SIZE > 0
    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
        if(pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
            for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
                pPDOEntry = (UINT32 *)(((UINT16 *)pPDO->pVarPtr) + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>4));    //goto PDO entry
                // we increment the expected output size depending on the mapped Entry
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;
#endif

#if MAX_PD_INPUT_SIZE > 0
    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if(pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
                for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
                    pPDOEntry = (UINT32 *)(((UINT16 *)pPDO->pVarPtr) + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>4));    //goto PDO entry
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;
#endif

#else
#if _WIN32
   #pragma message ("Warning: Define 'InputSize' and 'OutputSize'.")
#else
    #warning "Define 'InputSize' and 'OutputSize'."
#endif
#endif

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;
}

volatile void *TxPdoStd = NULL;
volatile uint8_t TxPdoStdNum = 0;
volatile void *TxPdoExt = NULL;
volatile uint8_t TxPdoExtNum = 0;
volatile void *RxPdoStd = NULL;
volatile uint8_t RxPdoStdNum = 0;
volatile void *RxPdoExt = NULL;
volatile uint8_t RxPdoExtNum = 0;
uint8_t FlagsTrigger = false;
uint8_t FlagsCycle = false;

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     index               index of the requested object.
\param     subindex            subindex of the requested object.
\param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
\param     pData               Pointer to the buffer where the data can be copied to
\param     bCompleteAccess     Indicates if a complete read of all subindices of the
                               object shall be done or not

\return    result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))
 *////////////////////////////////////////////////////////////////////////////////////////
UINT8 ConfiguredModuleIdentListWrite0xF030(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess)
{
    LOG_I("ConfiguredModuleIdentListWrite0xF030 index:0x%x, subindex:0x%x, dataSize:0x%x, bCompleteAccess:0x%x\r\n", index, subindex, dataSize, bCompleteAccess);
    for (size_t j = 0; j < dataSize; j++)
    {
        LOG_I("pData[%d]:0x%x\r\n", j, ((UINT8*)pData)[j]);

    }
    

    UINT16 i = subindex;
    UINT16 maxSubindex = ConfiguredModuleIdentList0xF030.u16SubIndex0;
    OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry;
    /* lastSubindex is used for complete access to make loop over the requested entries
       to be read, we initialize this variable with the requested subindex that only
       one loop will be done for a single access */
    UINT8 lastSubindex = subindex;

    if ( bCompleteAccess ) {
        if ( subindex == 0 )
        {
            /* we change the subindex 0 */
            maxSubindex = (UINT8) pData[0];
        }

        /* we write until the maximum subindex */
        lastSubindex = (UINT8)maxSubindex;
    } else {
        if (subindex > maxSubindex)
        {
            /* the maximum subindex is reached */
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        }
        else
        {
            /* we use the standard write function */
            for (i = subindex; i <= lastSubindex; i++)
            {
                /* we have to copy the entry */
                if (i == 0)
                {

                    ConfiguredModuleIdentList0xF030.u16SubIndex0 =  pData[0];

                    /* we increment the destination pointer by 2 because the subindex 0 will be
                        transmitted as UINT16 for a complete access */
                    pData++;
                }
                else
                {
                    UINT32 CurValue = ConfiguredModuleIdentList0xF030.aEntries[(i-1)];
                    UINT16 MBXMEM *pVarPtr = (UINT16 MBXMEM *) &ConfiguredModuleIdentList0xF030.aEntries[(i-1)];

                    pVarPtr[0] = pData[0];
                    pVarPtr[1] = pData[1];
                    pData += 2;

                    /*Check if valid value was written*/
                    if((ConfiguredModuleIdentList0xF030.aEntries[(i-1)] != (UINT32)MODULEID_CAN)
                    && (ConfiguredModuleIdentList0xF030.aEntries[(i-1)] != (UINT32)MODULEID_CANFD)
                    && (ConfiguredModuleIdentList0xF030.aEntries[(i-1)] != 0))
                    {
                        /*write previous value*/
                        ConfiguredModuleIdentList0xF030.aEntries[(i-1)] = CurValue;

                        /*reset subindex 0 (if required)*/
                        if(ConfiguredModuleIdentList0xF030.aEntries[(i-1)] != 0)
                        {
                            ConfiguredModuleIdentList0xF030.u16SubIndex0 = i;
                        }
                        else
                        {
                            /*current entry is 0 => set subindex0 value i-1*/
                            ConfiguredModuleIdentList0xF030.u16SubIndex0 = (i-1);
                        }
                        return ABORTIDX_VALUE_EXCEEDED;
                    }
                }
            }
        }
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     index               index of the requested object.
\param     subindex            subindex of the requested object.
\param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
\param     pData               Pointer to the buffer where the data can be copied to
\param     bCompleteAccess     Indicates if a complete read of all subindices of the
                               object shall be done or not

\return    result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))
 *////////////////////////////////////////////////////////////////////////////////////////
UINT8 ParameterSetWrite0xF800(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess)
{
    LOG_I("ParameterSetWrite0xF800 index:0x%x, subindex:0x%x, dataSize:0x%x, bCompleteAccess:0x%x\r\n", index, subindex, dataSize, bCompleteAccess);
    for (size_t j = 0; j < dataSize; j++)
    {
        LOG_I("pData[%d]:0x%x\r\n", j, ((UINT8*)pData)[j]);

    }
    
    UINT16 i = subindex;
    UINT16 maxSubindex = ParameterSet0xF800.u16SubIndex0;
    OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry;
    /* lastSubindex is used for complete access to make loop over the requested entries
       to be read, we initialize this variable with the requested subindex that only
       one loop will be done for a single access */
    UINT8 lastSubindex = subindex;

    if ( bCompleteAccess ) {
        if ( subindex == 0 )
        {
            /* we change the subindex 0 */
            maxSubindex = (UINT8) pData[0];
        }

        /* we write until the maximum subindex */
        lastSubindex = (UINT8)maxSubindex;
    } else {
        if (subindex > maxSubindex) {
            /* the maximum subindex is reached */
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        } else if(subindex == 2) {
            gw_mq_msg mq_msg;
            mq_msg.msg_id = GW_MQ_MSG_CAN_CONFIG_ID;
            ParameterSet0xF800.Baudrate = ((UINT8*) pData)[0];
            switch (ParameterSet0xF800.Baudrate)
            {
                case 0: //1M
                mq_msg.param = 1000000;
                break;
                case 1: //800k
                mq_msg.param = 800000;
                break;
                case 2: //500k
                mq_msg.param = 500000;
                break;
                case 3: //250k
                mq_msg.param = 250000;
                break;
                case 4: //125k
                mq_msg.param = 125000;
                break;
                case 5: //100k
                mq_msg.param = 100000;
                break;
                case 6: //50k
                mq_msg.param = 50000;
                break;
                case 7: //20k
                mq_msg.param = 20000;
                break;
                case 8: //10k
                mq_msg.param = 10000;
                break;
                case 255: //defined in bustiming register
                mq_msg.param = 255;
                break;
                default:
                mq_msg.param = 1000000;
                break;
            }
            gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 1);
        }
    }
    return 0;   
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param     index               index of the requested object.
\param     subindex            subindex of the requested object.
\param     objSize             size of the requested object data, calculated with OBJ_GetObjectLength
\param     pData               Pointer to the buffer where the data can be copied to
\param     bCompleteAccess     Indicates if a complete read of all subindices of the
                               object shall be done or not

 \return    result of the read operation (0 (success) or an abort code (ABORTIDX_.... defined in
            sdosrv.h))
 *////////////////////////////////////////////////////////////////////////////////////////
UINT8 Module100_ConfigInterfaceWrite0x8000(UINT16 index, UINT8 subindex, UINT32 dataSize, UINT16 MBXMEM * pData, UINT8 bCompleteAccess)
{
    LOG_I("Module100_ConfigInterfaceWrite0x8000 index:0x%x, subindex:0x%x, dataSize:0x%x, bCompleteAccess:0x%x\r\n", index, subindex, dataSize, bCompleteAccess);
    for (size_t j = 0; j < dataSize; j++)
    {
        LOG_I("pData[%d]:0x%x\r\n", j, ((UINT8*)pData)[j]);

    }
    UINT16 i = subindex;
    OBJCONST TOBJECT OBJMEM *pObj = OBJ_GetObjectHandle(index);
    UINT16 maxSubindex = ((TMODULE100OBJ8000*)pObj->pVarPtr)->u16SubIndex0;
    OBJCONST TSDOINFOENTRYDESC OBJMEM *pEntry;
    /* lastSubindex is used for complete access to make loop over the requested entries
       to be read, we initialize this variable with the requested subindex that only
       one loop will be done for a single access */
    UINT8 lastSubindex = subindex;

    if ( bCompleteAccess ) {
        if ( subindex == 0 )
        {
            /* we change the subindex 0 */
            maxSubindex = (UINT8) pData[0];
        }

        /* we write until the maximum subindex */
        lastSubindex = (UINT8)maxSubindex;
    } else {
        if (subindex > maxSubindex) {
            /* the maximum subindex is reached */
            return ABORTIDX_SUBINDEX_NOT_EXISTING;
        } else if(subindex == 32) {
            ((TMODULE100OBJ8000*)pObj->pVarPtr)->Flags = pData[0];
            if(((TMODULE100OBJ8000*)pObj->pVarPtr)->Flags & 1<<2) { // 触发一次
                FlagsTrigger = true;
            } else {
                FlagsTrigger = false;
            }
            if(!(((TMODULE100OBJ8000*)pObj->pVarPtr)->Flags & 1<<4) && ((TMODULE100OBJ8000*)pObj->pVarPtr)->CycleTimeMs != 0) { // 周期时间
                FlagsCycle = true;
            } else {
                FlagsCycle = false;
                gw_mq_msg mq_msg;
                mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_STOP_CYC_ID;
                gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);
            }
        } else if(subindex == 37) {
            ((TMODULE100OBJ8000*)pObj->pVarPtr)->CycleTimeMs = *((UINT32*)pData);
            if(!(((TMODULE100OBJ8000*)pObj->pVarPtr)->Flags & 1<<4) && ((TMODULE100OBJ8000*)pObj->pVarPtr)->CycleTimeMs != 0) { // 周期时间
                FlagsCycle = true;
            } else {
                FlagsCycle = false;
                gw_mq_msg mq_msg;
                mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_STOP_CYC_ID;
                gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);
            }
        }
    }
    return 0; 
}

void *gateway_apps_get_txpdo_std(uint8_t *num)
{
    if(TxPdoStdNum == 0 || TxPdoStd == NULL) {
        *num = 0;
        return NULL;
    }
    *num = TxPdoStdNum;
    return TxPdoStd;
}

void *gateway_apps_get_txpdo_ext(uint8_t *num)
{
    if(TxPdoExtNum == 0 || TxPdoExt == NULL) {
        *num = 0;
        return NULL;
    }
    *num = TxPdoExtNum;
    return TxPdoExt;
}

void *gateway_apps_get_rxpdo_std(uint8_t *num)
{
    if(RxPdoStdNum == 0 || RxPdoStd == NULL) {
        *num = 0;
        return NULL;
    }
    *num = RxPdoStdNum;
    return RxPdoStd;
}

void *gateway_apps_get_rxpdo_ext(uint8_t *num)
{
    if(RxPdoExtNum == 0 || RxPdoExt == NULL) {
        *num = 0;
        return NULL;
    }
    *num = RxPdoExtNum;
    return RxPdoExt;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data

\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_InputMapping(UINT16* pData)
{
    UINT32 u32Offset = 0;
    UINT16 u16PdoIndex = 0;
    UINT16 u16LastPdoIndex = 0;

    // 填写数据
    for (UINT16 u16Counter = 0; u16Counter < ConfiguredModuleIdentList0xF030.u16SubIndex0; u16Counter++)
    {
        TOBJECT OBJMEM* pModuleObjEntry = AssignedModules[u16Counter].pObjectDictionary;
        switch (AssignedModules[u16Counter].u32ModuleId)
        {
            case MODULEID_CAN:
                do {
                    if (pModuleObjEntry->Index >= 0x1A00 && pModuleObjEntry->Index <= 0x1BFF) {
                        UINT16 maxConfiguredSubindex = (pModuleObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
                        for(UINT16 u16Subindex = 1; u16Subindex <= maxConfiguredSubindex; u16Subindex++) 
                        {
                            u16PdoIndex = ((UINT32*)((UINT8 *)pModuleObjEntry->pVarPtr + 2))[u16Subindex-1] >> 16;
                            if(u16PdoIndex != u16LastPdoIndex) {
                                OBJCONST TOBJECT OBJMEM *pObj = OBJ_GetObjectHandle(u16PdoIndex);  
                                UINT8 maxPdoSubindex = (pObj->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
                                UINT32 u32ObjSize = (OBJ_GetEntryOffset(maxPdoSubindex+1, pObj) - 16);
                                MEMCPY((UINT8*)pData + u32Offset, (UINT8*)pObj->pVarPtr + 2, BIT2BYTE(u32ObjSize));  
                                u32Offset += BIT2BYTE(u32ObjSize);
                                OBJCONST TOBJECT OBJMEM *pObj0x8000 = OBJ_GetObjectHandle(0x8000 + u16Counter * MODULE_PDO_OBJECT_INCREMENT);  
                                if(!((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<3)) && u16PdoIndex == (0x6000 + u16Counter * MODULE_PDO_OBJECT_INCREMENT)) { //0x8000.32.bit3判断CAN帧类型 0:标准帧 1:扩展帧
                                    // 标准帧
                                    TxPdoStdNum = (BIT2BYTE(u32ObjSize)-8)/10;
                                    TxPdoStd = (void *)pObj->pVarPtr;
                                } else if(((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<3)) && u16PdoIndex == (0x6001 + u16Counter * MODULE_PDO_OBJECT_INCREMENT)) {
                                    // 扩展帧
                                    TxPdoExtNum = (BIT2BYTE(u32ObjSize)-8)/14;
                                    TxPdoExt = (void *)pObj->pVarPtr;                                
                                } else {

                                }
                            }
                            u16LastPdoIndex = u16PdoIndex;
                        }
                        break;
                    }
                    pModuleObjEntry = pModuleObjEntry->pNext;
                } while(pModuleObjEntry != NULL);
            break;
            default:
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_OutputMapping(UINT16* pData)
{
    UINT32 u32Offset = 0;
    UINT16 u16PdoIndex = 0;
    UINT16 u16LastPdoIndex = 0;

    // 填写数据
    for (UINT16 u16Counter = 0; u16Counter < ConfiguredModuleIdentList0xF030.u16SubIndex0; u16Counter++)
    {
        TOBJECT OBJMEM* pModuleObjEntry = AssignedModules[u16Counter].pObjectDictionary;
        switch (AssignedModules[u16Counter].u32ModuleId)
        {
            case MODULEID_CAN:
                do {
                    if (pModuleObjEntry->Index >= 0x1600 && pModuleObjEntry->Index <= 0x17FF) {
                        UINT16 maxConfiguredSubindex = (pModuleObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
                        for(UINT16 u16Subindex = 1; u16Subindex <= maxConfiguredSubindex; u16Subindex++) 
                        {
                            u16PdoIndex = ((UINT32*)((UINT8 *)pModuleObjEntry->pVarPtr + 2))[u16Subindex-1] >> 16;
                            if(u16PdoIndex != u16LastPdoIndex) {
                                OBJCONST TOBJECT OBJMEM *pObj = OBJ_GetObjectHandle(u16PdoIndex);  
                                UINT8 maxPdoSubindex = (pObj->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;
                                UINT32 u32ObjSize = (OBJ_GetEntryOffset(maxPdoSubindex+1, pObj) - 16);
                                MEMCPY((UINT8*)pObj->pVarPtr + 2, (UINT8*)pData + u32Offset, BIT2BYTE(u32ObjSize));
                                // static UINT32 mm=0;
                                // if(mm++>=10000) {
                                //     mm=0;
                                //     for(int i =0 ;i<40; i++) {
                                //         printf("%x ", ((UINT8*)pData)[i]);
                                //     }
                                //     printf(" index %x\r\n",u16PdoIndex); 
                                // }
                                u32Offset += BIT2BYTE(u32ObjSize);
                                OBJCONST TOBJECT OBJMEM *pObj0x8000 = OBJ_GetObjectHandle(0x8000 + u16Counter * MODULE_PDO_OBJECT_INCREMENT);  
                                if(!((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<3)) && u16PdoIndex == (0x7000 + u16Counter * MODULE_PDO_OBJECT_INCREMENT)) { //0x8000.32.bit3判断CAN帧类型 0:标准帧 1:扩展帧
                                    // 标准帧
                                    if((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<4)) { //bit4 triger mode bit2 trigger once
                                        if((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<2)) { //bit2 trigger once
                                            if(FlagsTrigger) {
                                                FlagsTrigger = false;
                                                RxPdoStd = (void *)pObj->pVarPtr;
                                                RxPdoStdNum = (BIT2BYTE(u32ObjSize)-6)/12;
                                                gw_mq_msg mq_msg;
                                                mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                                                mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_STD_TRIG_ID;
                                                gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);
                                            }
                                        }
                                    } else {
                                        if(FlagsCycle) {                
                                            FlagsCycle = false;        
                                            RxPdoStd = (void *)pObj->pVarPtr;
                                            RxPdoStdNum = (BIT2BYTE(u32ObjSize)-6)/12;
                                            gw_mq_msg mq_msg;
                                            mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                                            mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_STD_CYC_ID;
                                            mq_msg.param = ((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->CycleTimeMs;
                                            gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);   
                                        } 
                                    }
                                } else if(((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<3)) && u16PdoIndex == (0x7001 + u16Counter * MODULE_PDO_OBJECT_INCREMENT)) {
                                    // 扩展帧
                                    if((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<4)) { //bit4 triger mode bit2 trigger once
                                        if((((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->Flags)&(1<<2)) {
                                            if(FlagsTrigger) {
                                                FlagsTrigger = false;
                                                RxPdoExt = (void *)pObj->pVarPtr;
                                                RxPdoExtNum = (BIT2BYTE(u32ObjSize)-6)/16;
                                                gw_mq_msg mq_msg;
                                                mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                                                mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_EXT_TRIG_ID;
                                                gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);
                                            }
                                        }
                                    } else {
                                        if(FlagsCycle) {
                                            FlagsCycle = false;
                                            RxPdoExt = (void *)pObj->pVarPtr;
                                            RxPdoExtNum = (BIT2BYTE(u32ObjSize)-6)/16;
                                            gw_mq_msg mq_msg;
                                            mq_msg.msg_id = GW_MQ_MSG_CAN_TX_ID;
                                            mq_msg.submsg_id = GW_MQ_SUBMSG_CAN_TX_EXT_CYC_ID;
                                            mq_msg.param = ((TMODULE100OBJ8000*)pObj0x8000->pVarPtr)->CycleTimeMs;
                                            gw_mq_send(GW_MQ_CAN_ID, &mq_msg, 0);  
                                        }
                                    }
                                } else {

                                }
                            }
                            u16LastPdoIndex = u16PdoIndex;
                        }
                        break;
                    }
                    pModuleObjEntry = pModuleObjEntry->pNext;
                } while(pModuleObjEntry != NULL);
            break;
            default:
            break;
        }
    }


#if _WIN32
   #pragma message ("Warning: Implement output (Master->Slave) mapping")
#else
    #warning "Implement output (Master->Slave) mapping"
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_Application(void)
{
#if _WIN32
   #pragma message ("Warning: Implement the slave application")
#else
    #warning "Implement the slave application"
#endif
}

#if EXPLICIT_DEVICE_ID
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Read the Explicit Device ID (from an external ID switch)
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
#if _WIN32
   #pragma message ("Warning: Implement explicit Device ID latching")
#else
    #warning "Implement explicit Device ID latching"
#endif
    /* Explicit Device 5 is expected by Explicit Device ID conformance tests*/
    return 0x5;
}
#endif

#if USE_DEFAULT_MAIN
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This is the main function

*////////////////////////////////////////////////////////////////////////////////////////
#if _PIC24 && EL9800_HW
int main(void)
#elif _WIN32
int main(int argc, char* argv[])
#else
void main(void)
#endif
{
    /* initialize the Hardware and the EtherCAT Slave Controller */
#if FC1100_HW
#if _WIN32
    u16FcInstance = 0;
    if (argc > 1)
    {
        u16FcInstance = atoi(argv[1]);
    }
#endif
    if(HW_Init())
    {
        HW_Release();
        return;
    }
#else
    HW_Init();
#endif
    MainInit();

    Module_UpdateObjectDictionary();

    bRunApplication = TRUE;
    do
    {
        MainLoop();
        
    } while (bRunApplication == TRUE);

    HW_Release();
#if _PIC24
    return 0;
#endif
}
#endif //#if USE_DEFAULT_MAIN
/** @} */

