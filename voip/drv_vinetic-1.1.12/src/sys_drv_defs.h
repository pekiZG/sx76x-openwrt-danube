/****************************************************************************
                 Copyright (c) 2005  Infineon Technologies AG
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
   Module      : sys_drv_defs.h
   Date        : 2004-07-12
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
#ifndef WIN32
typedef void                     VOID;
#endif

typedef unsigned char            UCHAR;

typedef signed char              INT8;
typedef unsigned char            UINT8;
typedef signed short             INT16;
typedef unsigned short           UINT16;
#ifndef WIN32
typedef signed int               INT32;
#endif /* WIN32 */
typedef volatile INT8            VINT8;
typedef volatile UINT8           VUINT8;
typedef volatile INT16           VINT16;
typedef volatile UINT16          VUINT16;
#ifndef WIN32
typedef volatile INT32           VINT32;
typedef volatile UINT32          VUINT32;

typedef INT                      (*FUNCPTR)     (VOID);
typedef VOID                     (*VOIDFUNCPTR) (VOID);
#endif


#endif /* VXWORKS */

#ifndef basic_types
#define basic_types
typedef signed char              int8;
typedef unsigned char            uint8;
typedef signed short             int16;
typedef unsigned short           uint16;
typedef signed int               int32;
typedef unsigned int             uint32;
#endif /* basic_types */

#ifdef NEED_64BIT_TYPES
#ifndef WIN32
typedef signed long long         INT64;
typedef unsigned long long       UINT64;
#endif /* WIN32 */

#ifndef WIN32
typedef signed long long         int64;
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

