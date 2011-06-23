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
#ifndef DRV_VINETIC_VXWORKS_H
#define DRV_VINETIC_VXWORKS_H


/* ============================= */
/* Global definition             */
/* ============================= */


/* ============================= */
/* Local variable definition     */
/* ============================= */

#define MAX_DEVICE_HEADER       (VINETIC_MAX_DEVICES*(VINETIC_MAX_CH_NR+1))
#define VINETIC_DEVICE_NAME     "/dev/vin"

#define MAX_ISR_EVENTS          MAX_IRQ_EVENTS
#define VINETIC_DEVICE_CONTROL_CHN_TYPE     0


/*
    Device header passed to the IOS at the device driver creation during
    the system startup.
*/
typedef struct
{
    DEV_HDR           DevHdr;   /* VxWorks speciifc: IOS header               */
    IFX_int32_t       ddrvn;    /* Device Driver Number                       */
    IFX_int32_t       devn;     /* Device number                              */
    IFX_int32_t       chnn;     /* Channel Number                             */
    IFX_int32_t       InUse;    /* In Use counter                             */
    IFX_void_t*       pctx;     /* context pointer: device or channel context */
} Y_S_dev_header;

/*
    Global device driver structure.
    IOS Device driver Context.
*/
typedef struct
{
   /* Device Driver Number     */
   IFX_int32_t         ddrvn;
   /* Max device               */
   IFX_int32_t         maxdev;
   /* Max device channel       */
   IFX_int32_t         maxchn;
   /* Interface Semaphore      */
   IFXOS_mutex_t       SemDrvIF;
   /* device header pointer    */
   Y_S_dev_header*     pdevh[MAX_DEVICE_HEADER];
   /* VINETIC device pointers  */
   VINETIC_DEVICE*     pdevv[VINETIC_MAX_DEVICES];
} Y_S_dev_ctx;


/* ============================= */
/* Global function definition    */
/* ============================= */

/* Open function    */
IFX_int32_t    vinetic_open (
   Y_S_dev_header *pDevHeader,
   INT8 *pAnnex,
   IFX_int32_t flags);
/* Ioctl function   */
IFX_int32_t    vinetic_ioctl(
   Y_S_dev_header *pDevHeader,
   IFX_int32_t nCmd,
   IFX_int32_t nArgument);
/* Close function */
STATUS   vinetic_close(
   Y_S_dev_header *pDevHeader);
/* Read function    */
IFX_int32_t    vinetic_read (
   Y_S_dev_header *pDevHeader,
   IFX_uint8_t *pDest,
   IFX_int32_t nLength);
/* Write function */
IFX_int32_t    vinetic_write(
   Y_S_dev_header *pDevHeader,
   IFX_uint8_t *pSrc,
   IFX_int32_t nLength);

/* VINETIC Device driver Init */
IFX_int32_t    Vinetic_DeviceDriverInit(VOID);
/* VINETIC Device driver Stop */
IFX_int32_t    Vinetic_DeviceDriverStop(VOID);

#endif /* DRV_VINETIC_VXWORKS_H */
