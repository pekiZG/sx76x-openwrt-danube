/*******************************************************************************
       Copyright (c) 2004, Infineon Technologies.  All rights reserved.

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
   Module      : drv_amazon_api.c
   Date        : 2004-10-27
   Desription  : Implementation of API functions
   Remarks     : in the whole file, XYZ could be CPLD/FPGA or what ever device
                 != micro controller which need to be initialized.
*******************************************************************************/

/** \file
   This file contains the implementation of the generic API functions.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_amazon_api.h"

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Global variable declaration   */
/* ============================= */


/* ============================= */
/* Global functions declaration  */
/* ============================= */

/* API Low Level Functions import */
IFX_int32_t amazon_LL_BoardSetDefaultConf (amazon_Config_t     *pConf);
IFX_int32_t amazon_LL_BoardSetup          (amazon_AccessMode_e  nAccessMode,
                                             amazon_ClockRate_e   nClkRate);
IFX_int32_t amazon_LL_Board_ResetChip     (amazon_Reset_t       *pReset);
IFX_int32_t amazon_LL_BoardSetChipSelect  (IFX_uint32_t           nChipNum,
                                             amazon_AccessMode_e  nAccessMode);
IFX_int32_t amazon_LL_Board_GetAccessMode (amazon_AccessMode_e  *pAccessMode);
IFX_int32_t amazon_LL_Board_GetClockRate  (amazon_ClockRate_e   *pClockRate);
IFX_int32_t amazon_LL_Board_SetLed        (IFX_uint32_t           nLedNum,
                                             IFX_boolean_t          bLedState);
IFX_int32_t amazon_LL_Board_GetBoardVers  (IFX_int32_t           *pBoardVersion);
IFX_int32_t amazon_LL_Board_SetAccessMode (amazon_AccessMode_e  nAccessMode);
IFX_int32_t amazon_LL_Board_SetClockRate  (amazon_ClockRate_e   nClockRate);
IFX_int32_t amazon_LL_Board_RegAccess     (amazon_RegAccess_e   nRegAccess,
                                             amazon_Reg_t        *pReg);
/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */


IFX_int32_t amazon_Board_GetParams(amazon_CTRL_DEV *pDev,  amazon_GetDevParam_t *pParam)
{
    IFX_int32_t      ret;

   if (pDev->bBoardInitialized == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("EASY334_DRV :  Board not yet initialized\n\r"));
      return IFX_ERROR;
   }
   if (pParam->nChipNum >= pDev->nChipTotNum)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  Chip num %d out of range (>= %ld)\n\r",
              pParam->nChipNum, pDev->nChipTotNum));
      return IFX_ERROR;
   }
   /* set actual board parameters into control structure */
   ret = amazon_Board_GetConfig (pDev, NULL);
   if (ret == IFX_SUCCESS)
   {
      pParam->nAccessMode  = pDev->boardConf.nAccessMode;
      pParam->nClkRate     = pDev->boardConf.nClkRate;
      pParam->nBaseAddrPhy = (pDev->pChipInfo[pParam->nChipNum]).nBaseAddrPhy;
      pParam->nIrqNum      = (pDev->pChipInfo[pParam->nChipNum]).nIrqNum;
   }

   return ret;


    return IFX_SUCCESS;

}

/******************************************************************************/
/**
   Configures the parameters to be used for the board initialization

   \param pDev  - device pointer
   \param pConf - handle to amazon_Config_t struct

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      This function can be called if the board needs to be initialized with
      parameters different from default configuration. In case pConf = NULL,
      default parameters will be restored back.
      Default configuration is plattform specific and is set in low level.
   \see
   amazon_LL_BoardSetDefaultConf()
*/
/******************************************************************************/
IFX_int32_t amazon_Board_Config (amazon_CTRL_DEV *pDev,
                                  amazon_Config_t *pConf)
{
   IFX_int32_t ret = IFX_SUCCESS;

   if (pConf == NULL)
   {
      ret = amazon_LL_BoardSetDefaultConf (&pDev->boardConf);
   }
   else
   {
      pDev->boardConf = *pConf;
   }

   return ret;
}

/******************************************************************************/
/**
   Reads the configuration parameters used for the board initialization.

   \param pDev  - device pointer
   \param pConf - handle to amazon_Config_t struct /
                  NULL= Update internal structure directly

   \return
      IFX_SUCCESS or IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_Board_GetConfig (amazon_CTRL_DEV *pDev,
                                     amazon_Config_t *pConf)
{
   IFX_int32_t ret;
   IFX_boolean_t bNoUpdate = IFX_FALSE;

   if (pConf == NULL)
   {
      bNoUpdate = IFX_TRUE;
      pConf     = &pDev->boardConf;
   }
   if (pDev->bBoardInitialized == IFX_TRUE)
   {
      /* use config structure as tmp */
      ret = amazon_LL_Board_GetAccessMode (&pConf->nAccessMode);
      if (ret == IFX_SUCCESS)
         ret = amazon_LL_Board_GetClockRate (&pConf->nClkRate);
      if (ret == IFX_ERROR)
      {
         memset (pConf, 0, sizeof (amazon_Config_t));
         return IFX_ERROR;
      }
      /* update internal structure */
      if (bNoUpdate == IFX_FALSE)
      {
         pDev->boardConf.nAccessMode = pConf->nAccessMode;
         pDev->boardConf.nClkRate    = pConf->nClkRate;
      }
   }
   else
   {
      /* read from internal structure */
      *pConf = pDev->boardConf;
   }

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Initialize the Board and set internal control structure

   \param pDev  - device pointer

   \return
      Number of Vinetic chips on board or IFX_ERROR

   \remark
      This function calls low level functions to initialize the board (clock,\n
      chip select, irq line, ...).
      It is assumed that access to XYZ device if existing is possible here.

   \see
      amazon_Board_Config()\n
      amazon_LL_BoardSetup()\n
      amazon_LL_BoardSetChipSelect()\n
      amazon_LL_BoardInitIrq()\n
      amazon_ChipInfo[]\n
      namazonMaxVineticChips\n

*/
/******************************************************************************/
IFX_int32_t amazon_Board_Init (amazon_CTRL_DEV *pDev)
{
   IFX_int32_t ret;
   IFX_uint32_t i;
   amazon_Config_t *pInit = &pDev->boardConf;
   /*
   if (pDev->bXYZAccess == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  XYZ access not possible\n\r"));
      return IFX_ERROR;
   }
   */
   ret = amazon_LL_BoardSetup (pInit->nAccessMode, pInit->nClkRate);
   if (ret == IFX_SUCCESS)
   {
      
      for (i = 0; i < namazonMaxVineticChips; i++)
      {
         ret = amazon_LL_BoardSetChipSelect (i, pInit->nAccessMode);
         if (ret == IFX_ERROR)
            break;
      }
   }
   /* set internal control pointers */
   if (ret == IFX_SUCCESS)
   {
      /* map data */
      pDev->nChipTotNum       = namazonMaxVineticChips;
      pDev->pChipInfo         = (amazon_ChipInfo_t *)amazon_ChipInfo;
      pDev->bBoardInitialized = IFX_TRUE;
      /* return number of chips on board */
      //ret = (IFX_int32_t)namazonMaxVineticChips;
      ret=namazonMaxVineticChips;
   }

   return ret;
}

/******************************************************************************/
/**
   Get board parameters (base address, irq number, access mode, clock rate)
   pro vinetic chip.

   \param pDev   - device pointer
   \param pParam - pointer to amazon_GetDevParam_t

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      User application will call this function for every chip according
      to number of chips on board

   \see
      amazon_Board_Init()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_GetInfo (amazon_CTRL_DEV *pDev,
                                    amazon_GetDevParam_t *pParam)
{
   IFX_int32_t      ret;

   if (pDev->bBoardInitialized == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  Board not yet initialized\n\r"));
      return IFX_ERROR;
   }
   if (pParam->nChipNum >= pDev->nChipTotNum)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  Chip num %d out of range (>= %ld)\n\r",
              pParam->nChipNum, pDev->nChipTotNum));
      return IFX_ERROR;
   }
   /* set actual board parameters into control structure */
   ret = amazon_Board_GetConfig (pDev, NULL);
   if (ret == IFX_SUCCESS)
   {
      pParam->nAccessMode  = pDev->boardConf.nAccessMode;
      pParam->nClkRate     = pDev->boardConf.nClkRate;
      pParam->nBaseAddrPhy = (pDev->pChipInfo[pParam->nChipNum]).nBaseAddrPhy;
      pParam->nIrqNum      = (pDev->pChipInfo[pParam->nChipNum]).nIrqNum;
   }

   return ret;
}

/******************************************************************************/
/**
   Read a XYZ register (XYZ related to cpld, fpga or whatever)

   \param pDev - device pointer
   \param pReg - handle to amazon_Reg_t structure

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      This operation can be done only if XYZ access is possible.

   \see
      amazon_LL_Board_RegAccess()\n

*/
/******************************************************************************/
IFX_int32_t amazon_Board_ReadReg (amazon_CTRL_DEV *pDev,
                                    amazon_Reg_t *pReg)
{
   IFX_int32_t ret;

   if (pDev->bXYZAccess == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  XYZ access not possible\n\r"));
      return IFX_ERROR;
   }

   ret = amazon_LL_Board_RegAccess (amazon_XYZ_READ, pReg);

   return ret;
}

/******************************************************************************/
/**
   Write a XYZ register (XYZ related to cpld, fpga or whatever)
   \param pDev - device pointer
   \param pReg - handle to amazon_Reg_t structure

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      This operation can be done only if XYZ access is possible.

   \see
      amazon_LL_Board_RegAccess()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_WriteReg (amazon_CTRL_DEV *pDev,
                                     amazon_Reg_t *pReg)
{
   IFX_int32_t ret;

   if (pDev->bXYZAccess == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  XYZ access not possible\n\r"));
      return IFX_ERROR;
   }

   ret = amazon_LL_Board_RegAccess (amazon_XYZ_WRITE, pReg);

   return ret;
}

/******************************************************************************/
/**
   Modify a XYZ register (XYZ related to cpld, fpga or whatever)

   \param pDev - device pointer
   \param pReg - handle to amazon_Reg_t structure

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      This operation can be done only if XYZ access is possible.

   \see
      amazon_LL_Board_RegAccess()\n

*/
/******************************************************************************/
IFX_int32_t amazon_Board_ModifyReg (amazon_CTRL_DEV *pDev,
                                      amazon_Reg_t *pReg)
{
   IFX_int32_t ret;

   if (pDev->bXYZAccess == IFX_FALSE)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV :  XYZ access not possible\n\r"));
      return IFX_ERROR;
   }

   ret = amazon_LL_Board_RegAccess (amazon_XYZ_MODIFY, pReg);

   return ret;
}

/******************************************************************************/
/**
   Sets a board led

   \param pDev - device pointer
   \param pLed - handle to amazon_Led_t structure

   \return
      IFX_SUCCESS or IFX_ERROR



   \see
      amazon_LL_Board_SetLed()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_SetLed   (amazon_CTRL_DEV *pDev, amazon_Led_t* pLed)
{
   IFX_int32_t ret;

   /* to do: check if access to led registers is granted here */

   ret = amazon_LL_Board_SetLed (pLed->nLedNum, pLed->bLedState);

   return ret;
}

/******************************************************************************/
/**
   Resets the chip of given number

   \param pDev     - device pointer
   \param pReset   - handle to amazon_Reset_t structure

   \return
      IFX_SUCCESS or IFX_ERROR



   \see
      amazon_LL_Board_ResetChip()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_ResetChip (amazon_CTRL_DEV *pDev,
                                     amazon_Reset_t  *pReset)
{
   IFX_int32_t ret;

   /* to do: check if access to reset registers is granted here */

   ret = amazon_LL_Board_ResetChip (pReset);

   return ret;
}

/******************************************************************************/
/**
   Reads the board version

   \param pDev          - device pointer
   \param pBoardVersion - pointer to board version

   \return
      IFX_SUCCESS or IFX_ERROR



   \see
      amazon_LL_Board_ResetChip()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_GetBoardVers (amazon_CTRL_DEV *pDev,
                                         IFX_int32_t*pBoardVersion)
{
   IFX_int32_t ret;

   /* to do: check if access to version register is granted here */

   ret = amazon_LL_Board_GetBoardVers (pBoardVersion);

   return ret;
}




/******************************************************************************/
/**
   Sets the access mode

   \param pDev          - device pointer
   \param nAccessMode   - access mode to set

   \return
      IFX_SUCCESS or IFX_ERROR



   \see
      amazon_LL_Board_SetAccessMode()\n
      amazon_LL_BoardSetChipSelect()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_SetAccessMode (amazon_CTRL_DEV     *pDev,
                                         amazon_AccessMode_e  nAccessMode)
{
   IFX_int32_t ret, i;

   /* to do: check if access to access mode registers is granted here */

   ret = amazon_LL_Board_SetAccessMode (nAccessMode);
   if (ret == IFX_SUCCESS)
   {
      for (i = 0; i < pDev->nChipTotNum; i++)
      {
         ret = amazon_LL_BoardSetChipSelect (i, nAccessMode);
      }
   }

   return ret;
   }

/******************************************************************************/
/**
   Sets the clock rate

   \param pDev          - device pointer
   \param nClockRate    - clock rate

   \return
      IFX_SUCCESS or IFX_ERROR



   \see
      amazon_LL_Board_SetClockRate()\n
*/
/******************************************************************************/
IFX_int32_t amazon_Board_SetClockRate (amazon_CTRL_DEV *pDev,
                                        amazon_ClockRate_e   nClockRate)
{
   IFX_int32_t ret;

   /* to do: check if access to clock rate registers is granted here */

   ret = amazon_LL_Board_SetClockRate (nClockRate);

   return ret;
}

