/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

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
/* Defines                       */
/* ============================= */

#define TAPI_IOCTL_STACKSIZE        4000 /* allow some overhead 4 k */

/* ============================= */
/* Local Variables               */
/* ============================= */

/* ============================= */
/* Local Functions               */
/* ============================= */

IFX_LOCAL IFX_void_t TAPI_timer_call_back (IFX_int32_t arg);
IFX_LOCAL IFX_void_t TAPI_tqueue          (IFX_void_t *arg);


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
   IFX_int32_t ret         = IFX_SUCCESS;
   IFX_void_t *parg        = (IFX_void_t *)arg;
   IFX_void_t *p_iostack   = IFX_NULL;

   /* Get hold of memory to process this ioctl */
   p_iostack = IFXOS_MALLOC (TAPI_IOCTL_STACKSIZE);
   if (p_iostack == IFX_NULL)
   	return IFX_ERROR;

   /* check channel */
   switch (cmd)
   {
   /* Ringing Services */
   case IFX_TAPI_RING_CFG_SET:
   {
      IFX_TAPI_RING_CFG_t *p_tmp = (IFX_TAPI_RING_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_RING_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_RING_CFG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Ring_Config(pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_RING_CFG_GET:
   {
      IFX_TAPI_RING_CFG_t *p_tmp = (IFX_TAPI_RING_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_RING_CFG_t) < TAPI_IOCTL_STACKSIZE);
      ret =  TAPI_Phone_Ring_GetConfig(pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user (parg, p_tmp,
          sizeof(IFX_TAPI_RING_CFG_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_RING_CADENCE_SET:
      ret = TAPI_Phone_Ring_Cadence(pChannel, arg);
      break;

   case IFX_TAPI_RING_CADENCE_HR_SET:
   {
      IFX_TAPI_RING_CADENCE_t *p_tmp = (IFX_TAPI_RING_CADENCE_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_RING_CADENCE_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_RING_CADENCE_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_FALSE;
      }
      else
      {
         ret = TAPI_Phone_Ring_Cadence_High_Res(pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_RING_START:
      ret = TAPI_Phone_Ring_Start(pChannel);
      break;

   case IFX_TAPI_RING_STOP:
      ret = TAPI_Phone_Ring_Stop(pChannel);
      break;

   case IFX_TAPI_RING_MAX_SET:
      ret = TAPI_Phone_Set_MaxRings(pChannel, arg);
      break;

   case IFX_TAPI_RING:
      ret = TAPI_Phone_Ring(pChannel);
      break;

   /* Operation Control Services */
   case IFX_TAPI_LINE_FEED_SET:
      ret = TAPI_Phone_Set_Linefeed(pChannel, arg);
      break;

   case IFX_TAPI_LINE_HOOK_STATUS_GET:
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
   case IFX_TAPI_MAP_DATA_ADD:
   {
      IFX_TAPI_MAP_DATA_t *p_tmp = (IFX_TAPI_MAP_DATA_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_DATA_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_DATA_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_Data_Channel_Add (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_MAP_DATA_REMOVE:
   {
      IFX_TAPI_MAP_DATA_t *p_tmp = (IFX_TAPI_MAP_DATA_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_DATA_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_DATA_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_Data_Channel_Remove (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_MAP_PHONE_ADD:
   {
      IFX_TAPI_MAP_PHONE_t  *p_tmp = (IFX_TAPI_MAP_PHONE_t  *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_PHONE_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_PHONE_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_Phone_Channel_Add (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_MAP_PHONE_REMOVE:
   {
      IFX_TAPI_MAP_PHONE_t *p_tmp = (IFX_TAPI_MAP_PHONE_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_PHONE_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_PHONE_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_Phone_Channel_Remove (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_MAP_PCM_ADD:
   {
      IFX_TAPI_MAP_PCM_t  *p_tmp = (IFX_TAPI_MAP_PCM_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_PCM_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_PCM_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_PCM_Channel_Add (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_MAP_PCM_REMOVE:
   {
      IFX_TAPI_MAP_PCM_t  *p_tmp = (IFX_TAPI_MAP_PCM_t  *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_MAP_PCM_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_MAP_PCM_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_PCM_Channel_Remove (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_PKT_RTP_CFG_SET:
   {
      IFX_TAPI_PKT_RTP_CFG_t *p_tmp = (IFX_TAPI_PKT_RTP_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PKT_RTP_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PKT_RTP_CFG_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_Phone_RtpConf (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_PKT_RTP_PT_CFG_SET:
   {
      IFX_TAPI_PKT_RTP_PT_CFG_t *p_tmp =
                                     (IFX_TAPI_PKT_RTP_PT_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PKT_RTP_PT_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PKT_RTP_PT_CFG_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_Phone_Rtp_SetPayloadType (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_PKT_RTCP_STATISTICS_GET:
   {
      IFX_TAPI_PKT_RTCP_STATISTICS_t *p_tmp =
                                 (IFX_TAPI_PKT_RTCP_STATISTICS_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PKT_RTCP_STATISTICS_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_LL_Phone_RTCP_GetStatistics(pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user (parg, p_tmp,
                                sizeof(IFX_TAPI_PKT_RTCP_STATISTICS_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_PKT_RTCP_STATISTICS_RESET:
      ret = TAPI_LL_Phone_RTCP_Reset (pChannel);
      break;
   case IFX_TAPI_JB_CFG_SET:
   {
      IFX_TAPI_JB_CFG_t *p_tmp = (IFX_TAPI_JB_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_JB_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_JB_CFG_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_Phone_JB_Set (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_JB_STATISTICS_GET:
   {
      IFX_TAPI_JB_STATISTICS_t *p_tmp =
                                   (IFX_TAPI_JB_STATISTICS_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_JB_STATISTICS_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_LL_Phone_JB_GetStatistics(pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg, p_tmp,
                                   sizeof(IFX_TAPI_JB_STATISTICS_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_JB_STATISTICS_RESET:
      ret = TAPI_LL_Phone_JB_Reset (pChannel);
      break;

   case IFX_TAPI_PKT_AAL_CFG_SET:
   {
      IFX_TAPI_PCK_AAL_CFG_t *p_tmp = (IFX_TAPI_PCK_AAL_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PCK_AAL_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PCK_AAL_CFG_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_Phone_Aal_Set (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_PKT_AAL_PROFILE_SET:
   {
      IFX_TAPI_PCK_AAL_PROFILE_t *p_tmp =
                                      (IFX_TAPI_PCK_AAL_PROFILE_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PCK_AAL_PROFILE_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PCK_AAL_PROFILE_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_Phone_AalProfile (pChannel, p_tmp);
      break;
   }

   /* Recording Services */
   case IFX_TAPI_ENC_TYPE_SET:
      ret = TAPI_LL_Phone_Set_Rec_Codec (pChannel, arg);
      break;
   case IFX_TAPI_ENC_START:
      ret = TAPI_LL_Phone_Start_Recording (pChannel);
      break;
   case IFX_TAPI_ENC_STOP:
      ret = TAPI_LL_Phone_Stop_Recording(pChannel);
      break;
   case IFXPHONE_REC_HOLD:
      ret = TAPI_Phone_SetRecHold (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_REC_DEPTH:
      ret = TAPI_Phone_SetRecDepth (pChannel, (IFX_int32_t) arg);
      break;
   case IFX_TAPI_ENC_VOLUME_SET:
      ret = TAPI_Phone_SetRecVolume (pChannel, (IFX_int32_t)arg);
      break;
   case IFX_TAPI_ENC_LEVEL_SET:
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
   case IFX_TAPI_DEC_TYPE_SET:
      ret = TAPI_Phone_Set_Play_Codec(pChannel, arg);
      break;
   */
   case IFX_TAPI_DEC_START:
      ret = TAPI_LL_Phone_Start_Playing(pChannel);
      break;
   case IFX_TAPI_DEC_STOP:
      ret = TAPI_LL_Phone_Stop_Playing(pChannel);
      break;
   case IFXPHONE_PLAY_HOLD:
      ret = TAPI_Phone_SetPlayHold (pChannel, (IFX_int32_t)arg);
      break;
   case PHONE_PLAY_DEPTH:
      ret = TAPI_Phone_SetPlayDepth (pChannel, (IFX_int32_t) arg);
      break;
   case IFX_TAPI_DEC_VOLUME_SET:
      ret = TAPI_Phone_SetPlayVolume (pChannel, (IFX_int32_t)arg);
      break;
   case IFX_TAPI_DEC_LEVEL_GET:
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
   case IFX_TAPI_T38_MOD_START:
   {
      IFX_TAPI_T38_MOD_DATA_t *p_tmp = (IFX_TAPI_T38_MOD_DATA_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_T38_MOD_DATA_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg,  sizeof(IFX_TAPI_T38_MOD_DATA_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_FaxT38_SetModulator (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_T38_DEMOD_START:
   {
      IFX_TAPI_T38_DEMOD_DATA_t *p_tmp = (IFX_TAPI_T38_DEMOD_DATA_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_T38_DEMOD_DATA_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_T38_DEMOD_DATA_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
         ret = TAPI_LL_FaxT38_SetDemodulator (pChannel, p_tmp);
      break;
   }

   case IFX_TAPI_T38_STOP:
      ret = TAPI_LL_FaxT38_DisableDataPump (pChannel);
      break;

   case IFX_TAPI_T38_STATUS_GET:
   {
      IFX_TAPI_T38_STATUS_t *p_tmp = (IFX_TAPI_T38_STATUS_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_T38_STATUS_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_LL_FaxT38_GetStatus (pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg, p_tmp,
                                   sizeof(IFX_TAPI_T38_STATUS_t)) > 0))
      {
        TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
        ret = IFX_ERROR;
      }
      break;
   }
#endif /* TAPI_FAX_T38 */

   /* PCM Services */
   case IFX_TAPI_PCM_CFG_SET:
   {
      IFX_TAPI_PCM_CFG_t *p_tmp = (IFX_TAPI_PCM_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PCM_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PCM_CFG_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_PCM_Set_Config(pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_PCM_CFG_GET:
   {
      IFX_TAPI_PCM_CFG_t *p_tmp = (IFX_TAPI_PCM_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PCM_CFG_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_Phone_PCM_Get_Config(pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user (parg, p_tmp,
                                   sizeof(IFX_TAPI_PCM_CFG_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_PCM_ACTIVATION_SET:
      ret = TAPI_Phone_PCM_Set_Activation(pChannel, arg);
      break;

   case IFX_TAPI_PCM_ACTIVATION_GET:
      ret = TAPI_Phone_PCM_Get_Activation(pChannel, (IFX_boolean_t*) arg);
      break;

   /* Tone Services */
   case IFX_TAPI_TONE_LOCAL_PLAY:
      ret = TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
      break;

   case IFX_TAPI_TONE_NET_PLAY:
      ret = TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_NET);
      break;

   case IFX_TAPI_TONE_ON_TIME_SET:
      ret = TAPI_Phone_Tone_Set_On_Time(pChannel, arg);
      break;

   case IFX_TAPI_TONE_OFF_TIME_SET:
      ret = TAPI_Phone_Tone_Set_Off_Time(pChannel, arg);
      break;

   case IFX_TAPI_TONE_ON_TIME_GET:
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

   case IFX_TAPI_TONE_OFF_TIME_GET:
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

   case IFX_TAPI_TONE_STATUS_GET:
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

   case IFX_TAPI_TONE_DIALTONE_PLAY:
      ret = TAPI_Phone_Tone_Dial(pChannel);
      break;

   case IFX_TAPI_TONE_RINGBACK_PLAY:
      ret = TAPI_Phone_Tone_Ringback(pChannel);
      break;

   case IFX_TAPI_TONE_BUSY_PLAY:
      ret = TAPI_Phone_Tone_Busy(pChannel);
      break;

   case IFX_TAPI_TONE_STOP:
      ret = TAPI_Phone_Tone_Stop (pChannel, arg);
      break;

   case IFX_TAPI_TONE_LEVEL_SET:
   {
      IFX_TAPI_PREDEF_TONE_LEVEL_t  *p_tmp =
                                    (IFX_TAPI_PREDEF_TONE_LEVEL_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_PREDEF_TONE_LEVEL_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_PREDEF_TONE_LEVEL_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Tone_Set_Level(pChannel, p_tmp);
      }
      break;
   }
   /* Dial Services */
   case IFX_TAPI_PULSE_GET:
   {
      IFX_int32_t nPulseDigit;
      ret = TAPI_Phone_Get_Pulse(pChannel, &nPulseDigit);
      if ( (ret == IFX_SUCCESS) &&
           (copy_to_user (parg, &nPulseDigit, sizeof(IFX_int32_t)) > 0) )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_PULSE_ASCII_GET:
   {
      IFX_int32_t nPulseDigit;
      ret = TAPI_Phone_Get_Pulse_ASCII(pChannel, &nPulseDigit);
      if ( (ret == IFX_SUCCESS) &&
           (copy_to_user (parg, &nPulseDigit, sizeof(IFX_int32_t)) > 0) )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_PULSE_READY:
   {
      IFX_int32_t nPulseReady;
      ret = TAPI_Phone_Pulse_Ready(pChannel, &nPulseReady);
      if ( (ret == IFX_SUCCESS) &&
           (copy_to_user ( parg, &nPulseReady, sizeof(IFX_int32_t)) > 0) )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_LINE_HOOK_VT_SET:
   {
      IFX_TAPI_LINE_HOOK_VT_t *p_tmp = (IFX_TAPI_LINE_HOOK_VT_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LINE_HOOK_VT_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_LINE_HOOK_VT_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Validation_Time(pChannel, p_tmp);
      }
      break;
   }

#ifdef TAPI_DTMF
   case IFX_TAPI_TONE_DTMF_GET:
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

   case IFX_TAPI_TONE_DTMF_ASCII_GET:
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

   case IFX_TAPI_PKT_EV_OOB_SET:
      ret = TAPI_LL_Phone_Send_DTMF_OOB(pChannel, arg);
      break;

   case IFX_TAPI_TONE_DTMF_READY_GET:
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
   case IFX_TAPI_VERSION_GET:
   {
      IFX_char_t *p_tmp = (IFX_char_t *)p_iostack;

      /* make sure that the stacksize is sufficient */
      IFXOS_ASSERT(80 < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_Phone_Get_Version(p_tmp);
      /* check for ret (string length) > 0 */
      if ((ret > 0) &&  (copy_to_user (parg, p_tmp, 80) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_EXCEPTION_GET:
      ret = TAPI_Phone_Exception(pChannel);
      break;

   case IFX_TAPI_EXCEPTION_MASK:
      ret = TAPI_Phone_Mask_Exception(pChannel, arg);
      break;

   case IFX_TAPI_CH_STATUS_GET:
   {
      IFX_TAPI_CH_STATUS_t *p_tmp = IFX_NULL;
      IFX_int32_t ch;
      if (arg == 0)
      {
         ret = IFX_ERROR;
         break;
      }
      p_tmp = IFXOS_MALLOC(TAPI_MAX_CHANNELS * sizeof(IFX_TAPI_CH_STATUS_t));
      if (p_tmp == IFX_NULL)
      {
         ret = IFX_ERROR;
         break;
      }
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_CH_STATUS_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: ioctl parameter error\n\r"));
         ret = IFX_ERROR;
      }
      if ((ret == IFX_SUCCESS) && ((ch = p_tmp[0].channels) > TAPI_MAX_CHANNELS))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,
               ("TAPI: IFX_TAPI_CH_STATUS_GET: Ch%d: invalid parameters\n\r",
                pChannel->nChannel - 1));
         ret = IFX_ERROR;
      }
      if (ret == IFX_SUCCESS)
      {
         if (ch == 0)
            ch = 1;
         ret = TAPI_Phone_GetStatus(pChannel, p_tmp);
         if (copy_to_user (parg, p_tmp, sizeof(IFX_TAPI_CH_STATUS_t) * ch)
             > 0)
         {
            TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
      }
      IFXOS_FREE (p_tmp);
      break;
   }

   case IFX_TAPI_CH_INIT:
   {
      IFX_TAPI_CH_INIT_t  *p_tmp = (IFX_TAPI_CH_INIT_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CH_INIT_t) < TAPI_IOCTL_STACKSIZE);
      if ((arg != 0) && (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_CH_INIT_t)) > 0))
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
            ret = TAPI_Phone_Init(pChannel, p_tmp);
         }
      }
      break;
   }

#ifdef TAPI_VOICE
   case IFX_TAPI_ENC_FRAME_LEN_SET:
      ret = TAPI_LL_Phone_Set_Frame_Length(pChannel, arg);
      break;

   case IFX_TAPI_ENC_FRAME_LEN_GET:
   {
      IFX_int32_t length = 0;
      ret = TAPI_LL_Phone_Get_Frame_Length(pChannel, &length);
      if ((ret == IFX_SUCCESS) && (copy_to_user (parg, &length,
          sizeof(IFX_int32_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }
#endif /* TAPI_VOICE */

   case IFX_TAPI_ENC_VAD_CFG_SET:
      ret = TAPI_LL_Phone_VAD(pChannel, arg);
      break;

   case IFX_TAPI_CAP_NR:
   {
      IFX_int32_t cap;
      cap = TAPI_LL_Phone_Get_Capabilities(pChannel);
      copy_to_user( parg, &cap, sizeof(IFX_int32_t));
      break;
   }

   case IFX_TAPI_CAP_LIST:
   {
      IFX_TAPI_CAP_t *p_tmp = IFX_NULL;
      IFX_uint32_t   tmpcapsize;

      tmpcapsize = TAPI_LL_Phone_Get_Capabilities(pChannel);
      p_tmp = IFXOS_MALLOC(tmpcapsize * sizeof(IFX_TAPI_CAP_t));
      if (p_tmp == IFX_NULL)
      {
         ret = IFX_ERROR;
         break;
      }
      ret = TAPI_LL_Phone_Get_Capability_List(pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user( parg, p_tmp,
                                   sizeof(IFX_TAPI_CAP_t) * tmpcapsize) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      IFXOS_FREE(p_tmp);
      break;
   }

   case IFX_TAPI_CAP_CHECK:
   {
      IFX_TAPI_CAP_t *p_tmp = (IFX_TAPI_CAP_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CAP_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_CAP_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Phone_Check_Capability(pChannel, p_tmp);
         if (ret == IFX_SUCCESS || ret == 1)
         {
            copy_to_user (parg, p_tmp, sizeof(IFX_TAPI_CAP_t));
         }
      }
      break;
   }

   /* Metering Services */
   case IFX_TAPI_METER_CFG_SET:
   {
      IFX_TAPI_METER_CFG_t *p_tmp = (IFX_TAPI_METER_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_METER_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_METER_CFG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Meter_Config(pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_METER_START:
      ret = TAPI_Phone_Meter_Start(pChannel);
      break;

   case IFX_TAPI_METER_STOP:
      ret = TAPI_Phone_Meter_Stop(pChannel);
      break;

   /* Lec Configuration */
   case IFX_TAPI_LEC_PHONE_CFG_SET:
   {
      IFX_TAPI_LEC_CFG_t *p_tmp = (IFX_TAPI_LEC_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LEC_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_LEC_CFG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_LecConf (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_LEC_PHONE_CFG_GET:
   {
      IFX_TAPI_LEC_CFG_t *p_tmp = (IFX_TAPI_LEC_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LEC_CFG_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_Phone_GetLecConf (pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) &&  (copy_to_user ( parg, p_tmp,
                                    sizeof(IFX_TAPI_LEC_CFG_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_LEC_PCM_CFG_SET:
   {
      IFX_TAPI_LEC_CFG_t *p_tmp = (IFX_TAPI_LEC_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LEC_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_LEC_CFG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_LecConf_Pcm (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_LEC_PCM_CFG_GET:
   {
      IFX_TAPI_LEC_CFG_t *p_tmp = (IFX_TAPI_LEC_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LEC_CFG_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_Phone_GetLecConf_Pcm (pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) &&  (copy_to_user ( parg, p_tmp,
                                    sizeof(IFX_TAPI_LEC_CFG_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }
#ifdef TAPI_CID
   /* Caller ID Transmission service */
   case IFX_TAPI_CID_CFG_SET:
   {
      IFX_TAPI_CID_CFG_t *p_tmp = (IFX_TAPI_CID_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CID_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_CID_CFG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_CID_SetConfig (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_CID_TX_INFO_START:
   case IFX_TAPI_CID_TX_SEQ_START:
   {
      IFX_TAPI_CID_MSG_t *p_tmp = (IFX_TAPI_CID_MSG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CID_MSG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_CID_MSG_t)) > 0 )
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         if (cmd == IFX_TAPI_CID_TX_INFO_START)
            ret = TAPI_Phone_CID_Info_Tx (pChannel, p_tmp);
         else
            ret = TAPI_Phone_CID_Seq_Tx (pChannel, p_tmp);
      }
      break;
   }

   /* Caller ID Reception service */
   case IFX_TAPI_CID_RX_START:
      ret = TAPI_Phone_CidRx_Start (pChannel);
      break;

   case IFX_TAPI_CID_RX_STOP:
      ret = TAPI_Phone_CidRx_Stop (pChannel);
      break;

   case IFX_TAPI_CID_RX_STATUS_GET:
   {
      IFX_TAPI_CID_RX_STATUS_t *p_tmp = (IFX_TAPI_CID_RX_STATUS_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CID_RX_STATUS_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_LL_Phone_CidRx_Status (pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg, p_tmp,
                                   sizeof(IFX_TAPI_CID_RX_STATUS_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }

   case IFX_TAPI_CID_RX_DATA_GET:
   {
      IFX_TAPI_CID_RX_DATA_t *p_tmp = (IFX_TAPI_CID_RX_DATA_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_CID_RX_DATA_t) < TAPI_IOCTL_STACKSIZE);
      ret = TAPI_Phone_Get_CidRxData (pChannel, p_tmp);
      if ((ret == IFX_SUCCESS) && (copy_to_user ( parg, p_tmp,
           sizeof(IFX_TAPI_CID_RX_DATA_t)) > 0))
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: cannot copy to user space\n\r"));
         ret = IFX_ERROR;
      }
      break;
   }
#endif /* TAPI_CID */

   /* Report Level */
   case IFX_TAPI_DEBUG_REPORT_SET:
      SetTraceLevel(TAPI_DRV, arg);
      break;

   case IFX_TAPI_VERSION_CHECK:
   {
      IFX_TAPI_VERSION_t *p_vers = (IFX_TAPI_VERSION_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_VERSION_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_vers, parg, sizeof(IFX_TAPI_VERSION_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_Check_Version (p_vers);
      }
      break;
   }

   case IFX_TAPI_PHONE_VOLUME_SET:
   {
      IFX_TAPI_LINE_VOLUME_t *p_tmp = (IFX_TAPI_LINE_VOLUME_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LINE_VOLUME_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_LINE_VOLUME_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Phone_Volume (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_PCM_VOLUME_SET:
   {
      IFX_TAPI_LINE_VOLUME_t *p_tmp = (IFX_TAPI_LINE_VOLUME_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_LINE_VOLUME_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_LINE_VOLUME_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Phone_Pcm_Volume (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_LINE_LEVEL_SET:
   {
      ret = TAPI_LL_Phone_High_Level (pChannel, arg);
      break;
   }

   case IFX_TAPI_SIG_DETECT_ENABLE:
   {
      IFX_TAPI_SIG_DETECTION_t *p_tmp = (IFX_TAPI_SIG_DETECTION_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_SIG_DETECTION_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_SIG_DETECTION_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Enable_Signal (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_SIG_DETECT_DISABLE:
   {
      IFX_TAPI_SIG_DETECTION_t *p_tmp = (IFX_TAPI_SIG_DETECTION_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_SIG_DETECTION_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_SIG_DETECTION_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Disable_Signal (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_TONE_TABLE_CFG_SET:
   {
      IFX_TAPI_TONE_t *p_tmp = (IFX_TAPI_TONE_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_TONE_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_TONE_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         if (pChannel->pTapiDevice->pToneTbl)
         {
            ret = TAPI_Phone_Tone_TableConf (pChannel->pTapiDevice->pToneTbl,
                                             p_tmp);
         }
         else /* reluctant success indication */
            ret = IFX_SUCCESS;
      }
      break;
   }

   case IFX_TAPI_TONE_CPTD_START:
   {
      IFX_TAPI_TONE_CPTD_t *p_tmp = (IFX_TAPI_TONE_CPTD_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_TONE_CPTD_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_TONE_CPTD_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_Phone_DetectToneStart(pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_TONE_CPTD_STOP:
   {
      ret = TAPI_Phone_DetectToneStop(pChannel);
      break;
   }

   case IFX_TAPI_ENC_AGC_CFG:
   {
      IFX_TAPI_ENC_AGC_CFG_t *p_tmp = (IFX_TAPI_ENC_AGC_CFG_t *)p_iostack;
      IFXOS_ASSERT(sizeof(IFX_TAPI_ENC_AGC_CFG_t) < TAPI_IOCTL_STACKSIZE);
      if (copy_from_user (p_tmp, parg, sizeof(IFX_TAPI_ENC_AGC_CFG_t)) > 0 )
      {
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_Phone_AGC_Cfg (pChannel, p_tmp);
      }
      break;
   }

   case IFX_TAPI_ENC_AGC_ENABLE:
   {
      TAPI_LL_Phone_AGC_Enable (pChannel, arg);
      break;
   }


/* case IFX_TAPI_TEST_HOOKGEN:
      ret = TAPI_LL_Test_HookGen (pChannel, (IFX_boolean_t) arg);*/
   case IFX_TAPI_TEST_LOOP:
   {
      IFX_TAPI_TEST_LOOP_t testLoop;

      if (copy_from_user (&testLoop, parg, sizeof(IFX_TAPI_TEST_LOOP_t)) > 0)
      {
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: IFX_TAPI_TEST_LOOP: "
               "cannot copy from user space\n\r"));
         ret = IFX_ERROR;
      }
      else
      {
         ret = TAPI_LL_TestLoop (pChannel, &testLoop);
      }
      break;
   }

   default:
      TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: Ioctl 0x%08lX not handled by TAPI\n\r", cmd));
      ret = IFX_ERROR;
      break;
   }

   IFXOS_FREE(p_iostack);
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
IFX_void_t* TAPI_MapBuffer(IFX_void_t* p_buffer, IFX_uint32_t size)
{
   IFX_void_t* kernel_buffer;
   if (p_buffer == NULL)
      return NULL;
   kernel_buffer = vmalloc(size);
   if (kernel_buffer == NULL)
      return NULL;
   if (copy_from_user(kernel_buffer, p_buffer, size) > 0)
   {
      vfree(kernel_buffer);
      return NULL;
   }
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
IFX_void_t TAPI_UnmapBuffer(IFX_void_t* p_buffer)
{
   if (p_buffer != NULL)
      vfree(p_buffer);
}
#endif /* TAPI_CID */

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

#endif /* LINUX */


