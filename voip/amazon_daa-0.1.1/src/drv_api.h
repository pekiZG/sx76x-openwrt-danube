#ifndef _DRV_API_H_CPC5621
#define _DRV_API_H_CPC5621
/****************************************************************************
       Copyright (c) 2001, Infineon Technologies.  All rights reserved.

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
 ****************************************************************************
   Module      : $RCSfile: drv_api.h,v $
   Date        : $Date: 2004/08/02 08:49:40 $
   Description : This file contains the defines, the structures declarations
                 the tables declarations and the global functions declarations.
   Remarks:
      Use compiler switch ENABLE_TRACE for trace output, for debugging purposes.
      Compiler switch for OS is needed. Use LINUX for linux and VXWORKS for
      VxWorks..
*******************************************************************************/
/* ============================= */
/* Global Defines                */
/* ============================= */

/* Traces */
/** enable traces */
#define ENABLE_TRACE
/** enable logs, interrupt safe print's */
#define ENABLE_LOG


/* ============================= */
/* includes                      */
/* ============================= */

#ifdef OSE
#include "sys_drv_ose.h"
#endif
#ifdef LINUX
#include "sys_drv_linux.h"
#endif
#ifdef VXWORKS
#include "sys_drv_vxworks.h"
#endif

#include "sys_drv_defs.h"
#include "sys_drv_fifo.h"
#include "sys_drv_debug.h"

#ifdef LINUX
#include "drv_cpc5621_linux.h"
#endif

#ifdef VXWORKS
#include "drv_cpc5621_vxworks.h"
#endif

#ifdef NO_OS
#include "drv_cpc5621_noOS.h"
#endif

#ifdef VINETIC
#include "vinetic_io.h"
#include "drv_vinetic_gpio.h"
#endif

#ifdef DUSLIC
#include "duslic_io.h"
#endif


/* return values */

#ifndef OK
/** return value on success */
#define OK      0
#endif

#ifndef ERROR
/** return value on failure */
#define ERROR  (-1)
#endif

#endif /* _DRV_API_H_CPC5621 */
