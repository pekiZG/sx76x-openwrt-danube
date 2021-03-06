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
   Module      : $RCSfile: drv_cpc5621_vxworks_bsp.c,v $
   Date        : $Date: 2004/07/27 09:46:06 $
   Description : Template Driver, VxWorks BSP part
******************************************************************************/

/* ============================= */
/* Group=Main                    */
/* ============================= */


#if defined(VXWORKS) && defined(INCLUDE_CPC5621)
/*
  sorry for this style, but this is a easy way to include the driver code
  into the board support package of VxWorks
*/
#include "drv_api.h"
  #include "drv_cpc5621_api.h"
  #include "drv_cpc5621_vxworks.c"
  #include "drv_cpc5621_common.c"

#endif

