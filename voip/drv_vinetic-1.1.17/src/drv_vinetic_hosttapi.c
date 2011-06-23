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
   Module      : drv_vinetic_tapi.c
   Date        : 2003-10-28
   Description : This file contains the implementations of vinetic related
                 tapi low level functions.
*******************************************************************************/

/* ============================= */
/* includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_tapi.h"
#include "drv_vinetic_dcctl.h"
#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_tone.h"

#ifdef TAPI
/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* ============================= */
/*             Enums             */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */



/* ============================= */
/* Local variable definition     */
/* ============================= */


/* ============================= */
/* Local functions definitions   */
/* ============================= */


/* ============================= */
/* Global functions definitions  */
/* ============================= */

/*******************************************************************************
Description:
   Configures the predefined tone of given index and start the playing.
Arguments:
   pChannel       - handle to TAPI_CONNECTION structure
   nToneIndex     - user selected tone index
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   For 2CPE, predefined tones are also played via the DSP, as no ALM tone
   generator is available.
*******************************************************************************/
IFX_int32_t TAPI_LL_Host_Phone_Tone_On (TAPI_CONNECTION *pChannel,
                                        IFX_int32_t nToneIndex)
{
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
         ("Error, 2CPE has no TG : Play tone as simple "
          "tone via tone api instead\r\n"));

   return IFX_ERROR;
}

/*******************************************************************************
Description:
   Stops the predifined tone currently played
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Host_Phone_Tone_Off(TAPI_CONNECTION *pChannel)
{
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
         ("Error, 2CPE has no TG : Stop simple tone "
          "via tone api instead\r\n"));

   return IFX_ERROR;
}

/*******************************************************************************
Description:
   Enable / disable the automatic battery switch.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structured
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_AutoBatterySwitch(TAPI_CONNECTION *pChannel)
{
   TRACE (TAPI_DRV, DBG_LEVEL_NORMAL,
         ("VINETIC WARN: AutoBatterySwitch feature not supported\r\n"));
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Switch polarity.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structured
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Polarity (TAPI_CONNECTION * pChannel)
{
   TRACE (TAPI_DRV, DBG_LEVEL_NORMAL,
         ("VINETIC WARN: Polarity feature not supported\r\n"));
   return IFX_SUCCESS;
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to switch the
   line mode.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   nMode           - linefeed mode
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_SwitchLine (TAPI_CONNECTION *pChannel,
                                      IFX_int32_t nMode)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      ret;
   CMD_OpMode_t     opmod;

   /* sanity check */
   if (pChannel->nChannel >= pDev->nAnaChan)
   {
      return(IFX_SUCCESS);
   }

   memset (&opmod, 0, sizeof (CMD_OpMode_t));
   opmod.RW      = VIN_CMD_RD;
   opmod.CMD     = VIN_CMD_ALM;
   opmod.MOD     = VIN_MOD_DCCTL;
   opmod.ECMD    = VIN_ECMD_OPMOD;
   opmod.CH      = pCh->nChannel -1;
   opmod.LENGTH  = 1;

   /* check current configuration */
   ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *) &opmod),
                  (IFX_uint16_t *)((IFX_void_t *) &opmod), 1);
   if (ret != IFX_SUCCESS)
      goto error;

   /* check if transition is valid, prepare command */
   opmod.RW      = VIN_CMD_WR;

   switch (nMode)
   {
      case IFX_TAPI_LINE_FEED_ACTIVE:
      case IFX_TAPI_LINE_FEED_RING_PAUSE:
      /* for backward compatibility only */
      case IFX_TAPI_LINE_FEED_NORMAL_AUTO:
      case IFX_TAPI_LINE_FEED_ACTIVE_LOW:
      case IFX_TAPI_LINE_FEED_ACTIVE_BOOSTED:
         switch (opmod.OP_MODE)
         {
            case VIN_OPMOD_ACT:
            case VIN_OPMOD_RING:
            case VIN_OPMOD_ONHOOK:
               opmod.OP_MODE = VIN_OPMOD_ACT;
               /* in case of ring pause, leave reverse
                  polarity bit as is */
               if (nMode != IFX_TAPI_LINE_FEED_RING_PAUSE)
                  opmod.REV_POL = VIN_OPMOD_POL_NORMAL;
               break;
            default:
               ret = IFX_ERROR;
               goto error;
         }
         break;
      case IFX_TAPI_LINE_FEED_ACTIVE_REV:
      /* for backward compatibility only */
      case IFX_TAPI_LINE_FEED_REVERSED_AUTO:
      case IFX_TAPI_LINE_FEED_PARKED_REVERSED:
         switch (opmod.OP_MODE)
         {
            case VIN_OPMOD_ACT:
            case VIN_OPMOD_RING:
            case VIN_OPMOD_ONHOOK:
               opmod.OP_MODE = VIN_OPMOD_ACT;
               opmod.REV_POL = VIN_OPMOD_POL_REVERSE;
               break;
            default:
               ret = IFX_ERROR;
               goto error;
         }
         break;
      case IFX_TAPI_LINE_FEED_STANDBY:
         switch (opmod.OP_MODE)
         {
            case VIN_OPMOD_ACT:
            case VIN_OPMOD_RING:
            case VIN_OPMOD_ONHOOK:
            case VIN_OPMOD_PDH:
               opmod.OP_MODE = VIN_OPMOD_ONHOOK;
               opmod.REV_POL = VIN_OPMOD_POL_NORMAL;
               break;
            default:
               ret = IFX_ERROR;
               goto error;
         }
         break;
      case IFX_TAPI_LINE_FEED_DISABLED:
         opmod.OP_MODE = VIN_OPMOD_PDH;
         opmod.REV_POL = VIN_OPMOD_POL_NORMAL;
         break;
      case IFX_TAPI_LINE_FEED_RING_BURST:
         switch (opmod.OP_MODE)
         {
            case VIN_OPMOD_ACT:        /* check for hook in case of active? */
            case VIN_OPMOD_RING:
            case VIN_OPMOD_ONHOOK:
               /* leave reversal bit as is */
               opmod.OP_MODE = VIN_OPMOD_RING;
               break;
            default:
               ret = IFX_ERROR;
               goto error;
         }
         break;
      /* unsupported linemodes */
      case IFX_TAPI_LINE_FEED_HIGH_IMPEDANCE:
      case IFX_TAPI_LINE_FEED_GROUND_START:
      case IFX_TAPI_LINE_FEED_METER:
      case IFX_TAPI_LINE_FEED_ACT_TEST:
      case IFX_TAPI_LINE_FEED_ACT_TESTIN:
      case IFX_TAPI_LINE_FEED_DISABLED_RESISTIVE_SWITCH:
      default:
         ret = IFX_ERROR;
         goto error;
   }

   Vinetic_IrqLockDevice(pDev);
   /* now that we are protected against interrupts, check if linemode was
      possibly set in interrupt context after reading the current status
      (onOvertemp it might have been changed to power down).
      All automatic mode changes are save and would allow changes to
      other operating modes. */

   if ((pChannel->TapiOpControlData.nLineMode
        == IFX_TAPI_LINE_FEED_DISABLED) &&
       (opmod.OP_MODE != VIN_OPMOD_PDH))
   {
      ret = IFX_ERROR;
   }
   else
   {
      IFXOS_MutexLock (pDev->mbxAcc);
      ret = CmdWriteIsr (pDev, (IFX_uint16_t *)((IFX_void_t *) &opmod), 1);
      IFXOS_MutexUnlock (pDev->mbxAcc);
   }

   Vinetic_IrqUnlockDevice(pDev);

error:
   return ret;
}

/*******************************************************************************
Description:
   Prepare parameters and call the Target Configuration Function to set
   the ring configuration.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pRingConfig     - pointer to ring config structure
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Ring_Config(TAPI_CONNECTION *pChannel,
                                      IFX_TAPI_RING_CFG_t const *pRingConfig)
{
   /* not supported - fast ring trip is configured via bbd download
      because this setting affects also the hook thresholds */
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
         ("VINETIC ERROR: Ring Config supported only via download\r\n"));

   return IFX_ERROR;
}

/*******************************************************************************
Description:
   Configure metering mode of chip
Arguments:
   pChannel    - handle to TAPI_CONNECTION structure
   nMode       - use TTX (0) or reverse polarity (1)
   nFreq       - Default/12 KHz/16 KHz
Return:
   IFX_SUCCESS or IFX_ERROR
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Meter_Config(TAPI_CONNECTION *pChannel,
                                       IFX_uint8_t nMode, IFX_uint8_t nFreq)
{
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH,
         ("VINETIC ERROR: Metering feature not supported\r\n"));

   return IFX_ERROR;
}

/*******************************************************************************
Description:
   Sets the tone level of the tone currently played.
Arguments:
   pChannel        - handle to TAPI_CONNECTION structure
   pToneLevel      - pointer to IFX_TAPI_PREDEF_TONE_LEVEL_t structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark:

*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_Tone_Set_Level (TAPI_CONNECTION *pChannel,
                                          IFX_TAPI_PREDEF_TONE_LEVEL_t const *pToneLevel)
{
   TRACE (TAPI_DRV, DBG_LEVEL_HIGH, ("Error, 2CPE has no TG : Set tone level"
          " via tone table configuration instead\r\n"));

   return IFX_ERROR;
}

/*********************************************************************************
Description:
   This service enables or disables a high level path of a phone channel.
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
   bEnable  - The parameter represent a boolean value of \ref IFX_TAPI_LINE_LEVEL_t.
               - 0: IFX_TAPI_LINE_LEVEL_DISABLE, disable the high level path.
               - 1: IFX_TAPI_LINE_LEVEL_ENABLE, enable the high level path.
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   This service is intended for phone channels only and must be used in
   combination with IFX_TAPI_PHONE_VOLUME_SET or IFX_TAPI_PCM_VOLUME_SET
   to set the max. level (IFX_TAPI_LINE_VOLUME_HIGH) or to restore level
*******************************************************************************/
IFX_int32_t TAPI_LL_Phone_High_Level(TAPI_CONNECTION *pChannel,
                                     IFX_int32_t bEnable)
{
   VINETIC_DEVICE  *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL *pCh  = &pDev->pChannel[pChannel->nChannel];
   IFX_int32_t      ret;

   CMD_OpMode_t     opmod;

   /* sanity check */
   if (pChannel->nChannel >= pDev->nAnaChan)
   {
      return(IFX_SUCCESS);
   }

   memset (&opmod, 0, sizeof (CMD_OpMode_t));
   opmod.RW      = VIN_CMD_RD;
   opmod.CMD     = VIN_CMD_ALM;
   opmod.MOD     = VIN_MOD_DCCTL;
   opmod.ECMD    = VIN_ECMD_OPMOD;
   opmod.CH      = pCh->nChannel -1;
   opmod.LENGTH  = 1;

   /* check current configuration */
   ret = CmdRead (pDev, (IFX_uint16_t *) &opmod, (IFX_uint16_t *) &opmod, 1);
   if (ret != IFX_SUCCESS)
      goto error;

   Vinetic_IrqLockDevice(pDev);
   opmod.RW      = VIN_CMD_WR;
   opmod.HOWLER  = bEnable ? VIN_OPMOD_HOWLER_ON : VIN_OPMOD_HOWLER_OFF;

   /* check if linemode was possibly set in interrupt context after reading
      the current status (onOvertemp it might have been changed to power down */
   if ((pChannel->TapiOpControlData.nLineMode
        == IFX_TAPI_LINE_FEED_DISABLED) &&
       (opmod.OP_MODE != VIN_OPMOD_PDH))
   {
      /* line has been switched to power down in the meantime */
      ret = IFX_ERROR;
   }
   else
   {
      IFXOS_MutexLock (pDev->mbxAcc);
      ret = CmdWriteIsr (pDev, (IFX_uint16_t *)((IFX_void_t *) &opmod), 1);
      IFXOS_MutexUnlock (pDev->mbxAcc);
   }
   Vinetic_IrqUnlockDevice(pDev);

error:
   return ret;
}

/*********************************************************************************
Description:
   Restores the line state back after fault
Arguments:
   pChannel - handle to TAPI_CONNECTION structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks:
   On fault the line state is stored and with this function reapplied again
*******************************************************************************/
IFX_int32_t TAPI_LL_Restore_FaultLine(TAPI_CONNECTION *pChannel)
{
   /*VINETIC_DEVICE *pDev = (VINETIC_DEVICE *)pChannel->pDevice;
   VINETIC_CHANNEL* pCh = &pDev->pChannel[pChannel->nChannel];*/

   /** \todo Implement this function */

   return IFX_ERROR;
}

/**
This service generates an on or off hook event for the low level driver.
\param pChannel Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The hook event then gets to the hook state machine for
   validation. Depending on the timing of calling this interface
   also hook flash and pulse dialing can be verified. */
IFX_return_t TAPI_LL_Test_HookGen (TAPI_CONNECTION *pChannel,
                                   IFX_boolean_t bHook)
{
   TAPI_Phone_Event_HookState (pChannel, bHook);
   return IFX_SUCCESS;
}

/**
This service generates an on or off hook event for the low level driver.
\param pChannel Handle to TAPI_CONNECTION structure
\return Return value according to IFX_return_t
   - IFX_ERROR if an error occured
   - IFX_SUCCESS if successful
\remarks
   The hook event then gets to the hook state machine for
   validation. Depending on the timing of calling this interface
   also hook flash and pulse dialing can be verified. */
IFX_return_t TAPI_LL_TestLoop (TAPI_CONNECTION *pChannel,
                               IFX_TAPI_TEST_LOOP_t* pLoop)
{
   VINETIC_DEVICE    *pDev = pChannel->pDevice;
   CMD_DCCTL_Debug_t  debugCfg;
   IFX_int32_t        err         = IFX_SUCCESS;

   memset (&debugCfg, 0, sizeof (CMD_DCCTL_Debug_t));
   debugCfg.CMD     = VIN_CMD_ALM;
   debugCfg.MOD     = VIN_MOD_DCCTL;
   debugCfg.ECMD    = CMD_DCCTL_DEBUG;

   /* read out first */
   debugCfg.RW      = VIN_CMD_RD;
   debugCfg.CH      = pChannel->nChannel;
   debugCfg.LENGTH  = 5;
   err = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)&debugCfg),
                        (IFX_uint16_t *)((IFX_void_t *)&debugCfg), 5);


   if (err != IFX_SUCCESS)
      goto error;

   /* enable or disable testloop */
   debugCfg.RW     = VIN_CMD_WR;

   if (pLoop->bAnalog)
   {
      debugCfg.data1  = 0x0001;
      debugCfg.data3 |= 0x8040;
   }
   else
   {
      debugCfg.data1  = 0x0000;
      debugCfg.data3 &= ~0x0040;
   }
   err = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)&debugCfg), 5);

error:
   return err;
}

#endif /* TAPI */

