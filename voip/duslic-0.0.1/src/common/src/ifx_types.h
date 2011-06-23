#ifndef _IFX_TYPES_H
#define _IFX_TYPES_H
/****************************************************************************
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
 ****************************************************************************
   Module      : ifx_types.h
   Date        : 2004-05-21
*******************************************************************************/

/** \file
   Basic data types.
*/

/** This is the chracter datatype. */
typedef char            IFX_char_t;
/** This is the unsigned 8-bit datatype. */
typedef unsigned char   IFX_uint8_t;
/** This is the signed 8-bit datatype. */
typedef signed char     IFX_int8_t;
/** This is the unsigned 16-bit datatype. */
typedef unsigned short  IFX_uint16_t;
/** This is the signed 16-bit datatype. */
typedef signed short    IFX_int16_t;
/** This is the unsigned 32-bit datatype. */
typedef unsigned long   IFX_uint32_t;
/** This is the signed 32-bit datatype. */
typedef signed long     IFX_int32_t;
/** This is the float datatype. */
typedef float           IFX_float_t;
/** This is the void datatype. */
typedef void            IFX_void_t;

/** This is the volatile unsigned 8-bit datatype. */
typedef volatile IFX_uint8_t  IFX_vuint8_t;
/** This is the volatile signed 8-bit datatype. */
typedef volatile IFX_int8_t   IFX_vint8_t;
/** This is the volatile unsigned 16-bit datatype. */
typedef volatile IFX_uint16_t IFX_vuint16_t;
/** This is the volatile signed 16-bit datatype. */
typedef volatile IFX_int16_t  IFX_vint16_t;
/** This is the volatile unsigned 32-bit datatype. */
typedef volatile IFX_uint32_t IFX_vuint32_t;
/** This is the volatile signed 32-bit datatype. */
typedef volatile IFX_int32_t  IFX_vint32_t;
/** This is the volatile float datatype. */
typedef volatile IFX_float_t  IFX_vfloat_t;


/* A type for handling boolean issues. */
typedef enum {
   /** false */
	IFX_FALSE = 0,
   /** true */
   IFX_TRUE = 1
} IFX_boolean_t;


/**
   This type is used for parameters that should enable
   and disable a dedicated feature. */
typedef enum {
   /** disable */
	IFX_DISABLE = 0,
   /** enable */
	IFX_ENABLE = 1
} IFX_enDis_t;


/**
   This type has two states, even and odd.
*/
typedef enum {
   /** even */
	IFX_EVEN = 0,
   /** odd */
	IFX_ODD = 1
} IFX_evenOdd_t;


/**
   This type has two states, high and low.
*/
typedef enum {
	 IFX_LOW = 0,
	 IFX_HIGH = 1
} IFX_highLow_t;

#endif /* _IFX_TYPES_H */

