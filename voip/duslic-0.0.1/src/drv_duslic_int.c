/****************************************************************************
     Copyright (c) 2002-2004, Infineon Technologies.  All rights reserved.

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
   Module      : drv_duslic_int.c
   Date        : 2005-7-19
   Description : This file contains the implementation of the interrupt handler
   Remarks     : The implementation assumes that multiple instances of this
                 interrupt handler cannot preempt each other, i.e. if there
                 is more than one DUSLIC in your design all DUSLICs are
                 expected to raise interrupts at the same priority level.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "drv_duslic_api.h"
#include "drv_board.h"
#include "drv_duslic_int.h"
#include "drv_duslic_main.h"




/* ============================= */
/* Global function definition    */
/* ============================= */

/*******************************************************************************
Description:
   Channel interrupt handler routine for off-hook detection
Arguments:
   pCh - pointer to the duslic channel 
Return:
   IFX_ERR if handling error, IFX_SUCCES if success
Remarks:
   modify this function to meet specific user requirement
*******************************************************************************/
IFX_uint32_t Off_hook_detected(DUSLIC_CHANNEL* pCh)
{
  IFX_uint32_t err=IFX_ERR;

  return err; 
}





/*******************************************************************************
Description:
   DUSLIC Interrupt Rountine
Arguments:
   pDev - pointer to the device interface
Return:
   none
Remarks:
   If real time interrupt routine is used, serve real time events
   here according to flags set in real time interrupt routine
*******************************************************************************/
void DUSLIC_interrupt_routine(DUSLIC_DEVICE *pDev)
{

    int i;
    int int_flag=1;
    DUSLIC_CHANNEL* pCh;
    IFX_uint8_t INTREG[5];
    
//    printk("DUSLIC_interrupt_routine !!!!!!!!!!!!\n");
    for(i=0; i<DUSLIC_MAX_CHANNELS; i++)
    {
	pCh=&pDev->pChannel[i];
	while(1)
        {
	    pCh->ReadReg(pCh, SOP_CMD, INTREG1, &INTREG[1], 1);
	  
		 
           if(INTREG[1] & INTREG1_INT_CH) 
	   {
	    /*read out all the 4 interrupt registers first*/
	     pCh->ReadReg(pCh, SOP_CMD, INTREG1, &INTREG[1], 4);
//	     printk("INTREG[1,2] = %02x %02x %02x %02x\n",INTREG[1],INTREG[2],INTREG[3],INTREG[4]);

	     if(INTREG[1] & INTREG1_HOOK) Off_hook_detected(pCh);
	    	     DUSLIC_GpioIntDispatch((INT)pCh, INTREG[2] & 0x0f );
	   }
	   else
	   {
	   	break;
	   }
	 }
    }

    return;
}

