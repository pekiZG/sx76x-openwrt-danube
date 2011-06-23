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
   Module      : drv_tapi_vxworks.c
   Desription  : Contains OS functions for VxWorks.
*******************************************************************************/

#ifdef TAPI_CUSTOMER_BOARD
#include "drv_pots_proxy.c"
#else /* TAPI_CUSTOMER_BOARD */

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

#ifdef VXWORKS

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Function create a timer.
Arguments:
   pTimerEntry - Functionpointer to the call back function
   nArgument   - Argument including the Dev structure
                 (as integer pointer)
Return:
   Timer       - Timer ID handle for vxWorks
*******************************************************************************/
Timer_ID TAPI_Create_Timer(TIMER_ENTRY pTimerEntry, IFX_int32_t nArgument)
{
   /* use common timer functions if sys_timerlib_vxworks.c is included (done in BSPProj.c) */
#ifndef SYS_TIMERLIB_VXWORKS_H
   Timer_ID Timer;

   /* derive timer from CLK realtimer, do not use signal handler */
   if(timer_create(CLOCK_REALTIME, NULL, &Timer) == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_create (TAPI_Create_Timer)\n"));
      return (0);
   }

   /* connect timer to interrupt function */
   if(timer_connect(Timer, pTimerEntry, nArgument) == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_connect (TAPI_Create_Timer)\n"));
      return (0);
   }

   return (Timer);
#else
   return (InstallTimer(pTimerEntry,0,nArgument, IFX_FALSE));
#endif
}

/*******************************************************************************
Description:
   Function set and starts a timer with a specific time. It can be choose if the
   timer starts periodically.
Arguments:
   Timer_ID      - Timer ID handle for vxWorks
   nTime         - Time in ms
   bPeriodically - Starts the timer periodically or not
   bRestart      - Restart the timer or normal start
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/
IFX_boolean_t TAPI_SetTime_Timer(Timer_ID Timer, IFX_uint32_t nTime, IFX_boolean_t bPeriodically, IFX_boolean_t bRestart)
{
/* use common timer functions if sys_timerlib_vxworks.c is included (done in BSPProj.c) */
#ifndef SYS_TIMERLIB_VXWORKS_H
   struct itimerspec   timeToSet;        /* time to be set */
   struct timespec     timeValue;        /* timer expiration value */
   struct timespec     timeInterval;     /* timer period */

   /* stop timer */
   if(bRestart)
   {
      if(timer_cancel(Timer) == IFX_ERROR)
      {
         LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_cancel (TAPI_SetTime_Timer)\n"));
         return (IFX_FALSE);
      }
   }

   /* Initialize timer expiration value */
   timeValue.tv_sec        = (nTime/1000);
   timeValue.tv_nsec       = (nTime%1000) * 1000 * 1000;

   /* Initialize timer period */
   if (bPeriodically)
   {
      timeInterval.tv_sec     = (nTime/1000);
      timeInterval.tv_nsec    = (nTime%1000) * 1000 * 1000;
   }
   else
   {
      timeInterval.tv_sec     = 0;
      timeInterval.tv_nsec    = 0;
   }

   /* reset timer structure */
   memset ((IFX_char_t *) &timeToSet, 0, sizeof (struct itimerspec));

   /* Set the time to be set value */
   /* copy all parameter in simple steps. This is a workaround to avoid crashes
      on the CheckBoard, because of incompatiple compiler versions */
   timeToSet.it_value.tv_sec      = timeValue.tv_sec;
   timeToSet.it_value.tv_nsec      = timeValue.tv_nsec;
   timeToSet.it_interval.tv_sec   = timeInterval.tv_sec;
   timeToSet.it_interval.tv_nsec   = timeInterval.tv_nsec;

   /* pass timer value & reload value */
   /* Do not use 'TIMER_ABSTIME' flag because timer will then expire immediatly */
   if(timer_settime(Timer, 0, &timeToSet, NULL) == IFX_ERROR)
   {
      return (IFX_FALSE);
   }

   return (IFX_TRUE);
#else
   return (SetTimeTimer(Timer, nTime, bPeriodically, bRestart));
#endif
}

/*******************************************************************************
Description:
   Function stop a timer.
Arguments:
   Timer_ID      - Timer ID handle for vxWorks
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/

IFX_boolean_t TAPI_Stop_Timer(Timer_ID Timer)
{
#ifndef SYS_TIMERLIB_VXWORKS_H
   /* stop timer */
   if(timer_cancel(Timer) == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_cancel (TAPI_Stop_Timer)\n"));
      return (IFX_FALSE);
   }

   return (IFX_TRUE);
#else
   return (StopTimer(Timer));
#endif
}

/*******************************************************************************
Description:
   Function delete a timer.
Arguments:
   Timer_ID      - Timer ID handle for vxWorks
Return:
   Returns an error code: IFX_TRUE / IFX_FALSE
*******************************************************************************/

IFX_boolean_t TAPI_Delete_Timer(Timer_ID Timer)
{
#ifndef SYS_TIMERLIB_VXWORKS_H
   /* stop timer */
   if(timer_cancel(Timer) == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_cancel (TAPI_Delete_Timer)\n"));
      return (IFX_FALSE);
   }
   /* delete timer */
   if(timer_delete(Timer) == IFX_ERROR)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,("\nDRV_ERROR: timer_delete (TAPI_Delete_Timer)\n"));
      return (IFX_FALSE);
   }

   return (IFX_TRUE);
#else
   return (DeleteTimer(Timer));
#endif
}

/*=================*/
/* Tapi iocontrols */
/*=================*/

/*******************************************************************************
Description:
   I/O control of the TAPI
Arguments:
   pChannel  - handle to TAPI channel specific structure
   cmd       - I/O control command
   arg       - I/O control argument
Return Value:
   Returns error code in case of an error, otherwise returns ok.
*******************************************************************************/
IFX_int32_t TAPI_Ioctl(TAPI_CONNECTION *pChannel, IFX_uint32_t cmd, IFX_uint32_t arg)
{
   switch(cmd)
   {
      /* Ringing Services */
      case IFXPHONE_RING_CONFIG:
         return TAPI_Phone_Ring_Config(pChannel, (TAPI_RING_CONFIG *) arg);
      case IFXPHONE_RING_GET_CONFIG:
         return TAPI_Phone_Ring_GetConfig(pChannel, (TAPI_RING_CONFIG*)arg);
      case PHONE_RING_CADENCE:
         return TAPI_Phone_Ring_Cadence(pChannel, arg);
      case IFXPHONE_RING_CADENCE_HIGH_RES:
      {
         TAPI_RING_CADENCE *pCadence = (TAPI_RING_CADENCE*) arg;
         return TAPI_Phone_Ring_Cadence_High_Res(pChannel, pCadence);
      }
      case PHONE_RING_START:
         return TAPI_Phone_Ring_Start(pChannel, (TAPI_PHONE_CID *)arg);
      case PHONE_RING_STOP:
         return TAPI_Phone_Ring_Stop(pChannel);
      case PHONE_MAXRINGS:
         return TAPI_Phone_Set_MaxRings(pChannel, arg);
      case PHONE_RING:
         return TAPI_Phone_Ring(pChannel);

      /* Operation Control Services */
      case IFXPHONE_SET_LINEFEED:
         return TAPI_Phone_Set_Linefeed(pChannel, arg);
      case PHONE_HOOKSTATE:
         return TAPI_Phone_Hookstate(pChannel, (IFX_int32_t *) arg);
      /* PCM Services */
      case IFXPHONE_SET_PCM_CONFIG:
      {
         TAPI_PCM_CONFIG *pPCMConfig = (TAPI_PCM_CONFIG*) arg;
         return TAPI_Phone_PCM_Set_Config(pChannel, pPCMConfig);
      }
      case IFXPHONE_GET_PCM_CONFIG:
      {
         TAPI_PCM_CONFIG *pPCMConfig = (TAPI_PCM_CONFIG*) arg;
         return TAPI_Phone_PCM_Get_Config(pChannel, pPCMConfig);
      }
      case IFXPHONE_SET_PCM_ACTIVATION:
         return TAPI_Phone_PCM_Set_Activation(pChannel, arg);
      case IFXPHONE_GET_PCM_ACTIVATION:
         return TAPI_Phone_PCM_Get_Activation(pChannel);

      /* Tone Services */
      case PHONE_PLAY_TONE:
         return TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
      case IFXPHONE_TONE_PLAY_NET:
         return TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_NET);
      case PHONE_SET_TONE_ON_TIME:
         return TAPI_Phone_Tone_Set_On_Time(pChannel, arg);
      case PHONE_SET_TONE_OFF_TIME:
         return TAPI_Phone_Tone_Set_Off_Time(pChannel, arg);
      case PHONE_GET_TONE_ON_TIME:
         return TAPI_Phone_Tone_Get_On_Time(pChannel, (IFX_uint32_t *)arg);
      case PHONE_GET_TONE_OFF_TIME:
         return TAPI_Phone_Tone_Get_Off_Time(pChannel, (IFX_uint32_t *)arg);
      case PHONE_GET_TONE_STATE:
         return TAPI_Phone_Tone_Get_State(pChannel, (IFX_uint32_t *)arg);
      case IFXPHONE_SET_TONE_LEVEL:
      {
         TAPI_TONE_LEVEL *pToneLevel = (TAPI_TONE_LEVEL *) arg;
         return TAPI_Phone_Tone_Set_Level(pChannel, pToneLevel);
      }
      case PHONE_DIALTONE:
         return TAPI_Phone_Tone_Dial(pChannel);
      case PHONE_RINGBACK:
         return TAPI_Phone_Tone_Ringback(pChannel);
      case PHONE_BUSY:
         return TAPI_Phone_Tone_Busy(pChannel);
      case PHONE_CPT_STOP:
         return TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
      /* Complex Tones */
      case IFXPHONE_TONETABLE_CONF:
         return TAPI_Phone_Tone_TableConf(pChannel->pTapiDevice->pToneTbl,
                                           (TAPI_TONE *)arg);
      /* Dial Services */
      case IFXPHONE_GET_PULSE:
         return TAPI_Phone_Get_Pulse(pChannel, (IFX_int32_t *) arg);
      case IFXPHONE_GET_PULSE_ASCII:
         return TAPI_Phone_Get_Pulse_ASCII(pChannel, (IFX_int32_t *) arg);
      case IFXPHONE_PULSE_READY:
         return TAPI_Phone_Pulse_Ready (pChannel, (IFX_int32_t *) arg);
      case IFXPHONE_VALIDATION_TIME:
         return TAPI_Phone_Validation_Time(pChannel,
                 (TAPI_VALIDATION_TIMES *)arg);
#ifdef TAPI_DTMF
      case PHONE_GET_DTMF:
         return TAPI_Phone_Get_DTMF(pChannel, (IFX_int32_t *) arg);
      case PHONE_GET_DTMF_ASCII:
         return TAPI_Phone_Get_DTMF_ASCII(pChannel, (IFX_int32_t *) arg);
      case PHONE_DTMF_READY:
         return TAPI_Phone_DTMF_Ready(pChannel, (IFX_int32_t *) arg);
      case PHONE_DTMF_OOB:
         return TAPI_LL_Phone_Send_DTMF_OOB(pChannel, arg);
#endif /* TAPI_DTMF */

      /* Miscellaneous Services */
      case IFXPHONE_GET_VERSION:
         return TAPI_Phone_Get_Version((IFX_char_t*)arg);
      case PHONE_EXCEPTION:
         return TAPI_Phone_Exception(pChannel);
      case IFXPHONE_MASK_EXCEPTION:
         TAPI_Phone_Mask_Exception(pChannel, arg);
         return IFX_SUCCESS;
      case IFXPHONE_INIT:
         return TAPI_Phone_Init(pChannel, (TAPI_INIT *)arg);
      case PHONE_VAD:
         return TAPI_LL_Phone_VAD(pChannel, arg);
      case PHONE_CAPABILITIES:
         TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: PHONE_CAPABILITIES\n"));
         return TAPI_LL_Phone_Get_Capabilities(pChannel);
      case PHONE_CAPABILITIES_LIST:
      {
         TAPI_PHONE_CAPABILITY *pPhoneCap = (TAPI_PHONE_CAPABILITY *) arg;
         TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: PHONE_CAPABILITIES_LIST\n"));
         return TAPI_LL_Phone_Get_Capability_List(pChannel, pPhoneCap);
      }
      case PHONE_CAPABILITIES_CHECK:
      {
         TAPI_PHONE_CAPABILITY *pPhoneCap = (TAPI_PHONE_CAPABILITY *) arg;
         TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: PHONE_CAPABILITIES_CHECK\n"));
         return TAPI_LL_Phone_Check_Capability(pChannel, pPhoneCap);
      }
#ifdef TAPI_VOICE
      case PHONE_FRAME:
         TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: PHONE_FRAME\n"));
         return TAPI_LL_Phone_Set_Frame_Length(pChannel, arg);
      case IFXPHONE_GET_FRAME:
         TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: IFXPHONE_GET_FRAME\n"));
         return TAPI_LL_Phone_Get_Frame_Length(pChannel, (IFX_int32_t *)arg);
#endif /* TAPI_VOICE */

      /* Metering Services */
      case IFXPHONE_METER_CHAR:
      {
         TAPI_METER_CONFIG *pMeterConfig = (TAPI_METER_CONFIG *) arg;
         return TAPI_Phone_Meter_Config(pChannel, pMeterConfig);
      }
      case IFXPHONE_METER_START:
         return TAPI_Phone_Meter_Start(pChannel);
      case IFXPHONE_METER_STOP:
         return TAPI_Phone_Meter_Stop(pChannel);

      /* Lec Configuration */
      case IFXPHONE_LECCONF:
         return TAPI_Phone_LecConf (pChannel, (TAPI_LECCONF *)arg);
      case IFXPHONE_GETLECCONF:
         return TAPI_Phone_GetLecConf (pChannel, (TAPI_LECCONF *)arg);
      case IFXPHONE_LECCONF_PCM:
         return TAPI_Phone_LecConf_Pcm (pChannel, (TAPI_LECCONF *)arg);
      case IFXPHONE_GETLECCONF_PCM:
         return TAPI_Phone_GetLecConf_Pcm (pChannel, (TAPI_LECCONF *)arg);

      /* AGC Configuration */
      case IFXPHONE_AGC:
         return TAPI_LL_Phone_AgcEnable (pChannel, (TAPI_AGC_MODE)arg);

#ifdef TAPI_CID
        /* Caller ID Transmission service */
      case IFXPHONE_CIDCONF:
         return TAPI_Phone_CID_Config (pChannel, (TAPI_CID_CONFIG*)arg);
      case IFXPHONE_GETCIDCONF:
         return TAPI_Phone_CID_GetConfig (pChannel, (TAPI_CID_CONFIG*)arg);
      case IFXPHONE_DTMFCID_CONF:
         return TAPI_Phone_CID_ConfigDtmf (pChannel, (TAPI_DTMFCID*)arg);
      case IFXPHONE_CID_INFO_TX:
         return TAPI_Phone_CID_Info_Tx (pChannel, (TAPI_CID_INFO_t *)arg);

      /* Caller ID Reception service */
      case IFXPHONE_CIDRX_START:
         return TAPI_Phone_CidRx_Start (pChannel);
      case IFXPHONE_CIDRX_STOP:
         return TAPI_Phone_CidRx_Stop (pChannel);
      case IFXPHONE_CIDRX_STATUS:
         return TAPI_LL_Phone_CidRx_Status (pChannel, (TAPI_CIDRX_STATUS *) arg);
      case IFXPHONE_CIDRX_DATA:
         return TAPI_Phone_Get_CidRxData (pChannel, (TAPI_CIDRX_DATA *) arg);
      case IFXPHONE_CID2_CONF:
         return TAPI_Phone_CID2_Config (pChannel, (TAPI_CID2_CONFIG *) arg);
      case IFXPHONE_CID2_GETCONF:
         return TAPI_Phone_CID2_GetConfig (pChannel, (TAPI_CID2_CONFIG *) arg);
      case IFXPHONE_CID2_SEND:
         return TAPI_Phone_CID2_Send (pChannel, (TAPI_PHONE_CID *)arg);
#endif /* TAPI_CID */
#ifdef TAPI_VOICE
      /* Connection Services*/
      case IFXPHONE_DATA_ADD:
         return TAPI_Data_Channel_Add (pChannel, (TAPI_DATA_MAPPING *)arg);
      case IFXPHONE_DATA_REMOVE:
         return TAPI_Data_Channel_Remove (pChannel, (TAPI_DATA_MAPPING *)arg);
      case IFXPHONE_PHONE_ADD:
         return TAPI_Phone_Channel_Add (pChannel, (TAPI_PHONE_MAPPING *)arg);
      case IFXPHONE_PHONE_REMOVE:
         return TAPI_Phone_Channel_Remove (pChannel, (TAPI_PHONE_MAPPING *)arg);
      case IFXPHONE_PCM_ADD:
         return TAPI_LL_PCM_Channel_Add (pChannel, (TAPI_PCM_MAPPING *)arg);
      case IFXPHONE_PCM_REMOVE:
         return TAPI_LL_PCM_Channel_Remove (pChannel, (TAPI_PCM_MAPPING *)arg);
      case IFXPHONE_RTP_CONF:
         return TAPI_LL_Phone_RtpConf (pChannel, (TAPI_RTP_CONF*)arg);
      case IFXPHONE_RTP_PTCONF:
         return TAPI_LL_Phone_Rtp_SetPayloadType (pChannel, (TAPI_RTP_PT_CONF *)arg);
      case IFXPHONE_RTCP_STATISTICS:
         return TAPI_LL_Phone_RTCP_GetStatistics(pChannel, (TAPI_RTCP_DATA *)arg);
      case IFXPHONE_RTCP_RESET:
         return TAPI_LL_Phone_RTCP_Reset (pChannel);
      case IFXPHONE_JB_CONF:
         return TAPI_LL_Phone_JB_Set (pChannel, (TAPI_JB_CONF *)arg);
      case IFXPHONE_JB_STATISTICS:
         return TAPI_LL_Phone_JB_GetStatistics(pChannel, (TAPI_JB_DATA *)arg);
      case IFXPHONE_JB_RESET:
         return TAPI_LL_Phone_JB_Reset (pChannel);
      case IFXPHONE_AAL_CONF:
         return TAPI_LL_Phone_Aal_Set (pChannel, (TAPI_AAL_CONF *)arg);
      case IFXPHONE_AAL_PROFILE:
         return TAPI_LL_Phone_AalProfile (pChannel, (TAPI_AAL_PROFILE *)arg);
      /* Recording services */
      case PHONE_REC_CODEC:
         return TAPI_LL_Phone_Set_Rec_Codec (pChannel, arg);
      case PHONE_REC_START:
         return TAPI_LL_Phone_Start_Recording(pChannel);
      case PHONE_REC_STOP:
         return TAPI_LL_Phone_Stop_Recording(pChannel);
      case IFXPHONE_REC_HOLD:
         return TAPI_Phone_SetRecHold (pChannel, (IFX_int32_t)arg);
      case PHONE_REC_DEPTH:
         return TAPI_Phone_SetRecDepth (pChannel, (IFX_int32_t)arg);
      case PHONE_REC_VOLUME:
         return TAPI_Phone_SetRecVolume (pChannel, (IFX_int32_t)arg);
      case PHONE_REC_LEVEL:
         return TAPI_Phone_GetRecLevel (pChannel, (IFX_int32_t *)arg);
      /* Play Services*/
      /* not used */
      /*
      case PHONE_PLAY_CODEC:
         return(TAPI_Phone_Set_Play_Codec(pChannel, arg));
      */
      case PHONE_PLAY_START:
         return TAPI_LL_Phone_Start_Playing(pChannel);
      case PHONE_PLAY_STOP:
         return TAPI_LL_Phone_Stop_Playing(pChannel);
      case IFXPHONE_PLAY_HOLD:
         return TAPI_Phone_SetPlayHold (pChannel, (IFX_int32_t)arg);
      case PHONE_PLAY_DEPTH:
         return TAPI_Phone_SetPlayDepth (pChannel, (IFX_int32_t) arg);
      case PHONE_PLAY_VOLUME:
         return TAPI_Phone_SetPlayVolume (pChannel, (IFX_int32_t)arg);
      case PHONE_PLAY_LEVEL:
         return TAPI_Phone_GetPlayLevel (pChannel, (IFX_int32_t *)arg);

#endif /* TAPI_VOICE */
#ifdef TAPI_FAX_T38
      /* Fax Services */
      case IFXPHONE_FAX_SETMOD:
         return TAPI_LL_FaxT38_SetModulator (pChannel,
            (TAPI_FAX_MODDATA *)arg);
      case IFXPHONE_FAX_SETDEMOD:
         return TAPI_LL_FaxT38_SetDemodulator (pChannel,
            (TAPI_FAX_DEMODDATA *)arg);
      case IFXPHONE_FAX_DISABLE:
         return TAPI_LL_FaxT38_DisableDataPump (pChannel);
      case IFXPHONE_FAX_STATUS:
         return TAPI_LL_FaxT38_GetStatus (pChannel, (TAPI_FAX_STATUS *)arg);
#endif /* TAPI_FAX_T38 */
      /* Report Level */
      case IFXPHONE_REPORT_SET:
         SetTraceLevel(TAPI_DRV, arg);
         return IFX_SUCCESS;
      case IFXPHONE_PHONE_STATUS:
         return TAPI_Phone_GetStatus (pChannel, (TAPI_PHONE_STATUS *)arg);

      case IFXPHONE_VERSION_CHECK:
         return TAPI_Phone_Check_Version ((TAPI_PHONE_VERSION *)arg);

      case IFXPHONE_VOLUME:
         return TAPI_LL_Phone_Volume (pChannel, (TAPI_PHONE_VOLUME *)arg);

      case IFXPHONE_PCM_VOLUME:
         return TAPI_LL_Phone_Pcm_Volume (pChannel, (TAPI_PHONE_VOLUME *)arg);

      case IFXPHONE_HIGH_LEVEL:
         return TAPI_LL_Phone_High_Level (pChannel, arg);

      case IFXPHONE_ENABLE_SIGDETECT:
         return TAPI_LL_Enable_Signal (pChannel,
            (TAPI_PHONE_SIGDETECTION *)arg);
      case IFXPHONE_DISABLE_SIGDETECT:
         return TAPI_LL_Disable_Signal (pChannel,
            (TAPI_PHONE_SIGDETECTION *)arg);

      case IFXPHONE_CPTD_START:
         return TAPI_Phone_DetectToneStart(pChannel, (TAPI_PHONE_CPTD*) arg);
      case IFXPHONE_CPTD_STOP:
         return TAPI_Phone_DetectToneStop(pChannel);

      default:
         TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: unknown ioctl 0x%lx\n",cmd));
         return IFX_ERROR;
   }
}

#endif /* TAPI_CUSTOMER_BOARD */

#endif /* VXWORKS */

