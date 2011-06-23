#ifndef _DRV_VINETIC_SERIAL_H
#define _DRV_VINETIC_SERIAL_H
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
   Module      : drv_vinetic_serial.h
   Date        : 2005-03-08
   Description : This file contains the declarations of the low level serial
                 function via SPI.
*******************************************************************************/


/* ============================= */
/* Global Defines                */
/* ============================= */

/* vinetic spi recovery time in us */
#define VINETIC_SPI_RECOVERY_TIME  4

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_int32_t serial_write       (VINETIC_DEVICE *pDev, IFX_uint16_t *pData,
                                IFX_int32_t count);
IFX_int32_t serial_read        (VINETIC_DEVICE *pDev,  IFX_uint16_t cmd,
                                IFX_uint16_t   *pData, IFX_int32_t  count);
IFX_int32_t serial_diop_read   (VINETIC_DEVICE *pDev,  IFX_uint16_t *pCmd,
                                IFX_uint16_t   *pData, IFX_int32_t   count);
IFX_int32_t serial_read_packet (VINETIC_DEVICE *pDev, IFX_int32_t size);

#endif /* _DRV_VINETIC_SERIAL_H */

