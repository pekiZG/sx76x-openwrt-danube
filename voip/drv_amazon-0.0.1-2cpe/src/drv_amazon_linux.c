
/*******************************************************************************
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
********************************************************************************
   Module      : drv_amazon_linux.c
   Date        : 2004-10-25
   Description : Board driver Linux OS File.
*******************************************************************************/


/** \file
    This file contains the board driver implementation for linux OS.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

/* driver includes */
#include "drv_amazon_api.h"
#ifdef LINUX
/* system specific includes */
#ifdef CONFIG_DEVFS_FS
#include "linux/devfs_fs_kernel.h"
#endif /* CONFIG_DEVFS_FS */
#include "asm/amazon/amazon.h"
#include "asm-mips/amazon/irq.h"
#include "vinetic_io.h"
#include "sys_drv_ifxos.h"
#include "drv_config_user.h"
#define amazon_VINETIC1_IRQNUM          INT_NUM_IM2_IRL13
#define SSC_ADDR                0xB0100800
#define SSC_WHBSTATE            SSC_ADDR+0x00000018
#define SSC_GPOCON              SSC_ADDR+0x00000010
#define SSC_WHBGPOSTAT          SSC_ADDR+0x00000014
/* ============================= */
/* Local defines and macros      */
/* ============================= */
#define AMAZON_DEVNAME  "vinbsp"

/* ============================= */
/* Local structure               */
/* ============================= */
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#error "__BYTE_ORDER == __LITTLE_ENDIAN"
#endif
/* ============================= */
/* Global variable definition    */
/* ============================= */

MODULE_AUTHOR          ("Infineon Technologies, COM AC SD SW");
MODULE_DESCRIPTION     ("amazon Vinetic Board Driver - www.infineon.com");
MODULE_SUPPORTED_DEVICE("amazon DEVICE");

/* ============================= */
/* Global function declaration   */
/* ============================= */

int   init_module    (void);
void  cleanup_module (void);

/* Board common functions declaration */
IFX_int32_t amazon_DevInit  (IFX_uint32_t nDevNum);
IFX_int32_t amazon_DevReset (IFX_uint32_t nDevNum);
/* ============================= */
/* Local function declaration    */
/* ============================= */

static int amazon_open   (struct inode *inode, struct file *filp);
static int amazon_release(struct inode *inode, struct file *filp);
static int amazon_ioctl  (struct inode *inode, struct file *filp,
                            unsigned int cmd, unsigned long arg);

#if CONFIG_PROC_FS
static int  amazon_get_version_proc   (char *buf);
static int  amazon_get_status_proc    (char *buf);
static int  amazon_install_proc_entry (void);
static void amazon_remove_proc_entry  (void);
static int  amazon_read_proc          (char *page, char **start, off_t off,
                                         int count, int *eof, void *data);
#endif /* CONFIG_PROC_FS */

/* ============================= */
/* Local variable declaration    */
/* ============================= */

static IFX_uint8_t major_number = 0;
MODULE_PARM(major_number, "b");
MODULE_PARM_DESC(major_number, "to override automatic major number");

/** amazon device structure */
static amazon_CTRL_DEV amazonDev [MAX_amazon_INSTANCES];

/** mpc jtag driver callback functions */
static struct file_operations amazon_fops =
{
   owner  : THIS_MODULE,
   open   : amazon_open,
   release: amazon_release,
   ioctl  : amazon_ioctl,
};

#ifdef CONFIG_DEVFS_FS
/** handles for Dev FS */
static devfs_handle_t amazon_base_dir;
static devfs_handle_t amazon_handle [MAX_amazon_INSTANCES];
#endif

/* ============================= */
/* Local function definition     */
/* ============================= */

/******************************************************************************/
/**
  Open board device.

   \param  inode   - pointer to the inode
   \param  filp    - pointer to the file descriptor

   \return
      0 or negative error code if error
   \remark
      this function initializes memory for device structure and set in use flag
*/
/******************************************************************************/
static int amazon_open(struct inode *inode, struct file *filp)
{
   int ret = 0, num;

   TRACE(amazon_DRV, DBG_LEVEL_LOW,("amazon_DRV: open\n\r"));
   num = MINOR(inode->i_rdev);
   /* store dev_no in private_data like for dev FS */
   (int)(filp->private_data) = num;
   /* check device number */
   if (num >= MAX_amazon_INSTANCES)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV : unknown device at open\n\r"));
      ret = -ENODEV;
   }
   /* initialize memory if first device open */
   if ((ret == IFX_SUCCESS) && (samazon_Dev [num] == NULL))
   {
      /* initialize and cache device ptr */
      samazon_Dev [num] = &amazonDev[num];
      amazon_DevInit (num);
     
   }
   /* check if device was opened already */
   if ((ret == IFX_SUCCESS) && (samazon_Dev[num]->bInUse == IFX_TRUE))
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,("amazon_DRV :  Device already opened\n\r"));
      ret = -ENODEV;
   }
   /* set flags after succesfull opening */
   if (ret == IFX_SUCCESS)
   {
      /* set in use flag */
      samazon_Dev[num]->bInUse = IFX_TRUE;
     	/* increment module use counter */
      MOD_INC_USE_COUNT;
   }
   //printk("%d, m=%d\n",num,amazonDev[num].boardConf.nAccessMode); 
   return ret;
}

/******************************************************************************/
/**
  Close board device.

   \param  inode   - pointer to the inode
   \param  filp    - pointer to the file descriptor

   \return
      0 or negative error code

   \remark
      this function reset the in use flag
*/
/******************************************************************************/
static int amazon_release(struct inode *inode, struct file *filp)
{
   int ret = 0,
       num = (int)filp->private_data;

   TRACE(amazon_DRV, DBG_LEVEL_LOW,("amazon_DRV: close\n\r"));

   /* check device number */
   if (num >= MAX_amazon_INSTANCES)
   {
      TRACE (amazon_DRV, DBG_LEVEL_HIGH, ("amazon_DRV : unknown device at close\n\r"));
      ret = -ENODEV;
   }
   if (ret == IFX_SUCCESS)
   {
      /* reset in use flag */
      samazon_Dev[num]->bInUse = IFX_FALSE;
      /* decrement module use counter */
      MOD_DEC_USE_COUNT;
   }
   return ret;
};

void Board_InitIrq ()
{
        printk("Board_InitIrq!\n");
	//INT--GPIO P0.0-EXIN1, INT_NUM_IM2_IRL13
	//GPIO P0.0 IN/ALT0:1 ALT1:0
	(*AMAZON_GPIO_P0_DIR) = (*AMAZON_GPIO_P0_DIR) & 0xfffffffe;
	(*AMAZON_GPIO_P0_ALTSEL0) = (*AMAZON_GPIO_P0_ALTSEL0)| 1;
	(*AMAZON_GPIO_P0_ALTSEL1) = (*AMAZON_GPIO_P0_ALTSEL1)& 0xfffffffe;
	(*AMAZON_GPIO_P0_PUDSEL) =  (*AMAZON_GPIO_P0_PUDSEL) | 1;
	(*AMAZON_GPIO_P0_PUDEN) = (*AMAZON_GPIO_P0_PUDEN) | 1;
	//External Interrupt Node
	(*AMAZON_ICU_EXTINTCR) = (*AMAZON_ICU_EXTINTCR)|0x60; /* level triggered interrupt */
	(*AMAZON_ICU_IRNEN) = (*AMAZON_ICU_IRNEN)|0x2;
}

int SPI_Init(void)
{
        printk("SPI_Init!\n");
         /*GPIO 6 set as OUTPUT , high, CS for the slicofi */
        *AMAZON_GPIO_P0_OUT |=1<<6;
	*AMAZON_GPIO_P0_OD |=1<<6;
        *AMAZON_GPIO_P0_DIR|=1<<6;
        *AMAZON_GPIO_P0_ALTSEL0 &=~(1<<6);
        *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<6);

	
	// JGUI: GPIO9 used as CS for the vinetic on new board, 4 is USB on new board
        /*GPIO 9 set as OUTPUT , high */
        *AMAZON_GPIO_P0_DIR|=1<<9;
        *AMAZON_GPIO_P0_ALTSEL0 &=~(1<<9);
        *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<9);
        *AMAZON_GPIO_P0_OUT |=1<<9;
	*AMAZON_GPIO_P0_OD |=1<<9; // JGUI: addded this 

        // change CLK_OUT clock to 7.68 MHz
	*((volatile u32 *)(0xBF10300C))|=0x6000000;
	
        /*******GPIO 8**********/
	*((volatile u32 *)(AMAZON_GPIO_P0_DIR))|=0x100;
	*((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL0))|=0x100;
	*((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL1))&=(u32)~0x100;
	*((volatile u32 *)(AMAZON_GPIO_P0_OD))|=0x100;
	// change GPIO3 to 8Khz 
	*((volatile u32 *)(0xBF10303C))=0x8;
	// change GPIO3 to output
        /*******GPIO 3**********/
	*((volatile u32 *)(AMAZON_GPIO_P0_DIR))|=0x8;
	*((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL0))|=0x8;
	*((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL1))&=(u32)~0x8;
	*((volatile u32 *)(AMAZON_GPIO_P0_OD))|=0x8;

	*((volatile u32*)SSC_WHBSTATE) = 0x1;
	asm("SYNC"); 

        *AMAZON_SSC_GPOCON =0; /*use none of the chip select pins as the chip select pin*/

	/* TODO: Move this code to SPI module */
	*AMAZON_SSC_WHBSTATE = 1; /* disable SSC */
	*AMAZON_SSC_STATE &= ~1; /* switch to configuration state */
	*AMAZON_SSC_CON = (*AMAZON_SSC_CON & 0xffffff9f) | 0x00000040;
	*AMAZON_SSC_STATE |= 1;	/* switch back to operational state */
	*AMAZON_SSC_WHBSTATE = 2; /* enable SSC again */

		//Set Master mode
	*((volatile u32*)SSC_WHBSTATE) =0x8|2;
	
//TODO		
/* use ioctl system call to set some control parameters */
//	IOCTL(cmd,data)
/* by default SPI work in TXRX mode */
	 return IFX_SUCCESS;
}


void Board_InitPlatform ()
{
   printk("Board_InitPlatform!\n");
   *((volatile u32 *)(AMAZON_GPIO_P0_DIR))|=0x4000;
   *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL0))&=(u32)~0x4000;
  
   *((volatile u32 *)(AMAZON_GPIO_P0_ALTSEL1))&=(u32)~0x4000;
   *((volatile u32 *)(AMAZON_GPIO_P0_OD))|=0x4000;
   
   // bring duslic out of reset
   *((volatile u32 *)(AMAZON_GPIO_P1_OUT)) |= 0x0200;
   SPI_Init();;   /* Set reset pin back to high */	
  
   /*set gpio 27,set the relay, remove this if using fxo*/
/*   *AMAZON_GPIO_P1_OD|=1<<11;
   *AMAZON_GPIO_P1_DIR|=1<<11;
   *AMAZON_GPIO_P1_ALTSEL0 &=~(1<<11);
   *AMAZON_GPIO_P1_ALTSEL1 &=~(1<<11);
   *AMAZON_GPIO_P1_OUT |=1<<11;   
*/
#if 0
       /* If the Vinetic clock is derived from the TDM interface,
          we need to enable the corresponding GPIOs */
   
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

    printk("AMAZON_GPIO_P1_DIR=0x%04x, AMAZON_GPIO_P1_OUT=0x%04x, AMAZON_GPIO_P1_OD=0x%04x,\n"
           "AMAZON_GPIO_P1_ALTSEL0=0x%04x, AMAZON_GPIO_P1_ALTSEL1=0x%04x\n",
           *AMAZON_GPIO_P1_DIR, *AMAZON_GPIO_P1_OUT, *AMAZON_GPIO_P1_OD,
           *AMAZON_GPIO_P1_ALTSEL0, *AMAZON_GPIO_P1_ALTSEL1);
#endif
}


/******************************************************************************/
/**
   Board device control / configuration.

   \param inode - pointer to the inode
   \param filp  - pointer to the file descriptor
   \param cmd   - function id's
   \param arg   - functions's argument

   \return
      number of devices on board for FIO_AMAZON_INIT,
      otherwise IFX_SUCCESS or IFX_ERROR
*/
/******************************************************************************/
static int amazon_ioctl (struct inode *inode, struct file *filp,
                           unsigned int cmd, unsigned long arg)
{
   amazon_CTRL_DEV *pDev;
   IFX_int32_t       ret = IFX_SUCCESS,
                     num = (IFX_int32_t)filp->private_data;

   TRACE(amazon_DRV, DBG_LEVEL_LOW,("amazon_DRV: ioctl\n\r"));
   
   /* check the device number */
   if (num >= MAX_amazon_INSTANCES)
   {
      TRACE(amazon_DRV, DBG_LEVEL_HIGH,
           ("amazon_DRV: unknown device at ioctl\n\r"));
      return -ENODEV;
   }
   /* check pointer */
   if (samazon_Dev[num] == NULL)
   {
      TRACE(amazon_DRV, DBG_LEVEL_HIGH,
           ("amazon_DRV: invalid device pointer\n\r"));
      return -ENODEV;
   }
   /* map pointer */
   pDev = samazon_Dev[num];
   /* process io command */
   switch (cmd)
   {
   case FIO_AMAZON_DEBUGLEVEL:
      if ((arg >= DBG_LEVEL_LOW) && (arg <= DBG_LEVEL_OFF))
      {
         SetTraceLevel(amazon_DRV, arg);
         SetLogLevel  (amazon_DRV, arg);
      }
      break;
   case FIO_AMAZON_GET_VERSION:
      {
         int len = (strlen(DRV_amazon_VER_STR) + 1);
         ret = copy_to_user((char*)arg, DRV_amazon_VER_STR,
                            ((len < 80 )? len : 80));
         if (ret != 0)
         {
            TRACE(amazon_DRV,DBG_LEVEL_HIGH,("amazon_DRV: IoCtl (0x%08X) :"
                  " cannot copy to user space\n\r", cmd));
            ret = IFX_ERROR;
         }
         else
            ret = IFX_SUCCESS;
      }
      break;
   case FIO_AMAZON_CONFIG:
      {
         if (arg == 0)
         {
            ret = amazon_Board_Config (pDev, NULL);
         }
         else
         {
            amazon_Config_t  boardConf;
            ret = copy_from_user((unsigned long *)&boardConf,
                                 (unsigned long *)arg,
                               sizeof(boardConf));
            if (ret != 0)
            {
               TRACE(amazon_DRV,DBG_LEVEL_HIGH,("amazon_DRV: IoCtl (0x%08X) :"
                     " cannot copy from user space\n\r", cmd));
               ret = IFX_ERROR;
            }
            else
               ret = amazon_Board_Config (pDev, &boardConf);
         }
      }
      break;
   case FIO_AMAZON_GETCONFIG:
      {
         amazon_Config_t  boardConf;
         ret = amazon_Board_GetConfig (pDev, &boardConf);
         if (ret == IFX_SUCCESS)
         {
            ret = copy_to_user((unsigned long *)arg, (unsigned long *)&boardConf,
                                sizeof(boardConf));
            if (ret != 0)
            {
               TRACE(amazon_DRV,DBG_LEVEL_HIGH,("amazon_DRV: IoCtl (0x%08X) :"
                     " cannot copy to user space\n\r", cmd));
               ret = IFX_ERROR;
            }
            else
               ret = IFX_SUCCESS;
         }
      }
      break;
   case FIO_AMAZON_INIT:     
      printk("drv_amazon:FIO_AMAZON_INIT\n");
      Board_InitPlatform();
      Board_InitIrq();
      printk("init finished!\n");  
      ret=1;
      break;
   case FIO_AMAZON_RESETCHIP:
      {
          /* Set reset pin GPIO 14 to  low */
        *((volatile u32 *)(AMAZON_GPIO_P0_OUT))&=(u32)~0x4000;
        udelay(10);
        /* Set reset pin back to high */	
        //*((volatile u32 *)(AMAZON_GPIO_P0_OUT))|=0x4000;
        /* wait 12*125 uSec */
        //udelay (12*125);
	 
      }
      break;
   case FIO_AMAZON_GETBOARDPARAMS:
      {
	 VINETIC_BasicDeviceInit_t  devInit;
         devInit.AccessMode      =  VIN_ACCESS_SPI;
         devInit.nBaseAddress    =  0x1f;
         devInit.nIrqNum         =  amazon_VINETIC1_IRQNUM;
         copy_to_user ((void *)arg, (void *)&devInit, sizeof (devInit));
         
      }
	 break;
      case FIO_AMAZON_GET_BOARDVERS:
      {
         IFX_int32_t nBoardVersion;
         ret = amazon_Board_GetBoardVers (pDev, &nBoardVersion);
         if ((ret == IFX_SUCCESS) && (copy_to_user ((IFX_uint32_t *)arg,
             (IFX_uint32_t *)&nBoardVersion, sizeof(IFX_int32_t)) > 0))
         {
            TRACE(amazon_DRV,DBG_LEVEL_HIGH,
                 ("TAPI: cannot copy to user space\n\r"));
            ret = IFX_ERROR;
         }
      }
      break;
   case FIO_AMAZON_SET_ACCESSMODE:
      ret = amazon_Board_SetAccessMode (pDev, arg);
      break;
   case FIO_AMAZON_SET_CLOCKRATE:
      ret = amazon_Board_SetClockRate (pDev, arg);
      break;
   case FIO_AMAZON_DEACTIVATE_RESET:
      *((volatile u32 *)(AMAZON_GPIO_P0_OUT))|=0x4000;
      printk("drv_amazon:reset deactivated!\n");
      mdelay (5);
#if 0 
#if WITH_VINETIC_1_4
     /*fix me, add something to read isr of vinetic1.4 chip*/
      {
        unsigned short cmd={0};
        unsigned char data[6]={0};
        /*GPIO 9 SPI_CS_N used as output(CS0) */
        *AMAZON_GPIO_P0_DIR|=1<<9;
        *AMAZON_GPIO_P0_ALTSEL0 |=1<<9;
        *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<9);
        *AMAZON_GPIO_P0_OD |=1<<9;

	*((volatile u32*)SSC_WHBSTATE) = 0x1;
	asm("SYNC");

        *AMAZON_SSC_GPOCON |=0x0100; /*use cs0 as the chip select pin*/
	*AMAZON_SSC_WHBGPOSTAT =0x0100; /*have to disable all the chip select pins*/

		//Set Master mode
	*((volatile u32*)SSC_WHBSTATE) =0x8|2;

      amazon_ssc_cs_low(0);
      cmd=0x8000|0x4000|0x0;
      amazon_ssc_txrx((u8*)&cmd,2,(u8*)data,2);
      amazon_ssc_txrx(NULL,0,(u8*)data,2);
      amazon_ssc_cs_high(0);
      
      mdelay(5);
      
      printk("%02x%02x%02x%02x%02x%02x\n",\
      data[0],data[1],data[2],data[3],data[4],data[5]);
      memset(data,0,6);
      
      amazon_ssc_cs_low(0);
      memset(data,0,6);
      cmd=0x8000|0x4000|0x00C0;
      amazon_ssc_txrx((u8*)&cmd,2,(u8*)data,2);
      amazon_ssc_txrx(NULL,0,(u8*)data,4);
      amazon_ssc_cs_high(0);
      printk("%02x%02x%02x%02x%02x%02x\n",\
      data[0],data[1],data[2],data[3],data[4],data[5]);
      

      amazon_ssc_cs_low(0);
      memset(data,0,6);
      cmd=0x8000|0x4000|0x00A0;
      amazon_ssc_txrx((u8*)&cmd,2,(u8*)data,2);
      amazon_ssc_txrx(NULL,0,(u8*)data,4);

      amazon_ssc_cs_high(0);
      mdelay(5);
      printk("%02x%02x%02x%02x%02x%02x\n",\
      data[0],data[1],data[2],data[3],data[4],data[5]);
     
      /*GPIO 9 set as OUTPUT , high */
        *AMAZON_GPIO_P0_DIR|=1<<9;
        *AMAZON_GPIO_P0_ALTSEL0 &=~(1<<9);
        *AMAZON_GPIO_P0_ALTSEL1 &=~(1<<9);
        *AMAZON_GPIO_P0_OUT |=1<<9;
	*AMAZON_SSC_GPOCON =0; /*use none of the chip select pins as the chip select pin*/
	
		//Set Master mode
	*((volatile u32*)SSC_WHBSTATE) =0x8|2;
	
      }
#endif
#endif
      ret=1;
      break;
   default:
      TRACE(amazon_DRV, DBG_LEVEL_HIGH,
           ("amazon_DRV: Unknown IoCtl (0x%08X), arg 0x%08lX.\n\r", cmd, arg));
      ret = IFX_ERROR;
      break;
   }

   return ret;
}

#if CONFIG_PROC_FS
/******************************************************************************/
/**
   Read the version information from the driver.

   \param buf - destination buffer

   \return
    len
*/
/******************************************************************************/
static int amazon_get_version_proc (char *buf)
{
   int len;

   len = sprintf(buf, "%s\n\r", &amazon_WHATVERSION[4]);

   len += sprintf(buf + len, "Compiled on %s, %s for Linux kernel %s\n\r",
                __DATE__, __TIME__, UTS_RELEASE);

   return len;
}

/******************************************************************************/
/**
   Read the status information from the driver.

   \param buf - destination buffer

   \return
    len
*/
/******************************************************************************/
static int amazon_get_status_proc(char *buf)
{
   int len = 0, i, j;

   for (i = 0; i < MAX_amazon_INSTANCES; i++)
   {
      len += sprintf(buf+len, "********************************\n\r");
      len += sprintf(buf+len, " Device %d [ptr = 0x%08X] \n\r", i,
                     (int)samazon_Dev [i]);
      len += sprintf(buf+len, "--------------------------------\n\r");
      if (samazon_Dev [i] != NULL)
      {
         len += sprintf(buf+len, "Device Status : %s\n\r",
                        ((samazon_Dev[i]->bInUse) ? "opened" : "closed"));
         len += sprintf(buf+len, "Board Init   : %s\n\r",
                        ((samazon_Dev[i]->bBoardInitialized) ? "done":"not done"));
         if (samazon_Dev[i]->bBoardInitialized == IFX_TRUE)
         {
            len += sprintf(buf+len, "Number of Vinetics:  %ld\n\r\n\r",
                           samazon_Dev[i]->nChipTotNum);
            for (j = 0; j < samazon_Dev[i]->nChipTotNum; j++)
            {
               len += sprintf(buf+len, "Vinetic %d   :\n\r", j);
               len += sprintf(buf+len, "Access Mode  : %d\n\r",
                              samazon_Dev[i]->boardConf.nAccessMode);
               len += sprintf(buf+len, "Clock Rate   : %d\n\r",
                              samazon_Dev[i]->boardConf.nClkRate);
               len += sprintf(buf+len, "Base address : 0x%08lX\n\r",
                              (samazon_Dev[i]->pChipInfo[j]).nBaseAddrPhy);
               len += sprintf(buf+len, "Irq Number   : %d\n\r",
                              (samazon_Dev[i]->pChipInfo[j]).nIrqNum);
               len += sprintf(buf+len, "********************************\n\r");
            }
         }
         /* extend the list here */
      }
   }

   return len;
}

/******************************************************************************/
/**
   The proc filesystem: function to read and entry.

   \return
   len
*/
/******************************************************************************/
static int amazon_read_proc (char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
   int (*fn)(char *buf), len;

   if (data != NULL)
   {
     fn = data;
     len = fn (page);
   }
   else
   {
     return 0;
   }
   if (len <= off+count)
   {
     *eof = 1;
   }
   *start = page + off;
   len -= off;
   if (len > count)
   {
     len = count;
   }
   if (len < 0)
   {
     len = 0;
   }

   return len;
}

/******************************************************************************/
/**
   Initialize and install the proc entry

\return
   IFX_SUCCESS or IFX_ERROR
\remark
   Called by the kernel.
*/
/******************************************************************************/
static int amazon_install_proc_entry (void)
{
   struct proc_dir_entry *driver_proc_node;

   /* install the proc entry */
   TRACE(amazon_DRV,DBG_LEVEL_LOW, ("amazon_DRV: using proc fs\n\r"));

   driver_proc_node = proc_mkdir( "driver/" AMAZON_DEVNAME, NULL);
   if (driver_proc_node != NULL)
   {
      create_proc_read_entry("version" , S_IFREG|S_IRUGO, driver_proc_node,
                             amazon_read_proc, (void *)amazon_get_version_proc);
      create_proc_read_entry("status" , S_IFREG|S_IRUGO, driver_proc_node,
                             amazon_read_proc, (void *)amazon_get_status_proc);
   }
   else
   {
      TRACE(amazon_DRV, DBG_LEVEL_HIGH,("amazon_DRV: cannot "
            "create proc entry\n\r"));
      return IFX_ERROR;
   }

   return IFX_SUCCESS;
}

/******************************************************************************/
/**
   Remove proc entries

\remark
   Called by the kernel.
*/
/******************************************************************************/
static void amazon_remove_proc_entry(void)
{
   remove_proc_entry("driver/" AMAZON_DEVNAME "/version" ,0);
   remove_proc_entry("driver/" AMAZON_DEVNAME "/status",0);
   remove_proc_entry("driver/" AMAZON_DEVNAME ,0);

   return;
}

#endif /*CONFIG_PROC_FS*/

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/**
   returns the virtual address to access (Linux MMAP)

   \param nBaseAddrPhy - physical base address.
   \param nMemSize     - size of memory region to map.
   \param pMemName     - name of the memory region

   \return
      pointer to virtual memory for linux
   \remark
*/
/******************************************************************************/
IFX_void_t * amazon_os_get_memaddr (IFX_uint32_t nBaseAddrPhy,
                                     IFX_uint32_t nMemSize,
                                     IFX_char_t  *pMemName)
{
   IFX_void_t *pPtr = NULL;
   IFX_int32_t ret;

   ret = check_mem_region(nBaseAddrPhy, nMemSize);
   if (ret == IFX_SUCCESS)
   {
      /* request the io region */
      request_mem_region (nBaseAddrPhy, nMemSize, pMemName);
      /* get a virtual address */
      pPtr = ioremap_nocache(nBaseAddrPhy, nMemSize);
   }
   else
{
      TRACE (amazon_DRV, DBG_LEVEL_HIGH,
            ("ERROR: can't get I/O address 0x%lx\n\r", nBaseAddrPhy));
   }

   return pPtr;
}

/******************************************************************************/
/**
   release virtual memory

   \param pMemPtr   - virtual address.
   \param nBaseAddrPhy - physical base address.
   \param nMemSize     - size of memory region to map.

   \remark
*/
/******************************************************************************/
IFX_void_t amazon_os_release_memaddr (IFX_void_t * pMemPtr,
                                       IFX_uint32_t nBaseAddrPhy,
                                       IFX_uint32_t nMemSize)
{
   /* request the io region */
   release_mem_region (nBaseAddrPhy, nMemSize);
   iounmap (pMemPtr);
   pMemPtr = NULL;
}

/******************************************************************************/
/**
   Initialize the module

   \return
      Error code or 0 on success
   \remark
      called by the kernel.
*/
/******************************************************************************/
int init_module (void)
{
   int result = 0, i;

   printk(KERN_INFO "%s\n\r", &amazon_WHATVERSION [4]);
   printk(KERN_INFO "(c) Copyright 2004, Infineon Technologies AG\n\r");
   SetTraceLevel(amazon_DRV, DBG_LEVEL_HIGH);
   SetLogLevel  (amazon_DRV, DBG_LEVEL_HIGH);

#ifdef CONFIG_DEVFS_FS
   {
   
      char buf[10];
       
      /* clear devfs structures */
      memset (&(amazon_base_dir), 0x00, sizeof (devfs_handle_t));
      memset (&(amazon_handle), 0x00, sizeof(amazon_handle));
      /* Create base directory /dev/amazon */
      amazon_base_dir = devfs_mk_dir (NULL, AMAZON_DEVNAME, NULL);
      if (amazon_base_dir != NULL)
      {
         for (i = 0; i < MAX_amazon_INSTANCES; i++)
         {
            /* add board devices to dev fs */
            sprintf (buf, "%d", i);
            /* private_data contains device number for open function
               instead of minor number (dynamically assigned by dev_fs) */
            amazon_handle [i] =
                devfs_register(amazon_base_dir, buf, 0,
                               major_number, i, S_IFCHR|S_IRUGO|S_IWUGO,
                               &amazon_fops, (void*)i);
            if (amazon_handle [i] == NULL)
            {
               TRACE(amazon_DRV,DBG_LEVEL_HIGH, ("amazon_DRV: unable to add"
                     " device /dev/%s/%s to dev_fs\n\r", AMAZON_DEVNAME, buf));
               result = -ENODEV;
               break;
            }
         }
      }
      else
      {
         TRACE(amazon_DRV,DBG_LEVEL_HIGH,("amazon_DRV: Unable to create "
               "/dev/%s base directory in dev_fs\n\r", AMAZON_DEVNAME));
         result = (-ENODEV);
      }
      if (result != 0)
      {
         cleanup_module ();
         return result;
      }
   }
#else /* CONFIG_DEVFS_FS */
   {
      result = register_chrdev (major_number, AMAZON_DEVNAME,
                                &amazon_fops);
      if (result < 0)
      {
         TRACE(amazon_DRV,DBG_LEVEL_HIGH,
               ("amazon_DRV: can't get major %d\n\r", major_number));
         return result;
      }
      /* dynamic major */
      if (major_number == 0)
      {
         major_number = result;
         TRACE(amazon_DRV,DBG_LEVEL_HIGH,
               ("Using major number %d\n\r", major_number));
      }
      
   }
#endif /* CONFIG_DEVFS_FS */

   /* reset device pointers cache */
   for (i = 0; i < MAX_amazon_INSTANCES; i++)
   {
      samazon_Dev [i] = NULL;
   }

#if CONFIG_PROC_FS
   amazon_install_proc_entry ();
#endif /* CONFIG_PROC_FS */

   return 0;
}

/******************************************************************************/
/**
   Clean up the module if unloaded.

   \remark
   Called by the kernel.
*/
/******************************************************************************/
void cleanup_module(void)
{
   int i;

   /* Free the device data block */
   for (i = 0; i < MAX_amazon_INSTANCES; i++)
   {
      /* reset device */
      amazon_DevReset (i);
      /* remove device from cache */
      samazon_Dev [i] = NULL;
#ifdef CONFIG_DEVFS_FS
      /* remove AMAZON_DEVNAME devices from dev fs */
      if (amazon_handle [i] != NULL)
      {
         devfs_unregister (amazon_handle [i]);
         TRACE(amazon_DRV,DBG_LEVEL_HIGH, ("amazon_DRV: device "
               "/dev/%s/%d removed\n\r", AMAZON_DEVNAME, i));
      }
#endif /* CONFIG_DEVFS_FS */
   }
#ifdef CONFIG_DEVFS_FS
   /* remove AMAZON_DEVNAME base directory from dev fs */
   if (amazon_base_dir != NULL)
   {
      devfs_unregister (amazon_base_dir);
      TRACE(amazon_DRV,DBG_LEVEL_HIGH, ("amazon_DRV: base directory"
            " /dev/%s removed\n\r", AMAZON_DEVNAME));
   }
#else /* CONFIG_DEVFS_FS */
   if (major_number)
   {
      unregister_chrdev (major_number, AMAZON_DEVNAME);
   }
#endif /* CONFIG_DEVFS_FS */

#if CONFIG_PROC_FS
   amazon_remove_proc_entry ();
#endif /* CONFIG_PROC_FS */

   TRACE(amazon_DRV,DBG_LEVEL_NORMAL, ("amazon_DRV: cleanup successful\n\r"));
}

#endif /* LINUX */
