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
   Module      : drv_tapi_pcm.c
   Desription  : Contains PCM Services.
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
   Sets the configuration of the PCM interface
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pPCMConfig      - contains the configuration for pcm interface
Return:
   returns an error code:
      IFX_SUCCESS  -> configuration is set
      IFX_ERROR      -> configuration is NOT set
*******************************************************************************/
IFX_int32_t TAPI_Phone_PCM_Set_Config(TAPI_CONNECTION *pChannel,
                                      TAPI_PCM_CONFIG const *pPCMConfig)
{
   IFX_int32_t ret = IFX_SUCCESS;

   /* chip specific function */
   ret = TAPI_LL_Phone_PCM_Config(pChannel, pPCMConfig);

   /* save configuration */
   if (ret == IFX_SUCCESS)
      pChannel->TapiPCMData.PCMConfig = *pPCMConfig;

   return ret;
}


/*******************************************************************************
Description:
   Gets the configuration of the PCM interface
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pPCMConfig      - contains the configuration for pcm interface
Return:
   IFX_SUCCESS
*******************************************************************************/
IFX_int32_t TAPI_Phone_PCM_Get_Config(TAPI_CONNECTION *pChannel,
                                      TAPI_PCM_CONFIG *pPCMConfig)
{
   /* get configuration */
   *pPCMConfig = pChannel->TapiPCMData.PCMConfig;

   return IFX_SUCCESS;
}


/*******************************************************************************
Description:
   Activate or deactivate the pcm timeslots configured for this channel
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nMode           - 1: timeslot activated
                     0: timeslot deactivated
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_Phone_PCM_Set_Activation(TAPI_CONNECTION *pChannel,
                                          IFX_uint32_t nMode)
{
   IFX_int32_t ret = IFX_SUCCESS;

   if (nMode > 1)
   {
      LOG(TAPI_DRV,DBG_LEVEL_HIGH,
         ("\n\rDRV_ERROR: unknown mode %d (Phone_PCM_Activation)\n\r", nMode));
      return IFX_ERROR;
   }

   /* chip specific function */
   ret = TAPI_LL_Phone_PCM_Activation(pChannel, nMode);

   /* save activation level */
   if (ret == IFX_SUCCESS)
   {
      pChannel->TapiPCMData.bTimeSlotActive = (IFX_boolean_t)(nMode == 1);
   }

   return ret;
}

/*******************************************************************************
Description:
   Get the activation status from the pcm interface
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   returns the activation status:
                 IFX_TRUE  -> timeslot is activated
                 IFX_FALSE -> timeslot is deactivated
*******************************************************************************/
IFX_int32_t TAPI_Phone_PCM_Get_Activation(TAPI_CONNECTION *pChannel)
{
   return pChannel->TapiPCMData.bTimeSlotActive;
}

