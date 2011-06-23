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
   Module      : drv_tapi_cid.c
   Description  : Implementation of features related to CID (CID1/CID2/CID RX)
   Remarks     :
      - Reference used for CID1 implementation is ETSI EN 300 659-1 V.1.3.1
      - Reference used for CID2 implementation (not NTT) is TIA/EIA-777
      - Reference used for CID2 implementation (NTT) is NTT
        (Nippon Telegraph and Telephone Corporation)
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_tapi.h"

#ifdef TAPI_CID

/* ============================= */
/* FSK defines (ETSI)            */
/* ============================= */

/* parameter types */
#define CIDFSK_DATE_LEN          0x08

/* ============================= */
/* FSK defines (NTT)             */
/* ============================= */

/* positions in buffer */
#define CID_NTT_TYPE_POS         5
#define CID_NTT_MSGLEN_POS       6

/* parameter types */
#define CID_NTT_PT_NUMBER        0x02
#define CID_NTT_NUMBER_MAXLEN    0x14
#define CID_NTT_PT_NAME          0x03
#define CID_NTT_NAME_MAXLEN      0x14

/* control codes <b7b6b5 b4b3b2b1> */
#define CID_NTT_DLE              0x10 /* 0b 001 0000 */
#define CID_NTT_SOH              0x01 /* 0b 000 0001 */
#define CID_NTT_HEADER           0x07 /* 0b 000 0111 */
#define CID_NTT_STX              0x02 /* 0b 000 0010 */
#define CID_NTT_TYPE1            0x40 /* 0b 100 0000 */
#define CID_NTT_TYPE2            0x41 /* 0b 100 0001 */
#define CID_NTT_ETX              0x03 /* 0b 000 0011 */
#define CID_NTT_SI               0x0F /* 0b 000 1111 */
#define CID_NTT_SO               0x0E /* 0b 000 1110 */

/*#define CID_BUFFER_CHECK                                 \
         {\
         IFX_uint8_t i;\
         PRINTF ("number of Data : %d\r\n", pCidData->nCidParamLen);  \
         for (i = 0; i < pCidData->nCidParamLen; i++) {               \
            PRINTF ("Data %d : 0x%02X\r\n", i,     \
            pCidData->cidParam [i]); }\
         }*/

/* ============================= */
/* Local macro declaration       */
/* ============================= */

#define LIMIT_VAL(val,maxval)    if ((val) > (maxval)) { (val) = (maxval); }

/* ============================= */
/* Local functions declaration   */
/* ============================= */

/* FSK (ETSI) */
IFX_LOCAL IFX_void_t   cidfsk_codedate (TAPI_CID_SERVICE_TYPE_t type,
                                       IFX_uint32_t month, IFX_uint32_t day,
                                       IFX_uint32_t hour, IFX_uint32_t mn,
                                       IFX_uint8_t *pCidBuf, IFX_uint8_t *pPos);
IFX_LOCAL IFX_void_t   cidfsk_codetype (TAPI_CID_SERVICE_TYPE_t type,
                                       IFX_uint8_t const *buf, IFX_uint8_t len,
                                       IFX_uint8_t *pCidBuf, IFX_uint8_t *pPos);
IFX_LOCAL IFX_uint8_t  cidfsk_calccrc  (IFX_uint8_t const *pCidBuf, IFX_uint8_t len);
IFX_LOCAL IFX_int32_t  cidfsk_code     (TAPI_CID_DATA *pCidData,
                                        TAPI_PHONE_CID const *pPhoneCid);
IFX_LOCAL IFX_int32_t  cidfsk_code_new (TAPI_CID_DATA *pCidData,
                                        TAPI_CID_INFO_t const *pCidInfo);

/* DTMF (ETSI) */
IFX_LOCAL IFX_int32_t  ciddtmf_code    (TAPI_CONNECTION *pChannel,
                                        TAPI_PHONE_CID const *pPhoneCid);
IFX_LOCAL IFX_int32_t  ciddtmf_code_new(TAPI_CONNECTION *pChannel,
                                        TAPI_CID_INFO_t const *pCidInfo);

/* FSK (NTT) */
IFX_LOCAL IFX_void_t   cidfskntt_reversebyte (IFX_uint8_t *byte);
IFX_LOCAL IFX_void_t   cidfskntt_setparity   (IFX_uint8_t *byte);
IFX_LOCAL IFX_void_t   cidfskntt_codetype    (IFX_uint8_t type, IFX_uint8_t const *buf, IFX_uint8_t len, IFX_uint8_t *pCidBuf, IFX_uint8_t *pPos);
IFX_LOCAL IFX_uint16_t cidfskntt_calccrc     (IFX_uint8_t const *pCidBuf, IFX_uint8_t len);
IFX_LOCAL IFX_int32_t  cidfskntt_code        (TAPI_CID_DATA *pCidData, TAPI_PHONE_CID const *pPhoneCid);

/* CID 2 */
IFX_LOCAL IFX_int32_t cid2_mutevoice         (TAPI_CONNECTION *pChannel, IFX_boolean_t bMute);
IFX_LOCAL IFX_void_t  cid2_timercallback     (Timer_ID Timer, IFX_int32_t nArg);
IFX_LOCAL IFX_int32_t cid2default_start      (TAPI_CONNECTION *pChannel);
IFX_LOCAL IFX_int32_t cid2ntt_start          (TAPI_CONNECTION *pChannel);

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ====================================== */
/* Local functions definition CID default */
/* ====================================== */

/*******************************************************************************
Description:
   Code CID parameters for DTMF transmission.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pPhoneCid   - contains the caller id settings (TAPI_PHONE_CID)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   Only digits can be send via DTMF - automatic conversion of ASCII signs
   assumes that user passes characters.
*******************************************************************************/
IFX_LOCAL IFX_int32_t ciddtmf_code (TAPI_CONNECTION *pChannel,
                                    TAPI_PHONE_CID const *pPhoneCid)
{
   IFX_int32_t    ret         = IFX_SUCCESS;
   TAPI_DTMFCID   *pDtmfCid   = &pChannel->TapiDtmfCidConf;
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;
   IFX_int8_t     tone,
                  *pCidBuf    = (IFX_int8_t*)pCidData->cidParam;
   IFX_uint8_t    i,
                  *pPos       = &pCidData->nCidParamLen;

   /* initialize pos */
   *pPos = 0;
   /* CLI data */
   if ((pPhoneCid->service & CIDCS_CLI) && (pPhoneCid->numlen != 0))
   {
      /* set start tone */
      pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->startTone);
      /* Copy to internal CID buffer */
      for (i = 0; i < pPhoneCid->numlen; i++)
      {
         tone = pPhoneCid->number[i];
         /* to leave it  backward compatible, check if numbers instead of
            characters were passed for numbers */
         if (tone <= 9)
         {
            tone += '0';
         }
         pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (tone);
      }
   }
   /* Redirection data */
   if ((pPhoneCid->service & CIDCS_REDIR) && (pPhoneCid->numrdno != 0))
   {
      /* Set redirection tone (tone must be [A-D] for correct conversion) */
      pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->redirStartTone);
      /* Copy to internal CID buffer */
      for (i = 0; i < pPhoneCid->numrdno; i++)
      {
         tone = pPhoneCid->redirno[i];
         /* to leave it  backward compatible, check if numbers instead of
            characters were passed for numbers */
         if (tone <= 9)
         {
            tone += '0';
         }
         pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (tone);
      }
   }
   /* Information data */
   if ((pPhoneCid->service & CIDCS_INFO) && (pPhoneCid->infolen != 0))
   {
      /* Set information tone (tone must be [A-D] for correct conversion) */
      pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->infoStartTone);
      for (i = 0; i < pPhoneCid->infolen; i++)
      {
         tone = pPhoneCid->info[i];
         /* to leave it  backward compatible, check if numbers instead of
            characters were passed for numbers */
         if (tone <= 9)
         {
            tone += '0';
         }
         pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (tone);
      }
   }
   /* set stop tone */
   pCidBuf [(*pPos)++] = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->stopTone);

   /* check if buffer is valid */
   for (i = 0; i < *pPos; i++)
   {
       tone = pCidBuf [i];
       if (tone == IFX_ERROR)
       {
          TRACE (TAPI_DRV,DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Invalid tone %c\r\n",
                 tone));
          ret = IFX_ERROR;
          break;
       }
   }

   /*CID_BUFFER_CHECK*/

   return ret;
}


/*******************************************************************************
Description:
   Code CID parameters for DTMF transmission.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pPhoneCid   - contains the caller id settings (TAPI_PHONE_CID)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   Only digits can be send via DTMF - automatic conversion of ASCII signs
   assumes that user passes characters.
*******************************************************************************/
IFX_LOCAL IFX_int32_t ciddtmf_code_new(TAPI_CONNECTION *pChannel,
                                       TAPI_CID_INFO_t const *pCidInfo)
{
   IFX_int32_t    ret         = IFX_SUCCESS;
   TAPI_DTMFCID   *pDtmfCid   = &pChannel->TapiDtmfCidConf;
   TAPI_CID_DATA  *pCidData   = &pChannel->TapiCidTx;
   IFX_int8_t     tone,
                  *pCidBuf    = (IFX_int8_t*)pCidData->cidParam;
   IFX_uint8_t    i, el,
                  *pPos       = &pCidData->nCidParamLen;
   TAPI_CID_MSG_ELEMENT_t *pMessage;

   /* initialize pos */
   *pPos = 0;

   /* search for CLI data */
   for (el=0; el<pCidInfo->nMsgElements; el++)
   {
      pMessage = pCidInfo->message + el;

      if (pMessage->string.elementType == TAPI_CID_ST_CLI)
      {
         IFX_uint32_t len = pMessage->string.len;
         IFX_uint8_t pos = *pPos;
         LIMIT_VAL(len,20);
         /* set start tone */
         tone = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->startTone);
         if (tone == IFX_ERROR)
            break;
         pCidBuf [pos++] = tone;
         /* Copy to internal CID buffer */
         for (i = 0; i < len; i++)
         {
            tone = TAPI_LL_Phone_Dtmf_GetCode (pMessage->string.element[i]);
            if (tone == IFX_ERROR)
               break;
            pCidBuf [pos++] = tone;
         }
         if (tone!=IFX_ERROR)
            *pPos = pos;
         break;
      }
   }

   /* search for Redirection data */
   for (el=0; el<pCidInfo->nMsgElements; el++)
   {
      pMessage = pCidInfo->message + el;

      if (pMessage->string.elementType == TAPI_CID_ST_REDIR)
      {
         IFX_uint32_t len = pMessage->string.len;
         IFX_uint8_t pos = *pPos;
         LIMIT_VAL(len,20);
         /* set start tone */
         tone = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->redirStartTone);
         if (tone == IFX_ERROR)
            break;
         pCidBuf [pos++] = tone;
         /* Copy to internal CID buffer */
         for (i = 0; i < len; i++)
         {
            tone = TAPI_LL_Phone_Dtmf_GetCode (pMessage->string.element[i]);
            if (tone == IFX_ERROR)
               break;
            pCidBuf [pos++] = tone;
         }
         if (tone!=IFX_ERROR)
            *pPos = pos;
         break;
      }
   }

   /* Search for Information data */
   for (el=0; el<pCidInfo->nMsgElements; el++)
   {
      pMessage = pCidInfo->message + el;

      if (pMessage->string.elementType == TAPI_CID_ST_DISP)
      {
         IFX_uint32_t len = pMessage->string.len;
         IFX_uint8_t pos = *pPos;
         LIMIT_VAL(len,20);
         /* set start tone */
         tone = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->infoStartTone);
         if (tone == IFX_ERROR)
            break;
         pCidBuf [pos++] = tone;
         /* Copy to internal CID buffer */
         for (i = 0; i < len; i++)
         {
            tone = TAPI_LL_Phone_Dtmf_GetCode (pMessage->string.element[i]);
            if (tone == IFX_ERROR)
               break;
            pCidBuf [pos++] = tone;
         }
         if (tone!=IFX_ERROR)
            *pPos = pos;
         break;
      }
   }

   tone = TAPI_LL_Phone_Dtmf_GetCode (pDtmfCid->stopTone);

   if ((tone == IFX_ERROR) || (*pPos == 0))
   {
      TRACE (TAPI_DRV,DBG_LEVEL_NORMAL,
         ("\n\rDRV_ERROR: No valid data for DTMP CID\r\n"));
      ret = IFX_ERROR;
   }
   else
   {
      /* set stop tone */
      pCidBuf [(*pPos)++] = tone;
   }

   /* CID_BUFFER_CHECK; */

   return ret;
}


/*******************************************************************************
Description:
   Codes date in CID Fsk buffer
Arguments:
   pPhoneCid  - contains the caller id settings (TAPI_PHONE_CID)
   pCidBuf    - cid data buffer
   pPos       - first data position in cid buffer
Return:
   None
Remark:
   ref ETSI EN 300 659-1 V.1.3.1
*******************************************************************************/
IFX_LOCAL IFX_void_t cidfsk_codedate (TAPI_CID_SERVICE_TYPE_t type,
                                       IFX_uint32_t month, IFX_uint32_t day,
                                       IFX_uint32_t hour, IFX_uint32_t mn,
                                       IFX_uint8_t *pCidBuf, IFX_uint8_t *pPos)
{
   /* set date and time parameter type : 0x01 or 0x0F */
   pCidBuf [(*pPos)++] = type;
   /* set date and time parameter length : 8 */
   pCidBuf [(*pPos)++] = CIDFSK_DATE_LEN;
   /* set month most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)month / 10) + '0';
   /* set month lest significant digit */
   pCidBuf [(*pPos)++] = (month % 10) + '0';
   /* set day most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)day / 10) + '0';
   /* set day lest significant digit */
   pCidBuf [(*pPos)++] = (day % 10) + '0';
   /* set hour most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)hour / 10) + '0';
   /* set hour lest significant digit */
   pCidBuf [(*pPos)++] = (hour % 10) + '0';
   /* set minute most significant digit */
   pCidBuf [(*pPos)++] = ((IFX_uint8_t)mn / 10) + '0';
   /* set minute lest significant digit */
   pCidBuf [(*pPos)++] = (mn % 10) + '0';
}

/*******************************************************************************
Description:
   Codes data of indicated type in CID Fsk buffer
Arguments:
   type    - type
   buf     - buffer with number information (octets)
   len     - buffer size in octets
   pCidBuf - cid data buffer
   pPos    - first data position in cid buffer
Return:
   None
*******************************************************************************/
IFX_LOCAL IFX_void_t cidfsk_codetype (TAPI_CID_SERVICE_TYPE_t type, IFX_uint8_t const *buf,
                                      IFX_uint8_t len, IFX_uint8_t *pCidBuf,
                                      IFX_uint8_t *pPos)
{
   IFX_uint8_t i, byte;
   IFX_boolean_t b_numcorrection = IFX_FALSE;

   switch (type)
   {
   case TAPI_CID_ST_CLI:
   case TAPI_CID_ST_CDLI:
   case TAPI_CID_ST_CCLI:
   case TAPI_CID_ST_FIRSTCLI:
   case TAPI_CID_ST_REDIR:
      b_numcorrection = IFX_TRUE;
      break;
   default:
      break;
   }

   /* set service type */
   pCidBuf [(*pPos)++] = type;
   /* set element length  */
   pCidBuf [(*pPos)++] = len;
   /* calling party name characters */
   for (i = 0; i < len; i++)
   {
      /* read number or character */
      byte = buf [i];
      /* to leave it  backward compatible, check if numbers instead of
         characters were passed for numbers */
      if ((b_numcorrection == IFX_TRUE) && (byte <= 9))
      {
         byte += '0';
      }
      pCidBuf [(*pPos)++] = byte;
   }
}

/*******************************************************************************
Description:
   Calculate checksum of given CID buffer
Arguments:
   pCidBuf  - CID Fsk buffer of which checksum will be calculated
   len      - len of FSK buffer
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   ref ETSI EN 300 659-1 V.1.3.1
   checksum calculation : two's complement of the modulo 256 sum
   of all the octets  in the message starting from the Message type octet
   up to the end of message (excluding checksum it self)
*******************************************************************************/
IFX_LOCAL IFX_uint8_t cidfsk_calccrc (IFX_uint8_t const *pCidBuf, IFX_uint8_t len)
{
   IFX_uint32_t sumCID = 0;
   IFX_uint8_t  i, crc;

   /* sum all octets */
   for (i = 0; i < len; i++)
   {
      sumCID += pCidBuf [i];
   }
   /* calculate two's complement as crc value */
   crc = ((~(sumCID & 0xFF)) + 1);

   return crc;
}

/*******************************************************************************
Description:
   Code CID parameters for FSK transmission according to chosen services.
   (ref: Spec ETSI EN 300 659-3.)
Arguments:
   pCidData       - handle to TAPI_CID_DATA structure
   pPhoneCid      - contains the caller id settings (TAPI_PHONE_CID)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   CID parameters buffer should be filled according to services.
   specification used : ETSI EN 300 659-3 V1.3.1 (2001-01)
*******************************************************************************/
IFX_LOCAL IFX_int32_t cidfsk_code (TAPI_CID_DATA *pCidData,
                                   TAPI_PHONE_CID const *pPhoneCid)
{
   IFX_uint8_t *pCidBuf = pCidData->cidParam,
               *pPos    = &pCidData->nCidParamLen, crc;
   IFX_int32_t ret = IFX_SUCCESS;

   /* set call setup message type : 0x80 */
   pCidBuf [0] = TAPI_CID_MT_CSUP;
   /* start filling data at position 2 of cid buffer */
   *pPos = 2;
   /* send date ? */
   if (pPhoneCid->service & CIDCS_DATE)
   {
      if (((pPhoneCid->month < 1) || (pPhoneCid->month > 12)) ||
          ((pPhoneCid->day   < 1) || (pPhoneCid->day   > 31)) ||
          (/*(pPhoneCid->hour  < 0) ||*/ (pPhoneCid->hour  > 23)) ||
          (/*(pPhoneCid->mn    < 0) ||*/ (pPhoneCid->mn   > 59)))
      {
         ret = IFX_ERROR;
      }
      if (ret == IFX_SUCCESS)
      {
         cidfsk_codedate(  TAPI_CID_ST_DATE,
                           pPhoneCid->month, pPhoneCid->day,
                           pPhoneCid->hour, pPhoneCid->mn,
                           pCidBuf, pPos );
      }
   }
   /* send name ? */
   if ((pPhoneCid->service & CIDCS_NAME) && (ret == IFX_SUCCESS))
   {
      if (pPhoneCid->namelen > 50)
      {
         ret = IFX_ERROR;
      }
      if (ret == IFX_SUCCESS)
      {
         /*cidfsk_codename (pPhoneCid, pCidBuf, pPos);*/
         cidfsk_codetype (TAPI_CID_ST_NAME,
                          (IFX_uint8_t const *)pPhoneCid->name,
                          pPhoneCid->namelen,
                          pCidBuf, pPos);
      }
   }
   /* CLI or Absence of CLI is mandatory */
   if (ret == IFX_SUCCESS)
   {
      if (pPhoneCid->service & CIDCS_ABSCLI)
      {
         if (pPhoneCid->numlen != 1)
         {
            ret = IFX_ERROR;
         }
         if (ret == IFX_SUCCESS)
         {
            /*cidfsk_codeabsence (pPhoneCid, pCidBuf, pPos);*/
            cidfsk_codetype (TAPI_CID_ST_ABSCLI,
                             (IFX_uint8_t const *)pPhoneCid->number, 1,
                             pCidBuf, pPos);
         }
      }
      else if (pPhoneCid->service & CIDCS_CLI)
      {
         if (((pPhoneCid->numlen < 1) || (pPhoneCid->numlen > 20)))
         {
            ret = IFX_ERROR;
         }
         if (ret == IFX_SUCCESS)
         {
            /*cidfsk_codenumber (pPhoneCid, pCidBuf, pPos);*/
            cidfsk_codetype (TAPI_CID_ST_CLI,
                             (IFX_uint8_t const *)pPhoneCid->number,
                             pPhoneCid->numlen, pCidBuf, pPos);
         }
      }
      else
         ret = IFX_ERROR;
   }
   /* redirect number ? */
   if ((pPhoneCid->service & CIDCS_REDIR) && (ret == IFX_SUCCESS))
   {
      if (((pPhoneCid->numrdno < 1) || (pPhoneCid->numrdno > 20)))
      {
         ret = IFX_ERROR;
      }
      if (ret == IFX_SUCCESS)
      {
         /*cidfsk_coderedirect (pPhoneCid, pCidBuf, pPos);*/
         cidfsk_codetype (TAPI_CID_ST_REDIR,
                          (IFX_uint8_t const *)pPhoneCid->redirno,
                          pPhoneCid->numrdno, pCidBuf, pPos);
      }
   }
   if (ret == IFX_SUCCESS)
   {
      /* set message length without  first 2 commands */
      pCidBuf [1] = *pPos - 2;
      /* set crc */
      crc = cidfsk_calccrc (pCidBuf, *pPos);
      pCidBuf [(*pPos)++] = crc;
   }

   /*CID_BUFFER_CHECK*/

   return ret;
}

/*******************************************************************************
Description:
   Code CID parameters for FSK transmission according to chosen services.
   (ref: Spec ETSI EN 300 659-3.)
Arguments:
   pCidData    - handle to TAPI_CID_DATA structure
   pCidInfo    - contains the caller id settings (TAPI_CID_INFO_t)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   CID parameters buffer should be filled according to services.
   specification used : ETSI EN 300 659-3 V1.3.1 (2001-01)
*******************************************************************************/
IFX_LOCAL IFX_int32_t cidfsk_code_new(TAPI_CID_DATA *pCidData,
                                      TAPI_CID_INFO_t const *pCidInfo)
{
   IFX_uint8_t *pCidBuf = pCidData->cidParam,
               *pPos    = &pCidData->nCidParamLen, crc;
   IFX_uint32_t i;
   IFX_int32_t ret = IFX_SUCCESS;

   /* message type */
   pCidBuf [0] = pCidInfo->messageType;
   /* start filling data at position 2 of cid buffer */
   *pPos = 2;

   for (i=0; i<pCidInfo->nMsgElements; i++)
   {
      TAPI_CID_MSG_ELEMENT_t* pMessage = pCidInfo->message + i;
      TAPI_CID_SERVICE_TYPE_t type = pMessage->string.elementType;
      IFX_uint32_t len = pMessage->string.len;

      switch (type)
      {
      /* Elements with special coding (date, length 8) */
      case TAPI_CID_ST_DATE:
      case TAPI_CID_ST_CDATE:
         cidfsk_codedate ( type,
            pMessage->date.month, pMessage->date.day,
            pMessage->date.hour, pMessage->date.mn,
            pCidBuf, pPos);
         break;
      /* Elements with length 1 */
      case TAPI_CID_ST_ABSCLI:
      case TAPI_CID_ST_ABSNAME:
      case TAPI_CID_ST_VISINDIC:
      case TAPI_CID_ST_CT:
      case TAPI_CID_ST_MSGNR:
      case TAPI_CID_ST_FWCT:
      case TAPI_CID_ST_USRT:
      case TAPI_CID_ST_SINFO:
         cidfsk_codetype ( type,
            (IFX_uint8_t*)&pMessage->value.element, 1, pCidBuf, pPos);
         break;
      /* Elements with variable size,
         do length limitation depending on type */
      case TAPI_CID_ST_MSGIDENT:
         LIMIT_VAL(len,3);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_DURATION:
         LIMIT_VAL(len,6);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_XOPUSE:
         LIMIT_VAL(len,10);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_CHARGE:
      case TAPI_CID_ST_ACHARGE:
         LIMIT_VAL(len,16);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_CLI:
      case TAPI_CID_ST_CDLI:
      case TAPI_CID_ST_LMSGCLI:
      case TAPI_CID_ST_CCLI:
      case TAPI_CID_ST_FIRSTCLI:
      case TAPI_CID_ST_REDIR:
      case TAPI_CID_ST_NTID:
      case TAPI_CID_ST_CARID:
         LIMIT_VAL(len,20);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_TERMSEL:
         LIMIT_VAL(len,21);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_NAME:
         LIMIT_VAL(len,50);
         goto code_string;
         /*lint -fallthrough */
      case TAPI_CID_ST_DISP:
      default:
      code_string:
         LIMIT_VAL(len,253);
         cidfsk_codetype ( type,
            pMessage->string.element,
            len,
            pCidBuf, pPos);
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

   /*CID_BUFFER_CHECK*/

   return ret;
}

/* ==================================== */
/* Local functions definition CID NTT */
/* ==================================== */

/*******************************************************************************
Description:
   Codes data of indicated type in CID Fsk NTT buffer
Arguments:
   type    - type
   buf     - buffer with number information (octets)
   len     - buffer size in octets
   pCidBuf - cid data buffer
   pPos    - first data position in cid buffer
Return:
   None
*******************************************************************************/
IFX_LOCAL IFX_void_t cidfskntt_codetype (IFX_uint8_t type, IFX_uint8_t const *buf,
                                         IFX_uint8_t len, IFX_uint8_t *pCidBuf,
                                         IFX_uint8_t *pPos)
{
   IFX_uint8_t i, byte;

   /* Parameter type */
   pCidBuf [(*pPos)++] = type;
   /* when data [001 0000] is outputted, DLE is added to the head
      of the data before output and is part of CRC calculation */
   if (len == CID_NTT_DLE)
   {
      pCidBuf [(*pPos)++] = CID_NTT_DLE;
      /* add this to message content length */
      pCidBuf [CID_NTT_MSGLEN_POS] += 1;
   }
   /* Data content length */
   pCidBuf [(*pPos)++] = len;
   /* Data content */
   for (i = 0; i < len; i++)
   {
      byte = buf [i];
      /* to leave it  backward compatible, check if numbers instead of
         characters were passed for numbers */
      if ((type != CID_NTT_PT_NAME) && (byte <= 9))
      {
         byte += '0';
      }
      pCidBuf [(*pPos)++] = byte;
   }
   /* Message content length */
   pCidBuf [CID_NTT_MSGLEN_POS] += (2 + len);
}

/*******************************************************************************
Description:
   Formats a byte according to data format specified in NTT spec
Arguments:
   pChannel -  handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   1- The byte to format is assumed to be represented in format <b7b6b5b4b3b2b1>.
      After transformation, it will be <b1b2b3b4b5b6b7bp>.
      Therefore the parity bit must be calculated and added to every byte as MSBit,
      afterwards the byte must be swapped as follows :

           <b7b6b5b4b3b2b1> ==> <b1b2b3b4b5b6b7bp>.

   2- refer to NTT Specification, Edition 5
*******************************************************************************/
IFX_LOCAL IFX_void_t  cidfskntt_reversebyte (IFX_uint8_t *byte)
{
   IFX_uint8_t i, tmpByteSwap = 0, tmpByte = *byte;

   /* swap b8...b1 into b1...b8 */
   for (i = 0; i < 8; i++)
   {
      if (tmpByte & 0x1)
      {
         tmpByteSwap |= (0x80 >> i);
      }
      tmpByte >>= 1;
   }

   *byte = tmpByteSwap;
}

/*******************************************************************************
Description:
   Checks even parity and add parity bit as MSBit if applicable.
Arguments:
   byte2format -  byte to be formatted
Return:
   formatted byte with added parity
Remarks:
   1- The byte to format is assumed to be represented in format <b7b6b5b4b3b2b1>.
      After transformation, it will be <bpb7b6b5b4b3b2b1>.

   2- refer to NTT Specification, Edition 5
*******************************************************************************/
IFX_LOCAL IFX_void_t  cidfskntt_setparity (IFX_uint8_t *byte)
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
   /* set parity bit as MSB in case */
   if (ones_num %2 != 0)
      *byte |= 0x80;
}

/*******************************************************************************
Description:
   NTT FSK encoding.
Arguments:
   pChannel -  handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   1- The CRC16 for NTT FSK Buffer is generated with the polynom
      X^16 + X^12 + X^5 + X^2 + X^0 (Hexadecimal: 0x1021)
   2- The CRC calculation is quite complicated.
      The algorithm is:
      a)initial value of CRC = 0x0000.
      b)for each byte (from header to ETX)
            do reverse byte
            compute the CRC with G(x) (CRC1)
            CRC= CRC + CRC1
*******************************************************************************/
IFX_LOCAL IFX_uint16_t cidfskntt_calccrc (IFX_uint8_t const *pCidBuf, IFX_uint8_t len)
{
   IFX_uint16_t crc = 0, tmp;
   IFX_uint8_t i, j, byte;

   for (i = 0; i < len; i++)
   {
      /* get reversed data byte */
      byte = pCidBuf [i];
      cidfskntt_reversebyte (&byte);
      tmp   = (byte << 7);
      for (j = 0; j < 8; j++)
      {
         tmp  <<= 1;
         if ((tmp ^ crc) & 0x8000)
            crc = (crc << 1) ^ 0x1021;
         else
            crc <<= 1;
      }
   }
   /*PRINTF ("CRC16 = 0x%04X\r\n", crc);*/

   return crc;
}

/*******************************************************************************
Description:
   NTT FSK encoding.
Arguments:
   pCidData    - handle to TAPI_CID_DATA  structure
   pPhoneCid   - handle to TAPI_PHONE_CID structure
Return:
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
*******************************************************************************/
IFX_LOCAL IFX_int32_t  cidfskntt_code (TAPI_CID_DATA *pCidData,
                                       TAPI_PHONE_CID const *pPhoneCid)
{
   IFX_uint16_t      crc;
   IFX_uint8_t       *pCidBuf = pCidData->cidParam,
                     *pPos    = &pCidData->nCidParamLen, i, crc_h, crc_l;
   const IFX_uint8_t pNttHead [7] = {CID_NTT_DLE, CID_NTT_SOH, CID_NTT_HEADER,
                               CID_NTT_DLE, CID_NTT_STX, CID_NTT_TYPE1, 0},
                     pNttTail [2] = {CID_NTT_DLE, CID_NTT_ETX};

   if ((pPhoneCid->numlen  > CID_NTT_NUMBER_MAXLEN) ||
       (pPhoneCid->namelen > CID_NTT_NAME_MAXLEN)   ||
       ((pPhoneCid->numlen == 0) && (pPhoneCid->namelen == 0)))
   {
      return IFX_ERROR;
   }
   /* set head according to CID type */
   *pPos  = sizeof (pNttHead);
   memcpy (pCidBuf, (IFX_uint8_t*)pNttHead, *pPos);
   /* in case of type 2, overwrite type parameter */
   if (pCidData->nCidType == TAPI_CID_TYPE2)
   {
      pCidBuf [CID_NTT_TYPE_POS] = CID_NTT_TYPE2;
   }
   /* check if number is to encode */
   if ((pPhoneCid->service & CIDCS_CLI) && (pPhoneCid->numlen != 0))
   {
      cidfskntt_codetype (CID_NTT_PT_NUMBER,
                          (IFX_uint8_t const *)pPhoneCid->number,
                          pPhoneCid->numlen, pCidBuf, pPos);
   }
   /* check if name is to encode */
   if ((pPhoneCid->service & CIDCS_NAME) && (pPhoneCid->namelen != 0))
   {
      cidfskntt_codetype (CID_NTT_PT_NAME, (IFX_uint8_t const *)pPhoneCid->name,
                          pPhoneCid->namelen, pCidBuf, pPos);
   }
   /* set tail */
   memcpy (&pCidBuf[*pPos], (IFX_uint8_t*)pNttTail, sizeof (pNttTail));
   *pPos  += sizeof (pNttTail);
   /* set bytes parity */
   for (i = 0; i < *pPos; i++)
   {
      cidfskntt_setparity (&pCidBuf [i]);
   }
   /* calculate crc from HEADER to ETX */
   crc = cidfskntt_calccrc (&pCidBuf[2], (*pPos - 2));
   /* add reversed CRC-High to buffer */
   crc_h = ((crc >> 8) & 0xFF);
   cidfskntt_reversebyte(&crc_h);
   pCidBuf [(*pPos)++] = crc_h;
   /* add reversed CRC-Low to buffer */
   crc_l = (crc & 0xFF);
   cidfskntt_reversebyte (&crc_l);
   pCidBuf [(*pPos)++] = crc_l;

   /*CID_BUFFER_CHECK*/

   return IFX_SUCCESS;
}

/* =============================== */
/* Local functions definition CID2 */
/* =============================== */

/*******************************************************************************
Description:
   Mutes/Restores Voice talking and listening path during a CID2 processing.
Arguments:
   bMute    - IFX_TRUE = mute voice / IFX_FALSE = restore voice
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   !!! Actually, the Voice path is disabled for muting. This is not allowed for
   conferencing. When available, suitable routines also for conferencing will
   be used here.
*******************************************************************************/
IFX_LOCAL IFX_int32_t cid2_mutevoice (TAPI_CONNECTION *pChannel, IFX_boolean_t bMute)
{
   IFX_int32_t ret;

   if (bMute == IFX_FALSE)
   {
#ifdef TAPI_VOICE
      ret = TAPI_LL_Phone_Start_Recording (pChannel);
      if (ret == IFX_SUCCESS)
         ret = TAPI_LL_Phone_Start_Playing (pChannel);
#endif /* TAPI_VOICE */
   }
   else
   {
      pChannel->TapiCid2.state = TAPI_CID2_MUTE_VOICE;
#ifdef TAPI_VOICE
      ret = TAPI_LL_Phone_Stop_Recording (pChannel);
      if (ret == IFX_SUCCESS)
         TAPI_LL_Phone_Stop_Playing (pChannel);
#endif /* TAPI_VOICE */
   }

   return ret;
}

/*******************************************************************************
Description:
   CID2 timer.
Arguments:
   Timer    - timer id
   nArg     - timer callback argument
Return:
   none
*******************************************************************************/
IFX_LOCAL IFX_void_t cid2_timercallback (Timer_ID Timer, IFX_int32_t nArg)
{
   TAPI_CONNECTION *pChannel  = (TAPI_CONNECTION *) nArg;
   TAPI_CID2 *pCid2 = &pChannel->TapiCid2;
   IFX_int32_t ret = IFX_ERROR;
   IFX_boolean_t cleanUp = IFX_FALSE;

   /*PRINTF(".");*/
   switch (pCid2->state)
   {
   case  TAPI_CID2_CHECK_ACK:
      /*PRINTF ("?");*/
      if (pCid2->bAck == IFX_TRUE)
      {
         /* reset acknowledge flag */
         pCid2->bAck = IFX_FALSE;
         /* PRINTF ("!\n\r"); */
         /* Set line mode to normal. For CID transmission, this
            depends on platform. Low level takes care. */
         ret = TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
         if (ret == IFX_SUCCESS)
         {
            pCid2->state = TAPI_CID2_SEND_FSK;
            TAPI_SetTime_Timer (Timer, pCid2->conf.std.defaultS.ack2fskOutTime,
                                IFX_FALSE, IFX_FALSE);
         }
      }
      if (ret == IFX_ERROR)
      {
         /*PRINTF ("x\n\r");*/
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: No ACK in Time\n\r"));
         cleanUp = IFX_TRUE;
      }
      break;
   case  TAPI_CID2_SEND_FSK:
      /*PRINTF ("->\n\r");*/
      /* for nttS, additional time must be waited after CID is sent out */
      if (pCid2->conf.stdSelect == TAPI_CID2_NTT)
      {
         pChannel->TapiCidTx.nAdditionalTime = pCid2->conf.std.nttS.fskOut2restoreTime;
      }
      ret = TAPI_Phone_CID_Send (pChannel, Timer);
      if (ret == IFX_SUCCESS)
      {
         pCid2->state = TAPI_CID2_RESTORE_VOICE;
      }
      else
      {
         /*PRINTF ("x\n\r");*/
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: FSK transmission"
                                          " setup error\n\r"));
         cleanUp = IFX_TRUE;
      }
      break;
   case TAPI_CID2_RESTORE_VOICE:
   default:
      /*PRINTF ("x\n\r");*/
      cleanUp = IFX_TRUE;
      break;
   }
   /* in case of error or end of process, restore line as was before CID2
      and kill timer */
   if (cleanUp == IFX_TRUE)
   {
      /* set back line mode for voice */
      pChannel->TapiCidTx.bIsNewData = IFX_FALSE;
      TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
      /* reenable talking and listening voice path */
      cid2_mutevoice (pChannel, IFX_FALSE);
      /* CID2 is finished */
      pCid2->bCid2IsOn = IFX_FALSE;
      /* delete timer */
      ret = TAPI_Delete_Timer(Timer);
      if (ret)
      {
         /* delete cid2 timer id */
         pCid2->Cid2TimerID = 0;
      }
      else
      {
         TRACE (TAPI_DRV,DBG_LEVEL_HIGH,("\n\rDRV_ERROR: could not delete "
             "timer for ringing\n\r"));
      }
   }
}

/*******************************************************************************
Description:
   send CID2 according to default standards (TIA, ETSI, BT).
Arguments:
   pChannel    - handle to TAPI_CONNECTION  structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_LOCAL IFX_int32_t cid2default_start (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_SUCCESS;
   TAPI_CID2   *pCid2 = &pChannel->TapiCid2;

   /* send Alert Signal */
   ret = TAPI_LL_Phone_CID_Send_AS (pChannel);
   if (ret == IFX_SUCCESS)
   {
      /* tone was sent out. Set timer for time between alert signal and
        acknowledge occurrence. Check for acknowledge after
        AS duration + Cas2Ack time */
      TAPI_SetTime_Timer (pCid2->Cid2TimerID,
                         (pCid2->conf.std.defaultS.casDuration +
                          pCid2->conf.std.defaultS.cas2ackTime),
                          IFX_FALSE, IFX_FALSE);
      /* mute Voice : talking and listening paths */
      ret = cid2_mutevoice (pChannel, IFX_TRUE);
      /* if muting was ok expect ACK. Otherwise, cleanup at timer occurrence */
      if (ret == IFX_SUCCESS)
         pCid2->state = TAPI_CID2_CHECK_ACK;
   }
   if (ret != IFX_SUCCESS)
      pCid2->bCid2IsOn = IFX_FALSE;

   return ret;
}

/*******************************************************************************
Description:
   send CID2 according to NTT standard.
Arguments:
   pChannel    - handle to TAPI_CONNECTION  structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_LOCAL IFX_int32_t cid2ntt_start (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret = IFX_SUCCESS;
   TAPI_CID2   *pCid2 = &pChannel->TapiCid2;

   ret = cid2_mutevoice (pChannel, IFX_TRUE);
   /* send Alert Signal */
   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_LL_Phone_CID_Send_AS (pChannel);
   }
   if (ret == IFX_SUCCESS)
   {
      /* Tone wat sent out. Programm the timer to send CID at next occurrence */
      TAPI_SetTime_Timer (pCid2->Cid2TimerID, pCid2->conf.std.nttS.at2fskOutTime,
                          IFX_FALSE, IFX_FALSE);
       /* As line mode for FSK transmission depends on SLIC, low level
          should now switch to a suitable line mode for FSK transmission */
      pCid2->state = TAPI_CID2_SEND_FSK;
      ret = TAPI_Phone_Set_Linefeed (pChannel, TAPI_LINEFEED_NORMAL);
   }
   if (ret != IFX_SUCCESS)
      pCid2->bCid2IsOn = IFX_FALSE;

   return ret;
}

/* ============================================== */
/* Global functions definition CID data encoding  */
/* ============================================== */

/*******************************************************************************
Description:
   Processes user CID data according to chosen service and cid sending type
   (FSK, DTMF)
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   pPhoneCid      - contains the caller id settings (TAPI_PHONE_CID)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   - If a non supported service is set, error will be returned.
   - This implementation complies with ETSI standards
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_SetData (TAPI_CONNECTION *pChannel,
                                    TAPI_PHONE_CID const *pPhoneCid)
{
   IFX_int32_t  ret = IFX_SUCCESS;
   IFX_uint32_t i, services_ns;

   /* check if a service isn't supported. CLI is always supported */
   services_ns = pPhoneCid->service & ~(CIDCS_CLI | CIDCS_NAME | CIDCS_DATE |
                 CIDCS_REDIR | CIDCS_ABSCLI | CIDCS_INFO | CID_TRANSPARENT);
   if (services_ns != 0)
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: CID Services 0x%08lX"
          " not yet supported\n\r", services_ns));
      return IFX_ERROR;
   }
   /* either CLI or absence of CLI is mandatory */
   if (!(pPhoneCid->service & (CIDCS_CLI | CIDCS_ABSCLI)))
   {
      TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
            ("\n\rDRV_ERROR: either CLI or ABSCLI is mandatory\n\r"));
      return IFX_ERROR;
   }
   /* no data interpretation/encoding is done.
      Just copy data to internal buffer */
   if (pPhoneCid->service & CID_TRANSPARENT)
   {
      /* check buffer length */
      if (pPhoneCid->datalen > TAPI_MAX_CID_PARAMETER)
      {
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Maximun Cid data"
             " exeeded. Processing aborted\n\r"));
         return IFX_ERROR;
      }
      /* check buffer */
      if (pPhoneCid->pData == NULL)
      {
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Invalid tranparent "
             " Cid buffer. Processing aborted\n\r"));
         return IFX_ERROR;
      }
      /* just unmap data without interpretation for FSK and DTMF mode */
      for (i = 0; i < pPhoneCid->datalen; i++)
      {
         pChannel->TapiCidTx.cidParam [i] = (IFX_uint8_t)pPhoneCid->pData [i];
      }
      /* set length */
      pChannel->TapiCidTx.nCidParamLen = (IFX_uint8_t)pPhoneCid->datalen;
   }
   else
   {
      /* code FSK CID data */
      if (pChannel->TapiCidConfig.nMode == TAPI_CID_FSK_MODE)
      {
         if (pChannel->TapiCidConfig.nSpec == TAPI_CID_NTT)
         {
            /* code cid according to NTT specification */
            ret = cidfskntt_code (&pChannel->TapiCidTx, pPhoneCid);
         }
         else
         {
            /* Code CID according to default standard (ETSI) */
            ret = cidfsk_code (&pChannel->TapiCidTx, pPhoneCid);
         }
      }
      /* Code DTMF CID data */
      else
      {
         ret = ciddtmf_code (pChannel, pPhoneCid);
      }
   }
   /* save additional important data and reset CID data ptr */
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiCidTx.nCidTxState   = CID_TX_NONE;
      pChannel->TapiCidTx.bIsNewData    = IFX_TRUE;
      pChannel->TapiCidTx.nBurstCnt     = 0;
      pChannel->TapiCidTx.bCidAlertEvt  = IFX_FALSE;
      /* CID_BUFFER_CHECK */
   }

   return ret;
}

/* ========================================== */
/* CID Type 1 global functions (FSK and DTMF) */
/* ========================================== */

/*******************************************************************************
Description:
   Configures caller id transmitter
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pCidConfig  - contains the settings for cid configuration (TAPI_CID_CONFIG)
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   If Cid generation with FSL is chosen, low level configuration of
   CID transmitter willö be done.
   For CID generation with DTMF, it is assumed that a previous configuration
   with IFXPHONE_DTMFCID_CONF was done, otherwise, default parameters will be used.

   If there are CID data to be transmitted according to schema
    <ringing><pause_before_cid><cid_sending><pause_after_cid><ringing>
   following should apply (ref : ETSI EN 300 659-1 V1.3.1 (2001-01):
      - 500 ms <= pause_before_cid < 2000 ms
      - pause_after_cid >= 200 ms
   so a check of minimal time for CID will be done in this function.
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_Config (TAPI_CONNECTION *pChannel,
                                   TAPI_CID_CONFIG const *pCidConfig)
{
   IFX_int32_t ret = IFX_SUCCESS;

   /* This line shouldn't be ringing or sending cid 1 because
      cid configuration is affected here */
   if (pChannel->TapiRingData.bRingingMode == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Line is ringing\n\r"));
      return IFX_ERROR;
   }
   /* check if delay can be consumed with the periodical 50 ms heart beat timer */
   if ((pCidConfig->nAppearance == CID_BETWEEN_RINGBURST) &&
       (pCidConfig->nDelay % 50 != 0))
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Delay after 1. "
          " ring burst should be a multiple of 50 ms, period of high "
          " resolution cadence timer\n\r"));
      return IFX_ERROR;
   }
   else if ((pCidConfig->nMode == TAPI_CID_FSK_MODE) &&
            (pCidConfig->nDelay < TAPI_MIN_TIME_BEFORE_CID))
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Delay before "
          "CID transmission should be at least 500 ms. "
          "(ref ETSI EN 300 659-1 V1.3.1 -> RP-AS)\n\r"));
      return IFX_ERROR;
   }
   /* do configuration of CID transmitter */
   if (pCidConfig->nMode == TAPI_CID_FSK_MODE)
   {
      ret = TAPI_LL_Phone_CID_ConfFsk (pChannel, pCidConfig);
   }
   if (ret == IFX_SUCCESS)
   {
      /* begin of protected area */
      IFXOS_MutexLock(pChannel->TapiDataLock);
      /* save settings in TAPI structure */
      pChannel->TapiCidConfig = *pCidConfig;
      /* set CID type */
      pChannel->TapiCidTx.nCidType = TAPI_CID_TYPE1;
      /* initialize transmit state */
      pChannel->TapiCidTx.nCidTxState = CID_TX_NONE;
      /* additional time is 0*/
      pChannel->TapiCidTx.nAdditionalTime = 0;
      if (pChannel->TapiCidConfig.nBurstCount == 0)
      {
         if (pChannel->TapiCidConfig.nSpec == TAPI_CID_NTT)
         {
            pChannel->TapiCidConfig.nBurstCount = 5;
         }
         else if (pCidConfig->nAppearance == CID_BETWEEN_RINGBURST)
         {
            pChannel->TapiCidConfig.nBurstCount = 1;
         }
      }
      /* end of protected area */
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
   }

   return ret;
}

/*******************************************************************************
Description:
   Get caller id configuration
Arguments:
   pChannel      - handle to TAPI_CONNECTION structure
   pCidConfig    - handle to CID configuration structure (TAPI_CID_CONFIG)
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_GetConfig (TAPI_CONNECTION *pChannel,
                                      TAPI_CID_CONFIG *pCidConfig)
{
   /* get settings from TAPI structure */
   *pCidConfig = pChannel->TapiCidConfig;

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Do Dtmf caller id configuration
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pDtmfCid    - handle to DTMF CID structure (TAPI_DTMFCID)
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   Some parameters of the Interface structure are still unclear
   (i.e toneTime, sasTime, ackTone, infoStartTone )
   ! Function to be completed later. Still under construction.
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_ConfigDtmf (TAPI_CONNECTION *pChannel,
                                       TAPI_DTMFCID const *pDtmfCid)
{
   IFX_int32_t ret;

   /* This line shouldn't be ringing or sending cid 1 because
      cid configuration is affected here */
   if (pChannel->TapiRingData.bRingingMode == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Line is ringing\n\r"));
      return IFX_ERROR;
   }
   /* call low level function to do chip related configuration */
   ret = TAPI_LL_Phone_CID_ConfDtmf (pChannel, pDtmfCid);
   if (ret == IFX_SUCCESS)
   {
      /* begin of protected area */
      IFXOS_MutexLock(pChannel->TapiDataLock);
      /* cache user data */
      pChannel->TapiDtmfCidConf = *pDtmfCid;
      /* end of protected area */
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
   }

   return ret;
}

/*******************************************************************************
Description:
   Send CID (FSK or DTMF)
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   Timer       - id of timer which is triggering this state machine
   nAddTime    - additional time to add on estimated cid sending time
                 before next timer operation.
Return:
   IFX_SUCCESS or IFX_ERROR
Remark:
   This function is used in ringing module for example.
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_Send (TAPI_CONNECTION *pChannel, Timer_ID Timer)
{
   IFX_int32_t      ret = IFX_SUCCESS;
   IFX_uint32_t     nTime = TAPI_MIN_TIME_AFTER_CID;
   TAPI_CID_CONFIG *pCidConf = &pChannel->TapiCidConfig;
   TAPI_DTMFCID    *pDtmfCid = &pChannel->TapiDtmfCidConf;
   TAPI_CID_DATA   *pCidData = &pChannel->TapiCidTx;

   if (Timer != NULL)
   {
      /* Because the CID sending isn't blocking, calculate estimated time for
         the whole CID buffer to be processed */
      switch (pCidConf->nMode)
      {
      case TAPI_CID_FSK_MODE:
         nTime += (TAPI_CID_BYTE_TXTIME_MS * pCidData->nCidParamLen);
         break;
      case TAPI_CID_DTMF_MODE:
         nTime +=
            ((pDtmfCid->digitTime + pDtmfCid->interDigitTime) * pCidData->nCidParamLen);
         break;
      default:
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Unknown mode\n\r"));
         return IFX_ERROR;
      }
      /* check additional delay time if applicable */
      if (pCidData->nAdditionalTime > nTime)
      {
         /* Delay time is sufficient for sending cid. use it.
            Otherwise, use time calculated which ensures a CID transmission */
         nTime = pCidData->nAdditionalTime;
         /* reset additional time. caller must set it new if necessary */
         pCidData->nAdditionalTime = 0;
         /* schedule timer now if valid */
         TAPI_SetTime_Timer(Timer, nTime, IFX_FALSE, IFX_FALSE);
      }
      else
      {
         /* configured cadence pause time not sufficient for CID transmission */
         TAPI_Error_Event_Update (pChannel, TAPI_ERROR_RINGCADENCE_CIDTX);
         ret = IFX_ERROR;
      }
   }
   /* ask low level to send data if no previous error.
      It is assumed that the CID data were prepared already before
      calling this routine */
   /* P.S : The function is not blocking and returns immediately */
   if (ret == IFX_SUCCESS)
      ret = TAPI_LL_Phone_CID_Sending (pChannel, pCidData);

   return ret;
}

/*******************************************************************************
Description:
   CID 1 operation control
Arguments:
   pChannel -  handle to
Return:

Remark:

*******************************************************************************/
IFX_int32_t TAPI_Phone_CID1_OpControl (TAPI_CONNECTION *pChannel,
                                 IFX_int32_t nOperation, Timer_ID timerId,
                                 IFX_int32_t (*alert)(TAPI_CONNECTION*),
                                 IFX_int32_t (*cidtxend)(TAPI_CONNECTION*))
{
   IFX_int32_t ret = IFX_ERROR;

   switch (nOperation)
   {
   case  TAPI_CID1_ALERT:
      /* if something must be done for alerting, do it now */
      if (alert != NULL)
         ret = alert (pChannel);
      break;
   case  TAPI_CID1_SEND:
      /* send CID data if */
      if (pChannel->TapiCidTx.bIsNewData == IFX_TRUE)
      {
         /* only one trial */
         pChannel->TapiCidTx.bIsNewData = IFX_FALSE;
         /* Pass timer id and send CID .
            Note: If the timer is valid, it will be reschedule within
                  this function
         */
         ret = TAPI_Phone_CID_Send (pChannel, timerId);
      }
      else
      {
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_WARN: No Cid to send! \n\r"));
      }
      break;
   case  TAPI_CID1_END:
      if (pChannel->TapiCidTx.nCidTxState != CID_TX_END)
      {
         LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_WARN: Cid transmission "
          "not finished at timer new occurance ! \n\r"));
      }
      /* */
      if (cidtxend != NULL)
      {
         ret = cidtxend (pChannel);
      }
      break;
   default:
      break;
   }

   return ret;
}


/*******************************************************************************
Description:
   Processes user CID data according to chosen service and cid sending type
   (FSK, DTMF) and send it out directly.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pCidInfo   - contains the caller id settings (TAPI_CID_INFO_t)
Return:
   IFX_SUCCESS / IFX_ERROR
Remark:
   - If a non supported service is set, error will be returned.
   - This implementation complies with ETSI standards
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID_Info_Tx( TAPI_CONNECTION *pChannel,
                                    TAPI_CID_INFO_t const *pCidInfo)
{
   IFX_int32_t    ret = IFX_SUCCESS;
   IFX_uint32_t   i;
   TAPI_CID_MSG_ELEMENT_t* pMessage = pCidInfo->message;
   IFX_boolean_t mandatory = IFX_FALSE;

   for (i=0; i<pCidInfo->nMsgElements; i++)
   {
      switch ((pMessage+i)->string.elementType)
      {
      case TAPI_CID_ST_CLI:
      case TAPI_CID_ST_ABSCLI:
         if (pCidInfo->messageType==TAPI_CID_MT_CSUP)
            mandatory = IFX_TRUE;
         break;
      case TAPI_CID_ST_VISINDIC:
         if (pCidInfo->messageType==TAPI_CID_MT_MWI)
            mandatory = IFX_TRUE;
         break;
      case TAPI_CID_ST_CHARGE:
         if (pCidInfo->messageType==TAPI_CID_MT_AOC)
            mandatory = IFX_TRUE;
         break;
      case TAPI_CID_ST_DISP:
         if (pCidInfo->messageType==TAPI_CID_MT_SMS)
            mandatory = IFX_TRUE;
         break;
      default:
         break;
      }
      if (mandatory == IFX_TRUE)
         break;
   } /* for () */

   if (mandatory != IFX_TRUE)
      return IFX_ERROR;

   /* code FSK CID data */
   if (pChannel->TapiCidConfig.nMode == TAPI_CID_FSK_MODE)
   {
      if (pChannel->TapiCidConfig.nSpec == TAPI_CID_NTT)
      {
         /* code cid according to NTT specification */
         ret = IFX_ERROR /*cidfskntt_code (&pChannel->TapiCidTx, pPhoneCid)*/;
      }
      else
      {
         /* Code CID according to default standard (ETSI) */
         ret = cidfsk_code_new (&pChannel->TapiCidTx, pCidInfo);
      }
   }
   /* Code DTMF CID data */
   else
   {
      ret = ciddtmf_code_new (pChannel, pCidInfo);
   }

   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_LL_Phone_CID_Sending (pChannel, &pChannel->TapiCidTx);
   }

   return ret;
}


/* ============================= */
/* CID Type 2 global functions   */
/* ============================= */

/*******************************************************************************
Description:
   CID2 Configuration .
Arguments:
   pChannel    - handle to TAPI_CONNECTION  structure
   pCid2Conf   - handle to TAPI_CID2_CONFIG structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID2_Config (TAPI_CONNECTION  *pChannel,
                                    TAPI_CID2_CONFIG *pCid2Conf)
{
   IFX_int32_t  ret;
   IFX_int32_t  nAckTone;
   TAPI_CID2   *pCid2 = &pChannel->TapiCid2;

   if (pChannel->TapiCid2.bCid2IsOn == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: CID2 actually running on"
            " this channel\n\r"));
      return IFX_ERROR;
   }
   /* This line shouldn't be ringing or sending cid 1 because
      cid configuration is affected here */
   if (pChannel->TapiRingData.bRingingMode == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Line is ringing\n\r"));
      return IFX_ERROR;
   }
   /* high level CID generation mode */
   pChannel->TapiCidConfig.nHlev = TAPI_CID_HIGHLEVEL;
   /* set ITU V23 spec */
   pChannel->TapiCidConfig.nSpec = TAPI_CID_V23;
   /* handle cases now */
   switch (pCid2Conf->stdSelect)
   {
   case TAPI_CID2_TIA :
   case TAPI_CID2_ETSI:
   case TAPI_CID2_BT  :
      /* check if low level can encode ack tone */
      nAckTone = (IFX_int32_t)TAPI_LL_Phone_Dtmf_GetCode (pCid2Conf->std.defaultS.ackTone);
      if (nAckTone == IFX_ERROR)
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Invalid ACK Tone\n\r"));
         return IFX_ERROR;
      }
      /* check cas 2 ack time */
      if ((pCid2Conf->std.defaultS.cas2ackTime < TAPI_CID2_MINTIME_CAS2ACK) ||
          (pCid2Conf->std.defaultS.cas2ackTime > TAPI_CID2_MAXTIME_CAS2ACK))
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: CAS 2 ACK Time"
             " is out of limits\n\r"));
         return IFX_ERROR;
      }
      /* check ack 2 fskOut time */
      if ((pCid2Conf->std.defaultS.ack2fskOutTime < TAPI_CID2_MINTIME_ACK2FSKOUT) ||
          (pCid2Conf->std.defaultS.ack2fskOutTime > TAPI_CID2_MAXTIME_ACK2FSKOUT))
      {
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: ACK 2 FSKOUT Time"
             " is out of limit\n\r"));
         return IFX_ERROR;
      }
      /* set appropriate default for USA TIA specification : USA is bellcore */
      if (pCid2Conf->stdSelect == TAPI_CID2_TIA)
      {
         pChannel->TapiCidConfig.nSpec = TAPI_CID_BELLCORE;
      }
      /* save settings in TAPI structure */
      pCid2->conf.std.defaultS = pCid2Conf->std.defaultS;
      /* check cas duration */
      if (pCid2Conf->std.defaultS.casDuration == 0)
      {
         pCid2Conf->std.defaultS.casDuration  = TAPI_CID2_CASDURATION_DEFAULT;
      }
      /* overwrite acktone according to low level encoding */
      pCid2->conf.std.defaultS.ackTone = (IFX_uint8_t)nAckTone;
      /* save the ascii ack tone to restore it in case get config is done */
      pCid2->AckAscii = pCid2Conf->std.defaultS.ackTone;
      break;
   case TAPI_CID2_NTT:
      /* set appropriate specification in CID configuration
         structure for correct FSK encoding */
      pChannel->TapiCidConfig.nSpec = TAPI_CID_NTT;
      /* save settings in TAPI structure */
      pCid2->conf.std.nttS = pCid2Conf->std.nttS;
      if (pCid2->conf.std.nttS.at2fskOutTime == 0)
      {
         pCid2->conf.std.nttS.at2fskOutTime =
                                             TAPI_CID2_DEFAULTTIME_AT2FSKOUT;
      }
      if (pCid2->conf.std.nttS.fskOut2restoreTime == 0)
      {
         pCid2->conf.std.nttS.fskOut2restoreTime =
                                             TAPI_CID2_DEFAULTTIME_FSKOUT2VOICE;
      }
      break;
   default:
      /* other standards not supported yet */
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Unsupported standard\n\r"));
      return IFX_ERROR;
   }
   /* do appropriate default low level configuration */
   ret = TAPI_LL_Phone_CID_ConfFsk (pChannel, &pChannel->TapiCidConfig);
   /* do rest of settings */
   if (ret == IFX_SUCCESS)
   {
      /* sending mode is FSK */
      pChannel->TapiCidConfig.nMode = TAPI_CID_FSK_MODE;
      /* CID mode is type 2 */
      pChannel->TapiCidTx.nCidType = TAPI_CID_TYPE2;
      /* additional time is 0*/
      pChannel->TapiCidTx.nAdditionalTime = 0;
      /* set standard selected now */
      pCid2->conf.stdSelect = pCid2Conf->stdSelect;
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Read CID2 Configuration
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pPhoneCid   - handle to TAPI_PHONE_CID structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID2_GetConfig (TAPI_CONNECTION *pChannel,
                                 TAPI_CID2_CONFIG *pCid2Conf)
{
   TAPI_CID2 *pCid2 = &pChannel->TapiCid2;

   /* read standard applied */
   pCid2Conf->stdSelect = pCid2->conf.stdSelect;
   switch (pCid2->conf.stdSelect)
   {
   case TAPI_CID2_TIA :
   case TAPI_CID2_ETSI:
   case TAPI_CID2_BT  :
      /* read settings in TAPI structure */
      pCid2Conf->std.defaultS = pCid2->conf.std.defaultS;
      /* update ack tone also */
      pCid2Conf->std.defaultS.ackTone = pCid2->AckAscii;
      break;
   case TAPI_CID2_NTT:
      /* read settings in TAPI structure */
      pCid2Conf->std.nttS = pCid2->conf.std.nttS;
      break;
   default:
      /* other standards not supported yet */
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Unsupported standard\n\r"));
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Send CID type 2
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pPhoneCid   - handle to TAPI_PHONE_CID structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_CID2_Send (TAPI_CONNECTION *pChannel, TAPI_PHONE_CID const *pPhoneCid)
{
   IFX_int32_t  ret = IFX_SUCCESS;
   TAPI_CID2   *pCid2 = &pChannel->TapiCid2;

   /* This line shouldn't be ringing or sending cid 1 */
   if (pChannel->TapiRingData.bRingingMode == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Line is ringing\n\r"));
      return IFX_ERROR;
   }
   /* check if a processing is ongoing */
   if (pCid2->bCid2IsOn == IFX_TRUE)
   {
      TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: CID2 actually running on"
            " this channel\n\r"));
      return IFX_ERROR;
   }
   /* check line mode. It is assumed that the line mode is normal and
      that a voice connection could be ongoing now
    */
   if (pChannel->TapiOpControlData.nLineMode != TAPI_LINEFEED_NORMAL)
   {
      LOG(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Wrong Line mode\n\r"));
      return IFX_ERROR;
   }
   /* create the timer for cid2 flow */
   pCid2->Cid2TimerID = TAPI_Create_Timer((TIMER_ENTRY)cid2_timercallback,
                                          (IFX_int32_t)pChannel);
   if (pCid2->Cid2TimerID == 0)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\n\rDRV_ERROR: could not create"
         " timer for CID 2 sending\n\r"));
      ret = IFX_ERROR;
   }
   /* Prepare CID FSK data */
   if (ret == IFX_SUCCESS)
   {
      ret = TAPI_Phone_CID_SetData (pChannel, pPhoneCid);
   }
   if (ret == IFX_SUCCESS)
   {
      /* set flag now to avoid change of configuration */
      pCid2->bCid2IsOn = IFX_TRUE;
      /* Send FSK CID according to CID2 path */
      switch (pCid2->conf.stdSelect)
      {
      case TAPI_CID2_TIA :
      case TAPI_CID2_ETSI:
      case TAPI_CID2_BT  :
         ret = cid2default_start (pChannel);
         break;
      case TAPI_CID2_NTT:
         ret = cid2ntt_start (pChannel);
         break;
      default:
         /* other standards not supported yet */
         TRACE(TAPI_DRV, DBG_LEVEL_HIGH, ("\n\rDRV_ERROR: Unsupported standard\n\r"));
         ret = IFX_ERROR;
         break;
      }
   }

   return ret;
}

/* ============================= */
/* CID Receiver global functions */
/* ============================= */

/*******************************************************************************
Description:
   Starts caller id receiver.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The caller id status is updated after successful start at low level and the
   internal data buffer is reset.
*******************************************************************************/
IFX_int32_t TAPI_Phone_CidRx_Start (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   ret = TAPI_LL_Phone_CidRx_Start (pChannel);
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiCidRx.pData = NULL;
      pChannel->TapiCidRx.stat.nStatus = TAPI_CIDRX_STAT_ACTIVE;
   }

   return ret;
}

/*******************************************************************************
Description:
   Stop caller id receiver.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   The caller id status is updated after successful start at low level.
*******************************************************************************/
IFX_int32_t TAPI_Phone_CidRx_Stop (TAPI_CONNECTION *pChannel)
{
   IFX_int32_t ret;

   ret = TAPI_LL_Phone_CidRx_Stop (pChannel);
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiCidRx.stat.nStatus = TAPI_CIDRX_STAT_INACTIVE;
   }

   return ret;
}

/*******************************************************************************
Description:
   Get the cid data already collected.
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pCidRxData  - handle to TAPI_CIDRX_DATA structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   in case the data collection is ongoing, reading from actual buffer should be
   protected against concurrent access. The low level call guarantees this.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_CidRxData (TAPI_CONNECTION *pChannel, TAPI_CIDRX_DATA *pCidRxData)
{
   IFX_int32_t      ret = IFX_ERROR;
   TAPI_CIDRX_DATA *pFifo;
   TAPI_CIDRX      *pCidRx = &pChannel->TapiCidRx;

   if (pCidRx->stat.nError == TAPI_CIDRX_ERR_NOERR)
   {
      switch (pCidRx->stat.nStatus)
      {
      case TAPI_CIDRX_STAT_ONGOING:
         /* read from actual buffer */
         ret = TAPI_LL_Phone_Get_CidRxDataCollected (pChannel, pCidRxData);
         break;
      case TAPI_CIDRX_STAT_DATA_RDY:
         /* read from fifo */
         pFifo = (TAPI_CIDRX_DATA *)Fifo_readElement(&(pCidRx->TapiCidRxFifo));
         /* copy data into user buffer */
         if (pFifo != NULL)
         {
            *pCidRxData = *pFifo;
            pChannel->TapiCidRx.pData = NULL;
            pChannel->TapiCidRx.stat.nStatus = TAPI_CIDRX_STAT_ACTIVE;
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

/*******************************************************************************
Description:
   retrieves a buffer for caller id receiver data.
Arguments:
   pCh      - channel pointer
   nLen     - cid data  number received
Return:
   buffer pointer
Remark :
   This function should be called each time memory is needed for Cid data
   buffering
*******************************************************************************/
TAPI_CIDRX_DATA *TAPI_Phone_GetCidRxBuf (TAPI_CONNECTION *pChannel, IFX_uint32_t nLen)
{
   TAPI_CIDRX      *pCidRx = &pChannel->TapiCidRx;
   TAPI_CIDRX_DATA *pData  = pCidRx->pData;

   /* get a new buffer if there is actually no buffer allocated or if
      the previous buffer is short about to overflow. */
   if ((pData == NULL) ||
       (pData->nSize + nLen > TAPI_MAX_CID_PARAMETER))
   {
      /* get Buffer handler */
      pData = (TAPI_CIDRX_DATA *)Fifo_writeElement (&pCidRx->TapiCidRxFifo);
      if (pData != NULL)
      {
         memset (pData, 0, sizeof (TAPI_CIDRX_DATA));
         pCidRx->pData = pData;
      }
   }

   return pData;
}

#endif /* TAPI_CID */


