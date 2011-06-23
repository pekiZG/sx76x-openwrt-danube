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

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

#ifdef DEBUG
const IFX_char_t signalName [52][10] = {
   "Null\0",
   "PCM0-Out\0",
   "PCM1-Out\0",
   "PCM2-Out\0",
   "PCM3-Out\0",
   "PCM4-Out\0",
   "PCM5-Out\0",
   "PCM6-Out\0",
   "PCM7-Out\0",
   "PCM8-Out\0",
   "PCM9-Out\0",
   "PCM10-Out\0",
   "PCM11-Out\0",
   "PCM12-Out\0",
   "PCM13-Out\0",
   "PCM14-Out\0",
   "PCM15-Out\0",
   "ALM0-Out\0",
   "ALM1-Out\0",
   "ALM2-Out\0",
   "ALM3-Out\0",
   "\0","\0","\0",
   "COD0-Out\0",
   "COD1-Out\0",
   "COD2-Out\0",
   "COD3-Out\0",
   "COD4-Out\0",
   "COD5-Out\0",
   "COD6-Out\0",
   "COD7-Out\0",
   "\0","\0","\0","\0","\0","\0","\0","\0",
   "SIG0-OutA\0",
   "SIG0-OutB\0",
   "SIG1-OutA\0",
   "SIG1-OutB\0",
   "SIG2-OutA\0",
   "SIG2-OutB\0",
   "SIG3-OutA\0",
   "SIG3-OutB\0",
   "SIG4-OutA\0",
   "SIG4-OutB\0",
   "SIG5-OutA\0",
   "SIG5-OutB\0"
};
#endif /* DEBUG */


/* ============================= */
/* Local function definition     */
/* ============================= */


/** \defgroup VINETIC_CONNECTIONS VINETIC Connection Module */

/* ============================= */
/* Global function definition    */
/* ============================= */
/** \addtogroup VINETIC_CONNECTIONS */
/* @{ */

/**
   Add a signal connection to the internal shadow connection structure.
   Call Con_ConnectConfigure() to write the shadow structure to the chip.
\param pSrc      - handle to the source module which contains the output signal
\param pDst      - handle to the destination module where the output
                   is connected to
\param nSrcOut   - specifies which output to use when the source is a 
   signaling module, don't care for other module types (set to 0). 
   If set to LOCAL_SIG_OUT then the output towards the local side is
   used and with REMOTE_SIG_OUT the output towards the remote side.
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This function only connects two given modules if they are not already
   connected. The destination is set to modified if all was successful.
   nSignal is only needed for special signaling module handling.
   It is important which input is used for ALM, PCM and Coder. Also
   it is important to which input a module is connected to the signaling:
   local modules like ALM and PCM (not always) support auto suppresion and
   get the event playout. They are connected to In 1.
   Remote modules like coder and PCM in case of advanced with DSP are 
   connected to In 2.
   The coder channel then gets the Sig-OutA output of the signaling channel.
   The analog channel get the Sig-OutB (LOCAL_SIG_OUT). 
   
                          nSignal
                          pInputs
   Signalling module:      |----------------|
                       <-  | Out A     In 1 |  <-
                           |  upstream (tx) |
          remote <==       |  -  -  -  -  - |       ==> local
                           | downstream (rx)|
                       ->  | In 2     Out B |  ->
                           |----------------|
                                      nSignal2
                                      pInputs2
   
   */
int Con_ConnectPrepare (VINDSP_MODULE* pSrc, VINDSP_MODULE* pDst,
                        IFX_uint8_t nSrcOut)
{
   int i, in = -1;
   VINDSP_MODULE_SIGNAL **pInput;
   IFX_uint8_t sig;

   /* get signal number and pointer to the output of the source module */
   if (nSrcOut == LOCAL_SIG_OUT && pSrc->nModType == VINDSP_MT_SIG)
   {
      /* SIG module: use the local output (Out B) of the signalling module */
      pInput = &pSrc->pInputs2;
      sig = pSrc->nSignal2;
   }
   else
   {
      /* PCM-, COD-, ALM-module: use the standard output of given module */
      /* SIG module: use the remote output (Out A) of the signalling module */
      pInput = &pSrc->pInputs;
      sig = pSrc->nSignal;
   }
   /* *pInput now points to the first input signal of the list of input signals
      connected to this output. sig contains the number of the signal in the 
      signal array */

   /* return now if the output is already connected to one of the inputs */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if (pDst->in[i].i == sig)
         return IFX_SUCCESS;
   }
   /* find the index of a free input on the destination module */
   if (pDst->nModType == VINDSP_MT_SIG)
   {
      /* default input on signaling module is input 1 */
      in = LOCAL_SIG_IN;

      /* exception 1: coder connects always to input 2 */
      if (pSrc->nModType == VINDSP_MT_COD)
         in = REMOTE_SIG_IN;

      /* exception 2 for PCM: in advanced PCM it does not act as ALM, but as
         coder. In that case the ALM must be connected first, which is
         assured in the basic init. Dynamically assignment for PCM with DSP
         is not supported */
      if (pSrc->nModType == VINDSP_MT_PCM && nSrcOut == LOCAL_SIG_OUT)
         if (pDst->in[LOCAL_SIG_IN].i != ECMD_IX_EMPTY)
            in = REMOTE_SIG_IN;

      /* finally verify that the input selected is currently not in use */
      if (pDst->in[in].i != ECMD_IX_EMPTY)
      {
         TRACE(VINETIC, DBG_LEVEL_NORMAL,("Input signal on SIG already in use\n"));
         return IFX_ERROR;
      }
   }
   else
   {
      /* find free input on the destination module (COD, ALM or PCM) */
      for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
      {
         if (pDst->in[i].i == ECMD_IX_EMPTY)
         {
            in = i;
            break;
         }
      }
   }
#ifdef DEBUG
   TRACE(VINETIC, DBG_LEVEL_NORMAL,
          ("adding Signal %-10s to ", signalName[sig]));
   switch (pDst->nModType)
   {
   case VINDSP_MT_ALM:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("ALM"));
      break;
   case VINDSP_MT_PCM:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("PCM"));
      break;
   case VINDSP_MT_COD:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("COD"));
      break;
   case VINDSP_MT_SIG:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("SIG"));
      break;
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          (" (Out-Sig %s) [input:%d]\n", signalName[pDst->nSignal], in));
#endif
   if (in == -1)
   {
      /* placed after the trace to get details on the modules (debug only) */
      TRACE(VINETIC, 
            DBG_LEVEL_NORMAL,("No free input on destination module - stop\n"));
      return IFX_ERROR;
   }

   /* Connect output to the input */
   if (pDst->in[in].i != sig)
   {
      pDst->modified = IFX_TRUE;
      pDst->in[in].i = sig;
   }
   /* Set the pointer showing which output connects to this input */
   pDst->in[in].pOut = pSrc;
   /* Add the input to the linked list of all inputs attached to one output. 
      Always add the element to the head of the list.*/
   pDst->in[in].pNext = *pInput;
   *pInput = &pDst->in[in];

   return IFX_SUCCESS;
}

/**
   Removes a signal connection from the internal shadow connection structure.
   Call Con_ConnectConfigure() to write the shadow structure to the chip.
\param pSrc      - handle to the source module which contains the output signal
\param pDst      - handle to the destination module where the output
                   is connected to
\param nSrcOut   - specifies which output to use when the source is a 
   signaling module, don't care for other module types (set to 0). 
   If set to LOCAL_SIG_OUT then the output towards the local side is
   used and with REMOTE_SIG_OUT the output towards the remote side.
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   To remove a signal connection the input in the destination module has to
   be unlinked from the list the source module keeps of all inputs connecting
   to it's output. Then the input struct in the destination can be blanked.
   If all was successful the destination is set to modified.
*/
int Con_DisconnectPrepare (VINDSP_MODULE* pSrc, VINDSP_MODULE* pDst,
                           IFX_uint8_t nSigOut)
{
   int i, in = -1;
   VINDSP_MODULE_SIGNAL **pBase, 
                         *pTemp;

   /* find the input on the destination which is connected to the source */
   for (i = 0; i < MAX_MODULE_SIGNAL_INPUTS; i++)
   {
      if (pDst->in[i].pOut == pSrc)
      {
         in = i;
         break;
      }
   }
#ifdef DEBUG
   TRACE(VINETIC, DBG_LEVEL_NORMAL,
         ("removing Signal %-10s from ", signalName[pDst->in[in].i]));
   switch (pDst->nModType)
   {
   case VINDSP_MT_ALM:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("ALM"));
      break;
   case VINDSP_MT_PCM:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("PCM"));
      break;
   case VINDSP_MT_COD:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("COD"));
      break;
   case VINDSP_MT_SIG:
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("SIG"));
      break;
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL,
          (" (Out-Sig %s) [input:%d]\n", signalName[pDst->nSignal], in));
#endif
   if (in == -1)
   {
      /* placed after the trace to get details on the modules (debug only) */
      TRACE(VINETIC, DBG_LEVEL_NORMAL,("Given modules are not connected\n"));
      return IFX_ERROR;
   }
   /* Get a pointer to the basepointer of the list of inputs connected. */
   if (nSigOut == LOCAL_SIG_OUT && pSrc->nModType == VINDSP_MT_SIG)
      /* SIG-module only: inputs connected on local-side */
      pBase = &pSrc->pInputs2;
   else
      /* PCM-, COD-, ALM-module: standard input */
      /* SIG module: inputs connected on remote-side */
      pBase = &pSrc->pInputs;
   /* Get a pointer to the first element in the linked list of inputs. */
   pTemp = *pBase;
   /* pTemp now points to the first node in the list of input signals
      connected to this output. Now find the node of the input found
      above and remove this node from the linked list. */
   if (*pBase == &pDst->in[in])
   {
      /* special case: it is the first node so move the base pointer */
      *pBase = pDst->in[in].pNext;
   }
   else
   {
      /* Walk the list and try to find the node to be deleted. */
      while (pTemp != NULL && pTemp->pNext != &pDst->in[in])
         pTemp = pTemp->pNext;
      if (pTemp == NULL)
         /* not found! The data structure is corrupt - this should not happen*/
         return IFX_ERROR;
      /* unlink the node from the list */
      pTemp->pNext = pTemp->pNext->pNext;
   }
   /* clear this input -> connection is now removed */
   pDst->in[in].pNext = NULL;
   pDst->in[in].i     = ECMD_IX_EMPTY;
   pDst->in[in].pOut  = NULL;
   pDst->modified     = IFX_TRUE;

   return IFX_SUCCESS;
}

/**
   Write the configuration from the internal shadow connection structure
   to the chip (firmware).
\param pDev     - handle to device structure
\return
   IFX_SUCCESS or IFX_ERROR
*/
int Con_ConnectConfigure (VINETIC_DEVICE *pDev)
{
   int err = IFX_SUCCESS, ch;
   VINDSP_MODULE *pMod;

   IFXOS_MutexLock (pDev->memberAcc);

   /* write ALM module channel */
   for (ch = 0; err == IFX_SUCCESS && ch < pDev->nAnaChan; ++ch)
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
         if (err == IFX_SUCCESS)
            pMod->modified = IFX_FALSE;
      }
   }
   /* write CODER module channel */
   for (ch = 0; err == IFX_SUCCESS && ch < pDev->nCoderCnt; ++ch)
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
         if (err == IFX_SUCCESS)
            pMod->modified = IFX_FALSE;
      }
   }
   /* write PCM module channel */
   for (ch = 0; err == IFX_SUCCESS && ch < pDev->nPcmCnt; ++ch)
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
         if (err == IFX_SUCCESS)
            pMod->modified = IFX_FALSE;
      }
   }
   /* write SIGNALLING module channel */
   for (ch = 0; err == IFX_SUCCESS && ch < pDev->nSigCnt; ++ch)
   {
      pMod = &pDev->pSigCh[ch].signal;
      if (pMod->modified)
      {
         pDev->pSigCh[ch].sig_ch.bit.i1 = pMod->in[0].i;
         pDev->pSigCh[ch].sig_ch.bit.i2 = pMod->in[1].i;
         err = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
         if (err == IFX_SUCCESS)
            pMod->modified = IFX_FALSE;
      }
   }

   IFXOS_MutexUnlock (pDev->memberAcc);

   return err;
}

/* @} */
