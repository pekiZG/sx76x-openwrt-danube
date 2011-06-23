#ifndef _DRV_VINETIC_GPIO_H
#define _DRV_VINETIC_GPIO_H
/****************************************************************************
                 Copyright (c) 2005  Infineon Technologies AG
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

   EXCEPT FOR ANY LIABILITY DUE TO WILLFUL ACTS OR GROSS NEGLIGENCE AND
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************/
/**
   \file drv_vinetic_gpio.h VINETIC GPIO/IO resource managment module.
*/
/* ============================= */
/* Global Structures             */
/* ============================= */
typedef struct _VINETIC_GPIO  VINETIC_GPIO;
typedef struct _VINETIC_GPIO_RES VINETIC_GPIO_RES;

/** GPIO structure
\remarks
	The driver uses the same mechanism and structure for device GPIO and channel
	IO, called GPIO resource.
*/
struct _VINETIC_GPIO
{
   /** Handle to VINETIC device or CHANNEL stucture */
   IFX_int32_t devHandle;
   /** Pointer to enclosing reservation struct */
   VINETIC_GPIO_RES *pGpioRes;
   /** Mask of reserved bits for this resource (0 = free, 1 = reserved) */
   IFX_uint16_t nReserve;
   /** Error code for GPIO resource */
   IFX_int32_t err;
};

/** GPIO resource structure
\remarks
	The driver uses the same mechanism and structure for device GPIO and channel
	IO, called GPIO resource.
*/
struct _VINETIC_GPIO_RES
{
   /** Mask of reserved bits for all resources (0 = free, 1 = reserved) */
   IFX_uint16_t nGlobalReserve;
   /** Callback for IRQ handler */
   void (*gpioCallback[8])(int nDev, int nCh, unsigned short nEvt);
   /** Control struct for the single GPIOs */
   VINETIC_GPIO Gpio[8];
};

/* ============================= */
/* Global function declaration   */
/* ============================= */
IFX_void_t VINETIC_GpioInit(IFX_int32_t devHandle);
IFX_void_t VINETIC_GpioIntDispatch(IFX_int32_t devHandle, IFX_uint16_t nMask);

#endif /* _DRV_VINETIC_GPIO_H */

