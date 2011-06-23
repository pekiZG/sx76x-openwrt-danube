#ifndef _DRV_VINETICAPI_H
#define _DRV_VINETICAPI_H
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
 ****************************************************************************
   Module      : drv_vinetic_api.h
   Description : This file contains the defines, the structures declarations
                 the tables declarations and the global functions declarations.
*******************************************************************************/
/* includes */
#include "drv_api.h"
#include "vinetic_io.h"
#include "drv_tapi.h"
#include "drv_vinetic.h"
#include "drv_vinetic_gpio.h"
#if (defined EVALUATION) || (defined TESTING)
#include "drv_vinetic_eval_io.h"
#endif /* EVALUATION */
#ifdef QOS_SUPPORT
#include "drv_qos.h"
#endif /* QOS_SUPPORT */
#ifdef VIN_POLL
#include "lib_fifo.h"
#endif /* VIN_POLL */
#ifdef TAPI_LL_DRV
#include "drv_tapi_interface.h"
#include "drv_tapi_ll_interface.h"
#else
#endif /* TAPI_LL_DRV */


/* ============================= */
/* Trace groups                  */
/* ============================= */
/* Trace groups, defined in drv_vinetic_main.c */
DECLARE_TRACE_GROUP(VINETIC);
DECLARE_LOG_GROUP(VINETIC);
#ifdef VIN_SPI
DECLARE_TRACE_GROUP(SPI_DRV);
#endif /* VIN_SPI */

/* ============================= */
/* Data swapping macros          */
/* ============================= */

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
/*******************************************************************************
Description:
   Swapping of an unsigned integer value on little endian host system.
Arguments
   x  -  unsigned integer value
Return:
   swapped value
*******************************************************************************/
#ifndef SWAP_UINT32
#define SWAP_UINT32(x)         \
(                              \
     (((x)& 0xFF000000) >> 24) \
   | (((x)& 0x00FF0000) >>  8) \
   | (((x)& 0x0000FF00) <<  8) \
   | (((x)& 0x000000FF) << 24) \
)
#endif /* !SWAP_UINT32 */

/*******************************************************************************
Description:
   Swapping of an unsigned short value on little endian host system.
Arguments
   x  -  unsigned short value
Return:
   swapped value
*******************************************************************************/
#ifndef SWAP_UINT16
#define SWAP_UINT16(x)  ((((x)&0xFF00) >> 8) | (((x) & 0x00FF) << 8))
#endif /* !SWAP_UINT16 */

#elif (__BYTE_ORDER == __BIG_ENDIAN)
/*******************************************************************************
Description:
   Swapping of an unsigned integer value on big endian host system.
Remarks
   Empty on big endian platform
*******************************************************************************/
#ifndef SWAP_UINT32
#define SWAP_UINT32(x)  (x)
#endif /* !SWAP_UINT32 */

/*******************************************************************************
Description:
   Swapping of an unsigned short value on big endian host system.
Remarks
   Empty on big endian platform
*******************************************************************************/
#ifndef SWAP_UINT16
#define SWAP_UINT16(x)  (x)
#endif /* !SWAP_UINT16 */

#endif /* __BYTE_ORDER */

/** round the parameter */
#define ROUND(n)        ((( (n) * 10) + 5 ) / 10)
/** macro to divide the parameter by 10 and round accordingly */
#define ROUND_DIV10(n)  ( ( (n) +  5) / 10 )
/** macro to divide the parameter by 100 and round accordingly */
#define ROUND_DIV100(n) ( ( (n) + 50) / 100 )
/** macro to divide the parameter by 1000 and round accordingly */
#define ROUND_DIV1000(n)( ( (n) + 500)/ 1000 )

/** constant 2^2 */
#define C2_2            4
/** constant 2^6 */
#define C2_6            64
/** constant 2^8 */
#define C2_8            256
/** constant 2^13 */
#define C2_13           8192
/** constant 2^14 */
#define C2_14           16384
/** constant 2^15 */
#define C2_15           32768
/** constant 2^31 */
#define C2_31           2147483648UL
/** constant 2^32 */
#define C2_32           4294967295UL

/* ============================= */
/* Global Defines                */
/* ============================= */

/* fifo element number for packet mailbox events */
#define POBX_FIFO_SIZE        12
/* each fifo element has size of maximum packet plus 4 words timestamp */
#define POBX_ELEMENT_SIZE     (MAX_PACKET_WORD + 4)
/* Tapi Version */
#define TAPI_VERS             0x01000002
/* default payload type for RTP event packets */
#define DEFAULT_EVTPT         0x62

#ifdef TAPI
   /* PCM timeslot allocation management definitions */
   #define PCM_MAX_TS            127
   #define PCM_TS_AARAY          ((PCM_MAX_TS + 3) / 32)
   /* Note : the pcm highway depend on the chip actualy used and
             is defined in the appropriate api header */
#endif /* #ifdef TAPI */

/* ============================= */
/* VINETIC Common defines        */
/* ============================= */

#define VINETIC_MINOR_BASE       10
#define DEV_NAME                 "VINETIC"
#define VINETIC_MAJOR            230

#ifdef ADDED_SUPPORT
/* Interrupt Error Status FIFO size  */
#define ERRFIFO_SIZE             24
#endif /* ADDED_SUPPORT */

/* ============================= */
/* Driver timeouts               */
/* ============================= */

/* wait time in us for the next VINETIC access in case of polling register */
#define WAIT_POLLTIME   50

/* used by drv_vinetic_api.c */
/* time to wait until OBXML indicates data:
   OBXML_POLL_LOOP * WAIT_POLLTIME = poll time in us */
#define OBXML_POLL_LOOP 100

/* loops until free inbox space is available
   time = FIBXMS_POLL_LOOP * WAIT_POLLTIME = poll time in us */
#define FIBXMS_POLL_LOOP  1000

/* used by drv_vinetic_host.c */
/* timeout in ms for mailbox emty event in ms */
#ifdef EMULATION
#define WAIT_MBX_EMPTY                    10000
#else
#define WAIT_MBX_EMPTY                    100
#endif /* EMULATION */
/* timeout in ms for EDSP Ready event in ms */
#ifdef EMULATION
#define WAIT_EDSP                         5000
#else
#define WAIT_EDSP                         50
#endif /* EMULATION */

/* used by drv_vinetic_dwnld.c */
/* time to wait after Edsp Start Sc, in ms.
   The appropriate time to wait for it isn't yet stated by any specification,
   although not waiting leads to EDSP starting failure, coz the EDSP needs
   some time to setup after processing the Start Edsp Short Command

   In case of the VINETIC-CPE v2.1 the minimum time to wait after the Dld Ready
   bit is set is specified with 1 ms.
*/
#ifdef EMULATION
#define WAIT_AFTER_START_EDSP             3000
#else
#define WAIT_AFTER_START_EDSP             30
#endif /* EMULATION */

/* ============================= */
/* VINETIC flag settings         */
/* ============================= */

/* this value is used for variables which should have a defined
   content. Matches for bytes and replaces -1 to use variable space. */
#define NOTVALID               0xff

/* ============================= */
/* Global Macros                 */
/* ============================= */

#ifdef TESTING
/* macros for testing */
#define IF_TEST(test) \
   if (pDev->nTestCase == test && pDev->bTestStart)
#endif /* TESTING */

#ifdef DEBUG
#define VCH_MAGIC        0x81f56d4e
#define VDEV_MAGIC       0x81f56d4e

#define CHK_DEV_MAGIC IFXOS_ASSERT(pDev->magic == VDEV_MAGIC)
#define CHK_CH_MAGIC  IFXOS_ASSERT(pCh->magic  == VCH_MAGIC)
#else
#define CHK_DEV_MAGIC
#define CHK_CH_MAGIC
#endif /* DEBUG */

#ifndef LINUX
#define OS_MapBuffer(p_buffer,size)    (IFX_void_t*)(p_buffer)
#define OS_UnmapBuffer(p_buffer)       /* nothing */
#endif /* LINUX */

/* Sets CID Machine State */
#define CIDTX_SET_STATE(state)  do {\
        pCh->cidSend.nState = (state);\
        }while(0);

/* max CID Data size in word to send on one go */
#define MAX_CID_LOOP_DATA        10

/*******************************************************************************
Description:
   always returns the absolute value of argument given
Arguments:
   x  - argument
Return:
   absolute value of x
*******************************************************************************/
/* define ABS */
#define ABS(x) (((x) < 0) ? -(x) : (x))

/* regular format macros */
/*******************************************************************************
Description:
   Converts a value to one byte per masking
Arguments:
   val  : any value
Return:
   Low Byte
*******************************************************************************/
#define LOWBYTE(val)               (IFX_uint8_t)((val) & 0xFF)

/*******************************************************************************
Description:
   Extracts the high byte from a IFX_uint16_t value
Arguments:
   val  : any IFX_uint16_t value
Return:
   High Byte
*******************************************************************************/
#define HIGHBYTE(val)              (IFX_uint8_t)(((val)>>8) & 0xFF)

/*******************************************************************************
Description:
   Extracts the low Word from a IFX_uint32_t value
Arguments:
   val  : any IFX_uint32_t value
Return:
   Low Word
*******************************************************************************/
#define LOWWORD(val)               (IFX_uint16_t)((val) & 0xFFFF)

/*******************************************************************************
Description:
   Extracts the high Word from a IFX_uint32_t value
Arguments:
   val  : any IFX_uint32_t value
Return:
   High Word
*******************************************************************************/
#define HIGHWORD(val)              (IFX_uint16_t)(((val)>>16) & 0xFFFF)

/*******************************************************************************
Description:
   Convert two Bytes to one Word
Arguments:
   low  : any IFX_uint8_t value which will be LSB
   high : any IFX_uint8_t value which will be MSB
Return:
   Word
*******************************************************************************/
#define BYTE2WORD(low, high)       (IFX_uint16_t)(((high)<<8) | (low))

/*******************************************************************************
Description:
   Convert two Words to one Double Word
Arguments:
   low  : any IFX_uint16_t value which will be LSW
   high : any IFX_uint16_t value which will be MSW
Return:
   Double Word
*******************************************************************************/
#define WORD2LONG(low, high)       (IFX_uint32_t)(((high)<<16) | (low))


/* ================================ */
/* Macros related to irq handling   */
/* ================================ */

/*******************************************************************************
Description:
   Disables the interrupt line
Arguments
   irq -  irq line number
Remarks
   This macro is system specific. In case the Operating system methods can not
   be used for this purpose, define the correct macro in your user configuration
   file.
*******************************************************************************/
#ifndef VIN_DISABLE_IRQLINE
#define VIN_DISABLE_IRQLINE(irq)       IFXOS_IRQ_DISABLE(irq)
#endif

/*******************************************************************************
Description:
   Enables the interrupt line
Arguments
   irq -  irq line number
Remarks
   This macro is system specific. In case the Operating system methods can not
   be used for this purpose, define the correct macro in your user configuration
   file.
*******************************************************************************/
#ifndef VIN_ENABLE_IRQLINE
#define VIN_ENABLE_IRQLINE(irq)        IFXOS_IRQ_ENABLE(irq)
#endif


/*******************************************************************************
Description:
   Disables the global interrupt
Arguments
   var -  local mask that has to be stored for VIN_ENABLE_IRQGLOBAL
Remarks
   This macro is system specific. In case the Operating system methods can not
   be used for this purpose, define the correct macro in your user configuration
   file.
   This macro is used when no device specific interrupt handler or interrupt
   line is configured (nIrg < 0).
*******************************************************************************/
#ifndef VIN_DISABLE_IRQGLOBAL
#define VIN_DISABLE_IRQGLOBAL(var)       IFXOS_LOCKINT(var)
#endif

/*******************************************************************************
Description:
   Enables the global interrupt
Arguments
   var -  local mask that was stored by  VIN_DISABLE_IRQGLOBAL
Remarks
   This macro is system specific. In case the Operating system methods can not
   be used for this purpose, define the correct macro in your user configuration
   file.
   This macro is used when no device specific interrupt handler or interrupt
   line is configured (nIrg < 0).
*******************************************************************************/
#ifndef VIN_ENABLE_IRQGLOBAL
#define VIN_ENABLE_IRQGLOBAL(var)       IFXOS_UNLOCKINT(var)
#endif


/* ================================ */
/* Enumerations                     */
/* ================================ */

/* device states */
/* firmware successfully downloaded */
#define   DS_FW_DLD                    0x00000001
/* phi downloaded successfully, no further download will be allowed */
#define   DS_PHI_DLD                   0x00000002
/* DTMF sure detected tons masked (MSRE1) */
#define   DS_DTMF_SURE                 0x000004
/* DTMF probably detected tons masked (MSRE1)*/
#define   DS_DTMF_PROB                 0x000008
/* CRAM Downloaded */
#define   DS_CRAM_DLD                  0x000010
/* Basic Init done */
#define   DS_BASIC_INIT                0x000020
/* Tapi Init done */
#define   DS_TAPI_INIT                 0x000040
/* interrupt installed */
/* #define   DS_INT_ENABLED   0x000080 */
#if defined (ASYNC_CMD) && 1
/* command data was previously requested and maybe
   in command outbox. This flag will be reset if the data has been read */
#define   DS_CMDREQ                    0x000100
/* command data has been read and is available in pCmdData. This flag is
   used in CmdRead to check if requested data was read in another context */
#define   DS_CMDREAD                   0x000200
/* a read command was sent to the inbox. If another location reads out all
   mailbox content it will be signaled with DS_CMDREAD */
#define   DS_RDCMD_SENT                0x000400
#endif /* ASYNC_CMD */
/** device successfully initialized */
#define   DS_DEV_INIT                  0x000800
/** Coder module enabled  */
#define   DS_COD_EN                    0x001000
/** Analog module enabled  */
#define   DS_ALM_EN                    0x002000
/** signaling module enabled  */
#define   DS_SIG_EN                    0x004000
/** PCM module enabled  */
#define   DS_PCM_EN                    0x008000
/** DC Download done */
#define   DS_DC_DWLD                   0x010000
/** AC Download done */
#define   DS_AC_DWLD                   0x020000
/** Qos succesfully initialized */
#define   DS_QOS_INIT                  0x040000

/* Different driver modes for Interrupt handling */
/* The VINETIC driver can used in four different system mode:
      - Events and Voice Packets are handled via the IRQ-Routine of the driver
      - Events and Voice Packets are handled when the IRQ-Routine is called in
        a task context. The driver never enables the hardware interrupts
      - Events are handled via the IRQ-Routine and it is called during an
        interrupt. Voice Packets are exchanged via the polling mechanism
        (drv_vinetic_polled.c)
      - Events are handled when the IRQ-Routine is called in
        a task context. Voice Packets are exchanged via the polling mechanism
        (drv_vinetic_polled.c)

      VIN_VOICE_POLL
      --------------
      If this is set in the driver, the voice packets are exchanged
      through the polling (interleaved) routines. Otherwise by event handling
      in VINETIC IRQ routine.

      VIN_EVENT_POLL
      --------------
      If this is set, the driver does not enable interrupts. The VINETIC IRQ
      routine has to be called in task context.

      VIN_TAPI_WAKEUP
      ---------------
      If this is set, TAPI Wakeup is used for events/select mechanism.
      For default this bis set!
*/
#define VIN_VOICE_POLL                 (1 << 0) /* 1 */
#define VIN_EVENT_POLL                 (1 << 1) /* 2 */
#define VIN_TAPI_WAKEUP                (1 << 2) /* 4 */

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

/** CID sending states */
enum
{
   CID_SETUP         = 0,
   CID_PREPARE       = 1,
   CID_TRANSMIT      = 2,
   CID_OFFHOOK       = 3,
   CID_TRANSMIT_ERR  = 4,
   CID_TRANSMIT_END  = 5
};

/** DTMF transmission status
Note: The states ABORT and ERROR indicate the status of the /last/ DTMF
      transmission - a new transmission can be started right away without
      restriction. */
typedef enum
{
   /* DTMF generator idle */
   DTMF_READY,
   /* DTMF generator activated, waiting on first REQ */
   DTMF_START,
   /* DTMF generator transmitting */
   DTMF_TRANSMIT,
   /* DTMF transmission aborted (either by user or underrun) */
   DTMF_ABORT
} VINETIC_DTMF_STATE;

/** EDSP resources requiring specific masking handling */
typedef enum
{
   VIN_EDSP_CIS   = 0x0,
   VIN_EDSP_DTMFG = 0x1,
   VIN_EDSP_UTD   = 0x2,
   VIN_EDSP_CPT   = 0x3,
   VIN_EDSP_ATD   = 0x4,
   VIN_EDSP_UTG1  = 0x5,
   VIN_EDSP_UTG2  = 0x6
} VIN_EDSP_Res_t;

/* ============================= */
/* Global Structures             */
/* ============================= */

typedef struct _VINETIC_CHANNEL VINETIC_CHANNEL;
typedef struct _VINETIC_DEVICE  VINETIC_DEVICE;
typedef struct vinetic_interrupt_s vinetic_interrupt_t;
typedef struct _VINETIC_CAPABILITIES VINETIC_CAPABILITIES_t;

/* host api header file needs the above defined typedefs :
   VINETIC_CHANNEL and VINETIC_DEVICE */
#include "drv_vinetic_hostapi.h"
/* eval header also needs VINETIC_CHANNEL and VINETIC_DEVICE typedefs */
#ifdef EVALUATION
#include "drv_vinetic_eval.h"
#endif /* EVALUATION */

/** Data structure for one DSP signal input.
 It contains the signal array number for its input, a pointer to the next
 input connected to the same signal, the muting information and a
 pointer to the module to which the input is connected (used as previous
 pointer */
typedef struct _MODULE_SIGNAL
{
   /** input source as value taken from ECMD_IX_SIG.
   This input is within the list of input signals that are all connected
   to one output signal. */
   IFX_uint8_t i;
#ifdef MUTING_PRELIM
   /** if set to 1 this signal is muted and will actually not be connected.
   The firmware does not support muting signals. */
   IFX_uint8_t mute;
#endif
   /* parent module. in case of muting, it must be set to modified */
   struct _MODULE* pParent;
   /** Which output is it connected to. Value taken from ECMD_IX_SIG, but here
   as BYTE to save memory */
   struct _MODULE* pOut;
   /** points to the next input to which the outSig is connected to */
   struct _MODULE_SIGNAL* pNext;
} VINDSP_MODULE_SIGNAL;

/** Data structure for one DSP module, like analog line module (ALM), Coder,
    PCM. The signaling module inherits the member and adds an additional
    input list */
typedef struct _MODULE
{
   /** module inputs to copy to firmware messages */
   VINDSP_MODULE_SIGNAL in[MAX_MODULE_SIGNAL_INPUTS];
   /** points to the first input signal which connects to this output */
   VINDSP_MODULE_SIGNAL* pInputs;
   /* the signal array index */
   IFX_uint8_t nSignal;
   IFX_uint8_t modified;
   /** defines the module type, value out of VINDSP_MT */
   IFX_uint8_t nModType;
   /* the signal array index of a second output
      (only used for signaling modules) */
   IFX_uint8_t nSignal2;
   /** Second output pin (only used for the signaling module).
   It points to the first input signal which connects to this output */
   VINDSP_MODULE_SIGNAL* pInputs2;
#ifdef DEBUG
   IFX_char_t name[10];
#endif /* DEBUG */
}VINDSP_MODULE;

/** Structure for the analog line module channel
    including firmware message cache */
typedef struct
{
   VINDSP_MODULE signal;
   FWM_ALM_CH        ali_ch;
   IFX_uint16_t      ali_nelec;
   /* complete list here */
} VINETIC_ALMCH;

/** Structure for the signaling channel
   including firmware message cache */
typedef struct
{
   VINDSP_MODULE signal;
   FWM_SIG_CH        sig_ch;
   FWM_SIG_XTD       sig_atd1;
   FWM_SIG_XTD       sig_atd2;
   FWM_SIG_XTD       sig_utd1;
#ifndef VIN_2CPE
   FWM_SIG_XTD       sig_utd2;
#endif /* VIN_2CPE */
   FWM_SIG_DTMFGEN   sig_dtmfgen;
   IFX_uint16_t      nCpt;
   FWM_SIG_UTG       utg[TAPI_TONE_MAXRES];
   IFX_uint16_t      cid_sender[3];
} VINETIC_SIGCH;

/** Structure for the coder channel
   including firmware message cache */
typedef struct
{
   VINDSP_MODULE signal;
   FWM_COD_CH        cod_ch;
   /* local storage for the configured encoder. It is set in 'cod_ch' when the
      coder gets started and upstream is not muted */
   IFX_uint16_t      enc_conf;
   IFX_uint16_t      rtcp[14];
   IFX_boolean_t     rtcp_update;
   /** store currently configured SSRC */
   IFX_uint32_t         nSsrc;
   /** store currently configured Sequenz number */
   IFX_uint32_t         nSeqNr;
} VINETIC_CODCH;

/** Structure for the PCM channel
   including firmware message cache */
typedef struct
{
   VINDSP_MODULE signal;
   FWM_PCM_CH        pcm_ch;
   IFX_uint16_t      pcm_nelec;
} VINETIC_PCMCH;

/** CID sending structure */
typedef struct
{
   IFX_uint16_t pCmd [13];
   IFX_uint16_t pCid [128];
   IFX_TAPI_CID_HOOK_MODE_t  nCidType;
   IFX_vuint8_t nOdd;
   IFX_vuint8_t nCidCnt;
   IFX_vuint8_t nPos;
   IFX_vint32_t nState;
} VINETIC_CID;

/* DTMF transmission structure */
typedef struct
{
   /* Prevent multiple use */
   IFX_vint32_t       useCnt;
   /* DTMF transmission state */
   VINETIC_DTMF_STATE state;
   /* Callback function called on DTMF status changes */
   IFX_void_t         (*stateCb)(VINETIC_CHANNEL *pCh);
   /* Pointer to DTMF data to send */
   IFX_uint16_t       *pData;
   /* Number of DTMF words to send*/
   IFX_uint16_t  nWords;
   /* Number of DTMF word currently in transmission */
   IFX_uint16_t  nCurr;
   /* Number of DTMF words sent */
   IFX_uint16_t  nSent;
   /* Simplified mode (expect 8bit DTMF words in pDtmfData,
      only support digits 0-D, only supports high-level
      frequency generation mode) if set */
   IFX_boolean_t bByteMode;
} VINETIC_DTMF;

/** tone resource structure as time argument */
typedef struct
{
   /* timer id for voice path establisment */
   Timer_ID                Tone_Timer;
   VINETIC_CHANNEL* pCh;
   /** Resource number of the tone play unit */
   IFX_uint32_t        nRes;
}VINETIC_TONERES;

struct vinetic_interrupt_s
{
   /** OS/architecture specific IRQ number */
   IFX_int32_t             nIrq;
   /** "registered in OS" flag */
   IFX_boolean_t           bRegistered;
   /** interrupt enabled flag : IFX_TRUE=Enabled / IFX_FALSE=Disabled */
   volatile IFX_boolean_t  bIntEnabled;
#ifdef DEBUG_INT
   /* this is used to debug nested calls of
      Vinetic_IrqLockDevice/Vinetic_IrqUnlockDevice */
   IFX_int32_t             nCount;
#endif /* DEBUG */
   /** to lock against multiple enable/disable */
   IFXOS_mutex_t           intAcc;
   IFXOS_mutex_t           lockAcc;
   /* pointer to first vinetic device, registered for this irq */
   VINETIC_DEVICE          *pdev_head;
   /** pointer to next different irq */
   vinetic_interrupt_t     *next_irq;
};

/* capabilities structure */
struct _VINETIC_CAPABILITIES
{
   /* WLEC Support*/
   IFX_boolean_t    bWlec;
   /* Number of LEC resources per VINETIC device */
   IFX_uint8_t      nLecRes;
   /* Number of UTG resources per channel (== signaling module), either 1 or 2 */
   IFX_uint8_t      nUtgPerCh;
};

/* Channel structure */
struct _VINETIC_CHANNEL
{
   /* common fields with VINETIC_DEVICE structure */
#ifdef DEBUG
   IFX_uint32_t             magic;
#endif /* DEBUG */
   /* Actual  vinetic channel, starting with 1, a channel number 0 indicates
      the control device structure VINETIC_DEVICE */
   IFX_uint8_t              nChannel;
   /* tracking in use of the device */
   IFX_uint16_t             nInUse;
   /* flags for different purposes, see CH_FLAGS */
   IFX_uint32_t             nFlags;
   /* ptr to actual device */
   VINETIC_DEVICE           *pParent;
   /* GPIO Resource Structure */
   VINETIC_GPIO_RES         GpioRes;
   /* Cid Receiver packet structure */
   PACKET                   cidRxPacket;
   /* overall channel protection ( read/write/ioctl level)
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t            chAcc;
   /* wakeup queue for select on read */
   IFXOS_wakelist_t         SelRead;
   /** UTD signal detection type. channel number:utd number,
       needs protection */
   IFX_vuint32_t            nUtdSig[2];
   /* channel cid internal structure, need protection  */
   VINETIC_CID              cidSend;
   /* DTMF Generator Configuration */
   VINETIC_DTMF             dtmfSend;
   /* PCM channel mapping */
   IFX_uint8_t              nPcmCh;
   /* PCM channel maximum support Bit Mode (8-Bit or 16-Bit) */
   IFX_uint8_t             nPcmMaxResolution;
   /* host channel data structure, as defined in drv_vinetic_hostapi.h */
   VINETIC_HOST_CHANNEL     hostCh;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   /* voice streaming member */
   IFXOS_event_t            wqRead;
   IFXOS_wakelist_t         wqWrite;
   /* read packet function pointer */
   IFX_int32_t              (*if_read) (VINETIC_CHANNEL *pCh,
                                        IFX_uint8_t* buf, IFX_int32_t count);
   /* write packet function pointer */
   IFX_int32_t              (*if_write)(VINETIC_CHANNEL *pCh,
                                     const IFX_uint8_t* buf, IFX_int32_t count);
   /* Packet fifo, needs protection */
   FIFO                     rdFifo;
   PACKET*                  pFifo;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#ifdef EVALUATION
   IFX_void_t               *pEval;
#ifdef VIN_2CPE
   VOICESTREAM_CTRL_t       VsCtrl;
#endif /* VIN_2CPE */
#endif /* EVALUATION */
#ifdef TAPI
   TAPI_CONNECTION          *pTapiCh;
   VINETIC_TONERES          *pToneRes;
   /* timer id for tone service */
   /* predefined Tones could only be played on TG! */
   Timer_ID                 Tone_PredefTimer;
   /* tone counter for tone repetition. If 0 endlessly, TG only! */
   IFX_uint8_t              nTone_Cnt;
   /* step counter for ALM tone playout, TG only! */
   IFX_uint8_t              nToneStep;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   IFX_boolean_t            bVoiceConnect;
   IFX_TAPI_JB_CFG_t        VoiceJbConf;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   /* set flag when fax data are requested
      on downstream direction */
   volatile IFX_boolean_t   bFaxDataRequest;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
#endif /* TAPI */
#ifdef QOS_SUPPORT
   QOS_CTRL                 QosCrtl;
#endif /* QOS_SUPPORT */
#ifdef TAPI_LL_DRV
   TAPI_CHANNEL_t* pTESTTapiCh;
#endif /* TAPI_LL_DRV */
};

/* VINETIC global Device Stucture */
struct _VINETIC_DEVICE
{
#ifdef DEBUG
   IFX_uint32_t             magic;
#endif /* DEBUG */
   /* Vinetic channel. This value is always zero for the device structure.
      Different channel device structure are from type VINETIC_CHANNEL.
      The first field is also a nChannel. This gurantees that the channel
      field is always available, although the cast was wrong. But ensure
      the correct cast after reading the channel field */
   IFX_uint8_t              nChannel;
   /* tracking in use of the device */
   IFX_uint16_t             nInUse;
   /* device error status, never reset. read out for detail
      information if return value of function is not IFX_SUCCESS */
   IFX_int32_t              err;
   /* ALM Channel management structure */
   VINETIC_ALMCH            *pAlmCh;
   /* SIGNALLING Channel management structure */
   VINETIC_SIGCH            *pSigCh;
   /* PCM Channel management structure*/
   VINETIC_PCMCH            *pPcmCh;
   /* CODER Channel management structure*/
   VINETIC_CODCH            *pCodCh;
   /* physical base address for the vinetic device */
   IFX_uint32_t             nBaseAddr;
   /* access address for the vinetic device */
   IFX_vuint16_t            *pBaseAddr;
   /* absolute index of the device, when multible devices are existing
      For the first device the value is set to zero                        */
   IFX_int32_t               nDevNr;
   /* Status of device, see states defines .
      Must be protected against interrupts and concurrents tasks           */
   IFX_vuint32_t             nDevState;
   /* local storage for the interrupt flags that are returned during disabling
      the global interrupts. This value is used to enable the interrupts
      again */
   IFX_vuint32_t             nIrqMask;
   /* Additional flag for vxWorks if a real wakeup is needed */
   volatile IFX_boolean_t    bNeedWakeup;
   /* supported analog channels, read from REVISION register */
   IFX_uint8_t               nAnaChan;
   /* chip revision, read from REVISION register */
   IFX_uint8_t               nChipRev;
   /* chip major revision, set depending on REVISION register */
   IFX_uint8_t               nChipMajorRev;
   /* chip type, read from REVISION register */
   IFX_uint8_t               nChipType;
   /* maximum EDSP resources available */
   IFX_uint8_t               nMaxRes;
   /* number of EDSP coder available */
   IFX_uint8_t               nCoderCnt;
   /* number of EDSP signal detectors available */
   IFX_uint8_t               nSigCnt;
   /* number of PCM channel available */
   IFX_uint8_t               nPcmCnt;
#ifdef VIN_V14_SUPPORT
   /* Current running PCM Resolution mode */
   IFX_uint8_t                          nPcmResolution;
#endif /* VIN_V14_SUPPORT */
   /** number of maximum available tone generators */
   IFX_uint8_t               nToneGenCnt;
   /* bit array for available LEC resources in firmware */
   IFX_uint16_t              availLecRes;
   /* driver in polling/interrupt mode flag.
      Atomic variable, must be keep in memory because it
      can be modified in an asynchronous manner by some high priority tasks. */
   IFX_uint32_t              IrqPollMode;
   /* VINETIC mailbox concurent access protection mutex.
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t             mbxAcc;
   /* VINETIC share variables concurent access protection mutex.
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t             memberAcc;
   /* channel structures */
   VINETIC_CHANNEL           pChannel[VINETIC_MAX_CH_NR];
   /* wakeup queue for select on read */
   IFXOS_wakelist_t          EventList;
   /* configured PCM rate for chip */
   IFX_uint16_t              nPCMRate;
   /** GPIO Resource Structure */
   VINETIC_GPIO_RES          GpioRes;
   /* host device data structure, as defined in drv_vinetic_hostapi.h */
   VINETIC_HOST_DEVICE       hostDev;
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
/* merge : review usage and shift to host file if applicable */
   /* device packet read function pointer */
   IFX_int32_t              (*packet_read)(VINETIC_DEVICE *pDev,
                                           IFX_int32_t nSize);
   /* buffer management functions */
   IFX_uint8_t              (*getDataBuf) (VINETIC_DEVICE *pDev,
                                           IFX_uint16_t *pPacketCmd,
                                           FIFO **pFifo, PACKET **pPacket);
   IFX_void_t               (*putDataBuf) (VINETIC_DEVICE *pDev,
                                           IFX_uint16_t nCmd1, FIFO **pFifo,
                                           PACKET **pPacket);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#if 1
   /* variables for shared interrupt handling
      (see implementatiuon in drv_vinetic_vxworks.c) */
   vinetic_interrupt_t*     pIrq;
   VINETIC_DEVICE*          pInt_NextDev;
#else
   /* irq number */
   IFX_uint32_t             nIrq;
   /* interrupt enabled flag : IFX_TRUE=Enabled / IFX_FALSE=Disabled */
   IFX_vint32_t             bIntEnabled;
#endif
#ifdef ASYNC_CMD
   /* State of Mailbox, needs protection */
   IFX_vuint32_t            nMbxState;
   /* global buffer for command reading.
      Protected during any mailbox access */
   IFX_uint16_t             pCmdData [MAX_CMD_WORD];
   /* count of words read in the last mailbox outbox access */
   IFX_uint8_t              nCmdReadCnt;
   /* indicates the expected data to be in the outbox. CmdRead waits
      for it */
   IFX_uint8_t              nExpCmdData;
#endif /* ASYNC_CMD */
#ifdef TAPI
   /* entry to Tapi device, needs protection  */
   TAPI_DEV                 TapiDev;
   /* timeslot allocation management flags */
     /* value PcmRxTs need protection */
   IFX_uint32_t             PcmRxTs[PCM_HIGHWAY][PCM_TS_AARAY];
     /* value PcmTxTs need protection */
   IFX_uint32_t             PcmTxTs[PCM_HIGHWAY][PCM_TS_AARAY];
   /** stores the capability list for the device,
   IF to application-level */
   IFX_TAPI_CAP_t*          CapList;
   IFX_uint8_t              nMaxCaps;
   /* subelement to list all capabilities of this device
   for driver internal usage! */
   VINETIC_CAPABILITIES_t    VinCapabl;
#endif /* TAPI */
#ifdef VIN_POLL
   IFX_int32_t              *pFifo;
#endif /* VIN_POLL */
#ifdef EVALUATION
   IFX_void_t               *pEval;
#endif /* EVALUATION */
#ifdef TESTING
   /* firmware and chip test member */
   /* last command */
   IFX_uint16_t              lastCmd[2];
   IFX_int32_t               nErrLine;
   IFX_char_t                sErrFile[80];
   /* for general purposes, mainly for firmware and device testing */
   VINETIC_IO_DRVCFG         DrvCfg;
   IFX_int32_t               nTestCase;
   IFX_uint8_t               bTestStart;
   IFX_int32_t               nTestCnt;
   TEST_RESULTS              TestResult [MAX_TESTCH];
   TEST_PARM                 TestParm;
#endif /* TESTING */
};

/* ============================= */
/* Global variable declaration   */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* IOCTL specific functions, implemented in drv_vinetic_ioctl.c */
IFX_int32_t VINETIC_Common_Ioctl (VINETIC_CHANNEL *pCh, IFX_uint32_t iocmd,
                                  IFX_uint32_t ioarg);
#ifdef TAPI
IFX_int32_t VINETIC_Tapi_Ioctl   (VINETIC_CHANNEL *pCh, IFX_uint32_t iocmd,
                                  IFX_uint32_t ioarg, IFX_boolean_t bCtrlDev);
#endif /* TAPI */

/* Host functions, implemented in drv_vinetic_host.c */
/* Host data initialization functions */
IFX_int32_t VINETIC_Host_InitDevMembers   (VINETIC_DEVICE  *pDev);
IFX_int32_t VINETIC_Host_InitChMembers    (VINETIC_CHANNEL *pCh);
IFX_void_t  VINETIC_Host_ResetDevMembers  (VINETIC_DEVICE  *pDev);
IFX_int32_t VINETIC_Host_InitDevFWData    (VINETIC_DEVICE *pDev);
IFX_int32_t VINETIC_Host_SetupAccess      (VINETIC_DEVICE *pDev,
                                           VIN_ACCESS nAccessMode);
/* Host specific operations functions. */
IFX_int32_t VINETIC_Host_DefaultRegInit   (VINETIC_DEVICE *pDev);
IFX_int32_t VINETIC_Host_SwitchCmdMbxSize (VINETIC_DEVICE *pDev,
                                           IFX_boolean_t bMax);
IFX_int32_t VINETIC_Host_GetCmdMbxSize    (VINETIC_DEVICE *pDev,
                                           IFX_uint8_t *cmdmbx_size);
IFX_int32_t VINETIC_Host_Set_EdspIntMask  (VINETIC_CHANNEL *pCh,
                                           VIN_EDSP_Res_t   edspRes,
                                           IFX_boolean_t    nIsr,
                                           IFX_uint16_t     nRising,
                                           IFX_uint16_t     nFalling);
/* Host ioctl functions. */
IFX_int32_t VINETIC_Host_AccessTest       (VINETIC_DEVICE *pDev,
                                           IFX_uint16_t max_val);
IFX_int32_t VINETIC_Host_InitChip         (VINETIC_DEVICE *pDev,
                                           VINETIC_IO_INIT *pInit);

/* VINETIC OS specific functions, implemented in drv_vinetic_<os>.c */
IFX_void_t Vinetic_IrqLockDevice           (VINETIC_DEVICE *pDev);
IFX_void_t Vinetic_IrqUnlockDevice         (VINETIC_DEVICE *pDev);
IFX_void_t Vinetic_IrqUnlockGlobal         (VINETIC_DEVICE *pDev);
IFX_int32_t OS_Init_Baseadress             (VINETIC_DEVICE* pDev,
                                            IFX_uint32_t nBaseAddr);
IFX_void_t  OS_Install_VineticIRQHandler   (VINETIC_DEVICE* pDev,
                                            IFX_int32_t nIrq);
IFX_void_t  OS_UnInstall_VineticIRQHandler (VINETIC_DEVICE* pDev);
IFX_int32_t OS_GetDevice                   (IFX_int32_t nr,
                                            VINETIC_DEVICE** pDev);
#ifndef OS_MapBuffer
IFX_void_t* OS_MapBuffer                   (IFX_void_t* p_buffer,
                                            IFX_uint32_t size);
#endif /* OS_MapBuffer */
#ifndef OS_UnmapBuffer
IFX_void_t  OS_UnmapBuffer                 (IFX_void_t* p_buffer);
#endif /* OS_UnmapBuffer */
#ifdef NO_OS
IFX_int32_t VINETIC_No_OS_InstallDriver    (IFX_void_t);
IFX_void_t  VINETIC_No_OS_UninstallDriver  (IFX_void_t);
#endif /* NO_OS */

/* VINETIC API functions, implemented in drv_vinetic_api.c  */
IFX_int32_t CmdWrite       (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd,
                            IFX_uint8_t count);
IFX_int32_t CmdWriteIsr    (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd,
                            IFX_uint8_t count);
IFX_int32_t CmdRead        (VINETIC_DEVICE *pDev, IFX_uint16_t* pCmd,
                            IFX_uint16_t* pData,  IFX_uint8_t count);

#ifdef ASYNC_CMD
IFX_void_t CmdReadIsr      (VINETIC_DEVICE *pDev);
#endif /* ASYNC_CMD */
#ifdef TESTING
IFX_void_t PrintErrorInfo  (VINETIC_DEVICE *pDev);
#endif /* TESTING */

extern void cpb2dw(IFX_uint32_t* pDWbuf, IFX_uint8_t* pBbuf, IFX_uint32_t nB);
extern void cpb2w (IFX_uint16_t* pWbuf,IFX_uint8_t *pBbuf, IFX_uint32_t nB);


#endif /* _DRV_VINETICAPI_H */


