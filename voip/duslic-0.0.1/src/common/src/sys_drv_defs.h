/****************************************************************************
       Copyright (c) 2003, Infineon Technologies.  All rights reserved.

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
   Module      : $RCSfile: sys_drv_defs.h,v $
   Date        : $Date: 2004/10/07 03:53:32 $
   Description : This file contains definitions of some basic type definitions.
*******************************************************************************/

#ifndef _SYS_DRV_DEFS_H
#define _SYS_DRV_DEFS_H

/* ============================= */
/* Global Defines                */
/* ============================= */

#if defined (__GNUC__) || defined (__GNUG__)
/* GNU C or C++ compiler */
#undef __PACKED__
#define __PACKED__ __attribute__ ((packed))
#elif !defined (__PACKED__)
#define __PACKED__ /* nothing */
#endif

#ifndef VXWORKS
#define LOCAL                    static
#define IMPORT                   extern
#endif /* VXWORKS */

#ifndef PUBLIC
#define PUBLIC                   extern
#endif

#ifndef SUCCESS
#define SUCCESS                  0
#endif

#ifndef ERR
#define ERR                      -1
#endif

#ifndef NULL
#define NULL                      0
#endif

/* typedefs */
typedef unsigned char            BYTE;
typedef char                     CHAR;
typedef int                      INT;
typedef float                    FLOAT;
typedef short                    SHORT;

#ifndef WIN32
/* WORD must be 16 bit */
typedef unsigned short           WORD;
/* WORD must be 32 bit */
typedef unsigned long            DWORD;
#endif /* WIN32 */

#ifndef VXWORKS
typedef unsigned int             UINT;
#ifndef WIN32
/* DWORD must be 32 bit */
typedef unsigned int             UINT32;
#endif /* WIN32 */
#endif /* VXWORKS */


#ifndef VXWORKS
typedef void                     VOID;

typedef unsigned char            UCHAR;

typedef char                     INT8;
typedef unsigned char            UINT8;
typedef short                    INT16;
typedef unsigned short           UINT16;
#ifndef WIN32
typedef int                      INT32;
#endif /* WIN32 */
typedef volatile INT8            VINT8;
typedef volatile UINT8           VUINT8;
typedef volatile INT16           VINT16;
typedef volatile UINT16          VUINT16;
#ifndef WIN32
typedef volatile INT32           VINT32;
typedef volatile UINT32          VUINT32;
#endif

typedef INT                      (*FUNCPTR)     (VOID);
typedef VOID                     (*VOIDFUNCPTR) (VOID);

#endif /* VXWORKS */

#ifndef basic_types
#define basic_types
typedef char                     int8;
typedef unsigned char            uint8;
typedef short                    int16;
typedef unsigned short           uint16;
typedef int                      int32;
typedef unsigned int             uint32;
#endif /* basic_types */

#ifdef NEED_64BIT_TYPES
#ifndef WIN32
typedef long long                INT64;
typedef unsigned long long       UINT64;
#endif /* WIN32 */

#ifndef WIN32
typedef long long                int64;
typedef unsigned long long       uint64;
#endif
#endif

#ifndef __cplusplus
typedef enum {false, true} bool;
#endif

#if !(defined VXWORKS) && !(defined WIN32)
#ifndef BOOL
#ifdef FALSE
#undef FALSE
#endif /* FALSE */
#ifdef TRUE
#undef TRUE
#endif /* TRUE */
typedef enum {FALSE,TRUE}  BOOL;
#endif /* BOOL */
#endif /* !(defined VXWORKS) && !(defined WIN32) */

#ifndef _MKSTR_1
#define _MKSTR_1(x)    #x
#define _MKSTR(x)      _MKSTR_1(x)
#endif

#endif /* _SYS_DRV_DEFS_H */

