/*******************************************************************************
       Copyright (c) 2005, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
********************************************************************************
   Module      : drv_vinetic_host.c
   Description : This file implements the vinetic 2CPE host functions including
                 low level access according to the underlying access mode.
   Remarks     :

      - For SPI support, availability of following defines is required:

        1) SPI_MAXBYTES_SIZE
        2) SPI_CS_SET
        3) spi_ll_read_write

        Use the file drv_vinetic_config_user.h to set these defines
        for SPI
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_vinetic_api.h"
#include "drv_vinetic_host.h"
#include "drv_vinetic_main.h"
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_bbd.h"
#include "drv_vinetic_dcctl.h"
#include "drv_vinetic_dspconf.h"
#include "drv_vinetic_stream.h"

/* ============================= */
/* Local defines                 */
/* ============================= */

/* how often to try to read interrupt status register to
   be zero after a abort occurs */
#define VIN_2CPE_MAX_IR_CHECK                          10

/* AC DSP start addresses for ch 0 and ch 1 */
enum
{
   VIN_2CPE_ACDSP_STARTADDR_CH0 =  0x1000,
   VIN_2CPE_ACDSP_STARTADDR_CH1 =  0x2000
};

/* 2CPE user download flags */
enum
{
   /* user has set BBD ptr in init structure */
   V2CPE_IO_BBD  = 0x01,
   /* user has set PRAM fw ptr in init structure */
   V2CPE_IO_PRAM = 0x02,
   /* user has set DRAM fw ptr in init structure */
   V2CPE_IO_DRAM = 0x04,
   /* user has set AC ptr in init structure */
   V2CPE_IO_AC   = 0x08,
   /* user has set CRAM ptr in init structure */
   V2CPE_IO_CRAM = 0x10
};

/* ============================= */
/* Local variable definition     */
/* ============================= */

const IFX_uint16_t pAcStartAddr [] =
{
   VIN_2CPE_ACDSP_STARTADDR_CH0,
   VIN_2CPE_ACDSP_STARTADDR_CH1
};

/* ============================= */
/* Local structures              */
/* ============================= */

/* Vinetic 2CPE download structure */
typedef struct
{
   /* user download flag
      - V2CPE_IO_BBD
      - V2CPE_IO_PRAM
      - V2CPE_IO_DRAM
      - V2CPE_IO_AC
      - V2CPE_IO_CRAM
   */
   IFX_uint8_t         nIODwld;
   /* BBD data format, with blocks to download */
   bbd_format_t         bbdDwld;
   /* EDSP firmware download */
   VINETIC_EDSP_FWDWLD  edspDwld;
   /* AC download in BBD format */
   bbd_format_t         acDwld;
   /* CRAM download in BBD format */
   bbd_format_t         cramDwld;
   /* Ring Config download in BBD format */
   bbd_format_t         ringCfgDwld;
   /* DC Threshold download in BBD format */
   bbd_format_t         dcThrDwld;
} V2CPE_DOWNLOAD;

/* ============================= */
/* Local function declaration    */
/* ============================= */

IFX_LOCAL IFX_void_t Host_SetDwldPtr   (VINETIC_IO_INIT *pUsr,
                                        V2CPE_DOWNLOAD *pDwld);
IFX_LOCAL IFX_void_t Host_UnsetDwldPtr (V2CPE_DOWNLOAD *pDwld);

/* functions used at device initialization
   Note: Download functions can also be used for a broadcast
   download to all channels */
IFX_LOCAL IFX_int32_t Host_ClearPendingInt (VINETIC_DEVICE *pDev);
IFX_LOCAL IFX_int32_t Host_DownloadAC      (VINETIC_DEVICE *pDev,
                                            bbd_format_t   *bbd);
IFX_LOCAL IFX_int32_t Host_DownloadCram    (VINETIC_DEVICE *pDev,
                                            bbd_format_t   *bbd);
IFX_LOCAL IFX_int32_t Host_SetSlic         (VINETIC_DEVICE *pDev,
                                            bbd_format_t   *bbd);
IFX_LOCAL IFX_int32_t Host_SetRingCfg      (VINETIC_DEVICE *pDev,
                                            bbd_format_t   *bbd);
IFX_LOCAL IFX_int32_t Host_SetDcThr        (VINETIC_DEVICE *pDev,
                                            bbd_format_t   *bbd);

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/**
  Sets the download data pointers accordingly
\param
   pInit  - handle to user download data in VINETIC_IO_INIT
   pDwld  - handle to 2CPE relevant download data in V2CPE_DOWNLOAD
\return
   none
\remarks
   This function reserves virtual memory for download data and copies data
   from user space. It also parses the BBD buffer looking for specific download
   data (CRAM, ACDSP, RINGCFG, DCTRESHOLD)
*/
IFX_LOCAL IFX_void_t Host_SetDwldPtr   (VINETIC_IO_INIT *pInit,
                                        V2CPE_DOWNLOAD *pDwld)
{
   bbd_error_t err;

   /* do some initializations ... */
   memset (pDwld, 0, sizeof (V2CPE_DOWNLOAD));
   pDwld->bbdDwld.buf = NULL;
   /* bbd buffer has prio 1, so get memory for it at first */
   if (pInit->pBBDbuf != NULL)
   {
      pDwld->bbdDwld.buf   = OS_MapBuffer(pInit->pBBDbuf, pInit->bbd_size);
      if (pDwld->bbdDwld.buf != NULL)
      {
         pDwld->bbdDwld.size  = pInit->bbd_size;
         /* check bbd integrity now */
         err = bbd_check_integrity (&pDwld->bbdDwld, BBD_VIN_MAGIC);
         if (err != BBD_INTG_OK)
         {
            /* release memory allocated for BBD */
            OS_UnmapBuffer(pDwld->bbdDwld.buf);
            pDwld->bbdDwld.buf  = NULL;
            pDwld->bbdDwld.size = 0;
         }
         else
         {
            /* well, we got a valid BBD download data from user */
            pDwld->nIODwld |= V2CPE_IO_BBD;
         }
      }
   }
   if (pInit->pPRAMfw != NULL)
   {
      pDwld->edspDwld.pPRAMfw   = OS_MapBuffer(pInit->pPRAMfw, pInit->pram_size);
      if (pDwld->edspDwld.pPRAMfw != NULL)
      {
         pDwld->edspDwld.pram_size = pInit->pram_size;
         pDwld->nIODwld           |= V2CPE_IO_PRAM;
      }
   }
   if (pInit->pDRAMfw != NULL)
   {
      pDwld->edspDwld.pDRAMfw   = OS_MapBuffer(pInit->pDRAMfw, pInit->dram_size);
      if (pDwld->edspDwld.pDRAMfw != NULL)
      {
         pDwld->edspDwld.dram_size = pInit->dram_size;
         pDwld->nIODwld           |= V2CPE_IO_DRAM;
      }
   }
   if (pInit->pCram != NULL)
   {
      pDwld->cramDwld.buf   = OS_MapBuffer(pInit->pCram, pInit->cram_size);
      if (pDwld->cramDwld.buf != NULL)
      {
         pDwld->cramDwld.size  = pInit->cram_size;
         pDwld->nIODwld       |= V2CPE_IO_CRAM;
      }
   }
   /* check if necessary to parse BBD buffer for EDSP PRAM */
   if ((pDwld->nIODwld & V2CPE_IO_PRAM) == 0)
   {
      VINETIC_BBD_SetEdspPtr (&pDwld->bbdDwld, &pDwld->edspDwld, P_RAM);
      if (pDwld->edspDwld.pPRAMfw != NULL)
         pDwld->nIODwld |= V2CPE_IO_PRAM;
   }
   /* check if necessary to parse BBD buffer for EDSP DRAM */
   if ((pDwld->nIODwld & V2CPE_IO_DRAM) == 0)
   {
      VINETIC_BBD_SetEdspPtr (&pDwld->bbdDwld, &pDwld->edspDwld, D_RAM);
      if (pDwld->edspDwld.pDRAMfw != NULL)
         pDwld->nIODwld |= V2CPE_IO_DRAM;
   }
   /* set edsp download user flags if applicable */
   if ((pDwld->nIODwld & (V2CPE_IO_PRAM | V2CPE_IO_DRAM)) ==
       (V2CPE_IO_PRAM | V2CPE_IO_DRAM))
   {
      pDwld->edspDwld.nEdspFlags =
         (pInit->nFlags & (NO_EDSP_START | FW_AUTODWLD | NO_FW_DWLD));
   }
   else
   {
      /* User didn't download pram or dram binaries. Activate rom firmware */
      pDwld->edspDwld.nEdspFlags = (FW_AUTODWLD | NO_FW_DWLD);
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("WARN:  No extern firmware binaries PRAM/DRAM => "
             "ROM firmware will be activated\n\r"));
   }
   /* check if necessary to parse BBD buffer for AC */
   if ((pDwld->nIODwld & V2CPE_IO_AC) == 0)
   {
      VINETIC_BBD_SetPtr (&pDwld->bbdDwld, &pDwld->acDwld, BBD_VIN_AC_BLOCK);
   }
   /* check if necessary to parse BBD buffer for CRAM */
   if ((pDwld->nIODwld & V2CPE_IO_CRAM) == 0)
   {
      VINETIC_BBD_SetPtr (&pDwld->bbdDwld, &pDwld->cramDwld, BBD_VIN_CRAM_BLOCK);
   }
   /* parse BBD buffer for Ring Configuration */
   VINETIC_BBD_SetPtr (&pDwld->bbdDwld, &pDwld->ringCfgDwld,
                        BBD_VINETIC_RING_CFG_BLOCK);
   /* parse BBD buffer for DC Threshold  Configuration */
   VINETIC_BBD_SetPtr (&pDwld->bbdDwld, &pDwld->dcThrDwld,
                        BBD_VINETIC_DC_THRESHOLDS_BLOCK);
}

/**
  Releases the download data pointers accordingly
\param
   pDwld  - handle to 2CPE relevant download data in V2CPE_DOWNLOAD
\return
   none
\remarks
   This function frees virtual memory for download data.
*/
IFX_LOCAL IFX_void_t Host_UnsetDwldPtr (V2CPE_DOWNLOAD *pDwld)
{
//	printk("HUDP 1 %X\n",pDwld);
   if (pDwld->nIODwld & V2CPE_IO_BBD)
   {
      OS_UnmapBuffer(pDwld->bbdDwld.buf);
   }
//   printk("HUDP 2\n");
   if (pDwld->nIODwld & V2CPE_IO_PRAM)
   {
      OS_UnmapBuffer(pDwld->edspDwld.pPRAMfw);
   }
//	printk("HUDP 3\n");
   if (pDwld->nIODwld & V2CPE_IO_DRAM)
   {
      OS_UnmapBuffer(pDwld->edspDwld.pDRAMfw);
   }
//printk("HUDP 4\n");
   if (pDwld->nIODwld & V2CPE_IO_AC)
   {
      OS_UnmapBuffer(pDwld->acDwld.buf);
   }
//printk("HUDP 5\n");
   if (pDwld->nIODwld & V2CPE_IO_AC)
   {
      OS_UnmapBuffer(pDwld->acDwld.buf);
   }
//printk("HUDP 6\n");
   if (pDwld->nIODwld & V2CPE_IO_CRAM)
   {
      OS_UnmapBuffer(pDwld->cramDwld.buf);
   }
//printk("HUDP 7\n");
   memset (pDwld, 0, sizeof (V2CPE_DOWNLOAD));
//printk("HUDP 8\n");
}

/**
  Clears all pending interrupts
\param
   pDev  - handle to the device
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   This function clears pending interrupts after a reset operation.
   It must be save from interrupts.
   it returns an error code if the ISR != 0 after loop count.
*/
IFX_LOCAL IFX_int32_t Host_ClearPendingInt(VINETIC_DEVICE *pDev)
{
   IFX_uint16_t nIr = 0, i = 0;
   IFX_int32_t err = IFX_SUCCESS;

   /*
      \todo read interrupt register
   */
   /* check if interrupts are pending */
   for (i = 0; i <= VIN_2CPE_MAX_IR_CHECK; i ++)
   {
      if (nIr != 0)
      {
         /* clear hardware interrupts if applicable */
         if ((err == IFX_SUCCESS) /*&& (nIr & )*/)
         {
            /*
               \todo Read Hardware Status Register
            */
         }
         /* clear mbx interrupts if applicable  */
         if ((err == IFX_SUCCESS) /*&& (nIr & )*/)
         {
            /*
               \todo Read Mailbox Status Registers
            */
         }
         /* clear gpio interrupts if applicable */
         if ((err == IFX_SUCCESS) /*&& (nIr & )*/)
         {
            /*
               \todo Read GPIO Status Registers
            */
         }
         /* clear reset interrupt */
         if ((err == IFX_SUCCESS) /* && (nIr & )*/)
         {
            /*
               \todo Read Reset Status Registers
            */
         }
         /*
            \todo read interrupt register and check
                again until loop count is reached.
         */
      }
      if (nIr == 0)
         break;
   }
   if ((err == IFX_SUCCESS) && (nIr != 0))
   {
      SET_ERROR (ERR_INTSTUCK);
      err = IFX_ERROR;
   }
   /* check status */
   if (err != IFX_SUCCESS)
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: While clearing interrupts, IR = 0x%04X\n\r", nIr));
   }

   return err;
}

/**
  Download AC Micro Programm on all analog channels
\param
   pDev  - pointer to the device interface
\param
   bbd   - handle to BBD buffer
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   - Function parses whole BBD buffer looking for AC blocks which are then
     downloaded channelwise when available. BBD data integrity is assumed.
*/
IFX_LOCAL IFX_int32_t Host_DownloadAC (VINETIC_DEVICE *pDev,
                                       bbd_format_t   *bbd)
{
   IFX_int32_t      err = IFX_SUCCESS;
   IFX_uint8_t      i;
   bbd_block_t      bbd_ac;
   VINETIC_CHANNEL *pChTmp = NULL;

   /* resets */
   memset (&bbd_ac, 0, sizeof (bbd_ac));

   /* set BBD AC block chracteristics */
   bbd_ac.identifier = BBD_VIN_MAGIC;
   bbd_ac.tag        = BBD_VIN_AC_BLOCK;
   bbd_ac.index      = 0;
   while (err == IFX_SUCCESS)
   {
      /* read AC block of actual index */
      bbd_get_block(bbd, &bbd_ac);
      /* go out if no buffer found */
      if ((bbd_ac.data == NULL) || (bbd_ac.size == 0))
         break;
      /* download the found AC block on all available channels */
      for (i = 0; i < pDev->nAnaChan; i++)
      {
         pChTmp = &pDev->pChannel [i];
         err = VINETIC_Host_BBD_DownloadChAC(pChTmp, &bbd_ac);
         if (err == IFX_ERROR)
            break;
      }
      /* increment block index */
      if (err == IFX_SUCCESS)
         bbd_ac.index ++;
   }

   return err;
}

/**
   Download CRAM Coefficients on all analog channels
\param
   pDev       - handle to device
\param
   bbd   - handle to BBD buffer
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   - Function parses whole BBD buffer looking for CRAM blocks which are then
     downloaded channelwise when available. BBD data integrity is assumed.
*/
IFX_LOCAL IFX_int32_t Host_DownloadCram (VINETIC_DEVICE *pDev,
                                         bbd_format_t   *bbd)
{
   IFX_int32_t      err = IFX_SUCCESS;
   IFX_uint8_t      i;
   bbd_block_t      bbd_cram;
   VINETIC_CHANNEL *pChTmp = NULL;

   /* resets */
   memset (&bbd_cram, 0, sizeof (bbd_cram));

   /* set BBD CRAM block chracteristics */
   bbd_cram.identifier = BBD_VIN_MAGIC;
   bbd_cram.tag        = BBD_VIN_CRAM_BLOCK;
   bbd_cram.index      = 0;
   while (err == IFX_SUCCESS)
   {
      /* read CRAM block of actual index */
      bbd_get_block(bbd, &bbd_cram);
      /* go out if no buffer found */
      if ((bbd_cram.data == NULL) || (bbd_cram.size == 0))
         break;
      /* download the found CRAM block on all available channels */
      for (i = 0; i < pDev->nAnaChan; i++)
      {
         pChTmp = &pDev->pChannel [i];
         err = VINETIC_Host_BBD_DownloadChCram (pChTmp, &bbd_cram);
         if (err == IFX_ERROR)
            break;
      }
      /* increment block index */
      if (err == IFX_SUCCESS)
         bbd_cram.index ++;
   }
   /* set SLIC in case CRAM download was ok */
   if (err == IFX_SUCCESS)
      err = Host_SetSlic (pDev, bbd);

   return err;
}

/**
   Sets SLIC value read from BBD buffer.
\param
   pDev  - handle to device
\param
   bbd   - handle to BBD buffer
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   - Function parses whole BBD buffer looking for SLIC blocks which are then
     set when available. BBD data integrity is assumed.
*/
IFX_LOCAL IFX_int32_t Host_SetSlic (VINETIC_DEVICE *pDev,
                                    bbd_format_t   *bbd)
{
   IFX_uint16_t slic_val;
   bbd_block_t bbd_slic;

   /* resets */
   memset (&bbd_slic, 0, sizeof (bbd_slic));

   /* set BBD Slic block chracteristics */
   bbd_slic.identifier = BBD_VIN_MAGIC;
   bbd_slic.tag        = BBD_VIN_SLIC_BLOCK;
   bbd_slic.index      = 0;

   /* get slic block */
   bbd_get_block(bbd, &bbd_slic);
   /* go out if no buffer found */
   if ((bbd_slic.data == NULL) || (bbd_slic.size == 0))
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: No Slic value in BBD Buffer\n\r"));
      return IFX_ERROR;
   }

   /* get slic value */
   cpb2w (&slic_val, bbd_slic.data, sizeof (IFX_uint16_t));

   /* set slic value on all channels : Broadcast */
   return VINETIC_Host_SetSlic (&pDev->pChannel [0], IFX_TRUE, slic_val);
}

/**
   Does Ringing Configuration with values read from BBD buffer.
\param
   pDev  - handle to device
\param
   bbd   - handle to BBD buffer
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   - Function parses whole BBD buffer looking for Ring Config blocks which are
     then set when available. BBD data integrity is assumed.
*/
IFX_LOCAL IFX_int32_t Host_SetRingCfg (VINETIC_DEVICE *pDev,
                                       bbd_format_t   *bbd)
{
   bbd_block_t       bbd_ring;
   VINETIC_RingCfg_t ringCfg;
   IFX_int32_t       err = IFX_SUCCESS;

   /* resets */
   memset (&bbd_ring, 0, sizeof (bbd_ring));
   memset (&ringCfg, 0, sizeof (ringCfg));

   /* set BBD ring config block chracteristics */
   bbd_ring.identifier = BBD_VIN_MAGIC;
   bbd_ring.tag        = BBD_VINETIC_RING_CFG_BLOCK;
   bbd_ring.index      = 0;

   /* get ring config block */
   bbd_get_block(bbd, &bbd_ring);
   /* go out if no buffer found */
   if ((bbd_ring.data == NULL) || (bbd_ring.size == 0))
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("WARN: No Ring Configuration values in BBD Buffer\n\r"));
      return IFX_SUCCESS;
   }
   /* fill ring config parameter structure */
   cpb2w (&ringCfg.ring_freq, &bbd_ring.data [0], sizeof (IFX_uint16_t));
   cpb2w (&ringCfg.ring_amp, &bbd_ring.data [2], sizeof (IFX_uint16_t));
   cpb2w (&ringCfg.ring_hook_level, &bbd_ring.data [4], sizeof (IFX_uint16_t));
   /* version 2 or greater of the bbd block also has a field for the fast
      ring trip type */
   if (bbd_ring.version >= 2)
   {
      IFX_uint16_t nVal;

      cpb2w (&nVal, &bbd_ring.data [6], sizeof (IFX_uint16_t));
      switch (nVal)
      {
         case VINETIC_RING_TRIP_TYPE_NORMAL:
         case VINETIC_RING_TRIP_TYPE_FAST:
            ringCfg.ring_trip_type = (VINETIC_RING_TRIP_TYPE_t)nVal;
            break;
         default:
            err = IFX_ERROR;
            break;
      }
   }
   else
   {
      /* for all other versions of this block, use the default
         fast ring trip setting (normal) */
      ringCfg.ring_trip_type = VINETIC_RING_TRIP_TYPE_NORMAL;
   }
   /* version 3 or greater of bbd block also has the field for ring trip dup
      time */
   if (bbd_ring.version >= 3)
   {
      cpb2w (&ringCfg.ring_trip_dup_time, &bbd_ring.data [8],
             sizeof (IFX_uint16_t));
   }
   else
   {
      /* for all other versions of this block, use the default
         ring trip dup time setting */
      ringCfg.ring_trip_dup_time = VIN_2CPE_DEFAULT_RING_TRIP_DUP_TIME;
   }
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_RingCfg (&pDev->pChannel [0], IFX_TRUE, &ringCfg);
   /* set ring config on all channels : Broadcast */
   return err;
}

/**
   Does Ringing Configuration with values read from BBD buffer.
\param
   pDev  - handle to device
\param
   bbd   - handle to BBD buffer
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   - Function parses whole BBD buffer looking for Ring Config blocks which are
     then set when available. BBD data integrity is assumed.
*/
IFX_LOCAL IFX_int32_t Host_SetDcThr (VINETIC_DEVICE *pDev,
                                     bbd_format_t   *bbd)
{
   bbd_block_t bbd_dcthr;
   VINETIC_DcThr_t dcThr;

   /* initializations */
   memset (&bbd_dcthr, 0, sizeof(bbd_dcthr));
   memset (&dcThr, 0, sizeof (dcThr));

   /* set BBD DC Threshold block chracteristics */
   bbd_dcthr.identifier = BBD_VIN_MAGIC;
   bbd_dcthr.tag        = BBD_VINETIC_DC_THRESHOLDS_BLOCK;
   bbd_dcthr.index      = 0;

   /* get BBD dc treshold block */
   bbd_get_block(bbd, &bbd_dcthr);
   /* go out if no buffer found */
   if ((bbd_dcthr.data == NULL) || (bbd_dcthr.size == 0))
   {
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("WARN: No DC Threshold values in BBD Buffer\n\r"));
      return IFX_SUCCESS;
   }
   /* fill dc threshold parameter structure */
   cpb2w (&dcThr.hook_dup_time, &bbd_dcthr.data [0], sizeof (IFX_uint16_t));
   cpb2w (&dcThr.onhook_time, &bbd_dcthr.data [2], sizeof (IFX_uint16_t));
   /* version 2 or greater of bbd block also has the field for
      overtemperature dup time */
   if (bbd_dcthr.version >= 2)
   {
      cpb2w (&dcThr.ovt_dup_time, &bbd_dcthr.data [4],
             sizeof (IFX_uint16_t));
   }
   else
   {
      /* for all other versions of this block, use the default
         overtemperature dup time */
      dcThr.ovt_dup_time = VIN_2CPE_DEFAULT_OVT_DUP_TIME;
   }

   /* set ring config on all channels : Broadcast */
   return VINETIC_Host_SetDcThr (&pDev->pChannel [0], IFX_TRUE, &dcThr);
}

/*
 * Host data Initialization functions
 */

/**
   Initalizes host specific device parameters.
\param
   pDev   - handle to the device structure
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t VINETIC_Host_InitDevMembers (VINETIC_DEVICE  *pDev)
{
   IFX_int32_t ret = IFX_SUCCESS;

   VINETIC_GpioInit ((IFX_int32_t) pDev);
   return ret;
}

/**
   Initalizes host specific channel parameters on device open.
\param
   pCh   - handle to the channel structure
\return
   IFX_SUCCESS
*/
IFX_int32_t VINETIC_Host_InitChMembers (VINETIC_CHANNEL *pCh)
{
   IFX_uint8_t ch = (pCh->nChannel - 1);

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
   /* No limits sets by default */
   pCh->hostCh.b_GR909_limits = IFX_FALSE;
   /* setup operating mode header */
   pCh->hostCh.opmod.CMD     = VIN_CMD_ALM;
   pCh->hostCh.opmod.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.opmod.ECMD    = VIN_ECMD_OPMOD;
   pCh->hostCh.opmod.CH      = ch;
   pCh->hostCh.opmod.LENGTH  = CMD_OPMODE_LEN;
   /* setup header for GR909 line testing control */
   pCh->hostCh.gr909_ctrl.CMD     = VIN_CMD_ALM;
   pCh->hostCh.gr909_ctrl.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_ctrl.ECMD    = CMD_GR909_LINETESTING_CONTROL;
   pCh->hostCh.gr909_ctrl.CH      = ch;
   /* setup header for GR909 Pass/Fail Results */
   pCh->hostCh.gr909_result.RW    = VIN_CMD_RD;
   pCh->hostCh.gr909_result.CMD   = VIN_CMD_ALM;
   pCh->hostCh.gr909_result.MOD   = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_result.ECMD  = CMD_GR909_RESULT_PASS_FAIL;
   pCh->hostCh.gr909_result.CH    = ch;
   pCh->hostCh.gr909_result.LENGTH  = CMD_GR909_RESULT_PASS_FAIL_LEN;
   /* setup header for GR909 HPT Results */
   pCh->hostCh.gr909_hpt.RW      = VIN_CMD_RD;
   pCh->hostCh.gr909_hpt.CMD     = VIN_CMD_ALM;
   pCh->hostCh.gr909_hpt.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_hpt.ECMD    = CMD_GR909_RESULT_HPT;
   pCh->hostCh.gr909_hpt.CH      = ch;
   pCh->hostCh.gr909_hpt.LENGTH  = CMD_GR909_RESULT_HPT_LEN;
   /* setup header for GR909 FEFM Results */
   pCh->hostCh.gr909_femf.RW     = VIN_CMD_RD;
   pCh->hostCh.gr909_femf.CMD    = VIN_CMD_ALM;
   pCh->hostCh.gr909_femf.MOD    = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_femf.ECMD   = CMD_GR909_RESULT_FEMF;
   pCh->hostCh.gr909_femf.CH     = ch;
   pCh->hostCh.gr909_femf.LENGTH = CMD_GR909_RESULT_FEMF_LEN;
   /* setup header for GR909 RFT Results */
   pCh->hostCh.gr909_rft.RW      = VIN_CMD_RD;
   pCh->hostCh.gr909_rft.CMD     = VIN_CMD_ALM;
   pCh->hostCh.gr909_rft.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_rft.ECMD    = CMD_GR909_RESULT_RFT;
   pCh->hostCh.gr909_rft.CH      = ch;
   pCh->hostCh.gr909_rft.LENGTH  = CMD_GR909_RESULT_RFT_LEN;
   /* setup header for GR909 ROH Results */
   pCh->hostCh.gr909_roh.RW      = VIN_CMD_RD;
   pCh->hostCh.gr909_roh.CMD     = VIN_CMD_ALM;
   pCh->hostCh.gr909_roh.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_roh.ECMD    = CMD_GR909_RESULT_ROH;
   pCh->hostCh.gr909_roh.CH      = ch;
   pCh->hostCh.gr909_roh.LENGTH  = CMD_GR909_RESULT_ROH_LEN;
   /* setup header for GR909 RIT Results */
   pCh->hostCh.gr909_rit.RW      = VIN_CMD_RD;
   pCh->hostCh.gr909_rit.CMD     = VIN_CMD_ALM;
   pCh->hostCh.gr909_rit.MOD     = VIN_MOD_DCCTL;
   pCh->hostCh.gr909_rit.ECMD    = CMD_GR909_RESULT_RIT;
   pCh->hostCh.gr909_rit.CH      = ch;
   pCh->hostCh.gr909_rit.LENGTH  = CMD_GR909_RESULT_RIT_LEN;
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */

   TRACE (VINETIC, DBG_LEVEL_LOW, ("Host Chan Members Init, ch %d\r\n", ch));
   return IFX_SUCCESS;
}

/**
   Initalize host specific device Firmware Parameters
\param
   pDev   - handle to the device structure
\return
   IFX_SUCCESS
\remarks
   It is assumed that the used firmware supports as much
   signalling, pcm and coder channels as set in this function.
*/
IFX_int32_t VINETIC_Host_InitDevFWData (VINETIC_DEVICE *pDev)
{
   IFX_uint8_t tmp;

   /* coder channels */
   pDev->nCoderCnt = VIN_MAX_CODER;
   if ((pDev->nAnaChan < VINETIC_2CPE_ANA_CH_NR) && (pDev->nAnaChan > 0))
   {
      /* For 1CPE chips, the number of coders is the half of the
         number of coders in 2CPE chips */
      pDev->nCoderCnt >>= 1;
   }
  // printk("VINETIC_Host_InitDevFWData 1\n");
   /* signalling channels */
   if (pDev->hostDev.bRomFirmware == IFX_TRUE)
   {
      /* rom firmware supports only 3 signalling channels, so in case of
         auto download, set number of signalling channels accordingly */
      /** \todo remove this setting as soon as the rom firmware will be
           updated to support also 4 signalling channels. */
      pDev->nSigCnt   = VIN_MAX_SIG_ROM;
   }
   else
   {
      /* number of signalling channels supported in Firmware */
      pDev->nSigCnt   = VIN_MAX_SIG;
   }

 //  printk("VINETIC_Host_InitDevFWData 2\n");
   /* pcm channels */
   pDev->nPcmCnt   = VIN_MAX_PCM;
   /* Mas resources number,
      is the maximum between nSigCnt, nPcmCnt and nCoderCnt */
   tmp = max (pDev->nCoderCnt, pDev->nSigCnt);
   pDev->nMaxRes = max(pDev->nPcmCnt,  tmp);
   /* update capabilities according to used FW */
   if ((pDev->hostDev.nEdspVers & 0x0F00 ) == 0x0200)
   {
      pDev->VinCapabl.bWlec     = IFX_TRUE;
      pDev->VinCapabl.nUtgPerCh = 2;
      pDev->VinCapabl.nLecRes   = 2;
      /* two (UTG) tone genaration resources */
      pDev->nToneGenCnt = 2;
   }
   else
   {
      pDev->VinCapabl.bWlec     = IFX_FALSE;
      pDev->VinCapabl.nUtgPerCh = 1;
      pDev->VinCapabl.nLecRes   = 3;
      /* one (UTG) tone genaration resource */
      pDev->nToneGenCnt = 1;
   }

   return IFX_SUCCESS;
}

/**
  Reset host specific device members.
\param
   pDev   - pointer to the device structure
\return
   IFX_SUCCESS
\remarks
   Called by the generic device member reset function.

\todo Once sure that this function isn't usefull for 2CPE, it will
      be turned into an empty macro. Otherwise this note will be removed
*/
IFX_void_t  VINETIC_Host_ResetDevMembers (VINETIC_DEVICE  *pDev)
{
#if (VIN_CFG_FEATURES & VIN_FEAT_PACKET)
   VINETIC_CHANNEL  *pCh;
   IFX_int8_t i;

   /* initialize flags for channel */
   for (i = 0; i < VINETIC_2CPE_CH_NR; i++)
   {
      pCh = &pDev->pChannel[i];
      pCh->if_read  = VoIP_UpStream;
      pCh->if_write = VoIP_DownStream;
   }
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_PACKET) */
   VINETIC_GpioInit ((IFX_int32_t) pDev);
   /* reset rom firmware flag */
   pDev->hostDev.bRomFirmware = IFX_FALSE;

   return;
}

/**
   Stores chip access programmed via basic device initialization.
\param
   pDev     - handle to device structure
\param
   nAccess  - access mode as specified in VIN_ACCESS
\return
   IFX_SUCCESS
*/
IFX_int32_t VINETIC_Host_SetupAccess (VINETIC_DEVICE *pDev, VIN_ACCESS nAccess)
{
   /* store access mode in device structure */
   pDev->hostDev.nAccessMode = nAccess;

   return IFX_SUCCESS;
}

/*
 * Host operational functions
 */

/**
  Default initialization of 2CPE Registers.
\param
   pDev   - pointer to the device structure
\return
   Error code
\remarks
   Initializes the needed interrupt masks and default register values
*/
IFX_int32_t VINETIC_Host_DefaultRegInit(VINETIC_DEVICE *pDev)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint8_t i, offset;
   IFX_uint16_t nRegMaskR, nRegMaskF;
   IFX_uint16_t nRegMasksR[2], nRegMasksF[2];

   /* setup bitmask to enable the following HW/FW error (ERR_INT)
      interrupt sources */
   nRegMaskR =
      V2CPE_ERR_IEN_HW_ERR    |     /* EDSP hardware error */
      V2CPE_ERR_IEN_WD_FAIL   |     /* EDSP watchdog timeout */
      V2CPE_ERR_IEN_MCLK_FAIL |     /* MCLK failure */
      V2CPE_ERR_IEN_SYNC_FAIL |     /* PLL clock out of sync */
      V2CPE_ERR_IEN_EDSP_MIPS_OL |  /* EDSP MIPS overload */
      V2CPE_ERR_IEN_PCMB_CRASH|     /* PCM transmit highway B crash */
      V2CPE_ERR_IEN_PCMA_CRASH|     /* PCM transmit highway A crash */
      V2CPE_ERR_IEN_CMD_ERR   |     /* EDSP command error */
      V2CPE_ERR_IEN_VI_OV     |     /* voice inbox overflow */
      V2CPE_ERR_IEN_CI_OV     |     /* commmand inbox overflow */
      V2CPE_ERR_IEN_VO_UV     |     /* voice outbox underflow */
      V2CPE_ERR_IEN_CO_UV;          /* command outbox underflow */

   /* write bitmask to enable HW/FW (ERR_INT) error
      interrupt sources */
   REG_WRITE_PROT(pDev, V2CPE_ERR_IEN, nRegMaskR);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   /* setup bitmask to enable the following EDSP (EDSPx_INT1)
      interrupt sources */
   nRegMasksR[0] = 0;
   nRegMasksF[0] = 0;
#ifdef TAPI
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   nRegMasksR[0] =
      V2CPE_EDSP1_INTR1_UTD1_OK;    /* universal tone detection, receive */
#endif
   nRegMasksR[0] |=
      V2CPE_EDSP1_INTR1_DTMFR_DT |  /* valid DTMF key received */
      V2CPE_EDSP1_INTR1_DTMFG_BUF|  /* DTMF generator input buffer underflow */
      V2CPE_EDSP1_INTR1_DTMFG_REQ|  /* DTMF generator request */
      V2CPE_EDSP1_INTR1_DTMFG_ACT|  /* DTMF/UTG generator is active */
      V2CPE_EDSP1_INTR1_CIS_BUF  |  /* CID generator input buffer underflow */
      V2CPE_EDSP1_INTR1_CIS_REQ  |  /* CID generator request */
      V2CPE_EDSP1_INTR1_CIS_ACT;    /* CID generator is active */
#if (VIN_CFG_FEATURES & VIN_FEAT_VOICE)
   /* disable UTG activation interrupt */
   nRegMasksR[0] &= ~V2CPE_EDSP1_INTR1_DTMFG_ACT;
#endif
   /* enable UTG deactivation interrupt */
   nRegMasksF[0] |= V2CPE_EDSP1_INTR1_DTMFG_ACT;
   nRegMasksR[0] |= V2CPE_EDSP1_INTR1_UTD2_OK;
#ifdef TAPI_CID
   nRegMasksR[0] |=
      V2CPE_EDSP1_INTR1_CIDR_OF;    /* CID receiver overflow */
#endif
#endif /* TAPI */

   /* setup bitmask to enable the following group of EDSP (EDSPx_INT2)
      interrupt sources */
   nRegMasksR[1] = 0;
   nRegMasksF[1] = 0;
   nRegMasksR[1] =
      V2CPE_EDSP1_INTR2_PVPU_OF;    /* packet outbox overflow */
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
   nRegMasksR[1] |=
      V2CPE_EDSP1_INTR2_ATD2_DT  |  /* ATD1 detect */
      V2CPE_EDSP1_INTR2_FDP_REQ  |  /* fax data pump data request */
      V2CPE_EDSP1_INTR2_FDP_ERR;    /* fax datapump error */
#elif defined (ASYNC_CMD)
   nRegMasksR[1] |=
      V2CPE_EDSP1_INTR2_DEC_CHG;    /* decoder change */
#endif
#if (VIN_CFG_FEATURES & (VIN_FEAT_VOICE | VIN_FEAT_FAX_T38))
   nRegMasksR[1] |=
      V2CPE_EDSP1_INTR2_ATD2_DT; /* fax T38: DIS detection,
                                    fax modem: network tone detection */
#ifdef FW_ETU
   nRegMasksR[1] |=
      V2CPE_EDSP1_INTR2_EPOU_TRIG;
#endif
#endif

   /* write bitmasks to enable EDSP interrupts sources */
   for (i = 0, offset = 0; i < pDev->nCoderCnt; i++, offset += 4)
   {
      /* write bitmask to enable EDSP (EDSPx_INT1) interrupts */
      REG_WRITE_PROT(pDev, V2CPE_EDSP1_INTR1 + offset, nRegMasksR[0]);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      REG_WRITE_PROT(pDev, V2CPE_EDSP1_INTF1 + offset, nRegMasksF[0]);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);

      /* write bitmask to enable EDSP (EDSPx_INT2) interrupts */
      REG_WRITE_PROT(pDev, V2CPE_EDSP1_INTR2 + offset, nRegMasksR[1]);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      REG_WRITE_PROT(pDev, V2CPE_EDSP1_INTF2 + offset, nRegMasksF[1]);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
   }

   /* setup bitmask to enable the ALM (LINE_INT) interrupt sources */
   nRegMaskR =
      V2CPE_LINE1_INTR_ONHOOK    |  /* on-hook indication */
      V2CPE_LINE1_INTR_OFFHOOK   |  /* off-hook indication */
      V2CPE_LINE1_INTR_OTEMP;       /* over temperature warning */
   nRegMaskF = 0;

   /* write bitmask to enable the ALM (LINE_INT) interrupt sources */
   for (i = 0, offset = 0; i < pDev->nAnaChan; i++, offset += 2)
   {
      REG_WRITE_PROT(pDev, V2CPE_LINE1_INTR + offset, nRegMaskR);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      pDev->pChannel[i].hostCh.regLineX_IntR = nRegMaskR;
      REG_WRITE_PROT(pDev, V2CPE_LINE1_INTF + offset, nRegMaskF);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      pDev->pChannel[i].hostCh.regLineX_IntF = nRegMaskF;
   }

   /* setup bitmask to enable the following top-level (STAT_INT)
      interrupt sources */
   pDev->hostDev.nRegStatIen =
            V2CPE_STAT_IEN_VO_DATA |    /* voice outbox data */
#ifdef ASYNC_CMD
            V2CPE_STAT_IEN_CO_DATA |    /* command outbox data */
#endif
            V2CPE_STAT_IEN_WOKE_UP |    /* EDSP woke-up interrupt */
            V2CPE_STAT_IEN_GPIO    |    /* GPIO events interrupts */
            V2CPE_STAT_IEN_CRC_RDY |    /* CRC calculation ready */
            V2CPE_STAT_IEN_ERR     |    /* HW errors */
            V2CPE_STAT_IEN_CH;          /* channel events (any ALM or EDSP) */

   /* write bitmask for top-level interrupt (STAT_INT) sources */
   REG_WRITE_PROT(pDev, V2CPE_STAT_IEN, pDev->hostDev.nRegStatIen);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   return err;
}

/**
   Set EDSP resource  rising and falling edge interrupt masks according
   to given arguments
\param
   pCh      - handle to VINETIC_CHANNEL
\param
   edspRes  - EDSP ressource
\param
   nIsr     - Interrupt(IFX_TRUE)/Task(IFX_FALSE) context
\param
   nRising  - Rising edge mask : 0=disable, 1=enable
\param
   nFalling - Falling edge mask : 0=disable, 1=enable
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - This function is called by the dsp modules (_cid.c, _dspconf.c) to
     configure rising and falling edge interrupts accordingly.
   - Interrupt is enabled with bit set to 1 and disabled with bit set to 0.
   - Registers masks are the same for all registers. So V2CPE_EDSP1_INT1_XXX
     is used. Register offsets are adapted according to channel information.
   - This function can be used also within the interrupt routine
     (nIsr = IFX_TRUE).
*/
IFX_int32_t VINETIC_Host_Set_EdspIntMask (VINETIC_CHANNEL *pCh,
                                          VIN_EDSP_Res_t  edspRes,
                                          IFX_boolean_t   nIsr,
                                          IFX_uint16_t    nRising,
                                          IFX_uint16_t    nFalling)
{
   IFX_uint8_t  ch = (pCh->nChannel - 1);
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint16_t tmp_reg, nMask = 0, nOffsetR = 0, nOffsetF = 0;
   IFX_int16_t ret = IFX_SUCCESS;

   /* set falling/rising edge pointer, register offsets and interrupt masks
      according to EDSP ressource */
   switch (edspRes)
   {
      case VIN_EDSP_CIS:
      case VIN_EDSP_DTMFG:
      case VIN_EDSP_UTD:
      case VIN_EDSP_CPT:
      case VIN_EDSP_UTG1:
      case VIN_EDSP_UTG2:
         nOffsetR = V2CPE_EDSP1_INTR1;
         nOffsetF = V2CPE_EDSP1_INTF1;

         if (edspRes == VIN_EDSP_CIS)
         {
            nMask = (V2CPE_EDSP1_INT1_CIS_BUF | V2CPE_EDSP1_INT1_CIS_REQ |
                     V2CPE_EDSP1_INT1_CIS_ACT);
            break;
         }
         if (edspRes ==  VIN_EDSP_DTMFG)
         {
            nMask = (V2CPE_EDSP1_INT1_DTMFG_BUF | V2CPE_EDSP1_INT1_DTMFG_REQ |
                     V2CPE_EDSP1_INT1_DTMFG_ACT);
            break;
         }
         if (edspRes == VIN_EDSP_UTD)
         {
            nMask = (V2CPE_EDSP1_INT1_UTD1_OK | V2CPE_EDSP1_INT1_UTD2_OK);
            break;
         }
         if (edspRes == VIN_EDSP_CPT)
         {
            nMask = VIN_CPT_MASK;
            break;
         }
         if (edspRes == VIN_EDSP_UTG1)
         {
            nMask = VIN_UTG1_ACT_MASK;
            break;
         }

         if (edspRes == VIN_EDSP_UTG2)
         {
            nMask = VIN_UTG2_ACT_MASK;
            break;
         }

       /* no 'default' */
       /* lint -fallthrough */
       case VIN_EDSP_ATD:
         nOffsetR = V2CPE_EDSP1_INTR2;
         nOffsetF = V2CPE_EDSP1_INTF2;
         nMask    = (V2CPE_EDSP1_INT2_ATD1_DT | V2CPE_EDSP1_INT2_ATD2_DT |
                     V2CPE_EDSP1_INT2_ATD1_AM | V2CPE_EDSP1_INT2_ATD2_AM |
                     V2CPE_EDSP1_INT2_ATD1_NPR_MASK |
                     V2CPE_EDSP1_INT2_ATD2_NPR_MASK);
         break;
   }
   /* Set Rising edge Interrupt Mask
      Note: Register is read. Then all relevant mask bits are cleared.
      After this, register value is set according to parameter nRising
      and written back.*/
   if (nIsr == IFX_FALSE)
      VIN_HOST_PROTECT (pDev);
   REG_READ_UNPROT (pDev, (nOffsetR + (ch << 2)), &tmp_reg);
   CHECK_HOST_ERR (pDev, {ret = IFX_ERROR; goto EXIT_RELEASE;});
   tmp_reg &= ~nMask;
   tmp_reg |= nRising;
   REG_WRITE_UNPROT (pDev, (nOffsetR + (ch << 2)), tmp_reg);
   CHECK_HOST_ERR (pDev, {ret = IFX_ERROR; goto EXIT_RELEASE;});

   /* Set Falling edge Interrupt Mask
      Note: Register is read. Then all relevant mask bits are cleared.
      After this, register value is set according to parameter nRising
      and written back.*/
   REG_READ_UNPROT (pDev, (nOffsetF + (ch << 2)), &tmp_reg);
   CHECK_HOST_ERR (pDev, {ret = IFX_ERROR; goto EXIT_RELEASE;});
   tmp_reg &= ~nMask;
   tmp_reg |= nFalling;
   REG_WRITE_UNPROT (pDev, (nOffsetF + (ch << 2)), tmp_reg);
   CHECK_HOST_ERR (pDev, {ret = IFX_ERROR; goto EXIT_RELEASE;});

goto EXIT_RELEASE;
EXIT_RELEASE:
   if (nIsr == IFX_FALSE)
      VIN_HOST_RELEASE (pDev);

   return ret;
}

/**
  Maximizes/Minimizes Command Mailbox size according to bMax.
\param
   pDev    - pointer to the device interface
\param
   bMax    - TRUE = Maximise Mailbox / FALSE = Minumize Mailbox
\return
    IFX_SUCCESS or IFX_ERROR
\remarks
   This function switch the mailbox size and checks if the switch was done
   by reading the mailbox inbox size and doing some comparisons.
*/
IFX_int32_t VINETIC_Host_SwitchCmdMbxSize (VINETIC_DEVICE *pDev,
                                           IFX_boolean_t bMax)
{
   IFX_uint16_t glb_cfg_reg, box_clen_reg;
   IFX_uint8_t box_clen_wlen;

   /* read actual value of GLB_CFG register */
   REG_READ_PROT(pDev, V2CPE_GLB_CFG, &glb_cfg_reg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);
   /* FIFO_MD = 1 => maximize command mailbox size
      FIFO_MD = 0 => minimize command mailbox size */
   V2CPE_GLB_CFG_FIFO_MD_SET(glb_cfg_reg, ((bMax == IFX_TRUE) ? 1 : 0));
   /* write back GLB_CFG register */
   REG_WRITE_PROT(pDev, V2CPE_GLB_CFG, glb_cfg_reg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);
   /* read mailbox size */
   REG_READ_PROT(pDev, V2CPE_BOX_CLEN, &box_clen_reg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);
   /* check inbox size */
   box_clen_wlen = V2CPE_BOX_CLEN_WLEN_GET(box_clen_reg);

   if (bMax == IFX_TRUE)
   {
      if (box_clen_wlen != MAX_PACKET_WORD)
      {
         SET_ERROR(ERR_NOMAXCBX);
         return IFX_ERROR;
      }
   }
   else if (box_clen_wlen != MAX_CMD_WORD)
   {
      /* \todo set appropriate error code */
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/**
   Checks if the mailbox is empty
\param
   pDev         - handle to the device structure
\param
   full_size    - mail box size expected when empty (MAX_PACKET_WORD/MAX_CMD_WORD)
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   BOX_CLEN is polled within given timeout in ms. The polling is done in steps of
   WAIT_POLLTIME microsec.
*/
IFX_int32_t VINETIC_Host_CheckMbxEmpty  (VINETIC_DEVICE *pDev,
                                         IFX_int32_t full_size)
{
   IFX_int32_t usec = WAIT_MBX_EMPTY * 1000;
   IFX_uint16_t box_clen_reg;

   /* read BOX_CLEN in timeout till enough memory */
   do
   {
      IFXOS_DELAYUS (WAIT_POLLTIME);
      usec -= WAIT_POLLTIME;
      /* read mailbox size */
      REG_READ_PROT (pDev, V2CPE_BOX_CLEN, &box_clen_reg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      /* check free inbox size */
      if (V2CPE_BOX_CLEN_WLEN_GET(box_clen_reg) == full_size)
      {
         return IFX_SUCCESS;
      }
      else if (usec <= 0)
      {
         SET_ERROR (ERR_NO_MBXEMPTY);
         return IFX_ERROR;
      }
   } while (usec > 0);

   return IFX_ERROR;
}

/**
   Returns free command mailbox space
\param
   pDev         - handle to the device structure
\param
   cmdmbx_size  - command mailbox size
\return
   IFX_SUCCESS or IFX_ERROR
\remark
   - ptr to cmdmbx_size must be valid.
*/
IFX_int32_t VINETIC_Host_GetCmdMbxSize (VINETIC_DEVICE *pDev,
                                        IFX_uint8_t *cmdmbx_size)
{
   IFX_uint16_t mbx_size;

   *cmdmbx_size = 0;

   REG_READ_PROT(pDev, V2CPE_BOX_CLEN, &mbx_size);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   *cmdmbx_size = V2CPE_BOX_CLEN_WLEN_GET(mbx_size);

   return IFX_SUCCESS;
}

/**
   Checks download ready bit by polling bit within timeout
\param
   pDev      - pointer to the device interface
\return
   IFX_SUCCESS or IFX_ERROR
Remark :
*/
IFX_int32_t  VINETIC_Host_CheckDldReady  (VINETIC_DEVICE *pDev)
{
   IFX_int32_t  usec = WAIT_EDSP * 1000;
   IFX_uint16_t stat_reg;

   /* check download ready */
   do
   {
      IFXOS_DELAYUS (WAIT_POLLTIME);
      usec -= WAIT_POLLTIME;
      REG_READ_PROT (pDev, V2CPE_STAT, &stat_reg);
      CHECK_HOST_ERR(pDev, return IFX_ERROR);
      if (V2CPE_STAT_DL_RDY_GET(stat_reg))
      {
         return IFX_SUCCESS;
      }
   } while (usec > 0);

   /* something went wrong if you reach here */
   SET_ERROR (ERR_NO_DLRDY);

   return IFX_ERROR;
}

/**
   Resets host EDSP.
\param
   pDev      - pointer to the device interface
\return
   IFX_SUCCESS or IFX_ERROR
Remark :
   This must be done before doing the EDSP firmware download.
*/
IFX_int32_t  VINETIC_Host_ResetEdsp (VINETIC_DEVICE *pDev)
{
   IFX_uint16_t glb_ctrl_reg;

   REG_READ_PROT (pDev, V2CPE_GLB_CTRL, &glb_ctrl_reg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   /* RST = 1 : Reset EDSP */
   V2CPE_GLB_CTRL_RST_SET(glb_ctrl_reg, 1);
   /* ED = 1 : Start address = 0xFFFFFE */
   V2CPE_GLB_CTRL_ED_SET(glb_ctrl_reg, 1);
   /* HBOOT = 0b0011 : Download microprogram */
   V2CPE_GLB_CTRL_HBOOT_SET(glb_ctrl_reg, 0x3);

   REG_WRITE_PROT (pDev, V2CPE_GLB_CTRL, glb_ctrl_reg);
   CHECK_HOST_ERR(pDev, return IFX_ERROR);

   return IFX_SUCCESS;
}

/**
   Starts host EDSP.
\param
   pDev      - pointer to the device interface
\return
   IFX_SUCCESS or IFX_ERROR
Remark :
   The download End command starts also the EDSP.
*/
IFX_int32_t  VINETIC_Host_StartEdsp  (VINETIC_DEVICE *pDev)
{
   IFX_int32_t  err;
   IFX_uint16_t pDwldEnd [2] = {CMD1_EOP, ECMD_DWLD_END};

   /* Send command for download end */
   err =  CmdWrite (pDev, pDwldEnd, 0);
   /* check in STAT register if Download Ready Bit is set */
   if (err == IFX_SUCCESS)
      err = VINETIC_Host_CheckDldReady  (pDev);

   return err;
}

/*
 * Host download functions
 */

/**
  Download AC Micro Programm on given channel.
\param
   pCh      - pointer to the channel interface
\param
   bbd_ac   - handle to bbd AC block
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   - It is assumed that the given AC bbd block is valid and that the data
     representation in the bbd block is according to BBD specification,
     as follows:

     ofset_16 : 0xXX, 0xXX,
     data_16[]: 0xXX, 0xXX,
                0xXX, 0xXX,
                ...
     crc_16   : 0xXX, 0xXX
*/
IFX_int32_t VINETIC_Host_BBD_DownloadChAC (VINETIC_CHANNEL *pCh,
                                           bbd_block_t *bbd_ac)
{
   IFX_int32_t  err = IFX_ERROR;
   IFX_uint32_t count, pos = 0;
   IFX_uint8_t  len, *pByte;
   IFX_uint16_t ac_offset, ac_address,
                ac_crc, pCmd [MAX_CMD_WORD]  = {0};

   /* read offset */
   cpb2w (&ac_offset, &bbd_ac->data[0], sizeof (IFX_uint16_t));
   /* set AC payload pointer */
   pByte = &bbd_ac->data [2];
   /* set AC data size in words, removing offset bytes and and crc bytes */
   count = (bbd_ac->size - 4) >> 1 ;
   /* set AC Address where to load data */
   ac_address = pAcStartAddr [pCh->nChannel - 1] + ac_offset;
/* PRINTF ("AC : Offset=0x%04X,size(bytes)=0x%08lX,address=0x%04X,count=%ld\r\n",
            ac_offset, bbd_ac->size, ac_address, count);  */
   /* set CMD1_AOPW */
   pCmd[0] = CMD1_AOPW;
   /* write ACDSP data */
   while (count > 0)
   {
      /* calculate length to download (in words = 16bit) */
      if (count > (MAX_CMD_WORD - CMD_HEADER_CNT))
         len = (MAX_CMD_WORD - CMD_HEADER_CNT);
      else
         len = count;
      /* Set ACDSPADDR_HIGH in CMD1 */
      pCmd[0] |= HIGHBYTE(ac_address);
      /* Set CMD2 = (ACDSPADDR_LOW << 8) | DATA_LEN */
      pCmd [1] = (LOWBYTE(ac_address) << 8) | len;
      /* set ACDSP data while taking care of endianess  */
      cpb2w (&pCmd [2], &pByte [pos << 1], (len << 1));
      err = CmdWrite (pCh->pParent, pCmd, len);
      if (err == IFX_ERROR)
         break;
      /* \todo eventually call crc calculation routine here */
      /* increment position */
      pos += len;
      /* increment address */
      ac_address += len;
      /* decrement count */
      count -= len;
   }
   /* In case the download went through, read CRC from buffer and do checks */
   if (err == IFX_SUCCESS)
   {
      cpb2w (&ac_crc, &pByte [pos << 1], sizeof (IFX_uint16_t));
      TRACE(VINETIC, DBG_LEVEL_NORMAL, ("TODO: Compare AC CRC with BBD value :"
            " CRC=0x%04X\r\n", ac_crc));
      /* \todo Check download ACDSP CRC here, either by comparison or by
               reading it back from chip */
   }

   return err;
}

/**
   Download CRAM Coefficients on channel.
\param
   pCh        - handle to channel
\param
   bbd_cram   - handle to bbd CRAM block
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   - It is assumed that the given CRAM bbd block is valid and that the data
     representation in the bbd block is according to BBD specification,
     as follows:

     ofset_16 : 0xXX, 0xXX,
     data_16[]: 0xXX, 0xXX,
                0xXX, 0xXX,
                ...
     crc_16   : 0xXX, 0xXX
*/
IFX_int32_t VINETIC_Host_BBD_DownloadChCram  (VINETIC_CHANNEL *pCh,
                                              bbd_block_t     *bbd_cram)
{
   IFX_int32_t  err = IFX_ERROR;
   IFX_uint32_t count, pos = 0;
   IFX_uint8_t  len, *pByte;
   IFX_uint16_t cram_offset, cram_crc,
                pCmd [MAX_CMD_WORD]  = {0};

   /* read offset */
   cpb2w (&cram_offset, &bbd_cram->data[0], sizeof (IFX_uint16_t));
   /* set CRAM payload pointer */
   pByte = &bbd_cram->data [2];
   /* set CRAM data size in words, removing offset and crc bytes */
   count = (bbd_cram->size - 4) >> 1 ;
   /* CMD1 is a COP command  */
   pCmd[0] = CMD1_COP | (pCh->nChannel - 1);
   /* write CRAM data */
   while (count > 0)
   {
      /* calculate length to download (in words = 16bit) */
      if (count > (MAX_CMD_WORD - CMD_HEADER_CNT))
         len = (MAX_CMD_WORD - CMD_HEADER_CNT);
      else
         len = count;
      /* set CRAM offset in CMD2 */
      pCmd[1] = ((cram_offset + pos) << 8);
      /* set CRAM data while taking care of endianess  */
      cpb2w (&pCmd [2], &pByte  [pos << 1], (len << 1));
      /* write Data */
      err = CmdWrite (pCh->pParent, pCmd, len);
      if (err == IFX_ERROR)
         break;
      /* \todo eventually call crc calculation routine here */
      /* increment position */
      pos += len;
      /* decrement count */
      count -= len;
   }
   /* In case the download went through, read CRC from buffer and do checks */
   if (err == IFX_SUCCESS)
   {
      cpb2w (&cram_crc, &pByte [pos << 1], sizeof (IFX_uint16_t));
      /* \todo Check download CRAM CRC here, either by comparison or by
               reading it back from chip */
   }

   return err;
}

/**
   Sets Slic value on given channel(s).
\param
   pCh         - handle to channel structure
\param
   bBroadcast  - IFX_TRUE=all channels/IFX_FALSE=given channel.
\param
   slic_val    - selected slic type
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   In case of broadcast (bBroadcast = IFX_TRUE), slic is set on
   all channels.
*/
IFX_int32_t VINETIC_Host_SetSlic (VINETIC_CHANNEL *pCh,
                                  IFX_boolean_t    bBroadcast,
                                  IFX_uint16_t     slic_val)
{
   IFX_int32_t     err = IFX_SUCCESS;
   VINETIC_DEVICE *pDev = pCh->pParent;
   IFX_uint8_t     i, ch_start = 0, ch_stop = pDev->nAnaChan;
   CMD_Basic_Config_t   basicCfg;

   memset (&basicCfg, 0, sizeof (CMD_Basic_Config_t));
   basicCfg.CMD    = VIN_CMD_ALM;
   basicCfg.MOD    = VIN_MOD_DCCTL;
   basicCfg.ECMD   = VIN_ECMD_BASIC_CONFIG;
   basicCfg.LENGTH = 1;

   /* change start/stop if no broadcast */
   if (bBroadcast == IFX_FALSE)
   {
      ch_start = (pCh->nChannel - 1);
      ch_stop  = ch_start + 1;
   }
   /* set slic value now */
   for (i = ch_start; i < ch_stop; i++)
   {
      basicCfg.RW = VIN_CMD_RD;
      basicCfg.CH = i;
      /* read previous channel configuration,
         inclusive command header */
      err = CmdRead(pDev, (IFX_uint16_t *)((IFX_void_t *)&basicCfg),
                          (IFX_uint16_t *)((IFX_void_t *)&basicCfg), 1);
      if (err == IFX_ERROR)
         goto error;
      /* map slic type */
      switch (slic_val)
      {
         case BBD_VIN_SLIC_TYPE_E:
            basicCfg.SLIC_SEL = VIN_BASIC_CONFIG_SLIC_TYPE_E;
            break;
         case BBD_VIN_SLIC_TYPE_DC:
         default:
            basicCfg.SLIC_SEL = VIN_BASIC_CONFIG_SLIC_TYPE_DC;
            break;
      }
      basicCfg.RW = VIN_CMD_WR;
      /* write channel configuration with new slic */
      err = CmdWrite(pDev, (IFX_uint16_t *)((IFX_void_t *)&basicCfg), 1);
      if (err == IFX_ERROR)
         goto error;
   }

error:
   /* no error handling required */
   return err;
}

/**
   Set Ringing configuration on given channel(s).
\param
   pCh         - handle to channel structure
\param
   bBroadcast  - IFX_TRUE=all channels/IFX_FALSE=given channel.
\param
   p_ringCfg   - handle to ring configuration.
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   In case of broadcast (bBroadcast = IFX_TRUE), slic is set on
   all channels.
*/
IFX_int32_t  VINETIC_Host_RingCfg (VINETIC_CHANNEL   *pCh,
                                   IFX_boolean_t      bBroadcast,
                                   VINETIC_RingCfg_t *p_ringCfg)
{
   VINETIC_DEVICE    *pDev        = pCh->pParent;
   IFX_uint8_t        i, ch_start = 0, ch_stop = pDev->nAnaChan;
   CMD_Ring_Config_t  ringCfg;
   IFX_int32_t        err         = IFX_SUCCESS;

   memset (&ringCfg, 0, sizeof (CMD_Ring_Config_t));
   ringCfg.CMD     = VIN_CMD_ALM;
   ringCfg.MOD     = VIN_MOD_DCCTL;
   ringCfg.ECMD    = VIN_ECMD_RING_CONFIG;

   /* change start/stop if no broadcast */
   if (bBroadcast == IFX_FALSE)
   {
      ch_start = (pCh->nChannel - 1);
      ch_stop  = ch_start + 1;
   }

   for (i = ch_start; i < ch_stop; i++)
   {
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
      /* reset flag for this channel every time you are here */
      pDev->pChannel [i].hostCh.b_ring_cfg       = IFX_FALSE;
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */

      ringCfg.RW      = VIN_CMD_RD;
      ringCfg.CH      = i;
      ringCfg.LENGTH  = 9;
      err = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)&ringCfg),
                           (IFX_uint16_t *)((IFX_void_t *)&ringCfg), 9);
      if (err != IFX_SUCCESS)
         goto error;

      /*PRINTF ("rt duptime: old=%d new=%d\n",
                 ringCfg.TRIP_DUP, p_ringCfg->ring_trip_dup_time);*/
      ringCfg.RW         = VIN_CMD_WR;
      /* set the ring frequency */
      ringCfg.RING_F     = p_ringCfg->ring_freq;
      /* set the ring amplitude */
      ringCfg.RING_A     = p_ringCfg->ring_amp;
      /* set the ring hook level */
      ringCfg.RING_HL    = p_ringCfg->ring_hook_level;
      /* set the ring trip type */
      ringCfg.FAST_RT_EN = p_ringCfg->ring_trip_type;
      /* set ring trip dup time */
      ringCfg.TRIP_DUP   = p_ringCfg->ring_trip_dup_time;

      err = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)&ringCfg), 9);
      if (err != IFX_SUCCESS)
         goto error;

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
      /* Save Actual ring configuration for GR909 */
      pDev->pChannel [i].hostCh.ring_cfg   = ringCfg;
      pDev->pChannel [i].hostCh.b_ring_cfg       = IFX_TRUE;
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */
   }

error:
   return err;
}

/**
   Set DC Threshold configuration on given channel(s).
\param
   pCh         - handle to channel structure
\param
   bBroadcast  - IFX_TRUE=all channels/IFX_FALSE=given channel.
\param
   p_dcThr     - handle to dc threshold configuration.
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   In case of broadcast (bBroadcast = IFX_TRUE), slic is set on
   all channels.
*/
IFX_int32_t  VINETIC_Host_SetDcThr (VINETIC_CHANNEL *pCh,
                                    IFX_boolean_t    bBroadcast,
                                    VINETIC_DcThr_t *p_dcThr)
{
   VINETIC_DEVICE     *pDev = pCh->pParent;
   IFX_uint8_t         i, ch_start = 0, ch_stop = pDev->nAnaChan;
   CMD_Basic_Config_t  basicCfg;
   IFX_int32_t         err         = IFX_SUCCESS;

   memset (&basicCfg, 0, sizeof (CMD_Basic_Config_t));
   basicCfg.CMD      = VIN_CMD_ALM;
   basicCfg.MOD      = VIN_MOD_DCCTL;
   basicCfg.ECMD     = VIN_ECMD_BASIC_CONFIG;
   basicCfg.LENGTH   = 1;

   /* change start/stop if no broadcast */
   if (bBroadcast == IFX_FALSE)
   {
      ch_start = (pCh->nChannel - 1);
      ch_stop  = ch_start + 1;
   }
   /* set slic value now */
   for (i = ch_start; i < ch_stop; i++)
   {
      basicCfg.RW       = VIN_CMD_RD;
      basicCfg.CH       = i;
      err = CmdRead (pDev, (IFX_uint16_t *)((IFX_void_t *)&basicCfg),
                           (IFX_uint16_t *)((IFX_void_t *)&basicCfg), 1);
      if (err != IFX_SUCCESS)
         goto error;

      /*PRINTF ("dc ov duptime: old=%d new=%d\n",
                 basicCfg.OVT_DUP, p_dcThr->ovt_dup_time);*/
      basicCfg.RW       = VIN_CMD_WR;
      basicCfg.HOOK_SET = p_dcThr->onhook_time;
      basicCfg.HOOK_DUP = p_dcThr->hook_dup_time;
      basicCfg.OVT_DUP  = p_dcThr->ovt_dup_time;

      err = CmdWrite (pDev, (IFX_uint16_t *)((IFX_void_t *)&basicCfg), 1);
      if (err != IFX_SUCCESS)
         goto error;
   }

error:
   return err;
}

/*
 * Host ioctl functions
 */

/**
   Test the basic access of VINETIC device.
\param
   pDev    - pointer to the device interface
\param
   max_val - maximum test value ( <= 0xFFFF )
\return
   IFX_SUCCESS or IFX_ERROR
*/
IFX_int32_t VINETIC_Host_AccessTest (VINETIC_DEVICE *pDev, IFX_uint16_t max_val)
{
   IFX_uint16_t val, i, cnt = 0;
   IFX_uint8_t offset;

   if (max_val == 0)
      max_val = 0xFFFF;

   if (pDev->hostDev.nAccessMode == VIN_ACCESS_SPI)
      offset = V2CPE_DUPO_REG15;
   else
      offset = V2CPE_ADDR;

   TRACE (VINETIC, DBG_LEVEL_NORMAL, ("Starting access test : Write/Read/Compare"
          " val=[0 - 0x%04X] using register[offset 0x%02X]...\n\r",
           max_val, offset));

   for (i = 0; i <= max_val; i++)
   {
      REG_WRITE_PROT (pDev, offset, i);
      CHECK_HOST_ERR (pDev, return IFX_ERROR);
      REG_READ_PROT (pDev, offset, &val);
      CHECK_HOST_ERR (pDev, return IFX_ERROR);
      if (i != val)
      {
         cnt ++;
         TRACE (VINETIC, DBG_LEVEL_NORMAL, ("\n\rERROR drv_2cpe mismatch"
               " wr:0x%04X rd:0x%04X fail:%d\n\r", i, val, cnt));
      }
      REG_WRITE_PROT (pDev, offset, 0);
      CHECK_HOST_ERR (pDev, return IFX_ERROR);
      if (i % 1000)
      {
         TRACE (VINETIC, DBG_LEVEL_NORMAL,("."));
         TRACE (VINETIC, DBG_LEVEL_NORMAL,(" "));
      }
   }
   TRACE (VINETIC, DBG_LEVEL_NORMAL,(" finished\n\r"));

   if (cnt != 0)
   {
      SET_ERROR (ERR_TEST_FAIL);
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/**
  Initialize the VINETIC 2CPE Chip
\param
   pDev      - pointer to the device structure
\param
   pInit     - handle to the initialization structure
\return
   IFX_SUCCESS or IFX_ERROR
\remarks
   Initialization flow of 2CPE :
   - Disable Interrupts
   - Set chip endianness
   - Read Chip Version
   - Clear Interrupts
   - Enable Interrupts
   - Download Firmware
   - Download AC / CRAM/ Ring Config / DC values
   - Set Default firmware values
   - Set Default Register values
   - Set DSP idle mode
*/
IFX_int32_t VINETIC_Host_InitChip (VINETIC_DEVICE *pDev, VINETIC_IO_INIT *pInit)
{
   IFX_int32_t          err      = IFX_SUCCESS;
   IFX_uint8_t          i        = 0;
   IFX_uint16_t         nRev     = 0;
   V2CPE_DOWNLOAD       dwld2CPE;

   /* check if previous initialization was done */
   if (pDev->nDevState & DS_DEV_INIT)
      return IFX_SUCCESS;

   /* check if io init ptr valid */
   if (pInit == NULL)
   {
      SET_ERROR(ERR_FUNC_PARM);
      return IFX_ERROR;
   }

   Vinetic_IrqLockDevice (pDev);
   v2cpe_byte_swap (pDev);

   /* set 2CPE download pointers */
   Host_SetDwldPtr   (pInit, &dwld2CPE);
   /* read out chip version which contains details like
      - chip type        : mask 0xFXXX \todo verify this
      - channel number   : mask 0xXFXX \todo verify this
      - chip version     : mask 0xXXFX \todo verify this
      - DC Ctrl Revision : mask 0xXXXF \todo verify this
      \todo How to recognize 64 pin ? 100 pin ?
   */
   REG_READ_PROT (pDev, V2CPE_DUPO_REVISION, &nRev);
   if (pDev->err != ERR_OK)
   {
      err = IFX_ERROR;
      goto error;
   }
   /* decode version register and check */
   pDev->nChipRev = V2CPE_DUPO_REVISION_REV_GET (nRev);
   printk("\npDev->nChipRev = %X ; %X ; %X\n",pDev->nChipRev,VINETIC_2CPE_V21,nRev);
   if (pDev->nChipRev == VINETIC_2CPE_V21)
   {
      pDev->nAnaChan  = V2CPE_DUPO_REVISION_CHAN_GET (nRev);
      pDev->nChipType = V2CPE_DUPO_REVISION_TYPE_GET (nRev);
      pDev->nChipMajorRev = VINETIC_V2x;
      if (pDev->nAnaChan > VINETIC_2CPE_ANA_CH_NR)
         err = IFX_ERROR;
   }
   else
      err = IFX_ERROR;
   /* in case decoded version is not matching with expected ones */
   if (err == IFX_ERROR)
   {
      SET_ERROR (ERR_NO_VERSION);
      goto error;
   }
   /* clear interrupt register and interupt status registers */
   if ((err = Host_ClearPendingInt(pDev)) == IFX_ERROR)
      goto error;

   /* allow device interrupts */
   Vinetic_IrqUnlockDevice(pDev);

   /* reset rom firmware flag */
   pDev->hostDev.bRomFirmware = IFX_FALSE;
   /* download firmware */
   if ((err = Dwld_LoadFirmware(pDev, &dwld2CPE.edspDwld)) == IFX_SUCCESS)
   {
      /* get CRCs  */
      pInit->nPramCRC = dwld2CPE.edspDwld.nPramCRC;
      pInit->nDramCRC = dwld2CPE.edspDwld.nDramCRC;
      /* !!! workaround : Revision register gets updated only after
         the EDSP download is through. Update the channel info now */
      REG_READ_PROT (pDev, V2CPE_DUPO_REVISION, &nRev);
      CHECK_HOST_ERR(pDev, goto error);
      pDev->nAnaChan  = V2CPE_DUPO_REVISION_CHAN_GET (nRev);
      pDev->nChipMajorRev = VINETIC_V2x;
   }
   else
      goto error;
   
   /* download AC micro program on all channels if desired,
      as this is optional. */
   if (dwld2CPE.acDwld.buf != NULL)
   {
      if ((err = Host_DownloadAC (pDev, &dwld2CPE.acDwld)) == IFX_ERROR)
         goto error;
   }
   /* download CRAM on all channels if available.
      This can also be done separately via a dedicated ioctl */
   if (dwld2CPE.cramDwld.buf != NULL)
   {
      if ((err = Host_DownloadCram (pDev, &dwld2CPE.cramDwld))== IFX_ERROR)
         goto error;
   }
   /* set ring configuration on all channels to defaults or bbd download */
   if (dwld2CPE.ringCfgDwld.buf == IFX_NULL)
   {
      VINETIC_RingCfg_t  rc;
      memset (&rc, 0, sizeof(VINETIC_RingCfg_t));
      /* set the ring frequency */
      rc.ring_freq         = (VIN_2CPE_DEFAULT_RING_FREQ * 32768) / 4000;
      /* set the ring amplitude */
      rc.ring_amp          = VIN_2CPE_DEFAULT_RING_AMP;
      /* set the ring hook level */
      rc.ring_hook_level   = VIN_2CPE_DEFAULT_HOOK_LEVEL;
      /* set the ring trip type */
      rc.ring_trip_type    = VINETIC_RING_TRIP_TYPE_NORMAL;
      /* set the ring trip dup time */
      rc.ring_trip_dup_time = VIN_2CPE_DEFAULT_RING_TRIP_DUP_TIME;
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("WARN:  No ring configuration in BBD => default ring config is"
              " programmed\n\r"));
      err = VINETIC_Host_RingCfg (&pDev->pChannel[0], IFX_TRUE, &rc);
      if (err != IFX_SUCCESS)
         goto error;
   }
   else
   {
      if ((err = Host_SetRingCfg (pDev, &dwld2CPE.ringCfgDwld))== IFX_ERROR)
         goto error;
   }

   /* set DC thresholds on all channels to defaults or bbd download */
   if (dwld2CPE.dcThrDwld.buf == IFX_NULL)
   {
      VINETIC_DcThr_t dc;
      /* set the hook settling time, coef = t [ms]/32, t [0,32,64,96,..] */
      dc.onhook_time      = VIN_2CPE_DEFAULT_HOOK_SETTLING_TIME / 32;
      /* set dup time for hook detection, coef = t [ms] - 1 */
      dc.hook_dup_time    = VIN_2CPE_DEFAULT_HOOK_DUP_TIME - 1;
      /* set the overtemp dup time */
      dc.ovt_dup_time     = VIN_2CPE_DEFAULT_OVT_DUP_TIME;
      TRACE(VINETIC, DBG_LEVEL_HIGH,
            ("WARN:  No dc threshold configuration in BBD => default "
              "threshold configuration is programmed\n\r"));
      err = VINETIC_Host_SetDcThr (&pDev->pChannel[0],  IFX_TRUE, &dc);
      if (err != IFX_SUCCESS)
         goto error;
   }
   else
   {
      if ((err = Host_SetDcThr (pDev, &dwld2CPE.dcThrDwld))== IFX_ERROR)
         goto error;
   }
   /* now that downloads are finished, release memory */
   Host_UnsetDwldPtr (&dwld2CPE);
   /* set PCM channel resources needed for FW data configuration. */
   for (i = 0; i < VINETIC_MAX_CH_NR; i++)
     pDev->pChannel[i].nPcmCh = (IFX_uint8_t)i;
   /* set default firmware cache values now */
   if ((err = VINETIC_InitDevFWData(pDev)) == IFX_ERROR)
      goto error;
   /* set register default values */
   if ((err = VINETIC_Host_DefaultRegInit(pDev)) == IFX_ERROR)
      goto error;

   /* set DSP idle mode */
   if ((err = Dsp_SetIdleMode (pDev, IFX_TRUE)) == IFX_ERROR)
      goto error;

   pDev->nDevState |= DS_DEV_INIT;
   return IFX_SUCCESS;

error:
  // printk("ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
   /*
    This function disables the interrupt line of the device or the global
    interrupts instead. This is done by calling 'Vinetic_IrqLockDevice'.
    In case of an error the interrupt line is not unlocked again, but it
    is necessary to unlock the global interrupt of the system. */
   Vinetic_IrqUnlockGlobal (pDev);
   Host_UnsetDwldPtr (&dwld2CPE);
   return IFX_ERROR;
}


