#ifndef _SYS_DRV_NOOS_H
#define _SYS_DRV_NOOS_H
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
   Module      : $RCSfile: sys_drv_noOS.h,v $
   Date        : $Date: 2004/05/18 10:09:50 $
   Description : This file contains the OS-Less
                 specific defines
   Remarks     :
               - Drivers using this OS - file should be compiled with additional
                 compiler switch -DNO_OS
*******************************************************************************/

/** \file sys_drv_noOS.h
    \brief Mapping of driver macros to No OS system functions for availability of
           driver source code for No OS system.
*/

/* ============================= */
/* Global Includes               */
/* ============================= */

#include "sys_noOSLib.h"

/* ============================= */
/*          Endianess            */
/* ============================= */

/**  \defgroup SYS_DRV_NOOS_ENDIAN Endianess support Macros
     \brief The uC Endianess compiler switch should be
            added to the compiler defines.

     Add to your compiler switches :
     - -DNO_OS_LITTLE_ENDIAN for Little Endian
     - -DNO_OS_BIG_ENDIAN for Big Endian Endian
 */
/*\{*/
#ifdef __BYTE_ORDER
#undef __BYTE_ORDER
#endif
/* add indianness to your compiler switches */
#ifdef NO_OS_LITTLE_ENDIAN
   /** byte order is little endian */
   #define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined (NO_OS_BIG_ENDIAN)
   /** byte order is big endian */
   #define __BYTE_ORDER __BIG_ENDIAN
#else
   #error "Unknown Byteorder!"
#endif
/*\}*/

/* ============================= */
/* Function Macros               */
/* ============================= */

/**  \defgroup SYS_DRV_NOOS_MEM Hardware Memory Access macros

     Driver macros for memory base address are defined here.
*/
/*\{*/
/** Base address macro */
#define IMAP_ADDR                    No_OS_GetHwBaseAddr ()
/** Macro Function returning the base address */
#define GetImmr()                    IMAP_ADDR
/*\}*/

/* #define Cpu_Clk   */              /* to be done */

/** \defgroup SYS_DRV_NOOS_DELAY Delay Macros

   This section defines different driver delay macros for No OS system.
*/
/*\{*/
/** Wait milliseconds.  */
#define Wait(time)                   No_OS_Wait((time))
/** Wait active milliseconds */
#define Wait_Milli_Sec(time)         No_OS_Wait_ms((time))
/** Wait active microseconds */
#define Wait_Micro_Sec(time)         No_OS_Wait_us((time))
/*\}*/

/** \defgroup SYS_DRV_NOOS_MEMORY Memory macros

   Driver macros for allocating and freeing memory.
   If DEBUG is defined some functions to trace memory allocation are used
   (s. sys_debug.c)
*/
/*\{*/
#ifdef DEBUG
   /* define own functions for
      allocation and free */
   #define Alloc_Mem(size)           sys_dbgMalloc(size, __LINE__, __FILE__)
   #define Free_Mem(ptr)             sys_dbgFree((void*)ptr, __LINE__, __FILE__)
   /* wrapper for os malloc and
      free in case of debug */
   #define sysdebug_malloc(size)     malloc(size)
   #define sysdebug_free(ptr)        free((void*)ptr)
#else
   /** Allocate memory. */
   #define Alloc_Mem(size)           malloc(size)
   /** Free memory. */
   #define Free_Mem(ptr)             if (ptr) free((void*)ptr)
#endif
/** Adaptation of copy_from_user.  */
#define copy_from_user(to, from, n)  memcpy((to), (from), (n))
/** Adaptation of copy_to_user. */
#define copy_to_user(to, from, n)    memcpy((to), (from), (n))
/*\}*/

/** \defgroup SYS_DRV_NOOS_EVENT Event handling Macros */
/*\{*/
/** Initialize an event. */
#define Init_Event(event)            No_OS_InitEvent (&event, FALSE)
/** Validate if event was initialized correctly */
#define Valid_Event(queue) \
   (((queue)==NULL)?ERR:SUCCESS)

/** Signal an event. */
#define Wake_Up_Event(event)         No_OS_WakeupEvent(event)
/** Sleep on an event. */
#define WaitEvent(event, condition, timeout)  \
                                     No_OS_WaitEvent(event, condition, timeout)
/** Wait for an event with timeout.*/
/*
#define Sleep_On_Event(event, timeout)                                       \
      (No_OS_SleepOnEvent((event), ((timeout)==(WAIT_FOREVER)?(WAIT_FOREVER):\
      ((timeout)==(NO_WAIT)?(NO_WAIT):                                       \
      (((timeout) * No_OS_ClkRateGet() / 1000) + 1)))))
*/
#define Sleep_On_Event(event, timeout)	 No_OS_SleepOnEvent((event), (timeout))

/** Delete an event. */
#define Delete_Event(event)          No_OS_DeleteEvent (&event)
/* Reset event to initial state. */
#define Clear_Event(evt)
/* Initialize a queue */
#define Init_WakeupQueue(queue)      queue = 0;
/* Signal a queue */
#define Wake_Up_Queue(queue)
#define Wake_Down_Queue(queue)
#define Wake_Up_Node(node)
/*\}*/

/** \defgroup SYS_DRV_NOOS_MUTEX Mutex Macros */
/*\{*/
/** Create a mutex element. */
#define Sem_MutexInit(sem)           No_OS_InitEvent     (&sem, TRUE)
/** initialize a binary semaphore. */
#define Sem_BinaryInit(sem)          No_OS_InitEvent     (&sem, TRUE)
/** Lock a mutex section. */
#define Sem_Lock(sem)                No_OS_SleepOnEvent  (sem, WAIT_FOREVER)
/** Unlock a mutex section. */
#define Sem_Unlock(sem)              No_OS_WakeupEvent   (sem)
/** Delete a mutex element. */
#define Sem_MutexDelete(sem)         No_OS_DeleteEvent   (&sem)
/*\}*/

/** \defgroup SYS_DRV_NOOS_CLOCK Time/Clock Macros */
/*\{*/
/** Returns system tick in milliseconds. */
#define Get_Tick()                   No_OS_GetTickms()
/** Returns system tick. */
#define Get_Tick_Cnt()               No_OS_GetTickCnt()
/** Returns No OS clock rate. */
#define Get_ClkRate()                No_OS_ClkRateGet()
/*\}*/

/** \defgroup SYS_DRV_NOOS_LOCK Interrupts Macros */
/*\{*/
/** Lock variable. */
#define Lock_Variable              INT
/** Lock all interrupts. */
#define Lock_Interrupts(var)       var = No_OS_LockInt()
/** Unlock all interrupts. */
#define Unlock_Interrupts(var)     No_OS_UnlockInt(var)
/** Disable interrupts. */
#define Disable_Irq(irq)           No_OS_IntDisable((irq))
/** Enable interrupts. */
#define Enable_Irq(irq)            No_OS_IntEnable ((irq))
/* Lock/Unlock/Start/Delete Tasks */
#define Lock_Tasks()
#define Unlock_Tasks()
#define OS_Delete_Task(taskId)
#define OS_Start_Task(a, b, c, d, e, f) \
                                   0
/*\}*/



/** \defgroup SYS_DRV_NOOS_TYPES   Type definitions Macros */
/*\{*/
/** Event type. */
#define SYS_EVENT                  NO_OS_EVENT
/** Semaphore type. */
#define SEM_ID                     NO_OS_EVENT
/** Semaphore type. */
#define OS_MUTEX                   NO_OS_EVENT
/** Timer Element type. */
typedef INT                        TIMER_ELEMENT;
/** Return type for functions */
typedef INT                        STATUS;
/** Timestamp type. */
#define TIMESTAMP                  INT
/** Ioctl command definition Macro. */
#define _IO(magic, nr)             ((magic << 8) | nr)
/** Ioctl write command definition Macro. */
#define _IOW(magic, nr, type)      _IO(magic, nr)
/** Ioctl read command definition Macro. */
#define _IOR(magic, nr, type)      _IO(magic, nr)
#define SYS_NOSELECT               0x00000000
#define SYS_DATAIN                 0x00000001
#define SYS_EXCEPT                 0x00000002
#define SYS_DATAOUT                0x00000004
#define SEM_TYPE                   NO_OS_EVENT
#ifdef TAPI
#define  TAPI_WAKEUP_LIST          NO_OS_EVENT
#endif
/* Polling is used to wakeup an application. */
#define SYS_POLL                   NO_OS_EVENT
/* element in the wake-up list. */
#define SYS_POLL_NODE              NO_OS_EVENT
#define INLINE
/*\}*/

/* synchronize core */
#ifdef PPC
   /* synchronize PPC */
   #define SYNC                    __asm__("	sync")
   /* function to allocate dualported memory (only PPC) */
   #define CPM_Dpalloc(size)       sysGetDPRam(size)
   /* function to allocate uncached memory (only PPC) */
   #define Host_Alloc(size)        cacheDmaMalloc(size)
#endif

#ifdef DEBUG
#define ASSERT(expr)                             \
	if(!(expr))                                   \
	{                                             \
		printf ( "\n\r" __FILE__ ":%d: Assertion " \
		        #expr " failed!\n\r",__LINE__);    \
	}
#else
#define ASSERT(cond)
#endif

/* ============================= */
/* Global variable declaration   */
/* ============================= */


/* ============================= */
/* Global function declaration   */
/* ============================= */

/** \defgroup SYS_DRV_NOOS_FUNC   Exported driver functions */
/*\{*/
/** Initializes the device. */
PUBLIC INT  OS_InitDevice (INT nDevn);
/** Stops the device. */
PUBLIC VOID OS_StopDevice (VOID* pDevice);
/*\}*/

#endif /* _SYS_DRV_NOOS_H */

/*******************************************************************************
   $Log: sys_drv_noOS.h,v $
   Revision 1.31  2004/05/18 10:09:50  tauche
   functions HAL_LockInt() and HAL_UnlockInt() replaced by No_OS_LockInt and No_OS_UnlockInt() implemented in sys_noOSLib.c

   Revision 1.30  2004/03/19 08:07:09  martin
   #if PPC -> #ifdef PPC

   Revision 1.29  2004/03/04 17:58:05  tauche
   no message

   Revision 1.28  2004/02/18 10:53:17  kamdem
   @ replace with \ in doxygen comments

   Revision 1.27  2004/02/18 10:38:19  kamdem
   little comment change

   Revision 1.26  2004/02/09 12:03:11  kamdem
   include adapted

   Revision 1.25  2004/02/04 14:19:19  kamdem
   Improvements in the No OS Library :

   - if the target driver also defines one of the user interface
     macros "open", "close", ..., for any reason,
     -DDONT_USE_NO_OS_PROTOTYPES
      should be added to the compiler defines to avoid compiler
      errors

   - Drivers using this OS - file should be compiled with additional
      compiler switch -DNO_OS

   - More comments added

   Revision 1.24  2004/01/30 21:14:20  tauche
   __BYTE_ORDER has to be undefined, before it can be defined according to the
    noOS settings

   Revision 1.22  2004/01/30 18:49:23  tauche
   no message

   Revision 1.21  2004/01/30 18:04:52  kamdem
   more comments

   Revision 1.19  2004/01/28 17:54:55  kamdem
   Lib.h/c : Interface relevant info hidden in c-file
   Other  : implementation of _IO changed.

   Revision 1.18  2004/01/27 14:55:15  kamdem
   declaration of OS_Install_IRQ_Handler and  OS_UnInstall_IRQ_Handler
   removed due to multiple definitions linker error when compiling more
   than one driver under VxWorks.

   Revision 1.17  2004/01/27 13:13:20  kamdem
   * sys_drv_noOS.h : new defines added for _IOW and _IOR
   * sys_drv_debug.h :  DRV_LOG (x) calls printf
   * sys_drv_noOSLib.c : error corrected

   Revision 1.16  2004/01/27 11:39:09  kamdem
   conflicts solved

   Revision 1.15  2004/01/26 20:21:51  tauche
   typedef for SEM_ID removed (was vxworks specific)
   prototype for Vinetic-specific OS_(Un)Install_IRQ_Handler() removed

   Revision 1.14  2004/01/26 17:30:04  tauche
   Fixed SleepOnEvent() macro

   Revision 1.13  2004/01/23 19:53:53  kamdem
   improvments in the library. Use compiler switch  NON_OS_LITTLE_ENDIAN
   or NON_OS_BIG_ENDIAN to determine your endianess

   Revision 1.12  2004/01/23 12:24:07  kamdem
   new boolean parameter added to No_OS_InitEvent :
   - for semaphores, initialioze with TRUE
   - for events, initialize with FALSE

   Revision 1.7  2004/01/20 12:56:44  kamdem
   Events macros reviewed : Init_Event initializes the queue with one event
   from the event list

   Revision 1.4  2004/01/15 17:04:38  kamdem
   Hardware functions for disable/enable interrupts

   Revision 1.3  2004/01/13 17:02:03  kamdem
   some enhancements done.. NOT yet tested!!!

   Revision 1.2  2004/01/09 19:33:18  kamdem
   * interface library implementation for Non OS driver started. Code still
     under construction

*******************************************************************************/
