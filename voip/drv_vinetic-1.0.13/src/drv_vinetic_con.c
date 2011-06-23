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
   Module      : drv_vinetic_con.c
   Date        : 2005-02-10
*/

/**
   \file drv_vinetic_con.c VINETIC DSP module interconnection managment module.
   \remarks
  This module provides functions to connect different DSP modules. It is used
  for conferencing, but also for basic dynamic connections */

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_con.h"

/* ============================= */
/* Local function declaration    */
/* ============================= */

static int addSigIn (MODULE_SIGNAL_INPUTS * pSigIns, unsigned char nSignal,
                     IFX_uint8_t * pChg);
static int removeSigIn (MODULE_SIGNAL_INPUTS * pSigIns, unsigned char nSignal);

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#ifdef DEBUG
const IFX_char_t signalName[52][10] = {
   "Null\0",
   "PCM-Out0\0",
   "PCM-Out1\0",
   "PCM-Out2\0",
   "PCM-Out3\0",
   "PCM-Out4\0",
   "PCM-Out5\0",
   "PCM-Out6\0",
   "PCM-Out7\0",
   "PCM-Out8\0",
   "PCM-Out9\0",
   "PCM-Out10\0",
   "PCM-Out11\0",
   "PCM-Out12\0",
   "PCM-Out13\0",
   "PCM-Out14\0",
   "PCM-Out15\0",
   "ALM-Out0\0",
   "ALM-Out1\0",
   "ALM-Out2\0",
   "ALM-Out3\0",
   "\0", "\0", "\0",
   "COD-Out0\0",
   "COD-Out1\0",
   "COD-Out2\0",
   "COD-Out3\0",
   "COD-Out4\0",
   "COD-Out5\0",
   "COD-Out6\0",
   "COD-Out7\0",
   "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0",
   "SIG-OutA0\0",
   "SIG-OutB0\0",
   "SIG-OutA1\0",
   "SIG-OutB1\0",
   "SIG-OutA2\0",
   "SIG-OutB2\0",
   "SIG-OutA3\0",
   "SIG-OutB3\0",
   "SIG-OutA4\0",
   "SIG-OutB4\0",
   "SIG-OutA5\0",
   "SIG-OutB5\0"
};
#endif /* DEBUG */


/* ============================= */
/* Local function definition     */
/* ============================= */

/**
   Add a signal connection to the temporary signal array.
\param pSigIns      - handle to the internal signal array
\param nSignal      - specifies the signal to be added
\return
   IFX_SUCCESS or IFX_ERROR in the case that no free input slot was available
\remarks
   This function is used only internally by addModuleSigIn.
*/
static int addSigIn (MODULE_SIGNAL_INPUTS * pSigIns, unsigned char nSignal,
                     IFX_uint8_t * pChg)
{
   int i;

   *pChg = IFX_FALSE;
   /* check if already connected */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if ((*pSigIns)[i] == nSignal)
         return IFX_SUCCESS;
   }

   /* if not, find an empty input slot */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if ((*pSigIns)[i] == ECMD_IX_EMPTY)
      {
         (*pSigIns)[i] = nSignal;
         *pChg = IFX_TRUE;
         return IFX_SUCCESS;
      }
   }
   return IFX_ERROR;
}

/**
   Remove a signal connection from the temporary signal array.
   Note: This function is used only internally by removeModuleSigIn.
\param pSigIns      - handle to the internal signal array
\param nSignal      - specifies the signal to be removed
\return
   number of deleted signals, 0 if the signal was not connected
\remarks
*/
static int removeSigIn (MODULE_SIGNAL_INPUTS * pSigIns, unsigned char nSignal)
{
   int ret = 0;
   int i;

   /* find the input slot and remove signal */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if ((*pSigIns)[i] == nSignal)
      {
         (*pSigIns)[i] = ECMD_IX_EMPTY;
         ret++;
      }
   }
   return ret;
}

/** \defgroup VINETIC_CONNECTIONS VINETIC Connection Module */

/* ============================= */
/* Global function definition    */
/* ============================= */
/** \addtogroup VINETIC_CONNECTIONS */
/* @{ */

/**
   Add a signal connection to the internal shadow connection structure a
   configuration is not done
\param pSrc      - handle to the source module which contains the output signal
\param pDst      - handle to the destination module where the output
is connected to
\param nSignal   - specifies the direction in case of a signaling module,
   don't care for other module types. If set to ALM_TO_SIG_IN  it defines the
   receive path connected to the coder channel. The values can be ALM_TO_SIG_IN
   for analog and COD_TO_SIG_IN for remote. It can be left 0 for any module not
   equal to signaling module
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   The destination is set to modified if all was successful.
   nSignal is only needed for special signaling module handling.
   It is important which input is used for ALM, PCM and Coder. Also
   it is important to which input a module is connected to the signaling:
   local modules like ALM and PCM (not always) support auto suppresion and
   get the event playout. Remote modules like coder and PCM in case of advanced
   with DSP are connected to input 2
*/
int Con_ConnectPrepare (VINDSP_MODULE * pSrc, VINDSP_MODULE * pDst,
                        IFX_uint8_t nSignal)
{
   int i, in = -1;
   VINDSP_MODULE_SIGNAL *pTemp, **pInput;
   IFX_uint8_t sig;

   /* check if the second output signal of the signaling module is ment */
   if (nSignal == ALM_TO_SIG_IN && pSrc->nModType == VINDSP_MT_SIG)
   {
      pInput = &pSrc->pInputs2;
      sig = pSrc->nSignal2;
   }
   else
   {
      pInput = &pSrc->pInputs;
      sig = pSrc->nSignal;
   }
   /* *pInput now points to the first input signal of the list of input signals
      connected to this output. sig contains the value of the signal array to
      program to the FW */
   /* check if already connected */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if (pDst->in[i].i == sig)
         return IFX_SUCCESS;
   }
   if (pDst->nModType == VINDSP_MT_SIG)
   {
      /* signaling module requires special handling: coder comes always to
         input 2, all analogs (ALM and PCM) to input 1 */
      if (pSrc->nModType == VINDSP_MT_COD)
         in = REMOTE_SIG_IN;
      else
      {
         in = LOCAL_SIG_IN;
         /* special case for PCM: in advanced PCM does not act as ALM, but as
            coder. In that case the ALM must be connected before, which is
            assured in the basic init. Dynamically assignment for PCM with DSP
            is not supported */
         if (pSrc->nModType == VINDSP_MT_PCM && nSignal == ALM_TO_SIG_IN)
            if (pDst->in[0].i != ECMD_IX_EMPTY)
               in = REMOTE_SIG_IN;
      }
      if (pDst->in[in].i != ECMD_IX_EMPTY)
      {
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("No input signal\n"));
         return IFX_ERROR;
      }
   }
   else
   {
      /* find free one */
      for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
      {
         if (pDst->in[i].i == ECMD_IX_EMPTY)
         {
            in = i;
            break;
         }
      }
   }
   if (in == -1)
      return IFX_ERROR;
#ifdef DEBUG
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          ("adding Signal %-10s to ", signalName[sig]));
   switch (pDst->nModType)
   {
      case VINDSP_MT_ALM:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("ALM"));
         break;
      case VINDSP_MT_PCM:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("PCM"));
         break;
      case VINDSP_MT_COD:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("COD"));
         break;
      case VINDSP_MT_SIG:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("SIG"));
         break;
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          (" (Out-Sig %s)\n", signalName[pDst->nSignal]));
#endif
   /* set signal */
   if (pDst->in[in].i != sig)
   {
      pDst->modified = IFX_TRUE;
      pDst->in[in].i = sig;
   }
   pDst->in[in].pOut = pSrc;
   /* now find free list item and add to the list */
   pTemp = *pInput;
   if (pTemp == NULL)
   {
      *pInput = &pDst->in[in];
   }
   else
   {
      while (pTemp->pNext != NULL)
         pTemp = (pTemp)->pNext;
      pTemp->pNext = &pDst->in[in];
   }
   return IFX_SUCCESS;
}

/**
   Removes a signal connection from the internal shadow connection structure.
   A configuration is not done
\param pSrc      - handle to the source module which contains the output signal
\param pDst      - handle to the destination module where the output
is connected to
\param nSignal   - specifies the direction in case of a signaling module,
   don't care for other module types. If set to 1 it defines the receive path
   connected to the coder channel
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   The destination is set to modified if all was successful
*/
int Con_DisconnectPrepare (VINDSP_MODULE * pSrc, VINDSP_MODULE * pDst,
                           IFX_uint8_t nSignal)
{
   int i, in = -1;
   VINDSP_MODULE_SIGNAL **pTemp;

   /* remove dst input of src
      pDst->pInputs->pSig1->pNext->pSigX[pDst]->pNext->pSigY[ModY]->NULL */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      /* if out signal points to the destination */
      if (pDst->in[i].pOut == pSrc)
      {
         /* found my signal */
         in = i;
         break;
      }
   }
   if (in == -1)
      return IFX_ERROR;
#ifdef DEBUG
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          ("removing Signal %-10s from ", signalName[pDst->in[in].i]));
   switch (pDst->nModType)
   {
      case VINDSP_MT_ALM:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("ALM"));
         break;
      case VINDSP_MT_PCM:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("PCM"));
         break;
      case VINDSP_MT_COD:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("COD"));
         break;
      case VINDSP_MT_SIG:
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("SIG"));
         break;
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL, ("\n"));
#endif
   /* special handling for signaling module */
   if (nSignal == ALM_TO_SIG_IN && pSrc->nModType == VINDSP_MT_SIG)
      pTemp = &pSrc->pInputs2;
   else
      pTemp = &pSrc->pInputs;
   /* pTemp now points to the first input signal of the list of input signals
      connected to the source output. Now find the element which points to our
      signal */
   if (*pTemp == &pDst->in[in])
   {
      /* special case: it is the first signal */
      *pTemp = pDst->in[in].pNext;
   }
   else
   {
      while (*pTemp != NULL && (*pTemp)->pNext != &pDst->in[in])
         *pTemp = (*pTemp)->pNext;

      if (*pTemp == NULL)
         /* not found! */
         return IFX_ERROR;
      /* remove it from list */
      (*pTemp)->pNext = pDst->in[in].pNext;
   }
   /* reset connection */
   pDst->in[in].pNext = NULL;
   pDst->in[in].i = ECMD_IX_EMPTY;
   pDst->in[in].pOut = NULL;
   pDst->modified = IFX_TRUE;


   return IFX_SUCCESS;
}

/**
   Write the configuration according to pSigs
\param pC     - handle to channel structure
\param pSigs  - handle to input signal array
\return
   IFX_SUCCESS or IFX_ERROR
*/
int Con_ConnectConfigure (VINETIC_DEVICE * pDev)
{
   int err = IFX_SUCCESS, ch;
   VINDSP_MODULE *pMod;

   IFXOS_MutexLock (pDev->memberAcc);
   /* write ALM module channel */
   for (ch = 0; ch < pDev->nAnaChan; ++ch)
   {
      pMod = &pDev->pAlmCh[ch].signal;
      if (pMod->modified)
      {
         pDev->pAlmCh[ch].ali_ch.bit.i1 = pMod->in[0].i;
         pDev->pAlmCh[ch].ali_ch.bit.i2 = pMod->in[1].i;
         pDev->pAlmCh[ch].ali_ch.bit.i3 = pMod->in[2].i;
         pDev->pAlmCh[ch].ali_ch.bit.i4 = pMod->in[3].i;
         pDev->pAlmCh[ch].ali_ch.bit.i5 = pMod->in[4].i;
         err = CmdWrite (pDev, pDev->pAlmCh[ch].ali_ch.value, CMD_ALM_CH_LEN);
         pMod->modified = IFX_FALSE;
      }
   }
   for (ch = 0; ch < pDev->nCoderCnt; ++ch)
   {
      pMod = &pDev->pCodCh[ch].signal;
      if (pMod->modified)
      {
         pDev->pCodCh[ch].cod_ch.bit.i1 = pMod->in[0].i;
         pDev->pCodCh[ch].cod_ch.bit.i2 = pMod->in[1].i;
         pDev->pCodCh[ch].cod_ch.bit.i3 = pMod->in[2].i;
         pDev->pCodCh[ch].cod_ch.bit.i4 = pMod->in[3].i;
         pDev->pCodCh[ch].cod_ch.bit.i5 = pMod->in[4].i;
         err = CmdWrite (pDev, pDev->pCodCh[ch].cod_ch.value, CMD_COD_CH_LEN);
         pMod->modified = IFX_FALSE;
      }
   }
   for (ch = 0; ch < pDev->nPcmCnt; ++ch)
   {
      pMod = &pDev->pPcmCh[ch].signal;
      if (pMod->modified)
      {
         pDev->pPcmCh[ch].pcm_ch.bit.i1 = pMod->in[0].i;
         pDev->pPcmCh[ch].pcm_ch.bit.i2 = pMod->in[1].i;
         pDev->pPcmCh[ch].pcm_ch.bit.i3 = pMod->in[2].i;
         pDev->pPcmCh[ch].pcm_ch.bit.i4 = pMod->in[3].i;
         pDev->pPcmCh[ch].pcm_ch.bit.i5 = pMod->in[4].i;

         err = CmdWrite (pDev, pDev->pPcmCh[ch].pcm_ch.value, CMD_PCM_CH_LEN);
         pMod->modified = IFX_FALSE;
      }
   }
   for (ch = 0; ch < pDev->nSigCnt; ++ch)
   {
      pMod = &pDev->pSigCh[ch].signal;
      if (pMod->modified)
      {
         pDev->pSigCh[ch].sig_ch.bit.i1 = pMod->in[0].i;
         pDev->pSigCh[ch].sig_ch.bit.i2 = pMod->in[1].i;
         err = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
         pMod->modified = IFX_FALSE;

      }
   }
   IFXOS_MutexUnlock (pDev->memberAcc);
   return err;
}

/**
   Add a signal connection to the signal array
\param pCh          - handle to TAPI_CONNECTION structure
\param mt           - specifies the module type in which the signal should be
                 added
                 options are: COD, ALM, SIG
\param nSignal      - specifies the signal to be added
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   To keep this this function simple it does not offer to change the value
   of the signaling module i1 as this is not required to realize the
   conferencing implementation.
*/
int addModuleSigIn (VINETIC_CHANNEL * pCh, VINDSP_MT mt, unsigned char nSignal,
                    SIGNAL_ARRAY_INPUTS * pSigs)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   int err = IFX_SUCCESS;
   IFX_uint8_t chg;

   switch (mt)
   {
      case VINDSP_MT_COD:
#ifdef DEBUG
         TRACE (VINETIC, DBG_LEVEL_NORMAL,
                ("adding Signal %-10s to Cod In ch %d\n", signalName[nSignal],
                 pCh->nChannel - 1));
#endif /* DEBUG */
         err = addSigIn (&(pSigs->cod), nSignal, &chg);
         if (err == IFX_SUCCESS)
         {
            if (chg == IFX_TRUE)
               pSigs->codChg = IFX_TRUE;
         }
         else
         {
            SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
            err = IFX_ERROR;
         }
         break;
      case VINDSP_MT_ALM:
#ifdef DEBUG
         TRACE (VINETIC, DBG_LEVEL_NORMAL,
                ("adding Signal %-10s to Alm In ch %d\n", signalName[nSignal],
                 pCh->nChannel - 1));
#endif /* DEBUG */
         if (addSigIn (&(pSigs->alm), nSignal, &chg) == IFX_SUCCESS)
         {
            if (chg == IFX_TRUE)
               pSigs->almChg = IFX_TRUE;
         }
         else
         {
            SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
            err = IFX_ERROR;
         }
         break;
      case VINDSP_MT_SIG:
         /* we change only the signal input from to the coder side, to the
            analog line module the signal input keeps always connected. */
         if (pSigs->sig[1] == ECMD_IX_EMPTY)
         {
#ifdef DEBUG
            TRACE (VINETIC, DBG_LEVEL_NORMAL,
                   ("adding Signal %-10s to Sig In ch %d\n",
                    signalName[nSignal], pCh->nChannel - 1));
#endif /* DEBUG */
            pSigs->sig[1] = nSignal;
            pSigs->sigChg = IFX_TRUE;
         }
         else
         {
            SET_ERROR (ERR_NO_FREE_INPUT_SLOT);
            err = IFX_ERROR;
         }
         break;
      case VINDSP_MT_PCM:
         break;
   }

   return err;
}

/**
   Remove a signal connection from the signal array
\param pCh          - handle to TAPI_CONNECTION structure
\param mt           - specifies the module type in which the signal should be
                 removed
                 options are: VINDSP_MT_COD, VINDSP_MT_ALM, VINDSP_MT_SIG
\param nSignal      - specifies the signal to be removed
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   If nSignal was not connected to the module, IFX_SUCCESS is returned!
   To keep this this function simple it does not offer to change the value
   of the signaling module i1 as this is not required to realize the
   conferencing implementation.
*/
int removeModuleSigIn (VINETIC_CHANNEL * pCh, VINDSP_MT mt,
                       unsigned char nSignal, SIGNAL_ARRAY_INPUTS * pSigs)
{
/*   VINETIC_DEVICE     *pDev = pCh->pParent;
   SIGNAL_ARRAY_INPUTS sai;*/
   int rem;
   int err = IFX_SUCCESS;

   switch (mt)
   {
      case VINDSP_MT_COD:
         rem = removeSigIn (&(pSigs->cod), nSignal);
         if (rem == IFX_TRUE)
         {
            pSigs->codChg = IFX_TRUE;
#ifdef DEBUG
            TRACE (VINETIC, DBG_LEVEL_NORMAL,
                   ("remove Signal %-10s from Cod In ch %d\n",
                    signalName[nSignal], pCh->nChannel - 1));
#endif /* DEBUG */
         }
         break;
      case VINDSP_MT_ALM:
         rem = removeSigIn (&(pSigs->alm), nSignal);
         if (rem == IFX_TRUE)
         {
            pSigs->almChg = IFX_TRUE;
#ifdef DEBUG
            TRACE (VINETIC, DBG_LEVEL_NORMAL,
                   ("remove Signal %-10s from Alm In ch %d\n",
                    signalName[nSignal], pCh->nChannel - 1));
#endif /* DEBUG */
         }
         break;
      case VINDSP_MT_SIG:
         /* we change only the signal input from to the coder side, to the
            analog line module the signal input keeps always connected. */
         if (pSigs->sig[1] == nSignal)
         {
#ifdef DEBUG
            TRACE (VINETIC, DBG_LEVEL_NORMAL,
                   ("remove Signal %-10s from Sig In ch %d\n",
                    signalName[nSignal], pCh->nChannel - 1));
#endif /* DEBUG */
            pSigs->sigChg = IFX_TRUE;
            /* signal is always connected to ALM -> CHG! */
            pSigs->sig[1] = ECMD_IX_EMPTY;
         }
         break;
      case VINDSP_MT_PCM:
         break;
   }

   return err;
}

/**
   Update temporary local structure containing the Signal Array input signals
\param pChannel     - handle to TAPI_CONNECTION structure
\param pSigs        - handle to input signal array
\return
   none
\remarks
*/
void getSigIns (const VINETIC_CHANNEL * pCh, SIGNAL_ARRAY_INPUTS * pSigs)
{
   IFX_uint8_t ch = pCh->nChannel - 1;
   VINETIC_DEVICE *pDev = pCh->pParent;

   pSigs->almChg = IFX_FALSE;
   pSigs->codChg = IFX_FALSE;
   pSigs->sigChg = IFX_FALSE;
   pSigs->alm[0] = pDev->pAlmCh[ch].ali_ch.bit.i1;
   pSigs->alm[1] = pDev->pAlmCh[ch].ali_ch.bit.i2;
   pSigs->alm[2] = pDev->pAlmCh[ch].ali_ch.bit.i3;
   pSigs->alm[3] = pDev->pAlmCh[ch].ali_ch.bit.i4;
   pSigs->alm[4] = pDev->pAlmCh[ch].ali_ch.bit.i5;

   pSigs->sig[0] = pDev->pSigCh[ch].sig_ch.bit.i1;
   pSigs->sig[1] = pDev->pSigCh[ch].sig_ch.bit.i2;

   pSigs->cod[0] = pDev->pCodCh[ch].cod_ch.bit.i1;
   pSigs->cod[1] = pDev->pCodCh[ch].cod_ch.bit.i2;
   pSigs->cod[2] = pDev->pCodCh[ch].cod_ch.bit.i3;
   pSigs->cod[3] = pDev->pCodCh[ch].cod_ch.bit.i4;
   pSigs->cod[4] = pDev->pCodCh[ch].cod_ch.bit.i5;
}

/**
   Write the configuration according to pSigs
\param pC     - handle to channel structure
\param pSigs  - handle to input signal array
\return
   IFX_SUCCESS or IFX_ERROR
*/
int Con_ConfSignals (VINETIC_CHANNEL * pCh, SIGNAL_ARRAY_INPUTS * pSigs)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   int err = IFX_SUCCESS;
   IFX_uint8_t ch = pCh->nChannel - 1;

   IFXOS_MutexLock (pDev->memberAcc);
   /* write ALM module channel */
   if (pSigs->almChg)
   {
      pDev->pAlmCh[ch].ali_ch.bit.i1 = pSigs->alm[0];
      pDev->pAlmCh[ch].ali_ch.bit.i2 = pSigs->alm[1];
      pDev->pAlmCh[ch].ali_ch.bit.i3 = pSigs->alm[2];
      pDev->pAlmCh[ch].ali_ch.bit.i4 = pSigs->alm[3];
      pDev->pAlmCh[ch].ali_ch.bit.i5 = pSigs->alm[4];
      err = CmdWrite (pDev, pDev->pAlmCh[ch].ali_ch.value, CMD_ALM_CH_LEN);
      pSigs->almChg = IFX_FALSE;

   }
   if (pSigs->codChg)
   {
      pDev->pCodCh[ch].cod_ch.bit.i1 = pSigs->cod[0];
      pDev->pCodCh[ch].cod_ch.bit.i2 = pSigs->cod[1];
      pDev->pCodCh[ch].cod_ch.bit.i3 = pSigs->cod[2];
      pDev->pCodCh[ch].cod_ch.bit.i4 = pSigs->cod[3];
      pDev->pCodCh[ch].cod_ch.bit.i5 = pSigs->cod[4];
      err = CmdWrite (pDev, pDev->pCodCh[ch].cod_ch.value, CMD_COD_CH_LEN);
      pSigs->codChg = IFX_FALSE;
   }
   if (pSigs->sigChg)
   {
      pDev->pSigCh[ch].sig_ch.bit.i1 = pSigs->sig[0];
      pDev->pSigCh[ch].sig_ch.bit.i2 = pSigs->sig[1];
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
      pSigs->sigChg = IFX_FALSE;
   }
   IFXOS_MutexUnlock (pDev->memberAcc);
   return err;
}

/* @} */
