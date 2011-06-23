#ifndef _DRV_API_H
#define _DRV_API_H
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
   Module      : drv_api.h
   Description :
      This file contains the defines, the structures declarations,
      the tables declarations and the global functions declarations.
      It is the main configuration file for the project and can be adapted
      to own needs.
   Remarks:
      Compiler switch for OS is needed. Use LINUX for linux and VXWORKS for
      VxWorks. WIN32 is just for test purposes for function tests.
      Make sure PPC is defined if using a PPC system. Currently this driver
      is working without RTAI but the time resolution is only 10 ms, affecting
      a instable voice downstreaming.
*******************************************************************************/

/**
 \mainpage VINETIC Driver Description
 \section DRV_INTEGRATION Integrating the Driver
 \subsection COMP_SWITCHES Compiler Switches
 - LINUX        - must be specified for LINUX
 - VXWORKS      - must be specified for VxWorks
 - PPC          - specified for TQM8xx board support package
 - TAPI         - TAPI interface supported
 - VMB3324      - board selection for TAPI
 - DEBUG        - enables debug features as asserts for example
 - ENABLE_TRACE - enables print out to terminal
 - VIN_8BIT     - enable SW support for 8Bit modes when the VINETIC is
                  configured via pins.
 - VIN_SPI      - enable SPI access to vinetic.
 - CRAM_DEFAULT - selects the default CRAM paramter set
                currently "GERMAN" and "USA600" are valid.
                Since CRAM coefficients are board specific, this
                switch should be set in the board file.
 - VIN_DEFAULT_FW - include default firmware in driver build for VINETIC
               VIP V1.4
 - TESTING      - testing features. Stores the latest commands and
               provides more detailed error print out
 - ASYNC_CMD    - for asynchronous command access between interrupt and user
               context. No waiting on read needed in certain cases.
               (expert use)
 - TAPI_LT      - enable line testing (see drv_tapi_def.h)
 - VIN_V14_SUPPORT - Support of VINETIC V1.4 enabled
 - VIN_V21_SUPPORT - Support of VINETIC V2.1 enabled
 - TAPI_GR909    - Vinetic Gr909 add-on.
 - NO_DATA_WIRING - no data channel is connected to analog channel on
   TAPI initialization. Use of interface IFXPHONE_DATA_ADD is needed
   to have a working setup for VoIP for example */

/* ============================= */
/* Global Defines                */
/* ============================= */

/* define prototypes for VxWorks 5 */
#ifndef __PROTOTYPE_5_0
   #define __PROTOTYPE_5_0
#endif

#ifdef HAVE_CONFIG_H
#include <drv_config.h>
#endif

#if !defined(VIN_V21_SUPPORT) && !defined(VIN_V14_SUPPORT)
#define VIN_V14_SUPPORT
#endif /* !defined(VIN_V21_SUPPORT) && !defined(VIN_V14_SUPPORT) */

#if defined(VXWORKS) || defined(LINUX)
   /* define to enable the shared interrupt handling */
   #define VIN_SHARED_INTERRUPT
#endif /* defined(VXWORKS) || defined(LINUX) */

/* ============================= */
/* includes                      */
/* ============================= */
#include "ifx_types.h"     /* ifx type definitions */
#include "sys_drv_debug.h" /* debug features */
#include "sys_drv_fifo.h"  /* fifo (used for streaming) */

/* this included file is supposed to support following OS:
   - LINUX
   - VXWORKS
   - NO_OS
   - WINDOWS
*/
#include "sys_drv_ifxos.h"

#ifdef ENABLE_USER_CONFIG
/* Note: This file must be available in your build directory.
   copy the template related to your environement from src/ to
   your build directory and rename it to drv_config_user.h
*/
#include <drv_config_user.h>
#endif /* ENABLE_USER_CONFIG */


#endif /* _DRV_API_H */

