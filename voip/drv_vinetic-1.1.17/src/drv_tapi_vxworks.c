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
   case IFX_TAPI_RING_CFG_SET:
      return TAPI_Phone_Ring_Config(pChannel, (IFX_TAPI_RING_CFG_t *) arg);
   case IFX_TAPI_RING_CFG_GET:
      return TAPI_Phone_Ring_GetConfig(pChannel, (IFX_TAPI_RING_CFG_t*)arg);
   case IFX_TAPI_RING_CADENCE_SET:
      return TAPI_Phone_Ring_Cadence(pChannel, arg);
   case IFX_TAPI_RING_CADENCE_HR_SET:
   {
      IFX_TAPI_RING_CADENCE_t *pCadence = (IFX_TAPI_RING_CADENCE_t*) arg;
      return TAPI_Phone_Ring_Cadence_High_Res(pChannel, pCadence);
   }
   case IFX_TAPI_RING_START:
      return TAPI_Phone_Ring_Start(pChannel);

   case IFX_TAPI_RING_STOP:
      return TAPI_Phone_Ring_Stop(pChannel);
   case IFX_TAPI_RING_MAX_SET:
      return TAPI_Phone_Set_MaxRings(pChannel, arg);
   case IFX_TAPI_RING:
      return TAPI_Phone_Ring(pChannel);

   /* Operation Control Services */
   case IFX_TAPI_LINE_FEED_SET:
      return TAPI_Phone_Set_Linefeed(pChannel, arg);
   case IFX_TAPI_LINE_HOOK_STATUS_GET:
      return TAPI_Phone_Hookstate(pChannel, (IFX_int32_t *) arg);
   /* PCM Services */
   case IFX_TAPI_PCM_CFG_SET:
   {
      IFX_TAPI_PCM_CFG_t *pPCMConfig = (IFX_TAPI_PCM_CFG_t*) arg;
      return TAPI_Phone_PCM_Set_Config(pChannel, pPCMConfig);
   }
   case IFX_TAPI_PCM_CFG_GET:
   {
      IFX_TAPI_PCM_CFG_t *pPCMConfig = (IFX_TAPI_PCM_CFG_t*) arg;
      return TAPI_Phone_PCM_Get_Config(pChannel, pPCMConfig);
   }
   case IFX_TAPI_PCM_ACTIVATION_SET:
      return TAPI_Phone_PCM_Set_Activation(pChannel, arg);
   case IFX_TAPI_PCM_ACTIVATION_GET:
      return TAPI_Phone_PCM_Get_Activation(pChannel, (IFX_boolean_t*) arg);

   /* Tone Services */
   case IFX_TAPI_TONE_LOCAL_PLAY:
      return TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_LOCAL);
   case IFX_TAPI_TONE_NET_PLAY:
      return TAPI_Phone_Tone_Play(pChannel, arg, TAPI_TONE_DST_NET);
   case IFX_TAPI_TONE_ON_TIME_SET:
      return TAPI_Phone_Tone_Set_On_Time(pChannel, arg);
   case IFX_TAPI_TONE_OFF_TIME_SET:
      return TAPI_Phone_Tone_Set_Off_Time(pChannel, arg);
   case IFX_TAPI_TONE_ON_TIME_GET:
      return TAPI_Phone_Tone_Get_On_Time(pChannel, (IFX_uint32_t *)arg);
   case IFX_TAPI_TONE_OFF_TIME_GET:
      return TAPI_Phone_Tone_Get_Off_Time(pChannel, (IFX_uint32_t *)arg);
   case IFX_TAPI_TONE_STATUS_GET:
      return TAPI_Phone_Tone_Get_State(pChannel, (IFX_uint32_t *)arg);
   case IFX_TAPI_TONE_LEVEL_SET:
   {
      IFX_TAPI_PREDEF_TONE_LEVEL_t *pToneLevel = (IFX_TAPI_PREDEF_TONE_LEVEL_t *) arg;
      return TAPI_Phone_Tone_Set_Level(pChannel, pToneLevel);
   }
   case IFX_TAPI_TONE_DIALTONE_PLAY:
      return TAPI_Phone_Tone_Dial(pChannel);
   case IFX_TAPI_TONE_RINGBACK_PLAY:
      return TAPI_Phone_Tone_Ringback(pChannel);
   case IFX_TAPI_TONE_BUSY_PLAY:
      return TAPI_Phone_Tone_Busy(pChannel);
   case IFX_TAPI_TONE_STOP:
      return TAPI_Phone_Tone_Stop (pChannel, arg);
   /* Complex Tones */
   case IFX_TAPI_TONE_TABLE_CFG_SET:
   {
      if (pChannel->pTapiDevice->pToneTbl)
         return TAPI_Phone_Tone_TableConf(pChannel->pTapiDevice->pToneTbl,
               (IFX_TAPI_TONE_t *)arg);
      else /* reluctant success indication */
         return IFX_SUCCESS;
   }
   /* Dial Services */
   case IFX_TAPI_PULSE_GET:
      return TAPI_Phone_Get_Pulse(pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_PULSE_ASCII_GET:
      return TAPI_Phone_Get_Pulse_ASCII(pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_PULSE_READY:
      return TAPI_Phone_Pulse_Ready (pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_LINE_HOOK_VT_SET:
      return TAPI_Phone_Validation_Time(pChannel,
         (IFX_TAPI_LINE_HOOK_VT_t *)arg);
#ifdef TAPI_DTMF
   case IFX_TAPI_TONE_DTMF_GET:
      return TAPI_Phone_Get_DTMF(pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_TONE_DTMF_ASCII_GET:
      return TAPI_Phone_Get_DTMF_ASCII(pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_TONE_DTMF_READY_GET:
      return TAPI_Phone_DTMF_Ready(pChannel, (IFX_int32_t *) arg);
   case IFX_TAPI_PKT_EV_OOB_SET:
      return TAPI_LL_Phone_Send_DTMF_OOB(pChannel, arg);
#endif /* TAPI_DTMF */

   /* Miscellaneous Services */
   case IFX_TAPI_VERSION_GET:
      return TAPI_Phone_Get_Version((IFX_char_t*)arg);
   case IFX_TAPI_EXCEPTION_GET:
      return TAPI_Phone_Exception(pChannel);
   case IFX_TAPI_EXCEPTION_MASK:
      TAPI_Phone_Mask_Exception(pChannel, arg);
      return IFX_SUCCESS;
   case IFX_TAPI_CH_INIT:
      return TAPI_Phone_Init(pChannel, (IFX_TAPI_CH_INIT_t *)arg);
   case IFX_TAPI_ENC_VAD_CFG_SET:
      return TAPI_LL_Phone_VAD(pChannel, arg);
   case IFX_TAPI_CAP_NR:
      TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: IFX_TAPI_CAP_NR\n"));
      return TAPI_LL_Phone_Get_Capabilities(pChannel);
   case IFX_TAPI_CAP_LIST:
   {
      IFX_TAPI_CAP_t *pPhoneCap = (IFX_TAPI_CAP_t *) arg;
      TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: PHONE_CAPABILITIES_LIST\n"));
      return TAPI_LL_Phone_Get_Capability_List(pChannel, pPhoneCap);
   }
   case IFX_TAPI_CAP_CHECK:
   {
      IFX_TAPI_CAP_t *pPhoneCap = (IFX_TAPI_CAP_t *) arg;
      TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: IFX_TAPI_CAP_CHECK\n"));
      return TAPI_LL_Phone_Check_Capability(pChannel, pPhoneCap);
   }
#ifdef TAPI_VOICE
   case IFX_TAPI_ENC_FRAME_LEN_SET:
      TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: IFX_TAPI_ENC_FRAME_LEN_SET\n"));
      return TAPI_LL_Phone_Set_Frame_Length(pChannel, arg);
   case IFX_TAPI_ENC_FRAME_LEN_GET:
      TRACE(TAPI_DRV,DBG_LEVEL_LOW,("TAPI: IFX_TAPI_ENC_FRAME_LEN_GET\n"));
      return TAPI_LL_Phone_Get_Frame_Length(pChannel, (IFX_int32_t *)arg);
#endif /* TAPI_VOICE */

   /* Metering Services */
   case IFX_TAPI_METER_CFG_SET:
   {
      IFX_TAPI_METER_CFG_t *pMeterConfig = (IFX_TAPI_METER_CFG_t *) arg;
      return TAPI_Phone_Meter_Config(pChannel, pMeterConfig);
   }
   case IFX_TAPI_METER_START:
      return TAPI_Phone_Meter_Start(pChannel);
   case IFX_TAPI_METER_STOP:
      return TAPI_Phone_Meter_Stop(pChannel);

   /* Lec Configuration */
   case IFX_TAPI_LEC_PHONE_CFG_SET:
      return TAPI_Phone_LecConf (pChannel, (IFX_TAPI_LEC_CFG_t *)arg);
   case IFX_TAPI_LEC_PHONE_CFG_GET:
      return TAPI_Phone_GetLecConf (pChannel, (IFX_TAPI_LEC_CFG_t *)arg);
   case IFX_TAPI_LEC_PCM_CFG_SET:
      return TAPI_Phone_LecConf_Pcm (pChannel, (IFX_TAPI_LEC_CFG_t *)arg);
   case IFX_TAPI_LEC_PCM_CFG_GET:
      return TAPI_Phone_GetLecConf_Pcm (pChannel, (IFX_TAPI_LEC_CFG_t *)arg);
#ifdef TAPI_CID
   /* Caller ID Transmission service */
   case IFX_TAPI_CID_CFG_SET:
      return TAPI_Phone_CID_SetConfig (pChannel, (IFX_TAPI_CID_CFG_t *)arg);
   case IFX_TAPI_CID_TX_INFO_START:
      return TAPI_Phone_CID_Info_Tx (pChannel, (IFX_TAPI_CID_MSG_t *)arg);
   case IFX_TAPI_CID_TX_SEQ_START:
      return TAPI_Phone_CID_Seq_Tx (pChannel, (IFX_TAPI_CID_MSG_t *)arg);

   /* Caller ID Reception service */
   case IFX_TAPI_CID_RX_START:
      return TAPI_Phone_CidRx_Start (pChannel);
   case IFX_TAPI_CID_RX_STOP:
      return TAPI_Phone_CidRx_Stop (pChannel);
   case IFX_TAPI_CID_RX_STATUS_GET:
      return TAPI_LL_Phone_CidRx_Status (pChannel, (IFX_TAPI_CID_RX_STATUS_t *) arg);
   case IFX_TAPI_CID_RX_DATA_GET:
      return TAPI_Phone_Get_CidRxData (pChannel, (IFX_TAPI_CID_RX_DATA_t *) arg);
#endif /* TAPI_CID */
#ifdef TAPI_VOICE
   /* Connection Services*/
   case IFX_TAPI_MAP_DATA_ADD:
      return TAPI_Data_Channel_Add (pChannel, (IFX_TAPI_MAP_DATA_t *)arg);
   case IFX_TAPI_MAP_DATA_REMOVE:
      return TAPI_Data_Channel_Remove (pChannel, (IFX_TAPI_MAP_DATA_t *)arg);
   case IFX_TAPI_MAP_PHONE_ADD:
      return TAPI_Phone_Channel_Add (pChannel, (IFX_TAPI_MAP_PHONE_t *)arg);
   case IFX_TAPI_MAP_PHONE_REMOVE:
      return TAPI_Phone_Channel_Remove (pChannel, (IFX_TAPI_MAP_PHONE_t *)arg);
   case IFX_TAPI_MAP_PCM_ADD:
      return TAPI_LL_PCM_Channel_Add (pChannel, (IFX_TAPI_MAP_PCM_t *)arg);
   case IFX_TAPI_MAP_PCM_REMOVE:
      return TAPI_LL_PCM_Channel_Remove (pChannel, (IFX_TAPI_MAP_PCM_t *)arg);
   case IFX_TAPI_PKT_RTP_CFG_SET:
      return TAPI_LL_Phone_RtpConf (pChannel, (IFX_TAPI_PKT_RTP_CFG_t*)arg);
   case IFX_TAPI_PKT_RTP_PT_CFG_SET:
      return TAPI_LL_Phone_Rtp_SetPayloadType (pChannel, (IFX_TAPI_PKT_RTP_PT_CFG_t *)arg);
   case IFX_TAPI_PKT_RTCP_STATISTICS_GET:
      return TAPI_LL_Phone_RTCP_GetStatistics(pChannel, (IFX_TAPI_PKT_RTCP_STATISTICS_t *)arg);
   case IFX_TAPI_PKT_RTCP_STATISTICS_RESET:
      return TAPI_LL_Phone_RTCP_Reset (pChannel);
   case IFX_TAPI_JB_CFG_SET:
      return TAPI_LL_Phone_JB_Set (pChannel, (IFX_TAPI_JB_CFG_t *)arg);
   case IFX_TAPI_JB_STATISTICS_GET:
      return TAPI_LL_Phone_JB_GetStatistics(pChannel, (IFX_TAPI_JB_STATISTICS_t *)arg);
   case IFX_TAPI_JB_STATISTICS_RESET:
      return TAPI_LL_Phone_JB_Reset (pChannel);
   case IFX_TAPI_PKT_AAL_CFG_SET:
      return TAPI_LL_Phone_Aal_Set (pChannel, (IFX_TAPI_PCK_AAL_CFG_t *)arg);
   case IFX_TAPI_PKT_AAL_PROFILE_SET:
      return TAPI_LL_Phone_AalProfile (pChannel, (IFX_TAPI_PCK_AAL_PROFILE_t *)arg);
   /* Recording services */
   case IFX_TAPI_ENC_TYPE_SET:
      return TAPI_LL_Phone_Set_Rec_Codec (pChannel, arg);
   case IFX_TAPI_ENC_START:
      return TAPI_LL_Phone_Start_Recording(pChannel);
   case IFX_TAPI_ENC_STOP:
      return TAPI_LL_Phone_Stop_Recording(pChannel);
   case IFXPHONE_REC_HOLD:
      return TAPI_Phone_SetRecHold (pChannel, (IFX_int32_t)arg);
   case PHONE_REC_DEPTH:
      return TAPI_Phone_SetRecDepth (pChannel, (IFX_int32_t)arg);
   case IFX_TAPI_ENC_VOLUME_SET:
      return TAPI_Phone_SetRecVolume (pChannel, (IFX_int32_t)arg);
   case IFX_TAPI_ENC_LEVEL_SET:
      return TAPI_Phone_GetRecLevel (pChannel, (IFX_int32_t *)arg);
   /* Play Services*/
   /* not used */
   /*
   case IFX_TAPI_DEC_TYPE_SET:
      return(TAPI_Phone_Set_Play_Codec(pChannel, arg));
   */
   case IFX_TAPI_DEC_START:
      return TAPI_LL_Phone_Start_Playing(pChannel);
   case IFX_TAPI_DEC_STOP:
      return TAPI_LL_Phone_Stop_Playing(pChannel);
   case IFXPHONE_PLAY_HOLD:
      return TAPI_Phone_SetPlayHold (pChannel, (IFX_int32_t)arg);
   case PHONE_PLAY_DEPTH:
      return TAPI_Phone_SetPlayDepth (pChannel, (IFX_int32_t) arg);
   case IFX_TAPI_DEC_VOLUME_SET:
      return TAPI_Phone_SetPlayVolume (pChannel, (IFX_int32_t)arg);
   case IFX_TAPI_DEC_LEVEL_GET:
      return TAPI_Phone_GetPlayLevel (pChannel, (IFX_int32_t *)arg);

#endif /* TAPI_VOICE */
#ifdef TAPI_FAX_T38
   /* Fax Services */
   case IFX_TAPI_T38_MOD_START:
      return TAPI_LL_FaxT38_SetModulator (pChannel,
         (IFX_TAPI_T38_MOD_DATA_t *)arg);
   case IFX_TAPI_T38_DEMOD_START:
      return TAPI_LL_FaxT38_SetDemodulator (pChannel,
         (IFX_TAPI_T38_DEMOD_DATA_t *)arg);
   case IFX_TAPI_T38_STOP:
      return TAPI_LL_FaxT38_DisableDataPump (pChannel);
   case IFX_TAPI_T38_STATUS_GET:
      return TAPI_LL_FaxT38_GetStatus (pChannel, (IFX_TAPI_T38_STATUS_t *)arg);
#endif /* TAPI_FAX_T38 */
   /* Report Level */
   case IFX_TAPI_DEBUG_REPORT_SET:
      SetTraceLevel(TAPI_DRV, arg);
      return IFX_SUCCESS;
   case IFX_TAPI_CH_STATUS_GET:
      return TAPI_Phone_GetStatus (pChannel, (IFX_TAPI_CH_STATUS_t *)arg);

   case IFX_TAPI_VERSION_CHECK:
      return TAPI_Phone_Check_Version ((IFX_TAPI_VERSION_t *)arg);

   case IFX_TAPI_PHONE_VOLUME_SET:
      return TAPI_LL_Phone_Volume (pChannel, (IFX_TAPI_LINE_VOLUME_t *)arg);

   case IFX_TAPI_PCM_VOLUME_SET:
      return TAPI_LL_Phone_Pcm_Volume (pChannel, (IFX_TAPI_LINE_VOLUME_t *)arg);

   case IFX_TAPI_LINE_LEVEL_SET:
      return TAPI_LL_Phone_High_Level (pChannel, arg);

   case IFX_TAPI_SIG_DETECT_ENABLE:
      return TAPI_LL_Enable_Signal (pChannel,
         (IFX_TAPI_SIG_DETECTION_t *)arg);
   case IFX_TAPI_SIG_DETECT_DISABLE:
      return TAPI_LL_Disable_Signal (pChannel,
         (IFX_TAPI_SIG_DETECTION_t *)arg);

   case IFX_TAPI_TONE_CPTD_START:
      return TAPI_Phone_DetectToneStart(pChannel, (IFX_TAPI_TONE_CPTD_t*) arg);
   case IFX_TAPI_TONE_CPTD_STOP:
      return TAPI_Phone_DetectToneStop(pChannel);

   case IFX_TAPI_ENC_AGC_CFG:
      ret = TAPI_LL_Phone_AGC_Cfg (pChannel, (IFX_TAPI_ENC_AGC_CFG_t*) arg);
      break;
   case IFX_TAPI_ENC_AGC_ENABLE:
      TAPI_LL_Phone_AGC_Enable (pChannel, (IFX_TAPI_ENC_AGC_MODE_t) arg);
      break;
   
   case IFX_TAPI_TEST_HOOKGEN:
      return TAPI_LL_Test_HookGen (pChannel, (IFX_boolean_t) arg);
   case IFX_TAPI_TEST_LOOP:
      return TAPI_LL_TestLoop (pChannel, (IFX_TAPI_TEST_LOOP_t*) arg);

   default:
      TRACE(TAPI_DRV,DBG_LEVEL_HIGH,("TAPI: unknown ioctl 0x%lx\n",cmd));
      return IFX_ERROR;
   }
}

#endif /* TAPI_CUSTOMER_BOARD */

#endif /* VXWORKS */

