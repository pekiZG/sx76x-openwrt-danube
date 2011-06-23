#ifndef DRV_TAPI_H
#define DRV_TAPI_H
/****************************************************************************
                  Copyright (c) 2005  Infineon Technologies AG
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

   EXCEPT FOR ANY LIABILITY DUE TO WILLFUL ACTS OR GROSS NEGLIGENCE AND
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : drv_tapi.h
   Desription  : Contains TAPI functions declaration und structures.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#ifdef HAVE_CONFIG_H
#include <drv_config.h>
#endif

#include "sys_drv_debug.h"
#include "sys_drv_fifo.h"
#include "ifx_types.h"

/* this included file is supposed to support following OS:
   - LINUX
   - VXWORKS
   - NO_OS
   - WINDOWS
*/
#include "sys_drv_ifxos.h"

#ifdef VXWORKS
   #include "drv_tapi_vxworks.h"
   #include "sys_drv_timerlib_vxworks.h"
#elif defined(LINUX)
   #if 0
   #ifdef __NO_INLINE__
   #error "no inline, maybe use of -O0 ???"
   #endif /* __NO_INLINE__ */
   #endif
   #include "drv_tapi_linux.h"
#elif defined(NO_OS)
   #include "drv_tapi_noos.h"
#elif defined(WINDOWS)
   #include "drv_tapi_win.h"
#else
   #error Define your operating system!
#endif /* OS */

#include "drv_tapi_io.h"

/* ============================= */
/* Global defines                */
/* ============================= */

#define TAPI_MAJOR_VERS 2
#define TAPI_MINOR_VERS 3

#ifndef PULSE_DIAL_DETECTION
#define PULSE_DIAL_DETECTION
#endif

#define USA_RING_CADENCE             0xC0C0

/* number of internal tones for others driver purposes */
#define TAPI_MAX_RESERVED_TONES      10
/* maximum number of tones codes, comprising the maximum user tones and the
   maximum interna reserved tones. */
#define TAPI_MAX_TONE_CODE           (IFX_TAPI_TONE_INDEX_MAX + \
                                      TAPI_MAX_RESERVED_TONES)
#define TAPI_MAX_IMPEDANCE_SPEC_ID   6

#define TAPI_MIN_FLASH               80
#define TAPI_MAX_FLASH               200
#define TAPI_MIN_FLASH_MAKE       200
#define TAPI_MIN_DIGIT_LOW           30
#define TAPI_MAX_DIGIT_LOW           80
#define TAPI_MIN_DIGIT_HIGH          30
#define TAPI_MAX_DIGIT_HIGH          80
#define TAPI_MIN_OFF_HOOK            40
#define TAPI_MIN_ON_HOOK             400
#define TAPI_MIN_INTERDIGIT          300

/** time in ms while the line is in power down after the first Ground Key High */
#define GNDKH_RECHECKTIME1 10
/** time in ms to check the next Ground Key High after reenabling the line */
#define GNDKH_RECHECKTIME2 20

/* Upper limit for used channels as used in IFX_TAPI_CH_STATUS_GET */
#define TAPI_MAX_CHANNELS                 128
/** Maximum tone generators that can run in parallel */
#define TAPI_TONE_MAXRES 2
/* Signal detection masks */

/** V.25 2100Hz (CED) Modem/Fax Tone mask */
#define IFX_TAPI_SIG_CEDMASK              (IFX_TAPI_SIG_CED  |         \
                                             IFX_TAPI_SIG_CEDTX |         \
                                             IFX_TAPI_SIG_CEDRX)
/** CNG Fax Calling Tone (1100 Hz) mask */
#define IFX_TAPI_SIG_CNGFAXMASK           (IFX_TAPI_SIG_CNGFAX  |      \
                                             IFX_TAPI_SIG_CNGFAXRX |      \
                                             IFX_TAPI_SIG_CNGFAXTX)
/** CNG Modem Calling Tone (1300 Hz) mask */
#define IFX_TAPI_SIG_CNGMODMASK           (IFX_TAPI_SIG_CNGMOD  |      \
                                             IFX_TAPI_SIG_CNGMODRX |      \
                                             IFX_TAPI_SIG_CNGMODTX)

/** Phase reversal detection mask */
#define IFX_TAPI_SIG_PHASEREVMASK         (IFX_TAPI_SIG_PHASEREV  |    \
                                             IFX_TAPI_SIG_PHASEREVRX |    \
                                             IFX_TAPI_SIG_PHASEREVTX)
/** Mask the paths for DIS */
#define IFX_TAPI_SIG_DISMASK              (IFX_TAPI_SIG_DIS  |         \
                                             IFX_TAPI_SIG_DISRX |         \
                                             IFX_TAPI_SIG_DISTX)

/** Amplitude modulation mask */
#define IFX_TAPI_SIG_AMMASK               (IFX_TAPI_SIG_AM  |          \
                                             IFX_TAPI_SIG_AMRX |          \
                                             IFX_TAPI_SIG_AMTX)
/** Modem tone holding signal stopped mask */
#define IFX_TAPI_SIG_TONEHOLDING_ENDMASK  (IFX_TAPI_SIG_TONEHOLDING_END   |  \
                                              IFX_TAPI_SIG_TONEHOLDING_ENDRX |  \
                                              IFX_TAPI_SIG_TONEHOLDING_ENDTX)
/** End of signal CED detection mask */
#define IFX_TAPI_SIG_CEDENDMASK        (IFX_TAPI_SIG_CEDEND |          \
                                          IFX_TAPI_SIG_CEDENDRX |         \
                                          IFX_TAPI_SIG_CEDENDTX)
/** V8bis mask */
#define IFX_TAPI_SIG_V8BISMASK          (IFX_TAPI_SIG_V8BISRX |        \
                                           IFX_TAPI_SIG_V8BISTX)

/* ============================= */
/* Global variables declaration  */
/* ============================= */

/* Declarations for debug interface */
DECLARE_TRACE_GROUP  (TAPI_DRV);
DECLARE_LOG_GROUP    (TAPI_DRV);

/* =================================== */
/* Global typedef forward declarations */
/* =================================== */
typedef struct _TAPI_DEV         TAPI_DEV;
typedef struct _TAPI_CONNECTION  TAPI_CONNECTION;

/* =============================== */
/* Defines for ring timer states   */
/* =============================== */
enum
{
   /* initialization state */
   TAPI_RING_STATE_IDLE,
   /* initial non periodical ring pause */
   TAPI_RING_STATE_INITIAL_RINGING,
   /* periodical ring/pause cadence */
   TAPI_RING_STATE_PERIODICAL_RINGING
};

/* =============================== */
/* Defines for CID operations      */
/* =============================== */

/* Default indexes for CID alert tones */
enum _TAPI_CID_ALERTTONE_INDEX_DEFAULT
{
   /* Alert Tone default index, onhook */
   TAPI_CID_ATONE_INDEXDEF_ONHOOK       = IFX_TAPI_TONE_INDEX_MAX + 1,
   /* Alert Tone default index, offhook */
   TAPI_CID_ATONE_INDEXDEF_OFFHOOK      = IFX_TAPI_TONE_INDEX_MAX + 2,
   /* Alert Tone default index, AS NTT */
   TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT   = IFX_TAPI_TONE_INDEX_MAX + 3,
   /* define others cid reserved tones,
      should be in the range of reserved tones */
   TAPI_CID_ATONE_INDEXDEF_MAX          = TAPI_MAX_TONE_CODE
};

typedef enum
{
   /** ready to start sequence */
   TAPI_CID_STATE_IDLE,
   /** alert active */
   TAPI_CID_STATE_ALERT,
   /** waiting for ack */
   TAPI_CID_STATE_ACK,
   /** fsk or dtmf sending active */
   TAPI_CID_STATE_SENDING,
   /** fsk or dtmf sending completed (or timed out) */
   TAPI_CID_STATE_SENDING_COMPLETE
} TAPI_CID_STATE;

typedef enum
{
   TAPI_CID_ALERT_NONE,
   /** first ring burst */
   TAPI_CID_ALERT_FR,
   /** DTAS */
   TAPI_CID_ALERT_DTAS,
   /** Line Reversal with DTAS */
   TAPI_CID_ALERT_LR_DTAS,
   /** Ring Pulse */
   TAPI_CID_ALERT_RP,
   /** Open Switch Interval */
   TAPI_CID_ALERT_OSI,
   /** CPE Alert Signal */
   TAPI_CID_ALERT_CAS,
   /** Alert Signal (NTT) */
   TAPI_CID_ALERT_AS_NTT,
   /** CAR Signal (NTT) */
   TAPI_CID_ALERT_CAR_NTT
} TAPI_CID_ALERT_TYPE;

/* ============================= */
/* Enumeration for hook state    */
/* Finite State Machine          */
/* ============================= */
typedef enum
{
   /* phone is offhook, possibly waiting for interdigit timer */
   TAPI_HOOK_STATE_OFFHOOK,
   /* phone is offhook, waiting for next dialing pulse */
   TAPI_HOOK_STATE_PULSE_H_CONFIRM,
   /* phone has gone offhook: it may just be noise */
   TAPI_HOOK_STATE_OFFHOOK_VAL,
   /* phone has gone offhook while collecting pulses */
   TAPI_HOOK_STATE_PULSE_H_VAL,
   /* phone has gone offhook with an overlap of flash hook min time and digit
      low max time. In this state both digit 1 and flash are reported */
   TAPI_HOOK_STATE_PULSE_H_FLASH_VAL,
   /* phone is onhook, and no timers are running */
   TAPI_HOOK_STATE_ONHOOK,
   /* phone has remained onhook long enough to be low pulse: wait for next offhook to confirm */
   TAPI_HOOK_STATE_PULSE_L_CONFIRM,
   /* overlap of flash hook min time and digit low max time. In this
      state both digit 1 and flash are reported */
   TAPI_HOOK_STATE_PULSE_L_FLASH_CONFIRM,
   /* phone onhook (too long to be digit pulse): could be flash or final onhook */
   TAPI_HOOK_STATE_FLASH_WAIT,
   /* phone onhook (long enough to be flash): wait for next offhook to confirm this */
   TAPI_HOOK_STATE_FLASH_CONFIRM,
   /* validation of the flash hook time after off hook */
   TAPI_HOOK_STATE_FLASH_VAL,
   /* phone onhook (long enough to be final onhook): wait for onhook timer to confirm */
   TAPI_HOOK_STATE_ONHOOK_CONFIRM,
   /* phone has gone onhook: it may just be noise */
   TAPI_HOOK_STATE_ONHOOK_VAL,
   /* phone has gone onhook during pulse dialing: wait to validate */
   TAPI_HOOK_STATE_DIAL_L_VAL
}TAPI_HOOK_STATE;

/* =============================== */
/* Defines for complex tone states */
/* =============================== */
typedef enum
{
   /* initialization state */
   TAPI_CT_IDLE = 0,
   /* UTG tone sequence is active */
   TAPI_CT_ACTIVE,
   /* UTG tone sequence is deactived */
   TAPI_CT_DEACTIVATED
}TAPI_CMPLX_TONE_STATE;

/** tone play destinations */
typedef enum
{
 TAPI_TONE_DST_DEFAULT  = 0x0,
 TAPI_TONE_DST_LOCAL    = 0x1,
 TAPI_TONE_DST_NET      = 0x2,
 TAPI_TONE_DST_NETLOCAL = 0x3
}TAPI_TONE_DST;


/** \internal */

#ifdef TAPI_CID
/* ============================= */
/* Structure for CID data        */
/* ============================= */
typedef struct
{
   /* sets CID type 1 (ON-) or type 2 (OFF-HOOK) */
   IFX_TAPI_CID_HOOK_MODE_t    txHookMode;
   /* flag to know if transmission/sequence is active (and data is new) */
   IFX_boolean_t           bActive;
   /* Buffer for CID parameter coding  */
   IFX_uint8_t             cidParam [IFX_TAPI_CID_SIZE_MAX];
   /* number of CID parameters octets */
   IFX_uint8_t             nCidParamLen;

   /* reverse line mode for standards with Line Reversal */
   IFX_uint32_t            nLineModeReverse;

   /* Timer for caller id handling */
   Timer_ID                CidTimerID;

   /* CID transmission state */
   TAPI_CID_STATE          nCidState;
   /* common sub state (used by alert,...) */
   IFX_uint32_t            nCidSubState;

   /* alert type for sequence */
   TAPI_CID_ALERT_TYPE     nAlertType;
   /* Calculated transmission time according to FSK/DTMF buffer size */
   IFX_uint32_t            nTxTime;

   /* required acknowledge tone as chip specific code */
   IFX_uint8_t             ackToneCode;
   /* acknowledge flag */
   volatile IFX_boolean_t  bAck;

   /* flag, if muting for off hook cid was done */
   IFX_boolean_t           bMute;
   /* flag for starting periodical ringing */
   IFX_boolean_t           bRingStart;

   /* Instance of phone channel mapped to
      this data channel */
   TAPI_CONNECTION         *pPhoneCh;
} TAPI_CID_DATA;


typedef struct
{
   /** The configured CID standard */
   IFX_TAPI_CID_STD_t          nStandard;

   /** Type of ETSI Alert of onhook services associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_FR */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertRing;
   /** Type of ETSI Alert of onhook services not associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_RP. */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertNoRing;
   /** Tone table index for the alert tone to be used.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int            nAlertToneOnhook;
   unsigned int            nAlertToneOffhook;
  /* time needed to play alert tone */
   unsigned int            nAlertToneTime;
   /** Ring Pulse time interval. */
   unsigned int            ringPulseTime;
   /** Ring Pulse Loops. */
   unsigned int            ringPulseLoop;
   /** DTMF ACK after CAS, used for offhook transmission. Default DTMF 'D'. */
   char                    ackTone;
   /** Usage of OSI for offhook transmission. Default “no use”. */
   unsigned int            OSIoffhook;
   /** Lenght of the OSI signal in ms. Default 200 ms. */
   unsigned int            OSItime;
   /** Cadence Ring Burst time, used for TAPI_CID_ALERT_FR */
   unsigned int            cadenceRingBurst;
   /** Cadence Ring Pause time, used for TAPI_CID_ALERT_FR */
   unsigned int            cadenceRingPause;


   IFX_TAPI_CID_ABS_REASON_t   TapiCidDtmfAbsCli;
   IFX_TAPI_CID_TIMING_t       TapiCidTiming;
   IFX_TAPI_CID_FSK_CFG_t      TapiCidFskConf;
   IFX_TAPI_CID_DTMF_CFG_t     TapiCidDtmfConf;
} TAPI_CID_CONF;

typedef struct
{
   /* status */
   IFX_TAPI_CID_RX_STATUS_t  stat;
   /* ptr to actual cid rx buffer */
   IFX_TAPI_CID_RX_DATA_t   *pData;
   /* cid rx fifo */
   FIFO               TapiCidRxFifo;
} TAPI_CIDRX;

#endif /* TAPI_CID */

/* ============================= */
/** Structure for operation control
    \internal */
/* ============================= */
typedef struct
{
   /* interrupt routine save hookstatus here */
   unsigned char              bHookState;
   /* polarity status from the line; 1 = reversed; 0 = normal */
   unsigned char             nPolarity;
   /* automatic battery switch; 1 = automatic; 0 = normal */
   unsigned char             nBatterySw;
   /* last line feed mode; is set after ringing stops */
   unsigned char             nLineMode;
   /* set when the fault condition occurs of the device. It will be reseted
      when the line mode is modified */
   unsigned char             bFaulCond;
} TAPI_OPCONTROL_DATA;

/* ============================= */
/** Structure for ring data
    \internal */
/* ============================= */
typedef struct
{
   /* timer id for ringing service */
   Timer_ID                   RingTimerID;
   /* actual ring cadence state */
   IFX_uint8_t                nRingTimerState;
   /* is channel in ring_burst mode? */
   IFX_boolean_t              bIsRingBurstState;
   /* is channel ringing mode? */
   volatile IFX_boolean_t     bRingingMode;
   /* cadence data */
   IFX_TAPI_RING_CADENCE_t    RingCadence;
   /* ring configuration data */
   IFX_TAPI_RING_CFG_t        RingConfig;
   /* counter for cadence */
   IFX_uint8_t                nByteCounter;
   IFX_uint8_t                nBitCounter;
   /* maximum rings for blocking ring */
   IFX_uint32_t               nMaxRings;
   /* remaining rings during blocking ring */
   IFX_uint32_t               nRingsLeft;
}TAPI_RING_DATA;

/*@{*/

/* ============================= */
/* Structure for pcm data        */
/* ============================= */
typedef struct
{
   /* configuration data for pcm services */
   IFX_TAPI_PCM_CFG_t   PCMConfig;
   /* save activation status */
   IFX_boolean_t     bTimeSlotActive;
}TAPI_PCM_DATA;


/* ============================= */
/* Structure for tone data       */
/*                               */
/* ============================= */
typedef struct
{
   /* network tone on/off */
   IFX_boolean_t     bNetTone;
   /* off time duration */
   IFX_uint32_t      nOffTime;
   /* on time duration */
   IFX_uint32_t      nOnTime;
   /* tone playing state */
   IFX_uint32_t      nToneState;
   IFX_boolean_t     bUseSecondTG;
}TAPI_TG_TONE_DATA;


/* ============================= */
/* Structure for miscellaneous   */
/* data                          */
/* ============================= */
typedef enum
{
   /** no ground key detected */
   GNDKH_STATE_READY,
   /** first ground key high detected and line set to power down */
   GNDKH_STATE_POWDN,
   /** timer 1 expired and line is set to orign mode to recheck
       ground key high again */
   GNDKH_STATE_RECHECK,
   /** line is in fault state, cause ground key came twice */
   GNDKH_STATE_FAULT
}GNDKH_STATE;

typedef struct
{
   /* exception status */
   IFX_TAPI_EXCEPTION_t    nException;
   /* exception status mask */
   IFX_TAPI_EXCEPTION_t    nExceptionMask;
   IFX_int32_t       line;
   /** line signals for fax and modem detection */
   IFX_uint32_t      signal;
   /** Currently enabled line signals, zero means enabled.
      The current signal mask is set by the interface enable/disable signal */
   IFX_uint32_t      sigMask;
   /** device specific information. Bit oriented */
   IFX_uint32_t      device;
   /** Ground key high validation timer */
   Timer_ID          GndkhTimerID;
   /** current state of ground key high validation */
   IFX_int32_t       GndkhState;
   /** RFC2833 event played out */
   IFX_uint32_t      event;
   /** runtime errors */
   IFX_uint32_t      error;
}TAPI_MISC_DATA;

/* ============================= */
/* Structure for dial data       */
/*                               */
/* ============================= */
typedef struct
{
   /* state of the hookstate Finite State Machine */
   TAPI_HOOK_STATE   nHookState;
   /* timer id for dial service */
   Timer_ID          DialTimerID;
   /* number of hook changes */
   IFX_uint8_t       nHookChanges;
   struct
   {
      /* a flash hook might has been detected in case of on overlap
         with digit validation time */
      unsigned int bProbablyFlash:1;
   }state;
   /* fifo */
   FIFO              TapiDTMFFifo;
   FIFO              TapiPulseFifo;
   /* fifo buffer for pulse dial storage */
   IFX_uint8_t       pPulseStore[IFX_TAPI_PULSE_FIFO_SIZE];
   /* fifo buffer for DTMF dial storage */
   IFX_uint8_t       pDtmfStore[IFX_TAPI_DTMF_FIFO_SIZE];
}TAPI_DIAL_DATA;

/* ============================= */
/* Structure for metering data   */
/*                               */
/* ============================= */
typedef struct
{
   /* is metering active actually ? */
   IFX_boolean_t      bMeterActive;
   /* timer id for metering */
   Timer_ID           MeterTimerID;
   /* metering coniguration data */
   IFX_TAPI_METER_CFG_t  MeterConfig;
   /* last linemode befor metering start */
   IFX_uint8_t        nLastLineMode;
   /* is channel in metering mode? */
   IFX_boolean_t      bMeterBurstMode;
   /* elapsed count */
   IFX_uint32_t       nElapsedCnt;
}TAPI_METER_DATA;

/* ============================= */
/* Structure for internal tone   */
/* coefficients table            */
/* ============================= */
typedef enum
{
   TAPI_TONE_TYPE_NONE,
   TAPI_TONE_TYPE_SIMPLE,
   TAPI_TONE_TYPE_COMP,
   TAPI_TONE_TYPE_PREDEF
}TAPI_TONE_TYPE;

typedef struct
{
   TAPI_TONE_TYPE          type;
   union
   {
      IFX_int32_t          nPredefined;
      IFX_TAPI_TONE_SIMPLE_t     simple;
      IFX_TAPI_TONE_COMPOSED_t   composed;
   } tone;
}COMPLEX_TONE;

/* ============================= */
/* Structure for complex tone    */
/* data                          */
/* ============================= */
typedef struct
{
   /* type of tone sequence SIMPLE or COMPOSED */
   TAPI_TONE_TYPE          nType;
   IFX_uint32_t            nSimpleMaxReps;
   /* current repetition counter for simple tone sequence */
   IFX_uint32_t            nSimpleCurrReps;
   /* maximum number of repetitions for composed tone sequence */
   IFX_uint32_t            nComposedMaxReps;
   /* current repetition counter for composed tone sequence */
   IFX_uint32_t            nComposedCurrReps;
   /* pause time in ms betweensimple tones */
   IFX_uint32_t            nPauseTime;
   /* alternate voice timein ms between composed tones */
   IFX_uint32_t            nAlternateVoiceTime;
   /* current simple tone codeplaying */
   IFX_uint32_t            nSimpleToneCode;
   /* current composed tone code playing */
   IFX_uint32_t            nComposedToneCode;
   /* maximum simple toneswithin composed tone */
   IFX_uint32_t            nMaxToneCount;
   /* current simple tone within composed tone */
   IFX_uint32_t            nToneCounter;
   /* complex tone playing state */
   TAPI_CMPLX_TONE_STATE   nToneState;
   /** which source plays the tone
      (firmware module or analog tone generator) */
   IFX_int32_t             src;
   /** which direction to play the tone (net or local) */
   TAPI_TONE_DST dst;
   /** stores the index of the tone currently played */
   IFX_int32_t       nToneIndex;
}TAPI_TONE_DATA;

typedef struct
{
   /* startup on-hook timer value */
   IFX_uint32_t            nOnHookMinTime;
   IFX_uint32_t            nOnHookMaxTime;
   IFX_boolean_t           bOnHookRestore;
   /* startup hookflash timer value */
   IFX_uint32_t            nHookFlashMinTime;
   IFX_uint32_t            nHookFlashMaxTime;
   IFX_boolean_t           bHookFlashRestore;
}TAPI_VALIDATION_START;

/* ============================= */
/* channel specific structure    */
/* ============================= */
struct _TAPI_CONNECTION
{
   /* channel is installed? */
   IFX_boolean_t        bInstalled;
   /* channel number */
   IFX_uint8_t          nChannel;
   /* pointer to driver device structure */
   IFX_void_t           *pDevice;
   /* pointer to the tapi device structure */
   TAPI_DEV             *pTapiDevice;
   /* locking semaphore for protecting data */
   IFXOS_mutex_t        TapiDataLock;
   /* data structures for services */
   IFX_TAPI_LEC_CFG_t   TapiLecData;
   IFX_TAPI_LEC_CFG_t   TapiLecPcmData;
   TAPI_METER_DATA      TapiMeterData;
   TAPI_RING_DATA       TapiRingData;
   TAPI_OPCONTROL_DATA  TapiOpControlData;
   TAPI_PCM_DATA        TapiPCMData;
   TAPI_TG_TONE_DATA    TapiTgToneData;
   TAPI_MISC_DATA       TapiMiscData;
   TAPI_DIAL_DATA       TapiDialData;
   /* locking semaphore for ringing wait */
   IFXOS_event_t        TapiRingEvent;
#ifdef TAPI_VOICE
   /* cache jitter buffer configuration */
   IFX_TAPI_JB_CFG_t    TapiJbData;
   /* actual channel codec from IFX_TAPI_ENC_TYPE_t */
   IFX_int32_t          nCodec;
#endif /* TAPI_VOICE */
#if defined(TAPI_DTMF) || defined(TAPI_VOICE)
   /* event payload type*/
   IFX_uint8_t          nEvtPT;
#endif /* defined(TAPI_DTMF) || defined(TAPI_VOICE) */
#ifdef TAPI_FAX_T38
   /* stores the current fax status */
   IFX_TAPI_T38_STATUS_t      TapiFaxStatus;
#endif /* TAPI_FAX_T38 */
#ifdef TAPI_CID
   /* caller id config */
   TAPI_CID_CONF        TapiCidConf;
   /* caller id receiver data */
   TAPI_CIDRX           TapiCidRx;
   /* caller id transmit data */
   TAPI_CID_DATA        TapiCidTx;
#endif /* TAPI_CID */
   /* stores the validation timer settings */
   IFX_TAPI_LINE_HOOK_VT_t TapiHookOffTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiHookOnTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiHookFlashTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiHookFlashMakeTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiDigitLowTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiDigitHighTime;
   IFX_TAPI_LINE_HOOK_VT_t TapiInterDigitTime;
   /* complex tone data */
   TAPI_TONE_DATA  TapiComplexToneData [TAPI_TONE_MAXRES];
   /* If this structure is used as a phone channel, keep track of
      connected dsp data channels in the following bitmask variable */
   IFX_uint32_t          nPhoneDataChannels;
   /* If this structure is used as a dsp data channel, keep track of
      connected phone channels in the following bitmask variable */
   IFX_uint32_t          nDataPhoneChannels;
};

/* ============================= */
/* tapi structure                */
/* ============================= */
struct _TAPI_DEV
{
   /* number of channels; is set in device structure initialisation */
   IFX_uint8_t       nMaxChannel;
   /* channel specific structure */
   TAPI_CONNECTION   *pChannelCon;
   /** Internal tone coefficients table. We use the zero values as flags to indicate
       that a tone table entry is unconfigured */
   COMPLEX_TONE* pToneTbl;
};


/* ============================= */
/** TAPI  Global functions       */
/* ============================= */

/* Ringing Services */
IFX_int32_t TAPI_Phone_Ring_Cadence            (TAPI_CONNECTION *pChannel, IFX_uint32_t nCadence);
IFX_int32_t TAPI_Phone_Ring_Cadence_High_Res   (TAPI_CONNECTION *pChannel, IFX_TAPI_RING_CADENCE_t const *pCadence);
IFX_int32_t TAPI_Phone_Ring_Prepare            (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Ring_Start              (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Ring_Stop               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Ring_Config             (TAPI_CONNECTION *pChannel, IFX_TAPI_RING_CFG_t const *pRingConfig);
IFX_int32_t TAPI_Phone_Ring_GetConfig          (TAPI_CONNECTION *pChannel, IFX_TAPI_RING_CFG_t *pRingConfig);
IFX_int32_t TAPI_Phone_Set_MaxRings            (TAPI_CONNECTION *pChannel, IFX_uint32_t nArg);
IFX_int32_t TAPI_Phone_Ring                    (TAPI_CONNECTION *pChannel);
IFX_void_t  TAPI_Phone_Ring_OnTimer            (Timer_ID Timer, IFX_int32_t nArg);
IFX_int32_t TAPI_Phone_Ringtimer_Start         (TAPI_CONNECTION *pChannel);

/* Operation Control Services */
IFX_int32_t TAPI_Phone_Hookstate               (TAPI_CONNECTION *pChannel, IFX_int32_t *pState);
IFX_int32_t TAPI_Phone_Set_Linefeed            (TAPI_CONNECTION *pChannel, IFX_int32_t nMode);
IFX_int32_t TAPI_Phone_LecConf                 (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf);
IFX_int32_t TAPI_Phone_GetLecConf              (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf);
IFX_int32_t TAPI_Phone_LecConf_Pcm             (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf);
IFX_int32_t TAPI_Phone_GetLecConf_Pcm          (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf);
#ifdef TAPI_CID
IFX_int32_t TAPI_Phone_CID_SetDefaultConfig    (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_CID_SetConfig           (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_CFG_t const *pCidConf);
IFX_int32_t TAPI_Phone_CID_Info_Tx             (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_MSG_t const *pCidInfo);
IFX_int32_t TAPI_Phone_CID_Seq_Tx              (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_MSG_t const *pCidInfo);
IFX_void_t  TAPI_Phone_CID_OnTimer             (Timer_ID Timer, IFX_int32_t nArg);

IFX_int32_t TAPI_Phone_CidRx_Start             (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_CidRx_Stop              (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Get_CidRxData           (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_RX_DATA_t *pCidRxData);
IFX_TAPI_CID_RX_DATA_t *TAPI_Phone_GetCidRxBuf        (TAPI_CONNECTION *pChannel, IFX_uint32_t nLen);
#endif /* TAPI_CID */

/* Tone Services */
IFX_int32_t TAPI_Phone_Tone_Set_On_Time        (TAPI_CONNECTION *pChannel, IFX_uint32_t nTime);
IFX_int32_t TAPI_Phone_Tone_Set_Off_Time       (TAPI_CONNECTION *pChannel, IFX_uint32_t nTime);
IFX_int32_t TAPI_Phone_Tone_Get_On_Time        (TAPI_CONNECTION *pChannel, IFX_uint32_t *pOnTime);
IFX_int32_t TAPI_Phone_Tone_Get_Off_Time       (TAPI_CONNECTION *pChannel, IFX_uint32_t *pOffTime);
IFX_int32_t TAPI_Phone_Tone_Play               (TAPI_CONNECTION *pChannel,
                                                IFX_int32_t nToneIndex,
                                                TAPI_TONE_DST dst);
IFX_int32_t TAPI_Phone_Tone_Stop               (TAPI_CONNECTION *pChannel,
                                                IFX_int32_t nToneIndex);
IFX_int32_t TAPI_Phone_Tone_Set_Level          (TAPI_CONNECTION *pChannel, IFX_TAPI_PREDEF_TONE_LEVEL_t const *pToneLevel);
IFX_int32_t TAPI_Phone_Tone_Get_State          (TAPI_CONNECTION *pChannel, IFX_uint32_t *pToneState);
IFX_int32_t TAPI_Phone_Tone_Dial               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Tone_Ringback           (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Tone_Busy               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Add_SimpleTone          (COMPLEX_TONE* toneCoefficients,
                                                IFX_TAPI_TONE_SIMPLE_t const *pSimpleTone);
IFX_int32_t TAPI_Phone_Add_ComposedTone        (COMPLEX_TONE* toneCoefficients,
                                                IFX_TAPI_TONE_COMPOSED_t const *pComposedTone);
IFX_int32_t TAPI_Phone_Tone_TableConf          (COMPLEX_TONE* toneCoefficients,
                                                IFX_TAPI_TONE_t const *pTone);

IFX_int32_t TAPI_Phone_DetectToneStart         (TAPI_CONNECTION *pChannel, IFX_TAPI_TONE_CPTD_t const *signal);
IFX_int32_t TAPI_Phone_DetectToneStop          (TAPI_CONNECTION *pChannel);

/* PCM Services */
IFX_int32_t TAPI_Phone_PCM_Set_Config          (TAPI_CONNECTION *pChannel, IFX_TAPI_PCM_CFG_t const *pPCMConfig);
IFX_int32_t TAPI_Phone_PCM_Get_Config          (TAPI_CONNECTION *pChannel, IFX_TAPI_PCM_CFG_t *pPCMConfig);
IFX_int32_t TAPI_Phone_PCM_Set_Activation      (TAPI_CONNECTION *pChannel, IFX_uint32_t nMode);
IFX_int32_t TAPI_Phone_PCM_Get_Activation      (TAPI_CONNECTION *pChannel);

/* Miscellaneous Services */
IFX_int32_t TAPI_Phone_Get_Version             (IFX_char_t * version_string);
IFX_int32_t TAPI_Phone_Check_Version           (IFX_TAPI_VERSION_t const *vers);
IFX_uint32_t TAPI_Phone_Exception              (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Mask_Exception          (TAPI_CONNECTION *pChannel, IFX_uint32_t nException);
IFX_int32_t TAPI_Phone_Init                    (TAPI_CONNECTION *pChannel, IFX_TAPI_CH_INIT_t const *pInit);
IFX_int32_t TAPI_Phone_Validation_Time         (TAPI_CONNECTION *pChannel, IFX_TAPI_LINE_HOOK_VT_t const *pTime);
IFX_int32_t TAPI_Phone_GetStatus               (TAPI_CONNECTION *pChannel, IFX_TAPI_CH_STATUS_t *status);
IFX_void_t  TAPI_Gndkh_OnTimer                 (Timer_ID Timer, IFX_int32_t nArg);

/* Dial Services */
IFX_void_t  TAPI_Phone_Dial_OnTimer            (Timer_ID Timer, IFX_int32_t nArg);
IFX_int32_t TAPI_Phone_Get_Pulse               (TAPI_CONNECTION *pChannel, IFX_int32_t *pPulseDigit);
IFX_int32_t TAPI_Phone_Get_Pulse_ASCII         (TAPI_CONNECTION *pChannel, IFX_int32_t *pPulseDigit);
IFX_int32_t TAPI_Phone_Pulse_Ready             (TAPI_CONNECTION *pChannel, IFX_int32_t *pPulseReady);
#ifdef TAPI_DTMF
IFX_int32_t TAPI_Phone_DTMF_Ready              (TAPI_CONNECTION *pChannel, IFX_int32_t *pDtmfReady);
IFX_int32_t TAPI_Phone_Get_DTMF                (TAPI_CONNECTION *pChannel, IFX_int32_t *pDtmfDigit);
IFX_int32_t TAPI_Phone_Get_DTMF_ASCII          (TAPI_CONNECTION *pChannel, IFX_int32_t *pDtmfAscii);
#endif /* TAPI_DTMF */

/* Metering Services */
extern IFX_int32_t TAPI_Phone_Meter_Start      (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Meter_Stop              (TAPI_CONNECTION *pChannel);
IFX_void_t  TAPI_Phone_Meter_OnTimer           (Timer_ID Timer, IFX_int32_t nArg);
extern IFX_int32_t TAPI_Phone_Meter_Config     (TAPI_CONNECTION *pChannel, IFX_TAPI_METER_CFG_t const *pMeterConfig);

#ifdef TAPI_VOICE
/* Playing services */
IFX_int32_t TAPI_Phone_SetPlayHold             (TAPI_CONNECTION *pChannel, IFX_int32_t nHold);
IFX_int32_t TAPI_Phone_SetPlayVolume           (TAPI_CONNECTION *pChannel, IFX_int32_t nVolume);
IFX_int32_t TAPI_Phone_GetPlayLevel            (TAPI_CONNECTION *pChannel, IFX_int32_t *pLevel);
IFX_int32_t TAPI_Phone_SetPlayDepth            (TAPI_CONNECTION *pChannel, IFX_int32_t nFrameDepth);

/* Recording services */
IFX_int32_t TAPI_Phone_SetRecHold              (TAPI_CONNECTION *pChannel, IFX_int32_t nHold);
IFX_int32_t TAPI_Phone_SetRecVolume            (TAPI_CONNECTION *pChannel, IFX_int32_t nVolume);
IFX_int32_t TAPI_Phone_GetRecLevel             (TAPI_CONNECTION *pChannel, IFX_int32_t *pLevel);
IFX_int32_t TAPI_Phone_SetRecDepth             (TAPI_CONNECTION *pChannel, IFX_int32_t nFrameDepth);

/* Connection services */
IFX_int32_t TAPI_Data_Channel_Add              (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_DATA_t const *pMap);
IFX_int32_t TAPI_Data_Channel_Remove           (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_DATA_t const *pMap);
IFX_int32_t TAPI_Phone_Channel_Add             (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PHONE_t const *pMap);
IFX_int32_t TAPI_Phone_Channel_Remove          (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PHONE_t const *pMap);
IFX_int32_t TAPI_Phone_Get_Data_Channel        (TAPI_CONNECTION *pChannel, IFX_uint8_t *pDataChannel);
IFX_int32_t TAPI_Data_Get_Phone_Channel        (TAPI_CONNECTION *pChannel, IFX_uint8_t *pPhoneChannel);
IFX_int32_t TAPI_Phone_Fax_JB_Set              (TAPI_CONNECTION *pChannel, IFX_TAPI_JB_CFG_t *pJbConf);
#endif /* TAPI_VOICE */

/* ======================================== */
/**  Event Handling                         */
/* ======================================== */

IFX_void_t TAPI_Phone_Event_HookState          (TAPI_CONNECTION *pChannel, IFX_uint8_t bHookState);
IFX_void_t TAPI_Phone_Event_GNK                (TAPI_CONNECTION *pChannel);
IFX_void_t TAPI_Phone_Event_GNKH               (TAPI_CONNECTION *pChannel);
IFX_void_t TAPI_Phone_Event_GNKP               (TAPI_CONNECTION *pChannel);
#ifdef TAPI_GR909
IFX_void_t TAPI_Phone_Event_Gr909              (TAPI_CONNECTION *pChannel);
#endif /* TAPI_GR909 */
IFX_void_t TAPI_Phone_Event_Otemp              (TAPI_CONNECTION *pChannel);
IFX_void_t TAPI_Phone_Event_Device             (TAPI_CONNECTION *pChannel, IFX_uint32_t devEvt);
IFX_void_t TAPI_Phone_Event_Line               (TAPI_CONNECTION *pChannel,
                                                IFX_TAPI_LINE_STATUS_t stat);
IFX_void_t TAPI_WakeUp                         (TAPI_CONNECTION *pChannel);
IFX_void_t TAPI_WaitWakeUp                     (TAPI_CONNECTION *pChannel);
IFX_void_t TAPI_Signal_Event_Update            (TAPI_CONNECTION *pChannel, IFX_uint32_t signal);
IFX_void_t TAPI_Rfc2833_Event                  (TAPI_CONNECTION *pChannel, IFX_uint32_t event);
IFX_void_t TAPI_Error_Event_Update             (TAPI_CONNECTION *pChannel, IFX_uint32_t error);
#ifdef TAPI_DTMF
IFX_void_t TAPI_Phone_Event_DTMF               (TAPI_CONNECTION *pChannel, IFX_uint8_t nKey);
#endif /* TAPI_DTMF */
#ifdef TAPI_FAX_T38
IFX_void_t TAPI_FaxT38_Event_Update            (TAPI_CONNECTION *pChannel, IFX_uint8_t status, IFX_uint8_t error);
#endif /* TAPI_FAX_T38 */
#ifdef TAPI_CID
IFX_void_t TAPI_Phone_Event_CidRx              (TAPI_CONNECTION *pChannel);
#endif

/* ======================================== */
/**  Chip specific functions   (LOW LAYER)  */
/* ======================================== */

IFX_return_t TAPI_LL_Phone_Init                 (TAPI_CONNECTION *pChannel, IFX_TAPI_CH_INIT_t const *pInit);
IFX_int32_t TAPI_LL_Phone_SwitchLine           (TAPI_CONNECTION *pChannel, IFX_int32_t nMode);
IFX_int32_t TAPI_LL_Phone_AutoBatterySwitch (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Polarity             (TAPI_CONNECTION *pChannel);
IFX_return_t TAPI_LL_Phone_PCM_Activation       (TAPI_CONNECTION *pChannel, IFX_uint32_t nMode);
IFX_return_t  TAPI_LL_Phone_PCM_Config           (TAPI_CONNECTION *pChannel, IFX_TAPI_PCM_CFG_t const *pPCMConfig);
IFX_return_t  TAPI_LL_Phone_Pcm_Volume           (TAPI_CONNECTION *pChannel, IFX_TAPI_LINE_VOLUME_t const *pVol);
IFX_int32_t   TAPI_LL_Phone_Ring_Config          (TAPI_CONNECTION *pChannel, IFX_TAPI_RING_CFG_t const *pRingConfig);
IFX_int32_t   TAPI_LL_Phone_Meter_Config         (TAPI_CONNECTION *pChannel, IFX_uint8_t nMode, IFX_uint8_t nFreq);
IFX_return_t  TAPI_LL_Phone_Tone_Predef_Config   (COMPLEX_TONE* pToneTable);
IFX_return_t  TAPI_LL_Phone_Tone_Play            (TAPI_CONNECTION *pChannel, IFX_uint8_t res,
                                                COMPLEX_TONE const *pToneCoeff,
                                                TAPI_TONE_DST dst);
IFX_return_t  TAPI_LL_Phone_Tone_Stop            (TAPI_CONNECTION *pChannel, IFX_uint8_t res,
                                                COMPLEX_TONE const *pToneCoeff);
IFX_int32_t   TAPI_LL_Phone_Tone_Set_Level       (TAPI_CONNECTION *pChannel, IFX_TAPI_PREDEF_TONE_LEVEL_t const *pToneLevel);
IFX_uint32_t  TAPI_LL_Phone_Get_Capabilities     (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_Get_Capability_List  (TAPI_CONNECTION *pChannel, IFX_TAPI_CAP_t *pCapList);
IFX_int32_t   TAPI_LL_Phone_Check_Capability     (TAPI_CONNECTION *pChannel, IFX_TAPI_CAP_t *pCapList);
IFX_return_t  TAPI_LL_Phone_VAD                  (TAPI_CONNECTION *pChannel, IFX_int32_t nVAD);
IFX_return_t  TAPI_LL_Phone_LecConf              (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t const *pLecConf);
IFX_return_t  TAPI_LL_Phone_LecConf_Pcm          (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t const *pLecConf);
#ifdef TAPI_VOICE
IFX_return_t  TAPI_LL_Phone_Set_Frame_Length   (TAPI_CONNECTION *pChannel, IFX_int32_t nFrameLength);
IFX_return_t  TAPI_LL_Phone_Get_Frame_Length   (TAPI_CONNECTION *pChannel, IFX_int32_t *pFrameLength);
IFX_return_t  TAPI_LL_Phone_Set_Rec_Codec      (TAPI_CONNECTION *pChannel, IFX_int32_t nCodec);
IFX_return_t  TAPI_LL_Phone_Start_Recording    (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_Stop_Recording     (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_Start_Playing      (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_Stop_Playing       (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Voice_Enable             (TAPI_CONNECTION *pChannel, IFX_uint8_t nMode);
IFX_return_t  TAPI_LL_Data_Channel_Add         (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_DATA_t const *pMap);
IFX_return_t  TAPI_LL_Data_Channel_Remove      (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_DATA_t const *pMap);
IFX_return_t  TAPI_LL_Phone_Channel_Add        (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PHONE_t const *pMap);
IFX_return_t  TAPI_LL_Phone_Channel_Remove     (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PHONE_t const *pMap);
IFX_return_t  TAPI_LL_PCM_Channel_Add          (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PCM_t const *pMap);
IFX_return_t  TAPI_LL_PCM_Channel_Remove       (TAPI_CONNECTION *pChannel, IFX_TAPI_MAP_PCM_t const *pMap);
IFX_return_t  TAPI_LL_Phone_RtpConf            (TAPI_CONNECTION *pChannel, IFX_TAPI_PKT_RTP_CFG_t const *pRtpConf);
IFX_return_t  TAPI_LL_Phone_Rtp_SetPayloadType (TAPI_CONNECTION *pChannel, IFX_TAPI_PKT_RTP_PT_CFG_t const *pRtpPTConf);
IFX_return_t  TAPI_LL_Phone_RTCP_GetStatistics (TAPI_CONNECTION *pChannel, IFX_TAPI_PKT_RTCP_STATISTICS_t *pRtcpData);
IFX_return_t  TAPI_LL_Phone_RTCP_PrepareStatistics (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_RTCP_GatherStatistics  (TAPI_CONNECTION *pChannel, IFX_TAPI_PKT_RTCP_STATISTICS_t *pRTCP);
IFX_return_t  TAPI_LL_Phone_RTCP_Reset         (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_JB_Set             (TAPI_CONNECTION *pChannel, IFX_TAPI_JB_CFG_t const *pJbConf);
IFX_return_t  TAPI_LL_Phone_JB_GetStatistics   (TAPI_CONNECTION *pChannel, IFX_TAPI_JB_STATISTICS_t *pJbData);
IFX_return_t  TAPI_LL_Phone_JB_Reset           (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_Aal_Set            (TAPI_CONNECTION *pChannel, IFX_TAPI_PCK_AAL_CFG_t const *pAalConf);
IFX_return_t TAPI_LL_Phone_AgcEnable         (TAPI_CONNECTION *pChannel, IFX_TAPI_ENC_AGC_MODE_t Param);
#if 0
IFX_return_t  TAPI_LL_Phone_Aal_SwitchCoder    (TAPI_CONNECTION *pChannel);
#endif /* 0 */
IFX_return_t  TAPI_LL_Phone_AalProfile         (TAPI_CONNECTION *pChannel, IFX_TAPI_PCK_AAL_PROFILE_t const *pProfile);
#if 0
IFX_return_t  TAPI_LL_Fax_SwitchMode           (TAPI_CONNECTION *pChannel, TAPI_FAX_MODE const *pFaxMode);
/*#ifdef NETWORK_TONE*/
IFX_return_t  TAPI_LL_Phone_NetToneStart       (TAPI_CONNECTION *pChannel, IFX_TAPI_TONE_SIMPLE_t const *pTone);
IFX_return_t  TAPI_LL_Phone_NetToneStop        (TAPI_CONNECTION *pChannel);
/*#endif*/ /* NETWORK_TONE */
#endif
#endif /* TAPI_VOICE */
/* T38 Fax Services */
#ifdef TAPI_FAX_T38
IFX_return_t  TAPI_LL_FaxT38_SetModulator      (TAPI_CONNECTION *pChannel, IFX_TAPI_T38_MOD_DATA_t const *pFaxMod);
IFX_return_t  TAPI_LL_FaxT38_SetDemodulator    (TAPI_CONNECTION *pChannel, IFX_TAPI_T38_DEMOD_DATA_t const *pFaxDemod);
IFX_return_t  TAPI_LL_FaxT38_DisableDataPump   (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_FaxT38_GetStatus         (TAPI_CONNECTION *pChannel, IFX_TAPI_T38_STATUS_t *pFaxStatus);
#endif /* TAPI_FAX_T38 */
#ifdef TAPI_CID
IFX_return_t  TAPI_LL_Phone_Dtmf_GetCode            (IFX_char_t nChar, IFX_uint8_t *pDtmfCode);
IFX_return_t  TAPI_LL_Phone_CID_Sending             (TAPI_CONNECTION *pChannel, TAPI_CID_DATA const *pCid);
IFX_return_t  TAPI_LL_Phone_CidRx_Start             (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_CidRx_Stop              (TAPI_CONNECTION *pChannel);
IFX_void_t  TAPI_LL_Phone_CidRxDataCollect        (TAPI_CONNECTION *pChannel);
IFX_return_t  TAPI_LL_Phone_CidRx_Status            (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_RX_STATUS_t *pCidRxStatus);
IFX_return_t  TAPI_LL_Phone_Get_CidRxDataCollected  (TAPI_CONNECTION *pChannel, IFX_TAPI_CID_RX_DATA_t *pCidRxData);
IFX_return_t  TAPI_LL_Phone_CID_Send_AS             (TAPI_CONNECTION *pChannel);
#endif /* TAPI_CID */
#ifdef TAPI_DTMF
IFX_return_t  TAPI_LL_Phone_Send_DTMF_OOB          (TAPI_CONNECTION *pChannel, IFX_TAPI_PKT_EV_OOB_t nArg);
#endif /* TAPI_DTMF */
IFX_return_t  TAPI_LL_Phone_Volume                 (TAPI_CONNECTION *pChannel, IFX_TAPI_LINE_VOLUME_t const *pVol);
IFX_int32_t   TAPI_LL_Phone_High_Level             (TAPI_CONNECTION *pChannel, IFX_int32_t bEnable);
IFX_return_t  TAPI_LL_Enable_Signal                (TAPI_CONNECTION *pChannel, IFX_TAPI_SIG_DETECTION_t const *pSig);
IFX_return_t  TAPI_LL_Disable_Signal               (TAPI_CONNECTION *pChannel, IFX_TAPI_SIG_DETECTION_t const *pSig);
IFX_return_t  TAPI_LL_CPTD_Start                   (TAPI_CONNECTION *pChannel, IFX_TAPI_TONE_SIMPLE_t const *pTone, IFX_int32_t signal);
IFX_return_t  TAPI_LL_CPTD_Stop                    (TAPI_CONNECTION *pChannel);
IFX_int32_t   TAPI_LL_Restore_FaultLine            (TAPI_CONNECTION *pChannel);

#ifdef TAPI_LL_DRV
typedef struct _TAPI_CHANNEL  TAPI_CHANNEL_t;
typedef struct _TAPI_LL_CTX   TAPI_LL_CTX_t;

extern IFX_int32_t TAPI_LL_DrvInit (TAPI_LL_CTX_t* pCtx);
#endif /* TAPI_LL_DRV */

/* ======================================== */
/**  Timer functions                        */
/* ======================================== */

Timer_ID          TAPI_Create_Timer      (TIMER_ENTRY pTimerEntry, IFX_int32_t nArgument);
IFX_boolean_t     TAPI_SetTime_Timer     (Timer_ID Timer, IFX_uint32_t nTime, IFX_boolean_t bPeriodically, IFX_boolean_t bRestart);
IFX_boolean_t     TAPI_Delete_Timer      (Timer_ID Timer);
IFX_boolean_t     TAPI_Stop_Timer        (Timer_ID Timer);

/* ======================================== */
/**  Operating system specific functions    */
/* ======================================== */

IFX_int32_t TAPI_Ioctl  (TAPI_CONNECTION *pChannel, IFX_uint32_t cmd, IFX_uint32_t arg);

#ifdef TAPI_CID

#ifndef LINUX
#define TAPI_MapBuffer(p_buffer,size)  (IFX_void_t*)(p_buffer)
#define TAPI_UnmapBuffer(p_buffer)     /* nothing */
#else /* LINUX */
IFX_void_t* TAPI_MapBuffer    (IFX_void_t* p_buffer, IFX_uint32_t size);
IFX_void_t TAPI_UnmapBuffer   (IFX_void_t* p_buffer);
#endif /* LINUX */

#endif /* TAPI_CID */


/*@}*/

#endif  /* DRV_TAPI_H */

