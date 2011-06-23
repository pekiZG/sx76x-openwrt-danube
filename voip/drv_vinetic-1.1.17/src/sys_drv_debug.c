#ifndef _sys_drv_debug_c
#define _sys_drv_debug_c

#ifdef DEBUG
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

   EXCEPT FOR ANY LIABILITY DUE TO WILLFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : sys_drv_debug.c
   Date        : 2004/07/12 12:10:33
   Description : system debug interface
 ***************************************************************************/
/** \file
SYS DRIVER DEBUG: system debug interface
*/


#include "sys_drv_ifxos.h"
#include "sys_drv_debug.h"
/** maximum allowed allocations */
#define MAX_MEMALLOC 100

/** control structure for debug memory allocation */
typedef struct
{
   int line;
   int size;
   void *p;
   char sFile[30];
}DBG_MEM;

/** Debug memory alloc: contains actual debug informations */
static DBG_MEM G_Mem[MAX_MEMALLOC];
/** Debug memory alloc: current G_Mem index */
static int G_nCnt = 0;
/** Debug memory alloc: maximum index counter */
static int G_nMaxCnt = 0;
/** Debug memory alloc: maximum memory usage counter */
static int G_nMaxUse = 0;
/** Debug memory alloc: current memory usage */
static int G_inUse= 0;

/**
   Allocates a memory block.

\param
      size - desired memory size
\param
      line - code line
\param
      sFile - file name

\remarks
   Increases the usage counter and add the block to the list.
   Also manipulates the maximum usage counter
*/
void * sys_dbgMalloc(int size, int line, const char* sFile)
{
   void * p;
   int i;
   p = sysdebug_malloc(size);
   if (p != NULL)
   {
      IFXOS_ASSERT (G_nCnt < MAX_MEMALLOC);
      G_inUse += size;
      G_Mem[G_nCnt].line  = line;
      G_Mem[G_nCnt].size  = size;
      G_Mem[G_nCnt].p     = p;
      i = strlen(sFile);
      while (i > 0)
      {
         if (sFile[i] == '/' || sFile[i] == '\\')
            break;
         i--;
      }
      strcpy (G_Mem[G_nCnt].sFile, (char*)&(sFile[i]));
      G_nCnt++;
      /* set maximum values */
      if (G_nCnt > G_nMaxCnt)
         G_nMaxCnt = G_nCnt;
      if (G_inUse > G_nMaxUse)
         G_nMaxUse = G_inUse;
   }
   return p;
}

/**
  Free a memory block

\param
      pBuf  - handle to the buffer to be freed
\param
      line  - Caller line number
\param
      sFile - Caller File name
\remarks
   Decreases the usage counter and removes the block from the list
*/
void sys_dbgFree (void *pBuf, int line, const char* sFile)
{
   int i;
   if (pBuf == NULL)
   {
      PRINTF ("free on NULL pointer, %s, line %d\n\r", sFile, line);
      return;
   }
   /* find pointer in list to be removed */
   for (i=0; i < G_nCnt; i++)
   {
      if (G_Mem[i].p == pBuf)
      {
         /* reduce size counter */
         G_inUse -= G_Mem[i].size;
         G_nCnt--;
         /* copy last element to current location and mark as free */
         if (i != G_nCnt)
            memcpy (&G_Mem[i], &G_Mem[G_nCnt], sizeof (DBG_MEM));
         break;
      }
   }
   sysdebug_free(pBuf);
   pBuf = NULL;
}

/**
  Print out all debug results
*/
void dbgResult()
{
   int i;
   PRINTF ("Max. Mem Usage: %d, %d Allocations, %d/%d Left\n\r",
            G_nMaxUse, G_nMaxCnt, G_inUse, G_nCnt);
   if (G_nCnt != 0)
   {
      PRINTF ("File\t\t\tLine\tSize\n\r");
      for (i=0; i < G_nCnt; i++)
      {
         PRINTF ("%s\t%d\t%d\n\r",
               G_Mem[i].sFile, G_Mem[i].line, G_Mem[i].size);
      }
   }
}
#endif /* DEBUG */

#endif /* _sys_drv_debug_c */
