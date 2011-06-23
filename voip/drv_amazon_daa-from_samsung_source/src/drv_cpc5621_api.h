#ifndef _DRV_CPC5621_API_H
#define _DRV_CPC5621_API_H
/******************************************************************************
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
 ******************************************************************************
   Module      : $RCSfile: drv_cpc5621_api.h,v $
   Date        : $Date: 2004/08/06 12:06:50 $
   Description : Internal functional API of the driver.
******************************************************************************/

#include "drv_cpc5621_interface.h"
#include "drv_api.h"


#ifndef DRV_CPC5621_NAME
   #ifdef LINUX
/** device name */
#define DRV_CPC5621_NAME                    "cpc5621"
#define DRV_CPC5621_MAJOR                   220
#define DRV_CPC5621_MINOR_BASE              10
   #else
/** device name */
#define DRV_CPC5621_NAME                    "/dev/cpc5621"
   #endif
#else
   #error module name already specified
#endif


/** driver version, major number */
#define DRV_CPC5621_VER_MAJOR               0
/** driver version, minor number */
#define DRV_CPC5621_VER_MINOR               1
/** driver version, build number */
#define DRV_CPC5621_VER_STEP                1
/** driver version, package type */
#define DRV_CPC5621_VER_TYPE                1
/** driver version as string */
#define DRV_CPC5621_VER_STR                 "0.1.1.1"
/** driver version, what string */
#define DRV_CPC5621_WHAT_STR                "@(#)CPC5621 Driver, Version"DRV_CPC5621_VER_STR

/** maximum instances of this driver */
#define MAX_DEVICE                          1
#define MAX_CHANNEL                         8


/** CPC I/O pin ID */
#define DRV_CPC5621_OH                      0
#define DRV_CPC5621_RING                    4
#define DRV_CPC5621_BA_DETC                 2
#define DRV_CPC5621_POL_DETC                3
#if 0 // by wkhwang, 
#define DRV_CPC5621_CID                     4
#else
#define DRV_CPC5621_RING2                     1
#endif
#define DRV_CPC5621_MAX                     5
#define DRV_CPC5621_MASK                    0x1f
           
#define IO_HIGH                             0x1f
#define IO_LOW                              0

#define EXCEPTION_MASK                      0x7

#define RING_EXPIRE_TIME                   300  // by wkhwang,     30
#define CID_EXPIRE_TIME                     1800
#define BAT_EXPIRE_TIME                     150

typedef void (*_FUNCPTR) (int dev, int ch, WORD);


/** CPC5621 pin handler */
typedef struct {
    int id;
    int ioHandle;
    WORD nMask;
} PIN_HANDLE;

/** CPC5621 timer data */
typedef struct {
    UINT32      count;
    UINT32      tMin;
    TIMER_ID    TimerID;
} CPC5621_TIMER;

/** device related data */
typedef struct
{
#ifdef VXWORKS
    /** driver specific parameter */
    DEV_HDR         DevHdr;

    /** to see first open */
    BOOL            bNotFirst;
#endif

    /** successfully init */
    BOOL            bInit;

    /** exclusive open */
    BOOL            bOpen;
    
    UINT32          nInUse;

    /** support for select() */
    SYS_POLL        WakeupList;
    
    SEM_ID          ExceSem;
    
    INT_LOCK_T	    lock;
    
    /** device (DUSLIC) handler */
    INT32           devHandle;
    
    /** I/O handler table */
    PIN_HANDLE      pinHdlTab[DRV_CPC5621_MAX];
    
    UINT32          HookMode;
    
    UINT32          CidMode;
    
    CPC5621_TIMER   Ring;
    
    CPC5621_TIMER   Bat;
    
    CPC5621_TIMER   Cid;
    
    /** exception status */
    CPC5621_EXCEPTION nException;

#if 1 // by wkhwang,
    CPC5621_RING ringIndex;
    UINT32          RingDetect;
#endif
 
}CPC5621_DEV;


/** devices */
extern CPC5621_DEV* CPC5621_Devices[MAX_DEVICE][MAX_CHANNEL];

/** what string */
extern const char CPC5621_WHATVERSION[] ;

/** Declarations for debug interface, trace */
DECLARE_TRACE_GROUP(CPC5621_DRV);
/** Declarations for debug interface, log - interrupt safe */
DECLARE_LOG_GROUP(CPC5621_DRV);

UINT32 Drv_CPC5621_Init(CPC5621_DEV *pDev, int devNum, int chNum);
UINT32 Drv_CPC5621_Release(CPC5621_DEV *pDev);
#if 0 // by wkhwang, 
UINT32 Drv_CPC5621_DevInit(CPC5621_DEV *pDev);
#else
UINT32 Drv_CPC5621_DevInit(CPC5621_DEV *pDev, UINT32  nArg);
#endif
UINT32 Drv_CPC5621_Config(CPC5621_DEV *pDev, UINT32 nArg);   
UINT32 Drv_CPC5621_Set_OH(CPC5621_DEV *pDev, UINT32 nArg);   
UINT32 Drv_CPC5621_Get_OH(CPC5621_DEV *pDev, UINT32 nArg);
UINT32 Drv_CPC5621_Set_CID(CPC5621_DEV *pDev, UINT32 nArg);
UINT32 Drv_CPC5621_Get_CID(CPC5621_DEV *pDev, UINT32 nArg);
UINT32 Drv_CPC5621_Get_Ring(CPC5621_DEV *pDev, UINT32 nArg);
#if 0 // by wkhwang, 
UINT32 Drv_CPC5621_Get_Ring2(CPC5621_DEV *pDev, UINT32 nArg);
#endif
UINT32 Drv_CPC5621_Get_BA_Detect(CPC5621_DEV *pDev, UINT32 nArg);
UINT32 Drv_CPC5621_Get_POL_Detect(CPC5621_DEV *pDev, UINT32 nArg);
UINT32 Drv_CPC5621_Get_Exception(CPC5621_DEV *pDev, UINT32 nArg);



#endif  /* _DRV_CPC5621_API_H */

