#ifndef _DRV_API_H
#define _DRV_API_H
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
 - DEBUG        - enables debug features as asserts for example
 - ENABLE_TRACE - enables print out to terminal
 - INTERNAL     - for internal test features
 - VIN_8BIT     - enables 8 bit access to vinetic, if not necessary
                do not select, cause would result in larger code
 - VIN_SPI      - enable SPI access to vinetic.
 - NO_SPI_MUTEX - dont work with SPI Mutex for SPI modules
 - VIN_8S       - supports the VINETIC 8S, which are two chips in one
                package
 - VIN_DEFAULT_FW - include default firmware in driver build
 - TESTING      - testing interface features
 - ASYNC_CMD    - for asynchronous command access. no waiting on read
                needed in certain cases
 - TAPI_LT      - enable line testing (see drv_tapi_def.h)
 - TAPI_CID - support CID
 - TAPI_DTMF - support DTMF detection
 - TAPI_VOICE - support voice packet handling
 - VIN_POLL - support polling mode of events and packets
 - ASYNC_CMD - support asynchronous mailbox command handling, where read
               commands are send also from interrupt level
 - VIN_V14_SUPPORT - Support of VINETIC V1.4 enabled
 - VIN_V21_SUPPORT - Support of VINETIC V2.1 enabled
 - TAPI_GR909    - Vinetic Gr909 add-on.
 - EXCHFD_SUPPORT - enables support for exception file descriptors.
   since VxWorks does not support exception fds exceptions are
   implemented on fd0 (dev/vin10). Thus no channel specific exception
   is raised. Only for compability
 - DEFAULT_LM_NORMAL - set the default line mode to normal after
   TAPI initialization
 - NO_DATA_WIRING - no data channel is connected to analog channel on
   TAPI initialization. Use of interface IFX_TAPI_MAP_DATA_ADD is needed
   to have a working setup for VoIP for example
 - FW_ETU - support of firmware with event transmission unit
*/

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

#ifdef VIN_2CPE
/* Note: In case 2CPE is supported, V1.x and V2.X aren't supported
   because the chip architecture is totally different. */
#undef VIN_V14_SUPPORT
#undef VIN_V21_SUPPORT
#else
#if !defined(VIN_V21_SUPPORT) && !defined(VIN_V14_SUPPORT)
#define VIN_V14_SUPPORT
#endif /* !defined(VIN_V21_SUPPORT) && !defined(VIN_V14_SUPPORT) */
#endif /* VIN_2CPE */
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


/*
   list of possible features to check against VIN_CFG_FEATURES
*/

/* support for Vinetic 4S */
#define VIN_FEAT_VIN_S                     0x00000001
/* support for Vinetic 4M */
#define VIN_FEAT_VIN_M                     0x00000002
/* support for Vinetic 4C */
#define VIN_FEAT_VIN_C                     0x00000004
/* support for Vinetic 2CPE */
#define VIN_FEAT_VIN_2CPE                  0x00000008
#define VIN_FEAT_VIN_ALL                   (VIN_FEAT_VIN_S | VIN_FEAT_VIN_M | \
                                            VIN_FEAT_VIN_C | VIN_FEAT_VIN_2CPE)

/* support for line testing */
#define VIN_FEAT_LT                        0x00000100
/* support for GR909 tests */
#define VIN_FEAT_GR909                     0x00000200
/* support for voice connections */
#define VIN_FEAT_VOICE                     0x00000400
/* support for FAX connections */
#define VIN_FEAT_FAX_T38                   0x00000800
/* support for packet handling (AAL/RTP) */
#define VIN_FEAT_PACKET                    0x00001000

/* ---------------------- */

#ifndef VIN_CFG_FEATURES
/* if not already set, define features depending on TAPI and other defines */

#ifdef VIN_2CPE
/* In case the driver is compiled for 2CPE, Only 2CPE features will be supported.
   => No S, M or VIP support */
#define VIN_CFG_ADD_FEAT_VIN_TYPES         VIN_FEAT_VIN_2CPE
#else
/* this is used to get a compact driver,
   that supports only Vinetic 4S with PCM connected */
#ifdef VIN_S_SUPPORT_ONLY
#define VIN_CFG_ADD_FEAT_VIN_TYPES         VIN_FEAT_VIN_S
#else
#define VIN_CFG_ADD_FEAT_VIN_TYPES         (VIN_FEAT_VIN_S | VIN_FEAT_VIN_M | \
                                            VIN_FEAT_VIN_C)
#endif /* VIN_S_SUPPORT_ONLY */
#endif /* VIN_2CPE */

#ifdef TAPI
#ifdef TAPI_VOICE
#ifdef VIN_S_SUPPORT_ONLY
#error TAPI_VOICE not possible for Vinetic S
#endif
#define VIN_CFG_ADD_FEAT_VOICE   (VIN_FEAT_VOICE | VIN_FEAT_PACKET)
#else
#define VIN_CFG_ADD_FEAT_VOICE   0
#endif

#ifdef TAPI_FAX_T38
#ifdef VIN_S_SUPPORT_ONLY
#error TAPI_FAX_T38 not possible for Vinetic S
#endif
#define VIN_CFG_ADD_FEAT_FAX_T38 (VIN_FEAT_FAX_T38 | VIN_FEAT_PACKET)
#else
#define VIN_CFG_ADD_FEAT_FAX_T38 0
#endif

#if (defined (TAPI_LT) && !defined (VIN_2CPE))
#define VIN_CFG_ADD_FEAT_LT      VIN_FEAT_LT
#else
#define VIN_CFG_ADD_FEAT_LT      0
#endif

#if defined (TAPI_GR909) && (defined(VIN_V14_SUPPORT) || defined(VIN_2CPE))
#define VIN_CFG_ADD_FEAT_GR909   VIN_FEAT_GR909
#else
#ifdef TAPI_GR909
#warning "GR909 is only available for Vinetic V1.x or CPE"
#endif
#define VIN_CFG_ADD_FEAT_GR909   0
#endif
#endif /*  TAPI */

#define VIN_CFG_FEATURES      (  VIN_CFG_ADD_FEAT_VIN_TYPES \
                              |  VIN_CFG_ADD_FEAT_VOICE     \
                              |  VIN_CFG_ADD_FEAT_FAX_T38   \
                              |  VIN_CFG_ADD_FEAT_LT        \
                              |  VIN_CFG_ADD_FEAT_GR909 )

#endif /* VIN_CFG_FEATURES */

#endif /* _DRV_API_H */

