/*
** Copyright (C) 2005 Wu Qi Ming <Qi_Ming.Wu@infineon.com>
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
** Foundation, Inc., 59 Temple Place _ Suite 330, Boston, MA 02111_1307, USA.


 ****************************************************************************
   Module      : drv_duslic.h
   Date        : 2005_07_12
   Description :
      This file contains VINETIC specific defines as command defines and
      register definitions.
*******************************************************************************/

/*define register address for SOP */
#define REVISION   0x0
#define CHIPID1    0x01
#define CHIPID2    0x02
#define CHIPID3    0x03
#define FUSE       0x04
#define PCMC1      0x05 
#define XCR        0x06
#define INTREG1    0x07
#define INTREG2    0x08
#define INTREG3    0x09
#define INTREG4    0x0A
#define CHKR1      0x0B
#define CHKR2      0x0C
#define LMRES1     0x0D
#define LMRES2     0x0E
#define FUSE2      0x0F
#define FUSE3      0x10
#define MASK       0x11 
#define IOCTL1     0x12
#define IOCTL2     0x13
#define IOCTL3     0x14
#define BCR1       0x15
#define BCR2       0x16
#define BCR3       0x17
#define BCR4       0x18
#define DSCR       0x1A 
#define LMCR1      0x1C
#define LMCR2      0x1D
#define LMCR3      0x1E
#define OFR1       0x1F
#define OFR2       0x20
#define PCMR1      0x21  
#define PCMX1      0x25
#define TSTR1      0x29  
#define TSTR2      0x2A
#define TSTR3      0x2B
#define TSTR4      0x2C
#define TSTR5      0x2D





/*define bit position as registername_bitname style*/

#define INTREG1_INT_CH       0x80
#define INTREG1_HOOK         0x40   
#define INTREG1_GNDK         0x20
#define INTREG1_GNKP         0x10 
#define INTREG1_ICON         0x08
#define INTREG1_VTRLM        0x04
#define INTREG1_OTEMP        0x02
#define INTREG1_SYNC_FAIL    0x01
#define INTREG2_LM_THRES     0x80
#define INTREG2_READY        0x40
#define INTREG2_RSTAT        0x20
#define INTREG2_LM_OK        0x10
#define INTREG2_IO4_DU       0x08
#define INTREG2_IO3_DU       0x04 
#define INTREG2_IO2_DU       0x02
#define INTREG2_IO1_DU       0x01
#define INTREG3_DTMF_OK      0x80 
#define INTREG3_DTMF_KEY4    0x40
#define INTREG3_DTMF_KEY3    0x20
#define INTREG3_DTMF_KEY2    0x10
#define INTREG3_DTMF_KEY1    0x08
#define INTREG3_DTMF_KEY0    0x04
#define INTREG3_UTDR_OK      0x02
#define INTREG3_UTDX_OK      0x01 
#define INTREG4_EDSP_FAIL    0x80
#define INTREG4_CIS_BOF      0x08
#define INTREG4_CIS_BUF      0x04
#define INTREG4_CIS_REQ      0x02
#define INTREG4_CIS_ACT      0x01

#define BCR3_CRAM_EN         0x01


