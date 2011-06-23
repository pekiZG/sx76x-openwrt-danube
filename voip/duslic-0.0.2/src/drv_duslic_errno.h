#ifndef _DRV_ERRNO_H
#define _DRV_ERRNO_H
/****************************************************************************
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
 ****************************************************************************
   Module      : drv_errno.h
   Date        : 2002/08/08
*******************************************************************************/

/** \file This file contains error number definitions */

/* ============================= */
/* Global Defines                */
/* ============================= */

/** macro to signal and set an error. The error may also generate a
   trigger signal */
#ifdef TESTING
	#define SET_ERROR(no)                  \
	do {                                   \
	   Board_SetLED (BOARD_LED0, 1);       \
	   pDev->err = no;                     \
	   pDev->nErrLine = __LINE__;          \
	   strcpy (pDev->sErrFile, __FILE__);  \
	} while(0)
#else /* #ifdef TESTING */
		/* Customer specific definition.
		   This solution has to be a provisional solution
		   and has to be changed */
		#define SET_ERROR(no)               \
		   do {                             \
		      pDev->err = no;               \
	      } while(0)
#endif /* #else #ifdef TESTING */


/*TRACE (VINETIC, DBG_LEVEL_HIGH, ("Error in %s (%d)\n\r", __FILE__, __LINE__ ));*/

/** \defgroup ErrorCodes Driver and Chip Error Codes
 * @{
 */
enum DEV_ERR {
   /** 0x0: no error */
   ERR_OK = 0,
   /** 0x1: command error reported by vinetic, see last command */
   ERR_CERR,
   /** 0x2: command inbox overflow reported by vinetic */
   ERR_CIBX_OF,
   /** 0x3: host error reported by vinetic */
   ERR_HOST,
   /** 0x4: MIPS overload */
   ERR_MIPS_OL,
   /** 0x5: no command data received event within timeout.
   This error is obsolete, since the driver used a polling mode */
   ERR_NO_COBX,
   /** 0x6: no command data received within timeout */
   ERR_NO_DATA,
   /** 0x7: not enough inbox space for writing command */
   ERR_NO_FIBXMS,
   /** 0x8: more data then expected in outbox */
   ERR_MORE_DATA,
   /** 0x9: Mailbox was not empty after timeout. This error occurs while the
       driver tries to switch the mailbox sizes before and after the
       firmware download. The timeout is given in the constant WAIT_MBX_EMPTY */
   ERR_NO_MBXEMPTY,
   /** 0xA: download ready event has not occured */
   ERR_NO_DLRDY,
   /** 0xB: register read: expected values do not match */
   ERR_WRONGDATA,
   /** 0xC: OBXML is zero after COBX-DATA event, wrong behaviour of VINETIC
   After event OBXML must indicate data. This error is obsolete,
   since the driver used a polling mode */
   ERR_OBXML_ZERO,
   /** 0xD: Test chip access failed */
   ERR_TEST_FAIL,
   /** 0xE: Internal EDSP hardware error reported by
   VINETIC in HWSR1:HW-ERR */
   ERR_HW_ERR,
	/** 0xF: Mailbox Overflow Error */
	ERR_PIBX_OF,
   /** 0x10: invalid parameter in function call */
   ERR_FUNC_PARM,
   /** 0x11: timeout while waiting on channel status change */
   ERR_TO_CHSTATE,
   /** 0x12: buffer underrun in evaluation downstreaming */
   ERR_BUF_UN,
   /** 0x13: no memory by memory allocation */
   ERR_NO_MEM,
   /** 0x14: board previously not initialized */
   ERR_NOINIT,
   /** 0x15: interrupts can not be cleared */
   ERR_INTSTUCK,
   /** 0x16: line testing measurement is running */
   ERR_LT_ON,
   /** 0x17: PHI patch wasn't successfully downloaded. The problem was an chip
      access problem */
   ERR_NOPHI,
   /** 0x18: EDSP Failures */
   ERR_EDSP_FAIL,
   /** 0x19: CRC Fail while download */
   ERR_FWCRC_FAIL,
   /** 0x1A: TAPI not initialized */
   ERR_NO_TAPI,
   /** 0x1B: Error while using SPI Inteface */
   ERR_SPI,
   /** 0x1C: inconsistent or invalid parameters were provided */
   ERR_INVALID,
   /** 0x1D: no Data to copy to user space for GR909 measurement */
   ERR_GR909,
   /** 0x1E: CRC Fail while ac download for V1.4 */
   ERR_ACCRC_FAIL,
   /** 0x1F: couldn't read out chip version */
   ERR_NO_VERSION,
   /** 0x20: CRC Fail in DCCTRL download */
   ERR_DCCRC_FAIL,
   /** 0x21: unknown chip version */
   ERR_UNKNOWN_VERSION,
   /** 0x22: Linetesting, line is in Power Down High Impedance, measurement not possible */
   ERR_LT_LINE_IS_PDNH,
   /** 0x23: Linetesting, unknown Parameter */
   ERR_LT_UNKNOWN_PARAM,
   /** 0x24: Error while sending CID */
   ERR_CID_TRANSMIT,
   /** 0x25: Linetesting, timeout waiting for LM_OK */
   ERR_LT_TIMEOUT_LM_OK,
   /** 0x26: Linetesting, timeout waiting for RAMP_RDY */
   ERR_LT_TIMEOUT_LM_RAMP_RDY,
   /* 0x27: PRAM firmware not ok */
   ERR_PRAM_FW,
   /** 0x28: no firmware specified and not included in driver */
   ERR_NOFW,
   /** 0x29: PHI CRC is zero */
   ERR_PHICRC0,
   /** 0x2A: Arc download failed */
   ERR_ARCDWLD_FAIL,
   /** 0x2B: Arc boot failed after download */
   ERR_ARCDWLD_BOOT,
   /** 0x2C: Firmware binary is invalid */
   ERR_FWINVALID,
   /** 0x2D: Firmware version could not be read, no answer to command */
   ERR_NOFWVERS,
   /** 0x2E: Maximize mailbox failed */
   ERR_NOMAXCBX,
   /** 0x2F: Signaling module not enabled */
   ERR_SIGMOD_NOTEN,
   /** 0x30: Signaling channel not enabled */
   ERR_SIGCH_NOTEN,
   /** 0x31: coder configuration not valid */
   ERR_CODCONF_NOTVALID,
   /** 0x32: Linetesting, optimum result routine failed */
   ERR_LT_OPTRES_FAILED,
   /** 0x33: No free input found while connecting cod, sig and alm modules */
   ERR_NO_FREE_INPUT_SLOT,
   /** 0x34: feature or combination not supported */
   ERR_NOTSUPPORTED,
   /** 0x35: resource not available */
   ERR_NORESOURCE,
   /* add here ^ */
   /*---------------------------- severe errors ------------------------------*/
   /** driver initialization failed */
   ERR_DRVINIT_FAIL = 0x80,
   /** 0x81: general access error, RDQ bit is always 1 */
   ERR_DEV_ERR
};

/* }@ */
#endif /* _DRV_ERRNO_H */


