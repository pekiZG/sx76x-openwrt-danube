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
   Module      : drv_AMAZON_ACCESS.c
   Date        : 2004-10-27
   Desription  : Implementation of Low Level board functions
   Remarks     : in the whole file, XYZ could be CPLD/FPGA or what ever device
                 != micro controller which need to be initialized.
*******************************************************************************/

/** \file
    This file contains the implementation of Low Level board functions.
    Reference to control device is avoided in the whole file.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "asm/amazon/irq.h"
#include "asm/amazon/amazon.h"
#include "drv_amazon_api.h"

/* ============================= */
/* Defines and Macros            */
/* ============================= */

/*
 * Vinetic defines
 */

/** number of vinetic chips on board,
    <em>Please read your system documentation and adap.</em> */
#define amazon_MAX_VINETIC_CHIPS        1

/** interrupt line 1, <em>assuming your board has 2 chips.</em>
    <em>Please read your hardware documentation and adapt.</em> */
#define amazon_VINETIC1_IRQNUM          INT_NUM_IM2_IRL13
/** interrupt line 1, <em>assuming your board has 2 chips.</em>
    <em>Please read your hardware documentation and adapt.</em> */
#define amazon_VINETIC2_IRQNUM          '?'

/** vinetic chip 1 base addresse, <em>assuming your board has 2 chips.</em>
    <em>Please read your hardware documentation and  adapt.</em> */
#define amazon_VINETIC1_BASEADDR_PHY    '?'
/** vinetic chip 2 base addresse, <em>assuming your board has 2 chips.</em>
    <em>Please read your hardware documentation and  adapt.</em> */
#define amazon_VINETIC2_BASEADDR_PHY    '?'

/* other vinetic defines */

/*
 * XYZ defines (CPLD, FPGA or any other controlling device)
 */

/** cpld base address,<em>just as example, if your board has a CPLD.</em>
    <em> Please read your hardware documentation and adapt.</em> */
#define amazon_XYZ_BASEADDR_PHY        '?'
/** cpld memory region,<em>just as example, if your board has a CPLD.</em>
    <em> Please read your hardware documentation and adapt.</em> */
#define amazon_XYZ_REGION_SIZE         '?'
/** XYZ base address for access */
#define amazon_XYZ_BASE                     pXYZBase

/* other CPLD defines */
/* Get CPLD base ptr */
#define amazon_GET_XYZ_PTR(ptr)\
         (ptr) = (IFX_vuint8_t *)  \
           ((IFX_uint32_t *)amazon_os_get_memaddr(amazon_XYZ_BASEADDR_PHY,\
                                                   amazon_XYZ_REGION_SIZE, \
                                                   "amazon_XYZ"));
/* Free CPLD base ptr */
#define amazon_FREE_XYZ_PTR(ptr) \
         amazon_os_release_memaddr ((IFX_void_t *)((IFX_uint32_t *)(ptr)), \
                                      amazon_XYZ_BASEADDR_PHY, \
                                      amazon_XYZ_REGION_SIZE);

/** macro to read a xyz register */
#define amazon_XYZ_REG_READ(offset,val) \
do {\
   val = *(amazon_XYZ_BASE + (offset));\
} while (0);

/** macro to write a xyz register */
#define amazon_XYZ_REG_WRITE(offset,val) \
do {\
   *(amazon_XYZ_BASE + (offset)) = (val);\
} while (0);
/*
 * Controler defines
 */

/** controller base address,
    <em>as defined in your controller documentation (BSP).</em> */
#define amazon_CONTROLLER_BASEADDR_PHY  '?'

/* other controller defines */

/** Reset times */

/** Reset active time in micro seconds, must be at least 4 us */
#define amazon_RESET_TIME_US                4
/** Reset recovery time in ms, must be at least 12 * 125 us */
#define amazon_RESET_RECOVER_MS             2

/** maximum number of leds which can be controlled */
#define amazon_MAXLEDNUM                    '?'

/* ============================= */
/* Local variable definition     */
/* ============================= */
static IFX_vuint8_t *pXYZBase = NULL;
const amazon_ChipInfo_t amazon_ChipInfo [amazon_MAX_VINETIC_CHIPS] =
{
   {0, amazon_VINETIC1_BASEADDR_PHY, amazon_VINETIC1_IRQNUM},
   {1, amazon_VINETIC2_BASEADDR_PHY, amazon_VINETIC2_IRQNUM}
   /* extend list here according to number of vinetic chips on your system */
};

/** amazon default reset */
const static amazon_Reset_t  amazon_InitReset [amazon_MAX_VINETIC_CHIPS] =
{
   {0, amazon_RESET_ACTIVE},
   {1, amazon_RESET_ACTIVE}
   /* extend list here according to number of vinetic chips on your system */
};

const IFX_uint32_t namazonMaxVineticChips = amazon_MAX_VINETIC_CHIPS;

/* ============================= */
/* Global function declaration   */
/* ============================= */

/** Get memory pointer for board access. This is OS dependant */
IFX_void_t * amazon_os_get_memaddr (IFX_uint32_t nBaseAddrPhy,
                                     IFX_uint32_t nMemSize,
                                     IFX_char_t  *pMemName);
/** Release memory pointer. This is OS dependant */
IFX_void_t amazon_os_release_memaddr (IFX_void_t  *pMemPtr,
                                       IFX_uint32_t nBaseAddrPhy,
                                                IFX_uint32_t nMemSize);

/* ============================= */
/* Global variable definition    */
/* ============================= */


/* ============================= */
/* Local function declaration    */
/* ============================= */


/* ============================= */
/* Local function definition     */
/* ============================= */


/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   Initialize XYZ Access (XYZ could be CPLD/FPGA or what ever)

   \param none

   \return
      IFX_SUCCESS / IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_InitXYZAccess (IFX_void_t)
{
   /* get xyz base ptr */
   amazon_GET_XYZ_PTR (amazon_XYZ_BASE);
   /* Initialize XYZ Chip Select */
   if (amazon_XYZ_BASE != NULL)
   {

   }
   else
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
             ("amazon_DRV : Can't access XYZ\n\r"));
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Resets CPLD Access

   \param none

   \return
      none
*/
/******************************************************************************/
IFX_void_t amazon_LL_Board_ResetXYZAccess (IFX_void_t)
{
   /* reset XYZ chip select */
   amazon_FREE_XYZ_PTR (amazon_XYZ_BASE);
   amazon_XYZ_BASE = NULL;
}

/******************************************************************************/
/**
   Sets board default configuration

   \param pConf - handle to amazon_Config_t structure

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
   This configuration will be used at board initialization if the user doesn't
   configure other values. Otherwise user configuration will be used.

   \see
      amazon_Config_t
*/
/******************************************************************************/
IFX_int32_t  amazon_LL_BoardSetDefaultConf (amazon_Config_t *pConf)
{
   if (pConf == NULL)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
            ("amazon_DRV : invalid config pointer\n\r"));
      return IFX_ERROR;
   }
   pConf->nAccessMode = AMAZON_ACCESS_SPI;
   pConf->nClkRate    = amazon_CLK_2048_KHZ;

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Set Chip select to access vinetic number

   \param nChipNum    - Id of chip to access
   \param nAccessMode - Chip Access Mode

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark

   \see
      amazon_AccessMode_e\n
      amazon_ChipInfo[]\n
*/
/******************************************************************************/
IFX_int32_t amazon_LL_BoardSetChipSelect (IFX_uint32_t          nChipNum,
                                            amazon_AccessMode_e nAccessMode)
{
   IFX_int32_t ret = IFX_SUCCESS;
   IFX_uint32_t nBaseAddrPhy = amazon_ChipInfo[nChipNum].nBaseAddrPhy;

   TRACE (amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV : chip base address:"
          " 0x%08lX \n\r", nBaseAddrPhy));

   switch (nAccessMode)
   {
   case AMAZON_ACCESS_16BIT_MOTOROLA:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_16BIT_INTELMUX:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_16BIT_INTELDEMUX:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_8BIT_MOTOROLA:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_8BIT_INTELMUX:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_8BIT_INTELDEMUX:
      ret = IFX_ERROR;
      break;
   case AMAZON_ACCESS_SPI:
      ret = IFX_SUCCESS;
      break;
   }

   if (ret == IFX_ERROR)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV : Chipselect: "
             "Configured Access Mode (id = %d) not supported\n\r", nAccessMode));
   }

   return ret;
}

/******************************************************************************/
/**
   Resets the chip of given number

   \param pReset  - handle to reset structure.

   \return
      IFX_SUCCESS or IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_ResetChip (amazon_Reset_t  *pReset)
{
   /* check chip number */
   if (pReset->nChipNum < amazon_MAX_VINETIC_CHIPS)
   {
      if ((pReset->nResetMode == amazon_RESET_ACTIVE) ||
          (pReset->nResetMode == amazon_RESET_ACTIVE_DEACTIVE))
      {
         /* to do: set reset active (i.e set reset pin to low) */
         *((volatile u32 *)(AMAZON_GPIO_P0_OUT))&=~0x4000;
         /* Chip reset: wait 4 us (at least) */
         IFXOS_DELAYUS (amazon_RESET_TIME_US);
      }
      if ((pReset->nResetMode == amazon_RESET_DEACTIVE) ||
          (pReset->nResetMode == amazon_RESET_ACTIVE_DEACTIVE))
      {
         /* to do : unreset (i.e set reset pin back to high) */
         *((volatile u32 *)(AMAZON_GPIO_P0_OUT))|=0x4000;
         /* After reset: wait (at least) 12*8kHz before the first access */
         IFXOS_DELAYMS (amazon_RESET_RECOVER_MS);
      }
   }
   else
      return IFX_ERROR;

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Setup the board while setting the data bus speed and the access mode.

   \param nAccessMode - Chip Access Mode
   \param nClkRate    - Clock to set

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
      This function must be call first to setup the board accordingly

   \see
      amazon_AccessMode_e\n
      amazon_ClockRate_e\n
*/
/******************************************************************************/
IFX_int32_t amazon_LL_BoardSetup (amazon_AccessMode_e nAccessMode,
                                    amazon_ClockRate_e  nClkRate)
{
   IFX_int32_t ret = IFX_SUCCESS, i;
   
   /* to do : initialize your board according to access mode and clock rate to
              be set (i.e via CPLD or FPGA or what ever)
   */
   *((volatile u32 *)(AMAZON_GPIO_P0_DIR))|=0x4000;
   *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL0))&=(u32)~0x4000;
   *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL1))&=(u32)~0x4000;
   *((volatile u32 *)(AMAZON_GPIO_P0_OD))|=0x4000;
   /*irq setup*/
   	//INT--GPIO P0.0-EXIN1, INT_NUM_IM2_IRL13
	//GPIO P0.0 IN/ALT0:1 ALT1:0
	(*AMAZON_GPIO_P0_DIR) = (*AMAZON_GPIO_P0_DIR) & 0xfffffffe;
	(*AMAZON_GPIO_P0_ALTSEL0) = (*AMAZON_GPIO_P0_ALTSEL0)| 1;
	(*AMAZON_GPIO_P0_ALTSEL1) = (*AMAZON_GPIO_P0_ALTSEL1)& 0xfffffffe;
	//External Interrupt Node
	(*AMAZON_ICU_EXTINTCR) = (*AMAZON_ICU_EXTINTCR)|0x20;
	(*AMAZON_ICU_IRNEN) = (*AMAZON_ICU_IRNEN)|0x2;
   
   
   /* reset all vinetic chips on this plattform */
   for (i = 0; i < amazon_MAX_VINETIC_CHIPS; i++)
   {
      amazon_LL_Board_ResetChip ((amazon_Reset_t*)&amazon_InitReset[i]);
   }

   return ret;
}

/******************************************************************************/
/**
   Sets led according to number and state

   \param nLedNum   - Number of Led to set.
   \param bLedState - State to set.

   \return
      IFX_SUCCESS or IFX_ERROR
*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_SetLed (IFX_uint32_t   nLedNum,
                                      IFX_boolean_t  bLedState)
{
   /* check led number */
   if (nLedNum <=  amazon_MAXLEDNUM)
   {

      if (bLedState == IFX_TRUE)
      {
         /* to do: set led bit */
      }
      else
      {
         /* to do: clear led bit */
      }
   }
   else
      return IFX_ERROR;

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Reads the current access mode

   \param pAccessMode - ptr to the access mode.

   \return
      IFX_SUCCESS or IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_GetAccessMode (amazon_AccessMode_e  *pAccessMode)
{
   IFX_uint8_t nAccessMode = 0;

   /* to do: read actual access mode from board */
   
   /* return access mode via pAccessMode pointer */
   *pAccessMode = nAccessMode;

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Gets the actual Clock Rate

   \param pClockRate - ptr to the clock rate.

   \return
      IFX_SUCCESS or IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_GetClockRate  (amazon_ClockRate_e  *pClockRate)
{
   IFX_uint8_t  nClkRate = 0;

   /* to do: read actual clock rate from board*/

   /* return clock rate via pClockRate pointer*/
   *pClockRate = nClkRate;

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Reads the board version

   \param pBoardVersion - ptr to the board version.

   \return
      IFX_SUCCESS or IFX_ERROR

*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_GetBoardVers  (IFX_int32_t *pBoardVersion)
{
   /* to do: read board version and return it via pBoardVersion pointer */


   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Sets access mode

   \param nAccessMode - access mode.

   \return
      IFX_SUCCESS or IFX_ERROR

   \see
      amazon_AccessMode_e\n
*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_SetAccessMode (amazon_AccessMode_e nAccessMode)
{
   /* to do: set vinetic access mode */

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Sets clock rate

   \param nClockRate - clock rate.

   \return
      IFX_SUCCESS or IFX_ERROR

   \see
      amazon_ClockRate_e\n
*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_SetClockRate (amazon_ClockRate_e nClockRate)
{
   /* to do: set vinetic clock rate */

   return IFX_SUCCESS;
}


/******************************************************************************/
/**
   Read register

   \param nRegAccess - register access mode
   \param pReg       - handle to amazon_Reg_t structure.

   \return
      IFX_SUCCESS or IFX_ERROR

   \see
      amazon_Reg_t\n
*/
/******************************************************************************/
IFX_int32_t amazon_LL_Board_RegAccess (amazon_RegAccess_e  nRegAccess,
                                        amazon_Reg_t        *pReg)
{
   switch (nRegAccess)
   {
   case  amazon_XYZ_READ:
      amazon_XYZ_REG_READ(pReg->nRegOffset, pReg->nRegVal);
      break;
   case  amazon_XYZ_WRITE:
      amazon_XYZ_REG_WRITE(pReg->nRegOffset, pReg->nRegVal);
      break;
   case  amazon_XYZ_MODIFY:
      {
         IFX_uint8_t tmpVal;
         amazon_XYZ_REG_READ(pReg->nRegOffset, tmpVal);
         tmpVal = (tmpVal & ~pReg->nRegMask) | (pReg->nRegVal & pReg->nRegMask);
         amazon_XYZ_REG_WRITE (pReg->nRegOffset, tmpVal);
      }
      break;
   }

   return IFX_SUCCESS;
}
