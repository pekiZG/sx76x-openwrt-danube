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
   Module      : drv_amazon_common.c
   Date        : 2004-10-26
   Desription  : Contains the implementation of common functions
                 which aren't part of the API.
   Remarks     : in the whole file, XYZ could be CPLD/FPGA or what ever device
                 != micro controller which need to be initialized.
*******************************************************************************/

/** \file
     This file contains the implementation of common functions which aren't
     part of the API.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_amazon_api.h"

/* ============================= */
/* Local defines                 */
/* ============================= */

/* a chip id of this value is invaid */
#define INVALID_ID            -1

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Global variable declaration   */
/* ============================= */

/* ============================= */
/* Global functions declaration  */
/* ============================= */

IFX_int32_t amazon_LL_Board_InitXYZAccess  (IFX_void_t);
IFX_void_t  amazon_LL_Board_ResetXYZAccess (IFX_void_t);
IFX_int32_t amazon_LL_BoardSetDefaultConf  (amazon_Config_t *pConf);

/* ============================= */
/* Global variable definition    */
/* ============================= */

/** trace group implementation */
CREATE_TRACE_GROUP(amazon_DRV);
/** log group implementation */
CREATE_LOG_GROUP(amazon_DRV);

/** driver version string */
const char amazon_WHATVERSION [] = DRV_amazon_WHAT_STR;
/** device structures */
amazon_CTRL_DEV *samazon_Dev [MAX_amazon_INSTANCES] = {0};

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   Device private data initialization

   \param nDevNum - id of board device to initialize

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
   - This function allocates memory needed by the driver and initializes system
     data like semaphores or queues, if applicable.
   - Device Id is set as 1 based to identidy the device structure used
*/
/******************************************************************************/
IFX_int32_t amazon_DevInit (IFX_uint32_t nDevNum)
{
   IFX_int32_t ret;

   if (samazon_Dev [nDevNum] != NULL)
   {
      /* reset whole structure */
      memset (samazon_Dev [nDevNum], 0, sizeof (amazon_CTRL_DEV));
      /* allocate memory if applicable
      */
      /* set device id starting from 1 */
      samazon_Dev [nDevNum]->nDevNum = nDevNum;
         /* set default configuration */
      amazon_LL_BoardSetDefaultConf (&samazon_Dev[nDevNum]->boardConf);
      
   }
   else
      ret = IFX_ERROR;

   return ret;
}

/******************************************************************************/
/**
   Device private data uninitialization

   \param nDevNum - id of board device to initialize

   \return
      IFX_SUCCESS or IFX_ERROR

   \remark
   - This function releases memory needed by the driver and uninitializes system
     data like semaphores or queues, if applicable.
   - Device Id is reset to 0 to mark that this device isn't yet initialized.
*/
/******************************************************************************/
IFX_int32_t amazon_DevReset (IFX_uint32_t nDevNum)
{
   if (samazon_Dev [nDevNum] != NULL)
   {
      /* free memory if allocated at initialization time

      */
      /* release XYZ ptr if applicable */
      if (samazon_Dev[nDevNum]->bXYZAccess == IFX_TRUE)
      {
         amazon_LL_Board_ResetXYZAccess ();
         samazon_Dev[nDevNum]->bXYZAccess = IFX_FALSE;
      }
      /* reset device id: device not more supported */
      samazon_Dev [nDevNum]->nDevNum = INVALID_ID;
   }
   else
      return IFX_ERROR;

   return IFX_SUCCESS;
}

