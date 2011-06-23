#ifndef _SYS_DRV_OSE_H
#define _SYS_DRV_OSE_H
/****************************************************************************
       Copyright (c) 2003, Infineon Technologies.  All rights reserved.

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
   Module      : $RCSfile: sys_drv_ose.h,v $
   Date        : $Date: 2004/10/07 03:53:32 $
   Description : This file contains the includes and the defines
                 specific to the OSE OS

*******************************************************************************/

/** \file
   This file contains the includes and the defines specific to the OSE OS
*/

/** \defgroup SYS_DRV_OSE Definitions for OSE
*/

/* ============================= */
/* Global Includes               */
/* ============================= */

/* System includes*/

#include <ose.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

/*#include <sys_drv_delaylib_ose.h>*/
/*#include "driverlib.h"*/

/** \defgroup SYS_DRV_OSE_FEAT Compiler features
   \ingroup SYS_DRV_OSE
*/
/*@{*/
/** Do not inline functions. */
#define INLINE
/** Define functions only local. */
#define LOCAL  static
/*@}*/

/* ============================= */
/* Function Macros               */
/* ============================= */

/** \defgroup SYS_DRV_OSE_DELAY Delay functions
   \ingroup SYS_DRV_OSE

   This section defines different delay functions
   with active waiting or with enabling the scheduler.

*/
/*@{*/

#define GetImmr()                         IMAP_ADDR

#define IMAP_ADDR                                              \
({									                                    \
   int __ret = 0;							                           \
   __asm__("mfspr %0,638": "=r"(__ret) : );                    \
   __ret &= 0xFFFF0000;                                        \
   __ret;								                              \
})


/** Wait milliseconds with rescheduling.
 */
#define Wait(time)                        delay (time)
#define Wait_Milli_Sec(time)              delay (time)
/*#define Wait_Micro_Sec(time)              delayUsec(time)*/

/*@}*/

/** \defgroup SYS_DRV_OSE_MEMORY Memory functions
   \ingroup SYS_DRV_OSE

   This section defines functions for allocating and freeing memory.
   If DEBUG is defined some functions to trace memory allocation are used
   (s. sys_drv_debug.c)
*/
/*@{*/
#ifdef DEBUG
   /* define own functions for allocation and free */
/*
   #define Alloc_Mem(size)                      malloc(size)
   #define Free_Mem(ptr)                        free((void*)ptr)
*/

   #define Alloc_Mem(size)                      sys_dbgMalloc(size, __LINE__, __FILE__)
   #define Free_Mem(ptr)                        sys_dbgFree((void*)ptr, __LINE__, __FILE__)

   /* wrapper for os malloc and free in case of debug */
   #define sysdebug_malloc(size)                malloc(size)
   #define sysdebug_free(ptr)                   free((void*)ptr)
#else
   /** Allocate memory. */
   #define Alloc_Mem(size)                      malloc(size)
   /** Free memory. */
   #define Free_Mem(ptr)                        free((void*)ptr)
#endif


/** copy from user to kernel space
   \note Different return codes! */
#define copy_from_user(to, from, n)             memcpy(to, from, n)
/** copy from kernel to user space
   \note Different return codes! */
#define copy_to_user(to, from, n)               memcpy(to, from, n)

/*@}*/

/** \defgroup SYS_DRV_OSE_EVENT Event handling functions
   \ingroup SYS_DRV_OSE

   This section defines functions for event handling.
*/
/*@{*/

/** Initialize an event. */
#define Init_Event(sem) \
   (sem) = create_sem(0)

/** Signal an event. */
#define Wake_Up_Event(sem)              signal_sem (sem)

/** Wait for an event with timeout.
   Returns FALSE / 0 on timeout.
   \note VxWorks delay is very inaccurate, and cannot wait for less than one tick
   so we must wait at least one tick */
#define Sleep_On(sem, timeout)            wait_sem(sem)

#define Sleep_On_Event(sem, timeout)            wait_sem(sem)
/*@}*/

/** Initialize a queue. */
#define Init_WakeupQueue(queue)                 FIXME

/** Signal a queue. */
#define Wake_Up_Queue(queue)                    FIXME


/** \defgroup SYS_DRV_OSE_MUTEX Mutex functions
   \ingroup SYS_DRV_OSE

   This section has mutex definitions.
*/
/*@{*/

/** Create a mutex element. */
#define Sem_MutexInit(sem)                      \
   (sem) = create_sem(0)
/** Lock a mutex section. */
#define Sem_Lock(sem)                           wait_sem(sem)

/** Unlock a mutex section. */
#define Sem_Unlock(sem)                         signal_sem (sem)

/*@}*/

/** \defgroup SYS_DRV_OSE_CLOCK Time/Clock functions
   \ingroup SYS_DRV_OSE
*/
/*@{*/


/** Returns system tick in milliseconds. */
#define Get_Tick()                                    \
({                                                    \
(system_tick() / 1000 * get_ticks());                 \
})

/** Returns system tick */
#define Get_Tick_Cnt()                    

/*@}*/

/** \defgroup SYS_DRV_OSE_INTERRUPT Enable/Disable global interrupts
   \ingroup SYS_DRV_OSE
*/
/*@{*/

/*----------------------------------------------------------------------------*/
/* Lock mechanism for general processor interrupts                            */
/* Note: Please avoid to use the mechanism, because disabling all interrupts  */
/*       of the system is not a good solution.                                */
/*----------------------------------------------------------------------------*/
#define Lock_Variable                     FIXME
#define Lock_Interrupts(var)              FIXME
#define Unlock_Interrupts(var)            FIXME
/*----------------------------------------------------------------------------*/

/*@}*/

/* ============================= */
/* Global function declaration   */
/* ============================= */

#warning "please make use of these functions"
#if 0
/* Device Init. Called form Board_DevOpen when the device has been opened for
   the first time. The device structure will be cached */
extern int OS_InitDevice (int nDevn);
/* Device is stopped. Called on device close */
extern void OS_StopDevice (void* pDevice);
/* Install VINETIC ISR   */
extern void OS_Install_IRQ_Handler(void* pDevice, int nIrq);
/* Uninstall VINETIC ISR */
extern void OS_UnInstall_IRQ_Handler(void* pDevice);
#endif /* 0 */

/* ============================= */
/* Global structures declaration */
/* ============================= */

/* ============================= */
/* typedefs                      */
/* ============================= */

/** Use semaphores for events. */
/*typedef SEMAPHORE               *SYS_EVENT; FIXME*/
typedef SEMAPHORE *                SYS_EVENT;
/** task id */
typedef unsigned long            SYS_POLL;

/* return values */
#define SUCCESS 0
#define ERR  -1

#ifdef _IO
#undef _IO
#endif

# define _IO(x,y)       (((x)<<8)|y)

#endif /* _SYS_DRV_OSE_H */

