#ifndef _DRV_VINETIC_HOSTAPI_H
#define _DRV_VINETIC_HOSTAPI_H
/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************
   Module      : drv_vinetic_hostapi.h
   Description : This file contains the defines, the structures declarations
                 and the global functions declarations for the VINETIC 2CPE
                 host.
*******************************************************************************/

/* ================================ */
/* Includes                         */
/* ================================ */
#include "drv_vinetic_access.h"
#include "drv_vinetic_dcctl.h"
#ifdef EVENT_LOGGER_DEBUG
#include "event_logger.h"
#endif /* EVENT_LOGGER_DEBUG */

/* ================================ */
/* Defines                          */
/* ================================ */

/* channel defines */
/** VINETIC maximum channel number */
#define VINETIC_MAX_CH_NR              VINETIC_2CPE_CH_NR
/** VINETIC maximum analog channel number */
#define VINETIC_MAX_ANA_CH_NR          VINETIC_2CPE_ANA_CH_NR
/** Number of UTD submodules per signaling module */
#define VIN_NUM_UTD                    1
/** VINETIC 2CPE maximum EDSP channels */
#define VINETIC_2CPE_MAX_EDSP          4
/** Maximum Number of Coder */
#define VIN_MAX_CODER                  4
/** Maximum Number of Signalling */
#define VIN_MAX_SIG                    4
#define VIN_MAX_SIG_ROM                3 /* 3 signalling channel in ROM */
/** Maximum Number of PCM */
// JGUI: This should be 4 not 8, otherwise insmod will crash
#define VIN_MAX_PCM                    4 //8

/** default ring frequency */
#define VIN_2CPE_DEFAULT_RING_FREQ                     25 /* Hz */
/** default ring amplitude */
#define VIN_2CPE_DEFAULT_RING_AMP                      0x7FFF
/** default ring hook level */
#define VIN_2CPE_DEFAULT_HOOK_LEVEL                    0x4AA0
/** default ring trip dup time */
#define VIN_2CPE_DEFAULT_RING_TRIP_DUP_TIME            0x5 /* 10 ms */
/** default ring dc offset */
#define VIN_2CPE_DEFAULT_RING_DC_OFFSET                0x0000 /* 0 Vdc */
/** default hook settling time */
#define VIN_2CPE_DEFAULT_HOOK_SETTLING_TIME            64  /* ms */
/** default hook dup time */
#define VIN_2CPE_DEFAULT_HOOK_DUP_TIME                 12  /* ms */
/** default overtemperature dup time */
#define VIN_2CPE_DEFAULT_OVT_DUP_TIME                  0x08 /* 8 ms */

/* Mask for CIS_BUF and CIS_REQ */
#define VIN_CIS_BUF_REQ_MASK           (V2CPE_EDSP1_INT1_CIS_BUF | \
                                        V2CPE_EDSP1_INT1_CIS_REQ)
/* Mask for CIS_ACT */
#define VIN_CIS_ACT_MASK                V2CPE_EDSP1_INT1_CIS_ACT
/* Mask for UTG2_ACT */
#define VIN_UTG2_ACT_MASK               V2CPE_EDSP1_INT1_CIS_ACT
/* Mask for UTG1_ACT */
#define VIN_UTG1_ACT_MASK               V2CPE_EDSP1_INT1_DTMFG_ACT
/* Mask for DTMFG_BUF, DTMFG_REQ and DTMFG_ACT */
#define VIN_DTMFG_BUF_REQ_ACT_MASK     (V2CPE_EDSP1_INT1_DTMFG_BUF | \
                                        V2CPE_EDSP1_INT1_DTMFG_REQ | \
                                        V2CPE_EDSP1_INT1_DTMFG_ACT)
/* Mask for DTMFG_ACT */
#define VIN_DTMFG_ACK_MASK             V2CPE_EDSP1_INT1_DTMFG_ACT
/* Mask for UTD1_OK / UTD2_OK */
#define VIN_UTD1_OK_MASK               V2CPE_EDSP1_INT1_UTD1_OK
#define VIN_UTD2_OK_MASK               V2CPE_EDSP1_INT1_UTD2_OK
/* Mask for CPT */
#define VIN_CPT_MASK                   V2CPE_EDSP1_INT1_CPT
/* Mask for ATD1_X / ATD2_X */
#define VIN_ATD1_DT_MASK               V2CPE_EDSP1_INT2_ATD1_DT
#define VIN_ATD2_DT_MASK               V2CPE_EDSP1_INT2_ATD2_DT
#define VIN_ATD1_AM_MASK               V2CPE_EDSP1_INT2_ATD1_AM
#define VIN_ATD2_AM_MASK               V2CPE_EDSP1_INT2_ATD2_AM
#define VIN_ATD1_NPR_MASK              V2CPE_EDSP1_INT2_ATD1_NPR_MASK
#define VIN_ATD2_NPR_MASK              V2CPE_EDSP1_INT2_ATD2_NPR_MASK
#define VIN_ATD1_NPR_1_REV_MASK        0x2000 /** \todo verify this */
#define VIN_ATD2_NPR_1_REV_MASK        0x0200 /** \todo verify this */
#define VIN_MFTD1_MASK                 V2CPE_EDSP1_INT2_MFTD1
#define VIN_MFTD2_MASK                 V2CPE_EDSP1_INT2_MFTD2

#ifdef VIN_SPI
/* \todo these defines are currently not in drv_vinetic_host.h ...
   little work around !*/
#ifndef V2CPE_SPI_CMD_R
#define V2CPE_SPI_CMD_R                0x8000
#endif /* V2CPE_SPI_CMD_R */

#ifndef V2CPE_SPI_CMD_W
#define V2CPE_SPI_CMD_W                0x4000
#endif /* V2CPE_SPI_CMD_W */

#ifndef V2CPE_SPI_CMD_I
#define V2CPE_SPI_CMD_I                0x0001
#endif /* V2CPE_SPI_CMD_I */

/* SPI command device shift */
#define SPI_CMD_DEV_SHIFT              9

/* SPI address mask */
#define V2CPE_SPI_DEVADDR_MASK         0x1F
#endif /* VIN_SPI */

#ifndef V2CPE_DUPO_REG15
#define V2CPE_DUPO_REG15 0x3E
#endif /* V2CPE_DUPO_REG15 */

/* all xCPE have max 1 PCM I/O */
#define PCM_HIGHWAY                    1

#ifndef EVALUATION
/* event reporting/counting macros, can be modified/adapted if needed.
   It is currently used only for evaluation purposes */
#define VIN_REPORT_EVENT(handle,id,evt,stat)
/* 2CPE interrupts device events ids */
#define V2CPE_DEV_ERR_INT     0
#define V2CPE_DEV_IO_INT      0
#define V2CPE_DEV_STAT_INT    0
/* 2CPE interrupts channel events ids */
#define V2CPE_CH_LINE_INT     0
#define V2CPE_CH_EDSP_INT1    0
#define V2CPE_CH_EDSP_INT2    0
#endif /* EVALUATION */

#ifdef EVENT_LOGGER_DEBUG
/* Event Logger (debugging) macros, defining more
   meaningful names to Event Logger macros */
/* register read logging macro */
#define LOG_RD_REG(reg_offset, reg_data, count) \
                                       LOG_EVENT_T3(reg_offset, reg_data, count)
/* single register write logging macro */
#define LOG_WR_REG(reg_offset, val) \
   do {\
      IFX_uint16_t tmp;\
      tmp = (val);\
      LOG_EVENT_T4(reg_offset, &tmp, 1);\
   } while(0)

/* multiple register write logging macro */
#define LOG_WR_REG_MULTI(reg_offset, reg_data, count) \
                                       LOG_EVENT_T4(reg_offset, reg_data, count)
/* command read logging macro */
#define LOG_RD_CMD(pcmd, pdata, count, err) \
                                       LOG_EVENT_T5(pcmd, pdata, count, err)
/* command write logging macro */
#define LOG_WR_CMD(pdata, count, err) \
                                       LOG_EVENT_T6(pdata, count, err)
/* voice inbox read logging macro */
#define LOG_RD_PKT(pdata, count, err) \
                                       LOG_EVENT_T7(pdata, count, err)
/* voice outbox write logging macro */
#define LOG_WR_PKT(pdata, count, err) \
                                       LOG_EVENT_T8(pdata, count, err)
#else /* EVENT_LOGGER_DEBUG */
#define LOG_RD_REG(reg_offset, reg_data, count)
#define LOG_WR_REG(reg_offset, val)
#define LOG_WR_REG_MULTI(reg_offset, reg_data, count)
#define LOG_RD_CMD(pcmd, pdata, count, err)
#define LOG_WR_CMD(pdata, count, err)
#define LOG_RD_PKT(pdata, count, err)
#define LOG_WR_PKT(pdata, count, err)
#endif /* EVENT_LOGGER_DEBUG */
#if ((VIN_ACCESS_MODE == VIN_ACCESS_MODE_SPI) ||                     \
     (VIN_ACCESS_MODE == VIN_ACCESS_MODE_EVALUATION))
#define CHECK_HOST_ERR(pDev, exec_on_err)                            \
   if (pDev->err  == ERR_HOSTREG_ACCESS)                             \
   {                                                                 \
      LOG(VINETIC,DBG_LEVEL_HIGH,                                    \
            ("[%s, %d], VIN%ld: error on host register access\n\r",  \
            __FILE__, __LINE__, pDev->nDevNr));                      \
                                                                     \
      exec_on_err;                                                   \
   }
#else
#define CHECK_HOST_ERR(pDev, exec_on_err)
#endif

/* ============================= */
/* Global Structures             */
/* ============================= */

typedef struct _VINETIC_2CPE_HOST_CHANNEL VINETIC_HOST_CHANNEL;
typedef struct _VINETIC_2CPE_HOST_DEVICE  VINETIC_HOST_DEVICE;

typedef enum
{
   VINETIC_RING_TRIP_TYPE_NORMAL = 0,
   VINETIC_RING_TRIP_TYPE_FAST
} VINETIC_RING_TRIP_TYPE_t;

/* Ring Configuration */
typedef struct
{
   /* ring frequency */
   IFX_uint16_t ring_freq;
   /* ring amplitude */
   IFX_uint16_t ring_amp;
   /* ring hook level */
   IFX_uint16_t ring_hook_level;
   /* ring trip type */
   VINETIC_RING_TRIP_TYPE_t ring_trip_type;
   /* ring trip dup time*/
   IFX_uint16_t ring_trip_dup_time;
   /* ring dc offset */
   IFX_uint16_t ring_dco;
} VINETIC_RingCfg_t;

/* DC Thresholds */
typedef struct
{
   /* hook dup time */
   IFX_uint16_t hook_dup_time;
   /* onhook settling time */
   IFX_uint16_t onhook_time;
   /* overtempure duptime */
   IFX_uint16_t ovt_dup_time;
} VINETIC_DcThr_t;

/* Host specific channel structure.
   Structure element is part of the host global structure */
struct _VINETIC_2CPE_HOST_CHANNEL
{
   /* EdspX Stat2 register */
   IFX_uint16_t      regEdspX_Stat2;
   /* LineX rising edge interrupt mask */
   IFX_uint16_t      regLineX_IntR;
   /* LineX falling edge interrupt mask */
   IFX_uint16_t      regLineX_IntF;
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
   /* IFX_TRUE if ring config stored for this channel */
   IFX_boolean_t                   b_ring_cfg;
   /* Previous ring frequency */
   IFX_uint16_t                    ring_freq_prev;
   /* Ring configuration */
   CMD_Ring_Config_t               ring_cfg;
   /* IFX_TRUE: GR909 limits are set / IFX_FALSE: GR909 limits
      aren't set, so set defaults */
   IFX_boolean_t                   b_GR909_limits;
   /* Operating mode used for GR909 tests */
   CMD_OpMode_t                    opmod;
   /* GR909 linetesting control */
   CMD_GR909_Linetesting_Control_t gr909_ctrl;
   /* GR909 Pass/Fail Results */
   CMD_GR909_Result_Pass_Fail_t    gr909_result;
   /* GR909 HPT Results */
   CMD_GR909_Result_HPT_t          gr909_hpt;
   /* GR909 FEFM Results */
   CMD_GR909_Result_FEMF_t         gr909_femf;
   /* GR909 RFT Results */
   CMD_GR909_Result_RFT_t          gr909_rft;
   /* GR909 ROH Results */
   CMD_GR909_Result_ROH_t          gr909_roh;
   /* GR909 RIT Results */
   CMD_GR909_Result_RIT_t          gr909_rit;
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */
   /* extend here */
};

/* VINETIC Host specific device Stucture.
   Structure element is part of the host global structure */
struct _VINETIC_2CPE_HOST_DEVICE
{
   /* access mode set at basic device initialization,
      applies only in case of evaluation, as the access macros
      will be set at compile time in normal case. */
   VIN_ACCESS      nAccessMode;
   /* Rom firmware flag, set to true in case ROM firmware is used */
   IFX_boolean_t   bRomFirmware;
   /* cached value written to IO_OUT register */
   IFX_uint16_t    GpioOut;
   /* cached value of mbx handshake (DUPO_REG_8) */
   IFX_uint16_t    nMbxHandshake;
   /* cached value of STAT_IEN register */
   IFX_uint16_t    nRegStatIen;
#ifdef VIN_SPI
   /* SPI device address of Vinetic Chip. */
   IFX_uint8_t     nSPIDevAddr;
#endif /* VIN_SPI */
};


/* ============================= */
/* Global variable declaration   */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* Host specific functions, implemented in drv_vinetic_host.c */
IFX_int32_t VINETIC_Host_CheckMbxEmpty    (VINETIC_DEVICE  *pDev,
                                           IFX_int32_t      full_size);
IFX_int32_t  VINETIC_Host_CheckDldReady   (VINETIC_DEVICE  *pDev);
IFX_int32_t  VINETIC_Host_ResetEdsp       (VINETIC_DEVICE  *pDev);
IFX_int32_t  VINETIC_Host_StartEdsp       (VINETIC_DEVICE  *pDev);
IFX_int32_t  VINETIC_Host_SetSlic         (VINETIC_CHANNEL *pCh,
                                           IFX_boolean_t    bBroadcast,
                                           IFX_uint16_t     slic_val);
IFX_int32_t  VINETIC_Host_RingCfg         (VINETIC_CHANNEL *pCh,
                                           IFX_boolean_t    bBroadcast,
                                           VINETIC_RingCfg_t *p_ringCfg);
IFX_int32_t  VINETIC_Host_SetDcThr        (VINETIC_CHANNEL *pCh,
                                           IFX_boolean_t    bBroadcast,
                                           VINETIC_DcThr_t *p_dcThr);
#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)
/* GR909 ioctl functions, implemented in drv_vinetic_gr909.c */
extern IFX_int32_t  VINETIC_GR909_Start  (VINETIC_CHANNEL *pCh,
                                          VINETIC_IO_GR909_Start_t *p_start);
extern IFX_int32_t  VINETIC_GR909_Result (VINETIC_CHANNEL *pCh,
                                          VINETIC_IO_GR909_Result_t *p_results);
#endif /* VIN_CFG_FEATURES & VIN_FEAT_GR909 */

#endif /* _DRV_VINETIC_HOSTAPI_H */


