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

******************************************************************************/


#ifndef _DRV_VINETIC_DCCTL_H_
#define _DRV_VINETIC_DCCTL_H_

/** \file
   Owner of this chapter is: Gerhard Noessing 6036 IFAT DCV COM TMI
*/

#ifndef __PACKED__
   #if defined (__GNUC__) || defined (__GNUG__)
      /* GNU C or C++ compiler */
      #define __PACKED__ __attribute__ ((packed))
   #elif !defined (__PACKED__)
      #define __PACKED__      /* nothing */
   #endif
#endif


/** @defgroup _DRV_VINETIC_DCCTL_ Components
 *  @{
 */

#define VIN_CMD_RD                     0x1
#define VIN_CMD_WR                     0x0

#define VIN_CMD_ALM                    0x1
#define VIN_MOD_DCCTL                  0x0
#define VIN_ECMD_OPMOD                 0x0
#define VIN_ECMD_BASIC_CONFIG          0x1
#define VIN_ECMD_RING_CONFIG           0x2
#define VIN_ECMD_GR909_LT_CTRL         0x3


/* Opmode Control */
typedef enum VIN_OPMOD_
{
   VIN_OPMOD_PDH                     = 0x0,
   VIN_OPMOD_ONHOOK                  = 0x1,
   VIN_OPMOD_ACT                     = 0x2,
   VIN_OPMOD_RING                    = 0x3,
   VIN_OPMOD_GR909                   = 0x4,
   VIN_OPMOD_FXO                     = 0x5
} VIN_OPMOD;

typedef enum VIN_OPMOD_RP_
{
   VIN_OPMOD_POL_NORMAL              = 0x0,
   VIN_OPMOD_POL_REVERSE             = 0x1
} VIN_OPMOD_RP;

typedef enum VIN_OPMOD_HOWLER_
{
   VIN_OPMOD_HOWLER_OFF              = 0x0,
   VIN_OPMOD_HOWLER_ON               = 0x1
} VIN_OPMOD_HOWLER;


/* Basic Configuration */
#define VIN_BASIC_CONFIG_SLIC_TYPE_E   0x0
#define VIN_BASIC_CONFIG_SLIC_TYPE_DC  0x1

#ifdef __cplusplus
   extern "C" {
#endif

/* ----- Include section ----- */
/* ----- Include section (End) ----- */

/* ----- Define section ----- */
/* ----- Define section (End) ----- */

/** Message ID for CMD_OpMode */
#define CMD_OPMODE 0x01
#define CMD_OPMODE_LEN 0x01

/**
   This message allows the selection of the desired operating mode. Before selecting
   any other operating mode than PDH the device must be configured with configuration
   commands CMD_Basic_Config, CMD_Ring_Config and CMD_GR909_Linetesting_Control. The
   CMD_OpMode allows also to select normal or reverse polarity and howler tone support.
   Reverse polarity is only available in ACT and RING mode. Hower tone support is
   available in ACT OFFHOOK state only. In other modes these
*/
typedef struct CMD_OpMode CMD_OpMode_t;

/** Message ID for CMD_Basic_Config */
#define CMD_BASIC_CONFIG 0x02

/**
   This message configures the basic paramenters for the VINETIC-2VIP. It allows to
   select the connected SLIC type and the persistance times for active off-hook and
   on-hook transients. The HOOK-SET parameter allows to suppress the indication of
   wrong off-hooks when switching from any other mode to ONHOOK.
*/
typedef struct CMD_Basic_Config CMD_Basic_Config_t;

/** Message ID for CMD_Ring_Config */
#define CMD_RING_CONFIG 0x01

/**
   This message allows the configuration of all paramenters used for the RINGING
   operating mode. Beside ringing amplitude, frequency and ring-trip threshold the
   parameters for the ring cadence and the caller id signaling can be configured.
*/
typedef struct CMD_Ring_Config CMD_Ring_Config_t;

/** Message ID for CMD_GR909_Linetesting_Control */
#define CMD_GR909_LINETESTING_CONTROL 0x03
#define CMD_GR909_LINETESTING_CONTROL_LEN 0x0D

/**
   This command configures the GR909 Linetesting Module. It allows to set the pass/fail
   limits and which tests should run. Additional the region (EU or USA) for different
   power line frequencies can be selected.
*/
typedef struct CMD_GR909_Linetesting_Control CMD_GR909_Linetesting_Control_t;

/** Message ID for CMD_GR909_Result_Pass_Fail */
#define CMD_GR909_RESULT_PASS_FAIL 0x04
#define CMD_GR909_RESULT_PASS_FAIL_LEN 0x01

/**
   This message reports the Pass/Fail Results of the last invocation of the GR909
   Linetesting Module. If the linetesting has finished, an interrupt gr909_finished
   indicates the availabbility of results. The GR909_Result_pass_Fail command gives the
   information which tests had been performed and which of them passed. Detailed
   results are available for the valid tests only.
*/
typedef struct CMD_GR909_Result_Pass_Fail CMD_GR909_Result_Pass_Fail_t;

/** Message ID for CMD_GR909_Result_HPT */
#define CMD_GR909_RESULT_HPT 0x05
#define CMD_GR909_RESULT_HPT_LEN 0x06

/**
   This message reports the HPT Results of the last invocation of the GR909 Linetesting
   Module. The results are only valid if the bit HPT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
typedef struct CMD_GR909_Result_HPT CMD_GR909_Result_HPT_t;

/** Message ID for CMD_GR909_Result_FEMF */
#define CMD_GR909_RESULT_FEMF 0x06
#define CMD_GR909_RESULT_FEMF_LEN 0x06

/**
   This message reports the FEMF Results of the last invocation of the GR909
   Linetesting Module. The results are only valid if the bit FEMF_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
typedef struct CMD_GR909_Result_FEMF CMD_GR909_Result_FEMF_t;

/** Message ID for CMD_GR909_Result_RFT */
#define CMD_GR909_RESULT_RFT 0x07
#define CMD_GR909_RESULT_RFT_LEN 0x03

/**
   This message reports the resistive faults test results of the last invocation of the
   GR909 Linetesting Module. The results are only valid if the bit RFT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
typedef struct CMD_GR909_Result_RFT CMD_GR909_Result_RFT_t;

/** Message ID for CMD_GR909_Result_ROH */
#define CMD_GR909_RESULT_ROH 0x08
#define CMD_GR909_RESULT_ROH_LEN 0x02

/**
   This message reports the receiver off hook test results of the last invocation of
   the GR909 Linetesting Module. The results are only valid if the bit ROH_VALID in
   message CMD_GR909_Result_Pass_Fail shows the value valid.
*/
typedef struct CMD_GR909_Result_ROH CMD_GR909_Result_ROH_t;

/** Message ID for CMD_GR909_Result_RIT */
#define CMD_GR909_RESULT_RIT 0x09
#define CMD_GR909_RESULT_RIT_LEN 0x01

/**
   This message reports the ringer impedance test results of the last invocation of the
   GR909 Linetesting Module. The results are only valid if the bit RIT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
typedef struct CMD_GR909_Result_RIT CMD_GR909_Result_RIT_t;

/** Message ID for CMD_DCCTL_Debug */
#define CMD_DCCTL_DEBUG 0x0a

/**
   This message allows the manual setting of the DCCTL output registers (see ). In
   normal operating mode (bit ... not set) the message allows a monitoring of the
   current register setting. With bit ... set the contents of the message is used to
   control the behaviour of the AFE, the Carmel and the ASDSP.
*/
typedef struct CMD_DCCTL_Debug CMD_DCCTL_Debug_t;

/**
   This message allows the selection of the desired operating mode. Before selecting
   any other operating mode than PDH the device must be configured with configuration
   commands CMD_Basic_Config, CMD_Ring_Config and CMD_GR909_Linetesting_Control. The
   CMD_OpMode allows also to select normal or reverse polarity and howler tone support.
   Reverse polarity is only available in ACT and RING mode. Hower tone support is
   available in ACT OFFHOOK state only. In other modes these
*/
struct CMD_OpMode
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** Reverse Polarity in ACTIVE mode */
   IFX_uint32_t REV_POL : 1;
   /** HOWLER tone sending possible */
   IFX_uint32_t HOWLER : 1;
   /** Reserved */
   IFX_uint32_t Res01 : 10;
   /** Operating Mode */
   IFX_uint32_t OP_MODE : 4;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res02 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** Operating Mode */
   IFX_uint32_t OP_MODE : 4;
   /** Reserved */
   IFX_uint32_t Res03 : 10;
   /** HOWLER tone sending possible */
   IFX_uint32_t HOWLER : 1;
   /** Reverse Polarity in ACTIVE mode */
   IFX_uint32_t REV_POL : 1;
#endif
} __PACKED__ ;


/**
   This message configures the basic paramenters for the VINETIC-2VIP. It allows to
   select the connected SLIC type and the persistance times for active off-hook and
   on-hook transients. The HOOK-SET parameter allows to suppress the indication of
   wrong off-hooks when switching from any other mode to ONHOOK.
*/
struct CMD_Basic_Config
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** DUP time value for Hook debouncing */
   IFX_uint32_t HOOK_DUP : 4;
   /** SLIC type selection */
   IFX_uint32_t SLIC_SEL : 4;
   /** Settling time for ONHOOK mode */
   IFX_uint32_t HOOK_SET : 4;
   /** DUP time value for Overtemp debouncing */
   IFX_uint32_t OVT_DUP : 4;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** DUP time value for Overtemp debouncing */
   IFX_uint32_t OVT_DUP : 4;
   /** Settling time for ONHOOK mode */
   IFX_uint32_t HOOK_SET : 4;
   /** SLIC type selection */
   IFX_uint32_t SLIC_SEL : 4;
   /** DUP time value for Hook debouncing */
   IFX_uint32_t HOOK_DUP : 4;
#endif
} __PACKED__ ;


/**
   This message allows the configuration of all paramenters used for the RINGING
   operating mode. Beside ringing amplitude, frequency and ring-trip threshold the
   parameters for the ring cadence and the caller id signaling can be configured.
*/
struct CMD_Ring_Config
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** Ringing Frequency */
   IFX_uint32_t RING_F : 16;
   /** Ringing Amplitude */
   IFX_uint32_t RING_A : 16;
   /** Ringing Hook Level */
   IFX_uint32_t RING_HL : 16;
   /** Debounce time for Ring Trip */
   IFX_uint32_t TRIP_DUP : 4;
   /** Fast Ring Trip Enable */
   IFX_uint32_t FAST_RT_EN : 1;
   /** Caller ID sending mode */
   IFX_uint32_t CID_MODE : 3;
   /** Length of the Ring-Pulse-Alert signal before CID */
   IFX_uint32_t RPA_LENGTH : 8;
   /** Delay of CID Start signal. */
   IFX_uint32_t CID_START_DELAY : 8;
   /** Dealy after CID end indication */
   IFX_uint32_t CID_END_DELAY : 8;
   /** Ringing Burst 1 length */
   IFX_uint32_t BURST1 : 8;
   /** Ringing Pause 1 length */
   IFX_uint32_t PAUSE1 : 8;
   /** Ringing Burst 2 length */
   IFX_uint32_t BURST2 : 8;
   /** Ringing Pause 2 length */
   IFX_uint32_t PAUSE2 : 8;
   /** Ringing Burst 3 length */
   IFX_uint32_t BURST3 : 8;
   /** Ringing Pause 3 length */
   IFX_uint32_t PAUSE3 : 8;
   /** Ringing Burst 4 length */
   IFX_uint32_t BURST4 : 8;
   /** Ringing Pause 4 length */
   IFX_uint32_t PAUSE4 : 8;
   /** Ring DC Offset */
   IFX_uint32_t RING_DCO : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res02 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** DCCTL Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** Ringing Frequency */
   IFX_uint32_t RING_F : 16;
   /** Ringing Amplitude */
   IFX_uint32_t RING_A : 16;
   /** Ringing Hook Level */
   IFX_uint32_t RING_HL : 16;
   /** Length of the Ring-Pulse-Alert signal before CID */
   IFX_uint32_t RPA_LENGTH : 8;
   /** Caller ID sending mode */
   IFX_uint32_t CID_MODE : 3;
   /** Fast Ring Trip Enable */
   IFX_uint32_t FAST_RT_EN : 1;
   /** Debounce time for Ring Trip */
   IFX_uint32_t TRIP_DUP : 4;
   /** Dealy after CID end indication */
   IFX_uint32_t CID_END_DELAY : 8;
   /** Delay of CID Start signal. */
   IFX_uint32_t CID_START_DELAY : 8;
   /** Ringing Pause 1 length */
   IFX_uint32_t PAUSE1 : 8;
   /** Ringing Burst 1 length */
   IFX_uint32_t BURST1 : 8;
   /** Ringing Pause 2 length */
   IFX_uint32_t PAUSE2 : 8;
   /** Ringing Burst 2 length */
   IFX_uint32_t BURST2 : 8;
   /** Ringing Pause 3 length */
   IFX_uint32_t PAUSE3 : 8;
   /** Ringing Burst 3 length */
   IFX_uint32_t BURST3 : 8;
   /** Ringing Pause 4 length */
   IFX_uint32_t PAUSE4 : 8;
   /** Ringing Burst 4 length */
   IFX_uint32_t BURST4 : 8;
   /** Ring DC Offset */
   IFX_uint32_t RING_DCO : 16;
#endif
} __PACKED__ ;


/**
   This command configures the GR909 Linetesting Module. It allows to set the pass/fail
   limits and which tests should run. Additional the region (EU or USA) for different
   power line frequencies can be selected.
*/
struct CMD_GR909_Linetesting_Control
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** Reserved */
   IFX_uint32_t Res01 : 7;
   /** Country selection */
   IFX_uint32_t COUNTRY : 1;
   /** Reserved */
   IFX_uint32_t Res02 : 3;
   /** Hazardous Potential Test */
   IFX_uint32_t HPT : 1;
   /** Foreign ElectroMotive Forces Test */
   IFX_uint32_t FEMF : 1;
   /** Resitive Faults Test */
   IFX_uint32_t RFT : 1;
   /** Receiver Off-Hook Test */
   IFX_uint32_t ROH : 1;
   /** Ringer Impedance Test */
   IFX_uint32_t RIT : 1;
   /** HPT wire to GND AC limit */
   IFX_uint32_t HPT_W2G_AC_LIM : 16;
   /** HPT wire to wire AC limit */
   IFX_uint32_t HPT_W2W_AC_LIM : 16;
   /** HPT wire to GND DC limit */
   IFX_uint32_t HPT_W2G_DC_LIM : 16;
   /** HPT wire to wire DC limit */
   IFX_uint32_t HPT_W2W_DC_LIM : 16;
   /** FEMF wire to GND AC limit */
   IFX_uint32_t FEMF_W2G_AC_LIM : 16;
   /** FEMF wire to wire AC limit */
   IFX_uint32_t FEMF_W2W_AC_LIM : 16;
   /** FEMF wire to GND DC limit */
   IFX_uint32_t FEMF_W2G_DC_LIM : 16;
   /** FEMF wire to wire DC limit */
   IFX_uint32_t FEMF_W2W_DC_LIM : 16;
   /** RFT resistance limit */
   IFX_uint32_t RFT_RES_LIM : 16;
   /** ROH linearity limit */
   IFX_uint32_t ROH_LIN_LIM : 16;
   /** RIT lower limit */
   IFX_uint32_t RIT_LOW_LIM : 16;
   /** RIT higher limit */
   IFX_uint32_t RIT_HIGH_LIM : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res03 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** Ringer Impedance Test */
   IFX_uint32_t RIT : 1;
   /** Receiver Off-Hook Test */
   IFX_uint32_t ROH : 1;
   /** Resitive Faults Test */
   IFX_uint32_t RFT : 1;
   /** Foreign ElectroMotive Forces Test */
   IFX_uint32_t FEMF : 1;
   /** Hazardous Potential Test */
   IFX_uint32_t HPT : 1;
   /** Reserved */
   IFX_uint32_t Res04 : 3;
   /** Country selection */
   IFX_uint32_t COUNTRY : 1;
   /** Reserved */
   IFX_uint32_t Res05 : 7;
   /** HPT wire to GND AC limit */
   IFX_uint32_t HPT_W2G_AC_LIM : 16;
   /** HPT wire to wire AC limit */
   IFX_uint32_t HPT_W2W_AC_LIM : 16;
   /** HPT wire to GND DC limit */
   IFX_uint32_t HPT_W2G_DC_LIM : 16;
   /** HPT wire to wire DC limit */
   IFX_uint32_t HPT_W2W_DC_LIM : 16;
   /** FEMF wire to GND AC limit */
   IFX_uint32_t FEMF_W2G_AC_LIM : 16;
   /** FEMF wire to wire AC limit */
   IFX_uint32_t FEMF_W2W_AC_LIM : 16;
   /** FEMF wire to GND DC limit */
   IFX_uint32_t FEMF_W2G_DC_LIM : 16;
   /** FEMF wire to wire DC limit */
   IFX_uint32_t FEMF_W2W_DC_LIM : 16;
   /** RFT resistance limit */
   IFX_uint32_t RFT_RES_LIM : 16;
   /** ROH linearity limit */
   IFX_uint32_t ROH_LIN_LIM : 16;
   /** RIT lower limit */
   IFX_uint32_t RIT_LOW_LIM : 16;
   /** RIT higher limit */
   IFX_uint32_t RIT_HIGH_LIM : 16;
#endif
} __PACKED__ ;


/**
   This message reports the Pass/Fail Results of the last invocation of the GR909
   Linetesting Module. If the linetesting has finished, an interrupt gr909_finished
   indicates the availabbility of results. The GR909_Result_pass_Fail command gives the
   information which tests had been performed and which of them passed. Detailed
   results are available for the valid tests only.
*/
struct CMD_GR909_Result_Pass_Fail
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** Reserved */
   IFX_uint32_t Res01 : 3;
   /** Hazardous Potential Test valid */
   IFX_uint32_t HPT_VALID : 1;
   /** Foreign ElectroMotive Forces Test valid */
   IFX_uint32_t FEMF_VALID : 1;
   /** Resitive Faults Test valid */
   IFX_uint32_t RFT_VALID : 1;
   /** Receiver Off-Hook Test valid */
   IFX_uint32_t ROH_VALID : 1;
   /** Ringer Impedance Test valid */
   IFX_uint32_t RIT_VALID : 1;
   /** Reserved */
   IFX_uint32_t Res02 : 3;
   /** Hazardous Potential Test pass */
   IFX_uint32_t HPT_PASS : 1;
   /** Foreign ElectroMotive Forces Test pass */
   IFX_uint32_t FEMF_PASS : 1;
   /** Resitive Faults Test pass */
   IFX_uint32_t RFT_PASS : 1;
   /** Receiver Off-Hook Test pass */
   IFX_uint32_t ROH_PASS : 1;
   /** Ringer Impedance Test pass */
   IFX_uint32_t RIT_PASS : 1;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res03 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** Ringer Impedance Test pass */
   IFX_uint32_t RIT_PASS : 1;
   /** Receiver Off-Hook Test pass */
   IFX_uint32_t ROH_PASS : 1;
   /** Resitive Faults Test pass */
   IFX_uint32_t RFT_PASS : 1;
   /** Foreign ElectroMotive Forces Test pass */
   IFX_uint32_t FEMF_PASS : 1;
   /** Hazardous Potential Test pass */
   IFX_uint32_t HPT_PASS : 1;
   /** Reserved */
   IFX_uint32_t Res04 : 3;
   /** Ringer Impedance Test valid */
   IFX_uint32_t RIT_VALID : 1;
   /** Receiver Off-Hook Test valid */
   IFX_uint32_t ROH_VALID : 1;
   /** Resitive Faults Test valid */
   IFX_uint32_t RFT_VALID : 1;
   /** Foreign ElectroMotive Forces Test valid */
   IFX_uint32_t FEMF_VALID : 1;
   /** Hazardous Potential Test valid */
   IFX_uint32_t HPT_VALID : 1;
   /** Reserved */
   IFX_uint32_t Res05 : 3;
#endif
} __PACKED__ ;


/**
   This message reports the HPT Results of the last invocation of the GR909 Linetesting
   Module. The results are only valid if the bit HPT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
struct CMD_GR909_Result_HPT
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** HPT AC RING wire to GND result */
   IFX_uint32_t HPT_AC_R2G : 16;
   /** HPT AC TIP wire to GND result */
   IFX_uint32_t HPT_AC_T2G : 16;
   /** HPT AC TIP wire to RING wire result */
   IFX_uint32_t HPT_AC_T2R : 16;
   /** HPT DC RING wire to GND result */
   IFX_uint32_t HPT_DC_R2G : 16;
   /** HPT DC TIP wire to GND result */
   IFX_uint32_t HPT_DC_T2G : 16;
   /** HPT DC TIP wire to RING wire result */
   IFX_uint32_t HPT_DC_T2R : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** HPT AC RING wire to GND result */
   IFX_uint32_t HPT_AC_R2G : 16;
   /** HPT AC TIP wire to GND result */
   IFX_uint32_t HPT_AC_T2G : 16;
   /** HPT AC TIP wire to RING wire result */
   IFX_uint32_t HPT_AC_T2R : 16;
   /** HPT DC RING wire to GND result */
   IFX_uint32_t HPT_DC_R2G : 16;
   /** HPT DC TIP wire to GND result */
   IFX_uint32_t HPT_DC_T2G : 16;
   /** HPT DC TIP wire to RING wire result */
   IFX_uint32_t HPT_DC_T2R : 16;
#endif
} __PACKED__ ;


/**
   This message reports the FEMF Results of the last invocation of the GR909
   Linetesting Module. The results are only valid if the bit FEMF_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
struct CMD_GR909_Result_FEMF
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** FEMF AC RING wire to GND result */
   IFX_uint32_t FEMF_AC_R2G : 16;
   /** FEMF AC TIP wire to GND result */
   IFX_uint32_t FEMF_AC_T2G : 16;
   /** FEMF AC TIP wire to RING wire result */
   IFX_uint32_t FEMF_AC_T2R : 16;
   /** FEMF DC RING wire to GND result */
   IFX_uint32_t FEMF_DC_R2G : 16;
   /** FEMF DC TIP wire to GND result */
   IFX_uint32_t FEMF_DC_T2G : 16;
   /** FEMF DC TIP wire to RING wire result */
   IFX_uint32_t FEMF_DC_T2R : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read/Write */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** FEMF AC RING wire to GND result */
   IFX_uint32_t FEMF_AC_R2G : 16;
   /** FEMF AC TIP wire to GND result */
   IFX_uint32_t FEMF_AC_T2G : 16;
   /** FEMF AC TIP wire to RING wire result */
   IFX_uint32_t FEMF_AC_T2R : 16;
   /** FEMF DC RING wire to GND result */
   IFX_uint32_t FEMF_DC_R2G : 16;
   /** FEMF DC TIP wire to GND result */
   IFX_uint32_t FEMF_DC_T2G : 16;
   /** FEMF DC TIP wire to RING wire result */
   IFX_uint32_t FEMF_DC_T2R : 16;
#endif
} __PACKED__ ;


/**
   This message reports the resistive faults test results of the last invocation of the
   GR909 Linetesting Module. The results are only valid if the bit RFT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
struct CMD_GR909_Result_RFT
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** RFT RING wire to GND result */
   IFX_uint32_t RFT_R2G : 16;
   /** RFT TIP wire to GND result */
   IFX_uint32_t RFT_T2G : 16;
   /** RFT TIP wire to RING wire result */
   IFX_uint32_t RFT_T2R : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** RFT RING wire to GND result */
   IFX_uint32_t RFT_R2G : 16;
   /** RFT TIP wire to GND result */
   IFX_uint32_t RFT_T2G : 16;
   /** RFT TIP wire to RING wire result */
   IFX_uint32_t RFT_T2R : 16;
#endif
} __PACKED__ ;


/**
   This message reports the receiver off hook test results of the last invocation of
   the GR909 Linetesting Module. The results are only valid if the bit ROH_VALID in
   message CMD_GR909_Result_Pass_Fail shows the value valid.
*/
struct CMD_GR909_Result_ROH
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** ROH TIP wire to RING wire result for low voltage */
   IFX_uint32_t ROH_T2R_L : 16;
   /** ROH TIP wire to RING wire result for high voltage */
   IFX_uint32_t ROH_T2R_H : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** ROH TIP wire to RING wire result for low voltage */
   IFX_uint32_t ROH_T2R_L : 16;
   /** ROH TIP wire to RING wire result for high voltage */
   IFX_uint32_t ROH_T2R_H : 16;
#endif
} __PACKED__ ;


/**
   This message reports the ringer impedance test results of the last invocation of the
   GR909 Linetesting Module. The results are only valid if the bit RIT_VALID in message
   CMD_GR909_Result_Pass_Fail shows the value valid.
*/
struct CMD_GR909_Result_RIT
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** RIT result */
   IFX_uint32_t RIT_RES : 16;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** RIT result */
   IFX_uint32_t RIT_RES : 16;
#endif
} __PACKED__ ;


struct CMD_DCCTL_Debug
{
#if __BYTE_ORDER == __BIG_ENDIAN
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** Reserved */
   IFX_uint32_t Res00 : 4;
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** Data 1*/
   IFX_uint16_t data1;
   /** Data 2*/
   IFX_uint16_t data2;
   /** Data 3*/
   IFX_uint16_t data3;
   /** Data 4*/
   IFX_uint16_t data4;
   /** Data 5*/
   IFX_uint16_t data5;
#else
   /** Channel */
   IFX_uint32_t CH : 4;
   /** Reserved */
   IFX_uint32_t Res01 : 4;
   /** Command */
   IFX_uint32_t CMD : 5;
   /** don’t care */
   IFX_uint32_t BC : 1;
   /** Short command */
   IFX_uint32_t SC : 1;
   /** Read write Access */
   IFX_uint32_t RW : 1;
   /** Length of Message */
   IFX_uint32_t LENGTH : 8;
   /** GR909 Command */
   IFX_uint32_t ECMD : 5;
   /** Module */
   IFX_uint32_t MOD : 3;
   /** Data 1*/
   IFX_uint16_t data1;
   /** Data 2*/
   IFX_uint16_t data2;
   /** Data 3*/
   IFX_uint16_t data3;
   /** Data 4*/
   IFX_uint16_t data4;
   /** Data 5*/
   IFX_uint16_t data5;
#endif
} __PACKED__ ;


#ifdef __cplusplus
}
#endif

/** @} */
#endif
