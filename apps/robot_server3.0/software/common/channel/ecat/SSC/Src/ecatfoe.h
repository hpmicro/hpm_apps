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
\file ecatfoe.h
\author EthercatSSC@beckhoff.com

\version 5.12

<br>Changes to version V5.11:<br>
V5.12 FOE2: handle 16bit only access<br>
<br>Changes to version V5.10:<br>
V5.11 FOE1: handle busy on a read request, change OPMode to OPCode (same terms as in the spec)<br>
<br>Changes to version V5.01:<br>
V5.10 FOE1: Add new FoE error codes (0x800C,0x800D,0x800F)<br>
V5.10 FOE2: Update file handling to support File sizes greater than 0x8000 Bytes (change value of "FOE_MAXDATA")<br>
<br>Changes to version - :<br>
V5.01 : Start file change log
 */

/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "esc.h"
#include "mailbox.h"

#ifndef _ECATFOE_H_
#define _ECATFOE_H_

/*-----------------------------------------------------------------------------------------
------
------    Defines and Types
------
-----------------------------------------------------------------------------------------*/



/*---------------------------------------------
-    Services
-----------------------------------------------*/
#define    ECAT_FOE_OPCODE_RRQ                  1 /**< \brief Read request*/
#define    ECAT_FOE_OPCODE_WRQ                  2 /**< \brief Write request*/
#define    ECAT_FOE_OPCODE_DATA                 3 /**< \brief Data datagram*/
#define    ECAT_FOE_OPCODE_ACK                  4 /**< \brief Acknowledge datagram*/
#define    ECAT_FOE_OPCODE_ERR                  5 /**< \brief Error datagram*/
#define    ECAT_FOE_OPCODE_BUSY                 6 /**< \brief Busy datagram*/


/*---------------------------------------------
-    States
-----------------------------------------------*/
#define    FOE_READY                            0 /**< \brief Ready state*/
#define    FOE_WAIT_FOR_ACK                     1 /**< \brief Wait for acknowledgement state*/
#define    FOE_WAIT_FOR_DATA                    2 /**< \brief Wait for data state*/
#define    FOE_WAIT_FOR_RESET                   3 /**< \brief Wait for reset state*/
#define    FOE_WAIT_FOR_LAST_ACK                4 /**< \brief Wait for last acknowledgement state*/
#define    FOE_PROGRAMMING                      5 /**< \brief Programming state*/
#define    FOE_WAIT_FOR_LAST_DATA               6 /**< \brief Wait for last data state*/





/** \brief FoE header*/
typedef struct  MBX_STRUCT_PACKED_START
{
    UINT16        OpCode; /**< \brief OpCode
                             *
                             * 1 : RRQ<br>
                             * 2 : WRQ<br>
                             * 3 : DATA<br>
                             * 4 : ACK<br>
                             * 5 : ERR<br>
                             * 6 : BUSY*/
    union MBX_STRUCT_PACKED_START
    {
        UINT16        Password[2]; /**< \brief Password (used in Read request and Write request). 0 if unknown*/
        UINT16        PacketNo[2]; /**< \brief Packet number (used in DATA and ACK datagram)*/
        UINT16        ErrorCode[2]; /**< \brief Error code (used in ERR datagram)*/
        #define FOE_COMMAND_LOWWORD		0
        #define FOE_COMMAND_HIGHWORD	1
        struct MBX_STRUCT_PACKED_START
        {
            UINT16    Done; /**< \brief Done indication (used in BUSY datagram)*/
            UINT16    Entire;  /**< \brief Entire indication (used in BUSY datagram)*/
        }MBX_STRUCT_PACKED_END
        Busy; /**< \brief Busy variable*/
    }MBX_STRUCT_PACKED_END
    Cmd; /**< \brief Command field*/


}MBX_STRUCT_PACKED_END
TFOEHEADER;


#define     FOE_HEADER_SIZE     SIZEOF(TFOEHEADER) /**< \brief FoE header size*/

 /** \brief Complete FoE datagram*/
typedef struct MBX_STRUCT_PACKED_START
{
      TMBXHEADER        MbxHeader; /**< \brief Mailbox header*/
      TFOEHEADER        FoeHeader; /**< \brief FoE header*/
    UINT16              Data[((MAX_MBX_DATA_SIZE)-(FOE_HEADER_SIZE)) >> 1]; /**< \brief FoE Data buffer*/
}MBX_STRUCT_PACKED_END
TFOEMBX;




/*---------------------------------------------
-    Results
-----------------------------------------------*/
#define    FOE_ERROR                            0x8000 /**< \brief Error result*/
#define    FOE_FINISHED                         ((FOE_ERROR)-1) /**< \brief Datagram finished*/
#define    FOE_FINISHED_NOACK                   ((FOE_FINISHED)-1) /**< \brief Datagram finished no acknowledgement*/
#define    FOE_ACK                              ((FOE_FINISHED_NOACK)-1) /**< \brief Acknowledgement*/
#define    FOE_ACKFINISHED                      ((FOE_ACK)-1) /**< \brief Acknowledgement finished*/
#define    FOE_WAIT                             ((FOE_ACKFINISHED)-1) /**< \brief Wait */
#define    FOE_MAXBUSY                          ((FOE_WAIT)-1) /**< \brief Max busy indication*/
#define    FOE_MAXBUSY_ZERO                     ((FOE_MAXBUSY)-100) /**< \brief Min busy indication*/
#define    FOE_MAXDATA                          ((FOE_MAXBUSY_ZERO)-1) /**< \brief Maximum data within one FoE Datagram*/

#endif //_ECATFOE_H_

#if _ECATFOE_
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global Variables
------
-----------------------------------------------------------------------------------------*/
PROTO    UINT32                                 u32PacketNo; /**< \brief Stores the actual packet number to be expected*/
PROTO    UINT32                                 u32FileOffset;  /**< \brief Stores the next file offset to be sent */
PROTO    UINT32                                 u32LastFileOffset; /**< \brief Stores the actual sent file offset */
PROTO    UINT16                                 u16FileAccessState; /**< \brief Stores the actual state of the file transmission sequence */
PROTO    TMBX MBXMEM *                          pFoeSendStored; /**<If the mailbox service could not be sent (or stored),
                                                                                the FoE service will be stored in this variable
                                                                                and will be sent automatically from the mailbox handler
                                                                                (FOE_ContinueInd) when the send mailbox will be read
                                                                                the next time from the master */

PROTO   UINT16                                  u16FoeMaxSendBlockSize;  /**< \brief indicates the maximum size of an FoE fragment (from the slave to the master)*/
/*-----------------------------------------------------------------------------------------
------
------   Global Functions
------
-----------------------------------------------------------------------------------------*/

PROTO    void     FOE_Init(void);
PROTO    UINT8 FOE_ServiceInd(TFOEMBX MBXMEM * pFoeInd);
PROTO    void     FOE_ContinueInd(TMBX MBXMEM * pMbx);

#undef PROTO
/** @}*/
