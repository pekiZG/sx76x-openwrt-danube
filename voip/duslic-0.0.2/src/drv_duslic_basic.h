/*
** Copyright (C) 2005 infineon <Qi-Ming.Wu@infineon.com>
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
******************************************************************************
   Module      : drv_duslic_basic.h
   Date        : 2005/08/21
   Description : This file contains the declaration of the functions
                 for usual vinetic operations.
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

//IFX_int32_t DUSLIC_Write_Sc       (DUSLIC_DEVICE *pDev, DUSLIC_IO_WRITE_SC* pCmd);
//IFX_int32_t DUSLIC_Read_Sc        (DUSLIC_DEVICE *pDev, DUSLIC_IO_READ_SC* pCmd);
IFX_int32_t DUSLIC_Read_Cmd       (DUSLIC_DEVICE *pDev, DUSLIC_IO_MB_CMD* pCmd);
IFX_int32_t DUSLIC_Write_Cmd      (DUSLIC_DEVICE *pDev, DUSLIC_IO_MB_CMD* pCmd);
IFX_int32_t DUSLIC_Write_Reg      (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd);
IFX_int32_t DUSLIC_Read_Reg       (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd);
IFX_int32_t DUSLIC_Ciop           (DUSLIC_CHANNEL * pCh, DUSLIC_IO_REG_CMD * pCmd);




