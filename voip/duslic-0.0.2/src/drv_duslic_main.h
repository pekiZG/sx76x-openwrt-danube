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
*/

/*******************************************************************************
   Module      : drv_duslic_main
   Date        : 2001-01-12
   Description :
      Main driver operations as initializations called in open driver
 *****************************************************************************/

/* ============================= */
/* Global function definition    */
/* ============================= */

extern int  DUSLIC_Init          (DUSLIC_DEVICE *pDev, DUSLIC_IO_INIT *pInit);
extern int  DUSLIC_Set_Pcm       (DUSLIC_CHANNEL *pCh, DUSLIC_PCM_CONFIG *pPcmConf);
extern int  DUSLIC_Pcm_Active    (DUSLIC_CHANNEL *pCh, int status);
extern int  InitDevMember        (DUSLIC_DEVICE *pDev);
extern int  ResetDevMember       (DUSLIC_DEVICE *pDev);
extern int  ResetChMember        (DUSLIC_CHANNEL* pCh);
extern int  InitDuslic           (DUSLIC_DEVICE *pDev);
extern void ResetDevState        (DUSLIC_DEVICE *pDev);
extern void ExitDev              (DUSLIC_DEVICE *pDev);







