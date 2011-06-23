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

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND
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

/* min time before cid:  500 ms, ref : ETSI EN 300 659-1 V1.3.1*/
#define TAPI_MIN_TIME_BEFORE_CID 500
/* min time after cid:  200 ms, ref : ETSI EN 300 659-1 V1.3.1*/
#define TAPI_MIN_TIME_AFTER_CID  200
/* CID baudrate = 150 Bytes/sec without Pause. */
/* cid byte transmission time : approx 10 ms + 5 ms request time */
#define TAPI_CID_BYTE_TXTIME_MS  15 /* 15 ms */

#define USA_RING_CADENCE         0xC0C0

#define TAPI_MAX_TONE_CODE           256
#define TAPI_MAX_IMPEDANCE_SPEC_ID   6

#define TAPI_MIN_FLASH            80
#define TAPI_MAX_FLASH            200
#define TAPI_MIN_DIGIT_LOW        30
#define TAPI_MAX_DIGIT_LOW        80
#define TAPI_MIN_DIGIT_HIGH       30
#define TAPI_MAX_DIGIT_HIGH       80
#define TAPI_MIN_OFF_HOOK         40
#define TAPI_MIN_ON_HOOK          400
#define TAPI_MIN_INTERDIGIT       300

#define TAPI_DTMF_DEFAULT_STARTTONE      'A'
#define TAPI_DTMF_DEFAULT_STOPTONE       'C'
#define TAPI_DTMF_DEFAULT_INFOSTARTTONE  'B'
#define TAPI_DTMF_DEFAULT_REDIRSTARTTONE 'D'
#define TAPI_DTMF_DEFAULT_DIGITTIME      50 /*ms*/
#define TAPI_DTMF_DEFAULT_INTERDIGITTIME 50 /*ms*/

#define TAPI_CID2_CASDURATION_DEFAULT    80
#define TAPI_CID2_ACKTONE_DEFAULT        'D'
#define TAPI_CID2_CAS2ACK_DEFAULTTIME    100 /* ms */
#define TAPI_CID2_ACK2FSKOUT_DEFAULTTIME 45

/* default CID2 defines */

/* minimum time (ms) from CAS to ACK */
#define TAPI_CID2_MINTIME_CAS2ACK      55
/* maximum time (ms) from CAS to ACK */
#define TAPI_CID2_MAXTIME_CAS2ACK      100
/* minimum time (ms) from ACK to FSK data out */
#define TAPI_CID2_MINTIME_ACK2FSKOUT   45
/* maximum time (ms) from ACK to FSK data out */
#define TAPI_CID2_MAXTIME_ACK2FSKOUT   575

/* NTT CID2 defines */
/* default time (ms) from Alert Tone to FSK data out*/
#define TAPI_CID2_DEFAULTTIME_AT2FSKOUT   300
/* default time (ms)from FSK data out to line restoration */
#define TAPI_CID2_DEFAULTTIME_FSKOUT2VOICE 100

/** time in ms while the line is in power down after the first Ground Key High */
#define GNDKH_RECHECKTIME1 10
/** time in ms to check the next Ground Key High after reenabling the line */
#define GNDKH_RECHECKTIME2 20

/* Upper limit for used channels as used in IFXPHONE_PHONE_STATUS */
#define TAPI_MAX_CHANNELS                 128
/** Maximum tone generators that can run in parallel */
#define TAPI_TONE_MAXRES 2
/* Signal detection masks */

/** V.25 2100Hz (CED) Modem/Fax Tone mask */
#define TAPI_SIGNAL_CEDMASK               (TAPI_SIGNAL_CED |               \
                                          TAPI_SIGNAL_CEDTX |              \
                                          TAPI_SIGNAL_CEDRX)
/** CNG Fax Calling Tone (1100 Hz) mask */
#define TAPI_SIGNAL_CNGFAXMASK            (TAPI_SIGNAL_CNGFAX |            \
                                          TAPI_SIGNAL_CNGFAXRX |           \
                                          TAPI_SIGNAL_CNGFAXTX)
/** CNG Modem Calling Tone (1300 Hz) mask */
#define TAPI_SIGNAL_CNGMODMASK            (TAPI_SIGNAL_CNGMOD |            \
                                          TAPI_SIGNAL_CNGMODRX |           \
                                          TAPI_SIGNAL_CNGMODTX)

/** Phase reversal detection mask */
#define TAPI_SIGNAL_PHASEREVMASK          (TAPI_SIGNAL_PHASEREV |          \
                                          TAPI_SIGNAL_PHASEREVRX |         \
                                          TAPI_SIGNAL_PHASEREVTX)
/** Mask the paths for DIS */
#define TAPI_SIGNAL_DISMASK               (TAPI_SIGNAL_DIS |               \
                                          TAPI_SIGNAL_DISRX |              \
                                          TAPI_SIGNAL_DISTX)

/** Amplitude modulation mask */
#define TAPI_SIGNAL_AMMASK                (TAPI_SIGNAL_AM |                \
                                          TAPI_SIGNAL_AMRX |               \
                                          TAPI_SIGNAL_AMTX)
/** Modem tone holding signal stopped mask */
#define TAPI_SIGNAL_TONEHOLDING_ENDMASK  (TAPI_SIGNAL_TONEHOLDING_END |    \
                                          TAPI_SIGNAL_TONEHOLDING_ENDRX |  \
                                          TAPI_SIGNAL_TONEHOLDING_ENDTX)
/** End of signal CED detection mask */
#define TAPI_SIGNAL_CEDENDMASK            (TAPI_SIGNAL_CEDEND |            \
                                          TAPI_SIGNAL_CEDENDRX |           \
                                          TAPI_SIGNAL_CEDENDTX)
/** V8bis mask */
#define TAPI_SIGNAL_V8BISMASK             (TAPI_SIGNAL_V8BISRX |           \
                                           TAPI_SIGNAL_V8BISTX)

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
   TAPI_RING_STATE_IDLE = 0,
   /* special alert for CID including ringing */
   TAPI_RING_STATE_SALERT = 1,
   /* cid processing start */
   TAPI_RING_STATE_CID_START = 2,
   /* cid processing stop */
   TAPI_RING_STATE_CID_END = 3,
   /* initial non periodical ring pause */
   TAPI_RING_STATE_INITIAL_RINGING = 4,
   /* periodical ring/pause cadence */
   TAPI_RING_STATE_PERIODICAL_RINGING = 5
};

/* =============================== */
/* Defines for CID tx types        */
/* =============================== */

typedef enum
{
   /* CID type 1 : Onhook cid with ringing */
   TAPI_CID_TYPE1,
   /* CID type 2 : Offhook CID with voice */
   TAPI_CID_TYPE2
} TAPI_CID_TYPE;

/* =============================== */
/* Defines for CID1 operations     */
/* =============================== */
enum
{
   /* initialization state */
   TAPI_CID1_ALERT = 0,
   /* timer send CID */
   TAPI_CID1_SEND = 1,
   /* initial non periodical ring pause */
   TAPI_CID1_END = 2
};

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
   /* phone is onhook, and no timers are running */
   TAPI_HOOK_STATE_ONHOOK,
   /* phone has remained onhook long enough to be low pulse: wait for next offhook to confirm */
   TAPI_HOOK_STATE_PULSE_L_CONFIRM,
   /* phone onhook (too long to be digit pulse): could be flash or final onhook */
   TAPI_HOOK_STATE_FLASH_WAIT,
   /* phone onhook (long enough to be flash): wait for next offhook to confirm this */
   TAPI_HOOK_STATE_FLASH_CONFIRM,
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

/* =============================== */
/* Defines for Cid transmission    */
/* states                          */
/* =============================== */

enum
{
   CID_TX_NONE,
   CID_TX_START,
   CID_TX_HOOK,
   CID_TX_END
};

/* ================================== */
/* CID 2 states for the state machine */
/* ================================== */
typedef enum
{
   /* Send alert signal,
      valid for all actually implemented standards */
   TAPI_CID2_SEND_AS,
   /* Mute voice,
      valid for all actually implemented standards */
   TAPI_CID2_MUTE_VOICE,
   /* Check acknoledge, not valid for nttS */
   TAPI_CID2_CHECK_ACK,
   /* Send FSK,
      valid for all actually implemented standards */
   TAPI_CID2_SEND_FSK,
   /* Restore voice path,
      valid for all actually implemented standards */
   TAPI_CID2_RESTORE_VOICE
} TAPI_CID2_STATE;

/** \internal */

#ifdef TAPI_CID
/* ============================= */
/* Structure for CID data        */
/* ============================= */
typedef struct
{
   /* sets CID type 1 or type 2 */
   TAPI_CID_TYPE           nCidType;
   /* flag to know if data is new */
   IFX_boolean_t           bIsNewData;
   /* count ring bursts */
   IFX_uint32_t            nBurstCnt;
   /* Buffer for CID parameter coding  */
   IFX_uint8_t             cidParam [TAPI_MAX_CID_PARAMETER];
   /* number of CID parameters octets */
   IFX_uint8_t             nCidParamLen;
   /* CID transmission state */
   IFX_uint32_t            nCidTxState;
   /* additional time after CID is sent */
   IFX_uint32_t            nAdditionalTime;
   /* alert event */
   volatile IFX_boolean_t  bCidAlertEvt;
} TAPI_CID_DATA;

typedef struct
{
   /* status */
   TAPI_CIDRX_STATUS stat;
   /* ptr to actual cid rx buffer */
   TAPI_CIDRX_DATA   *pData;
   /* cid rx fifo */
   FIFO              TapiCidRxFifo;
} TAPI_CIDRX;

typedef struct
{
   /* cid type 2 config data */
   TAPI_CID2_CONFIG           conf;
   /* ASCII character for Acknowledgement */
   IFX_char_t                 AckAscii;

   /* cid type 2 flow state */
   IFX_uint32_t               state;
   /* timer id for CID2 flow */
   Timer_ID                   Cid2TimerID;
   /* is channel sending cid type 2? */
   volatile IFX_boolean_t     bCid2IsOn;
   /* acknowledge flag */
   volatile IFX_boolean_t     bAck;
   /* alert signal transmit flag  */
   volatile IFX_boolean_t     bAStxOk;
   /* alert signal event if */
   IFXOS_event_t     asEvt;
} TAPI_CID2;

#endif /* TAPI_CID */

/* ============================= */
/** Structure for ring data
    \internal */
/* ============================= */
typedef struct
{
   /* timer id for ringing service */
   Timer_ID                RingTimerID;
   /* actual ring cadence state */
   IFX_uint8_t             nRingTimerState;
   /* is channel in ring_burst mode? */
   IFX_boolean_t           bIsRingBurstState;
   /* is channel ringing mode? */
   volatile IFX_boolean_t  bRingingMode;
   /* last linemode befor ringing */
   IFX_uint8_t             nLastLineMode;
   /* cadence data */
   TAPI_RING_CADENCE       RingCadence;
   /* ring configuration data */
   TAPI_RING_CONFIG        RingConfig;
   /* counter for cadence */
   IFX_uint8_t             nByteCounter;
   IFX_uint8_t             nBitCounter;
   /* maximum rings for blocking ring */
   IFX_uint32_t            nMaxRings;
   /* remaining rings during blocking ring */
   IFX_uint32_t            nRingsLeft;
}TAPI_RING_DATA;

/*@{*/

/* ============================= */
/* Structure for pcm data        */
/* ============================= */
typedef struct
{
   /* configuration data for pcm services */
   TAPI_PCM_CONFIG   PCMConfig;
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
   TAPI_EXCEPTION    nException;
   /* exception status mask */
   TAPI_EXCEPTION    nExceptionMask;
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
   /* fifo */
   FIFO              TapiDTMFFifo;
   FIFO              TapiPulseFifo;
   /* fifo buffer for pulse dial storage */
   IFX_uint8_t       pPulseStore[TAPI_DIAL_FIFO_SIZE];
   /* fifo buffer for DTMF dial storage */
   IFX_uint8_t       pDtmfStore[TAPI_DTMF_FIFO_SIZE];
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
   TAPI_METER_CONFIG  MeterConfig;
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
      TAPI_SIMPLE_TONE     simple;
      TAPI_COMPOSED_TONE   composed;
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
   /** which source plays the tone (firmware module or analog tone generator */
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
   TAPI_LECCONF         TapiLecData;
   TAPI_LECCONF         TapiLecPcmData;
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
   TAPI_JB_CONF         TapiJbData;
   /* actual channel codec from TAPI_PHONE_CODEC */
   IFX_int32_t          nCodec;
   /* event payload type*/
   IFX_uint8_t          nEvtPT;
   /** store currently configured SSRC */
   IFX_uint32_t         nSsrc;
#endif /* TAPI_VOICE */
#if (defined (TAPI_VOICE) || defined (TAPI_FAX_T38))
   /* stores the current fax status */
   TAPI_FAX_STATUS      TapiFaxStatus;
#endif /* TAPI_VOICE || TAPI_FAX_T38 */
#ifdef TAPI_CID
   /* caller id receiver data */
   TAPI_CIDRX           TapiCidRx;
   TAPI_CID2            TapiCid2;
   /* caller id config */
   TAPI_CID_CONFIG      TapiCidConfig;
   /*Dtmf Cid config */
   TAPI_DTMFCID         TapiDtmfCidConf;
   /* caller id transmit data */
   TAPI_CID_DATA        TapiCidTx;
#endif /* TAPI_CID */
   /* stores the validation timer settings */
   TAPI_VALIDATION_TIMES TapiHookOffTime;
   TAPI_VALIDATION_TIMES TapiHookOnTime;
   TAPI_VALIDATION_TIMES TapiHookFlashTime;
   TAPI_VALIDATION_TIMES TapiDigitLowTime;
   TAPI_VALIDATION_TIMES TapiDigitHighTime;
   TAPI_VALIDATION_TIMES TapiInterDigitTime;
   /* complex tone data */
   TAPI_TONE_DATA  TapiComplexToneData [TAPI_TONE_MAXRES];
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
IFX_int32_t TAPI_Phone_Ring_Cadence_High_Res   (TAPI_CONNECTION *pChannel, TAPI_RING_CADENCE const *pCadence);
IFX_int32_t TAPI_Phone_Ring_Start              (TAPI_CONNECTION *pChannel, TAPI_PHONE_CID const *pPhoneCid);
IFX_int32_t TAPI_Phone_Ring_Stop               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Ring_Config             (TAPI_CONNECTION *pChannel, TAPI_RING_CONFIG const *pRingConfig);
IFX_int32_t TAPI_Phone_Ring_GetConfig          (TAPI_CONNECTION *pChannel, TAPI_RING_CONFIG *pRingConfig);
IFX_int32_t TAPI_Phone_Set_MaxRings            (TAPI_CONNECTION *pChannel, IFX_uint32_t nArg);
IFX_int32_t TAPI_Phone_Ring                    (TAPI_CONNECTION *pChannel);
IFX_void_t  TAPI_Phone_Ring_OnTimer            (Timer_ID Timer, IFX_int32_t nArg);

/* Operation Control Services */
IFX_int32_t TAPI_Phone_Hookstate               (TAPI_CONNECTION *pChannel, IFX_int32_t *pState);
IFX_int32_t TAPI_Phone_Set_Linefeed            (TAPI_CONNECTION *pChannel, IFX_int32_t nMode);
IFX_int32_t TAPI_Phone_LecConf                 (TAPI_CONNECTION *pChannel, TAPI_LECCONF *pLecConf);
IFX_int32_t TAPI_Phone_GetLecConf              (TAPI_CONNECTION *pChannel, TAPI_LECCONF *pLecConf);
IFX_int32_t TAPI_Phone_LecConf_Pcm             (TAPI_CONNECTION *pChannel, TAPI_LECCONF *pLecConf);
IFX_int32_t TAPI_Phone_GetLecConf_Pcm          (TAPI_CONNECTION *pChannel, TAPI_LECCONF *pLecConf);
#ifdef TAPI_CID
IFX_int32_t TAPI_Phone_CID_SetData             (TAPI_CONNECTION *pChannel, TAPI_PHONE_CID const *pPhoneCid);
IFX_int32_t TAPI_Phone_CID_Config              (TAPI_CONNECTION *pChannel, TAPI_CID_CONFIG const *pCidConfig);
IFX_int32_t TAPI_Phone_CID_GetConfig           (TAPI_CONNECTION *pChannel, TAPI_CID_CONFIG *pCidConfig);
IFX_int32_t TAPI_Phone_CID_ConfigDtmf          (TAPI_CONNECTION *pChannel, TAPI_DTMFCID const *pDtmfCid);
IFX_int32_t TAPI_Phone_CID_Send                (TAPI_CONNECTION *pChannel, Timer_ID Timer);
IFX_int32_t TAPI_Phone_CID1_OpControl          (TAPI_CONNECTION *pChannel, IFX_int32_t nOperation, Timer_ID timerId,
                                                IFX_int32_t (*alert)(TAPI_CONNECTION*),
                                                IFX_int32_t (*cidtxend)(TAPI_CONNECTION*));
IFX_int32_t TAPI_Phone_CID_Info_Tx             (TAPI_CONNECTION *pChannel, TAPI_CID_INFO_t const *pCidInfo);
IFX_int32_t TAPI_Phone_CidRx_Start             (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_CidRx_Stop              (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Get_CidRxData           (TAPI_CONNECTION *pChannel, TAPI_CIDRX_DATA *pCidRxData);
TAPI_CIDRX_DATA *TAPI_Phone_GetCidRxBuf        (TAPI_CONNECTION *pChannel, IFX_uint32_t nLen);
IFX_int32_t TAPI_Phone_CID2_Config             (TAPI_CONNECTION *pChannel, TAPI_CID2_CONFIG *pCid2Conf);
IFX_int32_t TAPI_Phone_CID2_GetConfig          (TAPI_CONNECTION *pChannel, TAPI_CID2_CONFIG *pCid2Conf);
IFX_int32_t TAPI_Phone_CID2_Send               (TAPI_CONNECTION *pChannel, TAPI_PHONE_CID const *pPhoneCid);
#endif /* TAPI_CID */

/* Tone Services */
IFX_int32_t TAPI_Phone_Tone_Set_On_Time        (TAPI_CONNECTION *pChannel, IFX_uint32_t nTime);
IFX_int32_t TAPI_Phone_Tone_Set_Off_Time       (TAPI_CONNECTION *pChannel, IFX_uint32_t nTime);
IFX_int32_t TAPI_Phone_Tone_Get_On_Time        (TAPI_CONNECTION *pChannel, IFX_uint32_t *pOnTime);
IFX_int32_t TAPI_Phone_Tone_Get_Off_Time       (TAPI_CONNECTION *pChannel, IFX_uint32_t *pOffTime);
IFX_int32_t TAPI_Phone_Tone_Play               (TAPI_CONNECTION *pChannel,
                                                IFX_int32_t nToneIndex,
                                                TAPI_TONE_DST dst);
IFX_int32_t TAPI_Phone_Tone_Set_Level          (TAPI_CONNECTION *pChannel, TAPI_TONE_LEVEL const *pToneLevel);
IFX_int32_t TAPI_Phone_Tone_Get_State          (TAPI_CONNECTION *pChannel, IFX_uint32_t *pToneState);
IFX_int32_t TAPI_Phone_Tone_Dial               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Tone_Ringback           (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Tone_Busy               (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Add_SimpleTone          (COMPLEX_TONE* toneCoefficients,
                                                TAPI_SIMPLE_TONE const *pSimpleTone);
IFX_int32_t TAPI_Phone_Add_ComposedTone        (COMPLEX_TONE* toneCoefficients,
                                                TAPI_COMPOSED_TONE const *pComposedTone);
IFX_int32_t TAPI_Phone_Tone_TableConf          (COMPLEX_TONE* toneCoefficients,
                                                TAPI_TONE const *pTone);

IFX_int32_t TAPI_Phone_DetectToneStart         (TAPI_CONNECTION *pChannel, TAPI_PHONE_CPTD const *signal);
IFX_int32_t TAPI_Phone_DetectToneStop          (TAPI_CONNECTION *pChannel);

/* PCM Services */
IFX_int32_t TAPI_Phone_PCM_Set_Config          (TAPI_CONNECTION *pChannel, TAPI_PCM_CONFIG const *pPCMConfig);
IFX_int32_t TAPI_Phone_PCM_Get_Config          (TAPI_CONNECTION *pChannel, TAPI_PCM_CONFIG *pPCMConfig);
IFX_int32_t TAPI_Phone_PCM_Set_Activation      (TAPI_CONNECTION *pChannel, IFX_uint32_t nMode);
IFX_int32_t TAPI_Phone_PCM_Get_Activation      (TAPI_CONNECTION *pChannel);

/* Miscellaneous Services */
IFX_int32_t TAPI_Phone_Get_Version             (IFX_char_t * version_string);
IFX_int32_t TAPI_Phone_Check_Version           (TAPI_PHONE_VERSION const *vers);
IFX_uint32_t TAPI_Phone_Exception              (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_Phone_Mask_Exception          (TAPI_CONNECTION *pChannel, IFX_uint32_t nException);
IFX_int32_t TAPI_Phone_Init                    (TAPI_CONNECTION *pChannel, TAPI_INIT const *pInit);
IFX_int32_t TAPI_Phone_Validation_Time         (TAPI_CONNECTION *pChannel, TAPI_VALIDATION_TIMES const *pTime);
IFX_int32_t TAPI_Phone_GetStatus               (TAPI_CONNECTION *pChannel, TAPI_PHONE_STATUS *status);
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
extern IFX_int32_t TAPI_Phone_Meter_Config     (TAPI_CONNECTION *pChannel, TAPI_METER_CONFIG const *pMeterConfig);

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
IFX_int32_t TAPI_Data_Channel_Add              (TAPI_CONNECTION *pChannel, TAPI_DATA_MAPPING const *pMap);
IFX_int32_t TAPI_Data_Channel_Remove           (TAPI_CONNECTION *pChannel, TAPI_DATA_MAPPING const *pMap);
IFX_int32_t TAPI_Phone_Channel_Add             (TAPI_CONNECTION *pChannel, TAPI_PHONE_MAPPING const *pMap);
IFX_int32_t TAPI_Phone_Channel_Remove          (TAPI_CONNECTION *pChannel, TAPI_PHONE_MAPPING const *pMap);
IFX_int32_t TAPI_Phone_Fax_JB_Set              (TAPI_CONNECTION *pChannel, TAPI_JB_CONF *pJbConf);
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
                                                TAPI_LINESTATUS stat);
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

IFX_int32_t TAPI_LL_Phone_Init                 (TAPI_CONNECTION *pChannel, TAPI_INIT const *pInit);
IFX_int32_t TAPI_LL_Phone_SwitchLine           (TAPI_CONNECTION *pChannel, IFX_int32_t nMode);
IFX_int32_t TAPI_LL_Phone_AutoBatterySwitch (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Polarity             (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_PCM_Activation       (TAPI_CONNECTION *pChannel, IFX_uint32_t nMode);
IFX_int32_t TAPI_LL_Phone_PCM_Config           (TAPI_CONNECTION *pChannel, TAPI_PCM_CONFIG const *pPCMConfig);
IFX_int32_t TAPI_LL_Phone_Pcm_Volume           (TAPI_CONNECTION *pChannel, TAPI_PHONE_VOLUME const *pVol);
IFX_int32_t TAPI_LL_Phone_Ring_Config          (TAPI_CONNECTION *pChannel, TAPI_RING_CONFIG const *pRingConfig);
IFX_int32_t TAPI_LL_Phone_Meter_Config         (TAPI_CONNECTION *pChannel, IFX_uint8_t nMode, IFX_uint8_t nFreq);
IFX_int32_t TAPI_LL_Phone_Tone_Play            (TAPI_CONNECTION *pChannel, IFX_uint8_t res,
                                                COMPLEX_TONE const *pToneCoeff,
                                                TAPI_TONE_DST dst);
IFX_int32_t TAPI_LL_Phone_Tone_Stop            (TAPI_CONNECTION *pChannel, IFX_uint8_t res,
                                                COMPLEX_TONE const *pToneCoeff);
IFX_int32_t TAPI_LL_Phone_Tone_Set_Level       (TAPI_CONNECTION *pChannel, TAPI_TONE_LEVEL const *pToneLevel);
IFX_int32_t TAPI_LL_Phone_Get_Capabilities     (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Get_Capability_List  (TAPI_CONNECTION *pChannel, TAPI_PHONE_CAPABILITY *pCapList);
IFX_int32_t TAPI_LL_Phone_Check_Capability     (TAPI_CONNECTION *pChannel, TAPI_PHONE_CAPABILITY *pCapList);
IFX_int32_t TAPI_LL_Phone_VAD                  (TAPI_CONNECTION *pChannel, IFX_int32_t nVAD);
IFX_int32_t TAPI_LL_Phone_LecConf              (TAPI_CONNECTION *pChannel, TAPI_LECCONF const *pLecConf);
IFX_int32_t TAPI_LL_Phone_LecConf_Pcm          (TAPI_CONNECTION *pChannel, TAPI_LECCONF const *pLecConf);
IFX_return_t TAPI_LL_Phone_AgcEnable           (TAPI_CONNECTION *pChannel, TAPI_AGC_MODE Param);
#ifdef TAPI_VOICE
IFX_int32_t TAPI_LL_Phone_Set_Frame_Length   (TAPI_CONNECTION *pChannel, IFX_int32_t nFrameLength);
IFX_int32_t TAPI_LL_Phone_Get_Frame_Length   (TAPI_CONNECTION *pChannel, IFX_int32_t *pFrameLength);
IFX_int32_t TAPI_LL_Phone_Set_Rec_Codec      (TAPI_CONNECTION *pChannel, IFX_int32_t nCodec);
IFX_int32_t TAPI_LL_Phone_Start_Recording    (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Stop_Recording     (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Start_Playing      (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Stop_Playing       (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Voice_Enable             (TAPI_CONNECTION *pChannel, IFX_uint8_t nMode);
IFX_int32_t TAPI_LL_Data_Channel_Add         (TAPI_CONNECTION *pChannel, TAPI_DATA_MAPPING const *pMap);
IFX_int32_t TAPI_LL_Data_Channel_Remove      (TAPI_CONNECTION *pChannel, TAPI_DATA_MAPPING const *pMap);
IFX_int32_t TAPI_LL_Phone_Channel_Add        (TAPI_CONNECTION *pChannel, TAPI_PHONE_MAPPING const *pMap);
IFX_int32_t TAPI_LL_Phone_Channel_Remove     (TAPI_CONNECTION *pChannel, TAPI_PHONE_MAPPING const *pMap);
IFX_int32_t TAPI_LL_PCM_Channel_Add          (TAPI_CONNECTION *pChannel, TAPI_PCM_MAPPING const *pMap);
IFX_int32_t TAPI_LL_PCM_Channel_Remove       (TAPI_CONNECTION *pChannel, TAPI_PCM_MAPPING const *pMap);
IFX_int32_t TAPI_LL_Phone_RtpConf            (TAPI_CONNECTION *pChannel, TAPI_RTP_CONF const *pRtpConf);
IFX_int32_t TAPI_LL_Phone_Rtp_SetPayloadType (TAPI_CONNECTION *pChannel, TAPI_RTP_PT_CONF const *pRtpPTConf);
IFX_int32_t TAPI_LL_Phone_RTCP_GetStatistics (TAPI_CONNECTION *pChannel, TAPI_RTCP_DATA *pRtcpData);
IFX_int32_t TAPI_LL_Phone_RTCP_PrepareStatistics (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_RTCP_GatherStatistics  (TAPI_CONNECTION *pChannel, TAPI_RTCP_DATA *pRTCP);
IFX_int32_t TAPI_LL_Phone_RTCP_Reset         (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_JB_Set             (TAPI_CONNECTION *pChannel, TAPI_JB_CONF const *pJbConf);
IFX_int32_t TAPI_LL_Phone_JB_GetStatistics   (TAPI_CONNECTION *pChannel, TAPI_JB_DATA *pJbData);
IFX_int32_t TAPI_LL_Phone_JB_Reset           (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_Aal_Set            (TAPI_CONNECTION *pChannel, TAPI_AAL_CONF const *pAalConf);
#if 0
IFX_int32_t TAPI_LL_Phone_Aal_SwitchCoder    (TAPI_CONNECTION *pChannel);
#endif /* 0 */
IFX_int32_t TAPI_LL_Phone_AalProfile         (TAPI_CONNECTION *pChannel, TAPI_AAL_PROFILE const *pProfile);
#if 0
IFX_int32_t TAPI_LL_Fax_SwitchMode           (TAPI_CONNECTION *pChannel, TAPI_FAX_MODE const *pFaxMode);
/*#ifdef NETWORK_TONE*/
IFX_int32_t TAPI_LL_Phone_NetToneStart       (TAPI_CONNECTION *pChannel, TAPI_SIMPLE_TONE const *pTone);
IFX_int32_t TAPI_LL_Phone_NetToneStop        (TAPI_CONNECTION *pChannel);
/*#endif*/ /* NETWORK_TONE */
#endif
#endif /* TAPI_VOICE */
/* T38 Fax Services */
#ifdef TAPI_FAX_T38
IFX_int32_t TAPI_LL_FaxT38_SetModulator      (TAPI_CONNECTION *pChannel, TAPI_FAX_MODDATA const *pFaxMod);
IFX_int32_t TAPI_LL_FaxT38_SetDemodulator    (TAPI_CONNECTION *pChannel, TAPI_FAX_DEMODDATA const *pFaxDemod);
IFX_int32_t TAPI_LL_FaxT38_DisableDataPump   (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_FaxT38_GetStatus         (TAPI_CONNECTION *pChannel, TAPI_FAX_STATUS *pFaxStatus);
#endif /* TAPI_FAX_T38 */
#ifdef TAPI_CID
IFX_int8_t  TAPI_LL_Phone_Dtmf_GetCode            (IFX_char_t nChar);
IFX_int32_t TAPI_LL_Phone_CID_ConfDtmf            (TAPI_CONNECTION *pChannel, TAPI_DTMFCID const *pDtmfCid);
IFX_int32_t TAPI_LL_Phone_CID_ConfFsk             (TAPI_CONNECTION *pChannel, TAPI_CID_CONFIG const *pCidConfig);
IFX_int32_t TAPI_LL_Phone_CID_Sending             (TAPI_CONNECTION *pChannel, TAPI_CID_DATA const *pCid);
IFX_int32_t TAPI_LL_Phone_CidRx_Start             (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_CidRx_Stop              (TAPI_CONNECTION *pChannel);
IFX_void_t  TAPI_LL_Phone_CidRxDataCollect        (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Phone_CidRx_Status            (TAPI_CONNECTION *pChannel, TAPI_CIDRX_STATUS *pCidRxStatus);
IFX_int32_t TAPI_LL_Phone_Get_CidRxDataCollected  (TAPI_CONNECTION *pChannel, TAPI_CIDRX_DATA *pCidRxData);
IFX_int32_t TAPI_LL_Phone_CID_Send_AS             (TAPI_CONNECTION *pChannel);
#endif /* TAPI_CID */
#ifdef TAPI_DTMF
IFX_int32_t TAPI_LL_Phone_Send_DTMF_OOB          (TAPI_CONNECTION *pChannel, IFX_int32_t nArg);
#endif /* TAPI_DTMF */
IFX_int32_t TAPI_LL_Phone_Volume                 (TAPI_CONNECTION *pChannel, TAPI_PHONE_VOLUME const *pVol);
IFX_int32_t TAPI_LL_Phone_High_Level             (TAPI_CONNECTION *pChannel, IFX_int32_t bEnable);
IFX_int32_t TAPI_LL_Enable_Signal                (TAPI_CONNECTION *pChannel, TAPI_PHONE_SIGDETECTION const *pSig);
IFX_int32_t TAPI_LL_Disable_Signal               (TAPI_CONNECTION *pChannel, TAPI_PHONE_SIGDETECTION const *pSig);
IFX_int32_t TAPI_LL_CPTD_Start                   (TAPI_CONNECTION *pChannel, TAPI_SIMPLE_TONE const *pTone, IFX_int32_t signal);
IFX_int32_t TAPI_LL_CPTD_Stop                    (TAPI_CONNECTION *pChannel);
IFX_int32_t TAPI_LL_Restore_FaultLine            (TAPI_CONNECTION *pChannel);

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

IFX_int32_t TAPI_Ioctl                           (TAPI_CONNECTION *pChannel, IFX_uint32_t cmd, IFX_uint32_t arg);

/*@}*/

#endif  /* DRV_TAPI_H */

