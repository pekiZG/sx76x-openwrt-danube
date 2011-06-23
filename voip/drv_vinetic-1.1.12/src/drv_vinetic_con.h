#ifndef _drv_vinetic_con_H
#define _drv_vinetic_con_H
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
   Module      : drv_vinetic_con.h
   Date        : 2005-02-16
   Description : This file contains the declaration of all interrup specific
                 messages.
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */

/** Module types of the firmware */
typedef enum
{
   VINDSP_MT_ALM,
   VINDSP_MT_SIG,
   VINDSP_MT_COD,
   VINDSP_MT_PCM
} VINDSP_MT;

/** defines which input of the signaling module is to be used: ALM uses input2 */
enum
{
   ALM_TO_SIG_IN = 1,
   COD_TO_SIG_IN = 0
};

/** which signaling input is used for local (with auto suppression) and remote
    (with event playout) connections. This is fixed by firmware */
enum
{
   REMOTE_SIG_IN = 1,
   LOCAL_SIG_IN = 0
};

#ifdef DEBUG
extern const IFX_char_t signalName [52][10];
#endif /* DEBUG */


/* ============================= */
/* Global Structures             */
/* ============================= */

typedef IFX_uint8_t MODULE_SIGNAL_INPUTS [MAX_MODULE_SIGNAL_INPUTS];

typedef struct
{
   MODULE_SIGNAL_INPUTS       alm;
   IFX_uint8_t almChg;
   MODULE_SIGNAL_INPUTS       sig; /* only two used */
   IFX_uint8_t sigChg;
   MODULE_SIGNAL_INPUTS       cod;
   IFX_uint8_t codChg;
} SIGNAL_ARRAY_INPUTS;

/* ============================= */
/* Global Variables              */
/* ============================= */

/* ============================= */
/* Global function declaration   */
/* ============================= */
extern int addModuleSigIn (VINETIC_CHANNEL *pCh,
                           VINDSP_MT mt,
                           unsigned char nSignal,
                           SIGNAL_ARRAY_INPUTS* pSigs);
extern int removeModuleSigIn (VINETIC_CHANNEL *pCh,
                              VINDSP_MT mt,
                              unsigned char nSignal,
                              SIGNAL_ARRAY_INPUTS* pSigs);
extern int Con_ConnectPrepare (VINDSP_MODULE* pSrc,
                               VINDSP_MODULE* pDst,
                               IFX_uint8_t nSignal);
extern int Con_DisconnectPrepare (VINDSP_MODULE* pSrc,
                                  VINDSP_MODULE* pDst,
                                  IFX_uint8_t nSignal);
extern int Con_ConnectConfigure (VINETIC_DEVICE *pDev);
extern int Con_ConfSignals (VINETIC_CHANNEL *pCh,
                     SIGNAL_ARRAY_INPUTS* pSigs);
extern void getSigIns (const VINETIC_CHANNEL *pCh, SIGNAL_ARRAY_INPUTS *pSigs);


#endif /* _drv_vinetic_con_H */

