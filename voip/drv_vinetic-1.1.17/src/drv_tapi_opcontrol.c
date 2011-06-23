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
   Module      : drv_tapi_opcontrol.c
   Desription  : Contains TAPI Operation Control Services.
   Remarks     :

   All operations done by functions in this module are phone
   related and assumes a phone channel file descriptor.
   Caller of anyone of the functions must make sure that a phone
   channel is used. In case data channel functions are invoked here,
   an instance of the data channel must be passed.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Reads the hook status from the device
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pState          - storage variable for hook state
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_Hookstate(TAPI_CONNECTION *pChannel, IFX_int32_t *pState)
{
   /* clear "Hook change" exception */
   pChannel->TapiMiscData.nException.Bits.hookstate = IFX_FALSE;

   if (pChannel->TapiOpControlData.bHookState)
   {
      *pState = IFX_TRUE;
   }
   else
   {
      *pState = IFX_FALSE;
   }

   return (IFX_SUCCESS);
}

/*******************************************************************************
Description:
   Sets the linefeeding mode of the device
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   Line mode is ALWAYS set, also if it was set before.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Set_Linefeed(TAPI_CONNECTION *pChannel, IFX_int32_t nMode)
{
   IFX_boolean_t       bBatSw  = IFX_FALSE;
   IFX_boolean_t       bPol    = IFX_FALSE;
   IFX_int32_t         ret = IFX_SUCCESS;
   unsigned char       tmp_nLineMode = pChannel->TapiOpControlData.nLineMode;
   unsigned char       tmp_nBatterySw = pChannel->TapiOpControlData.nBatterySw;
   unsigned char       tmp_nPolarity = pChannel->TapiOpControlData.nPolarity;

   /* save current linemode in tapi structure */
   pChannel->TapiOpControlData.nLineMode = (IFX_uint8_t)nMode;

   /* check if auto battery switch have to enabled */
   if ((nMode == IFX_TAPI_LINE_FEED_NORMAL_AUTO    ||
        nMode == IFX_TAPI_LINE_FEED_REVERSED_AUTO) &&
       !pChannel->TapiOpControlData.nBatterySw)
   {
      bBatSw = IFX_TRUE;
      pChannel->TapiOpControlData.nBatterySw = 0x01;
   }

   /* check if auto battery switch have to disabled */
   if (!(nMode == IFX_TAPI_LINE_FEED_NORMAL_AUTO    ||
         nMode == IFX_TAPI_LINE_FEED_REVERSED_AUTO) &&
       pChannel->TapiOpControlData.nBatterySw)
   {
      bBatSw = IFX_TRUE;
      pChannel->TapiOpControlData.nBatterySw = 0x00;
   }

   /* check if polarity have to change */
   if ((nMode == IFX_TAPI_LINE_FEED_ACTIVE_REV        ||
        nMode == IFX_TAPI_LINE_FEED_REVERSED_AUTO     ||
        nMode == IFX_TAPI_LINE_FEED_ACTIVE_RES_REVERSED) &&
       !pChannel->TapiOpControlData.nPolarity)
   {
      bPol = IFX_TRUE;
      pChannel->TapiOpControlData.nPolarity = 0x01;
   }
   if (!(nMode == IFX_TAPI_LINE_FEED_ACTIVE_REV       ||
         nMode == IFX_TAPI_LINE_FEED_REVERSED_AUTO    ||
         nMode == IFX_TAPI_LINE_FEED_ACTIVE_RES_REVERSED) &&
       pChannel->TapiOpControlData.nPolarity)
   {
      bPol = IFX_TRUE;
      pChannel->TapiOpControlData.nPolarity = 0x00;
   }

   /* call low level function to change operation mode */
   ret = TAPI_LL_Phone_SwitchLine(pChannel, nMode);

   if (ret != IFX_SUCCESS)
   {
      /* restore the old value, because the configuration on the driver failed */
      pChannel->TapiOpControlData.nLineMode = tmp_nLineMode;
   }

   /* switch polarity only when it is necessary */
   if (bPol && (ret == IFX_SUCCESS))
   {
      ret = TAPI_LL_Phone_Polarity(pChannel);
      if (ret != IFX_SUCCESS)
      {
         /* restore the old value, because the configuration on the driver failed */
         pChannel->TapiOpControlData.nPolarity = tmp_nPolarity;
      }
   }

   /* enable / disable automatic battery switch */
   if (bBatSw && (ret == IFX_SUCCESS))
   {
      ret = TAPI_LL_Phone_AutoBatterySwitch(pChannel);
      if (ret != IFX_SUCCESS)
      {
         /* restore the old value, because the configuration on the driver failed */
         pChannel->TapiOpControlData.nBatterySw = tmp_nBatterySw;
      }
   }

   return ret;
}

/*********************************************************************************
Description:
   sets Lec configuration.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to IFX_TAPI_LEC_CFG_t structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Phone_LecConf (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf)
{
   IFX_TAPI_LEC_CFG_t *pLecData = &pChannel->TapiLecData;
   IFX_int32_t ret;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* do some checks */
   if ((pLecConf->nGainIn  >  IFX_TAPI_LEC_GAIN_HIGH) ||
       (pLecConf->nGainOut >  IFX_TAPI_LEC_GAIN_HIGH) ||
       (pLecConf->nLen     <  IFX_TAPI_LEC_LEN_MIN)   ||
       (pLecConf->nLen     >  IFX_TAPI_LEC_LEN_MAX))
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: Gain In or Gain out or length out of range.\n\r"));
      /* end of protected area */
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }
   /* in case one of the gains is set off, set the other also off */
   if ((pLecConf->nGainIn  == IFX_TAPI_LEC_GAIN_OFF) ||
       (pLecConf->nGainOut == IFX_TAPI_LEC_GAIN_OFF))
   {
      pLecConf->nGainIn  = IFX_TAPI_LEC_GAIN_OFF;
      pLecConf->nGainOut = IFX_TAPI_LEC_GAIN_OFF;
   }
   /* save configuration */
   pLecData->bNlp     = pLecConf->bNlp    ;
   pLecData->nGainIn  = pLecConf->nGainIn ;
   pLecData->nGainOut = pLecConf->nGainOut;
   pLecData->nLen     = pLecConf->nLen    ;
#if 0
   pLecData->nType    = pLecConf->nType   ;
#endif /* 0 */

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   /* call low level for settings */
   ret = TAPI_LL_Phone_LecConf (pChannel, pLecConf);

   return ret;
}

/*********************************************************************************
Description:
   read actual lec configuration
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to IFX_TAPI_LEC_CFG_t structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_GetLecConf (TAPI_CONNECTION *pChannel,
                                   IFX_TAPI_LEC_CFG_t *pLecConf)
{
   IFX_TAPI_LEC_CFG_t *pLecData = &pChannel->TapiLecData;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* read config from channel structure */
   *pLecConf = *pLecData;

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return IFX_SUCCESS;
}


/*********************************************************************************
Description:
   sets Lec configuration for Pcm.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to IFX_TAPI_LEC_CFG_t structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Phone_LecConf_Pcm (TAPI_CONNECTION *pChannel, IFX_TAPI_LEC_CFG_t *pLecConf)
{
   IFX_TAPI_LEC_CFG_t *pLecData = &pChannel->TapiLecPcmData;
   IFX_int32_t ret;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* do some checks */
   if ((pLecConf->nGainIn  >  IFX_TAPI_LEC_GAIN_HIGH) ||
       (pLecConf->nGainOut >  IFX_TAPI_LEC_GAIN_HIGH) ||
       (pLecConf->nLen     <  IFX_TAPI_LEC_LEN_MIN)   ||
       (pLecConf->nLen     >  IFX_TAPI_LEC_LEN_MAX))
   {
      LOG (TAPI_DRV,DBG_LEVEL_HIGH,
          ("\n\rDRV_ERROR: Gain In or Gain out or length out of range.\n\r"));
      /* end of protected area */
      IFXOS_MutexUnlock(pChannel->TapiDataLock);
      return IFX_ERROR;
   }
   /* in case one of the gains is set off, set the other also off */
   if ((pLecConf->nGainIn  == IFX_TAPI_LEC_GAIN_OFF) ||
       (pLecConf->nGainOut == IFX_TAPI_LEC_GAIN_OFF))
   {
      pLecConf->nGainIn  = IFX_TAPI_LEC_GAIN_OFF;
      pLecConf->nGainOut = IFX_TAPI_LEC_GAIN_OFF;
   }
   /* save configuration */
   pLecData->bNlp     = pLecConf->bNlp    ;
   pLecData->nGainIn  = pLecConf->nGainIn ;
   pLecData->nGainOut = pLecConf->nGainOut;
   pLecData->nLen     = pLecConf->nLen    ;
#if 0
   pLecData->nType    = pLecConf->nType   ;
#endif /* 0 */

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   /* call low level for settings */
   ret = TAPI_LL_Phone_LecConf_Pcm (pChannel, pLecConf);

   return ret;
}


/*********************************************************************************
Description:
   read actual lec configuration for Pcm
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pLecConf        - handle to IFX_TAPI_LEC_CFG_t structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_GetLecConf_Pcm (TAPI_CONNECTION *pChannel,
                                       IFX_TAPI_LEC_CFG_t *pLecConf)
{
   IFX_TAPI_LEC_CFG_t *pLecData = &pChannel->TapiLecPcmData;

   /* begin of protected area */
   IFXOS_MutexLock(pChannel->TapiDataLock);

   /* read config from channel structure */
   *pLecConf = *pLecData;

   /* end of protected area */
   IFXOS_MutexUnlock(pChannel->TapiDataLock);

   return IFX_SUCCESS;
}

