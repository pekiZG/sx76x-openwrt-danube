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
   Module      : drv_tapi_cid.c

   Implementation of features related to CID (CID1/CID2/CID RX)

   All operations done by functions in this module are data
   related and assume a data channel file descriptor.
   Caller of anyone of the functions must make sure that a data
   channel is used. In case phone channel functions are invoked here,
   an instance of the phone channel must be passed.

*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_tapi.h"

#ifdef TAPI_CID

/* ================================= */
/* FSK defines (transmission time)   */
/* ================================= */

/* CID FSK transmit time tolerance,
   set to 5 ms for cid request time and software processing */
#define CID_FSK_TOLERANCE_MS     5

/* CID FSK transmission bitrate,  1200 bits/sec */
#define CID_FSK_BITRATE_SEC      1200

/* Number of bits per FSK character data
  Note : The CID sender encode each character data byte in a 10 bit long byte  */
#define CID_FSK_BYTE_BITSNUM     10

/* ============================= */
/* FSK defines (ETSI)            */
/* ============================= */

/* size of date element */
#define CIDFSK_DATE_LEN          0x08

/* maximum length of Service Type elements (the element buffer size is
   independent and may be an additional restriction) */
#define CID_ST_MSGIDENT_LEN      3
#define CID_ST_DURATION_LEN      6
#define CID_ST_XOPUSE_LEN        10
#define CID_ST_CHARGE_LEN        16
#define CID_ST_NUM_LEN           20
#define CID_ST_TERMSEL_LEN       21
#define CID_ST_NAME_LEN          50
#define CID_ST_DISP_LEN          253

/* ============================= */
/* FSK defines (NTT)             */
/* ============================= */

/* positions in buffer */
#define CID_NTT_TYPE_POS         5
#define CID_NTT_MSGLEN_POS       6

/* parameter types with corresponding length */
#define CID_NTT_PT_NUMBER        0x02
#define CID_NTT_NUMBER_MAXLEN    0x14
#define CID_NTT_PT_NAME          0x03
#define CID_NTT_NAME_MAXLEN      0x14

/* control codes <b7b6b5 b4b3b2b1> */
#define CID_NTT_DLE              0x10   /* 0b 001 0000 */
#define CID_NTT_SOH              0x01   /* 0b 000 0001 */
#define CID_NTT_HEADER           0x07   /* 0b 000 0111 */
#define CID_NTT_STX              0x02   /* 0b 000 0010 */
#define CID_NTT_TYPE1            0x40   /* 0b 100 0000 */
#define CID_NTT_TYPE2            0x41   /* 0b 100 0001 */
#define CID_NTT_ETX              0x03   /* 0b 000 0011 */
/*#define CID_NTT_SI             0x0F*/ /* 0b 000 1111 */
/*#define CID_NTT_SO             0x0E*/ /* 0b 000 1110 */

/* ============================= */
/* Local macro declaration       */
/* ============================= */

/* check value and limit against a maximum */
#define LIMIT_VAL(val,maxval) \
   do                         \
   {                          \
      if ((val) > (maxval))   \
         (val) = (maxval);    \
   } while(0)

#define MIN_TIMER_VAL   10

/* ============================= */
/* Local enum declaration        */
/* ============================= */

/** Status of state machines for upper layers. */
typedef enum
{
   /** state machine not ready */
   E_FSM_CONTINUE,
   /** abort with error */
   E_FSM_ERROR,
   /** state machine finished */
   E_FSM_COMPLETE
} FSM_STATUS_t;

/* ============================= */
/* Local functions declaration   */
/* ============================= */

/* DTMF (ETSI) */
IFX_LOCAL IFX_void_t   ciddtmf_abscli_etsi (IFX_TAPI_CID_ABS_REASON_t *pAbs);
IFX_LOCAL IFX_uint32_t ciddtmf_codeabscli  (IFX_TAPI_CID_ABS_REASON_t const *pAbsence,
                                            IFX_uint32_t nMsgElements,
                                            IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                            IFX_char_t tone,
                                            IFX_uint8_t *pBuf);
IFX_LOCAL IFX_uint32_t ciddtmf_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                         IFX_uint32_t nMsgElements,
                                         IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                         IFX_char_t tone,
                                         IFX_uint8_t *pBuf);
IFX_LOCAL IFX_int32_t  ciddtmf_code    (TAPI_CID_DATA *pCidData,
                                       IFX_TAPI_CID_DTMF_CFG_t const *pCidDtmfConf,
                                       IFX_TAPI_CID_ABS_REASON_t const *pDtmfAbsCli,
                                       IFX_uint32_t nMsgElements,
                                       IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);

/* FSK (ETSI) */
IFX_LOCAL IFX_void_t   cidfsk_codedate (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                       IFX_TAPI_CID_MSG_DATE_t const *date,
                                       IFX_uint8_t *pCidBuf,
                                       IFX_uint8_t *pPos);
IFX_LOCAL IFX_void_t   cidfsk_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                       IFX_uint8_t const *buf,
                                       IFX_uint8_t len,
                                       IFX_uint8_t *pCidBuf,
                                       IFX_uint8_t *pPos);
IFX_LOCAL IFX_uint8_t  cidfsk_calccrc  (IFX_uint8_t const *pCidBuf,
                                       IFX_uint8_t len);
IFX_LOCAL IFX_int32_t  cidfsk_code     (TAPI_CID_DATA *pCidData,
                                       IFX_TAPI_CID_MSG_TYPE_t messageType,
                                       IFX_uint32_t nMsgElements,
                                       IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);
/* FSK (NTT) */
IFX_LOCAL IFX_void_t   cidfskntt_setparity   (IFX_uint8_t *byte);
IFX_LOCAL IFX_void_t   cidfskntt_codetype    (IFX_uint8_t type,
                                             IFX_uint8_t const *buf,
                                             IFX_uint8_t len,
                                             IFX_uint8_t *pCidBuf,
                                             IFX_uint8_t *pPos);
IFX_LOCAL IFX_uint16_t cidfskntt_calccrc     (IFX_uint8_t const *pCidBuf,
                                             IFX_uint8_t len);
IFX_LOCAL IFX_int32_t  cidfskntt_code        (TAPI_CID_DATA *pCidData,
                                             IFX_TAPI_CID_HOOK_MODE_t txMode,
                                             IFX_uint32_t nMsgElements,
                                             IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);
/* CID local functions */
IFX_LOCAL IFX_int32_t cid_alert_predeftone   (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t cid_set_alerttone_time (TAPI_CONNECTION *pChannel,
                                              IFX_TAPI_CID_MSG_t const *pCidInfo);
IFX_LOCAL IFX_int32_t cid_set_reversedline   (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t cid_mutevoice          (TAPI_CONNECTION *pChannel,
                                              IFX_boolean_t bMute);
IFX_LOCAL IFX_void_t cidfsk_set_tx_time      (TAPI_CID_DATA *pCidData,
                                              IFX_TAPI_CID_FSK_CFG_t* pFskConf);
IFX_LOCAL IFX_int32_t cid_lookup_transparent(TAPI_CID_DATA *pCidData,
                                             TAPI_CID_CONF *pCidConf,
                                             IFX_uint32_t   nMsgElements,
                                             IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage);
IFX_LOCAL IFX_int32_t cid_prepare_data       (TAPI_CONNECTION *pChannel,
                                             IFX_TAPI_CID_MSG_t const *pCidInfo);
IFX_LOCAL IFX_int32_t cid_determine_alert_type(TAPI_CONNECTION *pChannel,
                                               IFX_TAPI_CID_MSG_t const *pCidInfo);
/* State Machine callbacks */
IFX_LOCAL FSM_STATUS_t cid_fsm_alert_exec    (TAPI_CONNECTION *pChannel);
IFX_LOCAL FSM_STATUS_t cid_fsm_ack_exec      (TAPI_CONNECTION *pChannel);
IFX_LOCAL FSM_STATUS_t cid_fsm_sending_exec  (TAPI_CONNECTION *pChannel);
IFX_LOCAL FSM_STATUS_t cid_fsm_sending_complete_exec(TAPI_CONNECTION *pChannel);
IFX_LOCAL FSM_STATUS_t cid_fsm_exec          (TAPI_CONNECTION *pChannel);


/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* default settings for cid timing configuration */
static const IFX_TAPI_CID_TIMING_t default_cid_timing =
{
   300,  /* beforeData */
   300,  /* dataOut2restoreTimeOnhook */
   60,   /* dataOut2restoreTimeOffhook */
   100,   /* ack2dataOutTime */
   160,  /* cas2ackTime */
   50,    /* afterAckTimeout */
   600,  /* afterFirstRing */
   500,  /* afterRingPulse */
   45,   /* afterDTASOnhook */
   100,  /* afterLineReversal */
   300,  /* afterOSI */
};

/* default settings for cid fsk configuration */
static const IFX_TAPI_CID_FSK_CFG_t default_cid_fsk_conf =
{
   -140, /* levelTX */
   -150, /* levelRX */
   300,  /* seizureTX */
   300,  /* seizureRX */
   180,  /* markTXOnhook */
   80,   /* markTXOffhook */
   180,  /* markRXOnhook */
   80    /* markRXOffhook */
};

/* default settings for cid dtmf configuration */
static const IFX_TAPI_CID_DTMF_CFG_t default_cid_dtmf_conf =
{
   'A',  /* startTone */
   'C',  /* stopTone */
   'B',  /* infoStartTone */
   'D',  /* redirStartTone */
   50,   /* digitTime */
   50    /* interDigitTime */
};

/* ============================= */
/* Local function definition     */
/* ============================= */

/**
   Sets ETSI parameters as default for ABSCLI reasons
   "unavailable" and "priv".
\param
   pAbs    - handle to IFX_TAPI_CID_ABS_REASON_t structure, must be valid.
\remarks
   This function is called at CID configuration for the standard
   IFX_TAPI_CID_STD_ETSI_DTMF only.
*/
IFX_LOCAL IFX_void_t ciddtmf_abscli_etsi (IFX_TAPI_CID_ABS_REASON_t *pAbs)
{
   if (pAbs == IFX_NULL)
      return;
   memset (pAbs->priv, 0, sizeof(IFX_TAPI_CID_MSG_LEN_MAX));
   memset (pAbs->unavailable, 0, sizeof(IFX_TAPI_CID_MSG_LEN_MAX));
   /* ETSI DTMF : */
   pAbs->len = 2;
   /* ETSI DTMF : unavailable = "00" */
   pAbs->unavailable [0] = '0';
   pAbs->unavailable [1] = '0';
   /* ETSI DTMF : priv = "01" */
   pAbs->priv [0] = '0';
   pAbs->priv [1] = '1';
}

/**
   Codes data of IFX_TAPI_CID_ST_ABSCLI type in CID buffer for DTMF
\param
   pAbsence       - handle to DTMF absence data coded at configuration time.
                    (Default is ETSI)
\param
   nMsgElements   - number of message elements
\param
   pMessage       - pointer to message elements
\param
   tone           - signalling tone for element
\param
   pBuf           - start position in cid data buffer
\return
   length of encoded element or 0 on error / missing element
*/
IFX_LOCAL IFX_uint32_t ciddtmf_codeabscli (IFX_TAPI_CID_ABS_REASON_t const *pAbsence,
                                           IFX_uint32_t nMsgElements,
                                           IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                           IFX_char_t tone,
                                           IFX_uint8_t *pBuf)
{
   IFX_int32_t             ret = IFX_ERROR;
   IFX_TAPI_CID_MSG_ELEMENT_t  const *pMsg;
   IFX_uint8_t             tonecode = 0;
   IFX_uint8_t             i, pos = 0,
                           *reason = IFX_NULL;

   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->value.elementType == IFX_TAPI_CID_ST_ABSCLI)
      {
         ret = IFX_SUCCESS;
         switch (pMsg->value.element)
         {
            case  IFX_TAPI_CID_ABSREASON_UNAV:
               reason = (IFX_uint8_t *)pAbsence->unavailable;
               break;
            case  IFX_TAPI_CID_ABSREASON_PRIV:
               reason = (IFX_uint8_t *)pAbsence->priv;
               break;
            default:
               ret = IFX_ERROR;
               break;
         }
         if (ret == IFX_ERROR)
            break;
         /* set element signalling tone */
         ret = TAPI_LL_Phone_Dtmf_GetCode (tone, &tonecode);
         if (ret == IFX_ERROR)
            break;
         pBuf[pos++] = tonecode;
         /* Copy to internal CID buffer */
         for (i = 0; i < pAbsence->len; i++)
         {
            /* ptr reason must not be NULL. */
            IFXOS_ASSERT(reason != IFX_NULL); 
            ret = TAPI_LL_Phone_Dtmf_GetCode (reason[i], &tonecode);
            if (ret == IFX_ERROR)
               break;
            pBuf [pos++] = tonecode;
         }
         /* leave loop after first occurrence,
            only one (the first) element of a type is used */
         break;
      }
   }
   if (ret == IFX_ERROR)
      return 0;
   else
      return pos;
}

/**
   Codes data of indicated type in CID buffer for DTMF
\param
   type           - service type
\param
   nMsgElements   - number of message elements
\param
   pMessage       - pointer to message elements
\param
   tone           - signalling tone for element
\param
   pBuf           - start position in cid data buffer
\return
   length of encoded element or 0 on error / missing element
*/
IFX_LOCAL IFX_uint32_t ciddtmf_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                         IFX_uint32_t nMsgElements,
                                         IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage,
                                         IFX_char_t tone,
                                         IFX_uint8_t *pBuf)
{
   IFX_int32_t             ret = IFX_ERROR;
   IFX_TAPI_CID_MSG_ELEMENT_t  const *pMsg;
   IFX_uint32_t            len;
   IFX_uint8_t              tonecode = 0;
   IFX_uint8_t             i, pos = 0;

   
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == type)
      {
	  
         len = pMsg->string.len;
         LIMIT_VAL(len,CID_ST_NUM_LEN);
         /* set element signalling tone */
         ret = TAPI_LL_Phone_Dtmf_GetCode (tone, &tonecode);
         if (ret == IFX_ERROR)
            break;
         pBuf[pos++] = tonecode;
         /* Copy to internal CID buffer */
         for (i = 0; i < len; i++)
         {
            ret = TAPI_LL_Phone_Dtmf_GetCode (pMsg->string.element[i], &tonecode);
            if (ret == IFX_ERROR)
               break;
            pBuf [pos++] = tonecode;
         }
         /* leave loop after first occurrence,
            only one (the first) element of a type is used */
         break;
      }
   }
   if (ret == IFX_ERROR)
      return 0;
   else
      return pos;
}

/**
   Code CID parameters for DTMF transmission.
\param
   pCidData       - handle to TAPI_CID_DATA structure
\param
   pCidDtmfConf   - handle to IFX_TAPI_CID_DTMF_CFG_t structure
\param
   nMsgElements   - number of message elements to add
\param
   pMessage       - pointer to message elements
\return
   IFX_SUCCESS / IFX_ERROR
*/
IFX_LOCAL IFX_int32_t ciddtmf_code (TAPI_CID_DATA *pCidData,
                                    IFX_TAPI_CID_DTMF_CFG_t const *pCidDtmfConf,
                                    IFX_TAPI_CID_ABS_REASON_t const *pDtmfAbsCli,
                                    IFX_uint32_t nMsgElements,
                                    IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   IFX_int32_t          ret      = IFX_SUCCESS;
   IFX_uint8_t          tone     = 0,
                        *pCidBuf = pCidData->cidParam;
   IFX_uint8_t          *pPos    = &pCidData->nCidParamLen;
   IFX_uint32_t         len;
   IFX_boolean_t        bInfo    = IFX_FALSE;

   /* initialize pos */
   *pPos = 0;

   /* encode CLI data.
      Note : CLI or ABSCLI are mandatory */
   len = ciddtmf_codetype(IFX_TAPI_CID_ST_CLI, nMsgElements, pMessage,
                          pCidDtmfConf->startTone, &pCidBuf [*pPos]);
   /* No CLI data, encode ABSCLI data.
      Note : CLI or ABSCLI are mandatory  */
   if (len == 0)
   {
      /* ABSCLI is sent as Information message. No further information message
         should be code in the buffer. */
      bInfo = IFX_TRUE;
      len = ciddtmf_codeabscli (pDtmfAbsCli, nMsgElements, pMessage,
                                pCidDtmfConf->infoStartTone, &pCidBuf [*pPos]);
      /* No CLI, no ABSCLI : error, at least one of both must be present */
      if (len == 0)
      {
         TRACE (TAPI_DRV,DBG_LEVEL_NORMAL,
            ("\n\rDRV_ERROR: No valid data for DTMF CID, "
               "mandatory element CLI or ABSCLI missing!\r\n"));
	 printk("\n\rDRV_ERROR: No valid data for DTMF CID, "
				                "mandatory element CLI or ABSCLI missing!\r\n");
         return IFX_ERROR;
      }
   }
   *pPos += len;
   /* encode Redirection data */
   len = ciddtmf_codetype(IFX_TAPI_CID_ST_REDIR, nMsgElements, pMessage,
                          pCidDtmfConf->redirStartTone, &pCidBuf [*pPos]);
   if (len != 0)
   {
      *pPos += len;
   }

   /* encode Information data (in DISP element).
      Note : This will be done only if there is not information values in the
             yet coded DTMF string (e.g. ABSCLI information) */
   if (bInfo == IFX_FALSE)
   {
      len = ciddtmf_codetype(IFX_TAPI_CID_ST_DISP, nMsgElements, pMessage,
                             pCidDtmfConf->infoStartTone, &pCidBuf [*pPos]);
      if (len != 0)
      {
         *pPos += len;
      }
   }
   ret = TAPI_LL_Phone_Dtmf_GetCode (pCidDtmfConf->stopTone, &tone);
   if (ret != IFX_ERROR)
   {
      /* set stop tone */
      pCidBuf [(*pPos)++] = tone;
   }

   /* set transmission time now */
   if (ret == IFX_SUCCESS)
   {
      pCidData->nTxTime = ((pCidDtmfConf->digitTime +
                            pCidDtmfConf->interDigitTime) *
                            pCidData->nCidParamLen);
   }
   else
   {
      TRACE (TAPI_DRV,DBG_LEVEL_NORMAL,
         ("\n\rDRV_ERROR: No valid data for DTMF CID\r\n"));
      printk("\n\rDRV_ERROR: No valid data for DTMF CID\r\n");
   }

   return ret;
}

/**
   Codes date in CID buffer.
   No check is done for valid values of date element.
\param
   type     - type
\param
   date     - pointer to date message element
\param
   pCidBuf  - cid data buffer
\param
   pPos     - current data position in cid buffer
\return
   None
Remark:
   ref ETSI EN 300 659-1 V.1.3.1
*/
IFX_LOCAL IFX_void_t cidfsk_codedate (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                      IFX_TAPI_CID_MSG_DATE_t const *date,
                                      IFX_uint8_t *pCidBuf,
                                      IFX_uint8_t *pPos)
{
   /* set date and time parameter type : 0x01 or 0x0F */
   pCidBuf [(*pPos)++] = type;
   /* set date and time parameter length : 8 */
   pCidBuf [(*pPos)++] = CIDFSK_DATE_LEN;
   /* set month most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->month / 10) + '0';
   /* set month least significant digit */
   pCidBuf [(*pPos)++] = (date->month % 10) + '0';
   /* set day most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->day / 10) + '0';
   /* set day least significant digit */
   pCidBuf [(*pPos)++] = (date->day % 10) + '0';
   /* set hour most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->hour / 10) + '0';
   /* set hour least significant digit */
   pCidBuf [(*pPos)++] = (date->hour % 10) + '0';
   /* set minute most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)date->mn / 10) + '0';
   /* set minute least significant digit */
   pCidBuf [(*pPos)++] = (date->mn % 10) + '0';
}

/**
   Codes data of indicated type in CID buffer
\param
   type     - service type
\param
   buf      - buffer with information (octets)
\param
   len      - buffer size in octets (=bytes)
\param
   pCidBuf  - cid data buffer
\param
   pPos     - current data position in cid buffer
\return
   None
*/
IFX_LOCAL IFX_void_t cidfsk_codetype (IFX_TAPI_CID_SERVICE_TYPE_t type,
                                      IFX_uint8_t const *buf,
                                      IFX_uint8_t len,
                                      IFX_uint8_t *pCidBuf,
                                      IFX_uint8_t *pPos)
{
   IFX_uint8_t i;

   /* set service type */
   pCidBuf[(*pPos)++] = type;
   /* set element length  */
   pCidBuf[(*pPos)++] = len;
   /* copy element content */
   for (i = 0; i < len; i++)
   {
      pCidBuf[(*pPos)++] = buf[i];
   }
}

/**
   Calculate checksum of given CID buffer
\param
   pCidBuf  - CID buffer of which checksum will be calculated
\param
   len      - len of FSK buffer
\return
   IFX_SUCCESS / IFX_ERROR
Remark:
   ref ETSI EN 300 659-1 V.1.3.1
   checksum calculation : twos complement of the modulo 256 sum
   of all the octets in the message starting from the Message type octet
   up to the end of message (excluding checksum it self)
*/
IFX_LOCAL IFX_uint8_t cidfsk_calccrc (IFX_uint8_t const *pCidBuf,
                                      IFX_uint8_t len)
{
   IFX_uint32_t sumCID = 0;
   IFX_uint8_t  i, crc;

   /* sum all octets */
   for (i = 0; i < len; i++)
   {
      sumCID += pCidBuf [i];
   }
   /* calculate twos complement as crc value */
   crc = ((~(sumCID & 0xFF)) + 1);

   return crc;
}

/**
   Set the FSK transmission time according to mark bits, seizure bits
   and data size in characters (bytes) to transmit.
\param
   pCidData  - handle to TAPI_CID_DATA structure
\param
   pFskConf  - handle to IFX_TAPI_CID_FSK_CFG_t structure
Remark:
   CID baudrate = 150 Bytes/sec = 1200 bits/sec without Pause.
*/
IFX_LOCAL IFX_void_t cidfsk_set_tx_time (TAPI_CID_DATA *pCidData,
                                         IFX_TAPI_CID_FSK_CFG_t* pFskConf)
{
   IFX_uint32_t bits_num;

   /* calculate the number of bits to transmit, assuming each FSK data byte
      is encoded into 10 bits by DSP before sending :
      bits_num   = mark_bits +  seizure_bits + (data_bytes_num * 10)
   */
   bits_num = pFskConf->seizureTX + (pCidData->nCidParamLen * CID_FSK_BYTE_BITSNUM);
   if (pCidData->txHookMode == IFX_TAPI_CID_HM_ONHOOK)
      bits_num += pFskConf->markTXOnhook;
   else
      bits_num += pFskConf->markTXOffhook;
   /* Calculate the FSK transmission time in ms, assuming a tolerance of 5 ms for
      cid request time and software processing  :
      tx_time_ms   = ((tx_bits_num * 1000) / tx_rate) + tx_tolerance
   */
   pCidData->nTxTime = ((bits_num * 1000) / CID_FSK_BITRATE_SEC) +
                        CID_FSK_TOLERANCE_MS;
}

/**
   Code CID parameters for FSK transmission according to chosen services.
   (ref: Spec ETSI EN 300 659-3.)
\param
   pCidData       - handle to TAPI_CID_DATA structure
\param
   messageType    - defines the message type (call setup, message waiting,...)
\param
   nMsgElements   - number of message elements to add
\param
   pMessage       - pointer to message elements
\return
   IFX_SUCCESS / IFX_ERROR
Remark:
   CID parameters buffer should be filled according to services.
   specification used : ETSI EN 300 659-3 V1.3.1 (2001-01)
*/
IFX_LOCAL IFX_int32_t cidfsk_code(TAPI_CID_DATA *pCidData,
                                    IFX_TAPI_CID_MSG_TYPE_t messageType,
                                    IFX_uint32_t nMsgElements,
                                    IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   IFX_int32_t    ret = IFX_SUCCESS;
   IFX_boolean_t  mandatory_found = IFX_FALSE;
   IFX_uint8_t    *pCidBuf = pCidData->cidParam,
                  *pPos    = &pCidData->nCidParamLen, crc;
   IFX_uint32_t   i;

   for (i=0; i<nMsgElements; i++)
   {
      switch ((pMessage+i)->string.elementType)
      {
      case IFX_TAPI_CID_ST_CLI:
      case IFX_TAPI_CID_ST_ABSCLI:
         if (messageType==IFX_TAPI_CID_MT_CSUP)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_VISINDIC:
         if (messageType==IFX_TAPI_CID_MT_MWI)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_CHARGE:
         if (messageType==IFX_TAPI_CID_MT_AOC)
            mandatory_found = IFX_TRUE;
         break;
      case IFX_TAPI_CID_ST_DISP:
         if (messageType==IFX_TAPI_CID_MT_SMS)
            mandatory_found = IFX_TRUE;
         break;
      default:
         break;
      }
      if (mandatory_found == IFX_TRUE)
         break;
   } /* for () */

   if (mandatory_found != IFX_TRUE)
   {
      return IFX_ERROR;
   }

   /* message type */
   pCidBuf [0] = messageType;
   /* start filling data at position 2 of cid buffer */
   *pPos = 2;

   for (i=0; i < nMsgElements; i++)
   {
      IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg = pMessage + i;
      IFX_TAPI_CID_SERVICE_TYPE_t type = pMsg->string.elementType;
      IFX_uint32_t len = pMsg->string.len;

      switch (type)
      {
      /* Elements with special coding (date, length 8) */
      case IFX_TAPI_CID_ST_DATE:
      case IFX_TAPI_CID_ST_CDATE:
         cidfsk_codedate ( type, &pMsg->date, pCidBuf, pPos);
         break;
      /* Elements with length 1 */
      case IFX_TAPI_CID_ST_ABSCLI:
      case IFX_TAPI_CID_ST_ABSNAME:
      case IFX_TAPI_CID_ST_VISINDIC:
      case IFX_TAPI_CID_ST_CT:
      case IFX_TAPI_CID_ST_MSGNR:
      case IFX_TAPI_CID_ST_FWCT:
      case IFX_TAPI_CID_ST_USRT:
      case IFX_TAPI_CID_ST_SINFO:
         cidfsk_codetype (type, (IFX_uint8_t*)&pMsg->value.element,
                          1, pCidBuf, pPos);
         break;
      /* Elements with variable size,
         do length limitation depending on type */
      case IFX_TAPI_CID_ST_MSGIDENT:
         LIMIT_VAL(len,CID_ST_MSGIDENT_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_DURATION:
         LIMIT_VAL(len,CID_ST_DURATION_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_XOPUSE:
         LIMIT_VAL(len,CID_ST_XOPUSE_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_CHARGE:
      case IFX_TAPI_CID_ST_ACHARGE:
         LIMIT_VAL(len,CID_ST_CHARGE_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_CLI:
      case IFX_TAPI_CID_ST_CDLI:
      case IFX_TAPI_CID_ST_LMSGCLI:
      case IFX_TAPI_CID_ST_CCLI:
      case IFX_TAPI_CID_ST_FIRSTCLI:
      case IFX_TAPI_CID_ST_REDIR:
      case IFX_TAPI_CID_ST_NTID:
      case IFX_TAPI_CID_ST_CARID:
         LIMIT_VAL(len,CID_ST_NUM_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_TERMSEL:
         LIMIT_VAL(len,CID_ST_TERMSEL_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_NAME:
         LIMIT_VAL(len,CID_ST_NAME_LEN);
         goto code_string;
      case IFX_TAPI_CID_ST_DISP:
      default:
code_string:
         LIMIT_VAL(len,CID_ST_DISP_LEN);
         cidfsk_codetype (type, pMsg->string.element, len, pCidBuf, pPos);
         break;
      }
   }
   if (ret == IFX_SUCCESS)
   {
      /* set message length without first 2 bytes */
      pCidBuf [1] = *pPos - 2;
      /* set crc */
      crc = cidfsk_calccrc (pCidBuf, *pPos);
      pCidBuf [(*pPos)++] = crc;
   }

   return ret;
}

/* ==================================== */
/* Local functions definition CID NTT */
/* ==================================== */

/**
   Codes data of indicated type in CID NTT buffer
\param
   type     - parameter type
\param
   buf      - buffer with information data (number or name)
\param
   len      - buffer size in octets
\param
   pCidBuf  - cid data buffer
\param
   pPos     - current data position in cid buffer
\return
   None
*/
IFX_LOCAL IFX_void_t cidfskntt_codetype (IFX_uint8_t type, IFX_uint8_t const *buf,
                                         IFX_uint8_t len, IFX_uint8_t *pCidBuf,
                                         IFX_uint8_t *pPos)
{
   IFX_uint8_t i;

   /* Parameter type */
   pCidBuf[(*pPos)++] = type;
   /* when data [001 0000] is outputted, DLE is added to the head
      of the data before output and is part of CRC calculation */
   if (len == CID_NTT_DLE)
   {
      pCidBuf[(*pPos)++] = CID_NTT_DLE;
      /* add this to message content length */
      pCidBuf[CID_NTT_MSGLEN_POS] += 1;
   }
   /* Data content length */
   pCidBuf[(*pPos)++] = len;
   /* Data content */
   for (i = 0; i < len; i++)
   {
      pCidBuf[(*pPos)++] = buf[i];
   }
   /* Message content length */
   pCidBuf [CID_NTT_MSGLEN_POS] += (2 + len);
}

/**
   Add parity bit if applicable.
\param
   byte  - pointer to byte to be updated
\return
   none
\remarks
   1- The byte to format is assumed to be represented in format <b7b6b5b4b3b2b1>.
      After transformation, it will be <bpb7b6b5b4b3b2b1>.
   2- refer to NTT Specification, Edition 5
*/
IFX_LOCAL IFX_void_t cidfskntt_setparity (IFX_uint8_t *byte)
{
   IFX_uint8_t i, ones_num = 0, tmpByte = *byte;

   /* check even parity for b7...b1 */
   for (i = 0; i < 7; i++)
   {
      /* count number of ones */
      if (tmpByte & 0x1)
         ones_num ++;
      tmpByte >>= 1;
   }
   /* set parity bit as MSB in case of odd num (lowest bit it set) */
   if (ones_num & 0x1)
      *byte |= 0x80;
}

/**
   Calculate CTC for NTT FSK encoding.
\param
   pCidBuf  - cid data buffer
\param
   len      - buffer size in octets
\return
   calculated CRC
\remarks
   1- The CRC16 for NTT FSK Buffer is generated with the polynom
      X^16 + X^12 + X^5 + X^2 + X^0 (Hexadecimal: 0x8408)
   2- The algorithm is:
      a)initial value of CRC = 0x0000.
      b)for each byte (from header to ETX)
            compute the CRC with G(x) (CRC1)
            CRC= CRC + CRC1
*/
IFX_LOCAL IFX_uint16_t cidfskntt_calccrc (IFX_uint8_t const *pCidBuf,
                                          IFX_uint8_t len)
{
   IFX_uint16_t crc = 0, tmp;
   IFX_uint8_t i, j, byte;

   for (i = 0; i < len; i++)
   {
      /* get reversed data byte */
      byte = pCidBuf [i];
      tmp   = (byte << 1);
      for (j = 8; j > 0; j--)
      {
         tmp  >>= 1;
         if ((tmp ^ crc) & 0x0001)
            crc = (crc >> 1) ^ 0x8408;
         else
            crc >>= 1;
      }
   }
   return crc;
}


/**
   NTT FSK encoding.
\param
   pCidData       - handle to TAPI_CID_DATA  structure
\param
   txMode         - tx mode (on - or off hook)
\param
   nMsgElements   - number of message elements to add
\param
   pMessage       - pointer to message elements
\return
   IFX_SUCCESS or IFX_ERROR
Remark
   - This specification complies with NTT standards
   - The structure of the frame is as follows:
   <DLE>
   <SOH>
   <header>
   <DLE>
   <STX>
   <type of message (0x40 for type 1, 0x41 for type 2, without their parity bit)>
   <length of message>
   <type of DATA>
   <length of DATA>
   <DATA>
   <DLE>
   <ETX>
   <CRC High reversed>
   <CRC Low  reversed>
*/
IFX_LOCAL IFX_int32_t  cidfskntt_code (TAPI_CID_DATA *pCidData,
                                       IFX_TAPI_CID_HOOK_MODE_t txMode,
                                       IFX_uint32_t nMsgElements,
                                       IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   static const IFX_uint8_t
      pNttHead [7] = {CID_NTT_DLE, CID_NTT_SOH, CID_NTT_HEADER,
                      CID_NTT_DLE, CID_NTT_STX, CID_NTT_TYPE1, 0/*len*/ },
      pNttTail [2] = {CID_NTT_DLE, CID_NTT_ETX};
   IFX_uint16_t   crc;
   IFX_uint8_t    *pCidBuf = pCidData->cidParam,
                  *pPos    = &pCidData->nCidParamLen, i;
   IFX_boolean_t  mandatory_found = IFX_FALSE;
   IFX_TAPI_CID_MSG_ELEMENT_t const *pMsg;

   /* set head according to CID type */
   *pPos  = sizeof (pNttHead);
   memcpy (pCidBuf, pNttHead, sizeof (pNttHead));
   /* in case of type 2, overwrite type parameter */
   if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
   {
      pCidBuf [CID_NTT_TYPE_POS] = CID_NTT_TYPE2;
   }

   /* search for CLI data */
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == IFX_TAPI_CID_ST_CLI)
      {
         IFX_uint32_t len = pMsg->string.len;
         LIMIT_VAL(len,CID_NTT_NUMBER_MAXLEN);
         cidfskntt_codetype (CID_NTT_PT_NUMBER, pMsg->string.element,
                             len, pCidBuf, pPos);
         mandatory_found = IFX_TRUE;
         break;
      }
   }

   /* search for NAME data */
   for (pMsg = pMessage; pMsg < pMessage + nMsgElements; pMsg++)
   {
      if (pMsg->string.elementType == IFX_TAPI_CID_ST_NAME)
      {
         IFX_uint32_t len = pMsg->string.len;
         LIMIT_VAL(len,CID_NTT_NAME_MAXLEN);
         cidfskntt_codetype (CID_NTT_PT_NAME, pMsg->string.element,
                             len, pCidBuf, pPos);
         mandatory_found = IFX_TRUE;
         break;
      }
   }
   if (mandatory_found != IFX_TRUE)
   {
      TRACE (TAPI_DRV,DBG_LEVEL_NORMAL,
         ("\n\rDRV_ERROR: No valid data for NTT CID, "
            "mandatory elements missing!\r\n"));
      return IFX_ERROR;
   }

   /* set tail */
   memcpy (&pCidBuf[*pPos], pNttTail, sizeof (pNttTail));
   *pPos  += sizeof (pNttTail);
   /* set bytes parity */
   for (i = 0; i < *pPos; i++)
   {
      cidfskntt_setparity (&pCidBuf [i]);
   }
   /* calculate crc from HEADER to ETX */
   crc = cidfskntt_calccrc (&pCidBuf[2], (*pPos - 2));

   /* add CRC to buffer */
   pCidBuf [(*pPos)++] = (crc & 0xFF);
   pCidBuf [(*pPos)++] = ((crc >> 8) & 0xFF);

   return IFX_SUCCESS;
}

/**
   Looks up if there is CID transparent data to transmit and copies
   this data into internal buffer.
\param
   pChannel    - handle to TAPI_CONNECTION structure
\param
   pCidConf    - handle to TAPI_CID_CONF structure
\param
   nMsgElements - number of message elements
\param
   pMessage     - handle to  IFX_TAPI_CID_MSG_ELEMENT_t structure
\return
   IFX_SUCCESS / IFX_ERROR
\remark:
   An error is returned in following cases:
   -  a part from transparent data, there are other CID elements
   -  no memory could be allocated for the user data.
*/
IFX_LOCAL IFX_int32_t cid_lookup_transparent(TAPI_CID_DATA *pCidData,
                                             TAPI_CID_CONF *pCidConf,
                                             IFX_uint32_t   nMsgElements,
                                             IFX_TAPI_CID_MSG_ELEMENT_t const *pMessage)
{
   IFX_int32_t    ret = IFX_SUCCESS;
   IFX_boolean_t  transparent_found = IFX_FALSE;
   IFX_uint32_t   i;

   for (i=0; i < nMsgElements; i++)
   {
      if (pMessage[i].transparent.elementType == IFX_TAPI_CID_ST_TRANSPARENT)
      {
         /* transparent message must be first in the queue, and complete because
            no encoding is done. Size must also match the internal buffer size,
            otherwise error */
         if ((i == 0) && (pMessage[i].transparent.len < IFX_TAPI_CID_SIZE_MAX))
         {
            transparent_found = IFX_TRUE;
         }
         else
         {
            /* wrong size or message elements mismatch: Transparent together
               with other elements types.*/
            ret = IFX_ERROR;
            TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Transparent data"
                   " mixed together with other elements types!\r\n"));
         }
      }
   }
   if (transparent_found == IFX_TRUE)
   {
      IFX_uint8_t *pTrans = TAPI_MapBuffer(pMessage[0].transparent.data,
                                           pMessage[0].transparent.len);
      if (pTrans == IFX_NULL)
      {
         ret = IFX_ERROR;
         TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: No memory for"
                " user transparent data !\r\n"));
      }
      else
      {
         memcpy (pCidData->cidParam, pMessage[0].transparent.data,
                 pMessage[0].transparent.len);
         pCidData->nCidParamLen = pMessage[0].transparent.len;
         if (pCidConf->nStandard == IFX_TAPI_CID_STD_ETSI_DTMF)
         {
            pCidData->nTxTime = ((pCidConf->TapiCidDtmfConf.digitTime +
                                  pCidConf->TapiCidDtmfConf.interDigitTime) *
                                  pCidData->nCidParamLen);
         }
         else
         {
           cidfsk_set_tx_time (pCidData, &pCidConf->TapiCidFskConf);
         }
         TAPI_UnmapBuffer(pTrans);
      }
   }

   return ret;
}

/**
   Prepare the CID data buffer.
\param
   pChannel - handle to TAPI_CONNECTION structure, data channel
\param
   pCidInfo - contains the caller id settings (IFX_TAPI_CID_MSG_t)
\return
   IFX_SUCCESS / IFX_ERROR
Remark:
   - operation is done on a data channel.
   - if an error is returned the CID buffer contains invalid data!
*/
IFX_LOCAL IFX_int32_t cid_prepare_data(TAPI_CONNECTION *pChannel,
                                       IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_int32_t                  ret = IFX_SUCCESS;
   IFX_TAPI_CID_MSG_ELEMENT_t  *pMessage = IFX_NULL;

   if (pCidInfo->nMsgElements == 0)
      return IFX_ERROR;

   pMessage = TAPI_MapBuffer(pCidInfo->message,
      pCidInfo->nMsgElements * sizeof(IFX_TAPI_CID_MSG_ELEMENT_t));

   if (pMessage == IFX_NULL)
      return IFX_ERROR;

   /* reset internal CID data structure */
   memset (pChannel->TapiCidTx.cidParam, 0, IFX_TAPI_CID_SIZE_MAX);
   pChannel->TapiCidTx.nCidParamLen = 0;

   ret = cid_lookup_transparent(&pChannel->TapiCidTx, &pChannel->TapiCidConf,
                                pCidInfo->nMsgElements, pMessage);
   /* In case there is no transparent CID data, look for CID message elements
      and code them accordingly */
   if ((ret == IFX_SUCCESS) && (pChannel->TapiCidTx.nCidParamLen == 0))
   {
      switch (pChannel->TapiCidConf.nStandard)
      {
      case IFX_TAPI_CID_STD_TELCORDIA:
      case IFX_TAPI_CID_STD_ETSI_FSK:
      case IFX_TAPI_CID_STD_SIN:
         /* Code CID according to standard (V.23 or Bellcore) */
         ret = cidfsk_code(&pChannel->TapiCidTx, pCidInfo->messageType,
                           pCidInfo->nMsgElements, pMessage);
         if (ret == IFX_SUCCESS)
            cidfsk_set_tx_time (&pChannel->TapiCidTx,
                                &pChannel->TapiCidConf.TapiCidFskConf);
         break;
      case IFX_TAPI_CID_STD_ETSI_DTMF:
         /* Code CID for DTMF */
         ret = ciddtmf_code(&pChannel->TapiCidTx,
                            &pChannel->TapiCidConf.TapiCidDtmfConf,
                            &pChannel->TapiCidConf.TapiCidDtmfAbsCli,
                            pCidInfo->nMsgElements, pMessage);
         break;
      case IFX_TAPI_CID_STD_NTT:
         /* Code CID for NTT (V.23)*/
         ret = cidfskntt_code(&pChannel->TapiCidTx, pCidInfo->txMode,
                              pCidInfo->nMsgElements, pMessage);
         if (ret == IFX_SUCCESS)
            cidfsk_set_tx_time (&pChannel->TapiCidTx,
                                &pChannel->TapiCidConf.TapiCidFskConf);
         break;
      }
   }

   if (ret == IFX_SUCCESS)
      pChannel->TapiCidTx.txHookMode = pCidInfo->txMode;

   TAPI_UnmapBuffer(pMessage);
   return ret;
}

/**
   Determine alert type from configuration and current type of cid to send.
\param
   pChannel - handle to TAPI_CONNECTION structure, data channel
\param
   pCidInfo - contains the caller id settings (IFX_TAPI_CID_MSG_t)
\return
   always IFX_SUCCESS
Remark:
   - operation is done on a data channel.
   - see tables in TAPI specification / documentation
*/
IFX_LOCAL IFX_int32_t cid_determine_alert_type(TAPI_CONNECTION *pChannel,
                                               IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   TAPI_CID_CONF            *pCidConf    = &pChannel->TapiCidConf;
   IFX_TAPI_CID_TIMING_t    *pCidTiming  = &pCidConf->TapiCidTiming;
   TAPI_CID_DATA            *pCidData    = &pChannel->TapiCidTx;
   IFX_TAPI_CID_MSG_TYPE_t   messageType = pCidInfo->messageType;
   IFX_TAPI_CID_HOOK_MODE_t  txMode      = pCidInfo->txMode;
   TAPI_CID_ALERT_TYPE       nAlertType  = TAPI_CID_ALERT_NONE;
   IFX_int32_t               ret         = IFX_SUCCESS;


   if ((txMode == IFX_TAPI_CID_HM_ONHOOK) && (messageType == IFX_TAPI_CID_MT_CSUP))
      pCidData->bRingStart = IFX_TRUE;

   switch (pCidConf->nStandard)
   {
   case IFX_TAPI_CID_STD_TELCORDIA:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_CAS;
      else
      {
         if (messageType == IFX_TAPI_CID_MT_CSUP)
            nAlertType = TAPI_CID_ALERT_FR;
         else
            nAlertType = TAPI_CID_ALERT_OSI;
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_FSK:
   case IFX_TAPI_CID_STD_ETSI_DTMF:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_DTAS;
      else
      {
         IFX_TAPI_CID_ALERT_ETSI_t etsi_alert;
         /* get the right ETSI alert (ringing or not!) */
         if (messageType == IFX_TAPI_CID_MT_CSUP)
            etsi_alert = pCidConf->nETSIAlertRing;
         else
            etsi_alert = pCidConf->nETSIAlertNoRing;
         /* map the ETSI-only enum to internal overall-enum */
         switch (etsi_alert)
         {
         case IFX_TAPI_CID_ALERT_ETSI_FR:
            {
               IFX_uint32_t nTimeCidTxSeq;
               /* calculate complete sequence time */
               nTimeCidTxSeq = pCidTiming->afterFirstRing +
                               pCidData->nTxTime +
                               pCidTiming->dataOut2restoreTimeOnhook;
               /* check if ring cadence pause is enough for cid sequence */
               if (pCidConf->cadenceRingPause >= nTimeCidTxSeq)
               {
                  /* Increase the _individual_ CID transmission time (nTxTime
                     for this CID transmission) in order to ensure the proper
                     ring pause timing.

                     +--1--+----2----+----3----+--------X------+
                     |     |         |         |               |
                     +-------pCidConf->cadenceRingPause--------+
                     |     |                         |         |
                     +--1--+---- 2' -----------------+----3----+

                     1)  pCidTiming->afterFirstRing
                     2)  pCidData->nTxTime
                     3)  pCidTiming->dataOut2restoreTimeOnhook
                     X)  pCidConf->cadenceRingPause - 1 - 2 -3

                     so the effective cid transmission time will
                     be increased to
                     2') pCidData->nTxTime += X
                  */
                  pCidData->nTxTime += (pCidConf->cadenceRingPause
                                        - nTimeCidTxSeq);
                  /* allert type */
                  nAlertType = TAPI_CID_ALERT_FR;
                  break;
               }
               /* configured cadence pause time not sufficient for CID transmission */
               TAPI_Error_Event_Update (pChannel, IFX_TAPI_RT_ERROR_RINGCADENCE_CIDTX);
               TRACE (TAPI_DRV,DBG_LEVEL_HIGH, ("cadence pause not sufficient"
                      " for CID transmission!\r\n"));
	       printk ("cadence pause not sufficient for CID transmission!\r\n");
               ret = IFX_ERROR;
            }
            break;
         case IFX_TAPI_CID_ALERT_ETSI_DTAS:
            nAlertType = TAPI_CID_ALERT_DTAS;
            break;
         case IFX_TAPI_CID_ALERT_ETSI_RP:
            nAlertType = TAPI_CID_ALERT_RP;
            break;
         case IFX_TAPI_CID_ALERT_ETSI_LRDTAS:
            nAlertType = TAPI_CID_ALERT_LR_DTAS;
            break;
         }
      }
      break;
   case IFX_TAPI_CID_STD_SIN:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_DTAS;
      else
         nAlertType = TAPI_CID_ALERT_LR_DTAS;
      break;
   case IFX_TAPI_CID_STD_NTT:
      if (txMode == IFX_TAPI_CID_HM_OFFHOOK)
         nAlertType = TAPI_CID_ALERT_AS_NTT;
      else
         nAlertType = TAPI_CID_ALERT_CAR_NTT;
      break;
   }

   pCidData->nAlertType = nAlertType;

   return ret;
}

/**
   Sets predifined alert tones for following CID alerts :
      - TAPI_CID_ALERT_CAS
      - TAPI_CID_ALERT_DTAS
      - TAPI_CID_ALERT_LR_DTAS
      - TAPI_CID_ALERT_AS_NTT
\param
   pChannel - handle to TAPI_CONNECTION structure, data channel
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a data channel.
   - For Caller ID alert Signal, predifined tones are defined accoding to
     indexes defined in enum _TAPI_CID_ALERTTONE_INDEX_DEFAULT.
     Theses indexes will be used as default for CID according to standard in
     case the user has not defined a suitable tone for his application.
*/
IFX_LOCAL IFX_int32_t cid_alert_predeftone (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t   ret = IFX_SUCCESS;
   COMPLEX_TONE* pToneTable = pChannel->pTapiDevice->pToneTbl;
   IFX_TAPI_TONE_SIMPLE_t a_ptone;

   /* if entry in the table was done already, escape */
   if ((pToneTable[TAPI_CID_ATONE_INDEXDEF_ONHOOK].tone.simple.index != 0)  &&
       (pToneTable[TAPI_CID_ATONE_INDEXDEF_OFFHOOK].tone.simple.index != 0) &&
       (pToneTable[TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT].tone.simple.index != 0)
   )
   {
      return IFX_SUCCESS;
   }
   /* reset tone structure */
   memset (&a_ptone, 0, sizeof (a_ptone));
   /* set parameters for CID alert tone onhook */
   a_ptone.index = TAPI_CID_ATONE_INDEXDEF_ONHOOK;
   a_ptone.frequencies [0] = (IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB);
   /* Frequencies for dual alert tone :
      f1 = 2130 Hz, f2 = 2750 Hz */
   a_ptone.freqA       = 2130;
   a_ptone.freqB       = 2750;
   /* For onhook, alert tone must be played at least 100 ms */
   a_ptone.cadence [0] = 100;
   a_ptone.cadence [1] = 0;
   /* Level for dual alert tone : -18 dB = -180 x 0.1 dB
      Reference : TIA  is  -22dB (dynamic range is -14 dB to -34 dB)
                  ETSI is -18dB */
   a_ptone.levelA      = -180;
   a_ptone.levelB      = -180;
   /* tone played only once */
   a_ptone.loop        = 1;
   ret = TAPI_Phone_Add_SimpleTone (pToneTable, &a_ptone);
   /* set parameters for CID alert tone offhook,
      same as for onhook except timing */
   if (ret == IFX_SUCCESS)
   {
      a_ptone.index = TAPI_CID_ATONE_INDEXDEF_OFFHOOK;
      /* For offhook, alert tone must be played at least 80 ms */
      a_ptone.cadence [0] = 80;
      ret = TAPI_Phone_Add_SimpleTone (pToneTable, &a_ptone);
   }
   /* set parameters for CID alert tone AS NTT, cadence is :
      <f1_1/f2, a ms><no freq, b ms><f1_2/f2, c ms> whereby :
      f1_1 = 852 Hz
      f1_2 = 1633 Hz
      f2   = 941 Hz
      60 ms <= a < 120 ms
      40 ms <= b < 70 ms
      60 ms <= c < 120 ms
   */
   if (ret == IFX_SUCCESS)
   {
      memset (&a_ptone, 0, sizeof (a_ptone));
      a_ptone.index = TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT;
      /* AS NTT tone freqs : f1_1 = 852 Hz, f1_2 = 941 Hz, f2 = 1633 Hz */
      a_ptone.freqA = 852;
      a_ptone.freqB = 1633;
      a_ptone.freqC = 941;
      /* AS NTT tone levels : -15 dB = -150 x 0.1 dB
         Lower frequency  (f1_x) : -15.5 - 0.8 L <= P <= -4.0 - 0.8 L
         Higher frequency (f2)   : -14.5 - L     <= P <= -3.0 - L
         Note : L is the subscriber line load for 1500 Hz. Nominal value 0 - 7 dB
                But there are cases of having more than 7 dB.
      */
      a_ptone.levelA = -150;
      a_ptone.levelB = -150;
      a_ptone.levelC = -150;
      /* AS NTT tone times : a = 100ms, b = 50ms, c = 100 ms */
      a_ptone.cadence [0] = 100;
      a_ptone.cadence [1] = 50;
      a_ptone.cadence [2] = 100;
      a_ptone.loop        = 1;
      /* AS NTT tone cadence */
      a_ptone.frequencies [0] = (IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB);
      a_ptone.frequencies [1] = 0;
      a_ptone.frequencies [2] = (IFX_TAPI_TONE_FREQC | IFX_TAPI_TONE_FREQB);
      ret = TAPI_Phone_Add_SimpleTone (pToneTable, &a_ptone);
   }

   if (ret == IFX_ERROR)
   {
      pChannel->TapiCidConf.nAlertToneTime   = 0;
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("CID alert predefined tone "
             "configuration failed for tone index %d\n\r", a_ptone.index));
   }

   return ret;
}

/**
   Sets alert tone time according to standard and transmission mode.
\param
   pChannel - handle to TAPI_CONNECTION structure, data channel
\param
   pCidInfo - contains the caller id settings (IFX_TAPI_CID_MSG_t)
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a data channel.
*/
IFX_LOCAL IFX_int32_t cid_set_alerttone_time (TAPI_CONNECTION *pChannel,
                                              IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   TAPI_CID_CONF          *pCidConf   = &pChannel->TapiCidConf;
   COMPLEX_TONE           *pTone      = pChannel->pTapiDevice->pToneTbl;
   IFX_TAPI_TONE_SIMPLE_t *pAlertTone = IFX_NULL;

   switch (pCidInfo->txMode)
   {
      case IFX_TAPI_CID_HM_ONHOOK:
         pAlertTone = &pTone[pCidConf->nAlertToneOnhook].tone.simple;
         pCidConf->nAlertToneTime =  pAlertTone->cadence [0];
         break;
      case IFX_TAPI_CID_HM_OFFHOOK:
         pAlertTone = &pTone[pCidConf->nAlertToneOffhook].tone.simple;
         if (pCidConf->nStandard == IFX_TAPI_CID_STD_NTT)
         {
            pCidConf->nAlertToneTime =  pAlertTone->cadence [0] +
                                        pAlertTone->cadence [1] +
                                        pAlertTone->cadence [2] ;
         }
         else
            pCidConf->nAlertToneTime =  pAlertTone->cadence [0];
         break;
   }

   return IFX_SUCCESS;
}

/**
   Sets reversed line mode for standards with Line Reversal.
\param
   pChannel - handle to TAPI_CONNECTION structure, data channel
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a data channel. If phone channel variables are used,
     instance of phone channel should be used.
*/
IFX_LOCAL IFX_int32_t cid_set_reversedline (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t  ret = IFX_SUCCESS;
   TAPI_CID_DATA *pCidData = &pChannel->TapiCidTx;

   /* reverse line */
   switch (pCidData->pPhoneCh->TapiOpControlData.nLineMode)
   {
   case IFX_TAPI_LINE_FEED_ACTIVE:
   case IFX_TAPI_LINE_FEED_RING_PAUSE:
      pCidData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE_REV;
      break;
   case IFX_TAPI_LINE_FEED_ACTIVE_REV:
      pCidData->nLineModeReverse = IFX_TAPI_LINE_FEED_ACTIVE;
      break;
   case IFX_TAPI_LINE_FEED_NORMAL_AUTO:
      pCidData->nLineModeReverse = IFX_TAPI_LINE_FEED_REVERSED_AUTO;
      break;
   case IFX_TAPI_LINE_FEED_REVERSED_AUTO:
      pCidData->nLineModeReverse = IFX_TAPI_LINE_FEED_NORMAL_AUTO;
      break;
   default:
      TRACE (TAPI_DRV,DBG_LEVEL_HIGH,
             ("No reverse option for actual line mode!\r\n"));
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

/**
   Mutes/Restores Voice talking and listening path during off hook CID processing.
\param
   pChannel    - handle to TAPI_CONNECTION structure, data channel
\param
   bMute - IFX_TRUE = mute voice / IFX_FALSE = restore voice
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a data channel
\todo :
   FIXME: Actually, the Voice path is disabled for muting. This is not allowed
   for conferencing.
*/
IFX_LOCAL IFX_int32_t cid_mutevoice (TAPI_CONNECTION *pChannel,
                                     IFX_boolean_t bMute)
{
   IFX_int32_t ret;

   if (bMute == IFX_FALSE)
   {
      ret = TAPI_LL_Phone_Start_Recording (pChannel);
      if (ret == IFX_SUCCESS)
         ret = TAPI_LL_Phone_Start_Playing (pChannel);
   }
   else
   {
      ret = TAPI_LL_Phone_Stop_Recording (pChannel);
      if (ret == IFX_SUCCESS)
         TAPI_LL_Phone_Stop_Playing (pChannel);
   }

   return ret;
}


/**
   State machine to handle the alert phase of the caller id sequence.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   FSM_STATUS_t
\remark
   - operation is done on a data channel. If phone channel operations are done
     (e.g. line modes), instance of phone channel should be used.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
IFX_LOCAL FSM_STATUS_t cid_fsm_alert_exec(TAPI_CONNECTION *pChannel)
{
   FSM_STATUS_t   ret         = E_FSM_CONTINUE;
   TAPI_CID_CONF  *pCidConf   = &pChannel->TapiCidConf;
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;
   TAPI_CONNECTION *pPhoneCh   = pCidData->pPhoneCh;
   TAPI_RING_DATA  *pRingData  = &pPhoneCh->TapiRingData;
   IFX_uint32_t   nTime       = 0;

   switch (pCidData->nAlertType)
   {
   case TAPI_CID_ALERT_NONE:
      nTime = MIN_TIMER_VAL;
      ret   = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_OSI:
      if (pCidData->nCidSubState==0)
      {
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_DISABLED);
         nTime = pCidConf->OSItime;
      }
      else
      {
         /* previous mode was high impedance mode, in which case the line
            was disabled. So set line back to stand by before setting the last
            line mode. */
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_STANDBY);
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_ACTIVE);
         nTime = pCidConf->TapiCidTiming.afterOSI;
         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_FR:
   case TAPI_CID_ALERT_RP:
      if (pCidData->nCidSubState==0)
      {
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_RING_BURST);
         if (pCidData->nAlertType == TAPI_CID_ALERT_FR)
            nTime = pCidConf->cadenceRingBurst;
         else
            nTime = pCidConf->ringPulseTime;
      }
      else
      {
         TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_RING_PAUSE);
         if (pCidData->nAlertType == TAPI_CID_ALERT_FR)
            nTime = pCidConf->TapiCidTiming.afterFirstRing;
         else
            nTime = pCidConf->TapiCidTiming.afterRingPulse;

         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_CAS:
      /* Play CAS tone on data channel */
      if (TAPI_Phone_Tone_Play (pChannel, pCidConf->nAlertToneOffhook,
                               TAPI_TONE_DST_LOCAL) == IFX_ERROR)
      {
         ret = E_FSM_ERROR;
         break;
      }
      /* mute voice path */
      if (cid_mutevoice(pChannel, IFX_TRUE) == IFX_SUCCESS)
         pCidData->bMute = IFX_TRUE;
      /* set time to wait considering tone play time (80 ms for Offhook)*/
      nTime = pCidConf->TapiCidTiming.cas2ackTime + pCidConf->nAlertToneTime;
      ret = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_DTAS:
      switch (pCidData->txHookMode)
      {
         case  IFX_TAPI_CID_HM_ONHOOK:
            /* Play Onhook DTAS tone on data channel */
            if (TAPI_Phone_Tone_Play (pChannel, pCidConf->nAlertToneOnhook,
                                      TAPI_TONE_DST_LOCAL) == IFX_ERROR)
            {
               ret = E_FSM_ERROR;
               break;
            }
            /* set time to wait considering tone play time (100 ms for Onhook) */
            nTime = pCidConf->TapiCidTiming.afterDTASOnhook +
                    pCidConf->nAlertToneTime;
            break;
         case IFX_TAPI_CID_HM_OFFHOOK:
            /* FIXME: use tone-api with pCidConf->nAlertToneOffhook */
            if (TAPI_Phone_Tone_Play (pChannel, pCidConf->nAlertToneOffhook,
                                      TAPI_TONE_DST_LOCAL)== IFX_ERROR)
            {
               ret = E_FSM_ERROR;
               break;
            }
            /* mute voice path */
            if (cid_mutevoice(pChannel, IFX_TRUE) == IFX_SUCCESS)
               pCidData->bMute = IFX_TRUE;
            /* set time to wait considering tone play time (80 ms for Offhook). */
            nTime = pCidConf->TapiCidTiming.cas2ackTime +
                    pCidConf->nAlertToneTime;
            break;
      }
      ret = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_LR_DTAS:
      if (pCidData->nCidSubState==0)
      {
         TAPI_Phone_Set_Linefeed (pPhoneCh, pCidData->nLineModeReverse);
         nTime = pCidConf->TapiCidTiming.afterLineReversal;
      }
      else
      {
         /* Play DTAS is onhook tone */
         if (TAPI_Phone_Tone_Play (pChannel, pCidConf->nAlertToneOnhook,
                                   TAPI_TONE_DST_LOCAL) == IFX_ERROR)
         {
            ret = E_FSM_ERROR;
            break;
         }
         /* set time to wait considering tone play time (100 ms for Onhook */
         nTime = pCidConf->TapiCidTiming.afterDTASOnhook +
                 pCidConf->nAlertToneTime;
         ret = E_FSM_COMPLETE;
      }
      break;
   case TAPI_CID_ALERT_AS_NTT:
      /* Play AS_NTT tone */
      if (TAPI_Phone_Tone_Play (pChannel, pCidConf->nAlertToneOffhook,
                                TAPI_TONE_DST_LOCAL) == IFX_ERROR)
      {
         ret = E_FSM_ERROR;
         break;
      }
      /* set time to wait, considering tone play time */
      nTime = pCidConf->TapiCidTiming.beforeData +
              pCidConf->nAlertToneTime;
      ret = E_FSM_COMPLETE;
      break;
   case TAPI_CID_ALERT_CAR_NTT:
      if (pCidData->nCidSubState==0)
      {
         TAPI_Phone_Set_Linefeed (pPhoneCh, pCidData->nLineModeReverse);
         nTime = pCidConf->TapiCidTiming.afterLineReversal;
      }
      else
      {
         if (pRingData->bIsRingBurstState == IFX_FALSE)
         {
            TAPI_Phone_Set_Linefeed (pPhoneCh, IFX_TAPI_LINE_FEED_RING_BURST);
            pRingData->bIsRingBurstState = IFX_TRUE;
            nTime = pCidConf->ringPulseTime;
         }
         else
         {
            TAPI_Phone_Set_Linefeed (pPhoneCh, pCidData->nLineModeReverse);
            pRingData->bIsRingBurstState = IFX_FALSE;
            nTime = pCidConf->ringPulseTime;
            /* for each ring pulse sequence (ringing / pause), substate is
               incremented by 2 */
            if (((pCidConf->ringPulseLoop << 1) == pCidData->nCidSubState) ||
                (pCidData->bAck == IFX_TRUE))
            {
               /* resume asap, using the minimum timer time */
               nTime = MIN_TIMER_VAL;
               ret   = E_FSM_COMPLETE;
            }
         }
      }
      break;
   }

   if (ret == E_FSM_CONTINUE)
      pCidData->nCidSubState++;

   if (ret != E_FSM_ERROR)
   {
      /* in case of no error, nTime must have been set accordingly. */
      IFXOS_ASSERT (nTime != 0);
      TAPI_SetTime_Timer (pCidData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);
   }

   return ret;
}


/**
   State machine to handle the ack phase of the caller id sequence.
   Will be called for off hook cid (not NTT) and on hook (only NTT)!
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   FSM_STATUS_t
\remarks
   - operation is done on a data channel
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken.
*/
IFX_LOCAL FSM_STATUS_t cid_fsm_ack_exec(TAPI_CONNECTION *pChannel)
{
   FSM_STATUS_t   ret         = E_FSM_CONTINUE;
   TAPI_CID_CONF  *pCidConf   = &pChannel->TapiCidConf;
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;
   IFX_uint32_t   nTime       = MIN_TIMER_VAL;

   if (pCidData->bAck == IFX_TRUE)
   {
      nTime = pCidConf->TapiCidTiming.ack2dataOutTime;
      ret   = E_FSM_COMPLETE;
   }
   else
   {
      /* TODO: start this timer to report cid event "Error: no ack" */
      nTime = pCidConf->TapiCidTiming.afterAckTimeout;
      TAPI_Error_Event_Update (pChannel, IFX_TAPI_RT_ERROR_CIDTX_NOACK);
      TRACE (TAPI_DRV,DBG_LEVEL_HIGH, ("no CID ack received!\r\n"));

      ret = E_FSM_ERROR;
   }

   if (ret != E_FSM_ERROR)
      TAPI_SetTime_Timer (pCidData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);

   return ret;
}

/**
   State machine to handle the cid sending phase of the caller id sequence.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   FSM_STATUS_t
\remarks
   - operation is done on a data channel
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
IFX_LOCAL FSM_STATUS_t cid_fsm_sending_exec(TAPI_CONNECTION *pChannel)
{
   FSM_STATUS_t   ret         = E_FSM_COMPLETE;
   TAPI_CID_CONF  *pCidConf   = &pChannel->TapiCidConf;
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;
   IFX_uint32_t   nTime       = pCidData->nTxTime;

   /* Transmit time must have been set at CID data preparation time */
   IFXOS_ASSERT(pCidData->nTxTime != 0);

   if (pCidData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK)
      nTime += pCidConf->TapiCidTiming.dataOut2restoreTimeOffhook;
   else
      nTime += pCidConf->TapiCidTiming.dataOut2restoreTimeOnhook;

   if (TAPI_LL_Phone_CID_Sending (pChannel, &pChannel->TapiCidTx) == IFX_ERROR)
      ret = E_FSM_ERROR;

   if (ret != E_FSM_ERROR)
      TAPI_SetTime_Timer (pCidData->CidTimerID, nTime, IFX_FALSE, IFX_FALSE);

   return ret;
}


/**
   State machine to handle the phase after cid sending.
   It may start ringing or restore the voice path.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   FSM_STATUS_t
\remark
   - operation is done on a data channel. In case a phone channel operation
     should be done here, an instance of the phone channel pointer should be
     used.
   - This function is called from the higher level fsm and assumes that the
   data_lock is taken and the initial line mode was saved.
*/
IFX_LOCAL FSM_STATUS_t cid_fsm_sending_complete_exec(TAPI_CONNECTION *pChannel)
{
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;

   if (pCidData->bMute == IFX_TRUE)
   {
      /* undo muting of voice path */
      if (cid_mutevoice(pChannel, IFX_FALSE) == IFX_SUCCESS)
         pCidData->bMute = IFX_FALSE;
   }

   if (pCidData->bRingStart == IFX_TRUE)
   {
      /* start ringing on the phone channel */
      TAPI_Phone_Ringtimer_Start(pCidData->pPhoneCh);
      pCidData->bRingStart = IFX_FALSE;
   }

   TAPI_SetTime_Timer (pCidData->CidTimerID, MIN_TIMER_VAL, IFX_FALSE, IFX_FALSE);

   /* this was the last part of cid sequence */
   pChannel->TapiCidTx.bActive = IFX_FALSE;

   return E_FSM_COMPLETE;
}


/**
   State machine to handle the overall caller id sequence.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   FSM_STATUS_t
\remark
   - operation is done on a data channel
   - This function assumes that the data_lock is taken and the initial line mode
   was saved.
*/
IFX_LOCAL FSM_STATUS_t cid_fsm_exec(TAPI_CONNECTION *pChannel)
{
   TAPI_CID_CONF *pCidConf = &pChannel->TapiCidConf;
   TAPI_CID_DATA *pCidData = &pChannel->TapiCidTx;
   FSM_STATUS_t   ret      = E_FSM_CONTINUE;

   switch (pCidData->nCidState)
   {
   case TAPI_CID_STATE_ALERT:
      ret = cid_fsm_alert_exec(pChannel);
      break;
   case TAPI_CID_STATE_ACK:
      ret = cid_fsm_ack_exec(pChannel);
      break;
   case TAPI_CID_STATE_SENDING:
      ret = cid_fsm_sending_exec(pChannel);
      break;
   case TAPI_CID_STATE_SENDING_COMPLETE:
      ret = cid_fsm_sending_complete_exec(pChannel);
      break;
   case TAPI_CID_STATE_IDLE:
       ret = E_FSM_COMPLETE;
       break;
   }

   /* update state */
   switch (ret)
   {
   case E_FSM_COMPLETE:
      pCidData->nCidSubState = 0;
      ret = E_FSM_CONTINUE;
      switch (pCidData->nCidState)
      {
      case TAPI_CID_STATE_ALERT:
         if (((pCidData->txHookMode == IFX_TAPI_CID_HM_ONHOOK) &&
              (pCidConf->nStandard != IFX_TAPI_CID_STD_NTT)) ||
             ((pCidData->txHookMode == IFX_TAPI_CID_HM_OFFHOOK) &&
              (pCidConf->nStandard == IFX_TAPI_CID_STD_NTT)))
            /* skip ack phase for on hook (not ntt) and off hook (only ntt) */
            pCidData->nCidState = TAPI_CID_STATE_SENDING;
         else
            pCidData->nCidState = TAPI_CID_STATE_ACK;
         break;
      case TAPI_CID_STATE_ACK:
         pCidData->nCidState = TAPI_CID_STATE_SENDING;
         break;
      case TAPI_CID_STATE_SENDING:
         pCidData->nCidState = TAPI_CID_STATE_SENDING_COMPLETE;
         break;
      case TAPI_CID_STATE_SENDING_COMPLETE:
         pCidData->nCidState = TAPI_CID_STATE_IDLE;
         break;
      case TAPI_CID_STATE_IDLE:
         ret = E_FSM_COMPLETE;
         /* no change */
         break;
      }
      break;
   case E_FSM_CONTINUE:
   case E_FSM_ERROR:
   default:
      break;
   }

   return ret;
}


/**
   Reset state machine variables.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - operation is done on a data channel
*/
IFX_LOCAL IFX_int32_t cid_fsm_reset(TAPI_CONNECTION *pChannel)
{
   TAPI_CID_DATA *pCidData = &pChannel->TapiCidTx;
   IFX_int32_t    ret;

   pCidData->bAck = IFX_FALSE;
   pCidData->nCidState = TAPI_CID_STATE_ALERT;
   pCidData->nCidSubState = 0;
   pCidData->bMute = IFX_FALSE;

   ret = cid_set_reversedline (pChannel);

   return ret;
}


/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   CID timer.
\param
   Timer - timer id
\param
   nArg  - timer callback argument -> pointer to TAPI channel
\return
   none
\remark
   timer runs on a data channel
*/
IFX_void_t TAPI_Phone_CID_OnTimer(Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION   *pChannel   = (TAPI_CONNECTION *) nArg;
   TAPI_CID_DATA     *pCidData   = &pChannel->TapiCidTx;
   FSM_STATUS_t      ret         = E_FSM_CONTINUE;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* execute cid state machine */
   ret = cid_fsm_exec(pChannel);

   if (ret != E_FSM_CONTINUE)
   {
      /* cleanup */
      pCidData->bActive = IFX_FALSE;
      if (pCidData->bMute)
      {
         /* undo muting of voice path */
         if (cid_mutevoice(pChannel, IFX_FALSE) == IFX_SUCCESS)
            pCidData->bMute = IFX_FALSE;
      }
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);
}


/**
   Processes user CID data according to chosen service and cid sending type
   (FSK, DTMF) and send it out directly.
\param
   pChannel  - handle to TAPI_CONNECTION structure
\param
   pCidInfo  - contains the caller id settings (IFX_TAPI_CID_MSG_t)
\return
   IFX_SUCCESS / IFX_ERROR
Remark:
   - If a non supported service is set, error will be returned.
   - This implementation complies with ETSI standards
   - This interface is assumed to be working on a data channel.
*/
IFX_int32_t TAPI_Phone_CID_Info_Tx( TAPI_CONNECTION *pChannel,
                                    IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_int32_t    ret = IFX_SUCCESS;
   IFX_uint8_t           phone_ch;
   TAPI_OPCONTROL_DATA  *pOpCtrl  = IFX_NULL;
   TAPI_CID_DATA        *pCidData = &pChannel->TapiCidTx;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* get phone channel on which this data channel is connected
      and update phone operations pointers accordingly */
   TAPI_Data_Get_Phone_Channel (pChannel, &phone_ch);
   pCidData->pPhoneCh = &pChannel->pTapiDevice->pChannelCon [phone_ch];
   pOpCtrl  = &pCidData->pPhoneCh->TapiOpControlData;

   /* check for on hook/off hook mismatch with current state */
   if ( ((pCidInfo->txMode == IFX_TAPI_CID_HM_OFFHOOK) &&
         (pOpCtrl->bHookState == IFX_FALSE)) ||
        ((pCidInfo->txMode == IFX_TAPI_CID_HM_ONHOOK) &&
         (pOpCtrl->bHookState == IFX_TRUE)) )
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Hookstate not matching"
             " with transmission mode!\r\n"));
      ret = IFX_ERROR;
   }

   /* caller id already active! */
   if (pChannel->TapiCidTx.bActive == IFX_TRUE)
      ret = IFX_ERROR;

   if (ret == IFX_SUCCESS)
      ret = cid_prepare_data(pChannel, pCidInfo);

   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_LL_Phone_CID_Sending (pChannel, pCidData);
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}


/**
   Processes user CID data according to chosen service and cid sending type
   (FSK, DTMF) and starts the complete CID sequence depending on the
   configuration.
\param
   pChannel - handle to TAPI_CONNECTION structure
\param
   pCidInfo - contains the caller id settings (IFX_TAPI_CID_MSG_t)
\return
   IFX_SUCCESS / IFX_ERROR
Remark:
   - If a non supported service is set, error will be returned.
   - This interface is assumed to be working on a data channel. In case a
     phone operation should be done (i.e line mode), an instance of the
     associated phone channel should be used.
*/
IFX_int32_t TAPI_Phone_CID_Seq_Tx(TAPI_CONNECTION *pChannel,
                                  IFX_TAPI_CID_MSG_t const *pCidInfo)
{
   IFX_int32_t          ret = IFX_SUCCESS;
   IFX_uint8_t           phone_ch;
   TAPI_CID_CONF        *pCidConf = &pChannel->TapiCidConf;
   TAPI_CID_DATA        *pCidData = &pChannel->TapiCidTx;
   TAPI_OPCONTROL_DATA  *pOpCtrl  = IFX_NULL;


   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* get phone channel on which this data channel is connected
      and update phone operations pointers accordingly */
   TAPI_Data_Get_Phone_Channel (pChannel, &phone_ch);
   pCidData->pPhoneCh = &pChannel->pTapiDevice->pChannelCon [phone_ch];
   pOpCtrl  = &pCidData->pPhoneCh->TapiOpControlData;

   /* check for on hook/off hook mismatch with current state */
   if ( ((pCidInfo->txMode == IFX_TAPI_CID_HM_OFFHOOK) &&
        (pOpCtrl->bHookState  == IFX_FALSE)) ||
        ((pCidInfo->txMode == IFX_TAPI_CID_HM_ONHOOK) &&
         (pOpCtrl->bHookState == IFX_TRUE))
      )
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Hookstate not matching"
             " with transmission mode!\r\n"));
      ret = IFX_ERROR;
   }
   if (ret == IFX_SUCCESS)
   {
      ret = cid_prepare_data(pChannel, pCidInfo);
      if (ret != IFX_SUCCESS) printk("\nDRV_ERROR: cid_prepare_data\n");
   }
   
   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_Phone_Ring_Prepare(pCidData->pPhoneCh);
      if (ret != IFX_SUCCESS) printk("\nDRV_ERROR: TAPI_Phone_Ring_Prepare\n");
   }

   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_LL_Phone_Dtmf_GetCode(pCidConf->ackTone, &pCidData->ackToneCode);
      if (ret != IFX_SUCCESS) printk("\nDRV_ERROR: TAPI_LL_Phone_Dtmf_GetCode\n");
   }

   if (ret == IFX_SUCCESS)
   {
      ret = cid_determine_alert_type(pChannel, pCidInfo);
      if (ret != IFX_SUCCESS) printk("\nDRV_ERROR: cid_determine_alert_type\n");
   }
   
   if (ret == IFX_SUCCESS)
   {
      ret = cid_set_alerttone_time (pChannel, pCidInfo);
      if (ret != IFX_SUCCESS) printk("\nDRV_ERROR: cid_set_alerttone_time\n");
   }
   
   /* check line mode and switch line to active for CID */
   if (ret == IFX_SUCCESS)
   {
      /* Line must be either in standby mode or in active mode to send CID.
         Any other mode is an error */
      switch (pOpCtrl->nLineMode)
      {
         case  IFX_TAPI_LINE_FEED_STANDBY:
         case  IFX_TAPI_LINE_FEED_RING_PAUSE:
            TAPI_Phone_Set_Linefeed (pCidData->pPhoneCh, IFX_TAPI_LINE_FEED_ACTIVE);
            break;
         default:
            if (pOpCtrl->nLineMode != IFX_TAPI_LINE_FEED_ACTIVE)
            {
               TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Line mode not"
                     " suitable for CID sequence transmission !\r\n"));
               ret = IFX_ERROR;
            }
            break;
      }
   }
   if (ret == IFX_SUCCESS)
   {
      /* reset for initial state of state machine */
      ret = cid_fsm_reset(pChannel);
      if (ret == IFX_SUCCESS)
      {
         /* Now the process can start */
         pChannel->TapiCidTx.bActive = IFX_TRUE;
         /* execute first step of state machine */
         if (cid_fsm_exec(pChannel) == E_FSM_ERROR)
         {
            ret = IFX_ERROR;
            pChannel->TapiCidTx.bActive = IFX_FALSE;
         }
      }
      else
      {
	      printk("\nDRV_ERROR: cid_fsm_reset\n");
      }
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/* ============================= */
/* CID config global functions   */
/* ============================= */

/**
   Set the default configuration for caller id.
\param
   pChannel - handle to TAPI_CONNECTION structure
\return
   always IFX_SUCCESS
*/
IFX_int32_t TAPI_Phone_CID_SetDefaultConfig (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_SUCCESS;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   pChannel->TapiCidConf.nStandard = IFX_TAPI_CID_STD_TELCORDIA;
   pChannel->TapiCidConf.nETSIAlertRing = IFX_TAPI_CID_ALERT_ETSI_FR;
   pChannel->TapiCidConf.nETSIAlertNoRing = IFX_TAPI_CID_ALERT_ETSI_RP;
   pChannel->TapiCidConf.nAlertToneOnhook  = 0;
   pChannel->TapiCidConf.nAlertToneOffhook  = 0;
   pChannel->TapiCidConf.ackTone = 'D';
   pChannel->TapiCidConf.ringPulseTime = 500;
   pChannel->TapiCidConf.ringPulseLoop = 5;
   pChannel->TapiCidConf.OSIoffhook = IFX_FALSE;
   pChannel->TapiCidConf.OSItime = 200;

   ciddtmf_abscli_etsi(&pChannel->TapiCidConf.TapiCidDtmfAbsCli);
   pChannel->TapiCidConf.TapiCidTiming   = default_cid_timing;
   pChannel->TapiCidConf.TapiCidFskConf  = default_cid_fsk_conf;
   pChannel->TapiCidConf.TapiCidDtmfConf = default_cid_dtmf_conf;

   /* setup predifined alert tones */
   ret = cid_alert_predeftone (pChannel);

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}


/**
   Set the configuration for caller id.
\param
   pChannel - handle to TAPI_CONNECTION structure
\param
   pCidConf - contains the new caller id configuration (IFX_TAPI_CID_CFG_t)
\return
   IFX_SUCCESS / IFX_ERROR
*/
IFX_int32_t TAPI_Phone_CID_SetConfig (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_CID_CFG_t const *pCidConf)
{
   IFX_TAPI_CID_TIMING_t     *pCIDTiming  = IFX_NULL;
   IFX_TAPI_CID_FSK_CFG_t    *pFSKConf    = IFX_NULL;
   IFX_TAPI_CID_DTMF_CFG_t   *pDTMFConf   = IFX_NULL;
   IFX_TAPI_CID_ABS_REASON_t *pDTMFAbsCli = IFX_NULL;
   IFX_TAPI_CID_STD_TYPE_t   *cfg         = IFX_NULL;
   TAPI_CID_CONF             *pTapiCidConf = &pChannel->TapiCidConf;
   IFX_int32_t               ret           = IFX_SUCCESS;

   cfg = TAPI_MapBuffer(pCidConf->cfg, sizeof(IFX_TAPI_CID_STD_TYPE_t));

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* reset settings, to have default values if they are not provided
      by the new configuration */
   pTapiCidConf->nETSIAlertRing    = IFX_TAPI_CID_ALERT_ETSI_FR;
   pTapiCidConf->nETSIAlertNoRing  = IFX_TAPI_CID_ALERT_ETSI_RP;
   pTapiCidConf->nAlertToneOnhook  = 0;
   pTapiCidConf->nAlertToneOffhook = 0;
   pTapiCidConf->ackTone           = 'D';
   pTapiCidConf->OSIoffhook        = IFX_FALSE;
   pTapiCidConf->OSItime           = 200;

   switch (pCidConf->nStandard)
   {
   case IFX_TAPI_CID_STD_TELCORDIA:
      if (cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_TELCORDIA_t *telcordia = &cfg->telcordia;
         pTapiCidConf->nAlertToneOnhook = telcordia->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = telcordia->nAlertToneOffhook;
         pTapiCidConf->ackTone = telcordia->ackTone;
         pTapiCidConf->OSIoffhook = telcordia->OSIoffhook;
         pTapiCidConf->OSItime = telcordia->OSItime;
         pCIDTiming = telcordia->pCIDTiming;
         pFSKConf = telcordia->pFSKConf;
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_FSK:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_ETSI_FSK_t *etsiFSK = &cfg->etsiFSK;
         pTapiCidConf->nAlertToneOnhook = etsiFSK->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = etsiFSK->nAlertToneOffhook;
         pTapiCidConf->ringPulseTime = etsiFSK->ringPulseTime;
         pTapiCidConf->ackTone = etsiFSK->ackTone;
         pTapiCidConf->nETSIAlertRing = etsiFSK->nETSIAlertRing;
         pTapiCidConf->nETSIAlertNoRing = etsiFSK->nETSIAlertNoRing;
         pCIDTiming = etsiFSK->pCIDTiming;
      }
      break;
   case IFX_TAPI_CID_STD_ETSI_DTMF:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_ETSI_DTMF_t *etsiDTMF = &cfg->etsiDTMF;
         pTapiCidConf->nAlertToneOnhook = etsiDTMF->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = etsiDTMF->nAlertToneOffhook;
         pTapiCidConf->ringPulseTime = etsiDTMF->ringPulseTime;
         pTapiCidConf->ackTone = etsiDTMF->ackTone;
         pTapiCidConf->nETSIAlertRing = etsiDTMF->nETSIAlertRing;
         pTapiCidConf->nETSIAlertNoRing = etsiDTMF->nETSIAlertNoRing;
         pDTMFConf   = etsiDTMF->pDTMFConf;
         pDTMFAbsCli = etsiDTMF->pABSCLICode;
         pCIDTiming  = etsiDTMF->pCIDTiming;
      }
      break;
   case IFX_TAPI_CID_STD_SIN:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_SIN_t *sin = &cfg->sin;
         pTapiCidConf->nAlertToneOnhook = sin->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = sin->nAlertToneOffhook;
         pTapiCidConf->ackTone = sin->ackTone;
         pCIDTiming = sin->pCIDTiming;
      }
      break;
   case IFX_TAPI_CID_STD_NTT:
      if (pCidConf->cfg != IFX_NULL)
      {
         IFX_TAPI_CID_STD_NTT_t *ntt = &cfg->ntt;
         pTapiCidConf->nAlertToneOnhook = ntt->nAlertToneOnhook;
         pTapiCidConf->nAlertToneOffhook = ntt->nAlertToneOffhook;
         pTapiCidConf->ringPulseTime = ntt->ringPulseTime;
         pTapiCidConf->ringPulseLoop = ntt->ringPulseLoop;
         pCIDTiming = ntt->pCIDTiming;
         if (ntt->ringPulseLoop == 0)
            ret = IFX_ERROR;
      }
      break;
   default:
      ret = IFX_ERROR;
      break;
   }

   if (ret != IFX_ERROR)
   {
      /* copy from application pointers or from default, if necessary */
      if (pCIDTiming == IFX_NULL)
         pTapiCidConf->TapiCidTiming = default_cid_timing;
      else
         IFXOS_CPY_USR2KERN(&pTapiCidConf->TapiCidTiming,
                            pCIDTiming, sizeof(IFX_TAPI_CID_TIMING_t));

      if (pFSKConf == IFX_NULL)
         pTapiCidConf->TapiCidFskConf = default_cid_fsk_conf;
      else
         IFXOS_CPY_USR2KERN(&pTapiCidConf->TapiCidFskConf,
                            pFSKConf, sizeof(IFX_TAPI_CID_FSK_CFG_t));

      if (pDTMFAbsCli == IFX_NULL)
         ciddtmf_abscli_etsi(&pTapiCidConf->TapiCidDtmfAbsCli);
      else
         IFXOS_CPY_USR2KERN(&pTapiCidConf->TapiCidDtmfAbsCli,
                            pDTMFAbsCli, sizeof(IFX_TAPI_CID_ABS_REASON_t));

      if (pDTMFConf == IFX_NULL)
         pTapiCidConf->TapiCidDtmfConf = default_cid_dtmf_conf;
      else
         IFXOS_CPY_USR2KERN(&pTapiCidConf->TapiCidDtmfConf,
                            pDTMFConf, sizeof(IFX_TAPI_CID_DTMF_CFG_t));

      /* finally save the standard */
      pTapiCidConf->nStandard = pCidConf->nStandard;
   }

   /* set default alert tones if applicable */
   if (ret != IFX_ERROR)
   {
      /* set onhook alert tone */
      if (pTapiCidConf->nAlertToneOnhook == 0)
      {
         pTapiCidConf->nAlertToneOnhook = TAPI_CID_ATONE_INDEXDEF_ONHOOK;
      }
      /* set offhook alert tone */
      if (pTapiCidConf->nAlertToneOffhook == 0)
      {
         if (pCidConf->nStandard == IFX_TAPI_CID_STD_NTT)
            pTapiCidConf->nAlertToneOffhook = TAPI_CID_ATONE_INDEXDEF_OFFHOOKNTT;
         else
            pTapiCidConf->nAlertToneOffhook = TAPI_CID_ATONE_INDEXDEF_OFFHOOK;
      }
   }

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return ret;
}

/* ============================= */
/* CID Receiver global functions */
/* ============================= */

/**
   Starts caller id receiver.
\param
   pChannel     - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   The caller id status is updated after successful start at low level and the
   internal data buffer is reset.
*/
IFX_int32_t TAPI_Phone_CidRx_Start (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   ret = TAPI_LL_Phone_CidRx_Start (pChannel);
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiCidRx.pData = IFX_NULL;
      pChannel->TapiCidRx.stat.nStatus = IFX_TAPI_CID_RX_STATE_ACTIVE;
   }

   return ret;
}

/**
   Stop caller id receiver.
\param
   pChannel     - handle to TAPI_CONNECTION structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   The caller id status is updated after successful start at low level.
*/
IFX_int32_t TAPI_Phone_CidRx_Stop (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   ret = TAPI_LL_Phone_CidRx_Stop (pChannel);
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiCidRx.stat.nStatus = IFX_TAPI_CID_RX_STATE_INACTIVE;
   }

   return ret;
}

/**
   Get the cid data already collected.
\param
   pChannel     - handle to TAPI_CONNECTION structure
\param
   pCidRxData   - handle to IFX_TAPI_CID_RX_DATA_t structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   in case the data collection is ongoing, reading from actual buffer should be
   protected against concurrent access. The low level call guarantees this.
*/
IFX_int32_t TAPI_Phone_Get_CidRxData (TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_CID_RX_DATA_t *pCidRxData)
{
   IFX_int32_t      ret = IFX_ERROR;
   IFX_TAPI_CID_RX_DATA_t *pFifo;
   TAPI_CIDRX      *pCidRx = &pChannel->TapiCidRx;

   if (pCidRx->stat.nError == IFX_TAPI_CID_RX_ERROR_NONE)
   {
      switch (pCidRx->stat.nStatus)
      {
      case IFX_TAPI_CID_RX_STATE_ONGOING:
         /* read from actual buffer */
         ret = TAPI_LL_Phone_Get_CidRxDataCollected (pChannel, pCidRxData);
         break;
      case IFX_TAPI_CID_RX_STATE_DATA_READY:
         /* read from fifo */
         pFifo =
          (IFX_TAPI_CID_RX_DATA_t *)Fifo_readElement(&(pCidRx->TapiCidRxFifo));
         /* copy data into user buffer */
         if (pFifo != IFX_NULL)
         {
            *pCidRxData = *pFifo;
            pChannel->TapiCidRx.pData = IFX_NULL;
            pChannel->TapiCidRx.stat.nStatus = IFX_TAPI_CID_RX_STATE_ACTIVE;
            ret = IFX_SUCCESS;
         }
         break;
      default:
         break;
      }
   }
   if (ret == IFX_ERROR)
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: No Cid Data copied\n\r"));

   return ret;
}

/**
   retrieves a buffer for caller id receiver data.
\param
   pCh       - channel pointer
\param
   nLen      - cid data  number received
\return
   buffer pointer
Remark :
   This function should be called each time memory is needed for Cid data
   buffering
*/
IFX_TAPI_CID_RX_DATA_t *TAPI_Phone_GetCidRxBuf (TAPI_CONNECTION *pChannel,
                                                IFX_uint32_t nLen)
{
   TAPI_CIDRX      *pCidRx = &pChannel->TapiCidRx;
   IFX_TAPI_CID_RX_DATA_t *pData  = pCidRx->pData;

   /* get a new buffer if there is actually no buffer allocated or if
      the previous buffer is short about to overflow. */
   if ((pData == IFX_NULL) ||
       (pData->nSize + nLen > IFX_TAPI_CID_SIZE_MAX))
   {
      /* get Buffer handler */
      pData =
          (IFX_TAPI_CID_RX_DATA_t *)Fifo_writeElement (&pCidRx->TapiCidRxFifo);
      if (pData != IFX_NULL)
      {
         memset (pData, 0, sizeof (IFX_TAPI_CID_RX_DATA_t));
         pCidRx->pData = pData;
      }
   }

   return pData;
}

#endif /* TAPI_CID */


