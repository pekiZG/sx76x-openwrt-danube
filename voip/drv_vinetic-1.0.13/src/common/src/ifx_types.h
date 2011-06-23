#ifndef _IFX_TYPES_H
#define _IFX_TYPES_H
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

/**
   This type has two states, success and error
*/
typedef enum {
   IFX_ERROR   = (-1),
   IFX_SUCCESS = 0
} IFX_return_t;

#define IFX_NULL         ((void *)0)

#endif /* _IFX_TYPES_H */

