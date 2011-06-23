#ifndef _DRV_VINETIC_LT_H
#define _DRV_VINETIC_LT_H
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
   Module      : drv_vinetic_lt
   Date        : 2004-04-15
   Description :
      Linetesting 2.0 implemenation
 *****************************************************************************/
/* ============================= */
/* Includes                      */
/* ============================= */

/* ============================= */
/* Defines                       */
/* ============================= */

/* linetesting configuration data */
typedef struct
{
   unsigned short             tIntCurrent;
   unsigned short             tIntVoltage;
   unsigned short             tIntResistance;
} VINETIC_LT_CONFIG;

/* ============================= */
/* Global variable declaration   */
/* ============================= */
extern VINETIC_LT_CONFIG ltCfg;

/* ============================= */
/* Global function definition    */
/* ============================= */
#if (VIN_CFG_FEATURES & VIN_FEAT_LT)
IFX_int32_t VINETIC_LT_Current         (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_CURRENT *pIO);
IFX_int32_t VINETIC_LT_Voltage         (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_VOLTAGE *pIO);
IFX_int32_t VINETIC_LT_Resistance      (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_RESISTANCE *pIO);
IFX_int32_t VINETIC_LT_Capacitance     (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_CAPACITANCE *pIO);
IFX_int32_t VINETIC_LT_Impedance       (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_IMPEDANCE *pIO);
IFX_int32_t VINETIC_LT_AC_Measurements (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_AC_MEASUREMENTS *pIO);
IFX_int32_t VINETIC_LT_AC_Direct       (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_AC_DIRECT *pIO);
#ifdef VIN_V21_SUPPORT
IFX_int32_t VINETIC_LT_Resistance_RC   (VINETIC_CHANNEL *pCh, VINETIC_IO_LT_RESISTANCE_RC *pIO);
#endif /* VIN_V21_SUPPORT */
IFX_int32_t VINETIC_LT_Init            (IFX_void_t);
IFX_int32_t VINETIC_LT_Config          (VINETIC_IO_LT_CONFIG *pIO);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_LT) */

IFX_int32_t VINETIC_DC_Offset_Calibration (VINETIC_DEVICE *pDev, VINETIC_IO_OFFSET_CALIBRATION *pCal);

#endif /* _DRV_VINETIC_LT_H */
