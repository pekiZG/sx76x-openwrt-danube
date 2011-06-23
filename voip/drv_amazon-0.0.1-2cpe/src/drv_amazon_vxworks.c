/*******************************************************************************
       Copyright (c) 2004, Infineon Technologies.  All rights reserved.

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
********************************************************************************
   Module      : drv_amazon_vxworks.c
   Date        : 2004-10-26
   Desription  : Board driver VxWorks OS File.
*******************************************************************************/

/** \file
    This file contains the board driver implementation for VxWorks OS.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

/* driver includes */
#include "drv_amazon_api.h"

#ifdef VXWORKS
/* ============================= */
/* Local defines and macros      */
/* ============================= */
#define amazon_DEVNAME  "/dev/amazon"


/* ============================= */
/* Local  structures             */
/* ============================= */

/* Global vxworks specific device
   driver structure. */
typedef struct
{
   /* this header is mandatory for
      vxworks drivers */
   DEV_HDR DevHdr;
   amazon_CTRL_DEV CtrlDev;
} amazon_CTRL_DEVCTX;

/* ============================= */
/* Global function declaration   */
/* ============================= */
INT32 amazon_init_driver  (VOID);
INT32 amazon_clear_driver (VOID);

/* Board common functions declaration */
IFX_int32_t amazon_DevInit  (IFX_uint32_t nDevNum);
IFX_int32_t amazon_DevReset (IFX_uint32_t nDevNum);
/* ============================= */
/* Local function declaration    */
/* ============================= */

LOCAL INT32 amazon_devcreate (VOID);
LOCAL INT32 amazon_devdelete (VOID);


LOCAL INT32 amazon_open  (amazon_CTRL_DEVCTX *pDevCtx,
                                  INT8 *pAnnex, INT32 flags);
LOCAL STATUS      amazon_close (amazon_CTRL_DEVCTX *pDevCtx);
LOCAL INT32 amazon_ioctl (amazon_CTRL_DEVCTX *pDevCtx, INT32 cmd,
                                  INT32 arg);


/* ============================= */
/* Local variable definition     */
/* ============================= */

/** driver number */
LOCAL INT32 amazon_drv_num = 0;

/** memory for board device contexes */
LOCAL amazon_CTRL_DEVCTX amazonDevCtx [MAX_amazon_INSTANCES];

/* ============================= */
/* Local function definition     */
/* ============================= */

/******************************************************************************/
/**
   Open the device.

   \param pDevCtx - device context private data
   \param pAnnex  - remaing part of string used during open()
   \param flags   - additional flags

   \return
   ERROR - on failure,
   non zero value on success
*/
/******************************************************************************/
LOCAL INT32 amazon_open (amazon_CTRL_DEVCTX *pDevCtx, INT8 *pAnnex,
                          INT32 flags)
{
    TRACE(amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV: open\n\r"));

    if (pDevCtx->CtrlDev.bInUse == IFX_TRUE)
    {
      TRACE(amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV: device already opened\n\r"));
      return (ERROR);
    }
    /* set in use variable */
    pDevCtx->CtrlDev.bInUse = IFX_TRUE;

    return (INT32)pDevCtx;
}

/******************************************************************************/
/**
   Close the device.

   \param pDevCtx - device context private data

   \return
    OK - on success
*/
/******************************************************************************/
LOCAL STATUS amazon_close (amazon_CTRL_DEVCTX *pDevCtx)
{
    TRACE(amazon_DRV, DBG_LEVEL_HIGH,("amazon_DRV: close\n\r"));

    amazon_DevReset (pDevCtx->CtrlDev.nDevNum - 1);
    pDevCtx->CtrlDev.bInUse = IFX_FALSE;

    return (OK);
}

/******************************************************************************/
/**
   Board device control / configuration.

   \param pDevCtx - device private data
   \param cmd     - ioctl command
   \param arg     - ioctl argument

   \return
      number of devices on board for FIO_amazon_INIT,
      otherwise OK or ERROR
*/
/******************************************************************************/
LOCAL INT32 amazon_ioctl (amazon_CTRL_DEVCTX *pDevCtx, INT32 cmd,
                                  INT32 arg)
{
   INT32 ret = OK;
   amazon_CTRL_DEV *pDev = &pDevCtx->CtrlDev;

   switch(cmd)
   {
   case FIO_amazon_DEBUGLEVEL:
      if ((arg >= DBG_LEVEL_LOW) && (arg <= DBG_LEVEL_OFF))
      {
         SetTraceLevel(amazon_DRV, (IFX_uint32_t)arg);
         SetLogLevel  (amazon_DRV, (IFX_uint32_t)arg);
      }
      break;
   case FIO_amazon_GET_VERSION:
      strncpy((char*)arg, DRV_amazon_VER_STR, 80);
      break;
   case FIO_amazon_CONFIG:
      ret = amazon_Board_Config (pDev, (amazon_Config_t *)arg);
      break;
   case FIO_amazon_GETCONFIG:
      ret = amazon_Board_GetConfig (pDev, (amazon_Config_t *)arg);
      break;
   case FIO_amazon_INIT:
      ret = amazon_Board_Init (pDev);
      break;
   case FIO_amazon_RESETCHIP:
      ret = amazon_Board_ResetChip (pDev, (amazon_Reset_t*)arg);
      break;
   case FIO_amazon_GETBOARDPARAMS:
      ret = amazon_Board_GetParams (pDev, (amazon_GetDevParam_t *)arg);
      break;
   case FIO_amazon_XYZREG_READ:
      ret = amazon_Board_ReadReg (pDev, (amazon_Reg_t*)arg);
      break;
   case FIO_amazon_XYZREG_WRITE:
      ret = amazon_Board_WriteReg (pDev, (amazon_Reg_t*)arg);
      break;
   case FIO_amazon_XYZREG_MODIFY:
      ret = amazon_Board_ModifyReg (pDev, (amazon_Reg_t*)arg);
      break;
   case FIO_amazon_SETLED:
      ret = amazon_Board_SetLed (pDev, (amazon_Led_t*)arg);
      break;
   case FIO_amazon_GET_BOARDVERS:
      ret = amazon_Board_GetBoardVers (pDev, (IFX_int32_t*)arg);
      break;
   case FIO_amazon_SET_ACCESSMODE:
      ret = amazon_Board_SetAccessMode (pDev, arg);
      break;
   case FIO_amazon_SET_CLOCKRATE:
      ret = amazon_Board_SetClockRate (pDev, arg);
      break;
   default:
      TRACE(amazon_DRV, DBG_LEVEL_HIGH,
           ("amazon_DRV: Unknown IoCtl (0x%08X), arg 0x%08lX.\n\r", cmd, arg));
      ret = ERROR;
      break;
   }

   return ret;
}

/******************************************************************************/
/**
   Board driver initialization.

   \return
   OK  or ERROR
*/
/******************************************************************************/
LOCAL INT32 amazon_devcreate (VOID)
{
   amazon_CTRL_DEVCTX *pDevCtx;
   IFX_uint32_t          i;
   INT32                 ret = ERROR;
   char                  buf[64];

   /* install driver */
   amazon_drv_num = iosDrvInstall(NULL, NULL, (FUNCPTR)amazon_open,
                                    (FUNCPTR)amazon_close, NULL,  NULL,
                                    (FUNCPTR)amazon_ioctl);
   if (amazon_drv_num <=  0)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV: unable to install the driver.\n\r"));
      return ERROR;
   }

   /* allocate memory and initialize devices */
   for (i = 0; i < MAX_amazon_INSTANCES; i++)
   {
      /* get hold of memory */
      pDevCtx = &amazonDevCtx [i];
      /* compose device name */
      sprintf(buf,"%s/%d", amazon_DEVNAME, i);
      /* Add device to the device list */
      ret = iosDevAdd(&(pDevCtx->DevHdr), buf, amazon_drv_num);
      if (ret == ERROR)
      {
         TRACE (amazon_DRV, DBG_LEVEL_HIGH,
                ("amazon_DRV: unable to create device.\n\r"));
         amazon_devdelete ();
         break;
      }
      /* initialize and cache device ptr */
      samazon_Dev [i] = &pDevCtx->CtrlDev;
      amazon_DevInit (i);
   }

   return ret;
}

/******************************************************************************/
/**
   Remove the Board Driver .

   \return
   OK
*/
/******************************************************************************/
LOCAL INT32 amazon_devdelete (VOID)
{
   IFX_uint32_t   i;

   for (i = 0; i < MAX_amazon_INSTANCES; i++)
   {
     if (samazon_Dev [i] != NULL)
     {
         /* reset device */
         amazon_DevReset (i);
         /* remove device */
         iosDevDelete((DEV_HDR *)&amazonDevCtx [i]);
         samazon_Dev [i] = NULL;
     }
   }
   /* remove the driver also */
   iosDrvRemove(amazon_drv_num, IFX_TRUE);
   /* and allow new DevCreate */
   amazon_drv_num = 0;

   return (OK);
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   returns the virtual address to access (Linux MMAP)

   \param nBaseAddrPhy - physical base address.
   \param nMemSize     - size of memory region to map.
   \param pMemName     - name of the memory region

   \return
      pointer to memory base address.
   \remark
*/
/******************************************************************************/
IFX_void_t * amazon_os_get_memaddr (IFX_uint32_t nBaseAddrPhy,
                                     IFX_uint32_t nMemSize,
                                     IFX_char_t  *pMemName)
{
   return (IFX_void_t *)nBaseAddrPhy;
}

/******************************************************************************/
/**
   release virtual memory

   \param pMemPtr   - virtual address.
   \param nBaseAddrPhy - physical base address.
   \param nMemSize     - size of memory region to map.

   \remark
*/
/******************************************************************************/
IFX_void_t amazon_os_release_memaddr (IFX_void_t  *pMemPtr,
                                       IFX_uint32_t nBaseAddrPhy,
                                       IFX_uint32_t nMemSize)
{
   return;
}

/******************************************************************************/
/**
   Initialise board driver.

   \return
   OK or ERROR
*/
/******************************************************************************/
INT32 amazon_init_driver (VOID)
{
   INT32 ret = OK, i;


   printf("%s\n\r", &amazon_WHATVERSION[4]);
   printf("(c) Copyright 2004, Infineon Technologies AG\n\n\n\r");
   SetTraceLevel(amazon_DRV, DBG_LEVEL_HIGH);
   SetLogLevel  (amazon_DRV, DBG_LEVEL_HIGH);

   /* create device driver if not yet done */
   if (amazon_drv_num <= 0)
   {
      for (i = 0; i < MAX_amazon_INSTANCES; i++)
      {
         memset (&amazonDevCtx[i].DevHdr, 0, sizeof (DEV_HDR));
      }
      ret = amazon_devcreate ();
   }
   else
   {
     TRACE (amazon_DRV, DBG_LEVEL_HIGH,
            ("amazon_DRV: driver already installed.\n\r"));
     ret = ERROR;
   }

   return ret;
}

/******************************************************************************/
/**
   clear board driver.

   \return
   OK or ERROR
*/
/******************************************************************************/
INT32 amazon_clear_driver (VOID)
{
   INT32 ret;

   /* create device driver if not yet done */
   if (amazon_drv_num > 0)
   {
      ret = amazon_devdelete ();
   }
   else
   {
     TRACE (amazon_DRV, DBG_LEVEL_HIGH,
            ("amazon_DRV: No driver installed.\n\r"));
     ret = ERROR;
   }

   return ret;
}

#endif /* VXWORKS */

