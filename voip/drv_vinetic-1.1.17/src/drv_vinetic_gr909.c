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
   Module      : drv_vinetic_gr909.c
   Description : This file contains the definition of the globals and local
                 defines and functions needed for Vinetic GR909 realtime
                 measurements.
   Remarks     : Driver should be compiled with -DTAPI_GR909 to support
                 this measurement. Feature is tested under Linux and VxWorks.
   Note        : Because floating points are not allowed in driver modules
                 under some OS, float values will be rounded to decimal values
                 in the whole file.
********************************************************************************/


/* ============================= */
/* includes                      */
/* ============================= */
#include "drv_vinetic_api.h"

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */

/* Defaults limit values for GR909 line testing */
#define HPT_W2G_AC_LIM_DEFAULT         0x2551 /* 50 Vrms */
#define HPT_W2W_AC_LIM_DEFAULT         0x2548 /* 50 Vrms */
#define HPT_W2G_DC_LIM_DEFAULT         0x46DF /* 135 V */
#define HPT_W2W_DC_LIM_DEFAULT         0x472D /* 135 V */
#define FEMF_W2G_AC_LIM_DEFAULT        0x0777 /* 10 Vrms */
#define FEMF_W2W_AC_LIM_DEFAULT        0x0775 /* 10 Vrms */
#define FEMF_W2G_DC_LIM_DEFAULT        0x02CC /* 6 V */
#define FEMF_W2W_DC_LIM_DEFAULT        0x032A /* 6 V */
#define RFT_RES_LIM_DEFAULT            0x217D /* 150 KOhm */
#define ROH_LIN_LIM_DEFAULT            0x000F /* 15% */
#define RIT_LOW_LIM_DEFAULT            0x0142 /* 1400 Ohm, set for 20 Hz */
#define RIT_HIGH_LIM_DEFAULT           0x23FC /* 40005 Ohm, set for 20 Hz */
#define RIT_RING_FREQ                  20     /* 20 Hz */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local structures              */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */


/* ============================= */
/* Local function definition     */
/* ============================= */


/* ============================= */
/* Global function definition    */
/* ============================= */

/**
   Starts GR909 tests according to test mask given
\param
   pCh     - pointer to the channel structure
\param
   p_start - pointer to VINETIC_IO_GR909_Start_t structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This function sets default threshold values in case they weren't set before.
*/
IFX_int32_t VINETIC_GR909_Start (VINETIC_CHANNEL *pCh,
                                 VINETIC_IO_GR909_Start_t *p_start)
{
   IFX_int32_t                      ret       = IFX_SUCCESS;
   IFX_uint8_t                      ch        = (pCh->nChannel - 1);
   VINETIC_DEVICE                  *pDev      = pCh->pParent;
   CMD_OpMode_t                    *p_opmod   = &pCh->hostCh.opmod;
   CMD_Ring_Config_t               *p_ringcfg = &pCh->hostCh.ring_cfg;
   CMD_GR909_Linetesting_Control_t *p_ctrl    = &pCh->hostCh.gr909_ctrl;

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);

   /* Nothing to test ? */
   if (p_start->test_mask == 0)
      goto error;
   /* adapt ring freqeuncy if necessary */
   if (p_start->test_mask & VINETIC_IO_GR909_RIT)
   {
      /* ring configuration must heve been done before, so check */
      if (pCh->hostCh.b_ring_cfg == IFX_FALSE)
         goto error;
      /* save previous ring frequency */
      pCh->hostCh.ring_freq_prev = p_ringcfg->RING_F;
      /* setup ringing frequency used for RIH */
      p_ringcfg->RW     = VIN_CMD_WR;
      p_ringcfg->RING_F = (RIT_RING_FREQ * 32768) / 4000;
      ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_ringcfg),
                      p_ringcfg->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
   }
   /* set line in power down mode */
   p_opmod->RW      = VIN_CMD_WR;
   p_opmod->OP_MODE = VIN_OPMOD_PDH;
   ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_opmod),
                   p_opmod->LENGTH);
   if (ret == IFX_ERROR)
      goto error;
   /* setup gr909 line testing control */
   p_ctrl->RW      = VIN_CMD_WR;
   p_ctrl->COUNTRY = p_start->pl_freq;
   p_ctrl->HPT     = ((p_start->test_mask & VINETIC_IO_GR909_HPT) ? 1 : 0);
   p_ctrl->FEMF    = ((p_start->test_mask & VINETIC_IO_GR909_FEMF)? 1 : 0);
   p_ctrl->RFT     = ((p_start->test_mask & VINETIC_IO_GR909_RFT) ? 1 : 0);
   p_ctrl->ROH     = ((p_start->test_mask & VINETIC_IO_GR909_ROH) ? 1 : 0);
   p_ctrl->RIT     = ((p_start->test_mask & VINETIC_IO_GR909_RIT) ? 1 : 0);
   /** \todo Workaround for RFT and ROH measurements : If one of these measurements
      is started, both bits should be set because the DC firmware can't handle
      the ROH measurement separately at the moment */
   if ((p_ctrl->RFT == 1) || (p_ctrl->ROH == 1))
   {
      p_ctrl->RFT = 1;
      p_ctrl->ROH = 1;
   }
   /* always write at least one command data */
   p_ctrl->LENGTH  = 1;
   if (pCh->hostCh.b_GR909_limits == IFX_FALSE)
   {
      p_ctrl->HPT_W2G_AC_LIM  = HPT_W2G_AC_LIM_DEFAULT;
      p_ctrl->HPT_W2W_AC_LIM  = HPT_W2W_AC_LIM_DEFAULT;
      p_ctrl->HPT_W2G_DC_LIM  = HPT_W2G_DC_LIM_DEFAULT;
      p_ctrl->HPT_W2W_DC_LIM  = HPT_W2W_DC_LIM_DEFAULT;
      p_ctrl->FEMF_W2G_AC_LIM = FEMF_W2G_AC_LIM_DEFAULT;
      p_ctrl->FEMF_W2W_AC_LIM = FEMF_W2W_AC_LIM_DEFAULT;
      p_ctrl->FEMF_W2G_DC_LIM = FEMF_W2G_DC_LIM_DEFAULT;
      p_ctrl->FEMF_W2W_DC_LIM = FEMF_W2W_DC_LIM_DEFAULT;
      p_ctrl->RFT_RES_LIM     = RFT_RES_LIM_DEFAULT;
      p_ctrl->ROH_LIN_LIM     = ROH_LIN_LIM_DEFAULT;
      p_ctrl->RIT_LOW_LIM     = RIT_LOW_LIM_DEFAULT;
      p_ctrl->RIT_HIGH_LIM    = RIT_HIGH_LIM_DEFAULT;
      /* adapt length */
      p_ctrl->LENGTH          = CMD_GR909_LINETESTING_CONTROL_LEN;
      /* don't program defaults on the next go */
      pCh->hostCh.b_GR909_limits = IFX_TRUE;
   }
   ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_ctrl),
                   p_ctrl->LENGTH);
   if (ret == IFX_ERROR)
      goto error;
   /* unmask interrupts */
   /* - enable rising edge interrupt */
   REG_WRITE_PROT(pDev, (V2CPE_LINE1_INTR + (ch << 1)),
                  V2CPE_LINE1_INT_LTEST_FIN_SET (pCh->hostCh.regLineX_IntR, 1));
   CHECK_HOST_ERR(pDev, goto error);
   /* - disable falling edge interrupt */
   REG_WRITE_PROT(pDev, (V2CPE_LINE1_INTF + (ch << 1)),
                  V2CPE_LINE1_INT_LTEST_FIN_SET (pCh->hostCh.regLineX_IntF, 0));
   CHECK_HOST_ERR(pDev, goto error);
   /* setup GR909 mode now */
   p_opmod->RW      = VIN_CMD_WR;
   p_opmod->OP_MODE = VIN_OPMOD_GR909;
   ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_opmod),
                   p_opmod->LENGTH);

error:
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   return ret;
}

/**
   Reads current GR909 results that are available.
\param
   pCh       - pointer to the channel structure
\param
   p_results - pointer to the VINETIC_IO_GR909_Result_t structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   After reading of results, PDH line mode is set and LINEX_INT.LTEST_FIN is
   masked
*/
IFX_int32_t VINETIC_GR909_Result (VINETIC_CHANNEL *pCh,
                                  VINETIC_IO_GR909_Result_t *p_results)
{
   IFX_int32_t                   ret       = IFX_SUCCESS;
   IFX_uint8_t                   ch        = (pCh->nChannel - 1);
   VINETIC_DEVICE               *pDev      = pCh->pParent;
   CMD_OpMode_t                 *p_opmod   = &pCh->hostCh.opmod;
   CMD_Ring_Config_t            *p_ringcfg = &pCh->hostCh.ring_cfg;
   CMD_GR909_Result_Pass_Fail_t *p_res     = &pCh->hostCh.gr909_result;

   /* protect channel from mutual access */
   IFXOS_MutexLock (pCh->chAcc);

   /* zero whole result parameter */
   memset (p_results, 0, sizeof (VINETIC_IO_GR909_Result_t));

   /* read GR909 results */
   ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_res),
                  (IFX_uint16_t *)((IFX_void_t *) p_res), p_res->LENGTH);
   if (ret == IFX_ERROR)
      goto error;

   /* check validity for HPT */
   if (p_res->HPT_VALID == 1)
   {
      CMD_GR909_Result_HPT_t *p_hpt  = &pCh->hostCh.gr909_hpt;

      p_results->valid |= VINETIC_IO_GR909_HPT;
      /* set passed flag accordingly */
      if (p_res->HPT_PASS == 1)
         p_results->passed |= VINETIC_IO_GR909_HPT;
      /* read results now */
      ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_hpt),
                     (IFX_uint16_t *)((IFX_void_t *) p_hpt), p_hpt->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
      /* set results for user */
      p_results->HPT_AC_R2G = (IFX_int16_t)p_hpt->HPT_AC_R2G;
      p_results->HPT_AC_T2G = (IFX_int16_t)p_hpt->HPT_AC_T2G;
      p_results->HPT_AC_T2R = (IFX_int16_t)p_hpt->HPT_AC_T2R;
      p_results->HPT_DC_R2G = (IFX_int16_t)p_hpt->HPT_DC_R2G;
      p_results->HPT_DC_T2G = (IFX_int16_t)p_hpt->HPT_DC_T2G;
      p_results->HPT_DC_T2R = (IFX_int16_t)p_hpt->HPT_DC_T2R;
   }
   /* check validity for FEMF */
   if (p_res->FEMF_VALID == 1)
   {
      CMD_GR909_Result_FEMF_t *p_femf = &pCh->hostCh.gr909_femf;

      p_results->valid |= VINETIC_IO_GR909_FEMF;
      /* set passed flag accordingly */
      if (p_res->FEMF_PASS == 1)
         p_results->passed |= VINETIC_IO_GR909_FEMF;
      /* read results now */
      ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_femf),
                     (IFX_uint16_t *)((IFX_void_t *) p_femf), p_femf->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
      /* set results for user */
      p_results->FEMF_AC_R2G = (IFX_int16_t)p_femf->FEMF_AC_R2G;
      p_results->FEMF_AC_T2G = (IFX_int16_t)p_femf->FEMF_AC_T2G;
      p_results->FEMF_AC_T2R = (IFX_int16_t)p_femf->FEMF_AC_T2R;
      p_results->FEMF_DC_R2G = (IFX_int16_t)p_femf->FEMF_DC_R2G;
      p_results->FEMF_DC_T2G = (IFX_int16_t)p_femf->FEMF_DC_T2G;
      p_results->FEMF_DC_T2R = (IFX_int16_t)p_femf->FEMF_DC_T2R;
   }
   /* check validity for RFT */
   if (p_res->RFT_VALID == 1)
   {
      CMD_GR909_Result_RFT_t *p_rft  = &pCh->hostCh.gr909_rft;

      p_results->valid |= VINETIC_IO_GR909_RFT;
      /* set passed flag accordingly */
      if (p_res->RFT_PASS == 1)
         p_results->passed |= VINETIC_IO_GR909_RFT;
      /* read results now */
      ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_rft),
                     (IFX_uint16_t *)((IFX_void_t *) p_rft), p_rft->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
      /* set results for user */
      p_results->RFT_R2G = (IFX_int16_t)p_rft->RFT_R2G;
      p_results->RFT_T2G = (IFX_int16_t)p_rft->RFT_T2G;
      p_results->RFT_T2R = (IFX_int16_t)p_rft->RFT_T2R;
   }
   /* check validity for ROH */
   if (p_res->ROH_VALID == 1)
   {
      CMD_GR909_Result_ROH_t *p_roh  = &pCh->hostCh.gr909_roh;

      p_results->valid |= VINETIC_IO_GR909_ROH;
      /* set passed flag accordingly */
      if (p_res->ROH_PASS == 1)
         p_results->passed |= VINETIC_IO_GR909_ROH;
      /* read results now */
      ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_roh),
                     (IFX_uint16_t *)((IFX_void_t *) p_roh), p_roh->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
      /* set results for user */
      p_results->ROH_T2R_L = (IFX_int16_t)p_roh->ROH_T2R_L;
      p_results->ROH_T2R_H = (IFX_int16_t)p_roh->ROH_T2R_H;
   }
   /* check validity for RIT */
   if (p_res->RIT_VALID == 1)
   {
      CMD_GR909_Result_RIT_t *p_rit  = &pCh->hostCh.gr909_rit;

      p_results->valid |= VINETIC_IO_GR909_RIT;
      /* set passed flag accordingly */
      if (p_res->RIT_PASS == 1)
         p_results->passed |= VINETIC_IO_GR909_RIT;
      /* read results now */
      ret = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)p_rit),
                     (IFX_uint16_t *)((IFX_void_t *) p_rit), p_rit->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
      /* set results for user */
      p_results->RIT_RES = (IFX_int16_t)p_rit->RIT_RES;
      /* restore previous ring frequency */
      p_ringcfg->RW     = VIN_CMD_WR;
      p_ringcfg->RING_F = pCh->hostCh.ring_freq_prev;
      ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_ringcfg),
                      p_ringcfg->LENGTH);
      if (ret == IFX_ERROR)
         goto error;
   }

   /* set line in power down mode */
   p_opmod->RW      = VIN_CMD_WR;
   p_opmod->OP_MODE = VIN_OPMOD_PDH;
   ret = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)p_opmod),
                   p_opmod->LENGTH);
   if (ret == IFX_ERROR)
      goto error;
   /* mask interrupts : disable rising edge interrupt */
   REG_WRITE_PROT(pDev, (V2CPE_LINE1_INTR + (ch << 1)),
                  V2CPE_LINE1_INT_LTEST_FIN_SET (pCh->hostCh.regLineX_IntR, 0));
   CHECK_HOST_ERR(pDev, goto error);

error:
   /* release channel */
   IFXOS_MutexUnlock (pCh->chAcc);
   return ret;
}

#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */



