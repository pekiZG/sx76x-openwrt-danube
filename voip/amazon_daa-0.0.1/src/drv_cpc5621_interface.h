#ifndef _DRV_CPC5621_INTERFACE_H
#define _DRV_CPC5621_INTERFACE_H
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
   Module      : $RCSfile: drv_cpc5621_interface.h,v $
   Date        : $Date: 2004/08/03 14:33:02 $
   Description : Interface to user application of the Template Driver
******************************************************************************/

/** magic number */
#define CPC5621_IOC_MAGIC 'C'

/** changes the level of debug outputs */
#define FIO_CPC5621_DEBUGLEVEL                  _IO(CPC5621_IOC_MAGIC, 1)

/** returns the version information */
#define FIO_CPC5621_GET_VERSION                 _IO(CPC5621_IOC_MAGIC, 2)


/** init CPC5621 device */
#define FIO_CPC5621_INIT                        _IO(CPC5621_IOC_MAGIC, 10)

/** config CPC5621 device */
#define FIO_CPC5621_CONFIG                      _IO(CPC5621_IOC_MAGIC, 11)

/** set state of OH pin */
#define FIO_CPC5621_SET_OH                      _IO(CPC5621_IOC_MAGIC, 12)

/** set state of CID pin */
#define FIO_CPC5621_SET_CID                     _IO(CPC5621_IOC_MAGIC, 13)

/** get state of OH pin */
#define FIO_CPC5621_GET_OH                      _IO(CPC5621_IOC_MAGIC, 14)

/** get state of CID pin */
#define FIO_CPC5621_GET_CID                     _IO(CPC5621_IOC_MAGIC, 15)

/** get state of Ring pin */
#define FIO_CPC5621_GET_RING                    _IO(CPC5621_IOC_MAGIC, 16)

/** get state of BATTERY DETECT pin */
#define FIO_CPC5621_GET_BAT_DETECT              _IO(CPC5621_IOC_MAGIC, 17)

/** get state of POLARITY DETECT pin */
#define FIO_CPC5621_GET_POL_DETECT              _IO(CPC5621_IOC_MAGIC, 18)

/** get state of exception */
#define FIO_CPC5621_EXCEPTION                   _IO(CPC5621_IOC_MAGIC, 19)




/** @{
 \ingroup  */
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 1
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 2
#endif

#define __BYTE_ORDER __BIG_ENDIAN
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
//#warning __BYTE_ORDER == __LITTLE_ENDIAN
typedef struct
{
   unsigned int ring:1;
   unsigned int battery:1;
   unsigned int polarity:1;
   unsigned int cidTimeOut:1;
   unsigned int flag:1;
   unsigned int reserved2:27;
} CPC5621_ECP_BITS;
#elif (__BYTE_ORDER == __BIG_ENDIAN)
//#warning __BYTE_ORDER == __BIG_ENDIAN
typedef struct
{
   unsigned int reserved2:27;
   unsigned int flag:1;
   unsigned int cidTimeOut:1;
   unsigned int polarity:1;
   unsigned int battery:1;
   unsigned int ring:1;
} CPC5621_ECP_BITS;
#else
#error Please define endian mode
#endif

/* exception status */
typedef union
{
	CPC5621_ECP_BITS        Bits;
	unsigned long           Status;
}CPC5621_EXCEPTION;


/** config CID timer and Ring timer */
typedef struct
{
    unsigned long           Cid_tMin;
    unsigned long           Ring_tMin;
    unsigned long           Bat_tMin;
}CPC5621_CONFIG;



#endif /* _DRV_CPC5621_INTERFACE_H */

