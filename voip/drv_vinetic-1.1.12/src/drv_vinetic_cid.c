/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
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
   Module      : drv_vinetic_cid.c
   Description : This file contains the implementation of the functions
                 for CID operations
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_cid.h"
#include "drv_vinetic_dspconf.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* max number of DTMF words per message */
#define DTMF_MAX_MSG_WORDS       10

/**
   Store status in channel structure and call according callback function if
   present
\params
   status - DTMF status
*/
#define DTMF_STATUS(status)\
do {\
   pDtmf->state = (status);\
   if (pDtmf->stateCb != IFX_NULL)\
      pDtmf->stateCb(pCh);\
} while (0)


/**
   If define is present the driver will not make use of the auto-deactivation
   feature of the DTMF/AT generator.
   When DTMF_NO_AUTODEACT is not defned, the AD bit will be set with the last
   portion of DTMF data and the generator will be deactivated by firmware after
   transmission.
   When DTMF_NO_AUTODEACT is defined, the AD bit is not set and the driver
   deactivates the generator on reception of the implirf BUF underrun interrupt.
\remarks
   DTMF_NO_AUTODEACT should be set to guarantee immediate deactivation of the
   DTMF/AT generator on hook-events.
*/
#define DTMF_NO_AUTODEACT

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */
IFX_LOCAL IFX_int32_t PrepareForCid      (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t OnCidRequest       (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t StopCid            (VINETIC_CHANNEL *pCh);
IFX_LOCAL IFX_int32_t SetupAfterCid      (VINETIC_CHANNEL *pCh);

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/*******************************************************************************
Description:
   setup to be done before a cid transmission.
Arguments:
   pCh      - channel pointer
Return:
   IFX_SUCCESS or IFX_ERROR
Remark:
   It is assumed that the CID coefficients (mark, seizure, brs, level) were set
   already accordingly by the caller of this function.
*******************************************************************************/
IFX_LOCAL IFX_int32_t PrepareForCid (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t     err  = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch   = (pCh->nChannel - 1);

   /* set the state to prepare to protect channel from a new cid sending while
      an ongoing cid sending is on. */
   CIDTX_SET_STATE (CID_PREPARE);
   /* store data into channel structure */
   pCh->cidSend.nPos     = 0;
   pCh->cidSend.pCmd [0] = (CMD1_EOP | (pCh->nChannel - 1));
   pCh->cidSend.pCmd [1] = ECMD_CID_DATA;
   pCh->cidSend.pCmd [2] = 0;
   /* Setup CID interrupt masks.
      Note : allow rising edge interrupts for CIS_BUF, CIS_REQ, CIS_ACT.
             Mask falling edge interrupts for CIS_BUF and CIS_REQ */
   err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CIS, IFX_FALSE,
                                       VIN_CIS_BUF_REQ_MASK, VIN_CIS_ACT_MASK);
   /* set adders and enable CID Sender */
   if (err == IFX_SUCCESS)
   {
       CIDTX_SET_STATE (CID_TRANSMIT);
      /* A1 = 1, A2 = 1 */
      pDev->pSigCh[ch].cid_sender[CMD_HEADER_CNT] |= (SIG_CID_A1_VOICE |
                                                      SIG_CID_A2_VOICE);
      err = Dsp_SetCidSender(pCh, IFX_TRUE);
   }
   if (err == IFX_ERROR)
   {
      /* allow CID setup on the next run */
      CIDTX_SET_STATE (CID_SETUP);
      pCh->cidSend.nCidCnt = 0;
      /* mask CID interrupts */
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CIS, IFX_FALSE, 0, 0);
   }
   /* if phone is actually off-hook, stop every thing for CID onhook */
   else  if ((pCh->cidSend.nCidType == IFX_TAPI_CID_HM_ONHOOK)
#ifdef TAPI
          && (pCh->pTapiCh->TapiCidTx.pPhoneCh->TapiOpControlData.bHookState == IFX_TRUE)
#endif /* TAPI */
   )
   {
      /* allow CID setup on the next run */
      CIDTX_SET_STATE (CID_SETUP);
      pCh->cidSend.nCidCnt = 0;
      /* In case of Offhook, disable Cid sender and mask CID interrupts now.
         Also leave initial state to allow next sending of CID */
      err = Dsp_SetCidSender(pCh, IFX_FALSE);
      /* mask CID interrupts */
      if (err == IFX_SUCCESS)
         err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CIS,
                                             IFX_FALSE, 0, 0);
   }

   return err;
}

/*******************************************************************************
Description:
   Does the needed settings at end of CID due to an error, an offhook event
   or the end of transmission
Arguments:
   pCh      - channel pointer
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks :
   This function can be called from Interrupt routine and therefore should not
   be blocking.
*******************************************************************************/
IFX_LOCAL IFX_int32_t StopCid (VINETIC_CHANNEL *pCh)
{
   IFX_int32_t     err  = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     ch   = (pCh->nChannel - 1);

   if (pCh->cidSend.nState == CID_OFFHOOK)
   {
      /* Mute both adder, so that FSK can not be heard */
      pDev->pSigCh[ch].cid_sender[0] &= ~CMD1_RD;
      pDev->pSigCh[ch].cid_sender[2] &= ~(SIG_CID_A1 | SIG_CID_A2);
      err = CmdWriteIsr (pDev, pDev->pSigCh[ch].cid_sender, 1);
      /* Disable Cid Sender now */
      pDev->pSigCh[ch].cid_sender[2] &= ~SIG_CID_EN;
      err = CmdWriteIsr (pDev, pDev->pSigCh[ch].cid_sender, 1);
#ifndef VIN_2CPE
#ifdef EVALUATION
   if (err == IFX_SUCCESS)
   {
      if ((((VINCH_EVAL *)pCh->pEval)->cidEval.bEvalCid == IFX_TRUE) &&
          (((VINCH_EVAL *)pCh->pEval)->cidEval.nCidOp != CIDEVAL_STOP))
      {
         ((VINCH_EVAL *)pCh->pEval)->cidEval.nCidOp = CIDEVAL_STOP;
         Eval_CidCallback ((IFX_uint32_t)pCh);
         err = ((pDev->err == ERR_CID_TRANSMIT) ? IFX_ERROR : IFX_SUCCESS);
      }
   }
#endif /* EVALUATION */
#endif /* VIN_2CPE */
   }
   /* Reset CID interrupt masks.
      Note : Mask rising edge interrupts for CIS_BUF, CIS_REQ, CIS_ACT.
             Mask falling edge interrupts for CIS_BUF, CIS_REQ, CIS_ACT. */
   if (err == IFX_SUCCESS)
   {
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_CIS, IFX_TRUE, 0, 0);
   }
   /* reset CID state */
   if (err == IFX_SUCCESS)
   {
      CIDTX_SET_STATE(CID_SETUP);
      pCh->cidSend.nCidCnt = 0;
   }

   return err;
}

/*******************************************************************************
Description:
   setup to be done after succesfull cid transmission
Arguments:
   pCh      - channel pointer
Return:
   IFX_SUCCESS or IFX_ERROR
Remarks :
   This function can be called from Interrupt routine and therefore should not
   be blocking.
*******************************************************************************/
IFX_LOCAL IFX_int32_t SetupAfterCid (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t    ch    = (pCh->nChannel - 1);
   IFX_int32_t    err   = IFX_SUCCESS;

   /* safety check. Function sould be called only at
      end of successfull transmission */
   if (pCh->cidSend.nState != CID_TRANSMIT)
      return IFX_SUCCESS;
   /* disable the CID sender : EN = 0 */
   pDev->pSigCh[ch].cid_sender[0] &= ~CMD1_RD;
   pDev->pSigCh[ch].cid_sender[2] &= ~SIG_CID_EN;
   /* to succesfully end the transmission, set autodeactivation : AD = 1*/
   pDev->pSigCh[ch].cid_sender[2] |= SIG_CID_AD;
   err = CmdWriteIsr (pDev, pDev->pSigCh[ch].cid_sender, 1);
#ifndef VIN_2CPE
#ifdef EVALUATION
   if (err == IFX_SUCCESS)
   {
      if ((((VINCH_EVAL *)pCh->pEval)->cidEval.bEvalCid == IFX_TRUE)  &&
          (((VINCH_EVAL *)pCh->pEval)->cidEval.nCidOp == CIDEVAL_PAUSE2))
      {
         Eval_CidCallback ((IFX_uint32_t)pCh);
         err = ((pDev->err == ERR_CID_TRANSMIT) ? IFX_ERROR : IFX_SUCCESS);
      }
   }
#endif /* EVALUATION */
#endif /* VIN_2CPE */

   return err;
}

/*******************************************************************************
Description:
   operations to be done on CId request (ISRE1 [CIS_REQ] = 1)
Arguments:
   pCh      - channel pointer
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   This function is called from Interrupt routine and therefore should not
   be blocking.
*******************************************************************************/
IFX_LOCAL IFX_int32_t OnCidRequest (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   VINETIC_CID    *pChCid  = &pCh->cidSend;
   IFX_uint8_t    i, len;
   IFX_int32_t    err;

   if (pChCid->nCidCnt - pChCid->nPos > MAX_CID_LOOP_DATA)
      len = MAX_CID_LOOP_DATA;
   /* last Cid data */
   else
   {
      len = (pChCid->nCidCnt - pChCid->nPos);
      pChCid->pCmd [2] = pCh->cidSend.nOdd;
   }
   /* code length, counting Odd word */
   pChCid->pCmd [1] = (pChCid->pCmd [1] & ~CMD2_LEN) | (len + 1);
   /* fill command buffer with new cid data */
   for (i = 0; i < len; i++)
      pChCid->pCmd [3 + i] = pChCid->pCid [pChCid->nPos + i];
   /* send cid */
   err = CmdWriteIsr (pDev, pChCid->pCmd, len + 1);
   if (err == IFX_SUCCESS)
   {
      /* adjust pos variable */
      pChCid->nPos += len;
      /* what to do at end of CID. */
      if (pChCid->nPos == pChCid->nCidCnt)
      {
         err = SetupAfterCid (pCh);
      }
   }

   return err;
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   CID FSK State Machine
\param
   pCh          - channel pointer
\return
   IFX_SUCCESS/IFX_ERROR
\remarks
   This function manages all CID states :
   - CID_SETUP        : settings to do before sending CID data
   - CID_TRANSMIT     : sending of CID data
   - CID_OFFHOOK      : stop Cid sender on Offhook
   - CID_TRANSMIT_ERR : settings after CID data send error (Buffer underrun i.e)

   This function can be called from Interrupt routine and therefore should not
   be blocking.
   CID baudrate = 150 Bytes/sec without Pause.
   1 CID Word = ~ 20 ms / Pause before request : 300 ms
*/
/******************************************************************************/
IFX_int32_t VINETIC_CidFskMachine (VINETIC_CHANNEL *pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t    err   = IFX_SUCCESS;

   switch (pCh->cidSend.nState)
   {
      case CID_SETUP:
         /* prepare for cid only when new cid data */
         if (pCh->cidSend.nCidCnt)
            err = PrepareForCid (pCh);
         break;
      case CID_TRANSMIT:
         err = OnCidRequest (pCh);
         break;
      case CID_OFFHOOK:
      case CID_TRANSMIT_END:
      case CID_TRANSMIT_ERR:
         err = StopCid (pCh);
         break;
      default:
         TRACE (VINETIC, DBG_LEVEL_HIGH,
                ("ERR: Unhandled Cid machine state\r\n"));
         err = IFX_ERROR;
         break;
   }
   if (err == IFX_ERROR)
   {
      SET_ERROR (ERR_CID_TRANSMIT);
      /* allow CID setup on the next run */
      CIDTX_SET_STATE(CID_SETUP);
      pCh->cidSend.nCidCnt = 0;
#ifdef TAPI_CID
      /* don't allow forthcoming ringing */
      pCh->pTapiCh->TapiCidTx.bRingStart = IFX_FALSE;
      /** \todo send cid transmission runtime error here */
#endif /* TAPI_CID */
   }

   return err;
}

/*******************************************************************************
Description:
   DTMF CID state machine
Arguments:
   pCh      - channel pointer
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   This function translates the DTMF to CID states. This function is called
   from DTMF module in ISR context.
   In case the dtmf transmission is finished or aborted, the cid flag must be
   reset for an eventual subsequent sending of CID FSK on this channel.
*******************************************************************************/
IFX_void_t VINETIC_CidDtmfMachine (VINETIC_CHANNEL *pCh)
{
   switch (pCh->dtmfSend.state)
   {
   case DTMF_READY:
#ifdef TAPI_CID
      TAPI_Signal_Event_Update(pCh->pTapiCh, IFX_TAPI_SIG_CIDENDTX);
#endif /* TAPI_CID */
      break;
   case DTMF_START:
   case DTMF_TRANSMIT:
      break;
   case DTMF_ABORT:
      pCh->dtmfSend.state = DTMF_READY;
#ifdef TAPI_CID
      /* don't allow forthcoming ringing */
      pCh->pTapiCh->TapiCidTx.bRingStart = IFX_FALSE;
      /** \todo send cid transmission runtime error here */
#endif /* TAPI_CID */
      break;
   default:
      break;
   }
}

/*******************************************************************************
 * DTMF Generator support
 ******************************************************************************/

/**
   Start DTMF generator
\param
   pCh          - channel pointer
\param
   pDtmfData    - pointer to the DTMF data to send
\param
   nDtmfSize    - number of DTMF words to send
\param
   nFG          - frequency generation mode (0 = low, 1 = high)
\param
   cbDtmfStatus - callback on DTMF status change (set to IFX_NULL if unused)
\param
   bByteMode    - format of pDtmfData (0 = 16bit, 1 = 8bit)
\return
   IFX_SUCCESS/IFX_ERROR
\remarks
   After triggering the DTMF transmission by calling VINETIC_DtmfStart, the
   transmission will be handled automatically. The DTMF data will be sent on
   interrupt request and stopped on end of transmission, error or hook event.
   The callback cbDtmfStatus can be used to track the status of the DTMF
   transmission.
   If bByteMode is set, the driver will convert the IFX_char_t data to DTMF words.
   This Mode only supports restricted DTMF signs 0 to D (no alert tones or pause).
   Only supports DTMF generator high level timing mode.

Note : when ET = 1 : MOD = 0 , FG = 1 (ref FW Spec).
                If done another way, it leads to CERR in FW.
*/
IFX_int32_t VINETIC_DtmfStart(VINETIC_CHANNEL *pCh, IFX_uint16_t *pDtmfData,
                              IFX_uint16_t nDtmfWords, IFX_uint32_t nFG,
                              IFX_void_t (*cbDtmfStatus)(VINETIC_CHANNEL *pCh),
                              IFX_boolean_t bByteMode)
{
   IFX_int32_t    err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev  = pCh->pParent;
   VINETIC_DTMF   *pDtmf = &pCh->dtmfSend;
   IFX_uint8_t     ch    = pCh->nChannel - 1;

   /* Prevent simultaneous usage of generator resource */
   if (++pDtmf->useCnt != 1)
   {
      pDtmf->useCnt--;
      return IFX_ERROR;
   }

   /* Store DTMF data in channel structure for non-blocking sending */
   pDtmf->nSent      = 0;
   pDtmf->stateCb    = cbDtmfStatus;
   pDtmf->pData      = pDtmfData;
   pDtmf->bByteMode  = bByteMode;
   pDtmf->nWords     = nDtmfWords;
   /* Enable DTMF generator interrupts
      Note: Allow rising edge interrupts for DTMFG_BUF, DTMFG_REQ, DTMFG_ACK.
            Allow falling edge interrupts for DTMFG_ACK and mask others. */
   err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_DTMFG, IFX_FALSE,
                                       VIN_DTMFG_BUF_REQ_ACT_MASK,
                                       VIN_DTMFG_ACK_MASK);
   if (err == IFX_SUCCESS)
   {
      DTMF_STATUS(DTMF_START);
      /* Hardcoded settings, disable, ET = 0, MOD = 1, FG from user,
         A1 = A2 = 01 */
      err = Dsp_DtmfGen(pCh, SIG_DTMGGEN_MOD
                        | (SIG_DTMGGEN_FG  & (nFG << SIG_DTMGGEN_FG_OF))
                        | (SIG_DTMGGEN_A1  & (0x01 << SIG_DTMGGEN_A1_OF))
                        | (SIG_DTMGGEN_A2  & (0x01 << SIG_DTMGGEN_A2_OF)),
                           0xFFF0, IFX_FALSE);
   }
   if (err == IFX_SUCCESS)
   {
      pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
      err = CmdWrite(pDev, pDev->pSigCh[ch].sig_dtmfgen.value, CMD_SIG_DTMFGEN_LEN);
   }

   return err;
}

/**
   Handle DTMF generator activation/deactivation
\param
   pCh   - channel pointer
\return
   IFX_ERROR/IFX_SUCCESS
\remarks
   Currently only used for detection of succesful deactivation if
   auto-deactivation feature of DTMF/AT generator is used.
*/
IFX_int32_t VINETIC_DtmfOnActivate(VINETIC_CHANNEL *pCh, IFX_boolean_t bAct)
{
   IFX_int32_t    err      = IFX_SUCCESS;
#ifndef DTMF_NO_AUTODEACT
   VINETIC_DTMF   *pDtmf   = &pCh->dtmfSend;
#endif

   if (bAct == IFX_TRUE)
   {
      /* DTMF/AT generator active */
   }
#ifndef DTMF_NO_AUTODEACT
   else
   {
      /* Detected auto-deactivation of DTMF/AT generator */
      DTMF_STATUS(DTMF_READY);
      pDtmf->pData  = IFX_NULL;
      pDtmf->nWords = 0;
      pDtmf->nSent  = 0;
      pDtmf->nCurr  = 0;
      pDtmf->useCnt--;
   }
#endif /* DTMF_NO_AUTODEACT */

   return err;
}


/**
   Handle DTMF generator underrun
\param
   pCh   - channel pointer
\return
   IFX_ERROR/IFX_SUCCESS
\remarks
   Stop the DTMF/AT generator with immediate effect on underrun. In case
   compiled with DTMF_NO_AUTODEACT a wanted underrun will occur on end of
   transmission.
*/
IFX_int32_t VINETIC_DtmfOnUnderrun(VINETIC_CHANNEL *pCh)
{
   IFX_int32_t err;

   err = VINETIC_DtmfStop(pCh, IFX_TRUE);
   return err;
}

/**
   Stop and disable DTMF generator
\remarks
   pCh   - channel pointer
\return
   IFX_ERROR/IFX_SUCCESS
\remarks
   Stops the DTMF/AT generator with immediate effect
*/
IFX_int32_t VINETIC_DtmfStop(VINETIC_CHANNEL *pCh, IFX_int32_t nIsr)
{
   IFX_int32_t          err      = IFX_SUCCESS;
   VINETIC_DTMF_STATE   status   = DTMF_READY;
   IFX_uint8_t          ch       = (pCh->nChannel - 1);
   VINETIC_DEVICE       *pDev    = pCh->pParent;
   VINETIC_DTMF         *pDtmf   = &pCh->dtmfSend;

   /* it could be that a request wasn't generated after the last transfer.
      So sent must be updated again. */
   pDtmf->nSent += pDtmf->nCurr;
   if(pDtmf->nSent == pDtmf->nWords)
   {
      status = DTMF_READY;
   }
   else
   {
      status = DTMF_ABORT;
   }

   /* Free DTMF string */
   pDtmf->pData  = IFX_NULL;
   pDtmf->nWords = 0;
   pDtmf->nSent  = 0;
   pDtmf->nCurr  = 0;
   pDtmf->useCnt--;

   /* Disable DTMF generator interrupts and generator itself */
   /* Stop the DTMF generator:
      - reactivate event transmission : ET = 1
      - Timing must be controlled by the events : MOD = 0
      - Frequency generation must be in high level mode : FG = 1
      - Adder A and B Configuration shouldn't be changed !
   */
   err = Dsp_DtmfGen(pCh, (SIG_DTMGGEN_ET | SIG_DTMGGEN_FG) ,
                     0xFF00, IFX_TRUE);
   /* enable the dtmf generator for event transmission : EN = 1 */
   if (err == IFX_SUCCESS)
   {
      pDev->pSigCh[ch].sig_dtmfgen.bit.en = 1;
      err = CmdWriteIsr (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                         CMD_SIG_DTMFGEN_LEN);
   }
   /* Disable generator interrupts
      Note: Disable rising edge interrupts for DTMFG_BUF, DTMFG_REQ, DTMFG_ACK.
            Disable falling edge interrupts for DTMFG_ACK and mask others. */
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_Set_EdspIntMask (pCh, VIN_EDSP_DTMFG, nIsr, 0, 0);

   DTMF_STATUS(status);

   return err;
}

/**
   Send data to DTMF generator
\param
   pCh   - channel pointer
\return
   IFX_ERROR/IFX_SUCCESS
\remarks
   DTMF data is taken from channel specific structure. Octets are expected to
   contain either DTC of frequency values acc. to DTMF generator mode. See
   description of DTMF/AT Generator, Data message for more details.
   Function to be called from interrupt mode (DTMF/AT generator request) only.
*/
IFX_int32_t VINETIC_DtmfOnRequest(VINETIC_CHANNEL *pCh)
{
   IFX_uint16_t    i, nSend;
   IFX_int32_t     err       = IFX_SUCCESS;
   IFX_uint16_t    pCmd[12] = {0};
   IFX_uint8_t     ch       = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev     = pCh->pParent;
   VINETIC_DTMF   *pDtmf    = &pCh->dtmfSend;
#ifndef DTMF_NO_AUTODEACT
   IFX_boolean_t   deact    = IFX_FALSE;
#endif /* DTMF_NO_AUTODEACT */

   if (pDtmf->state != DTMF_TRANSMIT)
   {
      DTMF_STATUS(DTMF_TRANSMIT);
   }

   /* Update number of sent DTMF signs (nSend is IFX_LOCAL and contains the number
      of signs sent on last call)*/
   pDtmf->nSent += pDtmf->nCurr;
   pDtmf->nCurr = 0;
#ifdef DTMF_NO_AUTODEACT
   if (pDtmf->nSent == pDtmf->nWords)
   {
      /* We can get one request too much without auto-deactivation... */
      return err;
   }
#endif /* DTMF_NO_AUTODEACT */

   /* Determine max. number of signs to transmit depending on generator mode */
   if (pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] & SIG_DTMGGEN_MOD )
   {
      /* High level timing mode, up to 5 signs in low level frequency mode or
         up to 10 in high level frequency mode */
      if ((pDtmf->nWords - pDtmf->nSent) > DTMF_MAX_MSG_WORDS)
      {
         nSend = DTMF_MAX_MSG_WORDS;
      }
      else
      {
         /* Last data portion... */
         nSend = (pDtmf->nWords - pDtmf->nSent);
#ifndef DTMF_NO_AUTODEACT
         deact = IFX_TRUE;
#endif /* DTMF_NO_AUTODEACT */
      }
   }
   else
   {
      /* Low level timing mode, only supports one sign per command */
      nSend = 1;
   }

   /* DTMF/AT generator, data message */
   pCmd [0] = CMD1_EOP | ch;
   pCmd [1] = ECMD_DTMF_DATA | nSend;
   if (pDtmf->bByteMode == IFX_TRUE)
   {
      for (i = 0; i < nSend; i++)
      {
         pCmd[2+i] = (IFX_uint16_t)
                      *((IFX_char_t*)pDtmf->pData + pDtmf->nSent + i);
      }
   }
   else
   {
      memcpy(&pCmd [2], (pDtmf->pData + pDtmf->nSent), nSend*2);
   }
   err = CmdWriteIsr(pDev, pCmd, nSend);

#ifndef DTMF_NO_AUTODEACT
   if ((err == IFX_SUCCESS) && (deact == IFX_TRUE))
   {
      /* Enable DTMF/AT generator auto-deactivation feature */
      err = Dsp_DtmfGen(pCh, SIG_DTMGGEN_AD ,
                        SIG_DTMGGEN_EN |SIG_DTMGGEN_AD , IFX_TRUE);
   }
#endif /* DTMF_NO_AUTODEACT */
   if (err == IFX_SUCCESS)
   {
      pDtmf->nCurr = nSend;
   }
   else
   {
      VINETIC_DtmfStop(pCh, IFX_TRUE);
   }

   return err;
}



