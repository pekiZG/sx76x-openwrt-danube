#ifndef _SYS_DRV_PSOS_H
#define _SYS_DRV_PSOS_H
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
   Module      : $RCSfile: sys_drv_psos.h,v $
   Date        : $Date: 2004/10/07 03:53:32 $
   Description : This file contains the includes and the defines
                 specific to the pSOS OS

*******************************************************************************/

/** \file
   This file contains the includes and the defines specific to the pSOS OS
*/

/** \defgroup SYS_DRV_PSOS Definitions for pSOS
*/

/* ============================= */
/* Global Includes               */
/* ============================= */

/* System includes*/

#include <psos.h>
#include <board.h>
#include <sys_conf.h> /* KC_TICKS2SEC */
#include "t_defs.h"
#include <hwifc.h>

/*
#include <drv/timer/timerDev.h>
#include <sys_drv_delaylib_psos.h>
#include <driverlib.h>
*/

/** \defgroup SYS_DRV_PSOS_FEAT Compiler features
   \ingroup SYS_DRV_PSOS
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

/** \defgroup SYS_DRV_PSOS_DELAY Delay functions
   \ingroup SYS_DRV_PSOS

   This section defines different delay functions
   with active waiting or with enabling the scheduler.

*/
/*@{*/

/** ticks per second */
#define OS_PER_SECOND()		  KC_TICKS2SEC
/** This macros converts seconds to ticks. */
#define OS_SEC2TICK(sec)      ((sec)*OS_PER_SECOND())
/** This macro converts mili seconds to ticks.
   ( ticks = n[milisecond] * ticks/[second] / 1000) )*/
#define OS_MSEC2TICK(msec)    ((msec)*OS_PER_SECOND()/1000)
/** This macro converts ticks to seconds. */
#define OS_TICK2SEC(tick)     ((tick)/OS_PER_SECOND())
/** This macro converts ticks to mili seconds. */
#define OS_TICK2MSEC(tick)    ((tick)*1000/OS_PER_SECOND())

/** Wait milliseconds with rescheduling.
 */
#define Wait(msec)             tm_wkafter(OS_MSEC2TICK(msec))

/*@}*/

/** \defgroup SYS_DRV_PSOS_MEMORY Memory functions
   \ingroup SYS_DRV_PSOS

   This section defines functions for allocating and freeing memory.
   If DEBUG is defined some functions to trace memory allocation are used
   (s. sys_drv_debug.c)
*/
/*@{*/
#ifdef DEBUG
   /* define own functions for allocation and free */
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
#define Host_Alloc(size)                  malloc(size)


#define GetImmr() (GetIMMR() & IO_MAP_MASK);

/** copy from user to kernel space
   \note Different return codes! */
#define copy_from_user(to, from, n)             memcpy(to, from, n)
/** copy from kernel to user space
   \note Different return codes! */
#define copy_to_user(to, from, n)               memcpy(to, from, n)

/*@}*/

/** \defgroup SYS_DRV_PSOS_EVENT Event handling functions
   \ingroup SYS_DRV_PSOS

   This section defines functions for event handling.
*/
/*@{*/

/** Initialize an event. */
#define Init_Event(name, event)                 sm_create(name, 1, SM_LOCAL | SM_FIFO | SM_BOUNDED, &event)

/** Signal an event. */
#define Wake_Up_Event(event)                    sm_v(event)

/** Wait for an event with timeout.
   Returns FALSE / 0 on timeout.
*/
#define Sleep_On_Event(event, timeout)          sm_p(event, SM_WAIT, OS_MSEC2TICK(timeout)) == 0 ? 1 : 0

/*@}*/

/** Initialize a wakeup event */
#define Init_WakeupQueue(wake_up, tid, event)   wake_up.tid = tid; wake_up.event = event;

/** Signal a wakeup event. */
#define Wake_Up_Queue(wake_up)                  ev_send(wake_up.tid, wake_up.event)


/** \defgroup SYS_DRV_PSOS_MUTEX Mutex functions
   \ingroup SYS_DRV_PSOS

   This section has mutex definitions.
*/
/*@{*/

/** Create a mutex element. */
#define Sem_MutexInit(name, sem)                sm_create(name, 1, SM_LOCAL | SM_FIFO, &sem)

/** Lock a mutex section. */
#define Sem_Lock(sem)                           sm_p(sem, SM_WAIT, 0);

/** Unlock a mutex section. */
#define Sem_Unlock(sem)                         sm_v(sem)

/*@}*/

/** \defgroup SYS_DRV_PSOS_CLOCK Time/Clock functions
   \ingroup SYS_DRV_PSOS
*/
/*@{*/

/** Returns CPU clock in Hz. */
#define Cpu_Clk                           GetSystemClock()

/** Returns system tick in milliseconds. */
#define Get_Tick()
/** Returns system tick */
#define Get_Tick_Cnt()                    

/*@}*/

/** \defgroup SYS_DRV_PSOS_INTERRUPT Enable/Disable global interrupts
   \ingroup SYS_DRV_PSOS
*/
/*@{*/

/*----------------------------------------------------------------------------*/
/* Lock mechanism for general processor interrupts                            */
/* Note: Please avoid to use the mechanism, because disabling all interrupts  */
/*       of the system is not a good solution.                                */
/*----------------------------------------------------------------------------*/
#define Lock_Variable
#define Lock_Interrupts(var)
#define Unlock_Interrupts(var)
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
typedef unsigned long            SYS_EVENT;

/** semaphore id */
typedef unsigned long            SEM_ID;

/**  */
typedef struct
{
   unsigned long tid;
   unsigned long event;
} wake_up_list_t;

typedef wake_up_list_t           SYS_POLL;

/* return values */
#define SUCCESS 0
#define ERR  -1

#define _IO(x,y)			         (y)

#endif /* _SYS_DRV_PSOS_H */

