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

   EXCEPT FOR ANY LIABILITY DUE TO WILLFUL ACTS OR GROSS NEGLIGENCE AND
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

/** \file drv_config_user.h
   This file is intended to customize some optional settings of the driver.

   In case of optional features (like SPI interface) it may also contain
   some required definitions where no default values are making sense.

   This file will only be used if the compiler switch ENABLE_USER_CONFIG
   is defined (e.g. by "configure --enable-user-config")

   \remark
   This file (drv_config_user.default.h) is intended as a template.
   All options are commented in detail, defined with their default values
   (where possible) and disabled with an "#if 0 / #endif" block.

   To use this file, make a copy with the name "drv_config_user.h" in your
   build- or source-directory and enable (#if 1) the options you want to change.
*/

#ifndef _DRV_CONFIG_USER_H
#define _DRV_CONFIG_USER_H

/* ============================= */
/* Includes                      */
/* ============================= */
/* if necessary, add your includes here */


/* ============================= */
/* Defines                       */
/* ============================= */
/** Macro to signal and set an error.
   Useful to generate a trigger signal during hardware debugging! */
#if 0
#define SET_ERROR(no)                     \
   do {                                   \
      /* do not change following line! */ \
      pDev->err = (IFX_int32_t)(no);      \
      IFXOS_ASSERT(IFX_FALSE);            \
   } while(0)
#endif

/** Number of accesses to ready flag, optimize to minimum of processor
   access times. VINETIC needs up to 1,5 us to recover.
   If necessary also the definition of the macro WAIT_FOR_READY_FLAG
   (see drv_vinetic_parallel.h) can be changed here,
   although this is not recommended!!! */
#if 0
#define WAIT_RDY                 500
#endif


/** SPI buffer size configured */
#if 0
#define SPI_MAXBYTES_SIZE        128
#endif

/** SPI chipselect set/unset
    \param high_low  - 0=LOW/1=HIGH
*/
#if 0
#define SPI_CS_SET(high_low)     \
   do                            \
   {                             \
      if ((high_low) == 0)       \
      {                          \
         /* set CSQ = LOW */     \
      }                          \
      else                       \
      {                          \
         /* set CSQ = HIGH */    \
      }                          \
   } while(0)
#endif

/** spi low level access function

   \param txptr  - transmit buffer pointer
   \param txsize - transmit buffer size
   \param rxptr  - receive buffer pointer
   \param rxsize - receive buffer size

   \remark
      This macro must map your spi low level read/write
      access routine.
*/
#if 0
#define spi_ll_read_write(txptr,txsize,rxptr,rxsize)
#endif

/** uC interface chip select recovery,
    as needed by some plattforms (i.e ATA, EASY4261)
*/
#if 0
#define PARACCESS_CS_RECOVER             \
   do                                    \
   {                                     \
      IFX_uint32_t tmp_reg;			     \
      IFX_vuint32_t *ptmp;               \
      ptmp =(IFX_vuint32_t*) 0x80000000; \
      tmp_reg=*ptmp;				           \
   } while(0)
#endif

/** Macro to disable the interrupt line of given irq number.

    \remark
       Define this macro in case the operating system methods
       as defined in sys_drv_ifxos.h aren't suitable for your system.
       (i.e FPGA controls interrupts)
*/
#if 0
#define VIN_DISABLE_IRQLINE(irq)                IFXOS_IRQ_DISABLE(irq)
#endif

/** Macro to enable the interrupt line of given irq number.

    \remark
       Define this macro in case the operating system methods
       as defined in sys_drv_ifxos.h aren't suitable for your system.
       (i.e FPGA controls interrupts)
*/
#if 0
#define VIN_ENABLE_IRQLINE(irq)                 IFXOS_IRQ_ENABLE(irq)
#endif

/** Macro to disable the globale interrupt.

    \remark
       Define this macro in case the operating system methods
       as defined in sys_drv_ifxos.h aren't suitable for your system.
       (i.e FPGA controls interrupts)
*/
#if 0
#define VIN_DISABLE_IRQGLOBAL(var)              IFXOS_LOCKINT(var)
#endif

/** Macro to enable the globale interrupt.

    \remark
       Define this macro in case the operating system methods
       as defined in sys_drv_ifxos.h aren't suitable for your system.
       (i.e FPGA controls interrupts)
*/
#if 0
#define VIN_ENABLE_IRQGLOBAL(var)               IFXOS_UNLOCKINT(var)
#endif

/** Macro to map the system function with takes care of interrupt handling
    registration.

    \param irq   -  irq number
    \param func  -  interrupt handler callback function
    \param arg   -  argument of interrupt handler callback function

    \remarks
      The macro is by default mapped to the operating system method. For systems
      integrating different routines, this macro must be adapted in the user
      configuration header file.

      This macro may have different arguments set according to the requirements
      of the system or operating system used.
*/
#if 0
#define VIN_SYS_REGISTER_INT_HANDLER(irq,func,arg)             \
            intConnect(INUM_TO_IVEC(irq), (VOIDFUNCPTR)(func), \
            (IFX_int32_t)(arg))
#endif

/** Macro to map the system function with takes care of interrupt handling
    unregistration.

    \param irq - irq number

    \remarks
      The macro is by default mapped to the operating system method. For systems
      integrating different routines, this macro must be adapted in the user
      configuration header file.

      This macro may have different arguments set according to the requirements
      of the system or operating system used.
*/
#if 0
#define VIN_SYS_UNREGISTER_INT_HANDLER(irq)  \
         VIN_SYS_REGISTER_INT_HANDLER((irq), OS_IRQHandler_Dummy, (irq))
#endif

/** Shift factor for Vinetic 1x parallel mux access,
    system dependent (i.e processor architecture, data bus connection).

   \remark
     Please adapt macro in user configuration file if necessary.
*/
#if 0
#define VIN_1X_PARACC_MUX_SHIFT   0
#endif

/** Shift factor for Vinetic 2x parallel mux access,
    system dependent (i.e processor architecture, data bus connection).

   \remark
     Please adapt macro in user configuration file if necessary.
*/
#if 0
#define VIN_2X_PARACC_MUX_SHIFT   1
#endif

/** Shift factor for Vinetic parallel access,
    exclusive mux access,
    system dependent (i.e processor architecture, data bus connection).

   \remark
     Please adapt macro in user configuration file if necessary.
*/
#if 0
#define VIN_PARACC_SHIFT          0
#endif

#endif /* _DRV_CONFIG_USER_H */