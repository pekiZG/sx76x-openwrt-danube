#ifndef _SYS_DRV_FIFO_H
#define _SYS_DRV_FIFO_H
/******************************************************************************
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
 ******************************************************************************
   Module      :
   Date        : 26.01.02
   Description : Fifo definitions and declarations.
 *****************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */

/* ============================= */
/* Local Macros  Definitions    */
/* ============================= */

typedef struct
{
   void* pStart;
   void* pEnd;
   void* pRead;
   void* pWrite;
   unsigned int  size;
   /* element count, changed on read and write: */
   volatile int count;
   int max_size; 
}FIFO;


/* ============================= */
/* Global function declaration   */
/* ============================= */

extern int   Fifo_Init          (FIFO* pFifo, void* pStart, void* pEnd, int size);
extern void  Fifo_Clear         (FIFO *pFifo);
extern void* Fifo_readElement   (FIFO *pFifo);
extern void* Fifo_writeElement  (FIFO *pFifo);
extern void  Fifo_returnElement (FIFO *pFifo);
extern int   Fifo_isEmpty       (FIFO *pFifo);
extern int   Fifo_isFull        (FIFO *pFifo);

#endif
