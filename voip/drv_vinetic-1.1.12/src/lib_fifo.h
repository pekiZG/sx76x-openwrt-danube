#ifndef _LIB_FIFO_H
#define _LIB_FIFO_H
/****************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

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
   Module      : lib_fifo.h
   Date        : 2003-08-16
   Description : see .c file
   Remarks     :
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

/* ============================= */
/* Global Defines                */
/* ============================= */
#ifndef ERROR
#define ERROR -1
#endif
#ifndef SUCCESS
#define SUCCESS 0
#endif

typedef struct _tFifoElement {
   struct _tFifoElement *pNext;
   void                 *pData;
   int                  length;
} tFifoElement;

typedef struct _FIFO_ID {
   tFifoElement         *pTop;
   tFifoElement         *pBottom;
   tFifoElement         *pPut;
   tFifoElement         *pGet;
   unsigned int         fifoSize;
   unsigned int         fifoElements;
} FIFO_ID;

FIFO_ID*      fifoInit      (const unsigned int initialElements);
void*         fifoGet       (FIFO_ID *pf, int *pLen);
int           fifoPut       (FIFO_ID *pf, const void *pData, const int len);

int           fifoReset     (FIFO_ID *pf);
int           fifoFree      (FIFO_ID *pf);
unsigned int  fifoSize      (const FIFO_ID *pf);

#ifdef DEBUG
unsigned int  fifoElements  (const FIFO_ID *pf);
int           fifoIntegrity (const FIFO_ID *pf);
#endif /* DEBUG */

#endif /* _LIB_FIFO_H */
