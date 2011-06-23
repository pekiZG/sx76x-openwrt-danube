/******************************************************************************
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
 ******************************************************************************
   Module      : sys_drv_fifo.c
   Date        : 02.12.01
   Description : Fifo implementation
   Remarks     :
      Initialize with Fifo_Init with previosly allocated memory.
      The functions Fifo_writeElement and Fifo_readElement return a pointer
      to the next element to be written or read respectively.
      If empty Fifo_readElement returns NULL. If full Fifo_writeElement
      returns NULL.
 *****************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_api.h"
#include "sys_drv_fifo.h"

/* ============================= */
/* Local Macros  Definitions    */
/* ============================= */

#define INCREMENT_INDEX(p)       \
{                                \
   if (p == pFifo->pEnd)         \
      p = pFifo->pStart;         \
   else                          \
      p = (void*)(((unsigned int)p) + pFifo->size); \
}

#define DECREMENT_INDEX(p)       \
{                                \
   if (p == pFifo->pStart)       \
      p = pFifo->pEnd;           \
   else                          \
      p = (void*)(((unsigned int)p) - pFifo->size); \
}


/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* ============================= */
/* Global function definition    */
/* ============================= */


/*******************************************************************************
Description	:
   Initializes the fifo structure
Result		:
   Always zero, otherwise error
Arguments   :
   *pFifo - Pointer to the Fifo structure
   *pStart - Pointer to the fifo start memory
   *pEnd - Pointer to the fifo end memory
   elSize - size of each element in bytes
*******************************************************************************/
int Fifo_Init (FIFO* pFifo, void* pStart, void* pEnd, int elSize)
{
   pFifo->pEnd   = pEnd;
   pFifo->pStart = pStart;
   pFifo->pRead  = pStart;
   pFifo->pWrite = pStart;
   pFifo->size   = elSize;
   pFifo->count  = 0;

   if (((unsigned int)pFifo->pEnd - (unsigned int)pFifo->pStart) % elSize != 0)
   {
      /* element size must be a multiple of fifo memory */
      return -1;
   }
   pFifo->max_size = ((unsigned int)pFifo->pEnd - (unsigned int)pFifo->pStart) / elSize + 1;

   return 0;
}


/*******************************************************************************
Description	:
   Clear the fifo
Arguments   :
   *pFifo - Pointer to the Fifo structure
*******************************************************************************/
void Fifo_Clear (FIFO *pFifo)
{
   pFifo->pRead  = pFifo->pStart;
   pFifo->pWrite = pFifo->pStart;
   pFifo->count = 0;
}

/*******************************************************************************
Description	:
   Get the next element to read from
Result		:
   Returns the element to read from, or NULL if no element available or an
   error occured
Arguments   :
   *pFifo - Pointer to the Fifo structure
Remarks		:
   Error occurs if fifo is empty
*******************************************************************************/
void* Fifo_readElement (FIFO *pFifo)
{
   void *ret = NULL;

   if (pFifo->count == 0)
   {
      return NULL;
   }
   ret = pFifo->pRead;

   INCREMENT_INDEX(pFifo->pRead);
   pFifo->count--;

   return ret;
}

/*******************************************************************************
Description	:
   Delivers empty status
Result		:
   Returns TRUE if empty (no data available)
Arguments   :
   *pFifo - Pointer to the Fifo structure
Remarks		:
   No change on fifo!
*******************************************************************************/
int Fifo_isEmpty (FIFO *pFifo)
{
   return (pFifo->count == 0);
}

/*******************************************************************************
Description	:
   Get the next element to write to
Result		:
   Returns the element to write to, or NULL in case of error
Arguments   :
   *pFifo - Pointer to the Fifo structure
Remarks		:
   Error occurs if the write pointer reaches the read pointer, meaning
   the fifo if full and would otherwise overwrite the next element.
*******************************************************************************/
void* Fifo_writeElement (FIFO *pFifo)
{
   void* ret = NULL;

   if (Fifo_isFull(pFifo))
      return NULL;
   else
   {
      /* get the next entry of the Fifo */
      ret = pFifo->pWrite;
      INCREMENT_INDEX(pFifo->pWrite);
      pFifo->count++;
      return ret;
   }
}

/*******************************************************************************
Description	:
   Delivers full status
Result		:
   Returns TRUE if full (overflow on next write)
Arguments   :
   *pFifo - Pointer to the Fifo structure
Remarks		:
   No change on fifo!
*******************************************************************************/
int Fifo_isFull (FIFO *pFifo)
{
   return (pFifo->count >= pFifo->max_size);
}


/*******************************************************************************
Description	:
   Returns the last element taken back to the fifo
Arguments   :
   *pFifo - Pointer to the Fifo structure
Remarks		:
   Makes an undo to a previosly Fifo_writeElement
*******************************************************************************/
void Fifo_returnElement (FIFO *pFifo)
{
   DECREMENT_INDEX(pFifo->pWrite);
   pFifo->count--;
}

/*******************************************************************************
Description:
   Get the number of stored elements
Arguments:
   *pFifo - Pointer to the Fifo structure
Return:
   Number of containing elements
*******************************************************************************/
INT Fifo_getCount(FIFO *pFifo)
{
   return pFifo->count;
}

/*
void FifoTest (VINETIC_DEVICE* pDev)
{
   WORD buf [3];
   WORD tst = 1;
   WORD *entry = NULL;
   FIFO fifo;

   Fifo_Init (&fifo, &buf[0], &buf[2], sizeof (WORD));

   entry = Fifo_writeElement (&fifo);
   if (entry != NULL)
      *entry = 1;
   entry = Fifo_writeElement (&fifo);
   if (entry != NULL)
   {
      *entry = 2;
      Fifo_returnElement (&fifo);
   }
   entry = Fifo_readElement (&fifo);
   if (entry != NULL)
      tst = *entry;
   entry = Fifo_readElement (&fifo);
   if (entry != NULL)
      tst = *entry;
}
*/
