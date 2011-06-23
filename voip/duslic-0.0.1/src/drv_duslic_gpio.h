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
*
 ******************************************************************************
   Module      : $RCSfile: drv_duslic_gpio.h,v $
   Revision    : $Revision: 0.0.1 $
   Date        : $Date: 2005/7/13 $       
******************************************************************************/


/* ============================= */
/* Global Structures             */
/* ============================= */
typedef struct _DUSLIC_GPIO  DUSLIC_GPIO;
typedef struct _DUSLIC_GPIO_RES DUSLIC_GPIO_RES;

/** GPIO structure 
\remarks
	The driver uses the same mechanism and structure for device GPIO and channel
	IO, called GPIO resource. 
*/
struct _DUSLIC_GPIO
{
    /** Handle to DUSLIC device or CHANNEL stucture */
    INT devHandle;
    /** Callback for IRQ handler */
    void (*gpioCallback)(INT nDev, INT nCh, WORD nEvt);      
    UINT16 nReserve;        
    INT nIO;
    /** Error code for GPIO resource */
    INT err;
};

/** GPIO resource structure 
\remarks
	The driver uses the same mechanism and structure for device GPIO and channel
	IO, called GPIO resource. 
*/
struct _DUSLIC_GPIO_RES
{
   /** Mask of reserved bits for all resources (0 = free, 1 = reserved) */
   UINT16 nGlobalReserve;                               
   
   /** Control struct for the single GPIOs */
   DUSLIC_GPIO Gpio[5];
};

/* ============================= */
/* Global function declaration   */
/* ============================= */
void DUSLIC_GpioInit(INT devHandle);
void DUSLIC_GpioIntDispatch(INT devHandle, IFX_uint8_t nMask);


