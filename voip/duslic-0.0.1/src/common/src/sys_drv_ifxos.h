#ifndef _SYS_DRV_IFXOS_H
#define _SYS_DRV_IFXOS_H
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
   Module      : sys_drv_ifxos.h
   Date        : 2004-04-23
   Description : This file contains the includes and the defines
                 specific to the OS
   Remarks     :
      The driver makes use of the following system resources:
      - malloc and free of memory
      - interrupt functions
      - timers
      - mutex semaphores
      - semaphores or events for internal communication
      - queues/semaphores for external waiting tasks in case of select
        or polling call
      - wait (with scheduling) and delay (without scheduling) functions
      - system time function to retrieve the system time
      - debug print out
      Some features are implemented in a project specific C module, as for
      example timers and interrupt related functions.
*******************************************************************************/
/** \file
SYS_DRV_IFXOS: operating system abstraction
*/

/* ============================= */
/* Global Includes               */
/* ============================= */

/* include operating system specific files */
#ifdef VXWORKS
#include <vxWorks.h>
   #if (CPU_FAMILY==PPC)
   #include <drv/intrCtl/ppc860Intr.h>
   #include <drv/multi/ppc860Siu.h>
   #include <drv/multi/ppc860Cpm.h>
   #include <drv/sio/ppc860Sio.h>
   #include <arch/ppc/ivPpc.h>
   #endif

   #include <drv/timer/timerDev.h>

   #include <selectLib.h>
   #include <iosLib.h>
   #include <stdlib.h>
   #include <stdio.h>
   #include <time.h>
   #include <taskLib.h>
   #include <cacheLib.h>
   #include <logLib.h>
   #include <intLib.h>
   #include <string.h>
   #include <sys/ioctl.h>
   #include <tickLib.h>
   #include <math.h>
   #include <netinet/in.h> /* ntohs(), etc. */
   #include "sys_drv_delaylib_vxworks.h"
#endif /* VXWORKS */
#ifdef LINUX
   #ifdef __KERNEL__
      #include <linux/kernel.h>
   #endif
   #ifdef MODULE
      #include <linux/module.h>
   #endif

   /*several includes*/
   #include <linux/fs.h>
   #include <linux/errno.h>
   /*proc-file system*/
   #include <linux/proc_fs.h>
   /*polling */
   #include <linux/poll.h>
   /* check-request-release region*/
   #include <linux/ioport.h>
   /*ioremap-kmalloc*/
   #include <linux/vmalloc.h>
   /* mdelay - udelay */
   #include <linux/delay.h>
   #include <linux/sched.h>
   #include <linux/param.h>
   #include <linux/interrupt.h>
   #include <asm/uaccess.h>
   #include <asm/io.h>
   #include <asm/semaphore.h>
   /* little / big endian */
   #include <asm/byteorder.h>
   /* Interrupts */
   #include <linux/irq.h>
   #include <linux/list.h>

   /*real time includes */
   #ifdef RTAI
   /* causes warnings, but may it not correct, contact Lineo for
      details */
   extern int set_dec_cpu6(unsigned int val);
   #include <rtai.h>
   #include <rtai_sched.h>
   #include <rtai_fifos.h>
   #include <rtai_shm.h>
   #endif
#endif /* LINUX */
#ifdef WINDOWS
   #include <stdio.h>
   #include <stdlib.h>
   #include <memory.h>
   #include <windows.h>
   #include <winbase.h>
   #include <WinSock.h>
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */

/******************************************************************************
                                 Basics
******************************************************************************/

/** \defgroup SYS_DRV_IFXOS_BASIC  Basic Macros
*/
/*@{*/

/** allocate memory */
#define IFXOS_MALLOC(size)                   malloc (size)
/** free memory */
#define IFXOS_FREE(ptr)                      free ((void*)ptr)

/** For local functions */
#define IFX_LOCAL  static
/* return values */
/** return value for success */
#define IFX_SUCCESS 0
/** return value for error  */
#define IFX_ERR  -1

#ifdef DEBUG
/** assert in debug code
\param expr - expression to be evaluated. If expr != TRUE assert is printed
              out with line number */
#define IFXOS_ASSERT(expr) \
	if(!(expr)) { \
		printk ( "\n\r" __FILE__ ":%d: Assertion " #expr " failed!\n\r",__LINE__); \
	}
#else
/** assert in debug code
\param expr - expression to be evaluated. If expr != TRUE assert is printed
              out with line number */
#define IFXOS_ASSERT(expr)
#endif

/** time stamp type */
typedef int IFXOS_TIMESTAMP;
/*@}*/

/******************************************************************************
                                Hardware related
******************************************************************************/
/** \defgroup SYS_DRV_IFXOS_HW  Hardware Related
 */
/*@{*/

/** Gets the processors base address. Avoid to use! */
#define IFXOS_UC_BASE

#ifdef VXWORKS
   #ifndef __LITTLE_ENDIAN
   #define __LITTLE_ENDIAN _LITTLE_ENDIAN
   #endif
   #ifndef __BIG_ENDIAN
   #define __BIG_ENDIAN    _BIG_ENDIAN
   #endif

   #define IFXOS_LITTLE_ENDIAN  _LITTLE_ENDIAN
   #define IFXOS_BIG_ENDIAN    _BIG_ENDIAN

   #if (_BYTE_ORDER == _LITTLE_ENDIAN)
      /** byte order is little endian */
      #define __BYTE_ORDER __LITTLE_ENDIAN
      #define IFX_BYTE_ORDER IFXOS_LITTLE_ENDIAN
   #elif (_BYTE_ORDER == _BIG_ENDIAN )
      /** byte order is big endian */
      #define __BYTE_ORDER __BIG_ENDIAN
      #define IFX_BYTE_ORDER IFXOS_BIG_ENDIAN
   #else
      #error "Unknown Byteorder!"
   #endif
#endif /* VXWORKS */
#ifdef LINUX
   #define IFXOS_LITTLE_ENDIAN  __LITTLE_ENDIAN
   #define IFXOS_BIG_ENDIAN     __BIG_ENDIAN

   #if defined ( __LITTLE_ENDIAN )
      #define __BYTE_ORDER __LITTLE_ENDIAN
      #define IFX_BYTE_ORDER IFXOS_LITTLE_ENDIAN
   /** byte order is big endian */
   #elif defined ( __BIG_ENDIAN )
      #define __BYTE_ORDER __BIG_ENDIAN
      #define IFX_BYTE_ORDER IFXOS_BIG_ENDIAN
   #endif
#endif /* LINUX */
#ifdef WINDOWS
   #define IFXOS_LITTLE_ENDIAN  3412
   #define IFXOS_BIG_ENDIAN     1234
   #define IFX_BYTE_ORDER IFXOS_LITTLE_ENDIAN
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */
/*@}*/

/** \defgroup SYS_DRV_IFXOS_INT Interrupt Handling
*/
/*@{*/

#ifdef VXWORKS
/** type for interrupt status store used in IFXOS_LOCKINT */
typedef int IFXOS_INTSTAT;
#endif /* VXWORKS */
#ifdef WINDOWS
/** type for interrupt status store used in IFXOS_LOCKINT */
typedef int IFXOS_INTSTAT;
#endif /* WINDOWS */
#ifdef LINUX
/** type for interrupt status store used in IFXOS_LOCKINT */
typedef unsigned long  IFXOS_INTSTAT;
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */

#ifdef VXWORKS
/** Lock interrupt handling
\param var - status variable of type IFXOS_INTSTAT
 */
#define IFXOS_LOCKINT(var) \
   var = intLock()
#endif /* VXWORKS */
#ifdef WINDOWS
#define IFXOS_LOCKINT(var)

#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_LOCKINT(var) \
   save_and_cli(var)
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */

/** Unlock interrupt handling
\param var - interrupt status variable of type IFXOS_INTSTAT
 */
#ifdef VXWORKS
#define IFXOS_UNLOCKINT(var) \
   intUnlock(var)
#endif /* VXWORKS */
#ifdef WINDOWS
#define IFXOS_UNLOCKINT(var)

#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_UNLOCKINT(var) \
   restore_flags(var)
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */


/** Disable interrupts.
\param irq - interrupt number
 */
#ifdef VXWORKS
#define IFXOS_IRQ_DISABLE(irq)      \
   intDisable(IVEC_TO_INUM((irq)))
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_IRQ_DISABLE(irq)      \
   disable_irq((irq))
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_IRQ_DISABLE(irq)
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */


/** Enable interrupts
\param irq - interrupt status variable of type IFXOS_INTSTAT
 */
#ifdef VXWORKS
#define IFXOS_IRQ_ENABLE(irq)      \
   intEnable(IVEC_TO_INUM((irq)))
#endif /* VXWORKS */
#ifdef WINDOWS
#define IFXOS_IRQ_ENABLE(irq)
#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_IRQ_ENABLE(irq)      \
   enable_irq((irq))
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */

/** lock task switching
 */
#ifdef VXWORKS
#define IFXOS_TASK_LOCK() \
   taskLock()
#endif /* VXWORKS */
#ifdef WINDOWS

#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_TASK_LOCK()
   /* nothing */
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */

/** unlock task switching
*/
#ifdef VXWORKS
#define IFXOS_TASK_UNLOCK() \
   taskUnlock()
#endif /* VXWORKS */
#ifdef WINDOWS

#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_TASK_UNLOCK()
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */
/*@}*/


/******************************************************************************
                                 Time related
******************************************************************************/
/** \defgroup SYS_DRV_IFXOS_TIME Time Related
   */
/*@{*/

/** Delay excution with task schedule
\param time - time to wait in ms */
#ifdef VXWORKS
#define IFXOS_Wait(time)                     \
   taskDelay((time==0)?0:               \
   (time <= (1000/sysClkRateGet()))? \
   1:time * sysClkRateGet() / 1000)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_Wait(time)   \
   do { \
		current->state = TASK_INTERRUPTIBLE; \
		schedule_timeout(HZ * (time) / 1000);\
   } while(0)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_Wait(time)   \
   Sleep(time)
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */


/** Short active delay in milli seconds without schedule
\param time - wait time in ms
 */
#ifdef VXWORKS
#define IFXOS_DELAYMS(time)           \
   delayMsec (time)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_DELAYMS(time)           \
   mdelay(time)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_DELAYMS(time)           \
   Sleep(time)
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */


/** Short active delay in micro seconds without schedule
\param time - wait time in us
 */
#ifdef VXWORKS
#define IFXOS_DELAYUS(time)      \
   delayUsec (time)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_DELAYUS(time)      \
   udelay(time)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_DELAYUS(time)      \
   Sleep(time / 1000)
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */
/*@}*/


/******************************************************************************
                     Events for internal driver communication
******************************************************************************/
/** \defgroup SYS_DRV_IFXOS_SYNC Event Macros
\remarks Event macros are used for driver internal commonication. For example
a driver call can wait on an interrupt.
   */
/*@{*/

#ifdef VXWORKS
/** waits forever */
#define IFXOS_WAIT_FOREVER WAIT_FOREVER
/** never wait */
#define IFXOS_NOWAIT       NO_WAIT
#endif /* VXWORKS */
#ifdef LINUX
/** waits forever */
#define IFXOS_WAIT_FOREVER 0xFFFF
/** never wait */
#define IFXOS_NOWAIT       0
#endif /* LINUX */
#ifdef WINDOWS
/** waits forever */
#define IFXOS_WAIT_FOREVER INFINITE
/** never wait */
#define IFXOS_NOWAIT       0
#endif /* WINDOWS */

/** Type of event */
#ifdef VXWORKS
typedef SEM_ID                         IFXOS_event_t;
#endif /* VXWORKS */
#ifdef WINDOWS
typedef HANDLE                         IFXOS_event_t;
#endif /* WINDOWS */
#ifdef LINUX
typedef wait_queue_head_t              IFXOS_event_t;
#endif /* LINUX */
#ifdef OSE
   #warning not defined
#endif /* OSE */

/** Initialize an event. No return
\param ev - event of type IFXOS_event_t
*/
#ifdef IFXOS_NOTDEFINED
#define OS_InitEvent(ev)
#endif /* IFXOS_NOTDEFINED */
#ifdef VXWORKS
#define IFXOS_InitEvent(ev) \
   (ev) = semBCreate(SEM_Q_FIFO, SEM_EMPTY)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_InitEvent(ev) \
   init_waitqueue_head (&(ev))
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_InitEvent(ev)      \
   ev = CreateEvent(NULL, FALSE, NULL, NULL)
#endif /* WINDOWS */
#ifdef OSE
   #warning not defined
#endif /* OSE */


/** Signal an event. After signaling the state is reset
\param ev - event
\return IFX_SUCCESS if success, otherwise IFX_ERR
 */
#ifdef LINUX
#define IFXOS_WakeUpEvent(ev)    \
   IFX_SUCCESS;wake_up_interruptible(&(ev))
#endif /* LINUX */
#ifdef VXWORKS
#define IFXOS_WakeUpEvent(ev)    \
   (semGive ((ev))==OK ? IFX_SUCCESS : IFX_ERR)
#endif /* VXWORKS */
#ifdef WINDOWS
#define IFXOS_WakeUpEvent(ev)    \
   PulseEvent(ev)
#endif /* WINDOWS */

/** Reset event to initial state
\param ev - event of type IFXOS_event_t
 */
#ifdef VXWORKS
#define IFXOS_ClearEvent(ev)     \
   semFlush (ev); \
   semTake ((ev), NO_WAIT)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_ClearEvent(ev)     \
   init_waitqueue_head (&(ev))
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_ClearEvent(ev)     \
   ResetEvent(ev)
#endif /* WINDOWS */

/** Waits for a specified event with a specified timeout
    to occur or timeout.
\param ev - event of type IFXOS_event_t
\param timeout    - time out in ms
\return  IFX_SUCCESS if event occured, otherwise IFX_ERR
*/
#ifdef VXWORKS
#define  IFXOS_WaitEvent_timeout(ev, timeout)          \
   (semTake (ev, (((timeout * sysClkRateGet()) / 1000)+1)) == ERROR ? IFX_ERR : IFX_SUCCESS)
#endif /* VXWORKS */
#ifdef WINDOWS
#define  IFXOS_WaitEvent_timeout(ev, timeout)          \
   (WaitForSingleObject(ev, timeout) == WAIT_OBJECT_0 ? IFX_SUCCESS : IFX_ERR)
#endif /* WINDOWS */
#ifdef LINUX
#define  IFXOS_WaitEvent_timeout(ev, timeout)          \
   (interruptible_sleep_on_timeout(&ev, timeout) == IFX_SUCCESS ? IFX_ERR : IFX_SUCCESS)
#endif /* LINUX */


/** Waits for a specified event with a specified condition
    to occur or timeout.
   \param event        - wait event
   \return  none
*/
#ifdef VXWORKS
#define  IFXOS_WaitEvent(event)          \
   semTake (event, WAIT_FOREVER)
#endif /* VXWORKS */
#ifdef LINUX
#define  IFXOS_WaitEvent(event)          \
   interruptible_sleep_on(event)
#endif /* LINUX */
#ifdef WINDOWS
#define  IFXOS_WaitEvent(event)          \
   WaitForSingleObject(event,INFINITE)
#endif /* WINDOWS */
/*@}*/

/******************************************************************************
               Mutural exclusion for internal driver communication
******************************************************************************/
/** \defgroup SYS_DRV_IFXOS_MUTEX Mutual Exclusion
A mutex is intended for critical sections.
*/

/*@{*/

#ifdef VXWORKS
/** Mutex type.
 */
typedef SEM_ID             IFXOS_mutex_t;
#endif /* VXWORKS */
#ifdef LINUX
typedef struct semaphore*  IFXOS_mutex_t;
#endif /* LINUX */
#ifdef WINDOWS
typedef HANDLE             IFXOS_mutex_t;
#endif /* WINDOWS */

/** initialze mutex
\param mutex - mutex handle
*/
#ifdef VXWORKS
#define IFXOS_MutexInit(mutex)  \
  (mutex) = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE)
#endif /* VXWORKS */
#ifdef WINDOWS
#define IFXOS_MutexInit(mutex) \
   (mutex) = CreateMutex(NULL, FALSE, NULL)
#endif /* WINDOWS */
#ifdef LINUX
#define IFXOS_MutexInit(mutex) \
   (mutex)=kmalloc(sizeof(struct semaphore), GFP_KERNEL); sema_init((mutex), 1)
#endif /* LINUX */

/** lock/take the mutex
\param mutex - mutex handle
*/
#ifdef VXWORKS
#define IFXOS_MutexLock(mutex)     \
   semTake(mutex, WAIT_FOREVER)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_MutexLock(mutex)     \
   down(mutex)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_MutexLock(mutex)     \
   WaitForSingleObject(mutex, INFINITE)
#endif /* WINDOWS */


/** unlock/give the mutex
\param mutex - mutex handle
\return IFX_SUCCESS for success, otherwise IFX_ERR
*/
#ifdef IFXOS_NOTDEFINED
#define IFXOS_MutexUnlock(mutex)
#endif /* IFXOS_NOTDEFINED */
#ifdef VXWORKS
#define IFXOS_MutexUnlock(mutex)   \
   semGive(mutex)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_MutexUnlock(mutex)   \
   up(mutex)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_MutexUnlock(mutex)   \
   ReleaseMutex(mutex)
#endif /* WINDOWS */


/** Delete a mutex element
\param mutex - mutex handle
 */
#ifdef VXWORKS
#define IFXOS_MutexDelete(mutex)      \
   semDelete(mutex); mutex = 0;
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_MutexDelete(mutex)      \
   kfree(mutex); mutex = 0;
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_MutexDelete(mutex)      \
   CloseHandle(mutex); mutex = 0;
#endif /* WINDOWS */
/*@}*/


/******************************************************************************
              driver external communication for select/polling
******************************************************************************/

/** \defgroup SYS_DRV_IFXOS_SELECT  Select and Polling.
\remarks
VxWorks distinguishes between semaphores, events, mutex and
also select wait queues. Linux does not, but anyway we need macros
to support VxWorks and other OS. */

/*@{*/

/** wakeup type for select wait queues */
#ifdef VXWORKS
typedef SEL_WAKEUP_LIST             	IFXOS_wakelist_t;
#endif /* VXWORKS */
#ifdef LINUX
typedef wait_queue_head_t              IFXOS_wakelist_t;
#endif /* LINUX */
#ifdef WINDOWS
typedef HANDLE                       	IFXOS_wakelist_t;
#endif /* WINDOWS */

/** Initialize a queue. */
#ifdef VXWORKS
#define IFXOS_Init_WakeList(queue)           \
   selWakeupListInit(&queue)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_Init_WakeList(queue)           \
   init_waitqueue_head (&(queue))
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_Init_WakeList(queue)
#endif /* WINDOWS */

/** if called initializes the sleep on the given queue.
    This macros returns immediately
\param queue - wait queue handle
\param opt - optional parameter for additional information.
             For VxWorks node parameter, for linux poll table
\param fp - optional file pointer (needed for linux)
     */
#ifdef VXWORKS
#define IFXOS_SleepQueue(queue, opt, fp)    \
   selNodeAdd(&queue, (SEL_WAKEUP_NODE*)opt)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_SleepQueue(queue, opt, fp)    \
   poll_wait((struct file *)fp, &queue, (poll_table *)opt)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_SleepQueue(queue, opt, fp)
#endif /* WINDOWS */

/** Wakes up a waiting queue in select
\param queue - wait queue handle
\param type - type of the queue: IFXOS_READQ or IFXOS_WRITEQ.
              Not needed for linux
*/
#ifdef VXWORKS
#define IFXOS_WakeUp(queue, type)    \
   selWakeupAll(&queue, type)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_WakeUp(queue, type)     \
   wake_up_interruptible(&queue)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_WakeUp(queue, type)       \
   PulseEvent(queue)
#endif /* WINDOWS */

/** definitions for select queues */
#ifdef VXWORKS
/** defines the write queue */
#define IFXOS_WRITEQ           SELWRITE
/** defines the read queue */
#define IFXOS_READQ            SELREAD
/** system event for write ready. Is returned on select to determine
    the kind of event  */
#define  IFXOS_SYSWRITE  0x00000000
/** system event for read ready. Is returned on select to determine
    the kind of event */
#define  IFXOS_SYSREAD   0x00000001
/** system exception, avoid usage, since it is not supported in all operating
    systemss */
#define  IFXOS_SYSEXCEPT IFXOS_SYSREAD
#endif /* VXWORKS */
#ifdef LINUX
/** defines the write queue */
#define IFXOS_WRITEQ           1
/** defines the read queue */
#define IFXOS_READQ            2
/** system event for write ready. Is returned on select to determine
    the kind of event  */
#define  IFXOS_SYSWRITE  POLLOUT
/** system event for read ready. Is returned on select to determine
    the kind of event */
#define  IFXOS_SYSREAD   POLLIN
/** system exception, avoid usage, since it is not supported in all operating
    systemss */
#define  IFXOS_SYSEXCEPT POLLPRI
#endif /* LINUX */
#ifdef WINDOWS
/** defines the write queue */
#define IFXOS_WRITEQ           0
/** defines the read queue */
#define IFXOS_READQ            0
/** system event for write ready. Is returned on select to determine
    the kind of event  */
#define  IFXOS_SYSWRITE  0x00000000
/** system event for read ready. Is returned on select to determine
    the kind of event */
#define  IFXOS_SYSREAD   0x00000001
/** system exception, avoid usage, since it is not supported in all operating
    systemss */
#define  IFXOS_SYSEXCEPT IFXOS_SYSREAD
#endif /* WINDOWS */
/*@}*/


/** \defgroup SYS_DRV_IFXOS_MISC Misc Macros and Function Declarations
 */
/*@{*/

/** copy from user to kernel space
   \note Different return codes! */
#ifdef VXWORKS
#define IFXOS_CPY_USR2KERN(to, from, n)       \
   memcpy((void *)to, (const void *)from, n)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_CPY_USR2KERN(to, from, n)       \
   copy_from_user((void *)to, (const void *)from, n)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_CPY_USR2KERN(to, from, n)       \
   memcpy(to, from, n)
#endif /* WINDOWS */

/** copy from kernel to user space
   \note Different return codes! */
#ifdef VXWORKS
#define IFXOS_CPY_KERN2USR(to, from, n)       \
   memcpy((void *)to, (const void *)from, n)
#endif /* VXWORKS */
#ifdef LINUX
#define IFXOS_CPY_KERN2USR(to, from, n)       \
   copy_to_user((void *)to, (const void *)from, n)
#endif /* LINUX */
#ifdef WINDOWS
#define IFXOS_CPY_KERN2USR(to, from, n)       \
   memcpy(to, from, n)
#endif /* WINDOWS */

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/** Device Init
\remarks Called form Board_DevOpen when the device has been opened for
   the first time. The device structure will be cached
\param nDevn - device number
 */
extern int IFXOS_InitDevice (int nDevn);
/** Device is stopped. Called on device close
\param pDevice - device pointer
*/
extern void IFXOS_StopDevice (void* pDevice);
/** Returns the processor time in milliseconds */
extern unsigned long IFXOS_GetTime(void);
/** Install interrupt service routine
\param pDevice - device pointer
\param nIrq - irq number
  */
extern void IFXOS_InstallIrq (void* pDevice, int nIrq);
/** Uninstall interrupt service routine
\param pDevice - device pointer
 */
extern void IFXOS_RemoveIrq  (void* pDevice);
/*@}*/

#endif /* _SYS_DRV_IFXOS_H */

