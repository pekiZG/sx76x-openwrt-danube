#ifndef _DRV_AMAZON_API_H
#define _DRV_AMAZON_API_H
/****************************************************************************
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
 ****************************************************************************
   Module      : drv_amazon_api.h
   Date        : 2004-10-25
   Description : Board driver API.
*******************************************************************************/
/** \file
    This is the vinetic board driver api file and contains declarations of
    control structures, api generic and low level functions, global
    defines and global includes.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "ifx_types.h"
#include "sys_drv_ifxos.h"
#include "sys_drv_debug.h"
#include "amazon_io.h"

#ifndef _MKSTR_1
#define _MKSTR_1(x)    #x
#define _MKSTR(x)      _MKSTR_1(x)
#endif

/* ============================= */
/* Global defines and macros     */
/* ============================= */

/** driver version, major number */
#define DRV_amazon_VER_MAJOR        0
/** driver version, minor number */
#define DRV_amazon_VER_MINOR        0
/** driver version, build number */
#define DRV_amazon_VER_STEP         1
/** driver version, package type */
#define DRV_amazon_VER_TYPE         8
/** driver version as string */
#define DRV_amazon_VER_STR          _MKSTR(DRV_amazon_VER_MAJOR)"."\
                                      _MKSTR(DRV_amazon_VER_MINOR)"."\
                                      _MKSTR(DRV_amazon_VER_STEP)"."\
                                      _MKSTR(DRV_amazon_VER_TYPE)

/** driver version, what string */
#define DRV_amazon_WHAT_STR          "@(#)amazon Board Driver, Version "\
                                       DRV_amazon_VER_STR

/** maximum instances of this driver */
#define MAX_amazon_INSTANCES        1


/* amazon trace/log group */
DECLARE_TRACE_GROUP (amazon_DRV);
DECLARE_LOG_GROUP   (amazon_DRV);

/* ============================= */
/* Global driver enums           */
/* ============================= */
typedef enum
{
   amazon_XYZ_READ,
   amazon_XYZ_WRITE,
   amazon_XYZ_MODIFY
} amazon_RegAccess_e;

/* ============================= */
/* Global structures             */
/* ============================= */

typedef struct
{
   /** Vinetic chip number for which
       information will be retrieved */
   unsigned int  nChipNum;
   /** Physical Base address to access
       the chip */
   unsigned long nBaseAddrPhy;
   /** Irq Line on which an interrupt
       handler will be installed */
   unsigned int  nIrqNum;
} amazon_ChipInfo_t;

/** amazon Driver Control structure */
typedef struct
{
   /** device number */
   IFX_uint32_t nDevNum;
   /** device state */
   IFX_boolean_t bInUse;
   /** XYZ Access possible ? */
   IFX_boolean_t bXYZAccess;
   /** Board Initialization state */
   IFX_boolean_t bBoardInitialized;
   /** board configuration parameters */
   amazon_Config_t boardConf;
   /** Number of Vinetic chips on this board */
   IFX_uint32_t  nChipTotNum;
   /** pointer to an Array of Chip information
       according to number of chips on the board */
   amazon_ChipInfo_t *pChipInfo;
} amazon_CTRL_DEV;

/* ============================= */
/* Global variable   declaration */
/* ============================= */

/** device pointers cache */
extern amazon_CTRL_DEV *samazon_Dev [MAX_amazon_INSTANCES];
/** board driver version */
extern const char amazon_WHATVERSION [];
/** chip info cache */
extern const amazon_ChipInfo_t amazon_ChipInfo [];
/** Maximum number of Vinetic per device */
extern const IFX_uint32_t namazonMaxVineticChips;

/* ============================= */
/* Global functions  declaration */
/* ============================= */



/* API high level (ioctl) functions declaration */
extern IFX_int32_t amazon_Board_Init         (amazon_CTRL_DEV *pDev);
extern IFX_int32_t amazon_Board_ResetChip    (amazon_CTRL_DEV *pDev,  amazon_Reset_t  *pReset);
extern IFX_int32_t amazon_Board_Config       (amazon_CTRL_DEV *pDev,  amazon_Config_t *pConf);
extern IFX_int32_t amazon_Board_GetConfig    (amazon_CTRL_DEV *pDev,  amazon_Config_t *pConf);
extern IFX_int32_t amazon_Board_GetParams    (amazon_CTRL_DEV *pDev,  amazon_GetDevParam_t *pParam);
extern IFX_int32_t amazon_Board_ReadReg      (amazon_CTRL_DEV *pDev,  amazon_Reg_t *pReg);
extern IFX_int32_t amazon_Board_WriteReg     (amazon_CTRL_DEV *pDev,  amazon_Reg_t *pReg);
extern IFX_int32_t amazon_Board_ModifyReg    (amazon_CTRL_DEV *pDev,  amazon_Reg_t *pReg);
extern IFX_int32_t amazon_Board_SetLed       (amazon_CTRL_DEV *pDev,  amazon_Led_t* pLed);
extern IFX_int32_t amazon_Board_GetBoardVers (amazon_CTRL_DEV *pDev,  IFX_int32_t*pBoardVersion);
extern IFX_int32_t amazon_Board_SetAccessMode (amazon_CTRL_DEV *pDev, amazon_AccessMode_e nAccessMode);
extern IFX_int32_t amazon_Board_SetClockRate  (amazon_CTRL_DEV *pDev, amazon_ClockRate_e  nClockRate);
#endif /* _DRV_amazon_API_H */
