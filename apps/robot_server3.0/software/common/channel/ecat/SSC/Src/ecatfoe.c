/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
* https://www.beckhoff.com/media/downloads/slave-stack-code/ethercat_ssc_license.pdf
*/

/**
\addtogroup FoE FileTransfer over EtherCAT
@{
*/

/**
\file ecatfoe.c
\author EthercatSSC@beckhoff.com
\brief Implementation
This file contains the FoE mailbox interface

\version 5.13

<br>Changes to version V5.12:<br>
V5.13 EOE1: <br>
V5.13 FOE1: handle failure on continue indication<br>
<br>Changes to version V5.11:<br>
V5.12 FOE2: handle 16bit only access<br>
V5.12 MBX4: in case of a disable mailbox queue and two consecutive foe uploads the mailbox receive handler is blocked<br>
<br>Changes to version V5.10:<br>
V5.11 FOE1: handle busy on a read request, change OPMode to OPCode (same terms as in the spec)<br>
<br>Changes to version V5.0:<br>
V5.10 MBX1: Remove multiple swapping of length filed in mailbox header<br>
<br>Changes to version V4.40:<br>
V5.0 FOE1: Free memory if FoE_ServciceInd() is left.<br>
V5.0 FOE2: Update error code (Bit 15 shall not be set).<br>
V5.0 FOE3: Update byte swapping for big endian and big endian 16 configurations.<br>
V5.0 FOE4: Update Error text handling for 16Bit operations.<br>
<br>Changes to version V4.11:<br>
V4.40 FOE1: correct FoE busy values<br>
V4.40 MBX2: HW_CopyToSendMailbox merge to a generic function (new: MBX_CopyToSendMailbox)<br>
<br>Changes to version V4.10:<br>
V4.11 FOE 1 fixed a problem with a huge FOE_MADATA define<br>
V4.11 FOE 2 u16PacketNo is now a 32bit variable (u32PacketNo) to conform with the spec<br>
<br>Changes to version V4.00:<br>
V4.10 FOE 1-3 when downloading the firmware the FoE state machine was not set to FOE_READY<br>
              when the last data segment was downloaded so that the next FoE service failed<br>
<br>Changes to version V3.20:<br>
V4.00 FOE 1 - if a FoE Error service is received it should not be acknowledged<br>
V4.00 FOE 2 - if a FoE service response could not be sent because the mailbox is full<br>
              it has to be re-transmit otherwise the response get lost<br>

*/

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/

#include "ecat_def.h"



#include "ecatslv.h"

#define    _ECATFOE_ 1
#include "ecatfoe.h"
#undef    _ECATFOE_
#define    _ECATFOE_ 0

#include "foeappl.h"
/*-----------------------------------------------------------------------------------------
------
------    functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This function initialize the FoE Interface.
*////////////////////////////////////////////////////////////////////////////////////////

void FOE_Init(void)
{
    /* no file transmission sequence is running */
    u16FileAccessState    = FOE_READY;
    /* initialize the expected packet number */
    u32PacketNo = 0;
    pFoeSendStored = NULL;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pFoeInd      Pointer to the received mailbox data from the master.

 \return    result of the operation (0 (success) or mailbox error code (MBXERR_.... defined in
            mailbox.h))

 \brief    This function is called when a FoE (File Access over EtherCAT) service is received from
             the master.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 FOE_ServiceInd(TFOEMBX MBXMEM * pFoeInd)
{
    /* initialize the result of the service checking */
    UINT16 nextState = ECAT_FOE_ERRCODE_ILLEGAL;
    /* dataSize contains the size of the file data */
    UINT16 dataSize = SWAPWORD(pFoeInd->MbxHeader.Length) - FOE_HEADER_SIZE;


    /* it has to be checked if the mailbox protocol is correct, the sent mailbox data length has to
       great enough for the service header of the FoE service */
    if ( SWAPWORD(pFoeInd->MbxHeader.Length) < FOE_HEADER_SIZE )
    {
        return MBXERR_SIZETOOSHORT;
    }

    switch ( SWAPWORD(pFoeInd->FoeHeader.OpCode) )
    {
    case ECAT_FOE_OPCODE_RRQ:
        /* file read is requested */
        if ( u16FileAccessState == FOE_READY )
        {
            UINT32 u32Password = SWAPDWORD((((UINT32)pFoeInd->FoeHeader.Cmd.Password[FOE_COMMAND_LOWWORD]) | ((UINT32)pFoeInd->FoeHeader.Cmd.Password[FOE_COMMAND_HIGHWORD]) << 16));
            /* last FoE sequence was finished, call application function */
            nextState = FOE_Read((UINT16 MBXMEM *)pFoeInd->Data, dataSize, (UINT16*)pFoeInd->Data, u32Password);

            /* u32LastFileOffset contains the offset of the file which is sent now */
            u32LastFileOffset = 0;

            if (nextState <= FOE_MAXDATA)
            {
                /* the first data packet shall be sent */
                u32PacketNo    = 1;
                /* u32FileOffset contains the offset of the file which shall be sent when the next FoE ACK is received */
                u32FileOffset = nextState;
            }
            else if (nextState <= FOE_MAXBUSY)
            {
                u32PacketNo    = 0;
                u32FileOffset = 0;
                u16FileAccessState = FOE_WAIT_FOR_ACK;
            }
        }
        break;

    case ECAT_FOE_OPCODE_WRQ:
        /* file write is requested */
        if ( u16FileAccessState == FOE_READY )
        {
            UINT32 u32Password = SWAPDWORD((((UINT32)pFoeInd->FoeHeader.Cmd.Password[FOE_COMMAND_LOWWORD]) | ((UINT32)pFoeInd->FoeHeader.Cmd.Password[FOE_COMMAND_HIGHWORD]) << 16));
            /* last FoE sequence was finished, call application function */
            nextState = FOE_Write((UINT16 MBXMEM *)pFoeInd->Data, dataSize, u32Password);
            if ( nextState == 0 )
            {
                /* checking was successful, sent a FoE Ack service */
                nextState = FOE_ACK;
            }
            /* initialize the packet number */
            u32PacketNo    = 0;
        }
        break;

    case ECAT_FOE_OPCODE_DATA:
        /* file data is received */
        if ( u16FileAccessState == FOE_WAIT_FOR_DATA
          || u16FileAccessState == FOE_WAIT_FOR_LAST_DATA )
        {
            UINT32 u32CmdPacketNo = SWAPDWORD((((UINT32)pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_LOWWORD]) | ((UINT32)pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_HIGHWORD]) << 16));

            /* we are waiting for file data, service is correct */
            if ( u32CmdPacketNo == u32PacketNo )
            {
                /* the packet number is correct, call application function to store the file data */
                nextState = FOE_Data((UINT16 MBXMEM *)pFoeInd->Data, dataSize);
                if ( nextState == 0 )
                {
                    /* checking was successful, sent a FoE Ack service */
                    nextState = FOE_ACK;
                }
            }
            else
            {
                nextState = ECAT_FOE_ERRCODE_PACKENO;
            }
        }
        break;

    case ECAT_FOE_OPCODE_ACK:
        /* acknowledge is received, next file part can be sent */
        if ( u16FileAccessState == FOE_WAIT_FOR_ACK )
        {
            /* we are waiting for an acknowledge, service is correct, call the application function
               to get the next part of the file */
            nextState = FOE_Ack( u32FileOffset, (UINT16 MBXMEM *)pFoeInd->Data );
            /* u32LastFileOffset contains the offset of the file which is sent now */
            u32LastFileOffset = u32FileOffset;

            if (nextState <= FOE_MAXDATA) 
            {
                /* u32FileOffset contains the offset of the file which shall be sent when the next FoE ACK is received */
                u32FileOffset += nextState;
                /* increment the packet number */
                u32PacketNo++;
            }
        }
        else if ( u16FileAccessState == FOE_WAIT_FOR_LAST_ACK )
        {
            /* we were waiting for the last acknowledge, now the sequence is finished */
            nextState = FOE_FINISHED_NOACK;
        }
        break;

    case ECAT_FOE_OPCODE_ERR:
        /* a FoE Error service is received */
        if ( u16FileAccessState != FOE_READY )
        {
            UINT32 u32CmdErrorCode = SWAPDWORD((((UINT32)pFoeInd->FoeHeader.Cmd.ErrorCode[FOE_COMMAND_LOWWORD]) | ((UINT32)pFoeInd->FoeHeader.Cmd.ErrorCode[FOE_COMMAND_HIGHWORD]) << 16));

            /* a file transmission sequence is active, inform the application, that this sequence
               was stopped */
            FOE_Error( u32CmdErrorCode );

            nextState = FOE_FINISHED;
        }
        break;

    case ECAT_FOE_OPCODE_BUSY:
        /* a FoE Busy service is received */
        if ( u16FileAccessState == FOE_WAIT_FOR_ACK
          || u16FileAccessState == FOE_WAIT_FOR_LAST_ACK )
        {
            /* we are waiting for an acknowledge, service is correct, call the application function
               to resend the last part of the file */
            nextState = FOE_Busy( SWAPWORD(pFoeInd->FoeHeader.Cmd.Busy.Done), u32LastFileOffset, (UINT16 MBXMEM *)pFoeInd->Data );
        }
        break;

    }

    if ( nextState <= FOE_MAXDATA )
    {
        /* we send DATA and wait for ACK */
        UINT32 d = SWAPDWORD(u32PacketNo);

        /* store the OpCode in the mailbox buffer */
        pFoeInd->FoeHeader.OpCode           = SWAPWORD(ECAT_FOE_OPCODE_DATA);
        /* store the packet number in the mailbox buffer */
        pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_LOWWORD] = ((d & 0x0000FFFF));
        pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_HIGHWORD] = ((d & 0xFFFF0000) >> 16);
     
        /* store the size of the mailbox data in the mailbox buffer */
        pFoeInd->MbxHeader.Length           = FOE_HEADER_SIZE + nextState;

        if ( nextState == u16SendMbxSize - FOE_HEADER_SIZE - MBX_HEADER_SIZE )
        {
            /* packets still following, we wait for an ACK */
            u16FileAccessState = FOE_WAIT_FOR_ACK;
        }
        else
        {
            /* it was the last Packet, we wait for the last ACK */
            u16FileAccessState = FOE_WAIT_FOR_LAST_ACK;
        }
    }
    else if ( nextState <= FOE_MAXBUSY )
    {
        /* we are still storing the received file data (in flash for example) and
           send BUSY and wait for the DATA to be sent again */
        /* store the OpCode in the mailbox buffer */
        pFoeInd->FoeHeader.OpCode                   = SWAPWORD(ECAT_FOE_OPCODE_BUSY);
        /* store the information how much progress we made until we can receive file data again */
        pFoeInd->FoeHeader.Cmd.Busy.Done            = SWAPWORD(nextState-FOE_MAXBUSY_ZERO);

        pFoeInd->FoeHeader.Cmd.Busy.Entire          = 0;
        /* store the size of the mailbox data in the mailbox buffer */

        pFoeInd->MbxHeader.Length                   = FOE_HEADER_SIZE;
    }
    else if ( nextState == FOE_ACK || nextState == FOE_ACKFINISHED )
    {
        /* we send ACK and wait for DATA
            the next file data is expected with an incremented packet number, but
            we have to acknowledge the old packet first */
        UINT32 d = SWAPDWORD(u32PacketNo);
        u32PacketNo++;
        
        /* store the OpCode in the mailbox buffer */
        pFoeInd->FoeHeader.OpCode                   = SWAPWORD(ECAT_FOE_OPCODE_ACK);
        /* store the packet number in the mailbox buffer */
        pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_LOWWORD] = ((d & 0x0000FFFF));
        pFoeInd->FoeHeader.Cmd.PacketNo[FOE_COMMAND_HIGHWORD] = ((d & 0xFFFF0000) >> 16);

        /* store the size of the mailbox data in the mailbox buffer */
        pFoeInd->MbxHeader.Length                   = SIZEOF(TFOEHEADER);

        /* we wait for the next data part */
        if ( nextState == FOE_ACK )
        {
            /* we wait for the next data part */
            u16FileAccessState = FOE_WAIT_FOR_DATA;
        }
        else
        {
            /* the last data part was received */
            u16FileAccessState = FOE_READY;
        }
    }
    else if ( nextState < FOE_ERROR )
    {
        /* the file transmission sequence is finished, we have to send nothing */
        u16FileAccessState = FOE_READY;
        APPL_FreeMailboxBuffer(pFoeInd);
        return 0;
    }
    else
    {
        UINT32 d = SWAPDWORD(nextState);
        UINT8 b = 0;

        /* store the OpCode in the mailbox buffer */
        pFoeInd->FoeHeader.OpCode                    = SWAPWORD(ECAT_FOE_OPCODE_ERR);
        /* store the ErrorCode in the mailbox buffer */
        pFoeInd->FoeHeader.Cmd.ErrorCode[FOE_COMMAND_LOWWORD] = ((d & 0x0000FFFF));
        pFoeInd->FoeHeader.Cmd.ErrorCode[FOE_COMMAND_HIGHWORD] = ((d & 0xFFFF0000) >> 16);

        /* store the size of the mailbox data in the mailbox buffer */
        pFoeInd->MbxHeader.Length                   = SIZEOF(TFOEHEADER);

        /* Error Text is returned in pFoeInd->Data */
        while (b < 32)
        {
            UINT16 data = ((UINT16 MBXMEM *) pFoeInd->Data)[(b >> 1)];

            if ((data & 0x00FF) == 0)
            {
                break;
            }
            b++;
        
            if ((data & 0xFF00) == 0)
            {
                break;
            }
            b++;
        }
        if ( b < 32 )
        {
            pFoeInd->MbxHeader.Length    += b+1;
        }

        /* the file transmission sequence is finished */
        u16FileAccessState = FOE_READY;
    }

    if ( bBootMode )
    {
        /* in BOOT mode the mailbox buffer is not sent via the mailbox functions
           because only FoE is allowed in BOOT mode, so we have to include the
            mailbox data link layer counter */
        pFoeInd->MbxHeader.Flags[MBX_OFFS_COUNTER] &= ~MBX_MASK_COUNTER;
        if ( (u8MbxReadCounter & 0x07) == 0 )
        {
            /* counter 0 is not allowed if mailbox data link layer is supported */
            u8MbxReadCounter = 1;
        }
        /* store the counter in the mailbox header */
        pFoeInd->MbxHeader.Flags[MBX_OFFS_COUNTER] |= u8MbxReadCounter << MBX_SHIFT_COUNTER;
        /* increment the counter for the next service */
          u8MbxReadCounter++;
        /* call the function to send the mailbox service directly,
           in BOOT mode we can be sure that the send mailbox is empty
           because no parallel services are allowed */
        MBX_CopyToSendMailbox((TMBX MBXMEM *) pFoeInd);
    }
    else
    {
        if ( MBX_MailboxSendReq((TMBX MBXMEM *) pFoeInd, FOE_SERVICE) != 0 )
        {
            /* if the mailbox service could not be sent (or stored), the response will be
               stored in the variable pFoeSendStored and will be sent automatically
                from the mailbox handler (FOE_ContinueInd) when the send mailbox will be read
                the next time from the master */
            pFoeSendStored = (TMBX MBXMEM *) pFoeInd;
        }
    }

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pMbx      Pointer to the free mailbox buffer

 \brief    This function is called when the next mailbox fragment can be sent.
*////////////////////////////////////////////////////////////////////////////////////////

void FOE_ContinueInd(TMBX MBXMEM * pMbx)
{
    if ( pFoeSendStored )
    {
        /* send the stored FoE service which could not be sent before */
/*ECATCHANGE_START(V5.13) FOE1*/
        if (MBX_MailboxSendReq(pFoeSendStored, FOE_SERVICE) == 0)
        {
            pFoeSendStored = 0;
        }
    }
/*ECATCHANGE_END(V5.13) EOE1*/
}

/** @} */

