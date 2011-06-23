#ifndef _DRV_VINETIC_GR909_H
#define _DRV_VINETIC_GR909_H

/****************************************************************************
                  Copyright © 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE, 
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS 
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING 
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP, 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE 
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY 
   THIRD PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY 
   INTELLECTUAL PROPERTY INFRINGEMENT. 

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : drv_vinetic_gr909.h
   Date        : 2003-10-30
   Description : This file contains the declaration of the globals defines
                 and functions needed for Vinetic GR909 realtime measurements
   Remarks     : Driver should be compiled with -DTAPI_GR909 to support
                 this measurement.
*******************************************************************************/

#if (VIN_CFG_FEATURES & VIN_FEAT_GR909)

/* ============================= */
/* Global Defines                */
/* ============================= */

/* states enums */
enum
{
   /* voltage measurement states */
   VOLT_PRESET,
   VOLT_RING_GND,
   VOLT_TIP_GND,
   VOLT_TIP_RNG,
   /* resistive fault states */
   RESFAULT_PRESET,
   RESFAULT_TIP_RNG,
   RESFAULT_TIP_GND,
   RESFAULT_RING_GND,
   /* receiver offhook states */
   RECOFFHOOK_PRESET,
   RECOFFHOOK_WITH15MA,
   RECOFFHOOK_WITH30MA,
   /* ringer states */
   RINGER_PRESET,
   RINGER_CURR_DC,
   RINGER_CURR_AC,
   /* common states */
   TEST_OK_STATE,
   TEST_NOTOK_STATE,
   INTERNAL_ERR_STATE
};

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global Variables              */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

extern IFX_int32_t  VINETIC_GR909_OSCreateTask (VINETIC_CHANNEL *pCh);
extern IFX_int32_t  VINETIC_GR909_OSDeleteTask (VINETIC_CHANNEL *pCh);
extern IFX_int32_t  VINETIC_GR909_OSTask       (IFX_uint32_t device);

extern IFX_int32_t  VINETIC_GR909_Result (VINETIC_CHANNEL *pCh, VINETIC_IO_GR909 *pResult);
extern IFX_int32_t  VINETIC_GR909_Start  (VINETIC_CHANNEL *pCh, VINETIC_IO_GR909 *pTest);
extern IFX_void_t   VINETIC_GR909_Stop   (VINETIC_CHANNEL *pCh, IFX_int32_t nStopFlag);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_GR909) */

#endif /* _DRV_VINETIC_GR909_H */

