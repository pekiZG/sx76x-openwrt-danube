#ifndef DRV_TAPINOOS_H
#define DRV_TAPINOOS_H
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

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************
   Module      : drv_Timer.h
   Desription  : Headerfile for the timer functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

/* ============================= */
/* Global Defines                */
/* ============================= */

/* ============================= */
/* Defines for hook              */
/* detection tolerance           */
/* ============================= */

#define TAPI_MIN_HOOK             1
#define TAPI_MAX_HOOK             4
#define TAPI_MIN_FLASH            5
#define TAPI_MAX_FLASH            7
#define TAPI_HOOK_TIMEOUT         8

/* Functionpointer to the callbackfunction */
typedef IFX_void_t (*TIMER_ENTRY)(IFX_void_t * timer_id, IFX_uint32_t arg);
typedef IFX_void_t (*timerfunc)(IFX_int32_t);

/* Timer ID */
typedef struct
{
   timerfunc     pNoOSTimer;
   IFX_int32_t   nNoOSArg;
   IFX_int32_t   nTimerId;
   IFX_boolean_t Periodical;
   IFX_uint32_t  nTimeout;
   IFX_uint32_t  nTimeRest;
   TIMER_ENTRY   pTimerEntry;
   IFX_int32_t   nArgument;
} *Timer_ID;


#endif  /* DRV_TAPINOOS_H */