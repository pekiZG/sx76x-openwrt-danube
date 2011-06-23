#ifdef VIN_POLL
/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY
   THIRD PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY
   INTELLECTUAL PROPERTY INFRINGEMENT.

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************/
/**
      \file drv_vinetic_polled.c Vinetic Driver Polling Interface implementation
      This file implements the complete vinetic polling interface including
      - poll control which configures the driver for polling mode
      - events polling
      - data upstream/downstream (see buffer management description).
   \remarks
   Open Issues : - assumption Up / Downstream are never called simultanously
                   or interrupt each other
                 - assumption _all_ interrupts are disabled, pure polled
                   device accesses (!)
*/

/**
  \addtogroup VIN_DRIVER_POLLING_INTERFACE VINETIC

  \section a How to use the polling interface ?

  \subsection aa Programming steps

   1- The client implements a tick generator with highest priority according
      to the sollicited polling period. Priority should be given in regard to
      the OS used.

   \code
   // set tick task
   static int setTickPeriod (int period)
   {

      sTaktSem = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
      if (sTaktSem == NULL)
         return (-1);
      tTickId = taskSpawn ("tsk_Tick", PRIORITY_HIGHEST, 0, TICKTASK_STACK_SIZE,
                           (FUNCPTR)tsk_Tick, sTaktSem, 0,0,0,0,0,0,0,0,0);
      if (tTickId == -1)
         return (-1);
      sysClkRateSet(1000/period);

      return (0);
   }
   // tick task
   void tsk_Tick(int tick)
   {
      while (!idlePoll)
      {
         taskDelay (ONE_TICK);
         semGive((SEM_ID)tick);
      }
   }
   \endcode

   2- The client implements a polling task with high priority for data and
     events polling, whereby polling task and tick task are synchronised for
     example with a signalling semaphore. Priority should be given in regard to
     the OS used\n

   \code
   // polling task
   void tsk_Poll (int mode, int period, int evt)
   {
      int nRet = 0, bufElementNum, aFd [MAX_CHAN_SUPPORTED] = {};
      void *bufPool [100];

      nRet = InitChans (aFd, mode);
      if (nRet == -1)
      {
         idlePoll = IFX_TRUE;
      }
      else
      {
         bufElementNum = 0;
         idlePoll = IFX_FALSE;
         nRet = setTickPeriod (period);
      }
      while (!idlePoll)
      {
         // take signalling semaphore
         semTake (sTaktSem, WAIT_FOREVER);
         // poll data
         dataHandling (&bufElementNum, bufPool);
         // poll events
         eventHandling (aFd, evt);
      }
      cleanupPoll (aFd);
   }
   \endcode

   3- The client configures the driver via provided interface by

      - setting polling mode
      - setting the buffer pool pointer
      - setting the buffer handlers

      The client can also decide to use interleave or normal polling by setting
      some functions pointers in the run time which map the driver normal or
      interleave functions provided.

   \code
   // data polling functions types
   typedef int (*PollUpStream)  (int *bufElementNum, void **bufPool);
   typedef int (*PollDownStream)(const int bufElementNum, void **bufPool);

   // data polling functions pointers
   static PollUpStream      doPollUp;
   static PollDownStream    doPollDown;

   //main function
   int startPoll (bool normal_polling)
   {
      int ret = -1, gFd0;

      gFd0  = open("/dev/vin10", O_RDWR, 0644);
      if (gFd0 <= 0)
         return -1
      memset (&vinPoll, 0, sizeof (VINETIC_IO_DRVCTRL));
      // configure driver for polling mode
      vinPoll.bPoll = IFX_TRUE;
      // driver should report events via poll/select
      vinPoll.bEvtSel = IFX_TRUE;
      // set driver buffer pool pointer
      vinPoll.pBufferPool = (void*)bufferPoolInit (sizeof(PACKET), 250, 0);
      // set buffer get function ptr
      vinPoll.getBuf = bufferPoolGet;
      // set buffer put function ptr
      vinPoll.putBuf = bufferPoolPut;
      // configure driver via ioctl with configuration device fd
      ret = ioctl (gFd0, FIO_VINETIC_DRV_CTRL, (int)&vinPoll);
      if (ret == -1)
         return -1
      // set function ptr for interleaved or normal polling
      if (!normal_polling)
      {
         doPollUp   = VINETIC_Poll_UpIntlv;
         doPollDown = VINETIC_Poll_DownIntlv;
         printf ("VINETIC interleaved access\n\r");
      }
      else
      {
         doPollUp   = VINETIC_Poll_Up;
         doPollDown = VINETIC_Poll_Down;
         printf ("VINETIC normal polling access\n\r");
      }
      // start polling task
      tPollId = taskSpawn ("tsk_Poll", PRIORITY_HIGH, 0, POLLTASK_STACK_SIZE,
                           (FUNCPTR)tsk_Poll, mode, period,evt,0,0,0,0,0,0,0);
      if (tPollId == -1)
         ret = -1;
   }
   \endcode

  \subsection ab About Data handling

   The high priority polling task does the calls in the following order in a
   while loop:
      -# data downstream
      -# data upstream
      -# channel unmapping

   The code could look like this :

   \code
   // data handling function called by the high priority polling task
   static int dataHandling (int *num, void **pool)
   {
      // downstream
      doPollDown (*num,  pool);
      // upstream
      doPollUp   (num, pool);
      // set appropriate channels for downstream
      SET_CHAN(*num, pool, next2nextChMap);

      return (0);
   }
   \endcode

  \subsection ac About event handling

   1- The client can handle events in the polling task which is here high prior
      like depicted in the image \image html Polling_HPT_Only.emf


      The code could look like this :

      \code
      // event handling function called by high prior polling task
      static int eventHandling (int *aFd, int evt)
      {
         TAPI_PHONE_STATUS events;
         int i, nHook;

         // poll events
         ioctl (gFd0, FIO_VINETIC_POLL_EVT, 0);
         // handle events
         for (i = 0; i < MAX_CHAN_SUPPORTED; i++)
         {
            events.channels = 0;
            ioctl (aFd [i], IFXPHONE_PHONE_STATUS, (int)&events);
            if (events.hook & TAPI_HOOKSTATUS_HOOK)
            {
              ioctl(aFd [i], PHONE_HOOKSTATE, (int)&nHook);
              printf ("ch%d -> HookState: %s\n\r", i, ((nHook == 0) ?
                "ONHOOK" : "OFFHOOK"));
            }
            if (events.hook & TAPI_HOOKSTATUS_FLASH)
            {
              printf ("ch%d -> Flashhook detected \n\r", i);
            }
            if (events.line & TAPI_LINESTATUS_GNDKEY)
            {
              printf ("ch%d -> ground key detected \n\r", i);
            }
            if (events.dialing & TAPI_DIALINGSTATUS_DTMF)
            {
              if (events.digit == 0xb)
                 events.digit = 0;
              printf ("ch%d -> DTMF Digit: %d \n\r", i, events.digit);
            }
         }
         return (0);
      }
      \endcode

   2- The client code can implement a low priority task which handles events and
      do other jobs like signalling or RTCP statistics reading like depicted in
      the image \image html Polling_HPT_LPT.emf


      In this case, the high prior polling task will just poll the events as
      follows :

      \code
      // event handling function called by high prior polling task
      static int eventHandling (int *aFd, int evt)
      {
         // poll events
         ioctl (gFd0, FIO_VINETIC_POLL_EVT, 0);
      }
      \endcode

  \section b Buffer Management for Data Polling Up/Downstream

  \subsection ba Generalities

  For efficient packet handling a buffer pool provides buffers in upstream
  direction and takes them back after the packets were sent to the Vinetic
  in downstream direction.

  The bufferpool has to be initialized by the application before the
  polling is started. The number of buffer elements has to be calculated
  according to the application (number of channels/codec etc.).

  All data polling functions receive an array of pointers as one
  of their arguments (ppBuffers). In upstream direction buffers are taken
  from the bufferpool, filled, added to the array of pointers (ppBuffers)
  and the counter nElements is increased accordingly.
  In downstream direction the array of buffer pointers has to be filled
  by the application. All pointers unequal NULL are interpreted as valid
  buffers, sent to the Vinetic and returned to the bufferpool. nElements
  does not count NULL pointers.

  Internally ppCurBuf is used to access the pointers in the array while
  ppBuffers does always point to the beginning of the array.

  The pointers used to access the array are shown here:

   \code

                 ppBuffers ----> +----+
                             |   | p1 + ----->  some buffer
                             +-> +----+
                 ppCurBuf  --|   | p2 | ----->  some other buffer
                             +-> +----+
                                  ...
   \endcode

  The polling routine support two different formats for the first 16-bit word.
  If the 'VIN_POLL_CODER' is set to '4', the following format is used:

   \code

      15                   8 7             2 1   0
      +---------------------+--------------------+
      |      ignored        |      dev      | ch |  word 1
      +---------------------+--------------------+
      dev: device number (starting @ 0)
      ch : channel number (starting @ 0)
   \endcode

   If the macro 'VIN_POLL_CODER' is set to a different value than '4', the
   following format for the first 16-bit word is used:

   \code

      15                   8 7                   0
      +---------------------+--------------------+
      |         dev         |         ch         |  word 1
      +---------------------+--------------------+
      dev: device number (starting @ 0)
      ch : channel number (starting @ 0)
   \endcode

   The following picture shows the bit fields of the second 16-bit word.

   \code

      15    13             8 7                   0
      +---------------------+--------------------+
      |   |odd| ignored     |        len         |  word 2
      +---------------------+--------------------+
      odd: this bit is set if the last payload byte is padding
      len: number of payload 16bit words
   \endcode

  \subsection bb Data Downstream (normal/interleave)

   Function sends packets the the VINETIC mailbox. An array of pointers
   provides the reference to the packets (ppBuffers). nElements is the
   number of valid pointers (packets) to be sent. NULL pointers in the
   array are allowed and will be ignored (i.e. NULL pointer do not count
   as packets in nElements).
   The packets are written to the Vinetic

      - in packet by packet for normal polling or
      - in an interleaved way (if more than one chip has active channels)

   After sending the packets to the VINETIC mailbox, the buffers will be
   returned to the bufferpool.
   The application is responsible to populate the cmd2 header fields such
   as packet length and odd bit!

   Following buffer usage description applies :

   \code

                pCurPkt   ---> +------------------+  packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
                pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+
   \endcode

  \subsection bc Data Upstream (normal/interleave)

   Function reads the amount of data indicated in OBXML interleaved for all
   VINETIC devices. The buffer is taken from the bufferPool (provided by the
   calling task). If no buffer is available or the mailbox is corrupted and
   has to be totally emptied the read data is dumped to the local ndevNull.

   Following buffer usage description applies :

   \code

          pDev->pCurPkt   ---> +------------------+  pDev->packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
          pDev->pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+
   \endcode

  \section c Protection against concurrent access

   When using the driver in polling mode, several tasks will be started by the
   client :

      - a polling task handling events and data , running in a high priority.
      - some other tasks taking care of signalling a.s.o

   So, the driver provides a general protection against concurrent access,
   whereby the main attention was payed to mailbox access and shared data
   (variables, fifos)

  \subsection ca Mailbox Access
   Each device has a mutex semaphore (called mbxAccess) to protect itself
   against concurrent mailbox access.

   - In case of Data handling (up/Downstream) in polling mode, the device
     mutexes are locked for all devices available at first before any other
     handling.
     When the function finishes its job or an error occurs (device error),
     the mutexes are released and low priority tasks can now access the mailbox.
     \see LOCK_ALL_DEVICES
     \see UNLOCK_ALL_DEVICES

   - In case of events polling (also done by the high priority polling task),
     each device locks its mutex, polls the events and unlocks the mutex
     \see VINETIC_Poll_Events

   - The lower priority tasks accessing the command mailbox (register/firmware
     messages) also lock / unlock the device mutexes to be protected against
     high priority task (preemptive RTOS), whereby the locking/unlocking is
     very short, so not to block the high priority task for too long.

  \subsection cb Shared data
   To protect concurrent access against shared variables, a lock/unlock
   of another mutex is done within some ioctls.

  \section d Multi Vinetic-Type Support
   The polling routines supports to poll some devices and leave other devices
   out. For the default configuration, the driver only polls the Vinetic VIP
   devices, after the TAPI driver has notified the device type.
   In order to add one device to the list, the client application has to call
   the ioctl-command 'FIO_VINETIC_POLL_DEV_ADD'.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_int.h"
#include "drv_vinetic_parallel.h"
#include "drv_vinetic_polled.h"
#include "drv_vinetic_vxworks.h"
#include "lib_fifo.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#ifndef VIN_POLL_CODER
   /** If the define 'VIN_POLL_CODER' is not set by the compile environment, a
       default will be used. */
   #define VIN_POLL_CODER 4
#endif /* #ifndef VIN_POLL_CODER */

#if (VIN_POLL_CODER == 4)
   /** This interface can be used if ony 4 coder channels are used inside the
       voice streaming */
   #define VIN_POLL_CH_BITS 2
#else /* #if (VIN_POLL_CODER == 4) */
   /** This interface has to be used if more than 4 coder channels are used
       for voice streaming. */
   #define VIN_POLL_CH_BITS 8
#endif /* #else #if (VIN_POLL_CODER == 4) */

/* define the bit mask to filter out the coder channel bits */
#define VIN_POLL_HDR1_CH         ((1 << VIN_POLL_CH_BITS) - 1)

/** loop all devices */
#define FOR_ALL_DEVICES  \
   for (devNo = 0; devNo < vinPollStruct.numCurDevsPoll; devNo++)

/** Mailbox protection Lock during data polling */
#define LOCK_ALL_DEVICES \
do\
{\
  for (devNo = 0; devNo < vinPollStruct.numCurDevsPoll; devNo++) \
  {\
   IFXOS_MutexLock ((vinPollStruct.pDevicePoll[devNo])->mbxAcc); \
  }\
} while (0)

/** Mailbox protection Release after end of data polling or error */
#define UNLOCK_ALL_DEVICES \
do\
{\
  for (devNo = 0; devNo < vinPollStruct.numCurDevsPoll; devNo++) \
  {\
   IFXOS_MutexUnlock ((vinPollStruct.pDevicePoll[devNo])->mbxAcc); \
  }\
} while (0)

typedef struct _VINETIC_INTERLEAVED
{
   IFX_int32_t                   packetLen;
   IFX_uint32_t                  mbxSize;
   PACKET                       *pCurPkt;
   IFX_uint16_t                 *pCurPktPos;
} VINETIC_INTERLEAVED;


/** states of the interleaved state machine */
enum
{
   /** initial state : no data yet read */
      STREAM_INTERLEAVED_INIT = -3,
   /** intermediate state : command 1 just read (contains channel info)*/
      STREAM_INTERLEAVED_CMD1 = -2,
   /** intermediate state : command 2 just read (contains length info) */
      STREAM_INTERLEAVED_CMD2 = -1
};

typedef struct _VINETIC_POLL_T
{
   /* current number of devices to support */
   IFX_uint32_t                  numCurDevsPoll;

   /* current number of devices to support */
   IFX_uint32_t                  numCurDevsEvents;

   /* driver in polling/interrupt mode flag.
      Atomic variable, must be keep in memory because it
      can be modified in an asynchronous manner by some high priority tasks. */
   IFX_uint32_t                        IrqPollMode;

   /* buffer handler functions ptr */
   void *           pbp;  /* pointer to buffer pool */
   void *        (* get) (void *pBufferPool);
   int           (* put) (void *pBuffer);

   VINETIC_DEVICE*               pDevicePoll[VINETIC_MAX_DEVICES];
   VINETIC_DEVICE*               pDeviceEvents[VINETIC_MAX_DEVICES];

}VINETIC_POLL_S;

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
static int VINETIC_IrqPollDevConf(VINETIC_DEVICE *pDev);
static int VINETIC_IrgPollEventConf(VINETIC_DEVICE *pDev);

/* ============================= */
/* Local variable definition     */
/* ============================= */
IFX_LOCAL VINETIC_POLL_S vinPollStruct = {0};
/* ============================= */
/* Local function definition     */
/* ============================= */

/******************************************************************************/
/**
   Configure the VINETIC-4VIP device for the polling scenario or back to
   interrupt mode. All messages are sent to the device in order to configure
   the event mask bits.
   \param
     - pDev:   Device handle
   \return
      - ERROR if error
      - OK if successful
   \remarks
*******************************************************************************/
static int VINETIC_IrqPollDevConf(VINETIC_DEVICE *pDev)
{
   if (pDev->IrqPollMode & VIN_VOICE_POLL)
   {
      /* Mask COBX and POBX as they aren't needed for polling and will
      increase the processing time of the event handling */
      if (RegModify (pDev, CMD1_IOP, MRBXSR2, BXSR2_POBX_DATA,
         BXSR2_POBX_DATA) != IFX_SUCCESS)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("IFX_ERROR: Error while masking "
            "COBX and POBX interrupts on dev %ld\n\r", pDev->nDevNr));
         return IFX_ERROR;
      }

      if (RegModify (pDev, CMD1_IOP, MFBXSR2, BXSR2_POBX_DATA,
         BXSR2_POBX_DATA) != IFX_SUCCESS)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("IFX_ERROR: Error while masking "
            "packet mailbox interrupts on dev %ld\n\r", pDev->nDevNr));
         return IFX_ERROR;
      }
   }
   else
   {
      /* Unmask POBX raising edge interrupts */
      if (RegModify (pDev, CMD1_IOP, MRBXSR2, BXSR2_POBX_DATA, 0)
         != IFX_SUCCESS)
      {
         TRACE (VINETIC, DBG_LEVEL_HIGH, ("IFX_ERROR: Error while masking "
            "packet mailbox interrupts on dev %ld\n\r", pDev->nDevNr));
         return IFX_ERROR;
      }
   }

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Enables/Disables the device specific interrupts for that device depending
   on the different event poll configuration.

   \param
     - pDev:   Device handle
   \return
      - ERROR if error
      - OK if successful
   \remarks
*******************************************************************************/
static int VINETIC_IrgPollEventConf(VINETIC_DEVICE *pDev)
{
   if (!(pDev->IrqPollMode & VIN_VOICE_POLL) ||
       !(pDev->IrqPollMode & VIN_EVENT_POLL))
   {
      /* voice packets or the device events are not running in polling mode.
         Interrupts could be enabled on the driver if an interrupt
         is registered */

      Vinetic_IrqUnlockDevice(pDev);
   }
   else
   {
      /* voice packets and the device events are running in polling mode.
         Interrupts could be disabled on the driver if an interrupt
         is registered */

      Vinetic_IrqLockDevice(pDev);
      Vinetic_IrqUnlockGlobal(pDev);
   }

   return IFX_SUCCESS;
}

/* ============================= */
/* Global function declaration   */
/* ============================= */


/******************************************************************************/
/**
   This function adds a device structure pointer to the poll list. The poll list
   contains the device handle that are polled for voice packets.
   This function allows to differentiate between Vinetic with Coder-Modules and
   Vinetic device without Coder-Modules.
   This function is called inside the TAPI during initialization of a TAPI
   channel.

   The client application can manually add a device by using the IOCTL-command
   'FIO_VINETIC_POLL_DEV_ADD'.

   \param
     -
   \return
      - ERROR if error
      - OK if successful
   \remarks
      This feature is only available if the compiler flag 'VIN_POLL' is set.
      It is required that the IOCTL-command 'FIO_VINETIC_DRV_CTRL' is called
      before 'IFXPHONE_INIT'.
*******************************************************************************/
int VINETIC_IrqPollDevAddPoll(VINETIC_DEVICE *pDev)
{
   IFX_uint32_t i;

   /* Search for an empty entry in the device pointer array.
      If the device is already set, return without an error */
   for ( i = 0; i < VINETIC_MAX_DEVICES; i++)
   {
      VINETIC_DEVICE** ptr = &(vinPollStruct.pDevicePoll[i]);

      if (*ptr == NULL)
      {
         /* empty entry found -> use it */
         *ptr = pDev;
         /* increment the number of current supported devices */
         vinPollStruct.numCurDevsPoll++;

         break;
      }
      else if (*ptr == pDev)
      {
         /* device is already configured to be used for voice streaming
            -> return without an error */
         break;
      }
   }

   if (i == VINETIC_MAX_DEVICES)
   {
      /* number of added devices has reached the allocated maximum */
      TRACE (VINETIC, DBG_LEVEL_HIGH,
         ("IFX_ERROR: Could not add device %ld to voice streaming polling",
         pDev->nDevNr));
      return IFX_ERROR;
   }

   return VINETIC_IrqPollDevConf(pDev);
}

/******************************************************************************/
/**
   This function adds a device structure pointer to the event configure list.
   This function is called inside the TAPI during initialization of a TAPI
   channel.

   \param
    pDev - device handle
   \return
      - ERROR if error
      - OK if successful
   \remarks
      This feature is only available if the compiler flag 'VIN_POLL' is set.
      It is required that the IOCTL-command 'FIO_VINETIC_DRV_CTRL' is called
      before 'IFXPHONE_INIT'.
*******************************************************************************/
int VINETIC_IrqPollDevAddEvent(VINETIC_DEVICE *pDev)
{
   IFX_uint32_t i;

   /* Search for an empty entry in the device pointer array.
      If the device is already set, return without an error */
   for ( i = 0; i < VINETIC_MAX_DEVICES; i++)
   {
      VINETIC_DEVICE** ptr = &(vinPollStruct.pDeviceEvents[i]);

      if (*ptr == NULL)
      {
         /* empty entry found -> use it */
         *ptr = pDev;
         /* increment the number of current supported devices */
         vinPollStruct.numCurDevsEvents++;

         break;
      }
      else if (*ptr == pDev)
      {
         /* device is already configured to be used for voice streaming
            -> return without an error */
         break;
      }
   }

   if (i == VINETIC_MAX_DEVICES)
   {
      /* number of added devices has reached the allocated maximum */
      TRACE (VINETIC, DBG_LEVEL_HIGH,
         ("IFX_ERROR: Could not add device %ld to voice streaming polling",
         pDev->nDevNr));
      return IFX_ERROR;
   }

   return VINETIC_IrgPollEventConf(pDev);
}


/******************************************************************************/
/**
   Switch between Polling and Interrupt mode

   \param    pCtrl - handle to VINETIC_IO_DRVCTRL structure

   \return IFX_SUCCESS or IFX_ERROR.

   \remark
      This function will be called once with any device file descriptor to
      configure all devices intoi polling mode according to
      VINETIC_MAX_DEVICES and assumes that all devices structures were
      initialized already : All devices must be opened before calling this
      function.
      In case of a switching to polling mode (pCtrl->bPoll = IFX_TRUE):
         - the buffer pool pointers are set once
         - the switch variable bIsPoll is set to IFX_TRUE for all devices
         - interrupt lines are disabled on uC side for all devices.
         - event reporting (poll/select) is switch according to client config
           (TRUE/FALSE)
         - packet mailbox interrupts POBX are masked on each vinetic device
      In case of a switch to interrupt mode (pCtrl->bPoll = IFX_FALSE)
         - device internal polling control variables are reseted
         - interrupts a enabled back
         - packet mailbox interrupts are enabled back.
   \remark
      In case of a switching to interrupt mode, buffer pool is freed and
      interrupt lines are enabled on uC side for all devices
*/
/******************************************************************************/
int VINETIC_IrqPollConf (VINETIC_IO_DRVCTRL *pCtrl)
{
   /** report events mode
       \arg 0 event read by application
       \arg 1 event read via poll/select
   */
   vinPollStruct.IrqPollMode = (pCtrl->bEvtSel == FALSE)? 0: VIN_TAPI_WAKEUP;
   /** packets control
       \arg 0 packets read by normal/interleave polling routines
       \arg 1 packets read on event polling
   */
   vinPollStruct.IrqPollMode |= (pCtrl->bDataEvt == FALSE)? VIN_VOICE_POLL: 0;
   /** driver polling/interrupt mode control flag
       \arg 0 configure driver for interrupt mode
       \arg 1 configure driver for polling   mode
   */
   vinPollStruct.IrqPollMode |= (pCtrl->bPoll == FALSE)? 0: VIN_EVENT_POLL;

   /* set polling mode */
   if (vinPollStruct.IrqPollMode & VIN_VOICE_POLL)
   {
      /* initialize the buffer pool related pointers */
      vinPollStruct.pbp = pCtrl->pBufferPool;
      vinPollStruct.get = pCtrl->getBuf;
      vinPollStruct.put = pCtrl->putBuf;
   }

   /* update the device event poll structure of all devices and
      enable/disable the interrupts accordingly */
   {
      IFX_uint32_t i;

      /* set driver into polling mode for all channels */
      for (i = 0; i < vinPollStruct.numCurDevsEvents; i++)
      {
         VINETIC_DEVICE *pTmpDev = vinPollStruct.pDeviceEvents[i];

         pTmpDev->IrqPollMode = vinPollStruct.IrqPollMode;

         if (VINETIC_IrgPollEventConf(pTmpDev) != IFX_SUCCESS)
            return IFX_ERROR;
      }
   }

   /* update the device voice poll structure of all devices and
      mask the voice events accordingly */
      /* dynamic number of devices is not supported.
         Call the device init-routine for each device here */
   {
      IFX_uint32_t i;

      /* set driver into polling mode for all channels */
      for (i = 0; i < vinPollStruct.numCurDevsPoll; i++)
      {
         VINETIC_DEVICE *pTmpDev = vinPollStruct.pDevicePoll[i];

         if (VINETIC_IrqPollDevConf(pTmpDev) != IFX_SUCCESS)
            return IFX_ERROR;
      }
   }

   return IFX_SUCCESS;
}


/******************************************************************************/
/**
   Reads interrupt status registers and updates events for each device

   \remark
   This function is called by a client who has disabled interrupts and is to
   call periodically to read the VINETIC status registers.
   \remark
   Mailbox access and shared variables are proctected against
   concurent access / tasks
*/
void  VINETIC_Poll_Events (void)
{
   IFX_int32_t      devNo;
   VINETIC_DEVICE  *pDev;

   for (devNo = 0; devNo < VINETIC_MAX_DEVICES; devNo++)
   {
      if (OS_GetDevice (devNo, &pDev) == IFX_ERROR)
         continue;
      /* protect possible mailbox access and
         shared data against concurent tasks */
      IFXOS_MutexLock (pDev->mbxAcc);
      /* process events on this device */
      VINETIC_interrupt_routine(pDev);
      /* release mailbox access protection for other tasks */
      IFXOS_MutexUnlock (pDev->mbxAcc);
   }
}

/******************************************************************************/
/**
   Send packets to the Vinetic mailbox (non interleaved mode)

   \param   nElements   - number of valid buffer pointers in the array
   \param   **ppBuffers - pointer to the array of buffer pointers

   \return  IFX_SUCCESS or IFX_ERROR

   \remarks
   Mailbox access is proctected against concurent access / tasks

   Function sends packets the the VINETIC mailbox. An array of pointers
   provides the reference to the packets (ppBuffers). nElements is the
   number of valid pointers (packets) to be sent. NULL pointers in the
   array are allowed and will be ignored (i.e. NULL pointer do not count
   as packets in nElements).
   The packets are written to the Vinetic in packet by packet. After sending
   the packets to the VINETIC mailbox, the buffers will be returned to
   the bufferpool.
   The application is responsible to populate the cmd2 header fields such
   as packet length and odd bit!


                pCurPkt   ---> +------------------+  packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
                pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+
*******************************************************************************/
int VINETIC_Poll_Down (const int nElements, void **ppBuffers)
{
   IFX_vuint16_t       *pIr;
   IFX_vuint16_t       *pNwd;
   VINETIC_DEVICE      *pDev;
   IFX_void_t         **ppCurBuf          = ppBuffers;
   PACKET              *pCurPkt           = *ppCurBuf;
   IFX_uint16_t        *pCurPktPos;
   IFX_int32_t          packetLen;
   IFX_uint32_t         devNo             = 0;
   IFX_uint32_t         ch;
   IFX_uint16_t         nFibxms;
#ifdef VIN_V14_SUPPORT
   IFX_int32_t          cnt;
#endif /* VIN_V14_SUPPORT */
   IFX_int32_t          countDown         = nElements;
   FIFO_ID             *fifoId            = NULL;

   if (nElements == 0)
      return IFX_SUCCESS;

   LOCK_ALL_DEVICES;

   /* as we need only one fifo, we use the first available one */
   for ( devNo = 0; devNo < vinPollStruct.numCurDevsPoll; devNo++)
   {
      VINETIC_DEVICE *devfifo = vinPollStruct.pDevicePoll[devNo];

      if (devfifo->pFifo != NULL)
      {
         fifoId = (FIFO_ID*)devfifo->pFifo;
         break;
      }
   }

   /* Send <nElements> pBuffers */
   for (;;)
   {
      /* While pointer is NULL, go to the next one */
      while (pCurPkt == NULL)
      {

         pCurPkt = *(++ppCurBuf);
      }

      /* Analyse Packet Header */
      /* the lowerest 8 bits describe the Coder-channel */
      ch  = (IFX_uint32_t)(pCurPkt->cmd1 & VIN_POLL_HDR1_CH);

      devNo     = (IFX_uint32_t) (pCurPkt->cmd1 >> VIN_POLL_CH_BITS);

      packetLen = (pCurPkt->cmd2 & VIN_BUF_HDR2_LEN) + CMD_HEADER_CNT;

      /* get current device pointer */
      if (devNo > vinPollStruct.numCurDevsPoll)
         continue;

      pDev = vinPollStruct.pDevicePoll[devNo];

      /* Check Packet Inbox Space and queue packet if not enough space */
      if (SC_READ (SC_RFIBXMS, &nFibxms, 1) != IFX_SUCCESS)
         return IFX_ERROR;

      nFibxms &= FIBXMS_PBOX;
      if (nFibxms >= packetLen)
      {
         /* Modify Packet Header: add VOP command and channel number */
         pCurPkt->cmd1    = CMD1_WR | CMD1_VOP | (ch & VIN_POLL_HDR1_CH);

         pIr              = (IFX_uint16_t *) pDev->pIntReg;
         pNwd             = pDev->pNwd;
         pCurPktPos       = &(pCurPkt->cmd1);

         /* Write Cmd Header and Voice data to the Vinetic */
         WAIT_FOR_READY_FLAG;                                        /* CMD 1 */
         *pNwd                                = *(pCurPktPos++);
         packetLen--;

         WAIT_FOR_READY_FLAG;                                        /* CMD 2 */
         *pNwd                                = *(pCurPktPos++);
         packetLen--;

         while(packetLen > 1)                                         /* data */
         {
            WAIT_FOR_READY_FLAG;
            *pNwd                             = *(pCurPktPos++);
            packetLen--;
         }

         WAIT_FOR_READY_FLAG;                                    /* last word */
         *(pDev->pEom)                        = *(pCurPktPos++);

         /* Return the buffer */
         if (vinPollStruct.put(pCurPkt) != IFX_SUCCESS)
         {
            UNLOCK_ALL_DEVICES;
            return IFX_ERROR;
         }
      }
      else
      {
         /* Queue packet for later processing */
         fifoPut(fifoId, pCurPkt, packetLen);
      }
      /* Get the next packet */
      if (countDown != 1)
      {
         --countDown;
         pCurPkt = *(++ppCurBuf);
      }
      else
      {
         pCurPkt = fifoGet(fifoId, (int*)&packetLen);
         /* if the last packet was sent ... leave */
         if (pCurPkt == NULL)
            break;
      }
   }
   UNLOCK_ALL_DEVICES;
   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   UNLOCK_ALL_DEVICES;
   TRACE (VINETIC, DBG_LEVEL_HIGH, ("\nERR ACCESS IFX_ERROR Down\n"));
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/******************************************************************************/
/**
   Serve the mailbox data event (non interleaved mode)

   \param   *pElements  - number of valid buffer pointers in the
                          array (filled by driver)
   \param   **ppBuffers - pointer to the array of buffer pointers

   \return  IFX_SUCCESS or IFX_ERROR

   \remarks:

   Mailbox access is proctected against concurent access / tasks

   Function reads the amount of data indicated in OBXML interleaved for all
   VINETIC devices. The buffer is taken from the bufferPool (provided by the
   calling task). If no buffer is available or the mailbox is corrupted and
   has to be totally emptied the read data is dumped to the local ndevNull.

          pDev->pCurPkt   ---> +------------------+  pDev->packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
          pDev->pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+

*******************************************************************************/
int VINETIC_Poll_Up (int *pElements, void **ppBuffers)
{
   IFX_LOCAL   IFX_uint16_t      ndevNull[MAX_PACKET_WORD];
   IFX_vuint16_t        *pIr;
   IFX_vuint16_t        *pNwd;
   VINETIC_DEVICE      *pDev;
   IFX_void_t         **ppCurBuf        = ppBuffers;
   PACKET              *pCurPkt;
   IFX_uint16_t        *pCurPktPos;
   IFX_uint16_t         packetLen;
   IFX_uint32_t         devNo;
   IFX_uint16_t         mbxSize;
#ifdef VIN_V14_SUPPORT
            IFX_int32_t          cnt;
#endif /* VIN_V14_SUPPORT */

   LOCK_ALL_DEVICES;

   /* Initalize */
   *pElements = 0;
   FOR_ALL_DEVICES
   {
      /* get current device pointer */
      pDev = vinPollStruct.pDevicePoll[devNo];

      pNwd = pDev->pNwd;
      pIr  = (IFX_uint16_t *) pDev->pIntReg;

      if (SC_READ (SC_ROBXML, &mbxSize, 1) != IFX_SUCCESS)
         return IFX_ERROR;

      mbxSize &= OBXML_PDATA;
      if (mbxSize == 0)
         continue;

      IFXOS_ASSERT (mbxSize <= MAX_PACKET_WORD);

      WAIT_FOR_READY_FLAG;                                      /* send RPOBX */
      *pNwd                         = SC_RPOBX;

      while (mbxSize > 0)
      {
         pCurPkt = vinPollStruct.get(vinPollStruct.pbp);

         if (pCurPkt != NULL)
         {

            /* Add to Buffer Array or dump if no free buffer available */
            *(ppCurBuf++) = pCurPkt;
            (*pElements)++;
         }
         else
         {
            pCurPkt                 = (PACKET *) &ndevNull;
         }

         pCurPktPos                 = (IFX_uint16_t *) &(pCurPkt->cmd1);

         WAIT_FOR_READY_FLAG;                           /* CMD IFX_uint16_t 1 */
         *pCurPktPos                 = *pNwd;

         *pCurPktPos = (IFX_uint16_t)
            ((*pCurPktPos  & CMD1_CH ) | (devNo << VIN_POLL_CH_BITS));

         pCurPktPos++;
         mbxSize--;

         WAIT_FOR_READY_FLAG;                           /* CMD IFX_uint16_t 2 */
         *pCurPktPos                = *pNwd;
         packetLen                  = *pCurPktPos & CMD2_LEN;
         pCurPktPos++;
         mbxSize--;

         mbxSize -= packetLen;
         while (packetLen > 1)                                   /* read data */
         {
            WAIT_FOR_READY_FLAG;
            *(pCurPktPos++)         = *pNwd;
            packetLen--;
         }

         WAIT_FOR_READY_FLAG;                                    /* last word */
         if (mbxSize != 0)
            *(pCurPktPos++)         = *pNwd;
         else
            *(pCurPktPos++)         = *(pDev->pEom);

      } /* end of while (mbxsize > 0) */
   }

   UNLOCK_ALL_DEVICES;
   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   UNLOCK_ALL_DEVICES;
   TRACE (VINETIC, DBG_LEVEL_HIGH, ("\nERR ACCESS IFX_ERROR Up\n"));
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/******************************************************************************/
/**
   Send packets to the Vinetic mailbox (interleaved mode)

   \param   nElements   - number of valid buffer pointers in the array
   \param   **ppBuffers - pointer to the array of buffer pointers

   \return  IFX_SUCCESS or IFX_ERROR

   \remarks
   Mailbox access is proctected against concurent access / tasks.

   Function sends packets the the VINETIC mailbox. An array of pointers
   provides the reference to the packets (ppBuffers). nElements is the
   number of valid pointers (packets) to be sent. NULL pointers in the
   array are allowed and will be ignored (i.e. NULL pointer do not count
   as packets in nElements).
   The packets are written to the Vinetic in an interleaved way (if more
   than one chip has active channels). After sending the packets to the
   VINETIC mailbox, the buffers will be returned to the bufferpool.
   The application is responsible to populate the cmd2 header fields such
   as packet length and odd bit!

          pDev->pCurPkt   ---> +------------------+  pDev->packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
          pDev->pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+

*******************************************************************************/
int VINETIC_Poll_DownIntlv (const int nElements, void **ppBuffers)
{
   IFX_LOCAL   IFX_boolean_t     printVINETIC_InterleavedDown = IFX_FALSE;
   IFX_vuint16_t                *pIr;
   VINETIC_INTERLEAVED           dwn [VINETIC_MAX_DEVICES];
   VINETIC_INTERLEAVED          *pDwn;
   IFX_void_t                  **ppCurBuf           = ppBuffers;
   PACKET                       *pPacket;
   IFX_uint16_t                  nFibxms;
   IFX_uint32_t                  devNo, ch, length;
   IFX_int32_t                   ret                = IFX_SUCCESS;
   IFX_int32_t                   packet;
   IFX_uint32_t                  mbxDataAvail       = 0;
#ifdef VIN_V14_SUPPORT
   IFX_uint32_t                  cnt;
#endif /* VIN_V14_SUPPORT */

   if (nElements == 0)
      return IFX_SUCCESS;

   LOCK_ALL_DEVICES;

   /* initialize statemachine */
   FOR_ALL_DEVICES
   {
      dwn[devNo].packetLen       = STREAM_INTERLEAVED_INIT;
      dwn[devNo].mbxSize         = 0;
   }

   /* sort the packets from the array which contains packets for all devices
      to device specific fifos for interleaving, init structures **************/
   for (packet = 0; packet < nElements; packet++)
   {
      /* while pointer is NULL, go to the next one */
      while (*ppCurBuf == NULL)
      {
         ppCurBuf++;
      }
      pPacket          = (PACKET*) *(ppCurBuf++);

      /* calculate logical channel 0..127 */
      ch        = pPacket->cmd1 & VIN_POLL_HDR1_CH;
      devNo     = (IFX_uint32_t) (pPacket->cmd1 >> VIN_POLL_CH_BITS);

      if (devNo > vinPollStruct.numCurDevsPoll)
         continue;

      {
         VINETIC_DEVICE *pDev = vinPollStruct.pDevicePoll[devNo];

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
         /* check payload. pData is aligned to network order, big endian */
         if ((pPacket->pData[0] & RTP_PT) == pDev->pChannel[ch].pTapiCh->nEvtPT)
         {
            /* create a valid command header for event packets */
            pPacket->cmd1    = (IFX_uint16_t)(CMD1_WR | CMD1_EVT | ch);
         }
         else
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
         {
            /* create a valid command header for voice packets */
            pPacket->cmd1    = (IFX_uint16_t)(CMD1_WR | CMD1_VOP | ch);
         }

         length           = (pPacket->cmd2 & VIN_BUF_HDR2_LEN) + CMD_HEADER_CNT;
         /* set required mailbox size */
         dwn[devNo].mbxSize     += length;

         /* add this buffer to device specific fifo */
         fifoPut ((FIFO_ID*)pDev->pFifo, pPacket, (int)length);
      }
   }

   /* check mailbox size and number of devices with data **********************/
   FOR_ALL_DEVICES
   {
      /* check mailboxsize for all devices for which we have data */
      if (dwn[devNo].mbxSize > 0)
      {
         /* get current device's pointers */
         VINETIC_DEVICE *pDev = vinPollStruct.pDevicePoll[devNo];

         if (SC_READ (SC_RFIBXMS, &nFibxms, 1) != IFX_SUCCESS)
            return IFX_ERROR;

         /* apply mask to get free space in packet inbox */
         nFibxms &= FIBXMS_PBOX;
         /* nFibxms must be words to send plus one,
            i.e. ">" is used instead of ">=" */
         if (nFibxms > dwn[devNo].mbxSize)
         {
            mbxDataAvail |= (1 << devNo);
         }
         else
         {  /* TBD find a better concept without dumping packets */
            /* return buffers to the pool */
            for (;;)
            {
               IFX_uint16_t *ptr = fifoGet((FIFO_ID*) pDev->pFifo, NULL);

               /* check if the fifo is empty already */
               if (ptr == NULL)
                  break;

               if (vinPollStruct.put(ptr) != IFX_SUCCESS)
                  break;
            }

            /* remove bufferpointers from fifo and set mailboxsize to zero */
            fifoReset ((FIFO_ID*) pDev->pFifo);
            dwn[devNo].mbxSize    = 0;
         }
      }
   }

   /* write packets interleaved ***********************************************/
   while (mbxDataAvail)
   {  FOR_ALL_DEVICES                                          /* index devNo */
      {
         /* if we have still data for this device to be sent... */
         if (dwn[devNo].mbxSize > 0)
         {
            /* get current device's pointers */
            VINETIC_DEVICE *pDev = vinPollStruct.pDevicePoll[devNo];

            pIr  = (IFX_uint16_t *) pDev->pIntReg;
            pDwn = &dwn[devNo];

            if (pDwn->packetLen > 0)
            {  /* write data **************************************************/
               WAIT_FOR_READY_FLAG;
               if (pDwn->mbxSize != 1)
                  *(pDev->pNwd)   = *(pDwn->pCurPktPos++);
               else
                  *(pDev->pEom)   = *(pDwn->pCurPktPos++);

               if (--(pDwn->packetLen) == 0)
               {
                  /* voice packet is written to the mailbox. Now return the
                     buffer element back to the buffer pool.
                     Set the local parameter back to the beginning of the
                     packet in order to ready for the next voice packet. */
                  pDwn->packetLen = STREAM_INTERLEAVED_INIT;

                  ret |= vinPollStruct.put(pDwn->pCurPkt);
               }

               if (--(pDwn->mbxSize) == 0)
               {
                  mbxDataAvail &= ~(1 << devNo);
               }
            }
            else
            {  /* initialize the packet ***************************************/
               switch (pDwn->packetLen)
               {
               case STREAM_INTERLEAVED_INIT:
               case STREAM_INTERLEAVED_CMD1:
                  pDwn->pCurPkt      = fifoGet((FIFO_ID*)pDev->pFifo, NULL);
                  pDwn->pCurPktPos   = &(pDwn->pCurPkt->cmd1);
                  WAIT_FOR_READY_FLAG;
                  *(pDev->pNwd)           = *(pDwn->pCurPktPos++);
                  pDwn->packetLen    = STREAM_INTERLEAVED_CMD2;
                  pDwn->mbxSize--;
                  break;
               case STREAM_INTERLEAVED_CMD2:
                  WAIT_FOR_READY_FLAG;
                  *(pDev->pNwd)           = *(pDwn->pCurPktPos++);
                  pDwn->packetLen    = pDwn->pCurPkt->cmd2 & CMD2_LEN;
                  pDwn->mbxSize--;
                  break;
               default:
                  if (printVINETIC_InterleavedDown == IFX_FALSE)
                  {
                     printVINETIC_InterleavedDown = IFX_TRUE;
                     TRACE (VINETIC, DBG_LEVEL_HIGH,
                        ("\nERR VINETIC_InterleavedDown, "
                        "unexpected state: %ld\n", pDwn->packetLen));
                     pDwn->packetLen = STREAM_INTERLEAVED_INIT;
                  }
               }
            }
         }
      }
   }

   UNLOCK_ALL_DEVICES;

   if (ret != IFX_SUCCESS)
      return IFX_ERROR;

   return IFX_SUCCESS;

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   UNLOCK_ALL_DEVICES;
   TRACE (VINETIC, DBG_LEVEL_HIGH, ("\nERR ACCESS IFX_ERROR (PolledDown)\n"));
   return IFX_ERROR;
#endif /* VIN_V14_SUPPORT */
}

/******************************************************************************/
/**
   Serves the mailbox data event (interleaved mode)

   \param   *pElements  - number of valid buffer pointers in the array
   \param   **ppBuffers - pointer to the array of buffer pointers

   \return  IFX_SUCCESS or IFX_ERROR

   \remarks
   Mailbox access is proctected against concurent access / tasks

   Function reads the amount of data indicated in OBXML interleaved for all
   VINETIC devices. The buffer is taken from the bufferPool (provided by the
   calling task). If no buffer is available or the mailbox is corrupted and
   has to be totally emptied the read data is dumped to the local ndevNull.

          pDev->pCurPkt   ---> +------------------+  pDev->packetLen
                            |> +  CMD HDR Word 1  +  -2
                            |> +  CMD HDR Word 2  +  -1
                            |> +------------------+
          pDev->pCurPktPos--|> +  RTP data        +  46 (depending on coder)
                            |> +                  +  45
                            |> +       ..         +   .
                            |> +                  +   0
                            |> +      0000        +
          end of buffer        +------------------+
          check pattern        +      AAAA        +
          of bufferPool        +------------------+

*******************************************************************************/
int VINETIC_Poll_UpIntlv (int *pElements, void **ppBuffers)
{
   IFX_LOCAL   IFX_boolean_t   printVINETIC_InterleavedUp = IFX_FALSE;
   IFX_LOCAL   IFX_uint16_t    ndevNull[MAX_PACKET_WORD];
   IFX_vuint16_t               *pIr;
   VINETIC_DEVICE              *pDev;
   IFX_void_t                  **ppCurBuf        = ppBuffers;
   VINETIC_INTERLEAVED         ups [VINETIC_MAX_DEVICES];
   VINETIC_INTERLEAVED         *pUps;
   IFX_uint32_t                devNo;
   IFX_uint32_t                mbxDataAvail    = 0;
   IFX_uint16_t                nVal;
#ifdef VIN_V14_SUPPORT
   IFX_uint32_t                cnt = 0;
#endif /* VIN_V14_SUPPORT */

   LOCK_ALL_DEVICES;

   /* initalize */
   *pElements = 0;
   /* read mailbox size, init state machine ***********************************/
   FOR_ALL_DEVICES                                             /* index devNo */
   {
      /* get current device pointer */
      pDev = vinPollStruct.pDevicePoll[devNo];

      pUps  = &ups[devNo];
      /* reset state machine */
      pUps->packetLen = STREAM_INTERLEAVED_INIT;

      if (SC_READ (SC_ROBXML, &nVal, 1) != IFX_SUCCESS)
         return IFX_ERROR;

      pUps->mbxSize = (nVal & OBXML_PDATA);
      if (pUps->mbxSize)
      {
         mbxDataAvail  |= (1 << devNo);
      }
   }

   /* read all available packets from all active channels/devices *************/
   while (mbxDataAvail)
   {
      FOR_ALL_DEVICES                                          /* index devNo */
      {
         /* if this device's mailbox still contains data... */
         if (ups[devNo].mbxSize > 0)
         {
            /* get current device's pointers */
            pDev = vinPollStruct.pDevicePoll[devNo];

            pIr  = (IFX_uint16_t *) pDev->pIntReg;
            pUps = &ups[devNo];

            /* decide if next word is voice data or cmd header */
            if (pUps->packetLen > 0)
            {  /* read voice data *********************************************/
               WAIT_FOR_READY_FLAG;
               /* read from NWD or EOM and store data in current buffer */
               if (pUps->mbxSize != 1)
                  *(pUps->pCurPktPos++)    = *(pDev->pNwd);
               else
                  *(pUps->pCurPktPos++)    = *(pDev->pEom);
               /* if last word of packet was read,
                  reset state machine to beginning of next packet */
               if (--(pUps->packetLen) == 0)
               {
                  pUps->pCurPkt            = NULL;
                  pUps->packetLen          = STREAM_INTERLEAVED_CMD1;
               }
               /* if last word of mailbox was read,
                  remove device from list of active devices */
               /* TBD: check may be needed only if packetlen is also zero.
                       Currently this is separate for safety reasons */
               if (--(pUps->mbxSize) == 0)
               {
                  mbxDataAvail            &= ~(1 << devNo);
               }
            }
            else
            {
               /* initial states: prepare packet read.
                  read cmd header / send SC_RPOBX *****************************/
               switch (pUps->packetLen)
               {
                  /* send mbx read command, init buffer ***********************/
                  /* Note: this state is reached only once per poll cycle */
                  case STREAM_INTERLEAVED_INIT:
                    WAIT_FOR_READY_FLAG;
                    *(pDev->pNwd) = SC_RPOBX;
                     pUps->packetLen    = STREAM_INTERLEAVED_CMD1;
                     break;
                  /* read first cmd word **************************************/
                  case STREAM_INTERLEAVED_CMD1:
                     /* retrieve a new buffer from the pool */
                     {
                        PACKET *ptr;

                        /* get a buffer from the pool */
                        ptr = vinPollStruct.get(vinPollStruct.pbp);

                        if (ptr != NULL)
                        {

                           /* Add buffer to array and inc the element counter */
                           *(ppCurBuf++) = ptr;
                           (*pElements)++;

                           pUps->pCurPkt = ptr;
                        }
                        else
                        {
                           pUps->pCurPkt = (PACKET *) &ndevNull;
                        }
                     }

                     /* set pCurPktPos to first cmd word in the buffer */
                     pUps->pCurPktPos = (IFX_uint16_t *) &(pUps->pCurPkt->cmd1);
                     WAIT_FOR_READY_FLAG;
                     /* read from the device */
                     nVal = *(pDev->pNwd);
                     /* store first cmd word and add the device number to the
                        channel information */
                     *(pUps->pCurPktPos++) = (IFX_uint16_t)
                        ((nVal & CMD1_CH) | (devNo << VIN_POLL_CH_BITS));
                     /* set next state */
                     pUps->packetLen  = STREAM_INTERLEAVED_CMD2;
                     pUps->mbxSize--;
                     break;
                  /* read second cmd word, pkt length *************************/
                  case STREAM_INTERLEAVED_CMD2:
                     WAIT_FOR_READY_FLAG;
                     /* read from the device */
                     nVal                  = *(pDev->pNwd);
                     /* store second cmd word in the buffer */
                     *(pUps->pCurPktPos++) = nVal;
                     /* mask packet length (in words) */
                     pUps->packetLen       = nVal & CMD2_LEN;

                     IFXOS_ASSERT(
                        pUps->packetLen <= (IFX_int32_t) pUps->mbxSize);

                     IFXOS_ASSERT(pUps->packetLen != 0);
                     pUps->mbxSize--;
                     break;
                  /* handle undefined states / error cases */
                  default:
                     if (printVINETIC_InterleavedUp == IFX_FALSE)
                     {
                        printVINETIC_InterleavedUp = IFX_TRUE;
                        TRACE (VINETIC, DBG_LEVEL_HIGH,
                           ("\nERR VINETIC_InterleavedUp, "
                            "unexpected state: %ld\n", pUps->packetLen));
                     }
                     /* check if the current buffer was already added to
                        the array of received buffers ppBuffers - if yes
                        we have to remove this entry ... */
                     if ((pUps->pCurPkt != NULL) &&
                         (pUps->pCurPkt == (PACKET*) ppCurBuf))
                     {
                        *(--ppCurBuf)   =  NULL;
                        (*pElements)--;
                     }

                     /* try to return the buffer element back to the buffer
                        pool. */
                     if (vinPollStruct.put(pUps->pCurPkt) != IFX_SUCCESS)
                     {
                        UNLOCK_ALL_DEVICES;
                        return (IFX_ERROR);
                     }

                     /* ... and dump the rest of the mailbox... */
                     pUps->pCurPkt      = (PACKET *) &ndevNull;
                     pUps->pCurPktPos   = (IFX_uint16_t *)   &ndevNull;
                     pUps->packetLen    = pUps->mbxSize;
               }
            }
         }
      }
   }

   UNLOCK_ALL_DEVICES;
   return (IFX_SUCCESS);

#ifdef VIN_V14_SUPPORT
ACCESS_ERR:
   UNLOCK_ALL_DEVICES;
   TRACE (VINETIC, DBG_LEVEL_HIGH, ("\nERR ACCESS IFX_ERROR (PolledUp)\n"));
   return(IFX_ERROR);
#endif /* VIN_V14_SUPPORT */
}

#endif /* VINETIC_POLL */

