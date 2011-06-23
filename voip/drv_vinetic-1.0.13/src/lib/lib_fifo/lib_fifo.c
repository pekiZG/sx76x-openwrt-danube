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
   Module      : lib_fifo.c
   Date        : 2003-08-16
   Description : This library implements a general FIFO functionality.
                 The FIFO stores data pointer and length information of all
                 its elements in a preallocated ring buffer of element
                 structures.
   Remarks     : The FIFO management structure contains the following
                 information:
                 pTop         - pointer to the top element's structure
                 pBottom      - pointer to the bottom element's structure
                 pGet         - pointer to the next element to be given
                 pPut         - pointer to the next element to be stored
                 fifoSize     - number of element structures in the ring buffer
                 fifoElements - number of element structures containing valid
                                data pointers and length information
                 The concept of the preallocated ring buffer allows the
                 fifoSize to grow after its initialisation while still
                 providing a very efficient element handling.
                 Note: The dynamic growth this is not implemented yet.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib_fifo.h"
/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */


/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
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


/*******************************************************************************
Description:
   Initialize the FIFO structure
Arguments:
   none
Return:
   fifo handle
*******************************************************************************/
FIFO_ID* fifoInit(const unsigned int initialElements)
{
   FIFO_ID        *pf;
   tFifoElement   *pe;

   /* Allocate memory for the FIFO management structure */
   pf = malloc(sizeof(FIFO_ID));
   if (pf == NULL)
      return NULL;
   /* Initialize the FIFO management structure and allocate memory for */
   /* the amount of elements the FIFO shall be able to handle */
   pf->fifoElements        = 0;
   pf->fifoSize            = initialElements;
   pf->pBottom             = malloc(initialElements * sizeof(tFifoElement));
   if (pf->pBottom == NULL)
   {
      free(pf);
      return (NULL);
   }
   pf->pTop                = pf->pBottom + (initialElements-1);
   /* Initialize the FIFO elements */
   pe                      = pf->pBottom;
   while (pf->pTop != pe)
   {
      pe->pNext            = pe+1;
      pe->pData            = NULL;
      pe++;
   }
   pf->pTop->pNext         = pf->pBottom;
   pf->pTop->pData         = NULL;
   pf->pPut                = pf->pBottom;
   pf->pGet                = pf->pBottom;
   return pf;
}

/*******************************************************************************
Description:
   Reset the FIFO (!! deletes ALL references to elements !!)
   This routine is not releasing the pre allocated memory for fifo elements.
   This has to be handled by the client application.
Arguments:
   pf     - a pointer to the FIFO management structure
Return:
   SUCCESS
*******************************************************************************/
int fifoReset(FIFO_ID *pf)
{
   pf->pPut                = pf->pBottom;
   pf->pGet                = pf->pBottom;
   pf->fifoElements        = 0;
   return SUCCESS;
}

/*******************************************************************************
Description:
   Add a new element to the FIFO
Arguments:
   pf     - a pointer to the FIFO management structure
   pData  - a pointer to the data to be stored in the FIFO
   len    - a length information to be stored together with the data pointer
            in the FIFO
Return:
   SUCCESS or ERROR if no more element can be added to the FIFO
*******************************************************************************/
int fifoPut(FIFO_ID *pf, const void *pData, const int len)
{
   tFifoElement *pe, *pNext;

   if (NULL == pf)
      return ERROR;

   pe    = pf->pPut;
   pNext = pe->pNext;

   /* check if the FIFO has room for a new element */
   if (pNext == pf->pGet)
      return ERROR;

   /* add the new element */
   pe->length              = len;
   pe->pData               = (void *) pData;
   pf->pPut                = pNext;
   pf->fifoElements++;
   return SUCCESS;
}

/*******************************************************************************
Description:
   Retrieve the first element from the FIFO
Arguments:
   pf     - a pointer to the FIFO management structure
   pLen   - a pointer to retrieve the stored length information
Return:
   A pointer to the data element stored in the FIFO or NULL if no more
   elements are available or the pointer to the FIFO management structure
   was NULL.
*******************************************************************************/
void* fifoGet(FIFO_ID *pf, int *pLen)
{
   void           *pData      = NULL;

   if (pf == NULL)
      return NULL;

   /* check if the FIFO has an element available */
   if (pf->pPut != pf->pGet)
   {
      pData                   = pf->pGet->pData;
      pf->pGet->pData         = NULL;
      if (pLen != NULL)
         *pLen                = pf->pGet->length;
      pf->pGet                = pf->pGet->pNext;
      pf->fifoElements--;
   }
   return pData;
}

/*******************************************************************************
Description:
   This routines frees all allocated resource. It terminates the fifo
   instance.
Arguments:
   pf     - a pointer to the FIFO management structure
Return:
   This function returns with an ERROR, if there are still elements in
   the fifo available. Please call 'fifoFree' before release the fifo.
*******************************************************************************/
int fifoFree(FIFO_ID *pf)
{
   /* check if the 'read' and the 'write' index point to the same fifo */
   /* element. Otherwise the fifo is not empty and can not be released */
   /* by this function. Please call explicite the 'fifoReset()' to     */
   /* reset the fifo before release the resources of the fifo.         */
   if (pf->pPut != pf->pGet)
   {
      return ERROR;
   }

   /* free all resources, which were allocated by the fifo */
   free(pf->pBottom);
   free(pf);

   return SUCCESS;
}

/*******************************************************************************
Description:
   get the size of the FIFO (overall number of elements)
Arguments:
   pf     - a pointer to the FIFO management structure
Return:
   the number of elements that can be max. stored in the fifo
*******************************************************************************/
unsigned int fifoSize(const FIFO_ID *pf)
{
   return(pf->fifoSize);
}

#ifdef DEBUG
/*******************************************************************************
Description:
   get the number of elements currently in the FIFO
Arguments:
   pf     - a pointer to the FIFO management structure
Return:
   the number of elements currently in the FIFO
*******************************************************************************/
unsigned int fifoElements(const FIFO_ID *pf)
{
   return(pf->fifoElements);
}

/*******************************************************************************
Description:
   Debug function
   Check the integrity of the FIFO by going through the linked list of
   FIFO elements checking that the number of linked elements equals the number
   of elements in the FIFO management structure.
Arguments:
   pf     - a pointer to the FIFO management structure
Return:
   SUCCESS or
   -n    - integrity error (negative number of cnt)
*******************************************************************************/
int fifoIntegrity(const FIFO_ID *pf)
{
   tFifoElement *pe;
   unsigned int  cnt = 0;

   pe = pf->pGet;
   while (pe != pf->pPut)
   {
      pe = pe->pNext;
      cnt++;
   }
   if (cnt != pf->fifoElements) {
      printf("ERROR fifoIntegrity fifoElements %d != cnt %d\n",
              pf->fifoElements, cnt);
      return (-cnt);
   }
   cnt = 0;
   pe = pf->pBottom;
   do
   {
      pe = pe->pNext;
      cnt++;
   } while (pe != pf->pBottom);
   if (cnt != pf->fifoSize) {
      printf("ERROR fifoIntegrity fifoSize %d != cnt %d\n",
              pf->fifoSize, cnt);
      return (-cnt);
   }

   return SUCCESS;
}
#endif /* DEBUG */
