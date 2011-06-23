/*
** Copyright (C) 2005 Wu Qi Ming <Qi-Ming.Wu@infineon.com>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


****************************************************************************
   Module      : drv_duslic_api.c
   Date        : 2005-07-11
   Description : This file contains the implementation of all board specific
                 functions
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_duslic_api.h"
#include "drv_board.h"
#include "asm/amazon/amazon.h"
#include "asm/amazon/irq.h"
#define GPIO27 0x0800
#define DUSLIC_RESET GPIO27

#ifdef DUSLIC_SPI
extern int ifx_ssc_open(struct inode *, struct file *);
extern int ifx_ssc_close(struct inode *, struct file *);		     
extern int amazon_ssc_cs_low(u32 pin);
extern int amazon_ssc_cs_high(u32 pin);
extern int amazon_ssc_txrx(char * tx_buf, u32 tx_len, char * rx_buf, u32 rx_len);
extern int ifx_ssc_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
		     
extern struct semaphore ssc_session_mutex;

/* #define SET_CS_LOW(pin) 	amazon_ssc_cs_low(pin); */
/* #define SET_CS_HIGH(pin)	amazon_ssc_cs_high(pin) */
/* #define SPI_IOCTL(cmd,data)	ifx_ssc_ioctl(0,0,cmd,data) */
#define SPI_CS_SET(high_low)     do  {                                                  \
             if ((high_low) == 0)                                                        \
             {                                                                           \
                 disable_irq(INT_NUM_IM2_IRL13); /* disable vinetic interrupts */        \
                 if (!in_interrupt())                                                    \
                     down(&ssc_session_mutex);                                           \
                 *AMAZON_GPIO_P0_OUT&=~(1<<6);                                           \
             }                                                                           \
             else                                                                        \
             {                                                                           \
                 *AMAZON_GPIO_P0_OUT|=1<<6;                                              \
                 if (!in_interrupt())                                                    \
                     up(&ssc_session_mutex);                                             \
                 enable_irq(INT_NUM_IM2_IRL13); /* allow vinetic interrupts */           \
                 udelay(5);                                                              \
             }                                                                           \
         } while(0)

#define SET_CS_LOW(pin) 	SPI_CS_SET(0);
#define SET_CS_HIGH(pin)	SPI_CS_SET(1)

static UINT rwSpi (UINT8 *pTxBuf, UINT nTxBuf, UINT8 *pRxBuf, UINT nRxBuf)
{
   int ret;
       /* TODO: find a proper solution for concurrent SPI accesses.
          Right now the open/close functions interfere with the vinetic driver,
          which relies on an activated ssc  interface. If close is called here,
          then the vinetic driver will fail accessing the ssc, resulting in an
          endless loop */
/*    if(ifx_ssc_open((struct inode*)0, NULL)!=-EBUSY) */
   {
    SET_CS_LOW(2);
 //   mdelay(5);
    amazon_ssc_txrx(pTxBuf, nTxBuf, pRxBuf, nRxBuf);
    SET_CS_HIGH(2);
//   mdelay(5);
/*     ifx_ssc_close((struct inode*)0, NULL); */
    ret=IFX_SUCCESS;
   }
   
/*    else */
/*    { */
/*      printk("duslic confliction!\n"); */
/*      ret=IFX_ERR; */
/*    } */
   
   return ret;
}
#endif /* DUSLIC_SPI*/


/*******************************************************************************
Description:
   Set the interrupt pin number
Arguments:
   nIrq : IRQ Line Number = IRQ Nr << 2
Return:
   None
*******************************************************************************/
void Board_InitIrq (int nIrq)
{
        /* TODO: Resource management, lock GPIOs, that are used */
#ifndef USE_VINETIC_GPIO
    /*GPIO 0 set to be EXIN mode*/
    *AMAZON_GPIO_P0_DIR &=~1;
    *AMAZON_GPIO_P0_ALTSEL0 |=1;
    *AMAZON_GPIO_P0_ALTSEL1 &=~1;
    /*set EXTIN1 to be enabled in falling edge*/
    (*AMAZON_ICU_EXTINTCR) = (*AMAZON_ICU_EXTINTCR)|0x20;
    /*enable EXTIN1*/
    (*AMAZON_ICU_IRNEN) = (*AMAZON_ICU_IRNEN)|0x2;
#endif
    return;

}

/****************************************************************************
Description:
   Acknoledge the Irq
Arguments:
   pDev - handle to the device
Return Value:
   None.
****************************************************************************/
void Board_AckIrq (int nIrq)
{
     /* Nothing to be done... */
}


/****************************************************************************
Description:

*****************************************************************************
Description:
   DUSLIC device structure initialization.
Arguments:
	none
Return:
   SUCCESS if successful otherwise ERR
Remarks:
   This Function is called when a file descriptor is opened for the first time.
   It initialize the device structure(s).
****************************************************************************/
int Board_DevOpen(int nDevn)
{
   int err=IFX_SUCCESS;
   DUSLIC_DEVICE* pDev = NULL;

   /* Call an OS dependend function at the first 	     */
   /* opening for the device, to allocate the memory   */
   /* for the VINETIC device which includes the device */
   /* context and the channel contextes.               */

   err = OS_InitDevice (nDevn);
      

   if (err == IFX_SUCCESS && OS_GetDevice(nDevn, &pDev) == IFX_SUCCESS)
   {

      switch (nDevn)
      {
         case 0:
            pDev->nIrq = INT_NUM_IM2_IRL13;
            break;
         default:
            pDev->nIrq = 0;
      }
   }

	/* Set the interrupt pin number in the CPU    */
   if (err == SUCCESS && pDev->nIrq)
   {
      Board_InitIrq (pDev->nIrq);
	   /* since both device share the same irq, it does not matter which */
	   /* device structure is passed here to get the irq vector				*/
      OS_Install_DuslicIRQHandler(pDev, pDev->nIrq);
   }
   return err;
}

#ifdef DUSLIC_SPI
/*******************************************************************************
Description:
   serial read trough SPI
Arguments:
   pDev: pointer to the device structure
   cmd : short command which allow a specific reading
   pData: pointer to the read buffer
   count: number of words to read
Return:
    0 if no error
   -1 if error
*******************************************************************************/
IFX_int32_t Board_SerialRead (struct _DUSLIC_DEVICE *pDev, IFX_uint8_t* cmd, \
                     IFX_int32_t cmd_count, IFX_uint8_t* pData, IFX_int32_t count)
{
   int err=IFX_SUCCESS;
   rwSpi(cmd, cmd_count, pData, count);
   return err;   
}

/*******************************************************************************
Description:
   serial write trough SPI
Arguments:
   pDev: pointer to the device structure
   count: number of words to write
   pData: pointer to the write buffer
Return:
    0 if no error
   -1 if error
Remarks:
*******************************************************************************/
IFX_int32_t Board_SerialWrite(struct _DUSLIC_DEVICE *pDev, IFX_uint8_t* pData, IFX_int32_t count)
{
/*   int i = 0;
   printk("Board_SerialWrite : ");
   for (i = 0;i < count;i++)
	   printk("0x%X ",pData[i]);
   printk("\n");
   */
   rwSpi(pData,count, NULL, 0);
   return IFX_SUCCESS;
}

#endif /*DUSLIC_SPI*/

/****************************************************************************
Description:
	Initializes the Duslic platform 
Arguments:
	none
Return Value:
	none
****************************************************************************/
void Board_InitPlatform ()
{
        /* TODO: Resource management, SPI/SSC GPIOs should be
           initialised in SSC driver */
        /* Currently we assume, that the vinetic driver is initialised first and
           that everything is set up properly. */ 
#if 0
#ifndef USE_VINETIC_GPIO
   /*GPIO 8 CLK_OUT used as output*/
    *AMAZON_GPIO_P0_DIR|=0x100;
    *AMAZON_GPIO_P0_ALTSEL0|=1<<8;
    *AMAZON_GPIO_P0_ALTSEL1&=(u32)(~(1<<8));
    *AMAZON_GPIO_P0_OD|=1<<8;
    *AMAZON_GPIO_P0_PUDSEL|=1<<8;
    *AMAZON_GPIO_P0_PUDEN |=1<<8;

    /*GPIO 10 SPI_DIN used as input */
    *AMAZON_GPIO_P0_DIR &=~(1<<10);
    *AMAZON_GPIO_P0_ALTSEL0 |=1<<10;
    *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<10);
    
    *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<11);
    *AMAZON_GPIO_P0_OD |=1<<11;
    
    /*GPIO 12 SPI clock used as output */
    *AMAZON_GPIO_P0_DIR |=1<<12;
    *AMAZON_GPIO_P0_ALTSEL0 |=1<<12;
    *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<12);
    *AMAZON_GPIO_P0_OD |=1<<12;
#endif    
    
    /*GPIO 13 SPI_CS2 used as output(CS2) */
    *AMAZON_GPIO_P0_DIR|=1<<13;
    *AMAZON_GPIO_P0_ALTSEL0 |=~(1<<13);
    *AMAZON_GPIO_P0_ALTSEL1 |=1<<13;
    *AMAZON_GPIO_P0_OD |=1<<13;
    
    
    /*GPIO 15 TDM_DO*/
    *AMAZON_GPIO_P0_DIR |=1<<15;
    *AMAZON_GPIO_P0_ALTSEL0 |=1<<15;
    *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<15);
    *AMAZON_GPIO_P0_OD |=1<<15;
    *AMAZON_GPIO_P0_PUDSEL|=1<<15;
    *AMAZON_GPIO_P0_PUDEN |=1<<15;

    /*GPIO 16 TDM_DO*/
    *AMAZON_GPIO_P1_DIR &=~1;
    *AMAZON_GPIO_P1_ALTSEL0 |=1;
    *AMAZON_GPIO_P1_ALTSEL1 &=~1;
    
    /*GPIO 17 TDM_DCL */
    *AMAZON_GPIO_P1_DIR|=0x02;
    *AMAZON_GPIO_P1_ALTSEL0|=0x02;
    *AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x02;
    *AMAZON_GPIO_P1_OD|=0x02;
    
    /*GPIO 18 TDM FSC*/
    *AMAZON_GPIO_P1_DIR|=0x04;
    *AMAZON_GPIO_P1_ALTSEL0|=0x04;
    *AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x04;
    *AMAZON_GPIO_P1_OD|=0x04;
    
    // change CLK_OUT clock to 7.68 MHz
    *((volatile u32 *)(0xBF10300C))|=0x6000000;
    
      
    // change GPIO3 to 8Khz 
    *((volatile u32 *)(0xBF10303C))=0x8;
    // change GPIO3 to output
    /*******GPIO 3**********/
    *((volatile u32 *)(AMAZON_GPIO_P0_DIR))|=0x8;
    *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL0))|=0x8;
    *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL1))&=(u32)~0x8;
    *((volatile u32 *)(AMAZON_GPIO_P0_OD))|=0x8;
    
       /*reset the duslic chip*/
    *AMAZON_GPIO_P0_DIR &= ~(1<<14);
    *AMAZON_GPIO_P0_ALTSEL0 &=~(1<<14);
    *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<14);
    *AMAZON_GPIO_P0_OUT |= 1<<14;
    mdelay(4);    
  

//    *((volatile u32*)AMAZON_SSC_WHBSTATE) = 0x1;
	asm("SYNC"); 

    //GPIO CS
    *AMAZON_SSC_GPOCON |=0x0400; /*use cs2 as chip select pin*/
	
    *AMAZON_SSC_WHBGPOSTAT |=0x0400; /*have to disable all the chip select pins first*/

    //Set Master mode
//    *((volatile u32*)AMAZON_SSC_WHBSTATE) =0x8|2;

#endif 
     return;
}

/****************************************************************************
Description:
	Reset the chip select of the power pc
Return Value:
	none
****************************************************************************/
void Board_ResetPlatform ()
{
}

