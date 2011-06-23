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
   Module      : duslic_io.h
   Date        : 2005-07-11
   Description : This file contains defines, structures declarations for
                 the driver user interface
   Remarks:
      uses ANSI c types: char for 8 bit, short for 16 bit, long for 32 bit
*******************************************************************************/

#ifndef _DUSLIC_IO_H
#define _DUSLIC_IO_H

#define MAX_CMD_BYTE 256 /*Fix me, find the largest command bytes it might be*/
#define MAX_RECEIVE_DATA_SIZE 256

#define CMD_RD_WRITE 0x0
#define CMD_RD_READ  0x80
#define CMD_OP_REG   0x40
#define CMD_OP_CIOP  0X0


#define DUS_IO_INIT_NO_CRAM_DWLD    0x01
#define DUS_IO_INIT_IO_EMUL         0x02
#define DUS_IO_INIT_TEST1           0x04
#define DUS_IO_INIT_TEST2           0x08
#define DUS_IO_INIT_DEFAULT         0xf0


#define SOP_CMD   4    
#define POP_CMD   5
#define COP_CMD   6
#define READ      0
#define WRITE     1


#define PDRx            7
#define PDH             0
#define ANY_ACTIVE      2
#define RING            5
#define ACTIVE_METERING 6
#define GROUND_START    4
#define RING_PAUSE      1

/* ============================= */
/* DUSLIC ioctl access          */
/* ============================= */

/* general mode settings (set or get information) */
#define 	IOSET  0
#define	IOGET  1

/* ============================= */
/* DUSLIC ioctl Defines         */
/* ============================= */

/* magic number */
#define DUSLIC_IOC_MAGIC 'D'


/* IOCMD global defines */
/* use driver in polling mode */
#define FIO_DUSLIC_DRV_CTRL				   _IO(DUSLIC_IOC_MAGIC,  1)
#define FIO_DUSLIC_POLL_EVT              _IO(DUSLIC_IOC_MAGIC,  2)
/** Write short command, using structure \ref DUSLIC_IO_WRITE_SC
   \note This interface is only for debugging and testing purposes.
   The use of this interface may disturb the driver operation */
#define FIO_DUSLIC_WSC						   _IO(DUSLIC_IOC_MAGIC, 11)
/** Read short command, using structure \ref DUSLIC_IO_READ_SC
   \note This interface is only for debugging and testing purposes.
   The use of this interface may disturb the driver operation */
#define FIO_DUSLIC_RSC						   _IO(DUSLIC_IOC_MAGIC,  4)

#define FIO_DUSLIC_CRC          		   _IO(DUSLIC_IOC_MAGIC,  5)
/** Read relevant version information using structure DUSLIC_IO_VERSION */
#define FIO_DUSLIC_VERS                  _IO(DUSLIC_IOC_MAGIC,  6)
#define FIO_DUSLIC_DRVVERS               _IO(DUSLIC_IOC_MAGIC,  7)
/* get reference to TAPI_CONNECTION, as void pointer */
#define FIO_DUSLIC_GET_CH_REF            _IO(DUSLIC_IOC_MAGIC,  8)
/* PHI download using structure DUSLIC_IO_PHI */
#define FIO_DUSLIC_DOW_PHI               _IO(DUSLIC_IOC_MAGIC,  9)
#define FIO_DUSLIC_INTERLEAVED           _IO(DUSLIC_IOC_MAGIC, 10)
#define FIO_DUSLIC_POLLED                _IO(DUSLIC_IOC_MAGIC, 11)

/** DUSLIC Board Configuration. Will be removed soon */
#define FIO_DUSLIC_BCONF                 _IO(DUSLIC_IOC_MAGIC, 12)
/** Set the driver report levels if the driver is compiled with
    ENABLE_TRACE */
#define FIO_DUSLIC_REPORT_SET            _IO(DUSLIC_IOC_MAGIC, 13)
/** Write command, using structure \ref DUSLIC_IO_MB_CMD
   \note This interface is only for debugging and testing purposes.
   The use of this interface may disturb the driver operation */
#define FIO_DUSLIC_WCMD                  _IO(DUSLIC_IOC_MAGIC, 14)
/** Read command, using structure \ref DUSLIC_IO_MB_CMD
   \note This interface is only for debugging and testing purposes.
   The use of this interface may disturb the driver operation */
#define FIO_DUSLIC_RCMD                  _IO(DUSLIC_IOC_MAGIC, 15)
/* FPI download */
#define FIO_DUSLIC_DOWNFPI               _IO(DUSLIC_IOC_MAGIC, 16)
/** DUSLIC Initialization. If the driver is compiled with TAPI support
    the interface IFXPHONE_INIT should be used */
#define FIO_DUSLIC_INIT                  _IO(DUSLIC_IOC_MAGIC, 17)
/** Driver runtime tracing of register accesses */
#define FIO_DUSLIC_RUNTIME_TRACE_SET     _IO(DUSLIC_IOC_MAGIC, 18)
/** Download cram coefficients to one or all DUSLIC channels.
   The parameter is a pointer to a \ref DUSLIC_IO_CRAM structure
   \code
   DUSLIC_IO_CRAM         ioCram;
   unsigned  int           nCh;
   int                     err = SUCCESS;

   memset(&ioCram, 0, sizeof(DUSLIC_IO_CRAM));

   ioCram.bBroadCast  = 1;
   ioCram.nFormat     = DUSLIC_IO_CRAM_FORMAT_2;
   ioCram.nStartAddr  = DUSLIC_COP_START_ADDRESS;
   ioCram.nLength     = sizeof(DUSLIC_COEFF) / 2;
   ioCram.nCRC        = 0;
   ioCram.bcr1.nData  = DUSLIC_COEFF_BCR[0][0];
   ioCram.bcr1.nMask  = DUSLIC_COEFF_BCR[0][1];
   ioCram.bcr2.nData  = DUSLIC_COEFF_BCR[1][0];
   ioCram.bcr2.nMask  = DUSLIC_COEFF_BCR[1][1];
   ioCram.tstr2.nData = DUSLIC_COEFF_TSTR2[0][0];
   ioCram.tstr2.nMask = DUSLIC_COEFF_TSTR2[0][1];

   memcpy (ioCram.aData, DUSLIC_COEFF, sizeof(DUSLIC_COEFF));
   err = ioctl (fd, FIO_DUSLIC_DOWNLOAD_CRAM, (INT) &ioCram);
   \endcode
   */
#define FIO_DUSLIC_DOWNLOAD_CRAM         _IO(DUSLIC_IOC_MAGIC, 19)

/** Get the last occured error. The error codes are listed in
   drv_DUSLIC_errno.h and are enumerated in \ref DEV_ERR
   \code
   ioctl (fd, FIO_DUSLIC_LASTERR, (INT) &err);
   \endcode
    */
#define FIO_DUSLIC_LASTERR               _IO(DUSLIC_IOC_MAGIC, 20)

/* vxworks provides no methode to differentiate between read and  */
/* exception filedescriptors, which is necessary e.g. for H.323   */
/* After wakeup vxworks has to determine what the source of the   */
/* wakeup() call was, which is done by calling the                */
/* FIO_DUSLIC_POLL-ioctl() after returning from select()         */
#define FIO_DUSLIC_POLL                  _IO(DUSLIC_IOC_MAGIC, 190)

/** @} */

/** @defgroup VIN_DRIVER_INTERFACE_LT Line Testing Interface
    Interfaces for low level line testing functions */
/** @{ */

/* Linetesting Ioctl Commands */
#ifdef TAPI

/* GR909 realtime measurements */
#define FIO_DUSLIC_GR909_START           _IO(DUSLIC_IOC_MAGIC, 21)
#define FIO_DUSLIC_GR909_STOP            _IO(DUSLIC_IOC_MAGIC, 22)
#define FIO_DUSLIC_GR909_RESULT          _IO(DUSLIC_IOC_MAGIC, 23)
/* FAULT current detection */
#define FIO_DUSLIC_FAULTCURR_SWITCHSLIC  _IO(DUSLIC_IOC_MAGIC, 24)
#endif /* TAPI */
/* Offset Calibration */
#define FIO_DUSLIC_OFFSET_CALIBRATION    _IO(DUSLIC_IOC_MAGIC, 25)
/* Linetesting 2.0 Ioctl Commands */
#define FIO_DUSLIC_LT_CURRENT            _IO(DUSLIC_IOC_MAGIC, 26)
#define FIO_DUSLIC_LT_VOLTAGE            _IO(DUSLIC_IOC_MAGIC, 27)
#define FIO_DUSLIC_LT_RESISTANCE         _IO(DUSLIC_IOC_MAGIC, 28)
#define FIO_DUSLIC_LT_CAPACITANCE        _IO(DUSLIC_IOC_MAGIC, 29)
#define FIO_DUSLIC_LT_IMPEDANCE          _IO(DUSLIC_IOC_MAGIC, 30)
#define FIO_DUSLIC_LT_AC_MEASUREMENTS    _IO(DUSLIC_IOC_MAGIC, 31)
#define FIO_DUSLIC_LT_AC_DIRECT          _IO(DUSLIC_IOC_MAGIC, 32)
#define FIO_DUSLIC_LT_RESISTANCE_NET     _IO(DUSLIC_IOC_MAGIC, 33)
#define FIO_DUSLIC_LT_RESISTANCE_RC      _IO(DUSLIC_IOC_MAGIC, 34)
                                               /* reserved until 40
                                                  for Linetesting */
#define FIO_DUSLIC_LT_CONFIG             _IO(DUSLIC_IOC_MAGIC, 40)
/** @} */

/** \addtogroup VIN_DRIVER_INTERFACE_GPIO GPIO Interface */
/** @{ */

/** Configure Configure device GPIO pins 0..7. Will be replaced by
   \ref FIO_DUSLIC_GPIO_CONFIG.
   Use structure \ref DUSLIC_IO_DEV_GPIO_CFG */
#define FIO_DUSLIC_DEV_GPIO_CFG          _IO(DUSLIC_IOC_MAGIC, 41)
/** Set GPIO pins values. Will be replaced by
   \ref FIO_DUSLIC_GPIO_CONFIG. */
#define FIO_DUSLIC_DEV_GPIO_SET          _IO(DUSLIC_IOC_MAGIC, 42)
/** Reserve GPIO pins for use. Use structure \ref DUSLIC_IO_GPIO_CONTROL */
#define FIO_DUSLIC_GPIO_RESERVE          _IO(DUSLIC_IOC_MAGIC, 43)
/** Configure GPIO pins. Use structure \ref DUSLIC_IO_GPIO_CONTROL */
#define FIO_DUSLIC_GPIO_CONFIG           _IO(DUSLIC_IOC_MAGIC, 44)
/** Set GPIO pin values. Use structure \ref DUSLIC_IO_GPIO_CONTROL */
#define FIO_DUSLIC_GPIO_SET              _IO(DUSLIC_IOC_MAGIC, 45)
/** Get GPIO pin values. Use structure \ref DUSLIC_IO_GPIO_CONTROL */
#define FIO_DUSLIC_GPIO_GET              _IO(DUSLIC_IOC_MAGIC, 46)
/** Set GPIO pin values. Use structure \ref DUSLIC_IO_GPIO_CONTROL */
#define FIO_DUSLIC_GPIO_RELEASE          _IO(DUSLIC_IOC_MAGIC, 47)

#define FIO_DUSLIC_READ_REG             _IO(DUSLIC_IOC_MAGIC, 48)
#define FIO_DUSLIC_WRITE_REG            _IO(DUSLIC_IOC_MAGIC, 49)
#define FIO_DUSLIC_CIOP_CMD             _IO(DUSLIC_IOC_MAGIC, 50)

#define FIO_DUSLIC_PCM_CONFIG           _IO(DUSLIC_IOC_MAGIC, 51)

#define FIO_DUSLIC_PCM_ACTIVE           _IO(DUSLIC_IOC_MAGIC, 52)



/** @} */

/** Packet Header Masks for channel number */
#define VIN_BUF_HDR1_CH         0x0003
/** Packet Header Masks for device number */
#define VIN_BUF_HDR1_DEV        0x00FC
/** Packet Header Masks for payload length in words */
#define VIN_BUF_HDR2_LEN        0x00FF
/** Packet Header Masks for odd bit
    (set if the last byte in the last word is padding) */
#define VIN_BUF_HDR2_ODD        0x2000

/* ============================= */
/* RTP packet defines            */
/* ============================= */

#define RTP_PT 0x007F

#define DUSLIC_IO_LINEMODE_MASK 0x0F00
#define DUSLIC_IO_LINESUB_MASK  0x00F0

/** Version Io structure */
typedef struct
{
   /** chip type */
   unsigned char nType;
   /** number of supported analog channels */
   unsigned char nChannel;
   unsigned short nChip;
   /** included TAPI version */
   unsigned long nTapiVers;
   /** driver version */
   unsigned long nDrvVers;
   /** EDSP major version */
   unsigned short  nEdspVers;
   /** EDSP version step */
   unsigned short  nEdspIntern;
} DUSLIC_IO_VERSION;

/* ============================= */
/* Initialization                */
/* ============================= */

/** structure usded for device initialization
 */
typedef struct
{
   unsigned char *p_ac_coef;
   unsigned long ac_coef_size;
   unsigned char *p_dc_coef;
   unsigned long dc_coef_size;
   unsigned char *p_gen_coef;
   unsigned long gen_coef_size;
   unsigned long param;
} DUSLIC_IO_INIT;


/* ============================= */
/* Basic Access                  */
/* ============================= */

/** IO structure for read short commands
  */
typedef struct
{
   /** if set to 1 a broadcast on all channel will be done.
      Not available for every command */
   unsigned char nBc;
   /** write command */
   unsigned short nCmd;
   /** channel id */
   unsigned char nCh;
   /** read words count */
   unsigned char count;
   /** read data */
   unsigned short pData [256];
} DUSLIC_IO_READ_SC;

/** IO structure for writing short commands */
typedef struct
{
   /** if set to 1 a broadcast on all channel will be done.
      Not available for every command */
   unsigned char nBc;
   /** write command */
   unsigned short nCmd;
   /** channel id */
   unsigned char nCh;
} DUSLIC_IO_WRITE_SC;

/** IO structure for write other commands */
typedef struct
{
   /** write command 1 */
   unsigned short cmd1;
   /** write command 2 */
   unsigned short cmd2;
   /** read or write data */
   unsigned short pData [256];
} DUSLIC_IO_MB_CMD;


typedef struct
{
   /** write command 1 */
   unsigned char cmd;
   /** write command 2 */
   unsigned char offset;
   /** read or write data */
   unsigned char pData[32];
   /* the number of registers*/
   int count;
} DUSLIC_IO_REG_CMD;


/*===================================
 * PCM configuration 
 *=================================*/
enum DS_PCM_HIGHWAY
{
    DS_PCM_HIGHWAY_A,
    DS_PCM_HIGHWAY_B
};

enum DS_PCM_RESOLUTION
{
    DS_PCM_A_LAW_8_BIT,
    DS_PCM_U_LAW_8_BIT,
    DS_PCM_LINEAR_16_BIT
};

enum DS_PCM_SAMPLE_RATE
{
    DS_PCM_SAMPLE_8K,
    DS_PCM_SAMPLE_16K
};

enum DS_PCM_CLOCK_MODE
{
    DS_PCM_SINGLE_CLOCK,
    DS_PCM_DOUBLE_CLOCK
};

enum DS_PCM_SLOPE
{   
    DS_PCM_TX_RISING    = 0x01,
    DS_PCM_TX_FALLING   = 0x02,
    DS_PCM_RX_RISING    = 0x04,
    DS_PCM_RX_FALLING   = 0x08
};

enum DS_PCM_DRIVEN_MODE
{
    DS_PCM_DRIVEN_ENTIRE,
    DS_PCM_DRIVEN_FIRST_HALF
};

enum DS_PCM_DATA_SHIFT
{
    DS_PCM_NO_SHIFT,
    DS_PCM_SHIFT_1T,
    DS_PCM_SHIFT_2T,
    DS_PCM_SHIFT_3T,
    DS_PCM_SHIFT_4T,
    DS_PCM_SHIFT_5T,
    DS_PCM_SHIFT_6T,
    DS_PCM_SHIFT_7T
};


/** Structure for PCM configuration  */
typedef struct
{
   /** PCM timeslot for the receive direction */
   unsigned long             nTimeslotRX;
   /** PCM timeslot for the transmit direction */
   unsigned long             nTimeslotTX;
   /** Defines the PCM highway number which is connected to the channel */
   unsigned long             nTxHighway;
   unsigned long             nRxHighway;
   /** defines the PCM interface coding

   - 0: A_LAW_8_BIT, 8 bit A law
   - 1: U_LAW_8_BIT, 8 bit u Law
   - 2: LINEAR_16_BIT, 16 bit linear */
   unsigned long             nResolution;
   /** Defines the PCM sample rate in kHz. */
   unsigned long             nRate;
   /** Clock mode **/
   unsigned long             nClockMode;
   /** Rx and Tx slope **/
   unsigned long             nTxRxSlope;
   /** Driving Mode **/
   unsigned long             nDriveMode;
   /** Shift period **/
   unsigned long             nShift;
} DUSLIC_PCM_CONFIG;




/* ============================= */
/* Board Configuration           */
/* ============================= */

typedef struct
{
   unsigned char nMode;
   unsigned char nHwReset;
   unsigned char nCMDREG1;
   unsigned char nCMDREG2;
   unsigned char nCMDREG3;
   unsigned char nCMDREG4;
   unsigned char nCMDREG5;
   int nBusMode;
   unsigned char nVersionReg;
   unsigned char nSlidReg;
} DUSLIC_IO_BOARD_CONF;

/* ============================= */
/* Download FPI                  */
/* ============================= */

typedef struct
{
	/* FPI Start Address */
	unsigned long nStartAddr;
	/* FPI Stop Address */
	unsigned long nStopAddr;
	/* Data Size*/
	int nSize;
	/* Data Buffer of 16 kB */
	unsigned short  *pData;
	/*return value: CRC*/
	unsigned short  nCrc;
} DUSLIC_IO_FPI_DOWNLOAD;


/* ============================= */
/* Download PHI                  */
/* ============================= */
typedef struct
{
   /* Data Buffer for PHI */
   unsigned short *pData;
   /* Data Size */
   unsigned long nSize;
   /* returned PHI CRC read from DUSLIC */
   unsigned short nCrc;
} DUSLIC_IO_PHI;


/* ============================= */
/* Download CRAM                 */
/* ============================= */
#define DUSLIC_IO_CRAM_DATA_SIZE     250    /* Words */
/** CRAM File Format */
typedef enum _DUSLIC_IO_CRAM_FORMAT
{
   /** Format for V1.6 and older */
   DUSLIC_IO_CRAM_FORMAT_1,
   /** Format for DUSLIC V2.1 */
   DUSLIC_IO_CRAM_FORMAT_2,
   DUSLIC_IO_CRAM_FORMAT_2_1,
   DUSLIC_IO_CRAM_FORMAT_2_2
} DUSLIC_IO_CRAM_FORMAT;

/** CRAM Download related setting of register values (used for activation) */
typedef struct
{
   unsigned short             nData;
   unsigned short             nMask;
} DUSLIC_IO_CRAM_REG_CFG;

/** Structure for CRAM download with the V1.4 and the V2.x format.
   This structure is used for the ioctl \ref FIO_DUSLIC_DOWNLOAD_CRAM */
typedef struct
{
   unsigned short             bBroadCast;
   DUSLIC_IO_CRAM_FORMAT     nFormat;
   unsigned char              nStartAddr;
   unsigned long              nLength;
   unsigned short             aData [DUSLIC_IO_CRAM_DATA_SIZE];
   DUSLIC_IO_CRAM_REG_CFG    bcr1;
   DUSLIC_IO_CRAM_REG_CFG    bcr2;
   DUSLIC_IO_CRAM_REG_CFG    tstr2;
   unsigned short             nCRC;
} DUSLIC_IO_CRAM;

/** ARC Download structure */
typedef struct
{
   /** Data buffer */
   unsigned long *pData;
   /** Amount of Data Size in bytes */
   unsigned long nSize;
} DUSLIC_IO_ARC;

/** @} */


/* ============================= */
/*    L i n e t e s t i n g      */
/* ============================= */

/** \defgroup VIN_DRIVER_INTERFACE_LT
 @{ */

/** Linetesting Configuration Mode */
typedef enum
{
  DUSLIC_IO_LT_CONFIG_READ,
  DUSLIC_IO_LT_CONFIG_WRITE
} DUSLIC_IO_LT_CONFIG_MODE;

/** Linetesting Configuration */
typedef struct
{
   DUSLIC_IO_LT_CONFIG_MODE  access;
   unsigned short             nCurrentIntegrationTime;
   unsigned short             nVoltageIntegrationTime;
   unsigned short             nResistanceIntegrationTime;
} DUSLIC_IO_LT_CONFIG;

/** Linetesting Magnitude Processing */
typedef enum _DUSLIC_IO_LT_RS
{  /* do not change the order of elements */
   /** activate a squarer before the levelmeter */
   DUSLIC_IO_LT_RS_SQUARER,
   /** activate a rectifier before the levelmeter */
   DUSLIC_IO_LT_RS_RECTIFIER,
   /** deactivate level meter magnitude processing */
   DUSLIC_IO_LT_RS_NONE
} DUSLIC_IO_LT_RS;

/** Linetesting Measurement Result */
typedef struct _DUSLIC_IO_LT_RESULT
{
      /** Number of Samples */
      unsigned short    nSamples;
      /** Shift factor used for the measurement\n
          Attention: for DUSLIC 2.1 a negative shift factor means 1/nShift */
      int               nShift;
      /** Level Meter result */
      int               nLMRES;
      /** Gain factor (only used for DUSLIC 1.x) */
      unsigned short    nGain;
      /** Normal or Reverse Polarity */
      unsigned char     bRevPol;
      /** Rectifier, Squarer or None of them was set */
      DUSLIC_IO_LT_RS  rs;
} DUSLIC_IO_LT_RESULT;

/** Linetesting Measurement Result Configuration */
typedef struct _DUSLIC_IO_LT_RESULT_CONFIG
{
   /** Chip Revision */
   unsigned char        nHwRevision;
   /** Linemode when the measurement was done */
   unsigned short       nLineMode;           /* TBD <- should be an enum */
} DUSLIC_IO_LT_RESULT_CONFIG;


/** Linetesting Current Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_CURRENT_MODE
{
   /** select DC Current IT */
   DUSLIC_IO_LT_CURRENT_MODE_IT,
   /** select DC Current IL */
   DUSLIC_IO_LT_CURRENT_MODE_IL
} DUSLIC_IO_LT_CURRENT_MODE;

/** Linetesting Current Measurement Submode Selection */
typedef enum _VINTIC_IO_LT_CURRENT_SUBMODE
{
   /** measure the DC part of the current */
   DUSLIC_IO_LT_CURRENT_SUBMODE_DC,
   /** measure the AC part of the current */
   DUSLIC_IO_LT_CURRENT_SUBMODE_AC
} DUSLIC_IO_LT_CURRENT_SUBMODE;

/** Linetesting Current Measurement */
typedef struct _DUSLIC_IO_LT_CURRENT
{
   /** Current Measurement Mode Selection (IT, IL) */
   DUSLIC_IO_LT_CURRENT_MODE       mode;
   /** Current Measurement Submode Selection (DC, AC) */
   DUSLIC_IO_LT_CURRENT_SUBMODE    submode;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result */
   DUSLIC_IO_LT_RESULT             res;
} DUSLIC_IO_LT_CURRENT;


/** Linetesting Voltage Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_VOLTAGE_MODE
{
   /** Select differential voltage measurement IO4 - IO3 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO4_IO3,
   /** Select differential voltage measurement IO4 - IO2 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO4_IO2,
   /** Select differential voltage measurement IO3 - IO2 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO3_IO2,
   /** Select voltage measurement DCN-DCP */
   DUSLIC_IO_LT_VOLTAGE_MODE_DCN_DCP,
   /** Select voltage measurement on IO4 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO4,
   /** Select voltage measurement on IO3 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO3,
   /** Select voltage measurement on IO2 */
   DUSLIC_IO_LT_VOLTAGE_MODE_IO2,
   /** Select voltage measurement on VDD */
   DUSLIC_IO_LT_VOLTAGE_MODE_VDD
} DUSLIC_IO_LT_VOLTAGE_MODE;

/** Linetesting Voltage Measurement Submode Selection */
typedef enum _DUSLIC_IO_LT_VOLTAGE_SUBMODE
{
   /** Select DC voltage measurement */
   DUSLIC_IO_LT_VOLTAGE_SUBMODE_DC,
   /** Select AC voltage measurement */
   DUSLIC_IO_LT_VOLTAGE_SUBMODE_AC
} DUSLIC_IO_LT_VOLTAGE_SUBMODE;

/** Linetesting Voltage Measurement */
typedef struct _DUSLIC_IO_LT_VOLTAGE
{
   /** Voltage measurement mode selection (LM_SEL) */
   DUSLIC_IO_LT_VOLTAGE_MODE       mode;
   /** Voltage measurement submode slection (AC, DC) */
   DUSLIC_IO_LT_VOLTAGE_SUBMODE    submode;
   /** Voltage measurement select foreign voltage\n
       (if set to TRUE the slic is set to HIRT mode during the measurement) */
   unsigned char                    bForeignVoltage;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result */
   DUSLIC_IO_LT_RESULT             res;
} DUSLIC_IO_LT_VOLTAGE;


/** Linetesting Resistance Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_RESISTANCE_MODE
{
   /** Resistance will be measured between Ring and Tip */
   DUSLIC_IO_LT_RESISTANCE_MODE_RING_TIP,
   /** Resistance will be measured between Ring and Ground */
   DUSLIC_IO_LT_RESISTANCE_MODE_RING_GROUND,
   /** Resistance will be measured between Tip and Ground */
   DUSLIC_IO_LT_RESISTANCE_MODE_TIP_GROUND
} DUSLIC_IO_LT_RESISTANCE_MODE;

/** Linetesting Resistance Measurement */
typedef struct _DUSLIC_IO_LT_RESISTANCE
{
   /** Linetesting Resistance Measurement Mode Selection\n
       (select between Tip/Ring, Tip/Ground, Ring/Ground) */
   DUSLIC_IO_LT_RESISTANCE_MODE    mode;
   /** Linetesting Resistance Measurement Submode Selection\n
       (the application has to tell the driver which io pins
        are connected to Tip/Ring)  */
   DUSLIC_IO_LT_VOLTAGE_MODE       submode;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result first voltage */
   DUSLIC_IO_LT_RESULT             res_U1;
   /** Level Meter Result second voltage
      (not used for off hook resistance measurement) */
   DUSLIC_IO_LT_RESULT             res_U2;
   /** Level Meter Result first current */
   DUSLIC_IO_LT_RESULT             res_I1;
   /** Level Meter Result second current
      (not used for off hook resistance measurement) */
   DUSLIC_IO_LT_RESULT             res_I2;
} DUSLIC_IO_LT_RESISTANCE;

/** Linetesting Resistance Measurement via RC network */
typedef struct _DUSLIC_IO_LT_RESISTANCE_RC
{
   /** Linetesting Resistance Measurement Mode Selection\n
       (select between Tip/Ground and Ring/Ground.\n
        Ring/Tip is not supported!)                                           */
   DUSLIC_IO_LT_RESISTANCE_MODE    mode;
/*   DUSLIC_IO_LT_RESISTANCE_SUBMODE submode;*/
   /** Linetesting Resistance Measurement Submode Selection\n
       (the application has to tell the driver which io pins
        are connected to Tip/Ring)  */
   DUSLIC_IO_LT_VOLTAGE_MODE       iopin;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result first voltage */
   DUSLIC_IO_LT_RESULT             res;
   /** Reciprocal value of the threshold scaling factor */
   int                              reciprocal_scale;
} DUSLIC_IO_LT_RESISTANCE_RC;


/* Linetesting 2.0 Resistance Network Measurement *************************** */
typedef enum _DUSLIC_IO_LT_RESISTANCE_NET_MODE
{
   DUSLIC_IO_LT_RESISTANCE_MODE_NET_HITHIR,
   DUSLIC_IO_LT_RESISTANCE_MODE_NET_HIGHOHM
} DUSLIC_IO_LT_RESISTANCE_NET_MODE;

typedef enum _DUSLIC_IO_LT_RESISTANCE_NET_SUBMODE
{
   _LTEST_NETR_KHIT,
   _LTEST_NETR_KHIR
} DUSLIC_IO_LT_RESISTANCE_NET_SUBMODE;

typedef struct _DUSLIC_IO_LT_RESISTANCE_NET
{
   /* Measurement Parameters */
   DUSLIC_IO_LT_RESISTANCE_NET_MODE       mode;
   DUSLIC_IO_LT_RESISTANCE_NET_SUBMODE    submode;
   DUSLIC_IO_LT_VOLTAGE_MODE              tip;
   DUSLIC_IO_LT_VOLTAGE_MODE              ring;
   int                                     lower_diff_voltage;
   int                                     adapted_voltage;
   /* Measuremet Results */
   DUSLIC_IO_LT_RESULT_CONFIG             cfg;
   /* Ractive */
   DUSLIC_IO_LT_RESULT                 res_Vt1Ract;
   DUSLIC_IO_LT_RESULT                 res_Vr1Ract;
   DUSLIC_IO_LT_RESULT                 res_Vt2Ract;
   DUSLIC_IO_LT_RESULT                 res_IT1Ract;
   DUSLIC_IO_LT_RESULT                 res_Vr2Ract;
   DUSLIC_IO_LT_RESULT                 res_IT2Ract;
   /* kHIT */
   DUSLIC_IO_LT_RESULT                 res_Vtg1kHIT;
   DUSLIC_IO_LT_RESULT                 res_Vrg1kHIT;
   DUSLIC_IO_LT_RESULT                 res_Vtg2kHIT;
   DUSLIC_IO_LT_RESULT                 res_Vrg2kHIT;
   /* kHIR */
   DUSLIC_IO_LT_RESULT                 res_Vtg1kHIR;
   DUSLIC_IO_LT_RESULT                 res_Vrg1kHIR;
   DUSLIC_IO_LT_RESULT                 res_Vtg2kHIR;
   DUSLIC_IO_LT_RESULT                 res_Vrg2kHIR;
   /* rHIR = rRG */
   DUSLIC_IO_LT_RESULT                 res_Vrg1rHIT;
   DUSLIC_IO_LT_RESULT                 res_IT1rHIT;
   DUSLIC_IO_LT_RESULT                 res_Vrg2rHIT;
   DUSLIC_IO_LT_RESULT                 res_IT2rHIT;
   /* rHIR = rTG */
   DUSLIC_IO_LT_RESULT                 res_Vtg1rHIR;
   DUSLIC_IO_LT_RESULT                 res_IT1rHIR;
   DUSLIC_IO_LT_RESULT                 res_Vtg2rHIR;
   DUSLIC_IO_LT_RESULT                 res_IT2rHIR;
} DUSLIC_IO_LT_RESISTANCE_NET;
/* END Linetesting 2.0 Resistance Network Measurement *********************** */

/** Linetesting Capacitance Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_CAPACITANCE_MODE
{
   /** Capacitance will be measured between Ring and Tip */
   DUSLIC_IO_LT_CAPACITANCE_MODE_RING_TIP,
   /** Capacitance will be measured between Ring and Ground */
   DUSLIC_IO_LT_CAPACITANCE_MODE_RING_GROUND,
   /** Capacitance will be measured between Tip and Ground */
   DUSLIC_IO_LT_CAPACITANCE_MODE_TIP_GROUND
} DUSLIC_IO_LT_CAPACITANCE_MODE;

/** Linetesting Capacitance Measurement */
typedef struct _DUSLIC_IO_LT_CAPACITANCE
{
   /** Linetesting Capacitance Measurement Mode Selection\n
       (select beween Tip/Ring, Tip/Ground, Ring/Ground) */
   DUSLIC_IO_LT_CAPACITANCE_MODE   mode;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result (rising and falling slope current) */
   DUSLIC_IO_LT_RESULT             res[2];
   /** returns the ramp slope the driver decided
       to use for this measurement */
   long                             slope;
} DUSLIC_IO_LT_CAPACITANCE;


/** Linetesting  Impedance Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_IMPEDANCE_MODE
{
   /** Impedance will be measured between Ring and Tip */
   DUSLIC_IO_LT_IMPEDANCE_MODE_RING_TIP,
   /** Impedance will be measured between Ring and Ground */
   DUSLIC_IO_LT_IMPEDANCE_MODE_RING_GROUND,
   /** Impedance will be measured between Tip and Ground */
   DUSLIC_IO_LT_IMPEDANCE_MODE_TIP_GROUND
} DUSLIC_IO_LT_IMPEDANCE_MODE;

/** Linetesting  Impedance Measurement Submode Selection */
typedef enum _DUSLIC_IO_LT_IMPEDANCE_SUBMODE
{
   /** Select the Ring Generator as signal source
       for the Impedance measurement */
   DUSLIC_IO_LT_IMPEDANCE_SUBMODE_RG,
   /** Select the Tone Generator as signal source
       for the Impedance measurement */
   DUSLIC_IO_LT_IMPEDANCE_SUBMODE_TG
} DUSLIC_IO_LT_IMPEDANCE_SUBMODE;

/** Linetesting  Impedance Measurement */
typedef struct _DUSLIC_IO_LT_IMPEDANCE
{
   /** Mode Selection\n
       (select between Ring/Tip, Ring/Ground and Tip/Ground) */
   DUSLIC_IO_LT_IMPEDANCE_MODE     mode;
   /** Submode Selection\n
       (select signal source for the Impedance measurement) */
   DUSLIC_IO_LT_IMPEDANCE_SUBMODE  submode;
   /** signal frequency to be used for impedance measurement */
   long                             freq;
   /** signal level to be used for the impedance measurement */
   long                             level;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result */
   DUSLIC_IO_LT_RESULT             res;
} DUSLIC_IO_LT_IMPEDANCE;


/** Bandpass quality selection (DUSLIC 1.x only) */
typedef enum _DUSLIC_IO_LT_BP_QUAL
{
   /** Select bandpass quality 1 */
   DUSLIC_IO_LT_BP_QUAL_1   = 0,
   /** Select bandpass quality 2 */
   DUSLIC_IO_LT_BP_QUAL_2   = 1,
   /** Select bandpass quality 4 */
   DUSLIC_IO_LT_BP_QUAL_4   = 2,
   /** Select bandpass quality 8 */
   DUSLIC_IO_LT_BP_QUAL_8   = 3,
   /** Select bandpass quality 16 */
   DUSLIC_IO_LT_BP_QUAL_16  = 4,
   /** Select bandpass quality 32 */
   DUSLIC_IO_LT_BP_QUAL_32  = 5,
   /** Select bandpass quality 64 */
   DUSLIC_IO_LT_BP_QUAL_64  = 6,
   /** Select bandpass quality 128 */
   DUSLIC_IO_LT_BP_QUAL_128 = 7
} DUSLIC_IO_LT_BP_QUAL;


/** Linetesting AC Measurement Mode Selection */
typedef enum _DUSLIC_IO_LT_AC_MODE
{
   /** Select the PCM4 Level Measurement */
   DUSLIC_IO_LT_AC_MODE_LEVEL,
   /** Select the PCM4 GainTracking Measurement */
   DUSLIC_IO_LT_AC_MODE_GAINTRACKING,
   /** Select the PCM4 Transhybrid Measurement */
   DUSLIC_IO_LT_AC_MODE_TRANSHYBRID,
   /** Select the PCM4 Idle Channel Noise Measurement */
   DUSLIC_IO_LT_AC_MODE_IDLENOISE,
   /** Select the PCM4 SNR Measurement */
   DUSLIC_IO_LT_AC_MODE_SNR
} DUSLIC_IO_LT_AC_MODE;

/** Linetesting AC Measurement */
typedef struct _DUSLIC_IO_LT_AC_MEASUREMENTS
{
   /** Linetesting AC Measurement Mode Selection\n
       (select the PCM4 measurement to be executed) */
   DUSLIC_IO_LT_AC_MODE            mode;
   /** signal frequency to be used for this measurement */
   int                              freq;
   /** signal level to be used for this measurement */
   int                              level;

   /** Level Meter Result Configuration */
   DUSLIC_IO_LT_RESULT_CONFIG      cfg;
   /** Level Meter Result */
   DUSLIC_IO_LT_RESULT             res;
   /** Level Meter Result (only used for SNR measurement) */
   DUSLIC_IO_LT_RESULT             res_noise;
} DUSLIC_IO_LT_AC_MEASUREMENTS;


/** Linetesting AC Direct Measurement Transhybrid Filter Configuration */
typedef enum _DUSLIC_IO_LT_AC_TH_FILTER
{  /* do not change the order of elements */
   /** switch transhybrid filter on during the measurement */
   DUSLIC_IO_LT_AC_TH_FILTER_ON,
   /** switch transhybrid filter off during the measurement */
   DUSLIC_IO_LT_AC_TH_FILTER_OFF
} DUSLIC_IO_LT_AC_TH_FILTER;

/** Linetesting AC Direct Measurement Level Meter Filter Structure */
typedef enum _DUSLIC_IO_LT_AC_LM_FILTER_TYPE
{
   /** select to bypass the level meter filter */
   DUSLIC_IO_LT_AC_LM_FILTER_OFF,
   /** select a bandpass filter structure in the level meter filter */
   DUSLIC_IO_LT_AC_LM_FILTER_BP,
   /** select a notch filter structure in the level meter filter */
   DUSLIC_IO_LT_AC_LM_FILTER_NOTCH,
   #ifdef VIN_V21_SUPPORT
   /** select a low pass filter structure in the level meter filter */
   DUSLIC_IO_LT_AC_LM_FILTER_LP,
   /** select a high pass filter structure in the level meter filter */
   DUSLIC_IO_LT_AC_LM_FILTER_HP,
   #endif /* VIN_V21_SUPPORT */
   /** don't change the filter settings */
   DUSLIC_IO_LT_AC_LM_FILTER_IGNORE
} DUSLIC_IO_LT_AC_LM_FILTER_TYPE;

/** Linetesting AC Direct Measurement Input Selection */
typedef enum _DUSLIC_IO_LT_AC_LM_SELECT
{
   /** select TX path to be measured */
   DUSLIC_IO_LT_AC_LM_SELECT_TX,
   /** select RX path to be measured */
   DUSLIC_IO_LT_AC_LM_SELECT_RX,
   /** select sum of RX and TX path to be measured */
   DUSLIC_IO_LT_AC_LM_SELECT_RX_TX
} DUSLIC_IO_LT_AC_LM_SELECT;

/** Linetesting AC Direct Measurement Level Meter Filter Configuration */
typedef struct _DUSLIC_IO_LT_AC_LM_FILTER
{
   /** select level meter filter structure */
   DUSLIC_IO_LT_AC_LM_FILTER_TYPE  type;
   /** select level meter bandpass quality (only DUSLIC 1.x) */
   DUSLIC_IO_LT_BP_QUAL            qual;
   /** select level meter filter frequency */
   int                              freq;
} DUSLIC_IO_LT_AC_LM_FILTER;

/** Linetesting  AC Direct Measurement Tone Generator State */
typedef enum _DUSLIC_IO_LT_AC_TG_STATE
{  /* do not change the order of elements */
   /** switch tone generator off */
   DUSLIC_IO_LT_AC_TG_STATE_OFF,
   /** switch tone generator on */
   DUSLIC_IO_LT_AC_TG_STATE_ON,
   /** do not change the tone generator configuration */
   DUSLIC_IO_LT_AC_TG_STATE_IGNORE
} DUSLIC_IO_LT_AC_TG_STATE;

/** Linetesting AC Direct Measurement Tone Generator Selection */
typedef enum _DUSLIC_IO_LT_TG
{  /* do not change the order of elements */
   /** select tone generator 1 */
   DUSLIC_IO_LT_TG_1,
   /** select tone generator 2 */
   DUSLIC_IO_LT_TG_2
} DUSLIC_IO_LT_TG;

/* ************************************************************************** */
typedef enum _DUSLIC_IO_DEV_GPIO
{
   DUSLIC_IO_DEV_GPIO_0               = 0x01,
   DUSLIC_IO_DEV_GPIO_1               = 0x02,
   DUSLIC_IO_DEV_GPIO_2               = 0x04,
   DUSLIC_IO_DEV_GPIO_3               = 0x08,
   DUSLIC_IO_DEV_GPIO_4               = 0x10,
   DUSLIC_IO_DEV_GPIO_5               = 0x20,
   DUSLIC_IO_DEV_GPIO_6               = 0x40,
   DUSLIC_IO_DEV_GPIO_7               = 0x80
} DUSLIC_IO_DEV_GPIO;

typedef enum _DUSLIC_IO_DEV_GPIO_CONTROLL
{
   DUSLIC_IO_DEV_GPIO_CONTROLL_PHI,
   DUSLIC_IO_DEV_GPIO_CONTROLL_RESERVED
} DUSLIC_IO_DEV_GPIO_CONTROLL;

typedef enum _DUSLIC_IO_DEV_GPIO_DRIVER
{
   DUSLIC_IO_DEV_GPIO_DRIVER_DISABLED,
   DUSLIC_IO_DEV_GPIO_DRIVER_ENABLED
} DUSLIC_IO_DEV_GPIO_DRIVER;

typedef struct _DUSLIC_IO_DEV_GPIO_CFG
{
   DUSLIC_IO_DEV_GPIO                   nGPIO;
   DUSLIC_IO_DEV_GPIO_CONTROLL          ctrld;
   DUSLIC_IO_DEV_GPIO_DRIVER            drOut;
   DUSLIC_IO_DEV_GPIO_DRIVER            drIn;
} DUSLIC_IO_DEV_GPIO_CFG;

typedef struct _DUSLIC_IO_DEV_GPIO_SET
{
   unsigned char                         mask;
   unsigned char                         value;
} DUSLIC_IO_DEV_GPIO_SET;

/* ************************************************************************** */
#ifndef USE_VINETIC_GPIO
/* ============================= */
/* GR909 real time measurements  */
/* ============================= */

/* GR909 possible measurements */
enum
{
   _GR909_HAZARDOUS_VOLTAGE = 0,
   _GR909_FOREIGN_FORCE,
   _GR909_RESISTIVE_FAULT,
   _GR909_RECEIVER_OFF_HOOK,
   _GR909_RINGER,
   _MAX_GR909_TESTS
};

/* GR909 status code */
enum
{
   _GR909_TEST_OK              = 0,
   _GR909_TEST_RUNNING         = 0x1,
   _GR909_TEST_FAILED,
   _GR909_MEASUREMENT_FAILED   = 0x07
};

/* measurement parameters enum */
/* of parameters set in output values buffer */
enum
{
/******************/
/* Voltage Tests  */
/******************/
/* HFV = Hazardeous and Foreign Voltages */
   /* Ring/Ground DC, AC, Period */
   _HFV_RG_DC = 0,
   _HFV_RG_AC,
   _HFV_RG_PERIOD,
   /* Tip/Gnd DC, AC, Period */
   _HFV_TG_DC,
   _HFV_TG_AC,
   _HFV_TG_PERIOD,
   /* Tip/Ring DC, AC, Period */
   _HFV_TR_DC,
   _HFV_TR_AC,
   _HFV_TR_PERIOD,
/**************************/
/* Resistive Faults Tests */
/**************************/
/* RF = Resistive Faults */
   _RF_IT_TR_NP = 0,
   _RF_V_TR_NP,
   _RF_IT_TR_RP,
   _RF_V_TR_RP,

   _RF_IT_TG_NP,
   _RF_V_TG_NP,
   _RF_IT_TG_RP,
   _RF_V_TG_RP,

   _RF_IT_RG_NP,
   _RF_V_RG_NP,
   _RF_IT_RG_RP,
   _RF_V_RG_RP,
/**************************/
/* Receiver Offhook Tests */
/**************************/
/* RO = Receiver Offhook */
   /* IT and DCN-DCP feed voltage for
      20 mA DC regulation */
   _RO_20MA_IT = 0,
   _RO_20MA_VOLT,
   /* IT and DCN-DCP feed voltage for
      30 mA DC regulation  */
   _RO_30MA_IT,
   _RO_30MA_VOLT,
/*****************/
/* Ringer  Tests */
/*****************/
/*RNG = Ringer */
   _RNG_AC_RMS = 0,
   _RNG_DC_VOLT,
   _RNG_SAMPLNR,
   _RNG_SHIFT,
   _RNG_IT_RESULT
};

#endif
/* =============================== */
/*    G P I O  I n t e r f a c e   */
/* =============================== */

/** \defgroup VIN_DRIVER_INTERFACE_GPIO GPIO Interface
    Control the device and channel specific IO pins */
/** @{ */

/** GPIO Control Structure */
typedef struct
{
	/** GPIO handle */
	int ioHandle;
	/** GPIO resource mask */
	unsigned short nGpio;
	/** Mask for current command */
	unsigned short nMask;
} DUSLIC_IO_GPIO_CONTROL;

/** }@ */

/** @} */

/** @defgroup VIN_KERNEL_INTERFACE Driver Kernel Interface
    Kernel Interface for GPIO/IO pin handling
  @{  */

/* ======================================= */
/*    E x p o r t e d  V a r i a b l e s   */
/* ======================================= */
/** GPIO Configuration Structure */
typedef struct _DUSLIC_GPIO_CONFIG
{
    /** Mask for GPIO resources */
	unsigned short nGpio;
	/** GPIO mode (input, output, interrupt) */
	unsigned int nMode;
    /** Callback for interrupt routine */
	void (*callback)(int nDev, int nCh, unsigned short nEvt);
} DUSLIC_GPIO_CONFIG;


#ifdef DUSLIC
typedef enum
{
	_GPIO_MODE_INPUT	=	0x0100,
	_GPIO_MODE_OUTPUT =	0x0200,
	_GPIO_MODE_INT =		0x0400,
	_GPIO_INT_RISING	=	0x1000,
	_GPIO_INT_FALLING =	0x2000,
	_GPIO_INT_DUP_05 = 	0x0000,
	_GPIO_INT_DUP_45 = 	0x0001,
	_GPIO_INT_DUP_85 = 	0x0002,
	_GPIO_INT_DUP_125 =	0x0003,
	_GPIO_INT_DUP_165 =	0x0004,
	_GPIO_INT_DUP_205 =	0x0005,
	_GPIO_INT_DUP_245 =	0x0006,
	_GPIO_INT_DUP_285 =	0x0007,
	_GPIO_INT_DUP_325 =	0x0008,
	_GPIO_INT_DUP_365 =	0x0009,
	_GPIO_INT_DUP_405 =	0x000A,
	_GPIO_INT_DUP_445 =	0x000B,
	_GPIO_INT_DUP_485 =	0x000C,
	_GPIO_INT_DUP_525 =	0x000D,
	_GPIO_INT_DUP_565 =	0x000E,
	_GPIO_INT_DUP_605 =	0x000F
} DUSLIC_GPIO_MODE;
#endif

/* ======================================= */
/*    E x p o r t e d  F u n c t i o n s   */
/* ======================================= */
/** used for linux to open the driver */
int DUSLIC_OpenKernel(int nDev, int nCh);
/** used for linux to release the driver */
int DUSLIC_ReleaseKernel(int nHandle);
int DUSLIC_GpioReserve(int devHandle, unsigned short nGpio);
int DUSLIC_GpioRelease(int ioHandle);
int DUSLIC_GpioConfig(int ioHandle, DUSLIC_GPIO_CONFIG *pCfg);
int DUSLIC_GpioSet(int ioHandle, unsigned short nSet, unsigned short nMask);
int DUSLIC_GpioGet(int ioHandle, unsigned short *nGet, unsigned short nMask);
int DUSLIC_GpioIntMask(int ioHandle, unsigned short nSet, unsigned short nMask,
                        unsigned int nMode);

/** }@ */

/* =============================== */
/*    Polling  Interface           */
/* =============================== */

/** \addtogroup VIN_DRIVER_POLLING_INTERFACE  */
/** @{ */

/** Polling Mode IO configuration structure */
typedef struct
{
   /** driver polling/interrupt mode control flag
       \arg 0 configure driver for interrupt mode
       \arg 1 configure driver for polling   mode
   */
   int bPoll;
   /** packets control
       \arg 0 packets read by normal/interleave polling routines
       \arg 1 packets read on event polling
   */
   int bDataEvt;
   /** report events mode
       \arg 0 event read by application
       \arg 1 event read via poll/select
   */
   int bEvtSel;
   /** Pointer to buffer pool control structure. The pointer is used for
       the retrieve and return functions to identify the buffer handlers
       control structure. The buffer pool is expected to be preinitialized */
   void *pBufferPool;
   /** function pointer to retrieve a empty buffer from the pool. The buffer
       is protected and can be exclusively used by the user. The size is
       predefined by the buffer pool handler. */
   void * (* getBuf) (void *pBuf);
   /** function pointer to return a used buffer to the pool */
   int    (* putBuf) (void *pBuf);
} DUSLIC_IO_DRVCTRL;

/* ======================================= */
/*    E x p o r t e d  F u n c t i o n s   */
/* ======================================= */

/** polling control function */
extern int DUSLIC_DrvCtrl  (DUSLIC_IO_DRVCTRL *pCtrl);
/** polling event handling */
extern void  DUSLIC_Poll_Events (void);
/** Normal polling downstream interface functions */
extern int DUSLIC_Poll_Down (const int nElements, void **ppBuffers);
/** Normal polling upstream interface functions */
extern int DUSLIC_Poll_Up   (int *pElements, void **ppBuffers);
/** Interleaved polling downstream interface functions */
extern int DUSLIC_Poll_DownIntlv (const int nElements, void **ppBuffers);
/** Interleaved polling upstream interface functions */
extern int DUSLIC_Poll_UpIntlv   (int *pElements, void **ppBuffers);



#endif /* _DUSLIC_IO_H */
