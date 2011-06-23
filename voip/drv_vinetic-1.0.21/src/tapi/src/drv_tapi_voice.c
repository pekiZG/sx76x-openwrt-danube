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
   Module      : drv_tapi_voice.c
   Desription  : Contains TAPI Voice Services : Play, Recording, Conferencing.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_tapi.h"

#ifdef TAPI_VOICE

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   This interface holds or resumes the playing on a data channel
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nHold       - 0 : Resume / != 0 : Hold
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetPlayHold (TAPI_CONNECTION *pChannel, IFX_int32_t nHold)
{
   /* Implement generic part here if */
   if (pChannel != NULL && (nHold==0 || nHold ==1))
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface sets the playing volume
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nVolume     - playing volume
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetPlayVolume (TAPI_CONNECTION *pChannel,
                                      IFX_int32_t nVolume)
{
   /* Implement generic part here if */
   if (pChannel != NULL && nVolume >= 0)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface returns the level of most recently played signal
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure.
   pLevel      - Level of most recently recorded signal.
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_GetPlayLevel (TAPI_CONNECTION *pChannel,
                                     IFX_int32_t *pLevel)
{
   /* Implement generic part here if */
   if (pChannel != NULL && pLevel != NULL)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface sets the number of frames which are buffered by the DSP
   before start of playing
Arguments:
   pChannel     - handle to TAPI_CONNECTION structure
   nFrameDepth  - number of audio frames buffered before recording
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetPlayDepth (TAPI_CONNECTION *pChannel,
                                     IFX_int32_t nFrameDepth)
{
   /* Implement generic part here if */
   if (pChannel != NULL && nFrameDepth >= 0)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}


/*******************************************************************************
Description:
   This interface holds or resumes the recording on a data channel
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nHold       - 0 : Resume / != 0 : Hold
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetRecHold (TAPI_CONNECTION *pChannel, IFX_int32_t nHold)
{
   /* Implement generic part here if */
   if (pChannel != NULL && (nHold==0 || nHold ==1))
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface sets the rcording volume
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nVolume     - recording volume
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetRecVolume (TAPI_CONNECTION *pChannel,
                                     IFX_int32_t nVolume)
{
   /* Implement generic part here if */
   if (pChannel != NULL && nVolume >= 0)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface returns the level of most recently recorded signal
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure.
   pLevel      - Level of most recently recorded signal.
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_GetRecLevel (TAPI_CONNECTION *pChannel,
                                    IFX_int32_t *pLevel)
{
   /* Implement generic part here if */
   if (pChannel != NULL && pLevel != NULL)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}

/*******************************************************************************
Description:
   This interface sets the number of frames which are buffered by the DSP
   before start of recording
Arguments:
   pChannel     - handle to TAPI_CONNECTION structure
   nFrameDepth  - number of audio frames buffered before recording
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   !! Not yet implemented
*******************************************************************************/
IFX_int32_t TAPI_Phone_SetRecDepth (TAPI_CONNECTION *pChannel,
                                    IFX_int32_t nFrameDepth)
{
   /* Implement generic part here if */
   if (pChannel != NULL && nFrameDepth >= 0)
      /* call low level function here if */
      return IFX_SUCCESS;
   else
      return IFX_ERROR;
}


/*******************************************************************************
Description:
   This interface adds a data channel to an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_DATA_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Data_Channel_Add (TAPI_CONNECTION *pChannel,
                                   TAPI_DATA_MAPPING const *pMap)
{
   TAPI_CONNECTION *pPhoneChannel = IFX_NULL;
   IFX_int32_t      ret = IFX_SUCCESS;

   /* Implement generic part here if */
   switch (pMap->nPlayStart)
   {
   case TAPI_DATA_MAPPING_UNCHANGED:
      /* do nothing */
      break;
   case TAPI_DATA_MAPPING_START:
      ret = TAPI_LL_Phone_Start_Playing (pChannel);
      break;
   case TAPI_DATA_MAPPING_STOP:
      ret = TAPI_LL_Phone_Stop_Playing  (pChannel);
      break;
   }
   if (ret == IFX_SUCCESS)
   {
      switch (pMap->nRecStart)
      {
      case TAPI_DATA_MAPPING_UNCHANGED:
         /* do nothing */
         break;
      case TAPI_DATA_MAPPING_START:
         ret = TAPI_LL_Phone_Start_Recording (pChannel);
         break;
      case TAPI_DATA_MAPPING_STOP:
         ret = TAPI_LL_Phone_Stop_Recording  (pChannel);
         break;
      }
   }

   /* call low level function here if */
   if (ret == IFX_SUCCESS)
      ret = TAPI_LL_Data_Channel_Add (pChannel, pMap);

   if (ret == IFX_SUCCESS)
   {
      pPhoneChannel = &pChannel->pTapiDevice->pChannelCon[pMap->nDstCh];
      /* Set appropriate dsp data channel on according phone channel */
      pPhoneChannel->nPhoneDataChannels |= (1 << pChannel->nChannel);
      /* Set appropriate phone channel on according dsp data channel */
      pChannel->nDataPhoneChannels      |= (1 << pMap->nDstCh);
   }

   return ret;
}

/*******************************************************************************
Description:
   This interface removes a data channel from an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_DATA_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Data_Channel_Remove (TAPI_CONNECTION *pChannel,
                                      TAPI_DATA_MAPPING const *pMap)
{
   TAPI_CONNECTION  *pPhoneChannel = IFX_NULL;
   IFX_int32_t       ret = IFX_SUCCESS;

   /* Implement generic part here if */
   switch (pMap->nPlayStart)
   {
   case TAPI_DATA_MAPPING_UNCHANGED:
      /* do nothing */
      break;
   case TAPI_DATA_MAPPING_START:
      ret = TAPI_LL_Phone_Start_Playing (pChannel);
      break;
   case TAPI_DATA_MAPPING_STOP:
      ret = TAPI_LL_Phone_Stop_Playing  (pChannel);
      break;
   }
   if (ret == IFX_SUCCESS)
   {
      switch (pMap->nRecStart)
      {
      case TAPI_DATA_MAPPING_UNCHANGED:
         /* do nothing */
         break;
      case TAPI_DATA_MAPPING_START:
         ret = TAPI_LL_Phone_Start_Recording (pChannel);
         break;
      case TAPI_DATA_MAPPING_STOP:
         ret = TAPI_LL_Phone_Stop_Recording  (pChannel);
         break;
      }
   }

   /* call low level function here if */
   if (ret == IFX_SUCCESS)
      ret = TAPI_LL_Data_Channel_Remove (pChannel, pMap);

   if (ret == IFX_SUCCESS)
   {
      pPhoneChannel = &pChannel->pTapiDevice->pChannelCon[pMap->nDstCh];
      /* Clear appropriate dsp data channel on according phone channel */
      pPhoneChannel->nPhoneDataChannels &= ~(1 << pChannel->nChannel);
      /* Clear appropriate phone channel on according dsp data channel */
      pChannel->nDataPhoneChannels      &= ~(1 << pMap->nDstCh);
   }

   return ret;
}

/*******************************************************************************
Description:
   This interface adds a phone channel to an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PHONE_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Phone_Channel_Add (TAPI_CONNECTION *pChannel,
                                    TAPI_PHONE_MAPPING const *pMap)
{
   /* Implement generic part here if */

   /* call low level function here if */
   return TAPI_LL_Phone_Channel_Add (pChannel, pMap);
}

/*******************************************************************************
Description:
   This interface removes a phone channel from an analog phone device
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   pMap        - handle to TAPI_PHONE_MAPPING structure
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
*******************************************************************************/
IFX_int32_t TAPI_Phone_Channel_Remove (TAPI_CONNECTION *pChannel,
                                       TAPI_PHONE_MAPPING const *pMap)
{
   /* Implement generic part here if */

   /* call low level function here if */
   return TAPI_LL_Phone_Channel_Remove (pChannel, pMap);
}

/*******************************************************************************
Description:
   This interface returns the phone channel mapped to to the data channel.
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   pPhoneChannel  - pointer to phone channel
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   - In case more than one phone channel are mapped to the data channel, the first
     one is retrieved. Therefore the user should make sure that this first channel
     is the one on which his phone activities are run.
   - If no phone channel is found, it will be assumed that phone and data
     channels are identical.
*******************************************************************************/
IFX_int32_t TAPI_Data_Get_Phone_Channel (TAPI_CONNECTION *pChannel,
                                         IFX_uint8_t *pPhoneChannel)
{
   IFX_uint8_t i;

   /* reference must be valid */
   if (pPhoneChannel == NULL)
      return IFX_ERROR;

   /* default: data and phone channels are identical */
   *pPhoneChannel = pChannel->nChannel;

   for (i = 0; i < pChannel->pTapiDevice->nMaxChannel ; i++)
   {
      if (((pChannel->nDataPhoneChannels >> i) & 0x1) == 0x1)
      {
         *pPhoneChannel = i;
         break;
      }
   }

   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   This interface returns the data channel mapped to the phone channel.
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   pDataChannel   - pointer to phone channel
Return:
   IFX_SUCCESS / IFX_ERROR
Remarks:
   - In case more than one data channel are mapped to the data channel,
     the first one is retrieved. Therefore the user should make sure that
     this first channel is the one on which his data activities are run.
   - If no data channel is found, it will be assumed that data and phone
     channels are identical.
*******************************************************************************/
IFX_int32_t TAPI_Phone_Get_Data_Channel (TAPI_CONNECTION *pChannel,
                                         IFX_uint8_t *pDataChannel)
{
   IFX_uint8_t i;

   /* reference must be valid */
   if (pDataChannel == IFX_NULL)
      return IFX_ERROR;

   /* default: data and phone channels are identical */
   *pDataChannel = pChannel->nChannel;

   for (i = 0; i < pChannel->pTapiDevice->nMaxChannel ; i++)
   {
      if (((pChannel->nPhoneDataChannels >> i) & 0x1) == 0x1)
      {
         *pDataChannel = i;
         break;
      }
   }

   return IFX_SUCCESS;
}

#endif /* TAPI_VOICE */


