/******************************************************************************
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
 ******************************************************************************
   Module      : sys_drv_fifo.c
   Description : Fifo implementation
   Remarks     :
      Initialize with Fifo_Init with previosly allocated memory.
      The functions Fifo_writeElement and Fifo_readElement return a pointer
      to the next element to be written or read respectively.
      If empty Fifo_readElement returns IFX_NULL. If full Fifo_writeElement
      returns IFX_NULL.
 *****************************************************************************/


/* ============================= */
/* Includes                      */
/* ============================= */

#include "sys_drv_fifo.h"

/* ============================= */
/* Local Macros  Definitions    */
/* ============================= */

/** increment FIFO index */
#define INCREMENT_INDEX(p)       \
{                                \
   if (p == pFifo->pEnd)         \
      p = pFifo->pStart;         \
   else                          \
      p = (IFX_void_t*)(((unsigned int)p) + pFifo->size); \
}

/** decrement FIFO index */
#define DECREMENT_INDEX(p)       \
{                                \
   if (p == pFifo->pStart)       \
      p = pFifo->pEnd;           \
   else                          \
      p = (IFX_void_t*)(((unsigned int)p) - pFifo->size); \
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


/**
   Initializes the fifo structure
   \param *pFifo - Pointer to the Fifo structure
   \param *pStart - Pointer to the fifo start memory
   \param *pEnd - Pointer to the fifo end memory
   \param elSize - size of each element in bytes
   \return
   Always zero, otherwise error
*/
IFX_int8_t Fifo_Init (FIFO* pFifo, IFX_void_t* pStart, IFX_void_t* pEnd, IFX_uint32_t elSize)
{
   pFifo->pEnd   = pEnd;
   pFifo->pStart = pStart;
   pFifo->pRead  = pStart;
   pFifo->pWrite = pStart;
   pFifo->size   = elSize;
   pFifo->count  = 0;

   if (((IFX_uint32_t)pFifo->pEnd - (IFX_uint32_t)pFifo->pStart) % elSize != 0)
   {
      /* element size must be a multiple of fifo memory */
      return -1;
   }
   pFifo->max_size = ((IFX_uint32_t)pFifo->pEnd - (IFX_uint32_t)pFifo->pStart) / elSize + 1;

   return 0;
}

/**
   Clear the fifo
   \param *pFifo - Pointer to the Fifo structure
*/
IFX_void_t Fifo_Clear (FIFO *pFifo)
{
   pFifo->pRead  = pFifo->pStart;
   pFifo->pWrite = pFifo->pStart;
   pFifo->count = 0;
}

/**
   Get the next element to read from
   \param *pFifo - Pointer to the Fifo structure
   \return
   Returns the element to read from, or IFX_NULL if no element available or an error occured
   \remark
   Error occurs if fifo is empty
*/
IFX_void_t* Fifo_readElement (FIFO *pFifo)
{
   IFX_void_t *ret = IFX_NULL;

   if (pFifo->count == 0)
   {
      return IFX_NULL;
   }
   ret = pFifo->pRead;

   INCREMENT_INDEX(pFifo->pRead);
   pFifo->count--;

   return ret;
}

/**
   Delivers empty status
   \param *pFifo - Pointer to the Fifo structure
   \return
   Returns TRUE if empty (no data available)
   \remark
   No change on fifo!
*/
IFX_int8_t Fifo_isEmpty (FIFO *pFifo)
{
   return (pFifo->count == 0);
}

/**
   Get the next element to write to
   \param *pFifo - Pointer to the Fifo structure
   \return
   Returns the element to write to, or IFX_NULL in case of error
   \remark
   Error occurs if the write pointer reaches the read pointer, meaning
   the fifo if full and would otherwise overwrite the next element.
*/
IFX_void_t* Fifo_writeElement (FIFO *pFifo)
{
   IFX_void_t* ret = IFX_NULL;

   if (Fifo_isFull(pFifo))
      return IFX_NULL;
   else
   {
      /* get the next entry of the Fifo */
      ret = pFifo->pWrite;
      INCREMENT_INDEX(pFifo->pWrite);
      pFifo->count++;
      return ret;
   }
}

/**
   Delivers full status
   \param *pFifo - Pointer to the Fifo structure
   \return
   TRUE if full (overflow on next write)
   \remark
   No change on fifo!
*/
IFX_int8_t Fifo_isFull (FIFO *pFifo)
{
   return (pFifo->count >= pFifo->max_size);
}

/**
   Returns the last element taken back to the fifo
   \param *pFifo - Pointer to the Fifo structure
   \remark
   Makes an undo to a previosly Fifo_writeElement
*/
IFX_void_t Fifo_returnElement (FIFO *pFifo)
{
   DECREMENT_INDEX(pFifo->pWrite);
   pFifo->count--;
}

/**
   Get the number of stored elements
   \param *pFifo - Pointer to the Fifo structure
   \return
   Number of containing elements
*/
IFX_uint32_t Fifo_getCount(FIFO *pFifo)
{
   return pFifo->count;
}

#if 0
/**
   test routine
*/
IFX_void_t FifoTest(IFX_void_t)
{
   IFX_uint16_t buf [3];
   IFX_uint16_t tst = 1;
   IFX_uint16_t *entry = IFX_NULL;
   FIFO fifo;

   Fifo_Init (&fifo, &buf[0], &buf[2], sizeof (IFX_uint16_t));

   entry = Fifo_writeElement (&fifo);
   if (entry != IFX_NULL)
      *entry = 1;
   entry = Fifo_writeElement (&fifo);
   if (entry != IFX_NULL)
   {
      *entry = 2;
      Fifo_returnElement (&fifo);
   }
   entry = Fifo_readElement (&fifo);
   if (entry != IFX_NULL)
      tst = *entry;
   entry = Fifo_readElement (&fifo);
   if (entry != IFX_NULL)
      tst = *entry;
}
#endif
