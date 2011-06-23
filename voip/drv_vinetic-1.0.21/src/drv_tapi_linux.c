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
   Module      : drv_tapi_linux.c
   Desription  : Contains all linux specific implementation, for example
                 timer functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"
#ifdef LINUX

/* ============================= */
/* Local Variables               */
/* ============================= */

/* ============================= */
/* Local Functions               */
/* ============================= */

IFX_LOCAL IFX_void_t TAPI_timer_call_back (IFX_int32_t arg);
IFX_LOCAL IFX_void_t TAPI_tqueue          (IFX_void_t *arg);
#ifdef TAPI_CID
IFX_LOCAL IFX_void_t* TAPI_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size);
IFX_LOCAL IFX_void_t TAPI_UnmapBuffer(IFX_void_t* p_buffer);
#endif /* TAPI_CID */

/* ============================= */
/* Global function definition    */
/* ============================= */


/*******************************************************************************
Description:
   General function for handling TAPI Ioctls.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   cmd      - Ioctl Command
   arg      - Ioctl Parameter
Return:
   IFX_TRUE or IFX_FALSE or requested value.
*******************************************************************************/
IFX_int32_t TAPI_Ioctl(TAPI_CONNECTION *pChannel, IFX_uint32_t cmd,
                       IFX_uint32_t arg)
{
   IFX_int32_t ret = IFX_SUCCESS;
   void * parg = (void *)arg;

   /* check channel */
   switch (cmd)
   {
   /* Ringing Services */
   case IFXPHONE_RING_CONFIG:
      {
         TAPI_RING_CONFIG tmpRingConfig;
         if (copy_from_user (&tmpRingConfig, parg, sizeof(TAPI_RING_CONFIG)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_Ring_Config(pChannel, &tmpRingConfig);
         }
         break;
      }

   case IFXPHONE_RING_GET_CONFIG:
      {
         TAPI_RING_CONFIG tmpRingConfig;
         ret =  TAPI_Phone_Ring_GetConfig(pChannel, &tmpRingConfig);
         if ((ret == IFX_SUCCESS) && (copy_to_user (parg,
            &tmpRingConfig, sizeof(TAPI_RING_CONFIG)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   case PHONE_RING_CADENCE:
      ret = TAPI_Phone_Ring_Cadence(pChannel, arg);
      break;

   case IFXPHONE_RING_CADENCE_HIGH_RES:
      {
         TAPI_RING_CADENCE tmpCadence;
         if (copy_from_user (&tmpCadence, parg, sizeof(TAPI_RING_CADENCE)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_FALSE;
         }
         else
         {
            ret = TAPI_Phone_Ring_Cadence_High_Res(pChannel, &tmpCadence);
         }
         break;
      }
   case PHONE_RING_START:
      {
         TAPI_PHONE_CID *pPhoneCid = NULL;
         #ifdef TAPI_CID
         TAPI_PHONE_CID tmpPhoneCid;
         IFX_uint8_t tmpTransData [TAPI_MAX_CID_PARAMETER] = {0};
         if ((arg) && copy_from_user (&tmpPhoneCid, parg,
                                      sizeof(TAPI_PHONE_CID)) == 0)
         {
            pPhoneCid = &tmpPhoneCid;
            /*
            We don't know if cid sending is ongoing at this level, so we
            better copy the data from user space in a temporary buffer,
            from wich processing data will be copied from in case, instead of
            copying the data directly in the processing buffer, what can corrupt
            data in an ongoing cid transmission. We of course copy data twice,
            but we are safe.
            */
            if ((tmpPhoneCid.datalen > 0)                       &&
                (tmpPhoneCid.datalen <= TAPI_MAX_CID_PARAMETER) &&
                (tmpPhoneCid.pData != NULL))
            {
               if (copy_from_user (tmpTransData, tmpPhoneCid.pData,
                                   tmpPhoneCid.datalen) == 0)
               {
                  pPhoneCid->pData = tmpTransData;
               }
            }
         }
         #endif /* TAPI_CID */
         ret = TAPI_Phone_Ring_Start(pChannel, pPhoneCid);
         break;
      }

   case PHONE_RING_STOP:
      ret = TAPI_Phone_Ring_Stop(pChannel);
      break;

   case PHONE_MAXRINGS:
      ret = TAPI_Phone_Set_MaxRings(pChannel, arg);
      break;

   case PHONE_RING:
      ret = TAPI_Phone_Ring(pChannel);
      break;

   /* Operation Control Services */
   case IFXPHONE_SET_LINEFEED:
      ret = TAPI_Phone_Set_Linefeed(pChannel, arg);
      break;

   case PHONE_HOOKSTATE:
      {
         IFX_int32_t nHook;
         ret = TAPI_Phone_Hookstate(pChannel, &nHook);
         if ((ret == IFX_SUCCESS) && (copy_to_user (parg,
             &nHook, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
#ifdef TAPI_VOICE
   /* Connection Services*/
   case IFXPHONE_DATA_ADD:
      {
         TAPI_DATA_MAPPING tmpPhoneMap;
         if (copy_from_user (&tmpPhoneMap, parg, sizeof(TAPI_DATA_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_Data_Channel_Add (pChannel, &tmpPhoneMap);
         break;
      }

   case IFXPHONE_DATA_REMOVE:
      {
         TAPI_DATA_MAPPING tmpPhoneMap;
         if (copy_from_user (&tmpPhoneMap, parg, sizeof(TAPI_DATA_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_Data_Channel_Remove (pChannel, &tmpPhoneMap);
         break;
      }

   case IFXPHONE_PHONE_ADD:
      {
         TAPI_PHONE_MAPPING      tmpPhonePhoneMap;
         if (copy_from_user (&tmpPhonePhoneMap, parg, sizeof(TAPI_PHONE_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_Phone_Channel_Add (pChannel, &tmpPhonePhoneMap);
         break;
      }

   case IFXPHONE_PHONE_REMOVE:
      {
         TAPI_PHONE_MAPPING      tmpPhonePhoneMap;
         if (copy_from_user (&tmpPhonePhoneMap, parg, sizeof(TAPI_PHONE_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_Phone_Channel_Remove (pChannel, &tmpPhonePhoneMap);
         break;
      }
   case IFXPHONE_PCM_ADD:
      {
         TAPI_PCM_MAPPING     tmpPcmMap;
         if (copy_from_user (&tmpPcmMap, parg, sizeof(TAPI_PCM_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_PCM_Channel_Add (pChannel, &tmpPcmMap);
         break;
      }
	case IFXPHONE_PCM_REMOVE:
      {
         TAPI_PCM_MAPPING     tmpPcmMap;
         if (copy_from_user (&tmpPcmMap, parg, sizeof(TAPI_PCM_MAPPING)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_PCM_Channel_Remove (pChannel, &tmpPcmMap);
         break;
      }
   case IFXPHONE_RTP_CONF:
      {
         TAPI_RTP_CONF tmpRtpConf;
         if (copy_from_user (&tmpRtpConf, parg, sizeof(TAPI_RTP_CONF)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_Phone_RtpConf (pChannel, &tmpRtpConf);
         break;
      }

   case IFXPHONE_RTP_PTCONF:
      {
         TAPI_RTP_PT_CONF         tmpRtpPTConf;
         if (copy_from_user (&tmpRtpPTConf, parg, sizeof(TAPI_RTP_PT_CONF)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_Phone_Rtp_SetPayloadType (pChannel, &tmpRtpPTConf);
         break;
      }

   case IFXPHONE_RTCP_STATISTICS:
      {
         TAPI_RTCP_DATA tmpRtcpData;
         ret = TAPI_LL_Phone_RTCP_GetStatistics(pChannel, &tmpRtcpData);
         if ((ret == IFX_SUCCESS) && (copy_to_user (parg,
             &tmpRtcpData, sizeof(TAPI_RTCP_DATA)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case IFXPHONE_RTCP_RESET:
      ret = TAPI_LL_Phone_RTCP_Reset (pChannel);
      break;
   case IFXPHONE_JB_CONF:
      {
         TAPI_JB_CONF tmpJbConf;
         if (copy_from_user (&tmpJbConf, parg, sizeof(TAPI_JB_CONF)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_Phone_JB_Set (pChannel, &tmpJbConf);
         break;
      }

   case IFXPHONE_JB_STATISTICS:
      {
         TAPI_JB_DATA tmpJbData;
         ret = TAPI_LL_Phone_JB_GetStatistics(pChannel, &tmpJbData);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &tmpJbData, sizeof(TAPI_JB_DATA)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case IFXPHONE_JB_RESET:
      ret = TAPI_LL_Phone_JB_Reset (pChannel);
      break;

   case IFXPHONE_AAL_CONF:
      {
         TAPI_AAL_CONF tmpAalConf;
         if (copy_from_user (&tmpAalConf, parg, sizeof(TAPI_AAL_CONF)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_Phone_Aal_Set (pChannel, &tmpAalConf);
         break;
      }

   case IFXPHONE_AAL_PROFILE:
      {
         TAPI_AAL_PROFILE tmp;
         if (copy_from_user (&tmp, parg, sizeof(TAPI_AAL_PROFILE)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_Phone_AalProfile (pChannel, &tmp);
      }
      break;

   /* Recording Services */
   case PHONE_REC_CODEC:
      ret = TAPI_LL_Phone_Set_Rec_Codec (pChannel, arg);
      break;
   #ifdef TAPI_VOICE
   case PHONE_REC_START:
      ret = TAPI_LL_Phone_Start_Recording (pChannel);
      break;
   case PHONE_REC_STOP:
      ret = TAPI_LL_Phone_Stop_Recording(pChannel);
      break;
#endif /* TAPI_VOICE */
   case IFXPHONE_REC_HOLD:
      ret = TAPI_Phone_SetRecHold (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_REC_DEPTH:
      ret = TAPI_Phone_SetRecDepth (pChannel, (IFX_int32_t) arg);
      break;
   case PHONE_REC_VOLUME:
      ret = TAPI_Phone_SetRecVolume (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_REC_LEVEL:
      {
         IFX_int32_t nLevel;
         ret = TAPI_Phone_GetRecLevel (pChannel, (IFX_int32_t *)&nLevel);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
               &nLevel, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   /* Play Services not used */
   /*
   case PHONE_PLAY_CODEC:
      ret = TAPI_Phone_Set_Play_Codec(pChannel, arg);
      break;
   */
#ifdef TAPI_VOICE
   case PHONE_PLAY_START:
      ret = TAPI_LL_Phone_Start_Playing(pChannel);
      break;
   case PHONE_PLAY_STOP:
      ret = TAPI_LL_Phone_Stop_Playing(pChannel);
      break;
#endif /* TAPI_VOICE */
   case IFXPHONE_PLAY_HOLD:
      ret = TAPI_Phone_SetPlayHold (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_PLAY_DEPTH:
      ret = TAPI_Phone_SetPlayDepth (pChannel, (IFX_int32_t) arg);
      break;
   case PHONE_PLAY_VOLUME:
      ret = TAPI_Phone_SetPlayVolume (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_PLAY_LEVEL:
      {
         IFX_int32_t nLevel;
         ret = TAPI_Phone_GetPlayLevel (pChannel, (IFX_int32_t *)&nLevel);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &nLevel, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
#endif /* TAPI_VOICE */
#ifdef TAPI_FAX_T38
   /* Fax Services */
   case IFXPHONE_FAX_SETMOD:
      {
         TAPI_FAX_MODDATA tmpFaxMod;
         if (copy_from_user (&tmpFaxMod, parg,  sizeof(TAPI_FAX_MODDATA)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_FaxT38_SetModulator (pChannel, &tmpFaxMod);
         break;
      }
   case IFXPHONE_FAX_SETDEMOD:
      {
         TAPI_FAX_DEMODDATA tmpFaxDemod;
         if (copy_from_user (&tmpFaxDemod, parg, sizeof(TAPI_FAX_DEMODDATA)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
            ret = TAPI_LL_FaxT38_SetDemodulator (pChannel, &tmpFaxDemod);
         break;
      }
   case IFXPHONE_FAX_DISABLE:
      ret = TAPI_LL_FaxT38_DisableDataPump (pChannel);
      break;
   case IFXPHONE_FAX_STATUS:
      {
         TAPI_FAX_STATUS         tmpFaxStatus;
         ret = TAPI_LL_FaxT38_GetStatus (pChannel, &tmpFaxStatus);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &tmpFaxStatus, sizeof(TAPI_FAX_STATUS)) > 0))
         {
           TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
           ret = IFX_ERROR;
         }
         break;
      }
#endif /* TAPI_FAX_T38 */

   /* PCM Services */
   case IFXPHONE_SET_PCM_CONFIG:
      {
         TAPI_PCM_CONFIG tmpPCMConfig;
         if (copy_from_user (&tmpPCMConfig, parg, sizeof(TAPI_PCM_CONFIG)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_PCM_Set_Config(pChannel, &tmpPCMConfig);
         }
         break;
      }

   case IFXPHONE_GET_PCM_CONFIG:
      {
         TAPI_PCM_CONFIG tmpPCMConfig;
         ret = TAPI_Phone_PCM_Get_Config(pChannel, &tmpPCMConfig);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &tmpPCMConfig, sizeof(TAPI_PCM_CONFIG)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case IFXPHONE_SET_PCM_ACTIVATION:
      ret = TAPI_Phone_PCM_Set_Activation(pChannel, arg);
      break;

   case IFXPHONE_GET_PCM_ACTIVATION:
      ret = TAPI_Phone_PCM_Get_Activation(pChannel);
      break;

   /* Tone Services */
   case PHONE_PLAY_TONE:
      ret = TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
      break;
      case IFXPHONE_TONE_PLAY_NET:
      ret = TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_NET);
      break;

   case PHONE_SET_TONE_ON_TIME:
      ret = TAPI_Phone_Tone_Set_On_Time(pChannel, arg);
      break;

   case PHONE_SET_TONE_OFF_TIME:
      ret = TAPI_Phone_Tone_Set_Off_Time(pChannel, arg);
      break;

   case PHONE_GET_TONE_ON_TIME:
      {
         IFX_int32_t nDuration;
         ret = TAPI_Phone_Tone_Get_On_Time(pChannel, &nDuration);
         if ((ret == IFX_SUCCESS) && (copy_to_user (parg,
             &nDuration, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case PHONE_GET_TONE_OFF_TIME:
      {
         IFX_int32_t nDuration;
         ret = TAPI_Phone_Tone_Get_Off_Time(pChannel, &nDuration);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &nDuration, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case PHONE_GET_TONE_STATE:
      {
         IFX_uint32_t nToneState;
         ret = TAPI_Phone_Tone_Get_State(pChannel, &nToneState);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &nToneState, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case PHONE_DIALTONE:
      ret = TAPI_Phone_Tone_Dial(pChannel);
      break;

   case PHONE_RINGBACK:
      ret = TAPI_Phone_Tone_Ringback(pChannel);
      break;

   case PHONE_BUSY:
      ret = TAPI_Phone_Tone_Busy(pChannel);
      break;

   case PHONE_CPT_STOP:
      ret = TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
      break;

   case IFXPHONE_SET_TONE_LEVEL:
   {
      TAPI_TONE_LEVEL         tmpToneLevel;
      if (copy_from_user (&tmpToneLevel, parg, sizeof(TAPI_TONE_LEVEL)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Tone_Set_Level(pChannel, &tmpToneLevel);
      }
      break;
   }
   /* Dial Services */
   case IFXPHONE_GET_PULSE:
   {
      IFX_int32_t nPulseDigit;
      ret = TAPI_Phone_Get_Pulse(pChannel, &nPulseDigit);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nPulseDigit, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFXPHONE_GET_PULSE_ASCII:
   {
      IFX_int32_t nPulseDigit;
      ret = TAPI_Phone_Get_Pulse_ASCII(pChannel, &nPulseDigit);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nPulseDigit, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFXPHONE_PULSE_READY:
   {
      IFX_int32_t nPulseReady;
      ret = TAPI_Phone_Pulse_Ready(pChannel, &nPulseReady);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nPulseReady, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }
#ifdef TAPI_DTMF
   case PHONE_GET_DTMF:
   {
      IFX_int32_t nDtmfDigit;
      ret = TAPI_Phone_Get_DTMF(pChannel, &nDtmfDigit);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nDtmfDigit, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case PHONE_GET_DTMF_ASCII:
   {
      IFX_int32_t nDtmfDigit;
      ret = TAPI_Phone_Get_DTMF_ASCII(pChannel, &nDtmfDigit);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nDtmfDigit, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case PHONE_DTMF_OOB:
      ret = TAPI_LL_Phone_Send_DTMF_OOB(pChannel, arg);
      break;

   case PHONE_DTMF_READY:
   {
      IFX_int32_t nDtmfReady;
      ret = TAPI_Phone_DTMF_Ready(pChannel, &nDtmfReady);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
          &nDtmfReady, sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }
#endif /* TAPI_DTMF */

   /* Miscellaneous Services */
   case IFXPHONE_GET_VERSION:
      {
         IFX_char_t tmpVersionString[80];
         ret = TAPI_Phone_Get_Version(&tmpVersionString[0]);
         if ((ret == IFX_SUCCESS) &&  (copy_to_user ( parg,
             &tmpVersionString[0], sizeof(tmpVersionString)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case PHONE_EXCEPTION:
      ret = TAPI_Phone_Exception(pChannel);
      break;

   case IFXPHONE_MASK_EXCEPTION:
      ret = TAPI_Phone_Mask_Exception(pChannel, arg);
      break;

   case IFXPHONE_PHONE_STATUS:
      {
/*          TAPI_PHONE_STATUS tmpStatus[TAPI_MAX_CHANNELS]; */
         TAPI_PHONE_STATUS *pTmpStatus;
         IFX_int32_t ch;

         if (arg == 0)
            return IFX_ERROR;

         pTmpStatus = kmalloc(sizeof(TAPI_PHONE_STATUS) * TAPI_MAX_CHANNELS, GFP_KERNEL);
         if (pTmpStatus == 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: insufficient memory to allocate TAPI_PHONE_STATUS structure\n"));
            return IFX_ERROR;
         }
         
         ret = copy_from_user (pTmpStatus, parg, sizeof(TAPI_PHONE_STATUS));
         if (ret != 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: ioctl parameter error\n\r"));
            kfree(pTmpStatus);
            return IFX_ERROR;
         }

         if ((ch = pTmpStatus[0].channels) > TAPI_MAX_CHANNELS) {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH, ("TAPI: IFXPHONE_PHONE_STATUS: Ch%d: invalid parameters\n\r",
                  pChannel->nChannel - 1));
            kfree(pTmpStatus);
            return IFX_ERROR;
         }

         if (ch == 0)
            ch = 1;
         ret = TAPI_Phone_GetStatus(pChannel, pTmpStatus);
         copy_to_user ( parg, pTmpStatus,
                        sizeof(TAPI_PHONE_STATUS) * ch);
         kfree(pTmpStatus);

      }
      break;

   case IFXPHONE_INIT:
      {
         TAPI_INIT  tmpInit;

         if ((arg != 0) && (copy_from_user (&tmpInit, parg, sizeof(TAPI_INIT)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            if (arg == 0)
            {
               ret = TAPI_Phone_Init(pChannel, NULL);
            }
            else
            {
               ret = TAPI_Phone_Init(pChannel, &tmpInit);
            }
         }
         break;
      }

#ifdef TAPI_VOICE
   case PHONE_FRAME:
      ret = TAPI_LL_Phone_Set_Frame_Length(pChannel, arg);
      break;

   case IFXPHONE_GET_FRAME:
      {
         IFX_int32_t length = 0;
         ret = TAPI_LL_Phone_Get_Frame_Length(pChannel, &length);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
            &length, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
#endif /* TAPI_VOICE */

   case PHONE_VAD:
      ret = TAPI_LL_Phone_VAD(pChannel, arg);
      break;

   case PHONE_CAPABILITIES:
   {
      IFX_int32_t cap;
      cap = TAPI_LL_Phone_Get_Capabilities(pChannel);
      copy_to_user( parg, &cap, sizeof(IFX_int32_t));
      break;
   }

   case PHONE_CAPABILITIES_LIST:
      {
         TAPI_PHONE_CAPABILITY *tmpPhoneCap = NULL;
         IFX_int32_t tmpcapsize = TAPI_LL_Phone_Get_Capabilities(pChannel);
         tmpPhoneCap = kmalloc(tmpcapsize * sizeof(TAPI_PHONE_CAPABILITY), GFP_KERNEL);
         ret = TAPI_LL_Phone_Get_Capability_List(pChannel, tmpPhoneCap);
         if ((ret == IFX_SUCCESS) && (copy_to_user( parg,
            tmpPhoneCap, sizeof(TAPI_PHONE_CAPABILITY) * tmpcapsize) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         kfree(tmpPhoneCap);
         break;
      }
   case PHONE_CAPABILITIES_CHECK:
      {
         /* !!! revise code */
         TAPI_PHONE_CAPABILITY *tmpPhoneCap = NULL;
         tmpPhoneCap = kmalloc(sizeof(TAPI_PHONE_CAPABILITY), GFP_KERNEL);
         if (copy_from_user (tmpPhoneCap, parg, sizeof(TAPI_PHONE_CAPABILITY)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_LL_Phone_Check_Capability(pChannel, tmpPhoneCap);
            if (ret == IFX_SUCCESS || ret == 1)
            {
               copy_to_user ( parg, tmpPhoneCap, sizeof(TAPI_PHONE_CAPABILITY));
            }
         }
         kfree(tmpPhoneCap);
         break;
      }
   /* Metering Services */
   case IFXPHONE_METER_CHAR:
      {
         TAPI_METER_CONFIG       tmpMeterConfig;
         if (copy_from_user (&tmpMeterConfig, parg, sizeof(TAPI_METER_CONFIG)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_Meter_Config(pChannel, &tmpMeterConfig);
         }
         break;
      }
   case IFXPHONE_METER_START:
      ret = TAPI_Phone_Meter_Start(pChannel);
      break;

   case IFXPHONE_METER_STOP:
      ret = TAPI_Phone_Meter_Stop(pChannel);
      break;

   /* Lec Configuration */
   case IFXPHONE_LECCONF:
      {
         TAPI_LECCONF tmpLecConf;
         if (copy_from_user (&tmpLecConf, parg, sizeof(TAPI_LECCONF)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_LecConf (pChannel, &tmpLecConf);
         }
         break;
      }
   case IFXPHONE_GETLECCONF:
      {
         TAPI_LECCONF tmpLecConf;
         ret = TAPI_Phone_GetLecConf (pChannel, &tmpLecConf);
         if ((ret == IFX_SUCCESS) &&  (copy_to_user ( parg,
             &tmpLecConf, sizeof(TAPI_LECCONF)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   case IFXPHONE_LECCONF_PCM:
      {
         TAPI_LECCONF tmpLecConf;
         if (copy_from_user (&tmpLecConf, parg, sizeof(TAPI_LECCONF)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_LecConf_Pcm (pChannel, &tmpLecConf);
         }
         break;
      }
   case IFXPHONE_GETLECCONF_PCM:
      {
         TAPI_LECCONF tmpLecConf;
         ret = TAPI_Phone_GetLecConf_Pcm (pChannel, &tmpLecConf);
         if ((ret == IFX_SUCCESS) &&  (copy_to_user ( parg,
             &tmpLecConf, sizeof(TAPI_LECCONF)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   /* AGC Configuration/Enable */
   case IFXPHONE_AGC:
      ret = TAPI_LL_Phone_AgcEnable (pChannel, (TAPI_AGC_MODE)arg);
#ifdef TAPI_CID
   /* Caller ID Transmission service */
   case IFXPHONE_CIDCONF:
      {
         TAPI_CID_CONFIG tmpCidConfig;
         if (copy_from_user (&tmpCidConfig, parg, sizeof(TAPI_CID_CONFIG)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_CID_Config (pChannel, &tmpCidConfig);
         }
         break;
      }
   case IFXPHONE_GETCIDCONF:
      {
         TAPI_CID_CONFIG tmpCidConfig;
         ret = TAPI_Phone_CID_GetConfig (pChannel, &tmpCidConfig);
         if ((ret == IFX_SUCCESS) && (copy_to_user (parg,
             &tmpCidConfig, sizeof(tmpCidConfig)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   case IFXPHONE_DTMFCID_CONF:
      {
         TAPI_DTMFCID tmpDtmfCid;
         if (copy_from_user (&tmpDtmfCid, parg, sizeof(TAPI_DTMFCID)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_CID_ConfigDtmf (pChannel, &tmpDtmfCid);
         }
         break;
      }

   case IFXPHONE_CID_INFO_TX:
      {
         TAPI_CID_INFO_t tmpCidInfo;
         if (copy_from_user (&tmpCidInfo, parg, sizeof(TAPI_CID_INFO_t)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            if (tmpCidInfo.nMsgElements == 0)
               ret = IFX_ERROR;
            else
            {
               tmpCidInfo.message = TAPI_MapBuffer(tmpCidInfo.message,
                  tmpCidInfo.nMsgElements * sizeof(TAPI_CID_MSG_ELEMENT_t));
               if (tmpCidInfo.message != NULL)
               {
                  ret = TAPI_Phone_CID_Info_Tx (pChannel, &tmpCidInfo);
                  TAPI_UnmapBuffer(tmpCidInfo.message);
               }
               else
                  ret = IFX_ERROR;
            }
         }
         break;
      }

   /* Caller ID Reception service */
   case IFXPHONE_CIDRX_START:
      ret = TAPI_Phone_CidRx_Start (pChannel);
      break;

   case IFXPHONE_CIDRX_STOP:
      ret = TAPI_Phone_CidRx_Stop (pChannel);
      break;

   case IFXPHONE_CIDRX_STATUS:
      {
         TAPI_CIDRX_STATUS tmpCidRxStatus;
         ret = TAPI_LL_Phone_CidRx_Status (pChannel, &tmpCidRxStatus);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
            &tmpCidRxStatus, sizeof(TAPI_CIDRX_STATUS)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }

   case IFXPHONE_CIDRX_DATA:
      {
         TAPI_CIDRX_DATA tmpCidRxData;
         ret = TAPI_Phone_Get_CidRxData (pChannel, &tmpCidRxData);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
            &tmpCidRxData, sizeof(TAPI_CIDRX_DATA)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   case IFXPHONE_CID2_CONF:
      {
         TAPI_CID2_CONFIG tmpCid2Conf;
         if (copy_from_user (&tmpCid2Conf, parg, sizeof(TAPI_CID2_CONFIG)) > 0 )
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_CID2_Config (pChannel, &tmpCid2Conf);
         }
         break;
      }
   case IFXPHONE_CID2_GETCONF:
      {
         TAPI_CID2_CONFIG tmpCid2Conf;
         ret = TAPI_Phone_CID2_GetConfig (pChannel, &tmpCid2Conf);
         if ((ret == IFX_SUCCESS) && (copy_to_user ( parg,
             &tmpCid2Conf, sizeof(TAPI_CID2_CONFIG)) > 0))
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
                 ("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
         break;
      }
   case IFXPHONE_CID2_SEND:
      {
         TAPI_PHONE_CID tmpPhoneCid;
         IFX_uint8_t tmpTransData [TAPI_MAX_CID_PARAMETER] = {0};
         if (copy_from_user (&tmpPhoneCid, parg, sizeof(TAPI_PHONE_CID)) > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
                 ("TAPI: cannot copy structure from user space\n\r"));
            ret = IFX_ERROR;
         }
         else if ((tmpPhoneCid.datalen > 0)                       &&
                  (tmpPhoneCid.datalen <= TAPI_MAX_CID_PARAMETER) &&
                  (tmpPhoneCid.pData != NULL))
         {
            /*
            We don't know if cid sending is ongoing at this level, so we
            better copy the data from user space in a temporary buffer,
            from wich processing data will be copied from in case, instead of
            copying the data directly in the processing buffer, what can corrupt
            data in an ongoing cid transmission. We of course copy data twice,
            but we are safe.
            */
            if (copy_from_user (tmpTransData, tmpPhoneCid.pData,
                                 tmpPhoneCid.datalen) > 0)
            {
               TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
                    ("TAPI: cannot copy data from user space\n\r"));
               ret = IFX_ERROR;
            }
            else
            {
               tmpPhoneCid.pData = tmpTransData;
            }
         }
         if (ret != IFX_ERROR)
         {
            ret = TAPI_Phone_CID2_Send (pChannel, &tmpPhoneCid);
         }
         break;
      }
#endif /* TAPI_CID */

   /* Report Level */
   case IFXPHONE_REPORT_SET:
      SetTraceLevel(TAPI_DRV, arg);
      break;

   case IFXPHONE_VERSION_CHECK:
      {
         TAPI_PHONE_VERSION vers;
         if (copy_from_user (&vers, parg, sizeof(TAPI_PHONE_VERSION)) > 0 )
         {
            ret = IFX_ERROR;
         }
         else
         {
            ret = TAPI_Phone_Check_Version (&vers);
         }
      }
      break;
   case IFXPHONE_VOLUME:
   {
      TAPI_PHONE_VOLUME tmp;
      if (copy_from_user (&tmp, parg, sizeof(TAPI_PHONE_VOLUME)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         return TAPI_LL_Phone_Volume (pChannel, &tmp);
      }
      break;
   }

   case IFXPHONE_PCM_VOLUME:
   {
      TAPI_PHONE_VOLUME tmp;
      if (copy_from_user (&tmp, parg, sizeof(TAPI_PHONE_VOLUME)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         return TAPI_LL_Phone_Pcm_Volume (pChannel, &tmp);
      }
      break;
   }

   case IFXPHONE_HIGH_LEVEL:
      return TAPI_LL_Phone_High_Level (pChannel, arg);

   case IFXPHONE_ENABLE_SIGDETECT:
   {
      TAPI_PHONE_SIGDETECTION tmp;
      if (copy_from_user (&tmp, parg, sizeof(TAPI_PHONE_SIGDETECTION)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         return TAPI_LL_Enable_Signal (pChannel, &tmp);
      }
      break;
   }

   case IFXPHONE_DISABLE_SIGDETECT:
   {
      TAPI_PHONE_SIGDETECTION tmp;
      if (copy_from_user (&tmp, parg, sizeof(TAPI_PHONE_SIGDETECTION)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         return TAPI_LL_Disable_Signal (pChannel, &tmp);
      }
      break;
   }
   case IFXPHONE_TONETABLE_CONF:
      {
         TAPI_TONE tmp;
         if (copy_from_user (&tmp, parg, sizeof(TAPI_TONE)) > 0 )
         {
            ret = IFX_ERROR;
         }
         else
         {
            return TAPI_Phone_Tone_TableConf (pChannel->pTapiDevice->pToneTbl,
                                                &tmp);
         }
         break;
      }

   case IFXPHONE_CPTD_START:
   {
      TAPI_PHONE_CPTD tmp;
      if (copy_from_user (&tmp, parg, sizeof(TAPI_PHONE_CPTD)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         return TAPI_Phone_DetectToneStart(pChannel, &tmp);
      }
      break;
   }
   case IFXPHONE_CPTD_STOP:
   {
      return TAPI_Phone_DetectToneStop(pChannel);
      break;
   }

   default:
      TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: Ioctl 0x%08lX not handled by TAPI\n\r", cmd));
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

/*******************************************************************************
Description:
   Function create a timer.
Arguments:
   pTimerEntry - Functionpointer to the call back function
   nArgument   - pointer to TAPI channel structure
Return:
   Timer_ID    - pointer to internal timer structure
Remarks:
   Initialize a task queue which will be scheduled once a timer interrupt occurs
   to execute the appropriate operation in a process context, process in which
   semaphores ... are allowed.
   Please notice that this task has to run under the keventd process, in which it
   can be executed thousands of times within a single timer tick.
*******************************************************************************/
Timer_ID TAPI_Create_Timer(TIMER_ENTRY pTimerEntry, IFX_int32_t nArgument)
{
   Timer_ID pTimerData;

   /* allocate memory for the channel */
   pTimerData = kmalloc(sizeof(*pTimerData), GFP_KERNEL);
   if (pTimerData == NULL)
      return NULL;

   /* set function to be called after timer expires */
   pTimerData->pTimerEntry = pTimerEntry;
   pTimerData->nArgument = nArgument;

   init_timer(&(pTimerData->Timer_List));

   /* set timer call back function */
   pTimerData->Timer_List.function = (linux_timer_callback)TAPI_timer_call_back;
   pTimerData->Timer_List.data = (IFX_int32_t) pTimerData;

   /* initialize tq_struct member of Timer_ID structure */
   memset(&(pTimerData->timerTask), 0, sizeof(struct tq_struct));
   /* Initialize Timer Task */
   pTimerData->timerTask.routine = TAPI_tqueue;
   pTimerData->timerTask.data = (IFX_void_t *) pTimerData;

   return pTimerData;
}

/*******************************************************************************
Description:
   Function set and starts a timer with a specific time. It can be choose if the
   timer starts periodically.
Arguments:
   Timer_ID      - pointer to internal timer structure
   nTime         - Time in ms
   bPeriodically - Starts the timer periodically or not
   bRestart      - Restart the timer or normal start
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/
IFX_boolean_t TAPI_SetTime_Timer(Timer_ID Timer, IFX_uint32_t nTime,
                                 IFX_boolean_t bPeriodically, IFX_boolean_t bRestart)
{
   if (Timer == 0)
   {
     TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("\n\rDRV_ERROR: TAPI_SetTime_Timer (Timer == 0)\n\r"));
     return IFX_FALSE;
   }
   Timer->Periodical_Time = HZ*nTime/1000;
   /* prevent restart of driver */
   Timer->bPeriodical = IFX_FALSE;
   /* remove driver from list */
   del_timer_sync(&(Timer->Timer_List));

   Timer->bPeriodical = bPeriodically;

   Timer->Timer_List.expires = jiffies + Timer->Periodical_Time;
   add_timer(&(Timer->Timer_List));

   return IFX_TRUE;
}


/*******************************************************************************
Description:
   Function stop a timer.
Arguments:
   Timer_ID      - pointer to internal timer structure
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/
IFX_boolean_t TAPI_Stop_Timer(Timer_ID Timer)
{
   /* stop timer */
   /* prevent restart of driver */
   Timer->bPeriodical = IFX_FALSE;
   /* remove driver from list */
   del_timer_sync(&(Timer->Timer_List));
   return (IFX_TRUE);
}

/*******************************************************************************
Description:
   Function delete a timer.
Arguments:
   Timer_ID      - pointer to internal timer structure
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/
IFX_boolean_t TAPI_Delete_Timer(Timer_ID Timer)
{
   if (Timer)
   {
      TAPI_Stop_Timer(Timer);
      /* free memory */
      kfree(Timer);
      return IFX_TRUE;
   }
   return IFX_FALSE;
}

/****************************************************************************
Description :
   Helper function to get a periodical timer
Arguments   :
   arg   - pointer to corresponding timer ID
Remarks     :
   This function will be executed in  the process context, so to avoid
   scheduling in Interrupt Mode while working with semaphores etc...
   The task is always running under the keventd process and is also running
   very quickly. Even on a very heavily loaded system, the latency in the
   scheduler queue is quite small
****************************************************************************/
IFX_LOCAL IFX_void_t TAPI_tqueue (IFX_void_t *arg)
{
   Timer_ID Timer = (Timer_ID) arg;
   /* Call TAPI Timer function */
   Timer->pTimerEntry(Timer, Timer->nArgument);
   if (Timer->bPeriodical)
   {
      /* remove driver from list */
      del_timer_sync(&(Timer->Timer_List));
      /* start new timer, then call function to gain precision */
      Timer->Timer_List.expires = jiffies + Timer->Periodical_Time;
      add_timer(&(Timer->Timer_List));
   }
}

/****************************************************************************
Description :
   Helper function to get a periodical timer
Arguments   :
   arg   - pointer to corresponding timer ID
Return      :
   NONE
****************************************************************************/
IFX_LOCAL IFX_void_t TAPI_timer_call_back (IFX_int32_t arg)
{
   Timer_ID Timer = (Timer_ID) arg;
   /* do the operation in process context,
      not in interrupt context */
   schedule_task (&(Timer->timerTask));
}

#ifdef TAPI_CID
/*******************************************************************************
Description:
   This function allocates a temporary kernel buffer
      and copies the user buffer contents to it.
Arguments:
   p_buffer - pointer to the user buffer.
   size     - size of the buffer.
Return Value:
   pointer to kernel buffer or NULL
Remarks:
   Buffer has to be released with \ref TAPI_UnmapBuffer
*******************************************************************************/
IFX_LOCAL IFX_void_t* TAPI_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size)
{
   IFX_void_t* kernel_buffer = vmalloc(size);
   if (kernel_buffer != NULL)
      copy_from_user(kernel_buffer, p_buffer, size);
   return kernel_buffer;
}

/*******************************************************************************
Description:
   This function releases a temporary kernel buffer.
Arguments:
   p_buffer - pointer to the kernel buffer.
Return Value:
   None.
Remarks:
  Counterpart of \ref TAPI_MapBuffer
*******************************************************************************/
IFX_LOCAL IFX_void_t TAPI_UnmapBuffer(IFX_void_t* p_buffer)
{
   if (p_buffer != NULL)
      vfree(p_buffer);
}
#endif /* TAPI_CID */

#endif /* LINUX */


