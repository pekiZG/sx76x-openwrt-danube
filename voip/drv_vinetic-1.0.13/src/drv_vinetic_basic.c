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
   Module      : drv_vinetic_basic.c
 This file contains the implementation of basic VINETIC
                 access functions as WriteCmd, ReadCmd, register access
                 and so on.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_vinetic_api.h"
#include "drv_vinetic_basic.h"
#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_con.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
IFX_LOCAL IFX_int32_t getCoder                    (IFX_int32_t nEncoder);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
IFX_LOCAL IFX_int32_t PCM_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath);
IFX_LOCAL IFX_int32_t ALM_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath);
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
IFX_LOCAL IFX_int32_t CODER_baseConf (VINETIC_CHANNEL * pCh);
IFX_LOCAL IFX_int32_t SIGNL_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
   function called to get Codec value according to Encoder
\param
   nEncoder   - Actual Encode
\return
   Coder value or IFX_ERROR
Remarks
*/
IFX_LOCAL IFX_int32_t getCoder (IFX_int32_t nEncoder)
{
   switch (nEncoder)
   {
   case COD_CH_G723_63_ENC:
      return (G723_63);
   case COD_CH_G723_53_ENC:
      return (G723_53);
   case COD_CH_G728_ENC:
      return (G728);
   case COD_CH_G729_ENC:
       return (G729);
   case COD_CH_G711_MLAW_ENC:
      return (MLAW);
   case COD_CH_G711_ALAW_ENC:
      return (ALAW);
   case COD_CH_G726_16_ENC:
      return (G726_16);
   case COD_CH_G726_24_ENC:
      return (G726_24);
   case COD_CH_G726_32_ENC:
      return (G726_32);
   case COD_CH_G726_40_ENC:
      return (G726_40);
   case COD_CH_G729_E_ENC:
      return (G729_E);
   default:
      return IFX_ERROR;
   }
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Write short command
   pDev: pointer to the device interface
   pCmd: pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t VINETIC_Write_Sc (VINETIC_DEVICE * pDev, VINETIC_IO_WRITE_SC * pCmd)
{
   IFX_uint16_t cmd = 0;
   IFX_int32_t  ret = IFX_SUCCESS;

   /* Set write cmd and write */
   cmd = CMD1_WR | CMD1_SC | pCmd->nCmd;
   if (pCmd->nBc)
   {
      cmd |= CMD1_BC;
   }
   cmd |= (pCmd->nCh & CMD1_CH);
   ret = ScWrite (pDev, cmd);
   return ret;
}

/**
   Read short command
   pDev: pointer to the device interface
   pCmd: pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t VINETIC_Read_Sc (VINETIC_DEVICE * pDev, VINETIC_IO_READ_SC * pCmd)
{
   IFX_uint16_t cmd = 0;
   IFX_int32_t  err = IFX_SUCCESS;

   /* set parameters */
   cmd = CMD1_RD | CMD1_SC | pCmd->nCmd;
   if (pCmd->nBc)
   {
      cmd |= CMD1_BC;
   }
   cmd |= (pCmd->nCh & CMD1_CH);
   err = ScRead (pDev, cmd, pCmd->pData, pCmd->count);

   return err;
}

/**
   Write VINETIC command
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR (return value of CmdWrite)
*/
IFX_int32_t VINETIC_Write_Cmd (VINETIC_DEVICE * pDev, VINETIC_IO_MB_CMD * pCmd)
{
   IFX_int32_t  ret;
   IFX_uint16_t *pData = (IFX_uint16_t *) pCmd;

   if ((pData[0] & CMD1_CMD) == CMD1_EOP && (pData[0] & CMD1_RD) != CMD1_RD)
   {
      IFXOS_MutexLock   (pDev->memberAcc);
      DispatchCmd (pDev, pData);
   }
   ret = CmdWrite (pDev, pData, (pCmd->cmd2 & CMD2_LEN));
   if ((pData[0] & CMD1_CMD) == CMD1_EOP)
   {
      IFXOS_MutexUnlock (pDev->memberAcc);
   }

   return ret;
}

/**
   Write command to read data
\param
   pDev  - pointer to the device interface
\param
   pCmd  - pointer to command data
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*/
IFX_int32_t VINETIC_Read_Cmd (VINETIC_DEVICE * pDev, VINETIC_IO_MB_CMD * pCmd)
{
   IFX_int32_t err = CmdRead (pDev, (IFX_uint16_t *) pCmd,
                              (IFX_uint16_t *) pCmd, pCmd->cmd2 & CMD2_LEN);

   /* if read failed set the cmd length to 0 */
   if (err != IFX_SUCCESS)
      pCmd->cmd2 = 0;

   return err;
}

#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
/**
  Configure VINETIC Chip according to Tapi settings
\param
   pDev  - pointer to VINETIC device structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This configuration will be done if tapi configuration mode is TAPI_VOICE_CODER

   Access to cached firmware messages is protected against concurrent tasks by
   the share variable mutex
*/
IFX_int32_t Basic_VoIPConf (VINETIC_CHANNEL * pCh)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t pCmd[3] = { 0 };

#if (VIN_CFG_FEATURES & VIN_FEAT_VIN_S)
   if ((pDev->nChipType == VINETIC_TYPE_S) &&
       (pDev->nChipMajorRev == VINETIC_V2x))
   {
      /* not supported for S type */
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VIN_S) */

   /* EOP Cmd */
   pCmd[0] = CMD1_EOP;
   /* EN = 1 */
   pCmd[2] = 0x8000;

   if (!(pDev->nDevState & DS_COD_EN))
   {
      /* configure Coder Module */
      pCmd[1] = ECMD_COD_CTL;
      err = CmdWrite (pDev, pCmd, 1);
      if (err == IFX_SUCCESS)
         /* set coder module to enabled */
         pDev->nDevState |= DS_COD_EN;
   }
   if (!(pDev->nDevState & DS_ALM_EN))
   {
      /* configure analog line module */
      pCmd[1] = ECMD_ALM_CTL;
      err = CmdWrite (pDev, pCmd, 1);
      if (err == IFX_SUCCESS)
         /* set coder module to enabled */
         pDev->nDevState |= DS_ALM_EN;
   }
   if (!(pDev->nDevState & DS_SIG_EN))
   {
      /* configure signaling module */
      pCmd[1] = ECMD_SIG_CTL;
      err = CmdWrite (pDev, pCmd, 1);
      if (err == IFX_SUCCESS)
         /* set coder module to enabled */
         pDev->nDevState |= DS_SIG_EN;
   }
#if 1
   if (!(pDev->nDevState & DS_PCM_EN))
   {
      /* configure PCM interface control */
      pCmd[1] = ECMD_PCM_CTL;
      pCmd[2] = PCM_CTL_EN;
      err = CmdWrite (pDev, pCmd, 1);
      if (err == IFX_SUCCESS)
         /* set coder module to enabled */
         pDev->nDevState |= DS_PCM_EN;
   }
   if (err != IFX_SUCCESS)
      return err;
#endif /** \todo quick fix: activate PCM module for possible connections  */

   /* protect fwmsgs against concurrent tasks */
   IFXOS_MutexLock (pDev->memberAcc);
   /* configure ALI for VoIP */
   if (pCh->nChannel <= pDev->nAnaChan)
      err = ALM_baseConf (pCh, TAPI_VOICE_CODER);
   /* configure Coder  */
   if (err == IFX_SUCCESS && (pCh->nChannel <= pDev->nCoderCnt))
      err = CODER_baseConf (pCh);
   /* Configure Signalling module */
   if (err == IFX_SUCCESS && (pCh->nChannel <= pDev->nSigCnt))
      err = SIGNL_baseConf (pCh, TAPI_VOICE_CODER);
   /* unlock */
   IFXOS_MutexUnlock (pDev->memberAcc);

   Con_ConnectConfigure (pDev);
   /* do other settings if every successfull */
   if (err == IFX_SUCCESS)
   {
      /* save configured coders for later checks */
      pCh->pTapiCh->nCodec = getCoder(pDev->pCodCh[pCh->nChannel - 1].enc_conf);
   }

   return err;
}
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */

/**
  Configure VINETIC Chip for Voice Coder purposes
\param
   pDev       - pointer to VINETIC device structure
\param
   nPcmType   - PCM Advanced or for application. Not yet in use.
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This function configures the chip for voice communication via PCM.
   The actual configuration is for PCM-S and the parameter nPcmType isn't
   evaluated . This will be done when the PCM-Standard config will be done.

   Access to cached firmware messages is protected against concurrent tasks by
   the share variable mutex
*/
IFX_int32_t Basic_PcmConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPcmType)
{
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t pCmd[3] = { 0 };
   IFX_int32_t err = IFX_SUCCESS;

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   if ((pDev->nChipType == VINETIC_TYPE_S) && (nPcmType != TAPI_PCM_PHONE))
   {
      /* not supported for S type */
      SET_ERROR (ERR_NOTSUPPORTED);
      return IFX_ERROR;
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

   /* EOP Cmd */
   pCmd[0] = CMD1_EOP;

   if (!(pDev->nDevState & DS_PCM_EN))
   {
      /* configure PCM interface control */
      pCmd[1] = ECMD_PCM_CTL;
      pCmd[2] = PCM_CTL_EN;
      err = CmdWrite (pDev, pCmd, 1);
   }
   if (err != IFX_SUCCESS)
      return err;
   /* set PCM module to enabled */
   pDev->nDevState |= DS_PCM_EN;

   if (!(pDev->nDevState & DS_ALM_EN))
   {
      /* configure analog line module */
      pCmd[1] = ECMD_ALM_CTL;
      pCmd[2] = ALM_CTL_EN;
      err = CmdWrite (pDev, pCmd, 1);
   }
   if (err != IFX_SUCCESS)
      return err;
   /* set analog line module to enabled */
   pDev->nDevState |= DS_ALM_EN;

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   if (nPcmType == TAPI_PCM_DSP)
   {
      if (!(pDev->nDevState & DS_SIG_EN))
      {
         /* configure signaling module */
         pCmd[1] = ECMD_SIG_CTL;
         pCmd[2] = SIG_CTL_EN;
         err = CmdWrite (pDev, pCmd, 1);
      }
      if (err != IFX_SUCCESS)
         return err;

      /* set signalling module to enabled */
      pDev->nDevState |= DS_SIG_EN;
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

   /* protect fwmsgs against concurrent tasks */
   IFXOS_MutexLock (pDev->memberAcc);

   /* configure ALI for PCM */
   if (pCh->nChannel <= pDev->nAnaChan)
      err = ALM_baseConf (pCh, nPcmType);
   if (err != IFX_SUCCESS)
      goto error;

   /* configure PCM  */
   if (pCh->nChannel <= pDev->nPcmCnt)
      err = PCM_baseConf (pCh, nPcmType);

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   if (err != IFX_SUCCESS)
      goto error;
   if (nPcmType == TAPI_PCM_DSP && (pCh->nChannel <= pDev->nSigCnt))
      /* Configure Signalling module */
      err = SIGNL_baseConf (pCh, TAPI_PCM_DSP);
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

error:
   IFXOS_MutexUnlock (pDev->memberAcc);
   if (err == IFX_SUCCESS)
       err = Con_ConnectConfigure (pDev);

   return err;
}

/**
   base PCM Module configuration
\param
   pDev  - pointer to the device interface
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remark:
   Use this function where needed to set the base configuration
   of the Coder Module. This function isn't an IOCTL function
   This function configures:
      - PCM Module
      - PCM Channels
*/
IFX_LOCAL IFX_int32_t PCM_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath)
{
   VINETIC_DEVICE *pDev = pCh->pParent;

   IFX_int32_t  err = IFX_SUCCESS;
   IFX_int32_t  ch = (pCh->nChannel - 1);

   switch (nPath)
   {
	   case TAPI_PCM_DSP:
         err = Con_ConnectPrepare (&pDev->pSigCh[ch].signal,
                                   &pDev->pPcmCh[ch].signal, 0);
   	   break;
	   case TAPI_PCM_PHONE:
	      if (ch < pDev->nAnaChan)
	            err = Con_ConnectPrepare (&pDev->pAlmCh[ch].signal,
	                                      &pDev->pPcmCh[ch].signal, 0);
         break;
      default:
         SET_ERROR (ERR_INVALID);
         return IFX_ERROR;
   }
   pDev->pPcmCh[ch].pcm_ch.bit.i2 = ECMD_IX_EMPTY;
   pDev->pPcmCh[ch].pcm_ch.bit.i3 = ECMD_IX_EMPTY;
   pDev->pPcmCh[ch].pcm_ch.bit.i4 = ECMD_IX_EMPTY;
   pDev->pPcmCh[ch].pcm_ch.bit.i5 = ECMD_IX_EMPTY;
   if (err == IFX_SUCCESS)
	   err = CmdWrite (pDev, pDev->pPcmCh[ch].pcm_ch.value, CMD_PCM_CH_LEN);
   return err;
}

/**
   base ALM configuration
\param
   pDev   - pointer to the device structure
\param
   nPath  - Voice path selected. Either PCM or CODEC
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remark:
   Use this function where needed to set the base configuration
   of Analog Line Module. This function isn't an IOCTL function
   This function configures:
      - ALI Module
      - all ALI channels
      - all ALI NELECs
*/
IFX_LOCAL IFX_int32_t ALM_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath)
{
   VINETIC_DEVICE *pDev         = pCh->pParent;
   IFX_int32_t  err             = IFX_SUCCESS;
   IFX_uint8_t  ch = (pCh->nChannel - 1);

   /* configure ALI channels ************************************************ */
   memset (&pDev->pAlmCh[ch].ali_ch.value[CMD_HEADER_CNT], 0,
           CMD_ALM_CH_LEN * 2);
   pDev->pAlmCh[ch].ali_ch.bit.en = 1;
   pDev->pAlmCh[ch].ali_ch.bit.gain_r   = ALM_GAIN_0DB;
   pDev->pAlmCh[ch].ali_ch.bit.gain_x   = ALM_GAIN_0DB;
   switch (nPath)
   {
   case TAPI_VOICE_CODER:
   case TAPI_PCM_DSP:
      /* signal connection */
      Con_ConnectPrepare (&pDev->pSigCh[ch].signal, &pDev->pAlmCh[ch].signal,
                          ALM_TO_SIG_IN);
      break;
   case TAPI_PCM_PHONE:
         Con_ConnectPrepare (&pDev->pPcmCh[ch].signal, &pDev->pAlmCh[ch].signal,
                             0);
      break;
   default:
      SET_ERROR (ERR_INVALID);
      return IFX_ERROR;
   }
   /* switch it on */
   err = CmdWrite (pDev, pDev->pAlmCh[ch].ali_ch.value, CMD_ALM_CH_LEN);
#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
   if (pDev->nChipType != VINETIC_TYPE_S)
   {
      /* configure ALI NELEC + Lec ressources ******************************* */
      if (err == IFX_SUCCESS)
         err = Dsp_SetNeLec_Alm (pCh, IFX_TRUE, IFX_TRUE);
   }
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */
   if (err != IFX_SUCCESS)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("ALM configuration failed\n\r"));
   }

   return err;
}

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))
/**
   base CODER Module configuration
\param
   pDev  - pointer to the device interface
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remark:
   Use this function where needed to set the base configuration
   of the Coder Module. This function isn't an IOCTL function
   This function configures:
      - CODER Module
      - all CODER Channels
      - all CODER Jitter Buffers
      - Coder Channel decoder status and Coder Channel profiles in case of AAL
*/
IFX_LOCAL IFX_int32_t CODER_baseConf (VINETIC_CHANNEL * pCh)
{
   VINETIC_DEVICE *pDev   = pCh->pParent;
   IFX_uint16_t pCmd[14] = { 0 };
   IFX_int32_t  err       = IFX_SUCCESS, i;
   IFX_uint8_t  ch        = (pCh->nChannel - 1);

   /* Configure Coder Channels RTP or AAL *********************************** */
   if (err == IFX_SUCCESS)
   {
      if ((pDev->nEdspVers & ECMD_VERS_EDSP_PRT) == ECMD_VERS_EDSP_PRT_AAL)
      {
         /* configure AAL profile */
         pCmd[1] = ECMD_COD_CHAAL;
         /* CID = 0x3E */
         pCmd[2] = 0x3E00;
         /* SQNR_INTV = 0x04 for 5.5 ms
            (G711, ALaw/ULaw - G726, 16Kbps/32 Kbps */
         pCmd[3] = COD_CHAAL_SQNR_INTV_5_5MS;
         /* Formula : LI = (bitrate[Kbps]*pte)/8 - 1 */

         /* LI = 0x2B, ENC = 0x03 : G711 ULaw, 64 Kbps */
         pCmd[4] = 0xAC03;
         /* LI = 0x0A, ENC = 0x04 : G726, 16 Kbps, 5.5 ms */
         pCmd[5] = 0x2804;
         /* LI = 0x0x15, ENC = 0x06: G726, 32 Kbps */
         pCmd[6] = 0x5406;
         for (i = 7; i <= 13; i++)
            pCmd[i] = 0;
         err = CmdWrite (pDev, pCmd, 12);
      }
   }
   /* Configure Coder Channels ********************************************** */
   if (err == IFX_SUCCESS)
   {
      /* reset all data fields/bits in the fw message
         “Coder Channel Speech Compression” to zero. Explicite set to zero
         is not necessary */
      memset (&pDev->pCodCh[ch].cod_ch.value[CMD_HEADER_CNT], 0,
              (CMD_COD_CH_LEN * 2));
      pDev->pCodCh[ch].signal.modified = IFX_TRUE;
      pDev->pCodCh[ch].cod_ch.value[3]      = 0xF700;
      pDev->pCodCh[ch].cod_ch.bit.codnr     = ch;
      pDev->pCodCh[ch].cod_ch.bit.gain1     = COD_GAIN_0DB;
      pDev->pCodCh[ch].cod_ch.bit.gain2     = COD_GAIN_0DB;
      /* store the encoder value local till the coder-module gets enabled and
         is not muted */
      pDev->pCodCh[ch].enc_conf             = COD_CH_G711_ULAW_ENC;
      switch (pDev->nEdspVers & ECMD_VERS_EDSP_PRT)
      {
         case ECMD_VERS_EDSP_PRT_AAL:
            pDev->pCodCh[ch].cod_ch.bit.pte = COD_CH_PTE_5_5MS;
            break;
         default:
            pDev->pCodCh[ch].cod_ch.bit.pte = COD_CH_PTE_10MS;
      }
#ifdef NO_DATA_WIRING
      pDev->pCodCh[ch].cod_ch.bit.i1        = ECMD_IX_EMPTY;
#else
      /* here use signal2 */
      Con_ConnectPrepare (&pDev->pSigCh[ch].signal, &pDev->pCodCh[ch].signal,
                          COD_TO_SIG_IN);
#endif /* NO_DATA_WIRING */
      /* configure it */
      err = CmdWrite (pDev, pDev->pCodCh[ch].cod_ch.value, CMD_COD_CH_LEN);
   }

   /* Configure coder channel decoder status to issue interrupts on change of
      Decoder or Packet time */
   if (err == IFX_SUCCESS)
   {
      if ((pDev->nEdspVers & ECMD_VERS_EDSP_PRT) == ECMD_VERS_EDSP_PRT_AAL)
      {
         /* set cmd 2 */
         pCmd[1] = ECMD_COD_CHDECSTAT;
         /* PTE = 1 (to set with profile), DC = 1 */
         pCmd[2] = (COD_CHDECSTAT_PTC | COD_CHDECSTAT_DC);
         err = CmdWrite (pDev, pCmd, 1);
      }
   }
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
   /* set up little endian mode in firmware */
   pCmd[0] = CMD1_EOP;
   pCmd[1] = ECMD_ENDIAN_CTL;
   pCmd[2] = ECMD_ENDIAN_LE;
   err = CmdWrite (pDev, pCmd, 1);
#endif /* (__BYTE_ORDER == __LITTLE_ENDIAN) */
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_VOICE) */
   if (err != IFX_SUCCESS)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("CODER configuration failed\n\r"));
   }
   return err;
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

#if (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M))

/**
   base SIGNALLING Module configuration
\param
   pDev  - pointer to the device interface
\param
   nPath  - Voice path selected. Either PCM or CODEC
\return
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remark:
   Use this function where needed to set the base configuration
   of Signalling Module. This function isn't an IOCTL function
   This function configures:
      - SIGNALLING Module
      - all SIGNALLING Channels
      - CID, DTMF gen & recv, ATDs, UTDs

   In case of PCM voice path, the signalling channle isn't yet connected to any
   PCM Output. This is to be changed later if.

   Note: the following configuration is suitable for network tone detection :
         pCmd [2] = (0x8000 | (((0x18 + i) << 8) & 0x3F00)) | (0x11 + i);
         The switching should be done when this feature is activated by ioctl.
         this configuration is still in examination.
*/
IFX_LOCAL IFX_int32_t SIGNL_baseConf (VINETIC_CHANNEL * pCh, IFX_uint8_t nPath)
{
   VINETIC_DEVICE *pDev  = pCh->pParent;
   IFX_uint16_t pCmd[9] = { 0 };
   IFX_int32_t  err      = IFX_SUCCESS;
   IFX_uint8_t  ch = (pCh->nChannel - 1);

   /* set channel */
   pCmd[0] = CMD1_EOP | ch;
   /* Configure Signalling Channels ***************************************** */
   pDev->pSigCh[ch].sig_ch.value[CMD_HEADER_CNT] = 0;
   pDev->pSigCh[ch].sig_ch.bit.en  = 1;
   pDev->pSigCh[ch].sig_ch.bit.i1  = ECMD_IX_ALM_OUT0 + ch;

   switch (nPath)
   {
      case TAPI_VOICE_CODER:
         if (ch < pDev->nAnaChan && ch < pDev->nSigCnt)
            /* EN = 1, i1 = ALI Output ch ch, i2 = Coder Output ch ch */
            Con_ConnectPrepare (&pDev->pAlmCh[ch].signal,
                                &pDev->pSigCh[ch].signal, 0);
         if (ch < pDev->nSigCnt && ch < pDev->nCoderCnt)
            Con_ConnectPrepare (&pDev->pCodCh[ch].signal,
                                &pDev->pSigCh[ch].signal, 0);
         break;
      case TAPI_PCM_DSP:
         if (ch < pDev->nAnaChan && ch < pDev->nSigCnt)
            /* EN = 1, i1 = ALI Output ch ch, i2 = PCM Output ch ch. First
               connect the ALM to sig input 1 (implied in con module) */
            Con_ConnectPrepare (&pDev->pAlmCh[ch].signal,
                                &pDev->pSigCh[ch].signal, 0);
         if (ch < pDev->nSigCnt && ch < pDev->nPcmCnt)
         {
            Con_ConnectPrepare (&pDev->pPcmCh[ch].signal,
                                &pDev->pSigCh[ch].signal, ALM_TO_SIG_IN);
         }
         break;
      case TAPI_PCM_PHONE:
      default:
         /* should not happen! */
         return IFX_ERROR;
   }
   /* switch it on */
   err = CmdWrite (pDev, pDev->pSigCh[ch].sig_ch.value, CMD_SIG_CH_LEN);
   /* set CID Defaults ****************************************************** */
   if (err == IFX_SUCCESS)
   {
      /* set CID coefficients for type 1 as default */
      err = SIGNL_SetCIDCoeff (pCh, CID_TYPE_1);
   }
   if (err == IFX_SUCCESS)
   {
      /* set length & cmd 2 */
      pCmd[1] = ECMD_CID_SEND;
      /* EN = 1 , AD = 1, HLEV = 1, V23 = 1, A1 = 1, A2 = 1 */
      pCmd[2] = (SIG_CID_EN | SIG_CID_AD | SIG_CID_HLEV | SIG_CID_V23 |
                 SIG_CID_A2_VOICE | ch);

      err = CmdWrite (pDev, pCmd, 1);
#ifdef TAPI_CID
      /* update internal cid configuration structure */
      if (err == IFX_SUCCESS)
      {
         pCh->pTapiCh->TapiCidConfig.nAutoDeac = (pCmd[2] & SIG_CID_AD) >> 13;
         pCh->pTapiCh->TapiCidConfig.nHlev = (pCmd[2] & SIG_CID_HLEV) >> 12;
         pCh->pTapiCh->TapiCidConfig.nSpec = (pCmd[2] & SIG_CID_V23) >> 11;
      }
#endif /* TAPI_CID */
   }
   /* Set Cid Receiver Defaults ******************************************* */
   /* P.S: Configuration is read from Chip, and just the ressource numbers are
      programmed. The default coefficients are used for the cid receiver.  */
   if (err == IFX_SUCCESS)
   {
      /* set length & cmd 2 */
      pCmd[1] = ECMD_CIDRX;
      /* CIDRNR = ch . Other parameters as is. */
      pCmd[2] = ch;
      err = CmdWrite (pDev, pCmd, 1);
   }
   /* Set DTMF Generator Defaults ******************************************* */
   if (err == IFX_SUCCESS)
   {
      /* EN = 1, AD = 1, MOD = 0, FGMOD = 1 */
      /* Note : when ET = 1 : MOD = 0 , FG = 1 (ref FW Spec). If done another
         way, it leads to CERR in FW. */
      pDev->pSigCh[ch].sig_dtmfgen.value[CMD_HEADER_CNT] = (0xCA00 | ch);
      pDev->pSigCh[ch].sig_dtmfgen.bit.addb = 1;
#ifdef FW_ETU
      pDev->pSigCh[ch].sig_dtmfgen.bit.et = 0;
#else
      pDev->pSigCh[ch].sig_dtmfgen.bit.et = 1;
#endif /* FW_ETU */
      err = CmdWrite (pDev, pDev->pSigCh[ch].sig_dtmfgen.value,
                      CMD_SIG_DTMFGEN_LEN);
#ifdef TAPI_CID
      if (err == IFX_SUCCESS)
      {
         err = TAPI_LL_Phone_CID_ConfDtmf (pCh->pTapiCh,
                                           &pCh->pTapiCh->TapiDtmfCidConf);
      }
#endif /* TAPI_CID */
   }
   /* Set DTMF Receiver Defaults ******************************************** */
   if (err == IFX_SUCCESS)
   {
      /* set length & cmd 2 */
      pCmd[1] = ECMD_DTMF_REC;
      /* EN = 1, ET = 1, AS = 1 */
      pCmd[2] = (0xC010 | ch);
      err = CmdWrite (pDev, pCmd, 1);
   }
   if (err == IFX_SUCCESS)
   {
      /* Set ATD1 Defaults for Answering Tone Detection (CED) Set ATD2 for CED
         network tone detection Default ATD coefficients can be used in this
         case */
      /* set resource information -> will never be changed */
      pDev->pSigCh[ch].sig_atd1.value[CMD_HEADER_CNT] = 0;
      pDev->pSigCh[ch].sig_atd1.bit.resnr = ch;
      pDev->pSigCh[ch].sig_atd2.value[CMD_HEADER_CNT] = 0;
      pDev->pSigCh[ch].sig_atd2.bit.resnr = ch + pDev->nSigCnt;
      /* These settings are for compatibilty reasons and will be overidden when
         IFXPHONE_ENABLE_SIGDETECT is called */
      err = Dsp_AtdConf (pCh, TAPI_SIGNAL_CEDTX | TAPI_SIGNAL_CEDRX |
                              TAPI_SIGNAL_PHASEREV | TAPI_SIGNAL_AM);
   }
   if (err == IFX_SUCCESS)
   {
      pDev->pSigCh[ch].sig_utd1.value[CMD_HEADER_CNT] = 0;
      pDev->pSigCh[ch].sig_utd2.value[CMD_HEADER_CNT] = 0;
      /* set UTD1/2 coefficients for Calling Tone Detection, 1100 Hz */
      /* UTD1 : Ressources 0 - 3 for Tone detection from SigInA (here ALI for
         Detection of Tones from ALM) UTD2 : Ressources 4 - 7 for Tone
         detection from SigInB (here Coder for Network Tone Detection) */
      /* select universal tone detection and set up resources -> will never be
         changed */
      pDev->pSigCh[ch].sig_utd1.bit.md = VIN_ECMD_SIGUTD_MD_UNV;
      pDev->pSigCh[ch].sig_utd2.bit.md = VIN_ECMD_SIGUTD_MD_UNV;
      pDev->pSigCh[ch].sig_utd1.bit.resnr = ch;
      pDev->pSigCh[ch].sig_utd2.bit.resnr = ch + pDev->nSigCnt;
      /* UTD1 is used for detection of tones comming from ALI UTD2 is used to
         detect tones comming for Coder (Network) */
      /* These settings are for compatibilty reasons and will be overidden when
         IFXPHONE_ENABLE_SIGDETECT is called */
      err = Dsp_UtdConf (pCh, TAPI_SIGNAL_CNGFAXTX | TAPI_SIGNAL_CNGFAXRX);
   }
   /* set UTGs without enabling them */
   if (err == IFX_SUCCESS)
   {
      /* set length & cmd 2 */
      pCmd[1] = ECMD_UTG;
      /* set channel */
      pCmd[0] = CMD1_EOP | ch;
      /* EN = 0, A1 = 0b10, A2 = 0b10, UTGNR = ch */
      pCmd[2] = 0x00A0 | ch;
      err = CmdWrite (pDev, pCmd, 1);
   }
   if (err != IFX_SUCCESS)
   {
      TRACE (VINETIC, DBG_LEVEL_HIGH,
             ("SIGNALING configuration failed." " Dev Err = %ld\n\r",
              pDev->err));
   }

   return err;
}

/**
  Set the CID coefficients, different from the default ones!
\param
   pCh        - pointer to VINETIC channel structure
\param
   cidType    - cid type as specified in VINETIC_CID_TYPE enumeration
\return
   IFX_SUCCESS or Error Code
Remark :
   1- The CID sender must be disabled before programming the coefficients, what
      is done in this function
   2- According to CID type (onhook=Type1, Offhook=Type2), different
      Mark/Seizure configuration apply according to specifications:
      GR-30-CORE : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark = 80 bits
      ETSI       : Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180+/-25 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark =  80+/-25bits
      SIN227     : Type 1 (Onhook)  => Seizure = 96 to 315 bits, Mark >= 55 bits
                   Type 2 (Offhook) => Seizure = 0 bits, Mark >= 55 bits

      To meet all these specifications, following is set as default:
         Type 1 (Onhook)  => Seizure = 300 bits, Mark = 180 bits
         Type 2 (Offhook) => Seizure =   0 bits, Mark = 80 bits
   3- Here are some dB levels for reference :
      (formula: level=32768*10^(dBval / 20)
      -5dB = 0x47FA, -10 dB = 0x287A, - 15 dB = 0x16C3, -20dB = 0xCCC ,
      -25 dB = 0x732, -30 dB = 0x40C , -35 dB = 0x246, -40 dB = 0x147
*/
int SIGNL_SetCIDCoeff (VINETIC_CHANNEL * pCh, VINETIC_CID_TYPE cidType)
{
   IFX_int32_t     err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t pCmd[2] = { 0 }, pCidSend[3] =
   {
   0}, pCidCoefs[6] =
   {
   0}, dconf = 0, seizure, mark;

   /* disable CID sender in enable */
   pCmd[0] = (CMD1_EOP | (pCh->nChannel - 1));
   pCmd[1] = ECMD_CID_SEND;
   err = CmdRead (pDev, pCmd, pCidSend, 1);
   if ((err == IFX_SUCCESS) && (pCidSend[2] & SIG_CID_EN))
   {
      /* store the default configuration */
      dconf = pCidSend[2];
      /* set EN = 0 */
      pCidSend[2] &= ~SIG_CID_EN;
      err = CmdWrite (pDev, pCidSend, 1);
   }
   if (err == IFX_SUCCESS)
   {
      /* set the new coefficients */
      pCidCoefs[0] = (CMD1_EOP |  (pCh->nChannel - 1));
      pCidCoefs[1] = ECMD_CID_COEF;
      /* level ~= -5 dB */
      pCidCoefs[2] = 0x47FA;
      /* set seizure and mark according to CID type */
      switch (cidType)
      {
      /* offhook CID, called CID type 2, NTT inclusive */
      case  CID_TYPE_2:
         seizure = 0;
         mark    = 80;
         break;
      /* onhook CID called CID type 1, NTT inclusive */
      default:
         seizure = 300;
         mark    = 180;
         break;
      }
      /* set Seizure */
      pCidCoefs[3] = seizure;
      /* set Mark */
      pCidCoefs[4] = mark;
      /* BRS = 8 */
      pCidCoefs[5] = 0x0008;
      err = CmdWrite (pDev, pCidCoefs, 4);
   }
   /* restore CID sender configuration if applicable */
   if ((err == IFX_SUCCESS) && (dconf != 0))
   {
      pCidSend[2] = dconf;
      err = CmdWrite (pDev, pCidSend, 1);
   }
   if (err != IFX_SUCCESS)
      TRACE (VINETIC, DBG_LEVEL_HIGH, ("CID Coefficients setting failed\n\r"));

   return err;
}
#endif /* (VIN_CFG_FEATURES & (VIN_FEAT_VIN_C | VIN_FEAT_VIN_M)) */

