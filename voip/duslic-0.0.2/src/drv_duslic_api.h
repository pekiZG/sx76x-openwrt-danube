/****************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ****************************************************************************
   Module      : drv_vinetic_api.h
   Date        : 2002-09-18
   Description : This file contains the defines, the structures declarations
                 the tables declarations and the global functions declarations.
*******************************************************************************/

#ifndef _DRV_DUSLIC_API_H
#define _DRV_DUSLIC_API_H

/* includes */
#include "drv_api.h"
#include "duslic_io.h"
#include "drv_duslic.h"
#include "drv_duslic_gpio.h"
#include "drv_duslic_errno.h"
#include "sys_drv_ifxos.h"


#define DUSLIC_MINOR_BASE       10
#define DUSLIC_MAJOR            240
#define DEV_NAME                "DUSLIC"


/* ================================ */
/* Enumerations                     */
/* ================================ */

/* device states */
/* firmware successfully downloaded */
#define   DS_FW_DLD         0x00000001
/* phi downloaded successfully, no further download will be allowed */
#define   DS_PHI_DLD        0x00000002
/* DTMF sure detected tons masked (MSRE1) */
#define   DS_DTMF_SURE      0x000004
/* DTMF probably detected tons masked (MSRE1)*/
#define   DS_DTMF_PROB      0x000008
/* CRAM Downloaded */
#define   DS_CRAM_DLD       0x000010
/* Board Config done */
#define   DS_BOARD_INIT     0x000020
/* Tapi Init done */
#define   DS_TAPI_INIT      0x000040
/* interrupt installed */
#define   DS_INT_ENABLED    0x000080
#ifdef ASYNC_CMD
/* command data was previously requested and maybe
   in command outbox. This flag will be reseted if the data has been read */
#define   DS_CMDREQ         0x000100
/* command data has been read and is available in pCmdData. This flag is
   used in CmdRead to check if requested data was read in another context */
#define   DS_CMDREAD        0x000200
/* a read command was sent to the inbox. If another location reads out all
   mailbox content it will be signaled with DS_CMDREAD */
#define   DS_RDCMD_SENT     0x000400
#endif /* ASYNC_CMD */
/** device successfully initialized */
#define   DS_DEV_INIT       0x000800
/** Coder module enabled  */
#define   DS_COD_EN         0x001000
/** Analog module enabled  */
#define   DS_ALM_EN         0x002000
/** signaling module enabled  */
#define   DS_SIG_EN         0x004000
/** PCM module enabled  */
#define   DS_PCM_EN         0x008000
/** DC Download done */
#define DS_DC_DWLD          0x010000
/** AC Download done */
#define DS_AC_DWLD          0x020000
/** Qos succesfully initialized */
#define   DS_QOS_INIT       0x040000

/* Indicates that a task is pending via select on this device */
/* Only used in vxWorks */
#define   DS_NEED_WAKEUP    0x100000

/* channel flags */
enum CH_FLAGS
{
   /* indicates a non-blocking read driver's function */
   CF_NONBLOCK             = 0x00000008,
   /** Tapi Init mode voice */
   CF_TAPI_VOICEMODE       = 0x00000100,
   /** Tapi Init mode PCM */
   CF_TAPI_PCMMODE         = 0x00000200,
   /** Tapi Init mode PCM VINTIC S */
   CF_TAPI_S_PCMMODE       = 0x00000400,
   /* Indicates that a task is pending via select on this device */
   /* Only used in vxWorks */
   CF_NEED_WAKEUP          = 0x00100000,
   /* indication the wakeup source of this channel */
   /* only used in vxworks */
   CF_WAKEUPSRC_TAPI       = 0x00200000,
   CF_WAKEUPSRC_STREAM     = 0x00400000,
   CF_WAKEUPSRC_GR909      = 0x00800000
};


typedef enum _DUSLIC_MAGIC_NUMBERS
{
    DUSLIC_DEVICE_MAGIC  = 0x44444557, /* "DDEV" */
    DUSLIC_CHANNEL_MAGIC = 0x4443414e  /* "DCHN" */
} DUSLIC_MAGIC_NUMBERS_t;


/** reports device reset to TAPI device specific status */
/* Device structure for channel */
typedef struct _DUSLIC_CHANNEL
{
  /* magic must be the first member */
   DUSLIC_MAGIC_NUMBERS_t	magic;

   void* pParent;				
   
   /* tracking in use of the device */
   IFX_int32_t                  nChannel;
   IFX_int32_t                  nInUse;
   IFX_int32_t                  GpioStatus; 
   IFX_int32_t                  GpioIntFlag; 
   IFX_int32_t                  nChState;
   
    /* flags for different purposes, see CH_FLAGS */
   IFX_uint32_t                nFlags;

   IFX_int32_t                 (* ReadReg) (struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, IFX_uint8_t offset,
                                IFX_uint8_t* pRegValue, IFX_int32_t count);
   
   IFX_int32_t                 (* WriteReg) (struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, IFX_uint8_t offset,
                                IFX_uint8_t* pRegValue, IFX_int32_t count);   
   
   IFX_int32_t                 (* ModifyReg) (struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, \
                                IFX_uint8_t offset, IFX_uint8_t RegValue, IFX_uint8_t BitMask);
   
   IFX_int32_t                 (* ciop)(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd);
   
   DUSLIC_GPIO_RES             GpioRes;   
   
}DUSLIC_CHANNEL;


typedef struct _DUSLIC_DEVICE
{
  /* magic must be the first member */
   DUSLIC_MAGIC_NUMBERS_t	magic;

    /* tracking in use of the device */
    IFX_int32_t           nChannel;
    IFX_int32_t           nInUse;
    IFX_int32_t           nDevNr;
    IFX_int32_t           err;
    IFX_int32_t           nIrq;
    IFX_int32_t           nDevState;
   
    DUSLIC_CHANNEL        pChannel[2]; //the maximum number of duslic channels is 2 
   
    /* device read function pointer */
    IFX_int32_t                 (* read) (struct _DUSLIC_DEVICE *pDev,IFX_uint8_t* cmd,\
                                         IFX_int32_t cmd_count, IFX_uint8_t* pData, IFX_int32_t count);
   
    /* device write function pointer */
    IFX_int32_t                (* write) (struct _DUSLIC_DEVICE *pDev, IFX_uint8_t* pData, IFX_int32_t count);
}DUSLIC_DEVICE;




extern INT OS_GetDevice (INT nr, DUSLIC_DEVICE** pDev); 

IFX_int32_t ReadReg(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, IFX_uint8_t offset,\
                    IFX_uint8_t* pRegValue, IFX_int32_t count);

IFX_int32_t  WriteReg(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, IFX_uint8_t offset,\
                                IFX_uint8_t* pRegValue, IFX_int32_t count);  
IFX_int32_t  ciop(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd);
IFX_int32_t  ModifyReg(struct _DUSLIC_CHANNEL *pCh, IFX_uint8_t cmd, IFX_uint8_t offset,\
                                IFX_uint8_t RegValue, IFX_uint8_t BitMask);
#endif /* _DRV_DUSLIC_API_H */
