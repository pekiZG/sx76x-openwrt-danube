/****************************************************************************
                  Copyright  2005  Infineon Technologies AG
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
 ****************************************************************************/

/** \file drv_config_user.h
   This file is intended to customize some optional settings of the driver.

   In case of optional features (like SPI interface) it may also contain
   some required definitions where no default values are making sense.

   This file will only be used if the compiler switch ENABLE_USER_CONFIG
   is defined (e.g. by "configure --enable-user-config")

   \remark
   This file (drv_config_user.amazon.h) is already prepared for systems 
   with the amazon chipset.
   
   To use this file, make a copy or link with the name "drv_config_user.h" 
   in your build- or source-directory.
*/

#ifndef _DRV_CONFIG_USER_H
#define _DRV_CONFIG_USER_H

/* ============================= */
/* Includes                      */
/* ============================= */
#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>

extern int amazon_ssc_cs_low(u32 pin);
extern int amazon_ssc_cs_high(u32 pin);
extern int amazon_ssc_txrx(char * tx_buf, u32 tx_len, char * rx_buf, u32 rx_len);


/* ============================= */
/* Defines                       */
/* ============================= */


/* SPI buffer size configured */
#define SPI_MAXBYTES_SIZE          32  /*SPI_BUFFER_SIZE*/

/* SPI chipselect set/unset */
//#define SPI_CS_SET(high_low) printk("a")

#define SPI_CS_SET(high_low)     do  {                \
            if ((high_low) == 0)                      \
            {                                        \
               amazon_ssc_cs_low(0);                  \
            }                                         \
            else                                      \
            {                                         \
               amazon_ssc_cs_high(0);                 \
            }                                         \
         } while(0)

/* spi low level access function */
//#define spi_ll_read_write(txptr,txsize,rxptr,rxsize) printk("a")

#define spi_ll_read_write(txptr,txsize,rxptr,rxsize)  \
                     amazon_ssc_txrx ((txptr),(txsize),(rxptr),(rxsize))

extern int ifx_ssc_open(struct inode *, struct file *);
extern int ifx_ssc_close(struct inode *, struct file *);

#endif /* _DRV_CONFIG_USER_H */

