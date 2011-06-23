#ifndef _DRV_VINETIC_PARALLEL_H
#define _DRV_VINETIC_PARALLEL_H
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
   Module      : drv_vinetic_parallel
   Description :
      Header file for drv_vinetic_parallel.c
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/* number of accesses to ready flag, optimize to minimum of processor
   access times. VINETIC needs up to 1,5 us to recover */
#ifndef WAIT_RDY
#define WAIT_RDY     500
#endif

/*******************************************************************************
Description:
   Waits for the Ready bit to signal ready
Remarks:
   Must be called for every low level access and is used only in parallel
   access module drv_vinetic_parallel.c
   Make sure that WAIT_RDY is not to small for your processor.
   VINETIC needs up to 1,5 us to recover.
*******************************************************************************/
#ifndef WAIT_FOR_READY_FLAG
#if (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT)
#define WAIT_FOR_READY_FLAG                      \
   do                                            \
   {                                             \
      if (pDev->nChipMajorRev < VINETIC_V2x)     \
      {                                          \
         cnt = 0;                                \
         while (*(pIr) & IR_RDYQ)                \
         {                                       \
           if (cnt++ > WAIT_RDY)                 \
               goto ACCESS_ERR;                  \
         }                                       \
      }                                          \
   }                                             \
   while(0)
#else
#ifdef VIN_V21_SUPPORT
      /* Code for V2.1 version */
#define WAIT_FOR_READY_FLAG      do {/* nothing */} while(0)
#endif /* VIN_V21_SUPPORT */

#ifdef VIN_V14_SUPPORT
      /* Code for V1.6 and older version */
#define WAIT_FOR_READY_FLAG                     \
   do                                           \
   {                                            \
      cnt = 0;                                  \
      while (*(pIr) & IR_RDYQ)                  \
      {                                         \
        if (cnt++ > WAIT_RDY)                   \
            goto ACCESS_ERR;                    \
      }                                         \
   }                                            \
   while(0)
#endif /* VIN_V14_SUPPORT */
#endif /* (defined VIN_V21_SUPPORT) && (defined VIN_V14_SUPPORT) */
#endif /* WAIT_FOR_READY_FLAG */

/* ============================= */
/* Global Structures             */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */

IFX_int32_t parallel_write         (VINETIC_DEVICE *pDev, IFX_uint16_t *pData, IFX_int32_t nWr);
IFX_int32_t parallel_read          (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData, IFX_int32_t nRd);
IFX_int32_t parallel_diop_read     (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t parallel_packet_read   (VINETIC_DEVICE *pDev, IFX_int32_t size);

IFX_int32_t par8bit_read           (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t par8bit_write          (VINETIC_DEVICE *pDev, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t par8bit_diop_read      (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t par8bit_packet_read    (VINETIC_DEVICE *pDev, IFX_int32_t size);

IFX_int32_t parint8bit_read        (VINETIC_DEVICE *pDev, IFX_uint16_t cmd, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t parint8bit_write       (VINETIC_DEVICE *pDev, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t parint8bit_diop_read   (VINETIC_DEVICE *pDev, IFX_uint16_t *pCmd, IFX_uint16_t *pData, IFX_int32_t count);
IFX_int32_t parint8bit_packet_read (VINETIC_DEVICE *pDev, IFX_int32_t size);

#endif /* _DRV_VINETIC_PARALLEL_H */

