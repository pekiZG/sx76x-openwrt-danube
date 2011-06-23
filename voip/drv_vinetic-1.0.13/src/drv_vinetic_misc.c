/****************************************************************************
                  Copyright  2005  Infineon Technologies AG
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
   Module      : drv_vinetic_misc.c
   Description : This file contains the implementation of the functions
                 for usual vinetic operations.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_version.h"
#include "drv_vinetic_api.h"
#include "drv_vinetic_misc.h"
#include "drv_vinetic_main.h"
/*
#include "drv_vinetic_dwnld.h"
#include "drv_vinetic_parallel.h"
*/
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

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/*******************************************************************************
Description:
   VINETIC Report Set for all vinetic messages
Arguments:
   pDev         - pointer to the device interface
   driver_level - new DBG_LEVEL
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t VINETIC_Report_Set(IFX_uint32_t driver_level)
{
   if ((driver_level > DBG_LEVEL_HIGH) || (driver_level < DBG_LEVEL_LOW))
   {
      SetTraceLevel(VINETIC, DBG_LEVEL_OFF);
      SetLogLevel(VINETIC, DBG_LEVEL_OFF);
   }
   else
   {
      SetTraceLevel(VINETIC, driver_level);
      SetLogLevel(VINETIC, driver_level);
   }

   return IFX_SUCCESS;
}

IFX_int32_t VINETIC_RuntimeTrace_Report_Set (IFX_uint32_t level)
{
#ifdef RUNTIME_TRACE
   SetTraceLevel (VINETIC_RUNTIME_TRACE, level);
   return IFX_SUCCESS;
#else
   return IFX_ERROR;
#endif /* RUNTIME_TRACE */
}

/*******************************************************************************
Description:
   Get the VINETIC Chip Version
Arguments:
   pDev - pointer to the device interface
   pInd - pointer to the user interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
*******************************************************************************/
IFX_int32_t VINETIC_Version (VINETIC_DEVICE *pDev, VINETIC_IO_VERSION *pInd)
{
   IFX_int32_t err = IFX_SUCCESS;
   IFX_uint16_t pCmd[2] = {0}, pData[4] = {0}, pRvCmd[2] = {0x8801, 0x8001}, nVers = 0;
#if 1
      	    {
	    unsigned short pIr [2];
   	    SC_READ (SC_RIR, pIr, 1);
            printk("pIr[0]=%4x\n",pIr[0]);
            SC_READ (SC_RHWSR, pIr, 2);
            printk("pIr[0]=%4X, pIr[1]=%4X\n",pIr[0], pIr[1]);
            SC_READ (SC_RBXSR, pIr, 2);
            printk("pIr[0]=%4X, pIr[1]=%4X\n",pIr[0], pIr[1]);
            }
#endif   
   
   /* read chip version */
   err = pDev->diop_read(pDev, pRvCmd, &nVers, 1);
printk ("Diop Read: nVers = 0x%04X err=%08x\n", nVers,err);
   if (err == IFX_SUCCESS)
   {
      /* set type */
      pInd->nType    = (nVers & REVISION_TYPE) >> 12;
      /* set channel */
      pInd->nChannel = (nVers & REVISION_ANACHAN) >> 8;
      /* set chip revision */
      pInd->nChip    = (nVers & REVISION_REV);
   }
   else
   {
      pInd->nChannel = 0;
      pInd->nChip    = 0;
      err = IFX_ERROR;
      SET_ERROR(ERR_DEV_ERR);
      TRACE(VINETIC, DBG_LEVEL_HIGH,
           ("IFX_ERROR: Read Chip Revision Register failed\n\r"));
   }
   /* assume no EDSP as default */
   pInd->nEdspVers   = NOTVALID;
   pInd->nEdspIntern = NOTVALID;
   /* read firmware version only if download already done */
   if ((pDev->nDevState & DS_FW_DLD) && (err == IFX_SUCCESS))
   {
      /*read edsp firmware version */
      pCmd[0] = CMD1_EOP;
      pCmd[1] = ECMD_VERS;
      err = CmdRead(pDev, pCmd, pData, 2);
      if (err == IFX_SUCCESS)
      {
         /* Set external and internal versions */
         pInd->nEdspVers   = pData[2];
         pInd->nEdspIntern = pData[3];
      }
      else
         TRACE(VINETIC, DBG_LEVEL_HIGH,
              ("IFX_ERROR: Read EDSP Firmware Version failed\n\r"));
   }
   /* set TAPI version */
   pInd->nTapiVers = TAPI_VERS;
   /* set Driver version*/
   pInd->nDrvVers = MAJORSTEP << 24 | MINORSTEP << 16 |
                    VERSIONSTEP << 8 | VERS_TYPE;
   return err;
}

/*******************************************************************************
Description:
   Get the VINETIC Driver Version
Arguments:
   pDev - pointer to the device interface
   pInd - pointer to the user interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remarks:
   The difference between this function and VINETIC_Version is just, that the
   vinetic chip is not accessed. This avoid error messages if just the driver
   version is of interest
*******************************************************************************/
IFX_int32_t VINETIC_DrvVersion (VINETIC_IO_VERSION *pInd)
{
   IFX_int32_t err = IFX_SUCCESS;
   /* set Driver version*/
   pInd->nDrvVers = MAJORSTEP << 24 | MINORSTEP << 16 |
                    VERSIONSTEP << 8 | VERS_TYPE;
   return err;
}

/*******************************************************************************
Description:
  get different versions (FW, HW, DRIVER, EDSP, ...)
Arguments:
   pDev - pointer to the device interface
Return:
    IFX_SUCCESS if no error, otherwise IFX_ERROR
Remarks:
   This function calls the IO function VINETIC_Version
*******************************************************************************/
IFX_int32_t VINETIC_GetVersions(VINETIC_DEVICE *pDev)
{
   VINETIC_IO_VERSION ioVers;
   IFX_int32_t err = IFX_SUCCESS;

   /* initialize version structure */
   memset(&ioVers, 0, sizeof(VINETIC_IO_VERSION));
   /* call version function */
   err = VINETIC_Version(pDev, &ioVers);
   if (err == IFX_SUCCESS)
   {
      /* set number of analog channels decoded from revision register */
      if ((ioVers.nEdspVers == NOTVALID) || (ioVers.nEdspIntern == NOTVALID))
         pDev->nAnaChan = ioVers.nChannel;
   }

   return err;
}

/*******************************************************************************
 * GPIO support (outdated)
 ******************************************************************************/
/*******************************************************************************
Description:
   Configure device GPIO pins 0..7
Arguments:
   pDev     - pointer to device structure
   pCfg     - pointer to configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   The configuration is written to the Vinetic only if the BoardInit has been
   done already (pDev->nDevState & DS_Board_Init).Otherwise the configuration
   is buffered and written when the chip is beeing initialized.
*******************************************************************************/
IFX_int32_t VINETIC_Dev_GPIO_Cfg (VINETIC_DEVICE *pDev,
                                  VINETIC_IO_DEV_GPIO_CFG const *pCfg)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (pDev->nDevState & DS_BASIC_INIT)
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR1_REG);

   if (err == IFX_SUCCESS)
   {
      GCR1.value &= (IFX_uint16_t)(0x00FF | (~ (pCfg->nGPIO<<8)));
      if (pCfg->ctrld == VINETIC_IO_DEV_GPIO_CONTROLL_RESERVED)
         GCR1.value |= (0xFF00 & (pCfg->nGPIO<<8));
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR1_REG);
   }
   if ((err == IFX_SUCCESS) && (pDev->nDevState & DS_BASIC_INIT))
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR2_REG);
   if (err == IFX_SUCCESS)
   {
      /* Input Enable */
      GCR2.value &= (IFX_uint16_t)(0x00FF | (~ (pCfg->nGPIO<<8)));
      if (pCfg->drIn == VINETIC_IO_DEV_GPIO_DRIVER_ENABLED)
         GCR2.value |= (0xFF00 & (pCfg->nGPIO<<8));
      /* Output Enable */
      GCR2.value &= (0xFF00 | (~ pCfg->nGPIO));
      if (pCfg->drOut == VINETIC_IO_DEV_GPIO_DRIVER_ENABLED)
         GCR2.value |= (0x00FF & pCfg->nGPIO);
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR2_REG);
   }

   return err;
}

/*******************************************************************************
Description:
   Set levels to device GPIO pins 0..7 (if configured as output)
Arguments:
   pDev     - pointer to device structure
   pCfg     - pointer to configuration structure
Return:
   IFX_SUCCESS or IFX_ERROR
Remark :
   The configuration is written to the Vinetic only if the BoardInit has been
   done already (pDev->nDevState & DS_Board_Init).Otherwise the configuration
   is buffered and written when the chip is beeing initialized.
*******************************************************************************/
IFX_int32_t VINETIC_Dev_GPIO_Set (VINETIC_DEVICE *pDev,
                                  VINETIC_IO_DEV_GPIO_SET const *pSet)
{
   IFX_int32_t err = IFX_SUCCESS;

   if (pDev->nDevState & DS_BASIC_INIT)
      err = rdReg((VINETIC_CHANNEL*) pDev, GCR1_REG);

   if (err == IFX_SUCCESS)
   {
      GCR1.value &= (0xFF00 | (~pSet->mask));
      GCR1.value |= (0x00FF & pSet->value);
      if (pDev->nDevState & DS_BASIC_INIT)
         err = wrReg((VINETIC_CHANNEL*) pDev, GCR1_REG);
   }

   return err;
}

/*******************************************************************************
Description:
   Test the basic access of VINETIC device
Arguments:
   pDev - pointer to the device interface
Return   :
   IFX_SUCCESS or IFX_ERROR
Remarks  :
   - This function writes a sequence of Data from 0 to 0xFFFF according to given
   write/read command words, read back and give a result to user
   The DIOP command used are locally programmed.

   - This function will be called at initialization time to test the chip acces
     and schould not be used for other purposes.
*******************************************************************************/
IFX_int32_t VINETIC_AccessTest (VINETIC_DEVICE *pDev, IFX_uint16_t max_val)
{
   IFX_int32_t  err = IFX_SUCCESS, i = 0;
#ifdef VIN_V14_SUPPORT
   IFX_uint16_t pCmdWr[3] = {0}, pCmdRd[3] = {0}, val = 0;
#endif /* VIN_V14_SUPPORT */
#ifdef VIN_V21_SUPPORT
   IFX_vuint16_t* pTest  = (IFX_vuint16_t*)((IFX_uint32_t)pDev->pBaseAddr  + 2);
#endif /* VIN_V21_SUPPORT */

#if (defined (VIN_V21_SUPPORT) && defined (VIN_V14_SUPPORT))
   if (pDev->nChipMajorRev == VINETIC_V2x)
   {
#endif /* defined(VIN_V14_SUPPORT) && defined(VIN_V21_SUPPORT) */
#ifdef VIN_V21_SUPPORT
      for (i = 0; i < max_val; i++)
      {
         (*pTest) = i;
         if (*pTest != i)
         {
            SET_ERROR (ERR_TEST_FAIL);
            err = IFX_ERROR;
            break;
         }
      }
#endif /* VIN_V21_SUPPORT */
#if defined(VIN_V14_SUPPORT) && defined(VIN_V21_SUPPORT)
   }
   else
   {
#endif /* defined(VIN_V14_SUPPORT) && defined(VIN_V21_SUPPORT) */
#ifdef VIN_V14_SUPPORT
   /* fill write and read cmd buffers,
     address 0x0070, free space */
   pCmdWr [0] = CMD1_DIOP | CMD1_WR;
   pCmdWr [1] = 0x7001;
   pCmdRd [0] = CMD1_DIOP | CMD1_RD;
   pCmdRd [1] = pCmdWr [1];

   /* lock */
   IFXOS_MutexLock(pDev->mbxAcc);
   for (i = 0; i < 10; i++)
   {
      pCmdWr[2] = i;
      err = pDev->write (pDev, pCmdWr, 3);
      if (err == IFX_SUCCESS)
         err = pDev->diop_read (pDev, pCmdRd, &val, 1);
      if (err != IFX_SUCCESS)
      {
         SET_ERROR (ERR_DEV_ERR);
         break;
      }
      else if (val != i)
      {
         SET_ERROR (ERR_TEST_FAIL);
         err = IFX_ERROR;
         break;
      }
   }
   /* release lock */
   IFXOS_MutexUnlock (pDev->mbxAcc);

   if (err != IFX_SUCCESS)
      TRACE (VINETIC, DBG_LEVEL_HIGH,
            ("ERROR: Test Chip Access failed\n\r"));
#endif /* VIN_V14_SUPPORT */
#if defined(VIN_V14_SUPPORT) && defined(VIN_V21_SUPPORT)
   }
#endif /* defined(VIN_V14_SUPPORT) && defined(VIN_V21_SUPPORT) */
   return err;
}


