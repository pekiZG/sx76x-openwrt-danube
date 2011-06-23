#ifndef _SYS_DRV_FIFO_H
#define _SYS_DRV_FIFO_H
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
   Module      : sys_drv_fifo.h
   Description : Fifo definitions and declarations.
 *****************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */
#include "ifx_types.h"

/* ============================= */
/* Local Macros  Definitions    */
/* ============================= */

/**
   FIFO data structure
*/
typedef struct
{
   /** start pointer of FIFO buffer */
   IFX_void_t* pStart;
   /** end pointer of FIFO buffer */
   IFX_void_t* pEnd;
   /** read pointer of FIFO buffer */
   IFX_void_t* pRead;
   /** write pointer of FIFO buffer */
   IFX_void_t* pWrite;
   /** element size */
   IFX_uint32_t size;
   /** element count, changed on read and write: */
   IFX_vuint32_t count;
   /** maximum of FIFO elements */
   IFX_uint32_t max_size;
} FIFO;


/* ============================= */
/* Global function declaration   */
/* ============================= */

extern IFX_int8_t  Fifo_Init (FIFO* pFifo, IFX_void_t* pStart,
                              IFX_void_t* pEnd, IFX_uint32_t size);
extern IFX_void_t  Fifo_Clear (FIFO *pFifo);
extern IFX_void_t* Fifo_readElement (FIFO *pFifo);
extern IFX_void_t* Fifo_writeElement (FIFO *pFifo);
extern IFX_void_t  Fifo_returnElement (FIFO *pFifo);
extern IFX_int8_t  Fifo_isEmpty (FIFO *pFifo);
extern IFX_int8_t  Fifo_isFull (FIFO *pFifo);
extern IFX_uint32_t Fifo_getCount(FIFO *pFifo);

#endif
