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
   Module      : drv_duslic_dwnld.h
   Date        : 2003-10-27
   Description : This file contains the declaration of the download and
                 CRC structures, macros and functions.
*******************************************************************************/
#include "drv_duslic_api.h"
/* ============================= */
/* Global function declaration   */
/* ============================= */

extern int Dwld_LoadCram         (DUSLIC_CHANNEL *pCh, DUSLIC_IO_INIT *pInit);
