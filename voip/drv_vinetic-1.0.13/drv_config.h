/* drv_config.h.  Generated by configure.  */
/* drv_config.h.in.  Generated from configure.in by autoheader.  */

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
 ******************************************************************************/
#ifndef _DRV_CONFIG_H_
#define _DRV_CONFIG_H_


/* enable debug features as asserts for example */
/* #undef DEBUG */

/* Enable trace outputs in general. */
#define ENABLE_TRACE 1

/* Enable user configuration. */
#define ENABLE_USER_CONFIG 1

/* Name of package */
#define PACKAGE "drv_vinetic"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "VINETIC Device Driver"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "VINETIC Device Driver 1.0.13"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "drv_vinetic"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.0.13"

/* Enable runtime traces as register access. */
#define RUNTIME_TRACE 1

/* Enable TAPI CID support. */
#define TAPI_CID 1

/* Enable TAPI DTMF support. */
#define TAPI_DTMF 1

/* Enable TAPI FAX support. */
#define TAPI_FAX_T38 1

/* Enable GR909 tests. */
/* #undef TAPI_GR909 */

/* Enable line testing. */
/* #undef TAPI_LT */

/* Enable Voice support. */
#define TAPI_VOICE 1

/* Version number of package */
#define VERSION "1.0.13"

/* Maximum VINETIC devices to support */
#define VINETIC_MAX_DEVICES 1

/* Enable use of proc filesystem entries (Linux only) */
#define VINETIC_USE_PROC 1

/* Enable 8 Bit bus access. */
/* #undef VIN_8BIT */

/* Enable including default firmware. */
#define VIN_DEFAULT_FW 1

/* Enable SPI interface, needs user configuration. */
#define VIN_SPI 1

/* Enable support of Vinetic version V1.x. */
#define VIN_V14_SUPPORT 1

/* Enable support of Vinetic version V2.x. */
#define VIN_V21_SUPPORT 1


#endif /* _DRV_CONFIG_H_ */

