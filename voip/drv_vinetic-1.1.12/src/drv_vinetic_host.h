#ifndef _DRV_VINETIC_HOST_H
#define _DRV_VINETIC_HOST_H
/****************************************************************************
       Copyright (c) 2005, Infineon Technologies.  All rights reserved.

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
   Module      : drv_vinetic_host.h
   Date        : 2005-05-10
   Description :
      This file contains VINETIC 2CPE specific defines for
      the host interface.
*******************************************************************************/

/**
 * \defgroup MODULE Parallel_Interface
 */
/*@{*/

/* io region size (should be enough in all modes) */
/* #warning !!! Define appropriate region size for 2CPE! */
#define VINETIC_REGION_SIZE   32

/**
 * \defgroup V2CPE_DATA V2CPE_DATA
 * Data Register for Indirect Access. This register is used to access registers beyond 0x1F if the host does not support a multiplexed bus access.
 */
/*@{*/


/** register offset */
#define V2CPE_DATA 0x00
/** reset value */
#define V2CPE_DATA_RESET 0x0000


/**
 * \defgroup V2CPE_DATA_DATA V2CPE_DATA_DATA
 * Data for Indirect Access. This register is only accessable for prallel port access modes. A write to this registers actually writes the data to the registers indexed by the ADDRESS register. A read from this register actually reads the data pointed to by the ADDRESS register. The ADDR register is incremented by one after the access. The memory layout of the channel specific registers is in ascending order to use the post-increment option.
 */
/*@{*/

/*@}*/ /* V2CPE_DATA_DATA */



/*@}*/ /*  V2CPE_DATA */



/**
 * \defgroup V2CPE_ADDR V2CPE_ADDR
 * Address Register for Indirect Access. The Address Register for Indirect Access is used together with the DATA register.
 */
/*@{*/


/** register offset */
#define V2CPE_ADDR 0x02
/** reset value */
#define V2CPE_ADDR_RESET 0x0000


/**
 * \defgroup V2CPE_ADDR_ADDR V2CPE_ADDR_ADDR
 * Data for Indirect Access. This register is only accessable for prallel port access modes. The register is used as an address for register indirect access modes. See the DATA register for more details. Byte access to this register is supported in order to speed-up chip access.
 */
/*@{*/

/*@}*/ /* V2CPE_ADDR_ADDR */



/*@}*/ /*  V2CPE_ADDR */


/*@}*/


/**
 * \defgroup MODULE Global
 */
/*@{*/



/**
 * \defgroup V2CPE_GLB_CTRL V2CPE_GLB_CTRL
 * Global Control Register. The Global Control Register controls the EDSP core in the system.
 */
/*@{*/


/** register offset */
#define V2CPE_GLB_CTRL 0x04
/** reset value */
#define V2CPE_GLB_CTRL_RESET 0x0103


/**
 * \defgroup V2CPE_GLB_CTRL_WDG V2CPE_GLB_CTRL_WDG
 * EDSP Watchdog.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CTRL_WDG 0x0400
/** get */
#define V2CPE_GLB_CTRL_WDG_GET(reg) (((reg) & V2CPE_GLB_CTRL_WDG) >> 10)
/** set */
#define V2CPE_GLB_CTRL_WDG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CTRL_WDG) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_GLB_CTRL_WDG */




/**
 * \defgroup V2CPE_GLB_CTRL_RST V2CPE_GLB_CTRL_RST
 * EDSP Reset. This register is automatically reset after a write to this register (pulse) In the case of ED=1 the EDSP jumps into the boot loader and the HBOOT field is used to branch to the requested boot loader routine. If ED=0 then the HBOOT field is ignored and the DSP starts directly from the RAM.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CTRL_RST 0x0200
/** get */
#define V2CPE_GLB_CTRL_RST_GET(reg) (((reg) & V2CPE_GLB_CTRL_RST) >> 9)
/** set */
#define V2CPE_GLB_CTRL_RST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CTRL_RST) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_GLB_CTRL_RST */




/**
 * \defgroup V2CPE_GLB_CTRL_ED V2CPE_GLB_CTRL_ED
 * EDSP Start Address.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CTRL_ED 0x0100
/** get */
#define V2CPE_GLB_CTRL_ED_GET(reg) (((reg) & V2CPE_GLB_CTRL_ED) >> 8)
/** set */
#define V2CPE_GLB_CTRL_ED_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CTRL_ED) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_GLB_CTRL_ED */




/**
 * \defgroup V2CPE_GLB_CTRL_EMU V2CPE_GLB_CTRL_EMU
 * EMU Boot Line.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CTRL_EMU 0x0080
/** get */
#define V2CPE_GLB_CTRL_EMU_GET(reg) (((reg) & V2CPE_GLB_CTRL_EMU) >> 7)
/** set */
#define V2CPE_GLB_CTRL_EMU_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CTRL_EMU) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_GLB_CTRL_EMU */




/**
 * \defgroup V2CPE_GLB_CTRL_HCLK V2CPE_GLB_CTRL_HCLK
 * Host EDSP Clock Control.
 */
/*@{*/

#define V2CPE_GLB_CTRL_HCLK_MASK  0x0070

#define V2CPE_GLB_CTRL_HCLK_GET(reg) (((reg)>> 4) & ((1 << 3) - 1))
#define V2CPE_GLB_CTRL_HCLK_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_GLB_CTRL_HCLK_MASK) | ((((1 << 3) - 1) & (val)) << 4) )
/*@}*/ /* V2CPE_GLB_CTRL_HCLK */




/**
 * \defgroup V2CPE_GLB_CTRL_HBOOT V2CPE_GLB_CTRL_HBOOT
 * Host EDSP Boot Control.
 */
/*@{*/

#define V2CPE_GLB_CTRL_HBOOT_MASK    0x000F

#define V2CPE_GLB_CTRL_HBOOT_GET(reg) (((reg)>> 0) & ((1 << 4) - 1))
#define V2CPE_GLB_CTRL_HBOOT_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_GLB_CTRL_HBOOT_MASK) | ((((1 << 4) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_GLB_CTRL_HBOOT */



/*@}*/ /*  V2CPE_GLB_CTRL */



/**
 * \defgroup V2CPE_GLB_CFG V2CPE_GLB_CFG
 * Global Configuration Register. The Global Configuration Register configures static system settings. Almost all bits configure the parallel host interface. The upper and lower half of the register must be programmed with an identical bit pattern, other combinations are invalid.
 */
/*@{*/


/** register offset */
#define V2CPE_GLB_CFG 0x06
/** reset value */
#define V2CPE_GLB_CFG_RESET 0x0000


/**
 * \defgroup V2CPE_GLB_CFG_DREQ_MD V2CPE_GLB_CFG_DREQ_MD
 * DMA Request Mode.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_DREQ_MD 0x8080
/** get */
#define V2CPE_GLB_CFG_DREQ_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_DREQ_MD) >> 7)
/** set */
#define V2CPE_GLB_CFG_DREQ_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_DREQ_MD) | (((val) & 1) << 7) | (((val) & 1) << 7 << 8))

/*@}*/ /* V2CPE_GLB_CFG_DREQ_MD */




/**
 * \defgroup V2CPE_GLB_CFG_DACK_MD V2CPE_GLB_CFG_DACK_MD
 * DMA Acknowledge Mode.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_DACK_MD 0x4040
/** get */
#define V2CPE_GLB_CFG_DACK_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_DACK_MD) >> 6)
/** set */
#define V2CPE_GLB_CFG_DACK_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_DACK_MD) | (((val) & 1) << 6 | (((val) & 1) << 6 << 8))

/*@}*/ /* V2CPE_GLB_CFG_DACK_MD */




/**
 * \defgroup V2CPE_GLB_CFG_RDY_MD V2CPE_GLB_CFG_RDY_MD
 * RDYQ Mode. Push-pull enable for theRDYpin.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_RDY_MD 0x2020
/** get */
#define V2CPE_GLB_CFG_RDY_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_RDY_MD) >> 5)
/** set */
#define V2CPE_GLB_CFG_RDY_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_RDY_MD) | (((val) & 1) << 5) | (((val) & 1) << 5 << 8))

/*@}*/ /* V2CPE_GLB_CFG_RDY_MD */




/**
 * \defgroup V2CPE_GLB_CFG_END_MD V2CPE_GLB_CFG_END_MD
 * Endian Mode. This bit reverses the default endian mode for the interface. The reset behavior for the Motorola Interface is big-endian mode and little-endian for all other interfaces.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_END_MD 0x1010
/** get */
#define V2CPE_GLB_CFG_END_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_END_MD) >> 4)
/** set */
#define V2CPE_GLB_CFG_END_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_END_MD) | (((val) & 1) << 4) | (((val) & 1) << 4 << 8))

/*@}*/ /* V2CPE_GLB_CFG_END_MD */




/**
 * \defgroup V2CPE_GLB_CFG_WO_MD V2CPE_GLB_CFG_WO_MD
 * Write-Order Mode. This bit reverses the default write-order mode for the interface. The reset behavior for the Interface is A0=0 followed by A0=1.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_WO_MD 0x0808
/** get */
#define V2CPE_GLB_CFG_WO_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_WO_MD) >> 3)
/** set */
#define V2CPE_GLB_CFG_WO_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_WO_MD) | (((val) & 1) << 3 | (((val) & 1) << 3 << 8))

/*@}*/ /* V2CPE_GLB_CFG_WO_MD */




/**
 * \defgroup V2CPE_GLB_CFG_FIFO_MD V2CPE_GLB_CFG_FIFO_MD
 * FIFO Mode. This bit controls the FIFO depth of the Voice-In and Command-In FIFOs. This bit must only be toggled when the VI_EMP and CI_EMP bits in the STAT registers are set.
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_FIFO_MD 0x0404
/** get */
#define V2CPE_GLB_CFG_FIFO_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_FIFO_MD) >> 2)
/** set */
#define V2CPE_GLB_CFG_FIFO_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_FIFO_MD) | (((val) & 1) << 2) | (((val) & 1) << 2 << 8))

/*@}*/ /* V2CPE_GLB_CFG_FIFO_MD */




/**
 * \defgroup V2CPE_GLB_CFG_DEL_MD V2CPE_GLB_CFG_DEL_MD
 * RDYQ Delay Mode. RDQ Timing
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_DEL_MD 0x0202
/** get */
#define V2CPE_GLB_CFG_DEL_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_DEL_MD) >> 1)
/** set */
#define V2CPE_GLB_CFG_DEL_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_DEL_MD) | (((val) & 1) << 1) | (((val) & 1) << 1 << 8))

/*@}*/ /* V2CPE_GLB_CFG_DEL_MD */




/**
 * \defgroup V2CPE_GLB_CFG_INT_MD V2CPE_GLB_CFG_INT_MD
 * INT Mode. Interrupt Line Polarity
 */
/*@{*/

/** mask */
#define V2CPE_GLB_CFG_INT_MD 0x0101
/** get */
#define V2CPE_GLB_CFG_INT_MD_GET(reg) (((reg) & V2CPE_GLB_CFG_INT_MD) >> 0)
/** set */
#define V2CPE_GLB_CFG_INT_MD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_GLB_CFG_INT_MD) | (((val) & 1) << 0) | (((val) & 1) << 0 << 8))

/*@}*/ /* V2CPE_GLB_CFG_INT_MD */



/*@}*/ /*  V2CPE_GLB_CFG */


/*@}*/


/**
 * \defgroup MODULE Status
 */
/*@{*/



/**
 * \defgroup V2CPE_STAT V2CPE_STAT
 * Status Register. The Status Register is the main register to gain information about the chip status. The upper 8 bits hold the mailbox status, followed by up to 6 user definable bits for general signalling. The ERR flag indicates a system error and requires a read of the ERR_INT register in order to get detailed error information / acknowledge. If the CHAN flag is set, then the STAT_CHAN register gives detailed information which channels have raised the interrupt. The corresponding channel status registers have then to be read.
 */
/*@{*/


/** register offset */
#define V2CPE_STAT 0x08
/** reset value */
#define V2CPE_STAT_RESET 0x0000


/**
 * \defgroup V2CPE_STAT_VO_RDY V2CPE_STAT_VO_RDY
 * HOST Voice Out-Box Ready.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_VO_RDY 0x8000
/** get */
#define V2CPE_STAT_VO_RDY_GET(reg) (((reg) & V2CPE_STAT_VO_RDY) >> 15)
/** set */
#define V2CPE_STAT_VO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_VO_RDY) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_STAT_VO_RDY */




/**
 * \defgroup V2CPE_STAT_VI_EMP V2CPE_STAT_VI_EMP
 * HOST Voice In-Box Empty.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_VI_EMP 0x2000
/** get */
#define V2CPE_STAT_VI_EMP_GET(reg) (((reg) & V2CPE_STAT_VI_EMP) >> 13)
/** set */
#define V2CPE_STAT_VI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_VI_EMP) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_STAT_VI_EMP */




/**
 * \defgroup V2CPE_STAT_CO_RDY V2CPE_STAT_CO_RDY
 * HOST Command Voice Out-Box Ready.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_CO_RDY 0x0800
/** get */
#define V2CPE_STAT_CO_RDY_GET(reg) (((reg) & V2CPE_STAT_CO_RDY) >> 11)
/** set */
#define V2CPE_STAT_CO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_CO_RDY) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_STAT_CO_RDY */




/**
 * \defgroup V2CPE_STAT_CI_EMP V2CPE_STAT_CI_EMP
 * HOST Command In-Box Empty.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_CI_EMP 0x0200
/** get */
#define V2CPE_STAT_CI_EMP_GET(reg) (((reg) & V2CPE_STAT_CI_EMP) >> 9)
/** set */
#define V2CPE_STAT_CI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_CI_EMP) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_STAT_CI_EMP */




/**
 * \defgroup V2CPE_STAT_DL_RDY V2CPE_STAT_DL_RDY
 * EDSP Download Ready. This bit will be set, after a download has been completed and theVINETIC®‑xis ready.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_DL_RDY 0x0080
/** get */
#define V2CPE_STAT_DL_RDY_GET(reg) (((reg) & V2CPE_STAT_DL_RDY) >> 7)
/** set */
#define V2CPE_STAT_DL_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_DL_RDY) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_STAT_DL_RDY */




/**
 * \defgroup V2CPE_STAT_WOKE_UP V2CPE_STAT_WOKE_UP
 * EDSP Woke Up. Indicates that the VINETIC is ready for programming after deep sleep.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_WOKE_UP 0x0040
/** get */
#define V2CPE_STAT_WOKE_UP_GET(reg) (((reg) & V2CPE_STAT_WOKE_UP) >> 6)
/** set */
#define V2CPE_STAT_WOKE_UP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_WOKE_UP) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_STAT_WOKE_UP */




/**
 * \defgroup V2CPE_STAT_GPIO V2CPE_STAT_GPIO
 * GPIO Interrupt. The bit is set when a GPIO interrupt is detected.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_GPIO 0x0020
/** get */
#define V2CPE_STAT_GPIO_GET(reg) (((reg) & V2CPE_STAT_GPIO) >> 5)
/** set */
#define V2CPE_STAT_GPIO_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_GPIO) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_STAT_GPIO */




/**
 * \defgroup V2CPE_STAT_EXP_STAT V2CPE_STAT_EXP_STAT
 * Status Expansion. The bit is reserfved for future use.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_EXP_STAT 0x0010
/** get */
#define V2CPE_STAT_EXP_STAT_GET(reg) (((reg) & V2CPE_STAT_EXP_STAT) >> 4)
/** set */
#define V2CPE_STAT_EXP_STAT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_EXP_STAT) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_STAT_EXP_STAT */




/**
 * \defgroup V2CPE_STAT_CRC_ERR V2CPE_STAT_CRC_ERR
 * CRC Error. The bit indicates a CRC error and is only valid, when the CRC_RDY bit is set. When the CRC_RDY bit is cleard, then the bit can be ignored.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_CRC_ERR 0x0008
/** get */
#define V2CPE_STAT_CRC_ERR_GET(reg) (((reg) & V2CPE_STAT_CRC_ERR) >> 3)
/** set */
#define V2CPE_STAT_CRC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_CRC_ERR) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_STAT_CRC_ERR */




/**
 * \defgroup V2CPE_STAT_CRC_RDY V2CPE_STAT_CRC_RDY
 * CRC Ready. The bit indicates a CRC calcuation has been completed. When this bit is set the CRC_ERR bit indicates, if the CRC calculation was successful.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_CRC_RDY 0x0004
/** get */
#define V2CPE_STAT_CRC_RDY_GET(reg) (((reg) & V2CPE_STAT_CRC_RDY) >> 2)
/** set */
#define V2CPE_STAT_CRC_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_CRC_RDY) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_STAT_CRC_RDY */




/**
 * \defgroup V2CPE_STAT_ERR V2CPE_STAT_ERR
 * Error Status Pending. This bit is set if at least one bit of the ERR register is set.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_ERR 0x0002
/** get */
#define V2CPE_STAT_ERR_GET(reg) (((reg) & V2CPE_STAT_ERR) >> 1)
/** set */
#define V2CPE_STAT_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_STAT_ERR */




/**
 * \defgroup V2CPE_STAT_CH V2CPE_STAT_CH
 * Channel Status Pending. This bit is set if at least one bit of the channel specific status registers have a status bit set.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_CH 0x0001
/** get */
#define V2CPE_STAT_CH_GET(reg) (((reg) & V2CPE_STAT_CH) >> 0)
/** set */
#define V2CPE_STAT_CH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_CH) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_STAT_CH */



/*@}*/ /*  V2CPE_STAT */



/**
 * \defgroup V2CPE_STAT_INT V2CPE_STAT_INT
 * Status Interrupt Register. The Status Interrupt Register detects all interrupts of the STAT register.
 */
/*@{*/


/** register offset */
#define V2CPE_STAT_INT 0x0C
/** reset value */
#define V2CPE_STAT_INT_RESET 0x0000


/**
 * \defgroup V2CPE_STAT_INT_VO_RDY V2CPE_STAT_INT_VO_RDY
 * Voice Out-Box Ready Interrupt. Since this is a level-sensitive interrupt the bit is identical to the corresponding bit in the STAT register. The bit can be acknowledged by either reading a data word from the Voic Out-Box or by disabling the interrupt source via the corresponding bit in the STAT_IEN register.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_VO_RDY 0x8000
/** get */
#define V2CPE_STAT_INT_VO_RDY_GET(reg) (((reg) & V2CPE_STAT_INT_VO_RDY) >> 15)
/** set */
#define V2CPE_STAT_INT_VO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_VO_RDY) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_STAT_INT_VO_RDY */




/**
 * \defgroup V2CPE_STAT_INT_VO_DATA V2CPE_STAT_INT_VO_DATA
 * Voice Out-Box Data Interrupt. New Data in voice out-box present. This bit is set if the VINETIC has put a new data packet into the voice out-box. The bit is acknowledge when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_VO_DATA 0x4000
/** get */
#define V2CPE_STAT_INT_VO_DATA_GET(reg) (((reg) & V2CPE_STAT_INT_VO_DATA) >> 14)
/** set */
#define V2CPE_STAT_INT_VO_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_VO_DATA) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_STAT_INT_VO_DATA */




/**
 * \defgroup V2CPE_STAT_INT_VI_EMP V2CPE_STAT_INT_VI_EMP
 * Voice In-Box Empty Interrupt. Since this is a level-sensitive interrupt the bit is identical to the corresponding bit in the STAT register. The bit can be acknowledged by either writing a data word to the Voice In-Box or by disabling the interrupt source via the corresponding bit in the STAT_IEN register.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_VI_EMP 0x2000
/** get */
#define V2CPE_STAT_INT_VI_EMP_GET(reg) (((reg) & V2CPE_STAT_INT_VI_EMP) >> 13)
/** set */
#define V2CPE_STAT_INT_VI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_VI_EMP) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_STAT_INT_VI_EMP */




/**
 * \defgroup V2CPE_STAT_INT_VI_DATA V2CPE_STAT_INT_VI_DATA
 * Voice In-Box Data Interrupt. Data in packet in-box processed. This bit is set if the VINETIC has fetchted a new data packet in the packet in-box. The bit is acknowledge when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_VI_DATA 0x1000
/** get */
#define V2CPE_STAT_INT_VI_DATA_GET(reg) (((reg) & V2CPE_STAT_INT_VI_DATA) >> 12)
/** set */
#define V2CPE_STAT_INT_VI_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_VI_DATA) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_STAT_INT_VI_DATA */




/**
 * \defgroup V2CPE_STAT_INT_CO_RDY V2CPE_STAT_INT_CO_RDY
 * Command Out-Box Ready Interrupt. Since this is a level-sensitive interrupt the bit is identical to the corresponding bit in the STAT register. The bit can be acknowledged by either reading a data word from the Command Out-Box or by disabling the interrupt source via the corresponding bit in the STAT_IEN register.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CO_RDY 0x0800
/** get */
#define V2CPE_STAT_INT_CO_RDY_GET(reg) (((reg) & V2CPE_STAT_INT_CO_RDY) >> 11)
/** set */
#define V2CPE_STAT_INT_CO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CO_RDY) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_STAT_INT_CO_RDY */




/**
 * \defgroup V2CPE_STAT_INT_CO_DATA V2CPE_STAT_INT_CO_DATA
 * Command Out-Box Data Interrupt. New Data in command out-box present. This bit is set if the VINETIC has put a new data packet into the command out-box. The bit is acknowledge when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CO_DATA 0x0400
/** get */
#define V2CPE_STAT_INT_CO_DATA_GET(reg) (((reg) & V2CPE_STAT_INT_CO_DATA) >> 10)
/** set */
#define V2CPE_STAT_INT_CO_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CO_DATA) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_STAT_INT_CO_DATA */




/**
 * \defgroup V2CPE_STAT_INT_CI_EMP V2CPE_STAT_INT_CI_EMP
 * Command In-Box Empty Interrupt. Since this is a level-sensitive interrupt the bit is identical to the corresponding bit in the STAT register. The bit can be acknowledged by either writing a data word to the Command In-Box or by disabling the interrupt source via the corresponding bit in the STAT_IEN register.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CI_EMP 0x0200
/** get */
#define V2CPE_STAT_INT_CI_EMP_GET(reg) (((reg) & V2CPE_STAT_INT_CI_EMP) >> 9)
/** set */
#define V2CPE_STAT_INT_CI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CI_EMP) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_STAT_INT_CI_EMP */




/**
 * \defgroup V2CPE_STAT_INT_CI_DATA V2CPE_STAT_INT_CI_DATA
 * Command In-Box Data Interrupt. Data in command in-box processed.This bit is set if the VINETIC has fetched a new data packet in the command in-box. The bit is acknowledge when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CI_DATA 0x0100
/** get */
#define V2CPE_STAT_INT_CI_DATA_GET(reg) (((reg) & V2CPE_STAT_INT_CI_DATA) >> 8)
/** set */
#define V2CPE_STAT_INT_CI_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CI_DATA) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_STAT_INT_CI_DATA */




/**
 * \defgroup V2CPE_STAT_INT_DL_RDY V2CPE_STAT_INT_DL_RDY
 * Download Ready Interrupt. The corresponding bit is set by the EDSP when the download has been finishded. The bits are acknowledged when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_DL_RDY 0x0080
/** get */
#define V2CPE_STAT_INT_DL_RDY_GET(reg) (((reg) & V2CPE_STAT_INT_DL_RDY) >> 7)
/** set */
#define V2CPE_STAT_INT_DL_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_DL_RDY) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_STAT_INT_DL_RDY */




/**
 * \defgroup V2CPE_STAT_INT_WOKE_UP V2CPE_STAT_INT_WOKE_UP
 * Woke Up Interrupt. The corresponding bit is set by the EDSP when after woke-up from deep-sleep. The bits are acknowledged when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_WOKE_UP 0x0040
/** get */
#define V2CPE_STAT_INT_WOKE_UP_GET(reg) (((reg) & V2CPE_STAT_INT_WOKE_UP) >> 6)
/** set */
#define V2CPE_STAT_INT_WOKE_UP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_WOKE_UP) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_STAT_INT_WOKE_UP */




/**
 * \defgroup V2CPE_STAT_INT_GPIO V2CPE_STAT_INT_GPIO
 * GPIO Interrupt. The corresponding bits are set by the EDSP. The bits are acknowledged when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_GPIO 0x0020
/** get */
#define V2CPE_STAT_INT_GPIO_GET(reg) (((reg) & V2CPE_STAT_INT_GPIO) >> 5)
/** set */
#define V2CPE_STAT_INT_GPIO_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_GPIO) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_STAT_INT_GPIO */




/**
 * \defgroup V2CPE_STAT_INT_EXP_STAT V2CPE_STAT_INT_EXP_STAT
 * Expansion Interrupt. The corresponding bits are set by the EDSP. The bits are acknowledged when this register is read.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_EXP_STAT 0x0010
/** get */
#define V2CPE_STAT_INT_EXP_STAT_GET(reg) (((reg) & V2CPE_STAT_INT_EXP_STAT) >> 4)
/** set */
#define V2CPE_STAT_INT_EXP_STAT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_EXP_STAT) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_STAT_INT_EXP_STAT */




/**
 * \defgroup V2CPE_STAT_INT_CRC_ERR V2CPE_STAT_INT_CRC_ERR
 * CRC Error Interrupt. The bit indicates a CRC error and is only valid, when the CRC_RDY bit is set. When the CRC_RDY bit is cleard, then the bit can be ignored.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CRC_ERR 0x0008
/** get */
#define V2CPE_STAT_INT_CRC_ERR_GET(reg) (((reg) & V2CPE_STAT_INT_CRC_ERR) >> 3)
/** set */
#define V2CPE_STAT_INT_CRC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CRC_ERR) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_STAT_INT_CRC_ERR */




/**
 * \defgroup V2CPE_STAT_INT_CRC_RDY V2CPE_STAT_INT_CRC_RDY
 * CRC Ready Interrupt. The bit indicates a CRC calcuation has been completed. When this bit is set the CRC_ERR bit indicates, if the CRC calculation was successful.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CRC_RDY 0x0004
/** get */
#define V2CPE_STAT_INT_CRC_RDY_GET(reg) (((reg) & V2CPE_STAT_INT_CRC_RDY) >> 2)
/** set */
#define V2CPE_STAT_INT_CRC_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CRC_RDY) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_STAT_INT_CRC_RDY */




/**
 * \defgroup V2CPE_STAT_INT_ERR V2CPE_STAT_INT_ERR
 * Error Interrupt Pending. This bit is set if at least one bit of the ERR_INT register has an interrupt pending and the corresponding bit in the ERR_IEN register has the interrupt enabled.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_ERR 0x0002
/** get */
#define V2CPE_STAT_INT_ERR_GET(reg) (((reg) & V2CPE_STAT_INT_ERR) >> 1)
/** set */
#define V2CPE_STAT_INT_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_STAT_INT_ERR */




/**
 * \defgroup V2CPE_STAT_INT_CH V2CPE_STAT_INT_CH
 * Channel Interrupt Pending. This bit is set if at least one STATx bit of the STAT_CHAN register has an interrupt pending.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_INT_CH 0x0001
/** get */
#define V2CPE_STAT_INT_CH_GET(reg) (((reg) & V2CPE_STAT_INT_CH) >> 0)
/** set */
#define V2CPE_STAT_INT_CH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_INT_CH) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_STAT_INT_CH */



/*@}*/ /*  V2CPE_STAT_INT */



/**
 * \defgroup V2CPE_STAT_CHAN V2CPE_STAT_CHAN
 * Status Channel Interrupt Register. The Status Channel Interrupt Register gives a quick overiew of up to 8 ALM and 8 EDSP channels. The register can be used in order to speed-up chip access.
 */
/*@{*/


/** register offset */
#define V2CPE_STAT_CHAN 0x0E
/** reset value */
#define V2CPE_STAT_CHAN_RESET 0x0000


/**
 * \defgroup V2CPE_STAT_CHAN_LINE V2CPE_STAT_CHAN_LINE
 * Analog-Line-Module Channel 1-0 Interrupt Pending. This bit will be set, if at least one bit of the corresponding status registers LINEx_INT has an interrupt pending and the corresponding bit int the LINEx_IENy registers has the interrupt enabled.
 */
/*@{*/

#define V2CPE_STAT_CHAN_LINE_MASK    0x0300

#define V2CPE_STAT_CHAN_LINE_GET(reg) (((reg)>> 8) & ((1 << 2) - 1))
#define V2CPE_STAT_CHAN_LINE_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_STAT_CHAN_LINE_MASK) | ((((1 << 2) - 1) & (val)) << 8) )
/*@}*/ /* V2CPE_STAT_CHAN_LINE */




/**
 * \defgroup V2CPE_STAT_CHAN_EDSP V2CPE_STAT_CHAN_EDSP
 * EDSP Channel 1-0 Interrupt Pending. This bit will be set, if at least one bit of the EDSPx_INTy registers has an interrupt pending and the corresponding bit in the EDSPx_IENy registers has the interrupt enabled.
 */
/*@{*/

#define V2CPE_STAT_CHAN_EDSP_MASK    0x00FF

#define V2CPE_STAT_CHAN_EDSP_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_STAT_CHAN_EDSP_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_STAT_CHAN_EDSP_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_STAT_CHAN_EDSP */



/*@}*/ /*  V2CPE_STAT_CHAN */



/**
 * \defgroup V2CPE_STAT_IEN V2CPE_STAT_IEN
 * Status Interrupt Enable Register. The Status Interrupt Enable Register enables the interrupts when for the corresponding bits in the STAT register. The interrupt line is generated by AND-ing the bits in the STAT_INT register with this mask and the OR-ing the resulting bits together.
 */
/*@{*/


/** register offset */
#define V2CPE_STAT_IEN 0x0A
/** reset value */
#define V2CPE_STAT_IEN_RESET 0x0000


/**
 * \defgroup V2CPE_STAT_IEN_VO_RDY V2CPE_STAT_IEN_VO_RDY
 * Voice Out-Box Ready Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_VO_RDY 0x8000
/** get */
#define V2CPE_STAT_IEN_VO_RDY_GET(reg) (((reg) & V2CPE_STAT_IEN_VO_RDY) >> 15)
/** set */
#define V2CPE_STAT_IEN_VO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_VO_RDY) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_STAT_IEN_VO_RDY */




/**
 * \defgroup V2CPE_STAT_IEN_VO_DATA V2CPE_STAT_IEN_VO_DATA
 * Voice Out-Box Data Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_VO_DATA 0x4000
/** get */
#define V2CPE_STAT_IEN_VO_DATA_GET(reg) (((reg) & V2CPE_STAT_IEN_VO_DATA) >> 14)
/** set */
#define V2CPE_STAT_IEN_VO_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_VO_DATA) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_STAT_IEN_VO_DATA */




/**
 * \defgroup V2CPE_STAT_IEN_VI_EMP V2CPE_STAT_IEN_VI_EMP
 * Voice In-Box Empty Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_VI_EMP 0x2000
/** get */
#define V2CPE_STAT_IEN_VI_EMP_GET(reg) (((reg) & V2CPE_STAT_IEN_VI_EMP) >> 13)
/** set */
#define V2CPE_STAT_IEN_VI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_VI_EMP) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_STAT_IEN_VI_EMP */




/**
 * \defgroup V2CPE_STAT_IEN_VI_DATA V2CPE_STAT_IEN_VI_DATA
 * Voice In-Box Overflow Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_VI_DATA 0x1000
/** get */
#define V2CPE_STAT_IEN_VI_DATA_GET(reg) (((reg) & V2CPE_STAT_IEN_VI_DATA) >> 12)
/** set */
#define V2CPE_STAT_IEN_VI_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_VI_DATA) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_STAT_IEN_VI_DATA */




/**
 * \defgroup V2CPE_STAT_IEN_CO_RDY V2CPE_STAT_IEN_CO_RDY
 * Command Out-Box Ready Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CO_RDY 0x0800
/** get */
#define V2CPE_STAT_IEN_CO_RDY_GET(reg) (((reg) & V2CPE_STAT_IEN_CO_RDY) >> 11)
/** set */
#define V2CPE_STAT_IEN_CO_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CO_RDY) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_STAT_IEN_CO_RDY */




/**
 * \defgroup V2CPE_STAT_IEN_CO_DATA V2CPE_STAT_IEN_CO_DATA
 * Command Out-Box Data Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CO_DATA 0x0400
/** get */
#define V2CPE_STAT_IEN_CO_DATA_GET(reg) (((reg) & V2CPE_STAT_IEN_CO_DATA) >> 10)
/** set */
#define V2CPE_STAT_IEN_CO_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CO_DATA) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_STAT_IEN_CO_DATA */




/**
 * \defgroup V2CPE_STAT_IEN_CI_EMP V2CPE_STAT_IEN_CI_EMP
 * Command In-Box Empty Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CI_EMP 0x0200
/** get */
#define V2CPE_STAT_IEN_CI_EMP_GET(reg) (((reg) & V2CPE_STAT_IEN_CI_EMP) >> 9)
/** set */
#define V2CPE_STAT_IEN_CI_EMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CI_EMP) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_STAT_IEN_CI_EMP */




/**
 * \defgroup V2CPE_STAT_IEN_CI_DATA V2CPE_STAT_IEN_CI_DATA
 * Command In-Box Overflow Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CI_DATA 0x0100
/** get */
#define V2CPE_STAT_IEN_CI_DATA_GET(reg) (((reg) & V2CPE_STAT_IEN_CI_DATA) >> 8)
/** set */
#define V2CPE_STAT_IEN_CI_DATA_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CI_DATA) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_STAT_IEN_CI_DATA */




/**
 * \defgroup V2CPE_STAT_IEN_DL_RDY V2CPE_STAT_IEN_DL_RDY
 * Download Ready Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_DL_RDY 0x0080
/** get */
#define V2CPE_STAT_IEN_DL_RDY_GET(reg) (((reg) & V2CPE_STAT_IEN_DL_RDY) >> 7)
/** set */
#define V2CPE_STAT_IEN_DL_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_DL_RDY) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_STAT_IEN_DL_RDY */




/**
 * \defgroup V2CPE_STAT_IEN_WOKE_UP V2CPE_STAT_IEN_WOKE_UP
 * Woke Up Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_WOKE_UP 0x0040
/** get */
#define V2CPE_STAT_IEN_WOKE_UP_GET(reg) (((reg) & V2CPE_STAT_IEN_WOKE_UP) >> 6)
/** set */
#define V2CPE_STAT_IEN_WOKE_UP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_WOKE_UP) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_STAT_IEN_WOKE_UP */




/**
 * \defgroup V2CPE_STAT_IEN_GPIO V2CPE_STAT_IEN_GPIO
 * GPIO Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_GPIO 0x0020
/** get */
#define V2CPE_STAT_IEN_GPIO_GET(reg) (((reg) & V2CPE_STAT_IEN_GPIO) >> 5)
/** set */
#define V2CPE_STAT_IEN_GPIO_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_GPIO) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_STAT_IEN_GPIO */




/**
 * \defgroup V2CPE_STAT_IEN_EXP_STAT V2CPE_STAT_IEN_EXP_STAT
 * Expansion Rising Edge Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_EXP_STAT 0x0010
/** get */
#define V2CPE_STAT_IEN_EXP_STAT_GET(reg) (((reg) & V2CPE_STAT_IEN_EXP_STAT) >> 4)
/** set */
#define V2CPE_STAT_IEN_EXP_STAT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_EXP_STAT) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_STAT_IEN_EXP_STAT */




/**
 * \defgroup V2CPE_STAT_IEN_CRC_ERR V2CPE_STAT_IEN_CRC_ERR
 * CRC Error Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CRC_ERR 0x0008
/** get */
#define V2CPE_STAT_IEN_CRC_ERR_GET(reg) (((reg) & V2CPE_STAT_IEN_CRC_ERR) >> 3)
/** set */
#define V2CPE_STAT_IEN_CRC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CRC_ERR) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_STAT_IEN_CRC_ERR */




/**
 * \defgroup V2CPE_STAT_IEN_CRC_RDY V2CPE_STAT_IEN_CRC_RDY
 * CRC Ready Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CRC_RDY 0x0004
/** get */
#define V2CPE_STAT_IEN_CRC_RDY_GET(reg) (((reg) & V2CPE_STAT_IEN_CRC_RDY) >> 2)
/** set */
#define V2CPE_STAT_IEN_CRC_RDY_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CRC_RDY) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_STAT_IEN_CRC_RDY */




/**
 * \defgroup V2CPE_STAT_IEN_ERR V2CPE_STAT_IEN_ERR
 * Error Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_ERR 0x0002
/** get */
#define V2CPE_STAT_IEN_ERR_GET(reg) (((reg) & V2CPE_STAT_IEN_ERR) >> 1)
/** set */
#define V2CPE_STAT_IEN_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_STAT_IEN_ERR */




/**
 * \defgroup V2CPE_STAT_IEN_CH V2CPE_STAT_IEN_CH
 * Channel Interrupt Mask.
 */
/*@{*/

/** mask */
#define V2CPE_STAT_IEN_CH 0x0001
/** get */
#define V2CPE_STAT_IEN_CH_GET(reg) (((reg) & V2CPE_STAT_IEN_CH) >> 0)
/** set */
#define V2CPE_STAT_IEN_CH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_STAT_IEN_CH) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_STAT_IEN_CH */



/*@}*/ /*  V2CPE_STAT_IEN */


/*@}*/


/**
 * \defgroup MODULE Error
 */
/*@{*/



/**
 * \defgroup V2CPE_ERR_INT V2CPE_ERR_INT
 * Error Interrupt Register. The Error Interrupt Register captures all error conditions of the Vinetic. All error bits can be acknowledged by writing a logical-1 to the corresponding bit position.
 */
/*@{*/


/** register offset */
#define V2CPE_ERR_INT 0x10
/** reset value */
#define V2CPE_ERR_INT_RESET 0x0000


/**
 * \defgroup V2CPE_ERR_INT_HW_ERR V2CPE_ERR_INT_HW_ERR
 * EDSP Hardware Error. InternalEDSPhardware error.This bit will be set if anEDSPinternal hardware error has been detected and the corresponding bit in the ERR_IEN register is set to a logical one. (e.g. bus conflict). This leads to a software reset.The bit is set by the hardware an can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_HW_ERR 0x8000
/** get */
#define V2CPE_ERR_INT_HW_ERR_GET(reg) (((reg) & V2CPE_ERR_INT_HW_ERR) >> 15)
/** set */
#define V2CPE_ERR_INT_HW_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_HW_ERR) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_ERR_INT_HW_ERR */




/**
 * \defgroup V2CPE_ERR_INT_WD_FAIL V2CPE_ERR_INT_WD_FAIL
 * EDSP Watchdog Failure. Indication ofEDSPfailure after watchdog activation.This bit will be set, if the EDSP reports an watchdog timer timeout and the corresponding bit in the ERR_IEN register is set to a logical one.The bit is set by the hardware and can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_WD_FAIL 0x4000
/** get */
#define V2CPE_ERR_INT_WD_FAIL_GET(reg) (((reg) & V2CPE_ERR_INT_WD_FAIL) >> 14)
/** set */
#define V2CPE_ERR_INT_WD_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_WD_FAIL) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_ERR_INT_WD_FAIL */




/**
 * \defgroup V2CPE_ERR_INT_MCLK_FAIL V2CPE_ERR_INT_MCLK_FAIL
 * System Masterclock Failure. The PLL clock is not locked.The bit is set if the EDSP reports an MCLK failure and the corresponding bit in the ERR_IEN register is set to a logical one and can be acknowledged if the correspondig bit is written with a logical one
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_MCLK_FAIL 0x2000
/** get */
#define V2CPE_ERR_INT_MCLK_FAIL_GET(reg) (((reg) & V2CPE_ERR_INT_MCLK_FAIL) >> 13)
/** set */
#define V2CPE_ERR_INT_MCLK_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_MCLK_FAIL) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_ERR_INT_MCLK_FAIL */




/**
 * \defgroup V2CPE_ERR_INT_SYNC_FAIL V2CPE_ERR_INT_SYNC_FAIL
 * System Synchronization Failure. Checks if multiples of MCLK cycles fit in a FSC frame (PLL divider mismatch). If this fails and the corresponding bit in the ERR_IEN register is set to a logical one this bit is set.The bit can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_SYNC_FAIL 0x1000
/** get */
#define V2CPE_ERR_INT_SYNC_FAIL_GET(reg) (((reg) & V2CPE_ERR_INT_SYNC_FAIL) >> 12)
/** set */
#define V2CPE_ERR_INT_SYNC_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_SYNC_FAIL) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_ERR_INT_SYNC_FAIL */




/**
 * \defgroup V2CPE_ERR_INT_MIPS_OL V2CPE_ERR_INT_MIPS_OL
 * EDSP MIPS Overload Bit. MIPS overload, indicates, that the start of the encoder or decoder was delayed due to a MIPS overload.The bit is set if the EDSP reports an MIPS overload and the corresponding bit in the ERR_IEN register is set. The bit can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_MIPS_OL 0x0800
/** get */
#define V2CPE_ERR_INT_MIPS_OL_GET(reg) (((reg) & V2CPE_ERR_INT_MIPS_OL) >> 11)
/** set */
#define V2CPE_ERR_INT_MIPS_OL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_MIPS_OL) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_ERR_INT_MIPS_OL */




/**
 * \defgroup V2CPE_ERR_INT_PCMB_CRASH V2CPE_ERR_INT_PCMB_CRASH
 * PCM Transmit Highway B Crash. Indication of multiple access of PCM transmit time slot of highway B.The bit is set if the EDSP reports an Highway B crash and the corresponding bit in the ERR_IEN register is set. The event can be acknowledged if the corresponding bit is written with a logical one
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_PCMB_CRASH 0x0400
/** get */
#define V2CPE_ERR_INT_PCMB_CRASH_GET(reg) (((reg) & V2CPE_ERR_INT_PCMB_CRASH) >> 10)
/** set */
#define V2CPE_ERR_INT_PCMB_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_PCMB_CRASH) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_ERR_INT_PCMB_CRASH */




/**
 * \defgroup V2CPE_ERR_INT_PCMA_CRASH V2CPE_ERR_INT_PCMA_CRASH
 * PCM Transmit Highway A Crash. Indication of multiple access of PCM transmit time slot of highway A.The bit is if the EDSP reports an Highway A crash and the corresponding bit in the ERR_IEN register is set. The event can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_PCMA_CRASH 0x0200
/** get */
#define V2CPE_ERR_INT_PCMA_CRASH_GET(reg) (((reg) & V2CPE_ERR_INT_PCMA_CRASH) >> 9)
/** set */
#define V2CPE_ERR_INT_PCMA_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_PCMA_CRASH) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_ERR_INT_PCMA_CRASH */




/**
 * \defgroup V2CPE_ERR_INT_CMD_ERR V2CPE_ERR_INT_CMD_ERR
 * EDSP Command Error. TheEDSPdetected a wrong command.The command in-box will be cleared and theEDSPwaits for the.The bit is set by the EDSP if a wrong command is detected and the corresponding bit in the ERR_IEN register is set. The event can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_CMD_ERR 0x0100
/** get */
#define V2CPE_ERR_INT_CMD_ERR_GET(reg) (((reg) & V2CPE_ERR_INT_CMD_ERR) >> 8)
/** set */
#define V2CPE_ERR_INT_CMD_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_CMD_ERR) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_ERR_INT_CMD_ERR */




/**
 * \defgroup V2CPE_ERR_INT_VI_OV V2CPE_ERR_INT_VI_OV
 * HOST Voice In-Box Overflow. This bit is set if the host writes more data to the packet in-box as allowed within the BOX_VLEN register and the corresponding bit in the ERR_IEN register is set. The data currently written will be discarded.The bit is set by the hardware an can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_VI_OV 0x0080
/** get */
#define V2CPE_ERR_INT_VI_OV_GET(reg) (((reg) & V2CPE_ERR_INT_VI_OV) >> 7)
/** set */
#define V2CPE_ERR_INT_VI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_VI_OV) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_ERR_INT_VI_OV */




/**
 * \defgroup V2CPE_ERR_INT_CI_OV V2CPE_ERR_INT_CI_OV
 * HOST Command In-Box Overflow. This bit is set if the host writes more data to the command in-box as stated in the BOX_CLEN register and the corresponding bit in the ERR_IEN register is set. The data currently written will be discarded.The bit is set by the hardware an can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_CI_OV 0x0040
/** get */
#define V2CPE_ERR_INT_CI_OV_GET(reg) (((reg) & V2CPE_ERR_INT_CI_OV) >> 6)
/** set */
#define V2CPE_ERR_INT_CI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_CI_OV) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_ERR_INT_CI_OV */




/**
 * \defgroup V2CPE_ERR_INT_VO_UV V2CPE_ERR_INT_VO_UV
 * HOST Voice Out-Box Underflow. This bit is set if the host read more data form the packet out-box as allowed within the BOX_VLEN register and the corresponding bit in the ERR_IEN register is set. If this error condition occures the box status itself is not changed and the data returned is undefined.The bit is set by the hardware an can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_VO_UV 0x0020
/** get */
#define V2CPE_ERR_INT_VO_UV_GET(reg) (((reg) & V2CPE_ERR_INT_VO_UV) >> 5)
/** set */
#define V2CPE_ERR_INT_VO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_VO_UV) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_ERR_INT_VO_UV */




/**
 * \defgroup V2CPE_ERR_INT_CO_UV V2CPE_ERR_INT_CO_UV
 * HOST Command In-Box Underflow. This bit is set if the host reads more data from the command out-box as stated in the BOX_CLEN register and the corresponding bit in the ERR_IEN register is set. If this error condition occures the box status itself is not changed and the data returned is undefined.The bit is set by the hardware an can be acknowledged if the correspondig bit is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_INT_CO_UV 0x0010
/** get */
#define V2CPE_ERR_INT_CO_UV_GET(reg) (((reg) & V2CPE_ERR_INT_CO_UV) >> 4)
/** set */
#define V2CPE_ERR_INT_CO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_INT_CO_UV) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_ERR_INT_CO_UV */



/*@}*/ /*  V2CPE_ERR_INT */



/**
 * \defgroup V2CPE_ERR_IEN V2CPE_ERR_IEN
 * Error Interrupt Enable Register. The Error Interrupt Enable Register holds all enable bits of all Vinetic Error Condítions. If the corresponding bit in the ERR_IEN register is set and a rising-edge in the ERR is detected, then the corresponding bit in the ERR_INT regoster is set and a host error interrupt is issued if the ERR bit in the STAT_IEN register is set.
 */
/*@{*/


/** register offset */
#define V2CPE_ERR_IEN 0x12
/** reset value */
#define V2CPE_ERR_IEN_RESET 0x0000


/**
 * \defgroup V2CPE_ERR_IEN_HW_ERR V2CPE_ERR_IEN_HW_ERR
 * EDSP Hardware Error Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_HW_ERR 0x8000
/** get */
#define V2CPE_ERR_IEN_HW_ERR_GET(reg) (((reg) & V2CPE_ERR_IEN_HW_ERR) >> 15)
/** set */
#define V2CPE_ERR_IEN_HW_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_HW_ERR) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_ERR_IEN_HW_ERR */




/**
 * \defgroup V2CPE_ERR_IEN_WD_FAIL V2CPE_ERR_IEN_WD_FAIL
 * EDSP Watchdog Failure Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_WD_FAIL 0x4000
/** get */
#define V2CPE_ERR_IEN_WD_FAIL_GET(reg) (((reg) & V2CPE_ERR_IEN_WD_FAIL) >> 14)
/** set */
#define V2CPE_ERR_IEN_WD_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_WD_FAIL) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_ERR_IEN_WD_FAIL */




/**
 * \defgroup V2CPE_ERR_IEN_MCLK_FAIL V2CPE_ERR_IEN_MCLK_FAIL
 * System Masterclock Failure Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_MCLK_FAIL 0x2000
/** get */
#define V2CPE_ERR_IEN_MCLK_FAIL_GET(reg) (((reg) & V2CPE_ERR_IEN_MCLK_FAIL) >> 13)
/** set */
#define V2CPE_ERR_IEN_MCLK_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_MCLK_FAIL) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_ERR_IEN_MCLK_FAIL */




/**
 * \defgroup V2CPE_ERR_IEN_SYNC_FAIL V2CPE_ERR_IEN_SYNC_FAIL
 * System Synchronization Failure Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_SYNC_FAIL 0x1000
/** get */
#define V2CPE_ERR_IEN_SYNC_FAIL_GET(reg) (((reg) & V2CPE_ERR_IEN_SYNC_FAIL) >> 12)
/** set */
#define V2CPE_ERR_IEN_SYNC_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_SYNC_FAIL) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_ERR_IEN_SYNC_FAIL */




/**
 * \defgroup V2CPE_ERR_IEN_EDSP_MIPS_OL V2CPE_ERR_IEN_EDSP_MIPS_OL
 * EDSP MIPS Overload Bit Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_EDSP_MIPS_OL 0x0800
/** get */
#define V2CPE_ERR_IEN_EDSP_MIPS_OL_GET(reg) (((reg) & V2CPE_ERR_IEN_EDSP_MIPS_OL) >> 11)
/** set */
#define V2CPE_ERR_IEN_EDSP_MIPS_OL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_EDSP_MIPS_OL) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_ERR_IEN_EDSP_MIPS_OL */




/**
 * \defgroup V2CPE_ERR_IEN_PCMB_CRASH V2CPE_ERR_IEN_PCMB_CRASH
 * PCM Transmit Highway B Crash Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_PCMB_CRASH 0x0400
/** get */
#define V2CPE_ERR_IEN_PCMB_CRASH_GET(reg) (((reg) & V2CPE_ERR_IEN_PCMB_CRASH) >> 10)
/** set */
#define V2CPE_ERR_IEN_PCMB_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_PCMB_CRASH) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_ERR_IEN_PCMB_CRASH */




/**
 * \defgroup V2CPE_ERR_IEN_PCMA_CRASH V2CPE_ERR_IEN_PCMA_CRASH
 * PCM Transmit Highway A Crash Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_PCMA_CRASH 0x0200
/** get */
#define V2CPE_ERR_IEN_PCMA_CRASH_GET(reg) (((reg) & V2CPE_ERR_IEN_PCMA_CRASH) >> 9)
/** set */
#define V2CPE_ERR_IEN_PCMA_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_PCMA_CRASH) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_ERR_IEN_PCMA_CRASH */




/**
 * \defgroup V2CPE_ERR_IEN_CMD_ERR V2CPE_ERR_IEN_CMD_ERR
 * EDSP Command Error Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_CMD_ERR 0x0100
/** get */
#define V2CPE_ERR_IEN_CMD_ERR_GET(reg) (((reg) & V2CPE_ERR_IEN_CMD_ERR) >> 8)
/** set */
#define V2CPE_ERR_IEN_CMD_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_CMD_ERR) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_ERR_IEN_CMD_ERR */




/**
 * \defgroup V2CPE_ERR_IEN_VI_OV V2CPE_ERR_IEN_VI_OV
 * HOST Packet In-Box Overflow Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_VI_OV 0x0080
/** get */
#define V2CPE_ERR_IEN_VI_OV_GET(reg) (((reg) & V2CPE_ERR_IEN_VI_OV) >> 7)
/** set */
#define V2CPE_ERR_IEN_VI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_VI_OV) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_ERR_IEN_VI_OV */




/**
 * \defgroup V2CPE_ERR_IEN_CI_OV V2CPE_ERR_IEN_CI_OV
 * HOST Command In-Box Overflow Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_CI_OV 0x0040
/** get */
#define V2CPE_ERR_IEN_CI_OV_GET(reg) (((reg) & V2CPE_ERR_IEN_CI_OV) >> 6)
/** set */
#define V2CPE_ERR_IEN_CI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_CI_OV) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_ERR_IEN_CI_OV */




/**
 * \defgroup V2CPE_ERR_IEN_VO_UV V2CPE_ERR_IEN_VO_UV
 * HOST Packet Out-Box Underflow Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_VO_UV 0x0020
/** get */
#define V2CPE_ERR_IEN_VO_UV_GET(reg) (((reg) & V2CPE_ERR_IEN_VO_UV) >> 5)
/** set */
#define V2CPE_ERR_IEN_VO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_VO_UV) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_ERR_IEN_VO_UV */




/**
 * \defgroup V2CPE_ERR_IEN_CO_UV V2CPE_ERR_IEN_CO_UV
 * HOST Command Out-Box Underflow Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_IEN_CO_UV 0x0010
/** get */
#define V2CPE_ERR_IEN_CO_UV_GET(reg) (((reg) & V2CPE_ERR_IEN_CO_UV) >> 4)
/** set */
#define V2CPE_ERR_IEN_CO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_IEN_CO_UV) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_ERR_IEN_CO_UV */



/*@}*/ /*  V2CPE_ERR_IEN */



/**
 * \defgroup V2CPE_ERR V2CPE_ERR
 * Error Register. The Error Register represents the state of error conditions of the Vinetic. Please notice, that polling of the error register does not guarantee that the event can be detected.
 */
/*@{*/


/** register offset */
#define V2CPE_ERR 0x14
/** reset value */
#define V2CPE_ERR_RESET 0x0000


/**
 * \defgroup V2CPE_ERR_HW_ERR V2CPE_ERR_HW_ERR
 * EDSP Hardware Error. InternalEDSPhardware error.This bit reflects the state of the corresponding bit reported by the EDSP (e.g. bus conflict). The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_HW_ERR 0x8000
/** get */
#define V2CPE_ERR_HW_ERR_GET(reg) (((reg) & V2CPE_ERR_HW_ERR) >> 15)
/** set */
#define V2CPE_ERR_HW_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_HW_ERR) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_ERR_HW_ERR */




/**
 * \defgroup V2CPE_ERR_WD_FAIL V2CPE_ERR_WD_FAIL
 * EDSP Watchdog Failure. Indication ofEDSPfailure after watchdog activation.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_WD_FAIL 0x4000
/** get */
#define V2CPE_ERR_WD_FAIL_GET(reg) (((reg) & V2CPE_ERR_WD_FAIL) >> 14)
/** set */
#define V2CPE_ERR_WD_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_WD_FAIL) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_ERR_WD_FAIL */




/**
 * \defgroup V2CPE_ERR_MCLK_FAIL V2CPE_ERR_MCLK_FAIL
 * System Masterclock Failure. The PLL clock is not locked.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_MCLK_FAIL 0x2000
/** get */
#define V2CPE_ERR_MCLK_FAIL_GET(reg) (((reg) & V2CPE_ERR_MCLK_FAIL) >> 13)
/** set */
#define V2CPE_ERR_MCLK_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_MCLK_FAIL) | (((val) & 1) << 13))

/*@}*/ /* V2CPE_ERR_MCLK_FAIL */




/**
 * \defgroup V2CPE_ERR_SYNC_FAIL V2CPE_ERR_SYNC_FAIL
 * System Synchronization Failure. Checks if multiples of MCLK cycles fit in a FSC frame (PLL divider mismatch).This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_SYNC_FAIL 0x1000
/** get */
#define V2CPE_ERR_SYNC_FAIL_GET(reg) (((reg) & V2CPE_ERR_SYNC_FAIL) >> 12)
/** set */
#define V2CPE_ERR_SYNC_FAIL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_SYNC_FAIL) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_ERR_SYNC_FAIL */




/**
 * \defgroup V2CPE_ERR_MIPS_OL V2CPE_ERR_MIPS_OL
 * EDSP MIPS Overload Bit. MIPS overload, indicates, that the start of the encoder or decoder was delayed due to a MIPS overload.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_MIPS_OL 0x0800
/** get */
#define V2CPE_ERR_MIPS_OL_GET(reg) (((reg) & V2CPE_ERR_MIPS_OL) >> 11)
/** set */
#define V2CPE_ERR_MIPS_OL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_MIPS_OL) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_ERR_MIPS_OL */




/**
 * \defgroup V2CPE_ERR_PCMB_CRASH V2CPE_ERR_PCMB_CRASH
 * PCM Transmit Highway B Crash. Indication of multiple access of PCM transmit time slot of highway B.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_PCMB_CRASH 0x0400
/** get */
#define V2CPE_ERR_PCMB_CRASH_GET(reg) (((reg) & V2CPE_ERR_PCMB_CRASH) >> 10)
/** set */
#define V2CPE_ERR_PCMB_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_PCMB_CRASH) | (((val) & 1) << 10))

/*@}*/ /* V2CPE_ERR_PCMB_CRASH */




/**
 * \defgroup V2CPE_ERR_PCMA_CRASH V2CPE_ERR_PCMA_CRASH
 * PCM Transmit Highway A Crash. Indication of multiple access of PCM transmit time slot of highway A.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_PCMA_CRASH 0x0200
/** get */
#define V2CPE_ERR_PCMA_CRASH_GET(reg) (((reg) & V2CPE_ERR_PCMA_CRASH) >> 9)
/** set */
#define V2CPE_ERR_PCMA_CRASH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_PCMA_CRASH) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_ERR_PCMA_CRASH */




/**
 * \defgroup V2CPE_ERR_CMD_ERR V2CPE_ERR_CMD_ERR
 * EDSP Command Error. TheEDSPdetected a wrong command.The command in-box will be cleared and theEDSPwaits for the.This bit reflects the state of the corresponding bit reported by the EDSP. The rising-edge of the event can be captured in the corresponding bit of the ERR_INT register.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_CMD_ERR 0x0100
/** get */
#define V2CPE_ERR_CMD_ERR_GET(reg) (((reg) & V2CPE_ERR_CMD_ERR) >> 8)
/** set */
#define V2CPE_ERR_CMD_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_CMD_ERR) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_ERR_CMD_ERR */




/**
 * \defgroup V2CPE_ERR_VI_OV V2CPE_ERR_VI_OV
 * HOST Voice In-Box Overflow. This bit is set if the host writes more data to the packet in-box as allowed within the BOX_VLEN register. The data currently written will be discarded.The bit is set by the hardware regardless of the state of the interrupt enable bit in the ERR_IEN register when an overflow occures. The bit is reset when the corresponding bit in the ERR_INT register is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_VI_OV 0x0080
/** get */
#define V2CPE_ERR_VI_OV_GET(reg) (((reg) & V2CPE_ERR_VI_OV) >> 7)
/** set */
#define V2CPE_ERR_VI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_VI_OV) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_ERR_VI_OV */




/**
 * \defgroup V2CPE_ERR_CI_OV V2CPE_ERR_CI_OV
 * HOST Command In-Box Overflow. This bit is set if the host writes more data to the command in-box as stated in the BOX_CLEN register. The data currently written will be discarded.The bit is set by the hardware regardless of the state of the interrupt enable bit in the ERR_IEN register when an overflow occures. The bit is reset when the corresponding bit in the ERR_INT register is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_CI_OV 0x0040
/** get */
#define V2CPE_ERR_CI_OV_GET(reg) (((reg) & V2CPE_ERR_CI_OV) >> 6)
/** set */
#define V2CPE_ERR_CI_OV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_CI_OV) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_ERR_CI_OV */




/**
 * \defgroup V2CPE_ERR_VO_UV V2CPE_ERR_VO_UV
 * HOST Voice Out-Box Underflow. This bit is set if the host read more data form the packet out-box as allowed within the BOX_VLEN register. If this error condition occures the box status itself is not changed and the data returned is undefined.The bit is set by the hardware regardless of the state of the interrupt enable bit in the ERR_IEN register when an underflow occures. The bit is reset when the corresponding bit in the ERR_INT register is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_VO_UV 0x0020
/** get */
#define V2CPE_ERR_VO_UV_GET(reg) (((reg) & V2CPE_ERR_VO_UV) >> 5)
/** set */
#define V2CPE_ERR_VO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_VO_UV) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_ERR_VO_UV */




/**
 * \defgroup V2CPE_ERR_CO_UV V2CPE_ERR_CO_UV
 * HOST Command In-Box Underflow. This bit is set if the host reads more data from the command out-box as stated in the BOX_CLEN register. If this error condition occures the box status itself is not changed and the data returned is undefined.The bit is set by the hardware regardless of the state of the interrupt enable bit in the ERR_IEN register when an underflow occures. The bit is reset when the corresponding bit in the ERR_INT register is written with a logical one.
 */
/*@{*/

/** mask */
#define V2CPE_ERR_CO_UV 0x0010
/** get */
#define V2CPE_ERR_CO_UV_GET(reg) (((reg) & V2CPE_ERR_CO_UV) >> 4)
/** set */
#define V2CPE_ERR_CO_UV_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_ERR_CO_UV) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_ERR_CO_UV */



/*@}*/ /*  V2CPE_ERR */


/*@}*/


/**
 * \defgroup MODULE Mailbox
 */
/*@{*/



/**
 * \defgroup V2CPE_BOX_CDATA V2CPE_BOX_CDATA
 * Box Command Data Register. The Box Command Data Register is physically mapped to the Command-In FIFO for register writes and to th Command-Out FIFO for register reads. The FIFOs accept EOP and EVT commands.
 */
/*@{*/


/** register offset */
#define V2CPE_BOX_CDATA 0x16
/** reset value */
#define V2CPE_BOX_CDATA_RESET 0x0000


/**
 * \defgroup V2CPE_BOX_CDATA_CMD V2CPE_BOX_CDATA_CMD
 * Command In/Out. A write to this register deposites a new data word in the Command-In Box of the VINETIC, a read returns the next data from the Command-Out FIFO.
 */
/*@{*/

/*@}*/ /* V2CPE_BOX_CDATA_CMD */



/*@}*/ /*  V2CPE_BOX_CDATA */



/**
 * \defgroup V2CPE_BOX_VDATA V2CPE_BOX_VDATA
 * Box Voice Data Register. The Box Command Data Register is physically mapped to the Voice-In FIFO for register writes and to th Voice-Out FIFO for register reads. The FIFOs accept VOP commands.
 */
/*@{*/


/** register offset */
#define V2CPE_BOX_VDATA 0x18
/** reset value */
#define V2CPE_BOX_VDATA_RESET 0x0000


/**
 * \defgroup V2CPE_BOX_VDATA_DATA V2CPE_BOX_VDATA_DATA
 * Voice Data In/Out. A write to this register deposites a new data word in the Voice-In Box of the VINETIC. A read from this register returns the next word of the Voice-OutBox.
 */
/*@{*/

/*@}*/ /* V2CPE_BOX_VDATA_DATA */



/*@}*/ /*  V2CPE_BOX_VDATA */



/**
 * \defgroup V2CPE_BOX_CMD V2CPE_BOX_CMD
 * Box Command Register. The Box Command Register is used for higher-level Host-Vinetic signalling. In this system the register is used for command abort and FIFO reset commands. This register is not used for regular communication.
 */
/*@{*/


/** register offset */
#define V2CPE_BOX_CMD 0x1A
/** reset value */
#define V2CPE_BOX_CMD_RESET 0x0000


/**
 * \defgroup V2CPE_BOX_CMD_CO_RES V2CPE_BOX_CMD_CO_RES
 * Host Command-Out FIFO Reset Request.
 */
/*@{*/

/** mask */
#define V2CPE_BOX_CMD_CO_RES 0x0020
/** get */
#define V2CPE_BOX_CMD_CO_RES_GET(reg) (((reg) & V2CPE_BOX_CMD_CO_RES) >> 5)
/** set */
#define V2CPE_BOX_CMD_CO_RES_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_BOX_CMD_CO_RES) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_BOX_CMD_CO_RES */




/**
 * \defgroup V2CPE_BOX_CMD_CI_RES V2CPE_BOX_CMD_CI_RES
 * Host Command-In FIFO Reset Request.
 */
/*@{*/

/** mask */
#define V2CPE_BOX_CMD_CI_RES 0x0010
/** get */
#define V2CPE_BOX_CMD_CI_RES_GET(reg) (((reg) & V2CPE_BOX_CMD_CI_RES) >> 4)
/** set */
#define V2CPE_BOX_CMD_CI_RES_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_BOX_CMD_CI_RES) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_BOX_CMD_CI_RES */




/**
 * \defgroup V2CPE_BOX_CMD_VO_RES V2CPE_BOX_CMD_VO_RES
 * Host Voice-Out FIFO Reset Request.
 */
/*@{*/

/** mask */
#define V2CPE_BOX_CMD_VO_RES 0x0004
/** get */
#define V2CPE_BOX_CMD_VO_RES_GET(reg) (((reg) & V2CPE_BOX_CMD_VO_RES) >> 2)
/** set */
#define V2CPE_BOX_CMD_VO_RES_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_BOX_CMD_VO_RES) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_BOX_CMD_VO_RES */




/**
 * \defgroup V2CPE_BOX_CMD_VI_RES V2CPE_BOX_CMD_VI_RES
 * Host Voice-In FIFO Reset Request.
 */
/*@{*/

/** mask */
#define V2CPE_BOX_CMD_VI_RES 0x0002
/** get */
#define V2CPE_BOX_CMD_VI_RES_GET(reg) (((reg) & V2CPE_BOX_CMD_VI_RES) >> 1)
/** set */
#define V2CPE_BOX_CMD_VI_RES_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_BOX_CMD_VI_RES) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_BOX_CMD_VI_RES */



/*@}*/ /*  V2CPE_BOX_CMD */



/**
 * \defgroup V2CPE_BOX_CLEN V2CPE_BOX_CLEN
 * Command FIFO Length Register. The Command FIFO Length Register reports the number of words in the Command-In/Out FIFO.
 */
/*@{*/


/** register offset */
#define V2CPE_BOX_CLEN 0x1C
/** reset value */
#define V2CPE_BOX_CLEN_RESET 0x1F00


/**
 * \defgroup V2CPE_BOX_CLEN_WLEN V2CPE_BOX_CLEN_WLEN
 * Free Command In-Box Space. Indicates the free command in-mailbox size in words.
 */
/*@{*/

#define V2CPE_BOX_CLEN_WLEN_MASK  0xFF00

#define V2CPE_BOX_CLEN_WLEN_GET(reg) (((reg)>> 8) & ((1 << 8) - 1))
#define V2CPE_BOX_CLEN_WLEN_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_BOX_CLEN_WLEN_MASK) | ((((1 << 8) - 1) & (val)) << 8) )
/*@}*/ /* V2CPE_BOX_CLEN_WLEN */




/**
 * \defgroup V2CPE_BOX_CLEN_RLEN V2CPE_BOX_CLEN_RLEN
 * Command Data Size. Indicates the number of data words in the out-mailbox which are ready for reading.
 */
/*@{*/

#define V2CPE_BOX_CLEN_RLEN_MASK  0x00FF

#define V2CPE_BOX_CLEN_RLEN_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_BOX_CLEN_RLEN_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_BOX_CLEN_RLEN_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_BOX_CLEN_RLEN */



/*@}*/ /*  V2CPE_BOX_CLEN */



/**
 * \defgroup V2CPE_BOX_VLEN V2CPE_BOX_VLEN
 * Voice FIFO Length Register. The Voice FIFO Length Register reports the number of words in the Voice-In/Out FIFO.
 */
/*@{*/


/** register offset */
#define V2CPE_BOX_VLEN 0x1E
/** reset value */
#define V2CPE_BOX_VLEN_RESET 0xFF00


/**
 * \defgroup V2CPE_BOX_VLEN_WLEN V2CPE_BOX_VLEN_WLEN
 * Free Packet In-Box Space [M-V]. Indicates the free packet in-mailbox size in words.
 */
/*@{*/

#define V2CPE_BOX_VLEN_WLEN_MASK  0xFF00

#define V2CPE_BOX_VLEN_WLEN_GET(reg) (((reg)>> 8) & ((1 << 8) - 1))
#define V2CPE_BOX_VLEN_WLEN_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_BOX_VLEN_WLEN_MASK) | ((((1 << 8) - 1) & (val)) << 8) )
/*@}*/ /* V2CPE_BOX_VLEN_WLEN */




/**
 * \defgroup V2CPE_BOX_VLEN_RLEN V2CPE_BOX_VLEN_RLEN
 * Voice Data Size. Indicates the number of data words in the packet out‑mailbox which are ready for reading.
 */
/*@{*/

#define V2CPE_BOX_VLEN_RLEN_MASK  0x00FF

#define V2CPE_BOX_VLEN_RLEN_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_BOX_VLEN_RLEN_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_BOX_VLEN_RLEN_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_BOX_VLEN_RLEN */



/*@}*/ /*  V2CPE_BOX_VLEN */


/*@}*/


/**
 * \defgroup MODULE General_Purpose_Input_Outputs
 */
/*@{*/



/**
 * \defgroup V2CPE_IO_CFG V2CPE_IO_CFG
 * IO Configuration Register. The GPIO Configuration Register defines the data direction of the different GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_CFG 0x20
/** reset value */
#define V2CPE_IO_CFG_RESET 0xFFFF


/**
 * \defgroup V2CPE_IO_CFG_DIR V2CPE_IO_CFG_DIR
 * GPIO7 - GPIO0 Data Direction.
 */
/*@{*/

#define V2CPE_IO_CFG_DIR_MASK  0x00FF

#define V2CPE_IO_CFG_DIR_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_CFG_DIR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_CFG_DIR_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_CFG_DIR */



/*@}*/ /*  V2CPE_IO_CFG */



/**
 * \defgroup V2CPE_IO_PULL V2CPE_IO_PULL
 * IO Pullup Resistor Register. The GPIO Data register is used to write and read the level of the differen GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_PULL 0x22
/** reset value */
#define V2CPE_IO_PULL_RESET 0xFFFF


/**
 * \defgroup V2CPE_IO_PULL_DATA V2CPE_IO_PULL_DATA
 * GPIO15 - GPIO0 Data.
 */
/*@{*/

#define V2CPE_IO_PULL_DATA_MASK   0x00FF

#define V2CPE_IO_PULL_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_PULL_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_PULL_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_PULL_DATA */



/*@}*/ /*  V2CPE_IO_PULL */



/**
 * \defgroup V2CPE_IO_OUT V2CPE_IO_OUT
 * IO Ouput Register. The IO Output register is used to write data to the different GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_OUT 0x24
/** reset value */
#define V2CPE_IO_OUT_RESET 0x0000


/**
 * \defgroup V2CPE_IO_OUT_DATA V2CPE_IO_OUT_DATA
 * GPIO7 - GPIO0 Data. IO0 to IO7 Data Register.The corresponding register bit has no effect, if the corresponding bit in the IO_CFG register is programmed as 1 (IO is input).0 - Pin is driven low.1 - Pin is driven high.
 */
/*@{*/

#define V2CPE_IO_OUT_DATA_MASK    0x00FF

#define V2CPE_IO_OUT_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_OUT_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_OUT_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_OUT_DATA */



/*@}*/ /*  V2CPE_IO_OUT */



/**
 * \defgroup V2CPE_IO_IN V2CPE_IO_IN
 * IO Input Register. The IO Input register is used to read data from the different GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_IN 0x26
/** reset value */
#define V2CPE_IO_IN_RESET 0x0000


/**
 * \defgroup V2CPE_IO_IN_DATA V2CPE_IO_IN_DATA
 * GPIO7 - GPIO0 Data. IO0 to IO7 Data Register.If the corresponding bit in the IO_CFG bit is programmed as input, the value of corresponding bi in the IO_OUT register is returned (whith internal delay).0 - Pin is externally driven low.1 - Pin is externally driven high.
 */
/*@{*/

#define V2CPE_IO_IN_DATA_MASK  0x00FF

#define V2CPE_IO_IN_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_IN_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_IN_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_IN_DATA */



/*@}*/ /*  V2CPE_IO_IN */



/**
 * \defgroup V2CPE_IO_INT V2CPE_IO_INT
 * IO Interrupt Register. The IO Input register is used to read data from the different GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_INT 0x28
/** reset value */
#define V2CPE_IO_INT_RESET 0x0000


/**
 * \defgroup V2CPE_IO_INT_DATA V2CPE_IO_INT_DATA
 * GPIO7 - GPIO0 Data. GPIO0 to GPIO7 Interrupt Register.If the corresponding bit in the IO_INTR and/or IO_INTF register is programmed as input, the value of corresponding bit in the IO_INT register is set. After the Host has read the interrupt information, the register must be written with 0 in order to signal the FW that a new update can be performed (interrupt acknoledge).0 - normal operation1 - interrupt occured
 */
/*@{*/

#define V2CPE_IO_INT_DATA_MASK    0x00FF

#define V2CPE_IO_INT_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_INT_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_INT_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_INT_DATA */



/*@}*/ /*  V2CPE_IO_INT */



/**
 * \defgroup V2CPE_IO_INTR V2CPE_IO_INTR
 * IO Interrupt Enable Register (rising-edge). The IO Input register is used to enable the detection of rising-edge interrupts for the GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_INTR 0x2A
/** reset value */
#define V2CPE_IO_INTR_RESET 0x0000


/**
 * \defgroup V2CPE_IO_INTR_DATA V2CPE_IO_INTR_DATA
 * GPIO7 - GPIO0 Rising Edge Interrupt Enable. If the corresponding bit in the IO_INTR register is set when a zero-to-one transition is detected within an 8 kHz interval.0 - Interrupt disable1 - Interrupt enable
 */
/*@{*/

#define V2CPE_IO_INTR_DATA_MASK   0x00FF

#define V2CPE_IO_INTR_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_INTR_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_INTR_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_INTR_DATA */



/*@}*/ /*  V2CPE_IO_INTR */



/**
 * \defgroup V2CPE_IO_INTF V2CPE_IO_INTF
 * IO Interrupt Enable Register (falling-edge). The IO Input register is used to enable the detection of falling-edge interrupts for the GPIO pins.
 */
/*@{*/


/** register offset */
#define V2CPE_IO_INTF 0x2C
/** reset value */
#define V2CPE_IO_INTF_RESET 0x0000


/**
 * \defgroup V2CPE_IO_INTF_DATA V2CPE_IO_INTF_DATA
 * GPIO7 - GPIO0 Rising Edge Interrupt Enable. If the corresponding bit in the IO_INTF register is set when a one-to-zero transition is detected within an 8 kHz interval.0 - Interrupt disable1 - Interrupt enable
 */
/*@{*/

#define V2CPE_IO_INTF_DATA_MASK   0x00FF

#define V2CPE_IO_INTF_DATA_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_IO_INTF_DATA_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_IO_INTF_DATA_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_IO_INTF_DATA */



/*@}*/ /*  V2CPE_IO_INTF */


/*@}*/


/**
 * \defgroup MODULE General_Purpose
 */
/*@{*/



/**
 * \defgroup V2CPE_DUPO_MBX_SYNC V2CPE_DUPO_MBX_SYNC
 * implements the mailbox handshake
 */
/*@{*/

/** register offset */
#define V2CPE_DUPO_MBX_SYNC 0x30
/** reset value */
#define V2CPE_DUPO_MBX_SYNC_RESET 0x0000

#define V2CPE_DUPO_MBX_SYNC_CMD_MASK 0x0001
#define V2CPE_DUPO_MBX_SYNC_CMD_SEM_TAKE(reg) \
       (reg) = ( (reg) | (unsigned short) V2CPE_DUPO_MBX_SYNC_CMD_MASK )
#define V2CPE_DUPO_MBX_SYNC_CMD_SEM_GIVE(reg) \
       (reg) = ( (reg) & (unsigned short) ~V2CPE_DUPO_MBX_SYNC_CMD_MASK )

#define V2CPE_DUPO_MBX_SYNC_PKT_MASK 0x0002
#define V2CPE_DUPO_MBX_SYNC_PKT_SEM_TAKE(reg) \
       (reg) = ( (reg) | (unsigned short) V2CPE_DUPO_MBX_SYNC_PKT_MASK )
#define V2CPE_DUPO_MBX_SYNC_PKT_SEM_GIVE(reg) \
       (reg) = ( (reg) & (unsigned short) ~V2CPE_DUPO_MBX_SYNC_PKT_MASK )

/** macro to take the cmd mailbox smaphore
  \param pDev    - handle to device
*/
#define V2CPE_CMD_MBX_SEM_TAKE(pDev)  do {\
   V2CPE_DUPO_MBX_SYNC_CMD_SEM_TAKE((pDev)->hostDev.nMbxHandshake);\
   REG_WRITE_UNPROT((pDev), V2CPE_DUPO_MBX_SYNC, (pDev)->hostDev.nMbxHandshake);\
   }while(0)

/** macro to give the cmd mailbox smaphore
  \param pDev    - handle to device
*/
#define V2CPE_CMD_MBX_SEM_GIVE(pDev) do{\
   V2CPE_DUPO_MBX_SYNC_CMD_SEM_GIVE((pDev)->hostDev.nMbxHandshake); \
   REG_WRITE_UNPROT((pDev), V2CPE_DUPO_MBX_SYNC, (pDev)->hostDev.nMbxHandshake);\
   } while(0)

/** macro to take the packet mailbox smaphore
  \param pDev    - handle to device
*/
#define V2CPE_PKT_MBX_SEM_TAKE(pDev) do{\
   V2CPE_DUPO_MBX_SYNC_PKT_SEM_TAKE((pDev)->hostDev.nMbxHandshake);\
   REG_WRITE_UNPROT((pDev), V2CPE_DUPO_MBX_SYNC, (pDev)->hostDev.nMbxHandshake);\
   } while(0)

/** macro to give the packet mailbox smaphore
  \param pDev    - handle to device
*/
#define V2CPE_PKT_MBX_SEM_GIVE(pDev) do {\
   V2CPE_DUPO_MBX_SYNC_PKT_SEM_GIVE((pDev)->hostDev.nMbxHandshake); \
   REG_WRITE_UNPROT((pDev), V2CPE_DUPO_MBX_SYNC, (pDev)->hostDev.nMbxHandshake);\
   } while(0)


/*@}*/ /*  V2CPE_DUPO_MBX_SYNC */


/**
 * \defgroup V2CPE_DUPO_REVISION V2CPE_DUPO_REVISION
 * Revision Number. Hold the FW/HW revision number (t.b.d.)
 */
/*@{*/


/** register offset */
#define V2CPE_DUPO_REVISION 0x3C
/** reset value */
#define V2CPE_DUPO_REVISION_RESET 0x0000


/**
 * \defgroup V2CPE_DUPO_REVISION_TYPE V2CPE_DUPO_REVISION_TYPE
 * Type Of Device.
 */
/*@{*/

#define V2CPE_DUPO_REVISION_TYPE_MASK   0xF000

#define V2CPE_DUPO_REVISION_TYPE_GET(reg) (((reg)>> 12) & ((1 << 4) - 1))
#define V2CPE_DUPO_REVISION_TYPE_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_DUPO_REVISION_TYPE_MASK) | ((((1 << 4) - 1) & (val)) << 12) )
/*@}*/ /* V2CPE_DUPO_REVISION_TYPE */




/**
 * \defgroup V2CPE_DUPO_REVISION_CHAN V2CPE_DUPO_REVISION_CHAN
 * Analog Channels. Number of analog channels which are supported by this device.
 */
/*@{*/

#define V2CPE_DUPO_REVISION_CHAN_MASK   0x0F00

#define V2CPE_DUPO_REVISION_CHAN_GET(reg) (((reg)>> 8) & ((1 << 4) - 1))
#define V2CPE_DUPO_REVISION_CHAN_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_DUPO_REVISION_CHAN_MASK) | ((((1 << 4) - 1) & (val)) << 8) )
/*@}*/ /* V2CPE_DUPO_REVISION_CHAN */




/**
 * \defgroup V2CPE_DUPO_REVISION_REV V2CPE_DUPO_REVISION_REV
 * Revision. Current HW-/FW-revision of theVINETIC®‑x.[7:4] HW-Version, [3:0] Firmware Version
 */
/*@{*/

#define V2CPE_DUPO_REVISION_REV_MASK    0x00FF

#define V2CPE_DUPO_REVISION_REV_GET(reg) (((reg)>> 0) & ((1 << 8) - 1))
#define V2CPE_DUPO_REVISION_REV_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_DUPO_REVISION_REV_MASK) | ((((1 << 8) - 1) & (val)) << 0) )
/*@}*/ /* V2CPE_DUPO_REVISION_REV */



/*@}*/ /*  V2CPE_DUPO_REVISION */


/*@}*/


/**
 * \defgroup MODULE EDSP
 */
/*@{*/



/**
 * \defgroup V2CPE_EDSP1_STAT1 V2CPE_EDSP1_STAT1
 * EDSP Status Register 1. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_STAT1 0x40
/** reset value */
#define V2CPE_EDSP1_STAT1_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFR_DT V2CPE_EDSP1_STAT1_DTMFR_DT
 * DTMF Receiver Key Detect. Valid DTMF key detected by the DTMF receiver.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_DTMFR_DT 0x8000
/** get */
#define V2CPE_EDSP1_STAT1_DTMFR_DT_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_DTMFR_DT) >> 15)
/** set */
#define V2CPE_EDSP1_STAT1_DTMFR_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFR_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFR_DT */




/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFR_PDT V2CPE_EDSP1_STAT1_DTMFR_PDT
 * DTMF Receiver Probably Key Detect. Probably DTMF key detected by the DTMF receiver.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_DTMFR_PDT 0x4000
/** get */
#define V2CPE_EDSP1_STAT1_DTMFR_PDT_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_DTMFR_PDT) >> 14)
/** set */
#define V2CPE_EDSP1_STAT1_DTMFR_PDT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFR_PDT) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFR_PDT */




/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFR_DTC V2CPE_EDSP1_STAT1_DTMFR_DTC
 * DTMF Receiver Key Decode. Valid DTMF keys decoded by the DTMF receiver, see DTMF Receiver Key Decode .
 */
/*@{*/

#define V2CPE_EDSP1_STAT1_DTMFR_DTC_MASK   0x3C00

#define V2CPE_EDSP1_STAT1_DTMFR_DTC_GET(reg) (((reg)>> 10) & ((1 << 4) - 1))
#define V2CPE_EDSP1_STAT1_DTMFR_DTC_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) )
/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFR_DTC */




/**
 * \defgroup V2CPE_EDSP1_STAT1_UTD1_OK V2CPE_EDSP1_STAT1_UTD1_OK
 * Universal Tone Detection Receive. (such as Fax/Modem tones).
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_UTD1_OK 0x0200
/** get */
#define V2CPE_EDSP1_STAT1_UTD1_OK_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_UTD1_OK) >> 9)
/** set */
#define V2CPE_EDSP1_STAT1_UTD1_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_UTD1_OK) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_EDSP1_STAT1_UTD1_OK */




/**
 * \defgroup V2CPE_EDSP1_STAT1_UTD2_OK V2CPE_EDSP1_STAT1_UTD2_OK
 * Universal Tone Detection Transmit. (such as fax/modem tones).
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_UTD2_OK 0x0100
/** get */
#define V2CPE_EDSP1_STAT1_UTD2_OK_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_UTD2_OK) >> 8)
/** set */
#define V2CPE_EDSP1_STAT1_UTD2_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_UTD2_OK) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_STAT1_UTD2_OK */




/**
 * \defgroup V2CPE_EDSP1_STAT1_IOM_EVT_CHG V2CPE_EDSP1_STAT1_IOM_EVT_CHG
 * IOM_EVT_CHG.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_IOM_EVT_CHG 0x0080
/** get */
#define V2CPE_EDSP1_STAT1_IOM_EVT_CHG_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_IOM_EVT_CHG) >> 7)
/** set */
#define V2CPE_EDSP1_STAT1_IOM_EVT_CHG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_IOM_EVT_CHG) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_EDSP1_STAT1_IOM_EVT_CHG */




/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFG_BUF V2CPE_EDSP1_STAT1_DTMFG_BUF
 * DTMF Generator Buffer. Underflow of the DTMF generator input buffer.The DTMF/AT generator has completely sent all signs.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_DTMFG_BUF 0x0020
/** get */
#define V2CPE_EDSP1_STAT1_DTMFG_BUF_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_DTMFG_BUF) >> 5)
/** set */
#define V2CPE_EDSP1_STAT1_DTMFG_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFG_BUF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFG_BUF */




/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFG_REQ V2CPE_EDSP1_STAT1_DTMFG_REQ
 * DTMF Generator Request. The DTMF/AT generator is sending the last two signs and requests a new sign.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_DTMFG_REQ 0x0010
/** get */
#define V2CPE_EDSP1_STAT1_DTMFG_REQ_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_DTMFG_REQ) >> 4)
/** set */
#define V2CPE_EDSP1_STAT1_DTMFG_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFG_REQ) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFG_REQ */




/**
 * \defgroup V2CPE_EDSP1_STAT1_DTMFG_ACT V2CPE_EDSP1_STAT1_DTMFG_ACT
 * DTMF Generator Active. DTMF/AT generator is active.This is a status bit only.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_DTMFG_ACT 0x0008
/** get */
#define V2CPE_EDSP1_STAT1_DTMFG_ACT_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_DTMFG_ACT) >> 3)
/** set */
#define V2CPE_EDSP1_STAT1_DTMFG_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_DTMFG_ACT) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_STAT1_DTMFG_ACT */




/**
 * \defgroup V2CPE_EDSP1_STAT1_CIS_BUF V2CPE_EDSP1_STAT1_CIS_BUF
 * Caller ID Input Buffer Underflow. The generator has completely sent all data and inserts stop bits.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_CIS_BUF 0x0004
/** get */
#define V2CPE_EDSP1_STAT1_CIS_BUF_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_CIS_BUF) >> 2)
/** set */
#define V2CPE_EDSP1_STAT1_CIS_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_CIS_BUF) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_STAT1_CIS_BUF */




/**
 * \defgroup V2CPE_EDSP1_STAT1_CIS_REQ V2CPE_EDSP1_STAT1_CIS_REQ
 * Caller ID Request. Caller ID data buffer requests more data to transmit, when the amount of data stored in the buffer is less than the buffer request size.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_CIS_REQ 0x0002
/** get */
#define V2CPE_EDSP1_STAT1_CIS_REQ_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_CIS_REQ) >> 1)
/** set */
#define V2CPE_EDSP1_STAT1_CIS_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_CIS_REQ) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_STAT1_CIS_REQ */




/**
 * \defgroup V2CPE_EDSP1_STAT1_CIS_ACT V2CPE_EDSP1_STAT1_CIS_ACT
 * Caller ID Generator Active. Indicates activity of the Caller ID Generator.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT1_CIS_ACT 0x0001
/** get */
#define V2CPE_EDSP1_STAT1_CIS_ACT_GET(reg) (((reg) & V2CPE_EDSP1_STAT1_CIS_ACT) >> 0)
/** set */
#define V2CPE_EDSP1_STAT1_CIS_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT1_CIS_ACT) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_STAT1_CIS_ACT */



/*@}*/ /*  V2CPE_EDSP1_STAT1 */



/**
 * \defgroup V2CPE_EDSP1_INT1 V2CPE_EDSP1_INT1
 * EDSP Interrupt Register 1. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INT1 0x70
/** reset value */
#define V2CPE_EDSP1_INT1_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFR_DT V2CPE_EDSP1_INT1_DTMFR_DT
 * DTMF Receiver Key Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_DTMFR_DT 0x8000
/** get */
#define V2CPE_EDSP1_INT1_DTMFR_DT_GET(reg) (((reg) & V2CPE_EDSP1_INT1_DTMFR_DT) >> 15)
/** set */
#define V2CPE_EDSP1_INT1_DTMFR_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFR_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INT1_DTMFR_DT */




/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFR_PDT V2CPE_EDSP1_INT1_DTMFR_PDT
 * DTMF Receiver Probably Key Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_DTMFR_PDT 0x4000
/** get */
#define V2CPE_EDSP1_INT1_DTMFR_PDT_GET(reg) (((reg) & V2CPE_EDSP1_INT1_DTMFR_PDT) >> 14)
/** set */
#define V2CPE_EDSP1_INT1_DTMFR_PDT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFR_PDT) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_EDSP1_INT1_DTMFR_PDT */




/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFR_DTC V2CPE_EDSP1_INT1_DTMFR_DTC
 * DTMF Receiver Key Decode.
 */
/*@{*/

#define V2CPE_EDSP1_INT1_DTMFR_DTC_MASK    0x3C00

#define V2CPE_EDSP1_INT1_DTMFR_DTC_GET(reg) (((reg)>> 10) & ((1 << 4) - 1))
#define V2CPE_EDSP1_INT1_DTMFR_DTC_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) )
/*@}*/ /* V2CPE_EDSP1_INT1_DTMFR_DTC */




/**
 * \defgroup V2CPE_EDSP1_INT1_UTD1_OK V2CPE_EDSP1_INT1_UTD1_OK
 * Universal Tone Detection Receive.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_UTD1_OK 0x0200
/** get */
#define V2CPE_EDSP1_INT1_UTD1_OK_GET(reg) (((reg) & V2CPE_EDSP1_INT1_UTD1_OK) >> 9)
/** set */
#define V2CPE_EDSP1_INT1_UTD1_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_UTD1_OK) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_EDSP1_INT1_UTD1_OK */




/**
 * \defgroup V2CPE_EDSP1_INT1_UTD2_OK V2CPE_EDSP1_INT1_UTD2_OK
 * Universal Tone Detection Transmit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_UTD2_OK 0x0100
/** get */
#define V2CPE_EDSP1_INT1_UTD2_OK_GET(reg) (((reg) & V2CPE_EDSP1_INT1_UTD2_OK) >> 8)
/** set */
#define V2CPE_EDSP1_INT1_UTD2_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_UTD2_OK) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INT1_UTD2_OK */




/**
 * \defgroup V2CPE_EDSP1_INT1_CPT V2CPE_EDSP1_INT1_CPT
 * Call Progress Tone Detection.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_CPT 0x0080
/** get */
#define V2CPE_EDSP1_INT1_CPT_GET(reg) (((reg) & V2CPE_EDSP1_INT1_CPT) >> 7)
/** set */
#define V2CPE_EDSP1_INT1_CPT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_CPT) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_EDSP1_INT1_CPT */




/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFG_BUF V2CPE_EDSP1_INT1_DTMFG_BUF
 * DTMF Generator Buffer.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_DTMFG_BUF 0x0020
/** get */
#define V2CPE_EDSP1_INT1_DTMFG_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INT1_DTMFG_BUF) >> 5)
/** set */
#define V2CPE_EDSP1_INT1_DTMFG_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFG_BUF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_INT1_DTMFG_BUF */




/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFG_REQ V2CPE_EDSP1_INT1_DTMFG_REQ
 * DTMF Generator Request.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_DTMFG_REQ 0x0010
/** get */
#define V2CPE_EDSP1_INT1_DTMFG_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INT1_DTMFG_REQ) >> 4)
/** set */
#define V2CPE_EDSP1_INT1_DTMFG_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFG_REQ) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INT1_DTMFG_REQ */




/**
 * \defgroup V2CPE_EDSP1_INT1_DTMFG_ACT V2CPE_EDSP1_INT1_DTMFG_ACT
 * DTMF Generator Active.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_DTMFG_ACT 0x0008
/** get */
#define V2CPE_EDSP1_INT1_DTMFG_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INT1_DTMFG_ACT) >> 3)
/** set */
#define V2CPE_EDSP1_INT1_DTMFG_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_DTMFG_ACT) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INT1_DTMFG_ACT */




/**
 * \defgroup V2CPE_EDSP1_INT1_CIS_BUF V2CPE_EDSP1_INT1_CIS_BUF
 * Caller ID Input Buffer Underflow.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_CIS_BUF 0x0004
/** get */
#define V2CPE_EDSP1_INT1_CIS_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INT1_CIS_BUF) >> 2)
/** set */
#define V2CPE_EDSP1_INT1_CIS_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_CIS_BUF) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INT1_CIS_BUF */




/**
 * \defgroup V2CPE_EDSP1_INT1_CIS_REQ V2CPE_EDSP1_INT1_CIS_REQ
 * Caller ID Request.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_CIS_REQ 0x0002
/** get */
#define V2CPE_EDSP1_INT1_CIS_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INT1_CIS_REQ) >> 1)
/** set */
#define V2CPE_EDSP1_INT1_CIS_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_CIS_REQ) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_INT1_CIS_REQ */




/**
 * \defgroup V2CPE_EDSP1_INT1_CIS_ACT V2CPE_EDSP1_INT1_CIS_ACT
 * Caller ID Generator Active.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT1_CIS_ACT 0x0001
/** get */
#define V2CPE_EDSP1_INT1_CIS_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INT1_CIS_ACT) >> 0)
/** set */
#define V2CPE_EDSP1_INT1_CIS_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT1_CIS_ACT) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_INT1_CIS_ACT */



/*@}*/ /*  V2CPE_EDSP1_INT1 */



/**
 * \defgroup V2CPE_EDSP1_INTR1 V2CPE_EDSP1_INTR1
 * EDSP Interrupt Enable Register 1. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INTR1 0xA0
/** reset value */
#define V2CPE_EDSP1_INTR1_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFR_DT V2CPE_EDSP1_INTR1_DTMFR_DT
 * Mask for DTMFR-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_DTMFR_DT 0x8000
/** get */
#define V2CPE_EDSP1_INTR1_DTMFR_DT_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_DTMFR_DT) >> 15)
/** set */
#define V2CPE_EDSP1_INTR1_DTMFR_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFR_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFR_DT */




/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFR_PDT V2CPE_EDSP1_INTR1_DTMFR_PDT
 * Mask for DTMFR-PDT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_DTMFR_PDT 0x4000
/** get */
#define V2CPE_EDSP1_INTR1_DTMFR_PDT_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_DTMFR_PDT) >> 14)
/** set */
#define V2CPE_EDSP1_INTR1_DTMFR_PDT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFR_PDT) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFR_PDT */




/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFR_DTC V2CPE_EDSP1_INTR1_DTMFR_DTC
 * Mask for DTMFR-DTC Bits.
 */
/*@{*/

#define V2CPE_EDSP1_INTR1_DTMFR_DTC_MASK   0x3C00

#define V2CPE_EDSP1_INTR1_DTMFR_DTC_GET(reg) (((reg)>> 10) & ((1 << 4) - 1))
#define V2CPE_EDSP1_INTR1_DTMFR_DTC_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) )
/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFR_DTC */




/**
 * \defgroup V2CPE_EDSP1_INTR1_UTD1_OK V2CPE_EDSP1_INTR1_UTD1_OK
 * Mask for UTD1-OK Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_UTD1_OK 0x0200
/** get */
#define V2CPE_EDSP1_INTR1_UTD1_OK_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_UTD1_OK) >> 9)
/** set */
#define V2CPE_EDSP1_INTR1_UTD1_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_UTD1_OK) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_EDSP1_INTR1_UTD1_OK */




/**
 * \defgroup V2CPE_EDSP1_INTR1_UTD2_OK V2CPE_EDSP1_INTR1_UTD2_OK
 * Mask for UTD2-OK Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_UTD2_OK 0x0100
/** get */
#define V2CPE_EDSP1_INTR1_UTD2_OK_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_UTD2_OK) >> 8)
/** set */
#define V2CPE_EDSP1_INTR1_UTD2_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_UTD2_OK) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INTR1_UTD2_OK */



/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFG_BUF V2CPE_EDSP1_INTR1_DTMFG_BUF
 * Mask for DTMFG-BUF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_CIDR_OF 0x0040
/** get */
#define V2CPE_EDSP1_INTR1_CIDR_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_CIDR_OF) >> 6)
/** set */
#define V2CPE_EDSP1_INTR1_CIDR_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_CIDR_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_INTR1_CIDR_OF */



/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFG_BUF V2CPE_EDSP1_INTR1_DTMFG_BUF
 * Mask for DTMFG-BUF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_DTMFG_BUF 0x0020
/** get */
#define V2CPE_EDSP1_INTR1_DTMFG_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_DTMFG_BUF) >> 5)
/** set */
#define V2CPE_EDSP1_INTR1_DTMFG_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFG_BUF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFG_BUF */




/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFG_REQ V2CPE_EDSP1_INTR1_DTMFG_REQ
 * Mask for DTMFG-REQ Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_DTMFG_REQ 0x0010
/** get */
#define V2CPE_EDSP1_INTR1_DTMFG_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_DTMFG_REQ) >> 4)
/** set */
#define V2CPE_EDSP1_INTR1_DTMFG_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFG_REQ) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFG_REQ */




/**
 * \defgroup V2CPE_EDSP1_INTR1_DTMFG_ACT V2CPE_EDSP1_INTR1_DTMFG_ACT
 * Mask for DTMFG-ACT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_DTMFG_ACT 0x0008
/** get */
#define V2CPE_EDSP1_INTR1_DTMFG_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_DTMFG_ACT) >> 3)
/** set */
#define V2CPE_EDSP1_INTR1_DTMFG_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_DTMFG_ACT) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INTR1_DTMFG_ACT */




/**
 * \defgroup V2CPE_EDSP1_INTR1_CIS_BUF V2CPE_EDSP1_INTR1_CIS_BUF
 * Mask for CIS-BUF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_CIS_BUF 0x0004
/** get */
#define V2CPE_EDSP1_INTR1_CIS_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_CIS_BUF) >> 2)
/** set */
#define V2CPE_EDSP1_INTR1_CIS_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_CIS_BUF) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INTR1_CIS_BUF */




/**
 * \defgroup V2CPE_EDSP1_INTR1_CIS_REQ V2CPE_EDSP1_INTR1_CIS_REQ
 * Mask for CIS-REQ Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_CIS_REQ 0x0002
/** get */
#define V2CPE_EDSP1_INTR1_CIS_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_CIS_REQ) >> 1)
/** set */
#define V2CPE_EDSP1_INTR1_CIS_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_CIS_REQ) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_INTR1_CIS_REQ */




/**
 * \defgroup V2CPE_EDSP1_INTR1_CIS_ACT V2CPE_EDSP1_INTR1_CIS_ACT
 * Mask for CIS-ACT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR1_CIS_ACT 0x0001
/** get */
#define V2CPE_EDSP1_INTR1_CIS_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INTR1_CIS_ACT) >> 0)
/** set */
#define V2CPE_EDSP1_INTR1_CIS_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR1_CIS_ACT) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_INTR1_CIS_ACT */



/*@}*/ /*  V2CPE_EDSP1_INTR1 */



/**
 * \defgroup V2CPE_EDSP1_INTF1 V2CPE_EDSP1_INTF1
 * EDSP Interrupt Enable Register 1. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INTF1 0xD0
/** reset value */
#define V2CPE_EDSP1_INTF1_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFRDT V2CPE_EDSP1_INTF1_DTMFRDT
 * Mask for DTMFR-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_DTMFRDT 0x8000
/** get */
#define V2CPE_EDSP1_INTF1_DTMFRDT_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_DTMFRDT) >> 15)
/** set */
#define V2CPE_EDSP1_INTF1_DTMFRDT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFRDT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFRDT */




/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFRPD V2CPE_EDSP1_INTF1_DTMFRPD
 * Mask for DTMFR-PDT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_DTMFRPD 0x4000
/** get */
#define V2CPE_EDSP1_INTF1_DTMFRPD_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_DTMFRPD) >> 14)
/** set */
#define V2CPE_EDSP1_INTF1_DTMFRPD_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFRPD) | (((val) & 1) << 14))

/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFRPD */




/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFR_DTC V2CPE_EDSP1_INTF1_DTMFR_DTC
 * Mask for DTMFR-DTC Bits.
 */
/*@{*/

#define V2CPE_EDSP1_INTF1_DTMFR_DTC_MASK   0x3C00

#define V2CPE_EDSP1_INTF1_DTMFR_DTC_GET(reg) (((reg)>> 10) & ((1 << 4) - 1))
#define V2CPE_EDSP1_INTF1_DTMFR_DTC_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFR_DTC_MASK) | ((((1 << 4) - 1) & (val)) << 10) )
/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFR_DTC */




/**
 * \defgroup V2CPE_EDSP1_INTF1_UTD1_OK V2CPE_EDSP1_INTF1_UTD1_OK
 * Mask for UTD1-OK Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_UTD1_OK 0x0200
/** get */
#define V2CPE_EDSP1_INTF1_UTD1_OK_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_UTD1_OK) >> 9)
/** set */
#define V2CPE_EDSP1_INTF1_UTD1_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_UTD1_OK) | (((val) & 1) << 9))

/*@}*/ /* V2CPE_EDSP1_INTF1_UTD1_OK */




/**
 * \defgroup V2CPE_EDSP1_INTF1_UTD2_OK V2CPE_EDSP1_INTF1_UTD2_OK
 * Mask for UTD2-OK Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_UTD2_OK 0x0100
/** get */
#define V2CPE_EDSP1_INTF1_UTD2_OK_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_UTD2_OK) >> 8)
/** set */
#define V2CPE_EDSP1_INTF1_UTD2_OK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_UTD2_OK) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INTF1_UTD2_OK */


/** mask */
#define V2CPE_EDSP1_INTF1_CIDR_OF 0x0040
/** get */
#define V2CPE_EDSP1_INTF1_CIDR_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_CIDR_OF) >> 6)
/** set */
#define V2CPE_EDSP1_INTF1_CIDR_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_CIDR_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_INTF1_CIDR_OF */



/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFG_BUF V2CPE_EDSP1_INTF1_DTMFG_BUF
 * Mask for DTMFG-BUF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_DTMFG_BUF 0x0020
/** get */
#define V2CPE_EDSP1_INTF1_DTMFG_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_DTMFG_BUF) >> 5)
/** set */
#define V2CPE_EDSP1_INTF1_DTMFG_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFG_BUF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFG_BUF */




/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFG_REQ V2CPE_EDSP1_INTF1_DTMFG_REQ
 * Mask for DTMFG-REQ Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_DTMFG_REQ 0x0010
/** get */
#define V2CPE_EDSP1_INTF1_DTMFG_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_DTMFG_REQ) >> 4)
/** set */
#define V2CPE_EDSP1_INTF1_DTMFG_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFG_REQ) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFG_REQ */




/**
 * \defgroup V2CPE_EDSP1_INTF1_DTMFG_ACT V2CPE_EDSP1_INTF1_DTMFG_ACT
 * Mask for DTMFG-ACT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_DTMFG_ACT 0x0008
/** get */
#define V2CPE_EDSP1_INTF1_DTMFG_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_DTMFG_ACT) >> 3)
/** set */
#define V2CPE_EDSP1_INTF1_DTMFG_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_DTMFG_ACT) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INTF1_DTMFG_ACT */




/**
 * \defgroup V2CPE_EDSP1_INTF1_CIS_BUF V2CPE_EDSP1_INTF1_CIS_BUF
 * Mask for CIS-BUF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_CIS_BUF 0x0004
/** get */
#define V2CPE_EDSP1_INTF1_CIS_BUF_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_CIS_BUF) >> 2)
/** set */
#define V2CPE_EDSP1_INTF1_CIS_BUF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_CIS_BUF) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INTF1_CIS_BUF */




/**
 * \defgroup V2CPE_EDSP1_INTF1_CIS_REQ V2CPE_EDSP1_INTF1_CIS_REQ
 * Mask for CIS-REQ Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_CIS_REQ 0x0002
/** get */
#define V2CPE_EDSP1_INTF1_CIS_REQ_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_CIS_REQ) >> 1)
/** set */
#define V2CPE_EDSP1_INTF1_CIS_REQ_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_CIS_REQ) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_INTF1_CIS_REQ */




/**
 * \defgroup V2CPE_EDSP1_INTF1_CIS_ACT V2CPE_EDSP1_INTF1_CIS_ACT
 * Mask for CIS-ACT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF1_CIS_ACT 0x0001
/** get */
#define V2CPE_EDSP1_INTF1_CIS_ACT_GET(reg) (((reg) & V2CPE_EDSP1_INTF1_CIS_ACT) >> 0)
/** set */
#define V2CPE_EDSP1_INTF1_CIS_ACT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF1_CIS_ACT) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_INTF1_CIS_ACT */



/*@}*/ /*  V2CPE_EDSP1_INTF1 */



/**
 * \defgroup V2CPE_EDSP1_STAT2 V2CPE_EDSP1_STAT2
 * EDSP Status Register 2. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_STAT2 0x42
/** reset value */
#define V2CPE_EDSP1_STAT2_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD1_DT V2CPE_EDSP1_STAT2_ATD1_DT
 * ATD1 Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_ATD1_DT 0x8000
/** get */
#define V2CPE_EDSP1_STAT2_ATD1_DT_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_ATD1_DT) >> 15)
/** set */
#define V2CPE_EDSP1_STAT2_ATD1_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD1_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_STAT2_ATD1_DT */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD1_NPR V2CPE_EDSP1_STAT2_ATD1_NPR
 * ATD1 Phase Reversals.
 */
/*@{*/

#define V2CPE_EDSP1_STAT2_ATD1_NPR_MASK    0x6000

#define V2CPE_EDSP1_STAT2_ATD1_NPR_GET(reg) (((reg)>> 13) & ((1 << 2) - 1))
#define V2CPE_EDSP1_STAT2_ATD1_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) )
/*@}*/ /* V2CPE_EDSP1_STAT2_ATD1_NPR */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD1_AM V2CPE_EDSP1_STAT2_ATD1_AM
 * ATD1 Amplitude Modulation. ATD1: Amplitude modulation detection.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_ATD1_AM 0x1000
/** get */
#define V2CPE_EDSP1_STAT2_ATD1_AM_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_ATD1_AM) >> 12)
/** set */
#define V2CPE_EDSP1_STAT2_ATD1_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD1_AM) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_EDSP1_STAT2_ATD1_AM */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD2_DT V2CPE_EDSP1_STAT2_ATD2_DT
 * ATD2 Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_ATD2_DT 0x0800
/** get */
#define V2CPE_EDSP1_STAT2_ATD2_DT_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_ATD2_DT) >> 11)
/** set */
#define V2CPE_EDSP1_STAT2_ATD2_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD2_DT) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_EDSP1_STAT2_ATD2_DT */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD2_NPR V2CPE_EDSP1_STAT2_ATD2_NPR
 * ATD2 Phase Reversals.
 */
/*@{*/

#define V2CPE_EDSP1_STAT2_ATD2_NPR_MASK    0x0600

#define V2CPE_EDSP1_STAT2_ATD2_NPR_GET(reg) (((reg)>> 9) & ((1 << 2) - 1))
#define V2CPE_EDSP1_STAT2_ATD2_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) )
/*@}*/ /* V2CPE_EDSP1_STAT2_ATD2_NPR */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ATD2_AM V2CPE_EDSP1_STAT2_ATD2_AM
 * ATD2 Amplitude Modulation.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_ATD2_AM 0x0100
/** get */
#define V2CPE_EDSP1_STAT2_ATD2_AM_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_ATD2_AM) >> 8)
/** set */
#define V2CPE_EDSP1_STAT2_ATD2_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ATD2_AM) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_STAT2_ATD2_AM */




/**
 * \defgroup V2CPE_EDSP1_STAT2_EPOU_ST V2CPE_EDSP1_STAT2_EPOU_ST
 * Event Play Out Unit Statistic. The event play out unit has an invalid, duplicate, late, early or event detected and therefore the event statistic has been modified.The host can read the event statistic via the “Command “Signaling Channel Event Statistic” [M-V]” on Page 278. After this command the bit will be reset.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_EPOU_ST 0x0080
/** get */
#define V2CPE_EDSP1_STAT2_EPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_EPOU_ST) >> 7)
/** set */
#define V2CPE_EDSP1_STAT2_EPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_EPOU_ST) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_EDSP1_STAT2_EPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_STAT2_ETU_OF V2CPE_EDSP1_STAT2_ETU_OF
 * Event Transmit Unit Overflow. The event transmit unit couldn't send the event to the data manager. The event was discarded.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_ETU_OF 0x0040
/** get */
#define V2CPE_EDSP1_STAT2_ETU_OF_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_ETU_OF) >> 6)
/** set */
#define V2CPE_EDSP1_STAT2_ETU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_ETU_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_STAT2_ETU_OF */




/**
 * \defgroup V2CPE_EDSP1_STAT2_PVPU_OF V2CPE_EDSP1_STAT2_PVPU_OF
 * Packetized Voice Protocol Unit Overflow. Indicates overflow of the packet out-box – thePacketized Voice Protocol Unitcouldn't write the output packet into the packet out-box. The data packet was discarded!
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_PVPU_OF 0x0020
/** get */
#define V2CPE_EDSP1_STAT2_PVPU_OF_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_PVPU_OF) >> 5)
/** set */
#define V2CPE_EDSP1_STAT2_PVPU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_PVPU_OF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_STAT2_PVPU_OF */




/**
 * \defgroup V2CPE_EDSP1_STAT2_VPOU_ST V2CPE_EDSP1_STAT2_VPOU_ST
 * Voice Play Out Unit Statistics. The voice play out unit has modified the jitter buffer statistic. A voice packet has been received which was invalid, received to late, received to early or was duplicated or a lost packet has been detected.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_VPOU_ST 0x0010
/** get */
#define V2CPE_EDSP1_STAT2_VPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_VPOU_ST) >> 4)
/** set */
#define V2CPE_EDSP1_STAT2_VPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_VPOU_ST) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_STAT2_VPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_STAT2_VPOU_JBL V2CPE_EDSP1_STAT2_VPOU_JBL
 * Voice Play Out Unit Jitter Buffer Low. The voice play out unit has reached the jitter buffer low limit. This bit is set if the jitter buffer size is below the lower limit (see “Command “Coder Channel JB Configuration” [M-V]” on Page 310, MIN_JB_SIZE).Otherwise the bit will be cleared. The bit VPOU-JBL will not be supported in the non adaptive mode (ADAP == 0).
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_VPOU_JBL 0x0008
/** get */
#define V2CPE_EDSP1_STAT2_VPOU_JBL_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_VPOU_JBL) >> 3)
/** set */
#define V2CPE_EDSP1_STAT2_VPOU_JBL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_VPOU_JBL) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_STAT2_VPOU_JBL */




/**
 * \defgroup V2CPE_EDSP1_STAT2_VPOU_JBH V2CPE_EDSP1_STAT2_VPOU_JBH
 * Voice Play Out Unit Jitter Buffer High. The voice play out unit has reached the jitter buffer high limit. This bit is set if the jitter buffer size is above the higher limit (see “Command “Coder Channel JB Configuration” [M-V]” on Page 310, MAX_JB_SIZE).Otherwise the bit will be cleared. The bit JBH will not be supported in the non adaptive mode (ADAP == 0).
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_VPOU_JBH 0x0004
/** get */
#define V2CPE_EDSP1_STAT2_VPOU_JBH_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_VPOU_JBH) >> 2)
/** set */
#define V2CPE_EDSP1_STAT2_VPOU_JBH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_VPOU_JBH) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_STAT2_VPOU_JBH */




/**
 * \defgroup V2CPE_EDSP1_STAT2_DEC_ERR V2CPE_EDSP1_STAT2_DEC_ERR
 * Decoder Error. The requested Decoder can't be activated due to the CL Bits in the “Command “Coder Channel” [M-V]” on Page 285.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_DEC_ERR 0x0002
/** get */
#define V2CPE_EDSP1_STAT2_DEC_ERR_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_DEC_ERR) >> 1)
/** set */
#define V2CPE_EDSP1_STAT2_DEC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_DEC_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_STAT2_DEC_ERR */




/**
 * \defgroup V2CPE_EDSP1_STAT2_DEC_CHG V2CPE_EDSP1_STAT2_DEC_CHG
 * Decoder Change. The kind of decoder or the packet time has been changed.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_STAT2_DEC_CHG 0x0001
/** get */
#define V2CPE_EDSP1_STAT2_DEC_CHG_GET(reg) (((reg) & V2CPE_EDSP1_STAT2_DEC_CHG) >> 0)
/** set */
#define V2CPE_EDSP1_STAT2_DEC_CHG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_STAT2_DEC_CHG) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_STAT2_DEC_CHG */



/*@}*/ /*  V2CPE_EDSP1_STAT2 */



/**
 * \defgroup V2CPE_EDSP1_INT2 V2CPE_EDSP1_INT2
 * EDSP Interrupt Register 2. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INT2 0x72
/** reset value */
#define V2CPE_EDSP1_INT2_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INT2_ATD1_DT V2CPE_EDSP1_INT2_ATD1_DT
 * ATD1 Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_ATD1_DT 0x8000
/** get */
#define V2CPE_EDSP1_INT2_ATD1_DT_GET(reg) (((reg) & V2CPE_EDSP1_INT2_ATD1_DT) >> 15)
/** set */
#define V2CPE_EDSP1_INT2_ATD1_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD1_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INT2_ATD1_DT */




/**
 * \defgroup V2CPE_EDSP1_INT2_ATD1_NPR V2CPE_EDSP1_INT2_ATD1_NPR
 * ATD1 Phase Reversals.
 */
/*@{*/

#define V2CPE_EDSP1_INT2_ATD1_NPR_MASK  0x6000

#define V2CPE_EDSP1_INT2_ATD1_NPR_GET(reg) (((reg)>> 13) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INT2_ATD1_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) )
/*@}*/ /* V2CPE_EDSP1_INT2_ATD1_NPR */




/**
 * \defgroup V2CPE_EDSP1_INT2_ATD1_AM V2CPE_EDSP1_INT2_ATD1_AM
 * ATD1 Amplitude Modulation.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_ATD1_AM 0x1000
/** get */
#define V2CPE_EDSP1_INT2_ATD1_AM_GET(reg) (((reg) & V2CPE_EDSP1_INT2_ATD1_AM) >> 12)
/** set */
#define V2CPE_EDSP1_INT2_ATD1_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD1_AM) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_EDSP1_INT2_ATD1_AM */




/**
 * \defgroup V2CPE_EDSP1_INT2_ATD2_DT V2CPE_EDSP1_INT2_ATD2_DT
 * ATD2 Detect.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_ATD2_DT 0x0800
/** get */
#define V2CPE_EDSP1_INT2_ATD2_DT_GET(reg) (((reg) & V2CPE_EDSP1_INT2_ATD2_DT) >> 11)
/** set */
#define V2CPE_EDSP1_INT2_ATD2_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD2_DT) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_EDSP1_INT2_ATD2_DT */




/**
 * \defgroup V2CPE_EDSP1_INT2_ATD2_NPR V2CPE_EDSP1_INT2_ATD2_NPR
 * ATD2 Phase Reversals.
 */
/*@{*/

#define V2CPE_EDSP1_INT2_ATD2_NPR_MASK  0x0600

#define V2CPE_EDSP1_INT2_ATD2_NPR_GET(reg) (((reg)>> 9) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INT2_ATD2_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) )
/*@}*/ /* V2CPE_EDSP1_INT2_ATD2_NPR */




/**
 * \defgroup V2CPE_EDSP1_INT2_ATD2_AM V2CPE_EDSP1_INT2_ATD2_AM
 * ATD2 Amplitude Modulation.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_ATD2_AM 0x0100
/** get */
#define V2CPE_EDSP1_INT2_ATD2_AM_GET(reg) (((reg) & V2CPE_EDSP1_INT2_ATD2_AM) >> 8)
/** set */
#define V2CPE_EDSP1_INT2_ATD2_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ATD2_AM) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INT2_ATD2_AM */




/**
 * \defgroup V2CPE_EDSP1_INT2_EPOU_ST V2CPE_EDSP1_INT2_EPOU_ST
 * Event Play Out Unit Statistic.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_EPOU_ST 0x0080
/** get */
#define V2CPE_EDSP1_INT2_EPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_INT2_EPOU_ST) >> 7)
/** set */
#define V2CPE_EDSP1_INT2_EPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_EPOU_ST) | (((val) & 1) << 7))

/*@}*/ /* V2CPE_EDSP1_INT2_EPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_INT2_ETU_OF V2CPE_EDSP1_INT2_ETU_OF
 * Event Transmit Unit Overflow.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_ETU_OF 0x0040
/** get */
#define V2CPE_EDSP1_INT2_ETU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INT2_ETU_OF) >> 6)
/** set */
#define V2CPE_EDSP1_INT2_ETU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_ETU_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_INT2_ETU_OF */




/**
 * \defgroup V2CPE_EDSP1_INT2_PVPU_OF V2CPE_EDSP1_INT2_PVPU_OF
 * Packetized Voice Protocol Unit Overflow.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_PVPU_OF 0x0020
/** get */
#define V2CPE_EDSP1_INT2_PVPU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INT2_PVPU_OF) >> 5)
/** set */
#define V2CPE_EDSP1_INT2_PVPU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_PVPU_OF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_INT2_PVPU_OF */




/**
 * \defgroup V2CPE_EDSP1_INT2_VPOU_ST V2CPE_EDSP1_INT2_VPOU_ST
 * Voice Play Out Unit Statistics.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_VPOU_ST 0x0010
/** get */
#define V2CPE_EDSP1_INT2_VPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_INT2_VPOU_ST) >> 4)
/** set */
#define V2CPE_EDSP1_INT2_VPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_VPOU_ST) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INT2_VPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_INT2_VPOU_JBL V2CPE_EDSP1_INT2_VPOU_JBL
 * Voice Play Out Unit Jitter Buffer Low.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_VPOU_JBL 0x0008
/** get */
#define V2CPE_EDSP1_INT2_VPOU_JBL_GET(reg) (((reg) & V2CPE_EDSP1_INT2_VPOU_JBL) >> 3)
/** set */
#define V2CPE_EDSP1_INT2_VPOU_JBL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_VPOU_JBL) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INT2_VPOU_JBL */




/**
 * \defgroup V2CPE_EDSP1_INT2_VPOU_JBH V2CPE_EDSP1_INT2_VPOU_JBH
 * Voice Play Out Unit Jitter Buffer High.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_VPOU_JBH 0x0004
/** get */
#define V2CPE_EDSP1_INT2_VPOU_JBH_GET(reg) (((reg) & V2CPE_EDSP1_INT2_VPOU_JBH) >> 2)
/** set */
#define V2CPE_EDSP1_INT2_VPOU_JBH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_VPOU_JBH) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INT2_VPOU_JBH */




/**
 * \defgroup V2CPE_EDSP1_INT2_DEC_ERR V2CPE_EDSP1_INT2_DEC_ERR
 * Decoder Error.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_DEC_ERR 0x0002
/** get */
#define V2CPE_EDSP1_INT2_DEC_ERR_GET(reg) (((reg) & V2CPE_EDSP1_INT2_DEC_ERR) >> 1)
/** set */
#define V2CPE_EDSP1_INT2_DEC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_DEC_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_INT2_DEC_ERR */




/**
 * \defgroup V2CPE_EDSP1_INT2_DEC_CHG V2CPE_EDSP1_INT2_DEC_CHG
 * Decoder Change.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INT2_DEC_CHG 0x0001
/** get */
#define V2CPE_EDSP1_INT2_DEC_CHG_GET(reg) (((reg) & V2CPE_EDSP1_INT2_DEC_CHG) >> 0)
/** set */
#define V2CPE_EDSP1_INT2_DEC_CHG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INT2_DEC_CHG) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_INT2_DEC_CHG */



/*@}*/ /*  V2CPE_EDSP1_INT2 */



/**
 * \defgroup V2CPE_EDSP1_INTR2 V2CPE_EDSP1_INTR2
 * EDSP Inerrupt Enable Register 2. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INTR2 0xA2
/** reset value */
#define V2CPE_EDSP1_INTR2_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD1_DT V2CPE_EDSP1_INTR2_ATD1_DT
 * Mask for ATD1-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_ATD1_DT 0x8000
/** get */
#define V2CPE_EDSP1_INTR2_ATD1_DT_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_ATD1_DT) >> 15)
/** set */
#define V2CPE_EDSP1_INTR2_ATD1_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD1_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INTR2_ATD1_DT */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD1_NPR V2CPE_EDSP1_INTR2_ATD1_NPR
 * Mask for ATD1-NPR Bit.
 */
/*@{*/

#define V2CPE_EDSP1_INTR2_ATD1_NPR_MASK    0x6000

#define V2CPE_EDSP1_INTR2_ATD1_NPR_GET(reg) (((reg)>> 13) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INTR2_ATD1_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) )
/*@}*/ /* V2CPE_EDSP1_INTR2_ATD1_NPR */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD1_AM V2CPE_EDSP1_INTR2_ATD1_AM
 * Mask for ATD1-AM Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_ATD1_AM 0x1000
/** get */
#define V2CPE_EDSP1_INTR2_ATD1_AM_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_ATD1_AM) >> 12)
/** set */
#define V2CPE_EDSP1_INTR2_ATD1_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD1_AM) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_EDSP1_INTR2_ATD1_AM */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD2_DT V2CPE_EDSP1_INTR2_ATD2_DT
 * Mask for ATD2-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_ATD2_DT 0x0800
/** get */
#define V2CPE_EDSP1_INTR2_ATD2_DT_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_ATD2_DT) >> 11)
/** set */
#define V2CPE_EDSP1_INTR2_ATD2_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD2_DT) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_EDSP1_INTR2_ATD2_DT */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD2_NPR V2CPE_EDSP1_INTR2_ATD2_NPR
 * Mask for ATD2-NPR Bits.
 */
/*@{*/

#define V2CPE_EDSP1_INTR2_ATD2_NPR_MASK    0x0600

#define V2CPE_EDSP1_INTR2_ATD2_NPR_GET(reg) (((reg)>> 9) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INTR2_ATD2_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) )
/*@}*/ /* V2CPE_EDSP1_INTR2_ATD2_NPR */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ATD2_AM V2CPE_EDSP1_INTR2_ATD2_AM
 * Mask for ATD2-AM Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_ATD2_AM 0x0100
/** get */
#define V2CPE_EDSP1_INTR2_ATD2_AM_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_ATD2_AM) >> 8)
/** set */
#define V2CPE_EDSP1_INTR2_ATD2_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ATD2_AM) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INTR2_ATD2_AM */




/**
 * \defgroup V2CPE_EDSP1_INTR2_ETU_OF V2CPE_EDSP1_INTR2_ETU_OF
 * Mask for ETU-OF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_ETU_OF 0x0040
/** get */
#define V2CPE_EDSP1_INTR2_ETU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_ETU_OF) >> 6)
/** set */
#define V2CPE_EDSP1_INTR2_ETU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_ETU_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_INTR2_ETU_OF */




/**
 * \defgroup V2CPE_EDSP1_INTR2_PVPU_OF V2CPE_EDSP1_INTR2_PVPU_OF
 * Mask for PVPU-OF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_PVPU_OF 0x0020
/** get */
#define V2CPE_EDSP1_INTR2_PVPU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_PVPU_OF) >> 5)
/** set */
#define V2CPE_EDSP1_INTR2_PVPU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_PVPU_OF) | (((val) & 1) << 5))
/** mask */
#define V2CPE_EDSP1_INTR2_FDP_ERR      V2CPE_EDSP1_INTR2_PVPU_OF

/*@}*/ /* V2CPE_EDSP1_INTR2_PVPU_OF */




/**
 * \defgroup V2CPE_EDSP1_INTR2_VPOU_ST V2CPE_EDSP1_INTR2_VPOU_ST
 * Mask for VPOU-STAT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_VPOU_ST 0x0010
/** get */
#define V2CPE_EDSP1_INTR2_VPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_VPOU_ST) >> 4)
/** set */
#define V2CPE_EDSP1_INTR2_VPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_VPOU_ST) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INTR2_VPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_INTR2_VPOU_JBL V2CPE_EDSP1_INTR2_VPOU_JBL
 * Mask for VPOU-JB Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_VPOU_JBL 0x0008
/** get */
#define V2CPE_EDSP1_INTR2_VPOU_JBL_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_VPOU_JBL) >> 3)
/** set */
#define V2CPE_EDSP1_INTR2_VPOU_JBL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_VPOU_JBL) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INTR2_VPOU_JBL */




/**
 * \defgroup V2CPE_EDSP1_INTR2_VPOU_JBH V2CPE_EDSP1_INTR2_VPOU_JBH
 * Mask for VPOU-JB Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_VPOU_JBH 0x0004
/** get */
#define V2CPE_EDSP1_INTR2_VPOU_JBH_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_VPOU_JBH) >> 2)
/** set */
#define V2CPE_EDSP1_INTR2_VPOU_JBH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_VPOU_JBH) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INTR2_VPOU_JBH */




/**
 * \defgroup V2CPE_EDSP1_INTR2_DEC_ERR V2CPE_EDSP1_INTR2_DEC_ERR
 * Mask for DEC-ERR Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_DEC_ERR 0x0002
/** get */
#define V2CPE_EDSP1_INTR2_DEC_ERR_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_DEC_ERR) >> 1)
/** set */
#define V2CPE_EDSP1_INTR2_DEC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_DEC_ERR) | (((val) & 1) << 1))
/** mask */
#define V2CPE_EDSP1_INTR2_EPOU_TRIG    V2CPE_EDSP1_INTR2_DEC_ERR
/*@}*/ /* V2CPE_EDSP1_INTR2_DEC_ERR */




/**
 * \defgroup V2CPE_EDSP1_INTR2_DEC_CHG V2CPE_EDSP1_INTR2_DEC_CHG
 * Mask for DEC-CHG Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTR2_DEC_CHG 0x0001
/** get */
#define V2CPE_EDSP1_INTR2_DEC_CHG_GET(reg) (((reg) & V2CPE_EDSP1_INTR2_DEC_CHG) >> 0)
/** set */
#define V2CPE_EDSP1_INTR2_DEC_CHG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTR2_DEC_CHG) | (((val) & 1) << 0))
/** mask */
#define V2CPE_EDSP1_INTR2_FDP_REQ      V2CPE_EDSP1_INTR2_DEC_CHG

/*@}*/ /* V2CPE_EDSP1_INTR2_DEC_CHG */



/*@}*/ /*  V2CPE_EDSP1_INTR2 */



/**
 * \defgroup V2CPE_EDSP1_INTF2 V2CPE_EDSP1_INTF2
 * EDSP Interrupt Enable Register 2. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_EDSP1_INTF2 0xD2
/** reset value */
#define V2CPE_EDSP1_INTF2_RESET 0x0000


/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD1_DT V2CPE_EDSP1_INTF2_ATD1_DT
 * Mask for ATD1-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_ATD1_DT 0x8000
/** get */
#define V2CPE_EDSP1_INTF2_ATD1_DT_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_ATD1_DT) >> 15)
/** set */
#define V2CPE_EDSP1_INTF2_ATD1_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD1_DT) | (((val) & 1) << 15))

/*@}*/ /* V2CPE_EDSP1_INTF2_ATD1_DT */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD1_NPR V2CPE_EDSP1_INTF2_ATD1_NPR
 * Mask for ATD1-NPR Bit.
 */
/*@{*/

#define V2CPE_EDSP1_INTF2_ATD1_NPR_MASK    0x6000

#define V2CPE_EDSP1_INTF2_ATD1_NPR_GET(reg) (((reg)>> 13) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INTF2_ATD1_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD1_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 13) )
/*@}*/ /* V2CPE_EDSP1_INTF2_ATD1_NPR */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD1_AM V2CPE_EDSP1_INTF2_ATD1_AM
 * Mask for ATD1-AM Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_ATD1_AM 0x1000
/** get */
#define V2CPE_EDSP1_INTF2_ATD1_AM_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_ATD1_AM) >> 12)
/** set */
#define V2CPE_EDSP1_INTF2_ATD1_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD1_AM) | (((val) & 1) << 12))

/*@}*/ /* V2CPE_EDSP1_INTF2_ATD1_AM */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD2_DT V2CPE_EDSP1_INTF2_ATD2_DT
 * Mask for ATD2-DT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_ATD2_DT 0x0800
/** get */
#define V2CPE_EDSP1_INTF2_ATD2_DT_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_ATD2_DT) >> 11)
/** set */
#define V2CPE_EDSP1_INTF2_ATD2_DT_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD2_DT) | (((val) & 1) << 11))

/*@}*/ /* V2CPE_EDSP1_INTF2_ATD2_DT */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD2_NPR V2CPE_EDSP1_INTF2_ATD2_NPR
 * Mask for ATD2-NPR Bits.
 */
/*@{*/

#define V2CPE_EDSP1_INTF2_ATD2_NPR_MASK    0x0600

#define V2CPE_EDSP1_INTF2_ATD2_NPR_GET(reg) (((reg)>> 9) & ((1 << 2) - 1))
#define V2CPE_EDSP1_INTF2_ATD2_NPR_SET(reg, val) (reg) = ( ((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD2_NPR_MASK) | ((((1 << 2) - 1) & (val)) << 9) )
/*@}*/ /* V2CPE_EDSP1_INTF2_ATD2_NPR */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ATD2_AM V2CPE_EDSP1_INTF2_ATD2_AM
 * Mask for ATD2-AM Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_ATD2_AM 0x0100
/** get */
#define V2CPE_EDSP1_INTF2_ATD2_AM_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_ATD2_AM) >> 8)
/** set */
#define V2CPE_EDSP1_INTF2_ATD2_AM_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ATD2_AM) | (((val) & 1) << 8))

/*@}*/ /* V2CPE_EDSP1_INTF2_ATD2_AM */




/**
 * \defgroup V2CPE_EDSP1_INTF2_ETU_OF V2CPE_EDSP1_INTF2_ETU_OF
 * Mask for ETU-OF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_ETU_OF 0x0040
/** get */
#define V2CPE_EDSP1_INTF2_ETU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_ETU_OF) >> 6)
/** set */
#define V2CPE_EDSP1_INTF2_ETU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_ETU_OF) | (((val) & 1) << 6))

/*@}*/ /* V2CPE_EDSP1_INTF2_ETU_OF */




/**
 * \defgroup V2CPE_EDSP1_INTF2_PVPU_OF V2CPE_EDSP1_INTF2_PVPU_OF
 * Mask for PVPU-OF Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_PVPU_OF 0x0020
/** get */
#define V2CPE_EDSP1_INTF2_PVPU_OF_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_PVPU_OF) >> 5)
/** set */
#define V2CPE_EDSP1_INTF2_PVPU_OF_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_PVPU_OF) | (((val) & 1) << 5))

/*@}*/ /* V2CPE_EDSP1_INTF2_PVPU_OF */




/**
 * \defgroup V2CPE_EDSP1_INTF2_VPOU_ST V2CPE_EDSP1_INTF2_VPOU_ST
 * Mask for VPOU-STAT Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_VPOU_ST 0x0010
/** get */
#define V2CPE_EDSP1_INTF2_VPOU_ST_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_VPOU_ST) >> 4)
/** set */
#define V2CPE_EDSP1_INTF2_VPOU_ST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_VPOU_ST) | (((val) & 1) << 4))

/*@}*/ /* V2CPE_EDSP1_INTF2_VPOU_ST */




/**
 * \defgroup V2CPE_EDSP1_INTF2_VPOU_JBL V2CPE_EDSP1_INTF2_VPOU_JBL
 * Mask for VPOU-JBL Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_VPOU_JBL 0x0008
/** get */
#define V2CPE_EDSP1_INTF2_VPOU_JBL_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_VPOU_JBL) >> 3)
/** set */
#define V2CPE_EDSP1_INTF2_VPOU_JBL_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_VPOU_JBL) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_EDSP1_INTF2_VPOU_JBL */




/**
 * \defgroup V2CPE_EDSP1_INTF2_VPOU_JBH V2CPE_EDSP1_INTF2_VPOU_JBH
 * Mask for VPOU-JBH Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_VPOU_JBH 0x0004
/** get */
#define V2CPE_EDSP1_INTF2_VPOU_JBH_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_VPOU_JBH) >> 2)
/** set */
#define V2CPE_EDSP1_INTF2_VPOU_JBH_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_VPOU_JBH) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_EDSP1_INTF2_VPOU_JBH */




/**
 * \defgroup V2CPE_EDSP1_INTF2_DEC_ERR V2CPE_EDSP1_INTF2_DEC_ERR
 * Mask for DEC-ERR Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_DEC_ERR 0x0002
/** get */
#define V2CPE_EDSP1_INTF2_DEC_ERR_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_DEC_ERR) >> 1)
/** set */
#define V2CPE_EDSP1_INTF2_DEC_ERR_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_DEC_ERR) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_EDSP1_INTF2_DEC_ERR */




/**
 * \defgroup V2CPE_EDSP1_INTF2_DEC_CHG V2CPE_EDSP1_INTF2_DEC_CHG
 * Mask for DEC-CHG Bit.
 */
/*@{*/

/** mask */
#define V2CPE_EDSP1_INTF2_DEC_CHG 0x0001
/** get */
#define V2CPE_EDSP1_INTF2_DEC_CHG_GET(reg) (((reg) & V2CPE_EDSP1_INTF2_DEC_CHG) >> 0)
/** set */
#define V2CPE_EDSP1_INTF2_DEC_CHG_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_EDSP1_INTF2_DEC_CHG) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_EDSP1_INTF2_DEC_CHG */



/*@}*/ /*  V2CPE_EDSP1_INTF2 */


/*@}*/


/**
 * \defgroup MODULE Analog_Line_Module
 */
/*@{*/



/**
 * \defgroup V2CPE_LINE1_STAT V2CPE_LINE1_STAT
 * ALM 1 Status Register. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_LINE1_STAT 0x60
/** reset value */
#define V2CPE_LINE1_STAT_RESET 0x0000


/**
 * \defgroup V2CPE_LINE1_STAT_ONHOOK V2CPE_LINE1_STAT_ONHOOK
 * On-Hook. Indicates on-hook for the loop in all operating modes (via the ITx pin); filtered by the DUP (Data Upstream Persistence) counter. Indicates ground start in case of ground start mode is selected.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_STAT_ONHOOK 0x0008
/** get */
#define V2CPE_LINE1_STAT_ONHOOK_GET(reg) (((reg) & V2CPE_LINE1_STAT_ONHOOK) >> 3)
/** set */
#define V2CPE_LINE1_STAT_ONHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_STAT_ONHOOK) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_LINE1_STAT_ONHOOK */




/**
 * \defgroup V2CPE_LINE1_STAT_OFFHOOK V2CPE_LINE1_STAT_OFFHOOK
 * Off-Hook. Indicates off-hook for the loop in all operating modes (via the ITx pin); filtered by the DUP (Data Upstream Persistence) counter. Indicates ground start in case of ground start mode is selected.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_STAT_OFFHOOK 0x0004
/** get */
#define V2CPE_LINE1_STAT_OFFHOOK_GET(reg) (((reg) & V2CPE_LINE1_STAT_OFFHOOK) >> 2)
/** set */
#define V2CPE_LINE1_STAT_OFFHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_STAT_OFFHOOK) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_LINE1_STAT_OFFHOOK */




/**
 * \defgroup V2CPE_LINE1_STAT_OTEMP V2CPE_LINE1_STAT_OTEMP
 * Overtemperature. Thermal overload warning from the SLIC line drivers.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_STAT_OTEMP 0x0002
/** get */
#define V2CPE_LINE1_STAT_OTEMP_GET(reg) (((reg) & V2CPE_LINE1_STAT_OTEMP) >> 1)
/** set */
#define V2CPE_LINE1_STAT_OTEMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_STAT_OTEMP) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_LINE1_STAT_OTEMP */




/**
 * \defgroup V2CPE_LINE1_STAT_LTEST_FIN V2CPE_LINE1_STAT_LTEST_FIN
 * Line Testing Finished.. Indicates, that the line testing has been completed.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_STAT_LTEST_FIN 0x0001
/** get */
#define V2CPE_LINE1_STAT_LTEST_FIN_GET(reg) (((reg) & V2CPE_LINE1_STAT_LTEST_FIN) >> 0)
/** set */
#define V2CPE_LINE1_STAT_LTEST_FIN_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_STAT_LTEST_FIN) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_LINE1_STAT_LTEST_FIN */



/*@}*/ /*  V2CPE_LINE1_STAT */



/**
 * \defgroup V2CPE_LINE1_INT V2CPE_LINE1_INT
 * ALM 1 Interrupt Register. xxx
 */
/*@{*/


/** register offset */
#define V2CPE_LINE1_INT 0x90
/** reset value */
#define V2CPE_LINE1_INT_RESET 0x0000


/**
 * \defgroup V2CPE_LINE1_INT_ONHOOK V2CPE_LINE1_INT_ONHOOK
 * Off-Hook Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INT_ONHOOK 0x0008
/** get */
#define V2CPE_LINE1_INT_ONHOOK_GET(reg) (((reg) & V2CPE_LINE1_INT_ONHOOK) >> 3)
/** set */
#define V2CPE_LINE1_INT_ONHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INT_ONHOOK) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_LINE1_INT_ONHOOK */




/**
 * \defgroup V2CPE_LINE1_INT_OFFHOOK V2CPE_LINE1_INT_OFFHOOK
 * On-Hook Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INT_OFFHOOK 0x0004
/** get */
#define V2CPE_LINE1_INT_OFFHOOK_GET(reg) (((reg) & V2CPE_LINE1_INT_OFFHOOK) >> 2)
/** set */
#define V2CPE_LINE1_INT_OFFHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INT_OFFHOOK) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_LINE1_INT_OFFHOOK */




/**
 * \defgroup V2CPE_LINE1_INT_OTEMP V2CPE_LINE1_INT_OTEMP
 * Overtemperature Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INT_OTEMP 0x0002
/** get */
#define V2CPE_LINE1_INT_OTEMP_GET(reg) (((reg) & V2CPE_LINE1_INT_OTEMP) >> 1)
/** set */
#define V2CPE_LINE1_INT_OTEMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INT_OTEMP) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_LINE1_INT_OTEMP */




/**
 * \defgroup V2CPE_LINE1_INT_LTEST_FIN V2CPE_LINE1_INT_LTEST_FIN
 * Line Testing Finished Interrupt.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INT_LTEST_FIN 0x0001
/** get */
#define V2CPE_LINE1_INT_LTEST_FIN_GET(reg) (((reg) & V2CPE_LINE1_INT_LTEST_FIN) >> 0)
/** set */
#define V2CPE_LINE1_INT_LTEST_FIN_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INT_LTEST_FIN) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_LINE1_INT_LTEST_FIN */



/*@}*/ /*  V2CPE_LINE1_INT */



/**
 * \defgroup V2CPE_LINE1_INTR V2CPE_LINE1_INTR
 * ALM 1 Interrupt Enable Register (rising-edge). xxx
 */
/*@{*/


/** register offset */
#define V2CPE_LINE1_INTR 0xC0
/** reset value */
#define V2CPE_LINE1_INTR_RESET 0x0000


/**
 * \defgroup V2CPE_LINE1_INTR_ONHOOK V2CPE_LINE1_INTR_ONHOOK
 * On-HOOK Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTR_ONHOOK 0x0008
/** get */
#define V2CPE_LINE1_INTR_ONHOOK_GET(reg) (((reg) & V2CPE_LINE1_INTR_ONHOOK) >> 3)
/** set */
#define V2CPE_LINE1_INTR_ONHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTR_ONHOOK) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_LINE1_INTR_ONHOOK */




/**
 * \defgroup V2CPE_LINE1_INTR_OFFHOOK V2CPE_LINE1_INTR_OFFHOOK
 * Off-HOOK Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTR_OFFHOOK 0x0004
/** get */
#define V2CPE_LINE1_INTR_OFFHOOK_GET(reg) (((reg) & V2CPE_LINE1_INTR_OFFHOOK) >> 2)
/** set */
#define V2CPE_LINE1_INTR_OFFHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTR_OFFHOOK) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_LINE1_INTR_OFFHOOK */




/**
 * \defgroup V2CPE_LINE1_INTR_OTEMP V2CPE_LINE1_INTR_OTEMP
 * Overtemperature Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTR_OTEMP 0x0002
/** get */
#define V2CPE_LINE1_INTR_OTEMP_GET(reg) (((reg) & V2CPE_LINE1_INTR_OTEMP) >> 1)
/** set */
#define V2CPE_LINE1_INTR_OTEMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTR_OTEMP) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_LINE1_INTR_OTEMP */




/**
 * \defgroup V2CPE_LINE1_INTR_LTEST_FIN V2CPE_LINE1_INTR_LTEST_FIN
 * Line Testing Finished Interrrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTR_LTEST_FIN 0x0001
/** get */
#define V2CPE_LINE1_INTR_LTEST_FIN_GET(reg) (((reg) & V2CPE_LINE1_INTR_LTEST_FIN) >> 0)
/** set */
#define V2CPE_LINE1_INTR_LTEST_FIN_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTR_LTEST_FIN) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_LINE1_INTR_LTEST_FIN */



/*@}*/ /*  V2CPE_LINE1_INTR */



/**
 * \defgroup V2CPE_LINE1_INTF V2CPE_LINE1_INTF
 * ALM1 Interrupt Enable Register (falling edge). This register has currently no sensful meaning in the system and will therefor be removed from the interface.
 */
/*@{*/


/** register offset */
#define V2CPE_LINE1_INTF 0xF0
/** reset value */
#define V2CPE_LINE1_INTF_RESET 0x0000


/**
 * \defgroup V2CPE_LINE1_INTF_ONHOOK V2CPE_LINE1_INTF_ONHOOK
 * On-HOOK Interrup Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTF_ONHOOK 0x0008
/** get */
#define V2CPE_LINE1_INTF_ONHOOK_GET(reg) (((reg) & V2CPE_LINE1_INTF_ONHOOK) >> 3)
/** set */
#define V2CPE_LINE1_INTF_ONHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTF_ONHOOK) | (((val) & 1) << 3))

/*@}*/ /* V2CPE_LINE1_INTF_ONHOOK */




/**
 * \defgroup V2CPE_LINE1_INTF_OFFHOOK V2CPE_LINE1_INTF_OFFHOOK
 * Off-HOOK Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTF_OFFHOOK 0x0004
/** get */
#define V2CPE_LINE1_INTF_OFFHOOK_GET(reg) (((reg) & V2CPE_LINE1_INTF_OFFHOOK) >> 2)
/** set */
#define V2CPE_LINE1_INTF_OFFHOOK_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTF_OFFHOOK) | (((val) & 1) << 2))

/*@}*/ /* V2CPE_LINE1_INTF_OFFHOOK */




/**
 * \defgroup V2CPE_LINE1_INTF_OTEMP V2CPE_LINE1_INTF_OTEMP
 * Overtemperature Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTF_OTEMP 0x0002
/** get */
#define V2CPE_LINE1_INTF_OTEMP_GET(reg) (((reg) & V2CPE_LINE1_INTF_OTEMP) >> 1)
/** set */
#define V2CPE_LINE1_INTF_OTEMP_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTF_OTEMP) | (((val) & 1) << 1))

/*@}*/ /* V2CPE_LINE1_INTF_OTEMP */




/**
 * \defgroup V2CPE_LINE1_INTF_LTEST V2CPE_LINE1_INTF_LTEST
 * Line Testing Finished Interrupt Enable.
 */
/*@{*/

/** mask */
#define V2CPE_LINE1_INTF_LTEST 0x0001
/** get */
#define V2CPE_LINE1_INTF_LTEST_GET(reg) (((reg) & V2CPE_LINE1_INTF_LTEST) >> 0)
/** set */
#define V2CPE_LINE1_INTF_LTEST_SET(reg, val) (reg) = (((reg) & (unsigned short) ~V2CPE_LINE1_INTF_LTEST) | (((val) & 1) << 0))

/*@}*/ /* V2CPE_LINE1_INTF_LTEST */



/*@}*/ /*  V2CPE_LINE1_INTF */


/*@}*/

#endif /* _DRV_VINETIC_HOST_H */

