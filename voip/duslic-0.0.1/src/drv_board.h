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
   Module      : drv_board.h
   Date        : 2005-7-5
   Description :
      Generic file for all board specific global
      functions. Includes the specific file of our board.
      This file should always included to gain board access. That
      keeps the including file board independend.
  
*******************************************************************************/
/* ============================= */
/* Global Includes               */
/* ============================= */

extern void Board_InitIrq        (int nIrq);
extern void Board_AckIrq         (int nIrq);
extern int  Board_Init           (DUSLIC_DEVICE *pDev);
extern void Board_InitPlatform   (void);
extern void Board_ResetPlatform  (void);
extern int  Board_DevOpen        (int nDevn);

/* these function are board specific, althougth they use OS methods */
/* Its implementation is done in the OS file */
extern void OS_Install_DuslicIRQHandler (void* pDevice, int nIrq);


#if defined(AMAZON)
	#include "drv_duslic_amazon.h"	
#endif
