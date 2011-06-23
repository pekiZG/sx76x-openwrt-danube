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
#include "drv_vinetic.h"
#include "drv_vinetic_gpio.h"
#ifdef QOS_SUPPORT
#include "drv_qos.h"
#endif /* QOS_SUPPORT */

/*
   list of possible features to check against VIN_CFG_FEATURES
*/

/* support for Vinetic 4S */
#define VIN_FEAT_VIN_S                          0x00000001
/* support for Vinetic 4M */
#define VIN_FEAT_VIN_M                          0x00000002
/* support for Vinetic 4C */
#define VIN_FEAT_VIN_C                          0x00000004

/* support for line testing */
#define VIN_FEAT_LT                             0x00000100
/* support for GR909 tests */
#define VIN_FEAT_GR909                          0x00000200
/* support for voice connections */
#define VIN_FEAT_VOICE                          0x00000400
/* support for FAX connections */
#define VIN_FEAT_FAX_T38                        0x00000800
/* support for packet handling (AAL/RTP) */
#define VIN_FEAT_PACKET                         0x00001000

/* ---------------------- */

#ifndef VIN_CFG_FEATURES
/* if not already set, define features depending on TAPI and other defines */

/* this is used to get a compact driver,
   that supports only Vinetic 4S with PCM connected */
#ifdef VIN_S_SUPPORT_ONLY
#define VIN_CFG_ADD_FEAT_VIN_TYPES  VIN_FEAT_VIN_S
#else
#define VIN_CFG_ADD_FEAT_VIN_TYPES  (VIN_FEAT_VIN_S | VIN_FEAT_VIN_M | VIN_FEAT_VIN_C)
#endif

#ifdef TAPI_VOICE
#ifdef VIN_S_SUPPORT_ONLY
#error TAPI_VOICE not possible for Vinetic S
#endif
#define VIN_CFG_ADD_FEAT_VOICE   (VIN_FEAT_VOICE | VIN_FEAT_PACKET)
#else
#define VIN_CFG_ADD_FEAT_VOICE   0
#endif

#ifdef TAPI_FAX_T38
#ifdef VIN_S_SUPPORT_ONLY
#error TAPI_FAX_T38 not possible for Vinetic S
#endif
#define VIN_CFG_ADD_FEAT_FAX_T38 (VIN_FEAT_FAX_T38 | VIN_FEAT_PACKET)
#else
#define VIN_CFG_ADD_FEAT_FAX_T38 0
#endif

#ifdef TAPI_LT
#define VIN_CFG_ADD_FEAT_LT      VIN_FEAT_LT
#else
#define VIN_CFG_ADD_FEAT_LT      0
#endif

#if defined (TAPI_GR909) && defined(VIN_V14_SUPPORT)
#define VIN_CFG_ADD_FEAT_GR909   VIN_FEAT_GR909
#else
#ifdef TAPI_GR909
#warning "GR909 is only available for Vinetic V1.x"
#endif
#define VIN_CFG_ADD_FEAT_GR909   0
#endif

#define VIN_CFG_FEATURES      (  VIN_CFG_ADD_FEAT_VIN_TYPES \
                              |  VIN_CFG_ADD_FEAT_VOICE     \
                              |  VIN_CFG_ADD_FEAT_FAX_T38   \
                              |  VIN_CFG_ADD_FEAT_LT        \
                              |  VIN_CFG_ADD_FEAT_GR909 )

#endif /* VIN_CFG_FEATURES */


#ifdef TAPI
#include "drv_tapi.h"
#endif /* TAPI */

#ifdef VIN_POLL
#include "lib_fifo.h"
#endif /* VIN_POLL */


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

#ifdef TAPI
   /* PCM timeslot allocation management definitions */
   #define PCM_MAX_TS            127
   #define PCM_TS_AARAY          ((PCM_MAX_TS + 3) / 32)
   #define PCM_HIGHWAY           2
#endif /* #ifdef TAPI */

/* fifo element number for packet mailbox events */
#define POBX_FIFO_SIZE        12
/* each fifo element has size of maximum packet plus 4 words timestamp */
#define POBX_ELEMENT_SIZE     (MAX_PACKET_WORD + 4)
/* Tapi Version */
#define TAPI_VERS             0x01000002
/* default payload type for RTP event packets */
#define DEFAULT_EVTPT         0x62

/* ============================= */
/* VINETIC Common defines        */
/* ============================= */

#define VINETIC_MINOR_BASE       10
#define DEV_NAME          "VINETIC"
#define VINETIC_MAJOR           230

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

/* used by drv_vinetic_dwnld.c */

/* timeout in ms for mailbox emty event in ms */
#define WAIT_MBX_EMPTY                    100
/* timeout in ms for EDSP Ready event in ms */
#define WAIT_EDSP                         50

/* PHI CRC time to wait. According to data sheet wait approximatively 0,256 s */
#define PHI_CRC_TIME                      300
/* time to wait after Edsp Start Sc, in ms.
   The appropriate time to wait for it isn't yet stated by any specification,
   although not waiting leads to EDSP starting failure, coz the EDSP needs
   some time to setup after processing the Start Edsp Short Command */
#define WAIT_AFTER_START_EDSP             30

/* used in drv_vinetic_lt.c */
/* DC Control needs about 500us to process (2kHz clock). In the worst case a
  the DC Control will react one cycle later. In addition we have to wait two
  times (read/write) the CARMEL DSP reaction time of 125 us.  This means
  that the minimum time to wait is 1250 us */
#define DCDSP_DELAY                              1300
#define DCDSP_DELAY_V21                          1375

/* ============================= */
/* VINETIC flag settings         */
/* ============================= */

/* this value is used for variables which should have a defined
   content. Matches for bytes and replaces -1 to use variable space. */
#define NOTVALID               0xff

/* ============================= */
/* Global Macros                 */
/* ============================= */

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
#endif

/*******************************************************************************
Description:
   sends a short command and reads appropriate data from vinetic
Arguments:
   cmd   : short command to read the vinetic
   pData : buffer containing the read data
   count : number of word awaited in reading
Return:
   IFX_SUCCESS if no error, otherwise IFX_ERROR
Remarks:
   Not protected from interrupts and by a mutex. so this macro can
   be used in interrupt.
   For the TESTING variant, it is necessary to know
   that it may be a 2-line command!
*******************************************************************************/
#ifdef TESTING
#define SC_READ(cmd, pData, count)           \
   pDev->read (pDev, (cmd), pData, count);   \
   pDev->lastSC = cmd
#else
#define SC_READ(cmd, pData, count)           \
   pDev->read (pDev, (cmd), (pData), (count))
#endif /* TESTING */

/*******************************************************************************
Description:
   write a short command to vinetic
Arguments:
   cmd  : short command to write to the vinetic
Return:
   none
Remarks:
  Not protected from interrupts and by a mutex. so this macro can
  be used in interrupt.
*******************************************************************************/
#ifdef TESTING
#define SC_WRITE(cmd)                                       \
do {                                                        \
   pDev->lastSC = (cmd);                                    \
   if (pDev->write (pDev, &pDev->lastSC, 1) == IFX_ERROR)   \
   {  SET_ERROR(ERR_DEV_ERR);  }                            \
}while(0)
#else
#define SC_WRITE(cmd)                                       \
do {                                                        \
   IFX_uint16_t nCmd = (cmd);                               \
   if (pDev->write (pDev, &nCmd, 1) == IFX_ERROR)           \
   {  SET_ERROR(ERR_DEV_ERR);  }                            \
}while(0)

#endif /* TESTING */

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

/*******************************************************************************
Description:
   copy a byte buffer into a word buffer respecting the endianess.
Argiments
   pWbuf  : Word buffer
   pBbuf  : Byte buffer
   nB     : size of Bytes to be copied
Return:
Remarks:
   No matter if muP supports big endian or little endian, this macro will still
   copy the data in the apropriate way for a big endian target chip like Vinetic
*******************************************************************************/
#define cpb2w(pWbuf,pBbuf,nB)                      \
{                                                  \
   IFX_int32_t i = 0, nW = (nB)>>1;                \
   IFX_uint16_t* pTmp = (IFX_uint16_t *)(pBbuf);   \
   for ( i = 0; i < nW; i++ )                      \
   {                                               \
      (pWbuf)[i] = SWAP_UINT16(pTmp[i]);           \
   }                                               \
}

/*******************************************************************************
Description:
   copy a byte buffer into a dword buffer respecting the endianess.
Argiments
   pDWbuf  : Word buffer
   pBbuf  : Byte buffer
   nB     : size of Bytes to be copied
Return:
Remarks:
   No matter if muP supports big endian or little endian, this macro will still
   copy the data in the apropriate way for a big endian target chip like Vinetic
*******************************************************************************/
#define cpb2dw(pDWbuf,pBbuf,nB)                    \
{                                                  \
   IFX_int32_t i = 0, nDW = (nB)>>2;               \
   IFX_uint32_t *pTmp = (IFX_uint32_t *)(pBbuf);   \
   for ( i = 0; i < nDW; i++ )                     \
   {                                               \
      (pDWbuf)[i] = SWAP_UINT32(pTmp[i]);          \
   }                                               \
}

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
#define VIN_ENABLE_IRQLINE(irq)       IFXOS_IRQ_ENABLE(irq)
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
#define   DS_FW_DLD   0x00000001
/* phi downloaded successfully, no further download will be allowed */
#define   DS_PHI_DLD     0x00000002
/* DTMF sure detected tons masked (MSRE1) */
#define   DS_DTMF_SURE   0x000004
/* DTMF probably detected tons masked (MSRE1)*/
#define   DS_DTMF_PROB   0x000008
/* CRAM Downloaded */
#define   DS_CRAM_DLD    0x000010
/* Basic Init done */
#define   DS_BASIC_INIT  0x000020
/* Tapi Init done */
#define   DS_TAPI_INIT   0x000040
/* interrupt installed */
/* #define   DS_INT_ENABLED   0x000080 */
#if defined (ASYNC_CMD) && 1
/* command data was previously requested and maybe
   in command outbox. This flag will be reset if the data has been read */
#define   DS_CMDREQ     0x000100
/* command data has been read and is available in pCmdData. This flag is
   used in CmdRead to check if requested data was read in another context */
#define   DS_CMDREAD    0x000200
/* a read command was sent to the inbox. If another location reads out all
   mailbox content it will be signaled with DS_CMDREAD */
#define   DS_RDCMD_SENT    0x000400
#endif /* ASYNC_CMD */
/** device successfully initialized */
#define   DS_DEV_INIT      0x000800
/** Coder module enabled  */
#define   DS_COD_EN        0x001000
/** Analog module enabled  */
#define   DS_ALM_EN        0x002000
/** signaling module enabled  */
#define   DS_SIG_EN        0x004000
/** PCM module enabled  */
#define   DS_PCM_EN        0x008000
/** DC Download done */
#define DS_DC_DWLD         0x010000
/** AC Download done */
#define DS_AC_DWLD         0x020000
/** Qos succesfully initialized */
#define   DS_QOS_INIT      0x040000

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
#define VIN_VOICE_POLL     (1 << 0) /* 1 */
#define VIN_EVENT_POLL     (1 << 1) /* 2 */
#define VIN_TAPI_WAKEUP    (1 << 2) /* 4 */

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

/* ============================= */
/* Global Structures             */
/* ============================= */

typedef struct _VINETIC_CHANNEL VINETIC_CHANNEL;
typedef struct _VINETIC_DEVICE VINETIC_DEVICE;
typedef struct vinetic_interrupt_s vinetic_interrupt_t;

#ifdef TAPI
/* line testing configuration structure */
typedef struct
{
   IFX_boolean_t bRampEnable;
   IFX_boolean_t bRampReverse;
   IFX_int32_t   nIntegrationTime;
} VINETIC_LT_CONF;

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
typedef struct
{
   /* save/restore flag */
   IFX_boolean_t bReg;
   /* register set */
   IFX_uint16_t nIOCTL1;
   IFX_uint16_t nBCR1;
   IFX_uint16_t nIK1;
   IFX_uint16_t nRMS;
   IFX_uint16_t nRO1;
   IFX_uint16_t nLMDC;
   IFX_uint16_t nRGFR;
   IFX_uint16_t nBCR2;
   IFX_uint16_t nLMCR;
   IFX_uint16_t nRTR;
   IFX_uint16_t nOFR;
   IFX_uint16_t nOPMODE;
   IFX_uint16_t pMSRS [2]; /*SRS1 mask registers */
} VINETIC_GR909_REG;

typedef struct
{
   /* state machine queue */
   IFXOS_event_t      wqState;
   /* register backup/restore */
   VINETIC_GR909_REG  reg;
   /* GR909 IO values */
   VINETIC_IO_GR909   Usr;
   /* bolean for data availability */
   IFX_boolean_t      bData;
   /* actual measurement state */
   IFX_uint32_t       nState;
   /* gr909 task id */
   IFX_int32_t        gr909tskId;
   /* callback function for actual gr909 state machine */
   IFX_int32_t       (* Gr909Call) (VINETIC_CHANNEL* pCh);
} VINETIC_GR909;

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
#endif /* TAPI */

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
}VINDSP_MODULE_SIGNAL;

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
   IFX_char_t* name;
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
   FWM_SIG_XTD       sig_utd2;
   FWM_SIG_DTMFGEN   sig_dtmfgen;
   IFX_uint16_t      nCpt;
   FWM_SIG_UTG       utg;
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
} VINETIC_CODCH;

/** Structure for the PCM channel
   including firmware message cache */
typedef struct
{
   VINDSP_MODULE signal;
   FWM_PCM_CH        pcm_ch;
   IFX_uint16_t      pcm_nelec;
} VINETIC_PCMCH;

typedef struct _VINETIC_OFFSET_COMP
{
   IFX_int16_t          nSlic;
   IFX_int16_t          nPrefi;
#ifdef VIN_V21_SUPPORT
   IFX_int16_t          nSlicPregain;
#endif /* VIN_V21_SUPPORT */
   IFX_int16_t          bInitialized;
} VINETIC_OFFSET_COMP;


/* CID sending states */
enum
{
   CID_SETUP,
   CID_PREPARE,
   CID_TRANSMIT,
   CID_OFFHOOK,
   CID_TRANSMIT_ERR
};

typedef enum
{
   /* CID with ringing : type 1 */
   CID_TYPE_1,
   /* CID with ringing japan : type 1*/
   CID_TYPE_1_NTT,
   /* CID while speaking : type 2 */
   CID_TYPE_2
} VINETIC_CID_TYPE;

/* CID sending structure */
typedef struct
{
   IFX_uint16_t nCidIrMask;
   IFX_uint16_t pCmd [13];
   IFX_uint16_t pCid [128];
   VINETIC_CID_TYPE  nCidType;
   IFX_vuint8_t nOdd;
   IFX_vuint8_t nCidCnt;
   IFX_vuint8_t nPos;
   IFX_vint32_t nState;
} VINETIC_CID;

/* DTMF transmission status
Note: The states ABORT and ERROR indicate the status of the /last/ DTMF
      transmission - a new transmission can be started right away without
      restriction. */
typedef enum
{
   DTMF_READY,    /* DTMF generator idle */
   DTMF_START,    /* DTMF generator activated, waiting on first REQ */
   DTMF_TRANSMIT, /* DTMF generator transmitting */
   DTMF_ABORT     /* DTMF transmission aborted (either by user or underrun) */
} VINETIC_DTMF_STATE;

/* DTMF transmission structure */
typedef struct
{
   IFX_vint32_t useCnt;    /* Prevent multiple use */
   VINETIC_DTMF_STATE state; /* DTMF transmission state */
   IFX_void_t   (*stateCb)(VINETIC_CHANNEL *pCh);
                           /* Callback function called on DTMF status changes */
   IFX_uint16_t *pData;    /* Pointer to DTMF data to send */
   IFX_uint16_t  nWords;   /* Number of DTMF words to send*/
   IFX_uint16_t  nCurr;    /* Number of DTMF word currently in transmission */
   IFX_uint16_t  nSent;    /* Number of DTMF words sent */
   IFX_boolean_t bByteMode;  /* Simplified mode (expect 8bit DTMF words in pDtmfData,
                        only support digits 0-D, only supports high-level
                        frequency generation mode) if set */
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

/* Device structure for channel */
struct _VINETIC_CHANNEL
{
#ifdef DEBUG
   IFX_uint32_t             magic;
#endif /* DEBUG */
   /* common fields with VINETIC_DEVICE structure */
   /* Actual  vinetic channel, starting with 1, a channel number 0 indicates
      the control device structure VINETIC_DEVICE */
   IFX_uint8_t              nChannel;
   /* tracking in use of the device */
   IFX_uint16_t             nInUse;
   /* flags for different purposes, see CH_FLAGS */
   IFX_uint32_t             nFlags;
   /* register device event flags, must be keep in memory because it
      can be modified in an asynchronous manner by the interrupt routine
      or a high priority tasks
   */
   IFX_vuint16_t           IsrFlag [MAX_CH_EVENTS]; /* <-- need protection */
   /* ptr to actual device */
   VINETIC_DEVICE          * pParent;
   /* Channel specific fields */
   /* Queues */
   IFXOS_event_t           wqLT_LM_OK;
   IFXOS_event_t           wqLT_RAMP_RDY;
   /**************************************************************************/
   /* voice streaming member                                                 */
   /**************************************************************************/
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   IFXOS_event_t           wqRead;
   IFXOS_event_t           wqWrite;
   /* read packet function pointer */
   IFX_int32_t                     (* if_read) (VINETIC_CHANNEL *pCh,
                                          IFX_uint8_t* buf, IFX_int32_t count);
   /* write packet function pointer */
   IFX_int32_t                     (* if_write) (VINETIC_CHANNEL *pCh,
                                          const IFX_uint8_t* buf, IFX_int32_t count);
   FIFO                    rdFifo;      /* <-- need protection */
   PACKET*                 pFifo;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   PACKET                  cidRxPacket;
   /* overall channel protection ( read/write/ioctl level)
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t           chAcc;
   /* wakeup queue for select on read */
   IFXOS_wakelist_t        SelRead;
   /* Firmware Message handling structure */
   /** UTD signal detection type. channel number:utd number */
   IFX_vuint32_t           nUtdSig[2];  /* <-- need protection */
   /* channel cid internal structure */
   VINETIC_CID             cidSend;     /* <-- need protection */
   /* GPIO Resource Structure */
   VINETIC_GPIO_RES        GpioRes;
   /* DTMF Generator Configuration */
   VINETIC_DTMF            dtmfSend;
   /* PCM channel mapping */
   IFX_uint8_t             nPcmCh;
#ifdef TAPI
   TAPI_CONNECTION         *pTapiCh;
   VINETIC_TONERES*        pToneRes;
   /* timer id for tone service */
   Timer_ID                Tone_PredefTimer;
   /* tone counter for tone repetition. If 0 endlessly */
   IFX_uint8_t             nTone_Cnt;
   /* step counter for ALM tone playout */
   IFX_uint8_t             nToneStep;
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   IFX_boolean_t           bVoiceConnect;
   TAPI_JB_CONF            VoiceJbConf;
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   /* line testing parameter */
   VINETIC_LT_CONF         ltConf;
   VINETIC_OFFSET_COMP     offset;
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
   /* GR909 test parameter */
   VINETIC_GR909           Gr909;        /* <-- need protection */
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */
#endif /* TAPI */
#ifdef QOS_SUPPORT
   QOS_CTRL                QosCrtl;
#endif /* QOS_SUPPORT */
   IFX_uint16_t            regSRE2; /* value of SRE2 register updated upon each ISR2 interrupt */
   /* Register initialization values */
   IFX_uint16_t            initIOCTL1;
   IFX_uint16_t            initIOCTL2;
   IFX_uint16_t            initMF_SRS1;
   IFX_uint16_t            initMR_SRS1;
   /* Channel specific registers */
   LMRES_REG_TYPE          regLMRES;
   CCHKR_REG_TYPE          regCCHKR;
   IOCTL1_REG_TYPE         regIOCTL1;
   IOCTL2_REG_TYPE         regIOCTL2;
   BCR1_REG_TYPE           regBCR1;
   BCR2_REG_TYPE           regBCR2;
   DSCR_REG_TYPE           regDSCR;
   LMCR_REG_TYPE           regLMCR;
   RTR_REG_TYPE            regRTR;
   OFR_REG_TYPE            regOFR;
   AUTOMOD_REG_TYPE        regAUTOMOD;
   TSTR1_REG_TYPE          regTSTR1;
   TSTR2_REG_TYPE          regTSTR2;
   TSTR3_REG_TYPE          regTSTR3;
   MR_SRS1_REG_TYPE        regMR_SRS1;
   MR_SRS2_REG_TYPE        regMR_SRS2;
   MF_SRS1_REG_TYPE        regMF_SRS1;
   MF_SRS2_REG_TYPE        regMF_SRS2;
   OPMOD_CUR_REG_TYPE      regOPMOD_CUR;
   TG1F_REG_TYPE           regTG1F;
   BP1F_REG_TYPE           regBP1F;
   TG2F_REG_TYPE           regTG2F;
   BP2F_REG_TYPE           regBP2F;
   TG1A_REG_TYPE           regTG1A;
   TG2A_REG_TYPE           regTG2A;
   BP12Q_REG_TYPE          regBP12Q;
   BP3F_REG_TYPE           regBP3F;
   RGFR_REG_TYPE           regRGFR;
   LMAC_REG_TYPE           regLMAC;
   LMDC_REG_TYPE           regLMDC;
   V1_REG_TYPE             regV1;
   RGTP_REG_TYPE           regRGTP;
   CREST_REG_TYPE          regCREST;
   RO1_REG_TYPE            regRO1;
   RO2_REG_TYPE            regRO2;
   RO3_REG_TYPE            regRO3;
   RGD_REG_TYPE            regRGD;
   I2_REG_TYPE             regI2;
   VLIM_REG_TYPE           regVLIM;
   VK1_REG_TYPE            regVK1;
   IK1_REG_TYPE            regIK1;
   VRTL_REG_TYPE           regVRTL;
   HPD_REG_TYPE            regHPD;
   HACT_REG_TYPE           regHACT;
   HRING_REG_TYPE          regHRING;
   HACRT_REG_TYPE          regHACRT;
   HLS_REG_TYPE            regHLS;
   HMW_REG_TYPE            regHMW;
   HAHY_REG_TYPE           regHAHY;
   FRTR_REG_TYPE           regFRTR;
   CRAMP_REG_TYPE          regCRAMP;
#ifdef VIN_V21_SUPPORT
   LMRES2_REG_TYPE         regLMRES2;
   LMCR2_REG_TYPE          regLMCR2;
   LM_THRES_REG_TYPE       regLM_THRES;
   LM_RCLOWTHRES_REG_TYPE  regLM_RCLOWTHRES;
   LM_ITIME_REG_TYPE       regLM_ITIME;
   LM_DELAY_REG_TYPE       regLM_DELAY;
   LM_SHIFT_REG_TYPE       regLM_SHIFT;
   LM_FILT1_REG_TYPE       regLM_FILT1;
   LM_FILT2_REG_TYPE       regLM_FILT2;
   LM_FILT3_REG_TYPE       regLM_FILT3;
   LM_FILT4_REG_TYPE       regLM_FILT4;
   LM_FILT5_REG_TYPE       regLM_FILT5;
#endif /* VIN_V21_SUPPORT */
};


struct vinetic_interrupt_s
{
   /** OS/architecture specific IRQ number */
   IFX_int32_t             nIrq;
   /** "registered in OS" flag */
   IFX_boolean_t           bRegistered;
   /** interrupt enabled flag : IFX_TRUE=Enabled / IFX_FALSE=Disabled */
   IFX_boolean_t           bIntEnabled;
#ifdef DEBUG_INT
   /* this is used to debug nested calls of
      Vinetic_IrqLockDevice/Vinetic_IrqUnlockDevice */
   IFX_int32_t             nCount;
#endif /* DEBUG */
   /** to lock against multiple enable/disable */
   IFXOS_mutex_t           intAcc;
   /* pointer to first vinetic device, registered for this irq */
   VINETIC_DEVICE          *pdev_head;
   /** pointer to next different irq */
   vinetic_interrupt_t     *next_irq;
};


/* VINETIC global Device Stucture */
struct _VINETIC_DEVICE
{
#ifdef DEBUG
   IFX_uint32_t                         magic;
#endif /* DEBUG */
   /* Vinetic channel. This value is always zero for the device structure.
      Different channel device structure are from type VINETIC_CHANNEL.
      The first field is also a nChannel. This gurantees that the channel
      field is always available, although the cast was wrong. But ensure
      the correct cast after reading the channel field */
   IFX_uint8_t                           nChannel;
   /* tracking in use of the device */
   IFX_uint16_t                          nInUse;
   /* device error status, never reset. read out for detail
      information if return value of function is not IFX_SUCCESS */
   IFX_int32_t                           err;
   VINETIC_ALMCH* pAlmCh;
   VINETIC_SIGCH* pSigCh;
   VINETIC_PCMCH* pPcmCh;
   VINETIC_CODCH* pCodCh;
   /*** Board access parameters ****/
   /* physical base address for the vinetic device (from basic config) */
   IFX_uint32_t                          nBaseAddr;
   /* ioremap address for the vinetic base address */
   IFX_vuint16_t                         *pBaseAddr;
   /* address pointer for EOM and NWD of VINETIC. */
   IFX_vuint16_t                         *pEom;
   IFX_vuint16_t                         *pNwd;
   /* ioremap address for the vinetic Interrupt Register */
   IFX_vuint16_t                         *pIntReg;
   /* device read function pointer */
   IFX_int32_t                 (* read) (VINETIC_DEVICE *pDev,
                                 IFX_uint16_t cmd, IFX_uint16_t *pData, IFX_int32_t count);
   /* device write function pointer */
   IFX_int32_t                (* write) (VINETIC_DEVICE *pDev,
                                 IFX_uint16_t* pData, IFX_int32_t count);
   /* device DIOP read function pointer */
   IFX_int32_t            (* diop_read) (VINETIC_DEVICE *pDev,
                                 IFX_uint16_t *pCmd, IFX_uint16_t *pData, IFX_int32_t count);
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   /* device packet read function pointer */
   IFX_int32_t          (* packet_read) (VINETIC_DEVICE *pDev, IFX_int32_t nSize);

   IFX_uint8_t         (* getDataBuf) (VINETIC_DEVICE *pDev,
                                IFX_uint16_t *pPacketCmd, FIFO **pFifo,
                                PACKET **pPacket);
   IFX_void_t          (*putDataBuf) (VINETIC_DEVICE *pDev,
                                IFX_uint16_t nCmd1, FIFO **pFifo,
                                PACKET **pPacket);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
#ifdef VIN_SHARED_INTERRUPT
   /* variables for shared interrupt handling
      (see implementatiuon in dev_vinetic_vxworks.c) */
   vinetic_interrupt_t*                  pIrq;
   VINETIC_DEVICE*                       pInt_NextDev;
#else
   /* interrupt specific data */
   /* irq number */
   IFX_uint32_t                          nIrq;
   /* interrupt enabled flag : IFX_TRUE=Enabled / IFX_FALSE=Disabled */
   IFX_int32_t                           bIntEnabled;
#endif
   /* local storage for the interrupt flags that are returned during disabling
      the global interrupts. This value is used to enable the interrupts
      again */
   IFX_uint32_t                          nIrqMask;
   /* Active mode to use on this device */
   IFX_uint16_t                          nActiveMode;
   /* absolute index of the device, when multible devices are existing
      For the first device the value is set to zero                        */
   IFX_int32_t                           nDevNr;
   /* Status of device, see states defines .
      Must be protected against interrupts and concurrents tasks           */
   IFX_vuint32_t                         nDevState;
   /* Additional flag for vxWorks if a real wakeup is needed */
   volatile IFX_boolean_t                bNeedWakeup;

   /* supported analog channels, read from REVISION register */
   IFX_uint8_t                          nAnaChan;
   /* chip revision, read from REVISION register */
   IFX_uint8_t                          nChipRev;
   /* chip type, read from REVISION register */
   IFX_uint8_t                          nChipType;
   /* chip major revision, set depending on REVISION register */
   IFX_uint8_t                          nChipMajorRev;
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   /* EDSP features and version number */
   IFX_uint16_t                          nEdspVers;
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
   /* maximum EDSP resources available */
   IFX_uint8_t                          nMaxRes;
   /* number of EDSP coder available */
   IFX_uint8_t                          nCoderCnt;
   /* number of EDSP signal detectors available */
   IFX_uint8_t                          nSigCnt;
   /* number of PCM channel available */
   IFX_uint8_t                          nPcmCnt;
   /** number of maximum available tone generators */
   IFX_uint8_t                          nToneGenCnt;
   /* bit array for available LEC resources in firmware */
   IFX_uint16_t                         availLecRes;
   /* driver in polling/interrupt mode flag.
      Atomic variable, must be keep in memory because it
      can be modified in an asynchronous manner by some high priority tasks. */
   IFX_uint32_t                        IrqPollMode;
   /* VINETIC mailbox concurent access protection mutex.
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t                        mbxAcc;
   /* VINETIC share variables concurent access protection mutex.
      PS: Avoid nested locking of this mutex. It can lead to a deadlock */
   IFXOS_mutex_t                        memberAcc;
   /* register device event flags, must be keep in memory because it
      can be modified in an asynchronous manner by the interrupt routine
      or a high priority tasks
   */
   IFX_vuint16_t                        IsrFlag [MAX_IRQ_EVENTS];
   /* channel structures */
   VINETIC_CHANNEL                      pChannel[VINETIC_CH_NR];
   /* wakeup queue for select on read */
   IFXOS_wakelist_t                     EventList;
#ifdef ASYNC_CMD
   /* State of Mailbox */
   IFX_vuint32_t                        nMbxState;
   /* global buffer for command reading.
      Protected during any mailbox access */
   IFX_uint16_t                         pCmdData [MAX_CMD_WORD];
   /* count of words read in the last mailbox outbox access */
   IFX_uint8_t                          nCmdReadCnt;
   /* indicates the expected data to be in the outbox. CmdRead waits
      for it */
   IFX_uint8_t                          nExpCmdData;
#endif /* ASYNC_CMD */
#ifdef TAPI
   /* entry to Tapi device */
   TAPI_DEV                             TapiDev; /* <-- need protection */
   /* timeslot allocation management flags */
     /* value PcmRxTs need protection */
   IFX_uint32_t                         PcmRxTs[PCM_HIGHWAY][PCM_TS_AARAY];
     /* value PcmTxTs need protection */
   IFX_uint32_t                         PcmTxTs[PCM_HIGHWAY][PCM_TS_AARAY];
   /** stores the capability list for the device */
   TAPI_PHONE_CAPABILITY*               CapList;
   IFX_uint8_t                          nMaxCaps;
#endif /* TAPI */
#ifdef TESTING
   /* firmware and chip test member */
   /* last command */
   IFX_uint16_t                          lastCmd[2];
   IFX_uint16_t                          lastSC;
   IFX_int32_t                           nErrLine;
   IFX_char_t                            sErrFile[80];
#endif /* TESTING */
   GCR1_REG_TYPE                         regGCR1;
   GCR2_REG_TYPE                         regGCR2;
   MRSRGPIO_REG_TYPE                     regMRSRGPIO;
   MFSRGPIO_REG_TYPE                     regMFSRGPIO;
   IFX_uint16_t                          regMFSRE1[8];
   IFX_uint16_t                          regMFSRE2[8];
   IFX_uint16_t                          regMRSRE1[8];
   IFX_uint16_t                          regMRSRE2[8];
#ifdef VIN_POLL
   IFX_int32_t                           *pFifo;
#endif /* VIN_POLL */
   /** GPIO Resource Structure */
   VINETIC_GPIO_RES              GpioRes;
};

/* ============================= */
/* Global variable declaration   */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

extern IFX_void_t Vinetic_IrqLockDevice(VINETIC_DEVICE *pDev);
extern IFX_void_t Vinetic_IrqUnlockDevice(VINETIC_DEVICE *pDev);
extern IFX_void_t Vinetic_IrqUnlockGlobal(VINETIC_DEVICE *pDev);
extern IFX_int32_t VINETIC_SelectCh(VINETIC_CHANNEL* pCh, IFX_int32_t node, IFX_int32_t opt);
extern IFX_int32_t OS_Init_Baseadress(VINETIC_DEVICE* pDev, IFX_uint32_t nBaseAddr);
#ifndef OS_MapBuffer
extern IFX_void_t* OS_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size);
#endif /* OS_MapBuffer */
#ifndef OS_UnmapBuffer
extern IFX_void_t OS_UnmapBuffer(IFX_void_t* p_buffer);
#endif /* OS_UnmapBuffer */
extern IFX_void_t OS_Install_VineticIRQHandler(VINETIC_DEVICE* pDev, IFX_int32_t nIrq);
extern IFX_void_t OS_UnInstall_VineticIRQHandler (VINETIC_DEVICE* pDev);

extern IFX_int32_t OS_GetDevice (IFX_int32_t nr, VINETIC_DEVICE** pDev);

/* VINETIC API */
extern IFX_int32_t ScWrite      (VINETIC_DEVICE *pDev, IFX_uint16_t cmd);
extern IFX_int32_t ScRead       (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData,
                                 IFX_uint8_t count);
extern IFX_int32_t CmdWrite     (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint8_t count);
extern IFX_int32_t CmdWriteIsr  (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint8_t count);
extern IFX_int32_t CmdRead      (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint16_t *pData,
                                 IFX_uint8_t count);
extern IFX_void_t DispatchCmd   (VINETIC_DEVICE *pDev, IFX_uint16_t const *pData);
#ifdef ASYNC_CMD
extern IFX_void_t CmdReadIsr    (VINETIC_DEVICE *pDev);
#endif /* ASYNC_CMD */
extern IFX_int32_t RegWrite     (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData,
                                 IFX_uint8_t count, IFX_uint8_t offset);
extern IFX_int32_t RegRead      (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData,
                                 IFX_uint8_t count, IFX_uint8_t offset);
extern IFX_int32_t RegModify    (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint8_t offset,
                                 IFX_uint16_t nMask, IFX_uint16_t nSetBits);
#ifdef NO_OS
IMPORT IFX_int32_t VINETIC_No_OS_InstallDriver   (IFX_void_t);
IMPORT IFX_void_t  VINETIC_No_OS_UninstallDriver (IFX_void_t);
#endif /* NO_OS */

IFX_void_t   prReg         (IFX_void_t *reg);
IFX_int32_t  wrReg         (VINETIC_CHANNEL *pCh, IFX_void_t *reg);
IFX_int32_t  rdReg         (VINETIC_CHANNEL *pCh, IFX_void_t *reg);
IFX_int32_t  setRegVal     (VINETIC_CHANNEL *pCh, IFX_void_t *reg, IFX_int32_t val);
IFX_int32_t  getRegVal     (VINETIC_CHANNEL *pCh, IFX_void_t *reg);
IFX_int32_t  storeRegVal   (VINETIC_CHANNEL *pCh, IFX_void_t *reg);
IFX_int32_t  restoreRegVal (VINETIC_CHANNEL *pCh, IFX_void_t *reg);
/* ============================= */
/* Global variables declaration  */
/* ============================= */

/* Trace groups, defined in drv_vinetic_main.c */
#ifdef VIN_SPI
DECLARE_TRACE_GROUP(SPI_DRV);
#endif /* VIN_SPI */
DECLARE_TRACE_GROUP(VINETIC);
DECLARE_LOG_GROUP(VINETIC);

#ifdef RUNTIME_TRACE
DECLARE_TRACE_GROUP(VINETIC_RUNTIME_TRACE);
#endif /* RUNTIME_TRACE */

#ifdef TESTING
extern IFX_void_t PrintErrorInfo(VINETIC_DEVICE *pDev);
#endif /* TESTING */

#endif /* _DRV_VINETICAPI_H */


