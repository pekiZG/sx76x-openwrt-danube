#ifndef _DRV_VINETIC_STREAM_H
#define _DRV_VINETIC_STREAM_H
/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************
   Module      : drv_vinetic_stream.h
   Date        : 2001-11-20
   Description : This file contains the definition of the global functions for
                 the Voice Up- and Downstreaming
*******************************************************************************/

/* ============================= */
/* Global Defines                */
/* ============================= */


/* ============================= */
/* Global Structures             */
/* ============================= */


/* ============================= */
/* Global function declaration   */
/* ============================= */
extern IFX_int32_t VoIP_UpStream   (VINETIC_CHANNEL *pCh,       IFX_uint8_t* buf, IFX_int32_t count);
extern IFX_int32_t VoIP_DownStream (VINETIC_CHANNEL *pCh, const IFX_uint8_t* buf, IFX_int32_t count);
#if (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38)
extern IFX_int32_t Fax_UpStream    (VINETIC_CHANNEL *pCh,       IFX_uint8_t* buf, IFX_int32_t count);
extern IFX_int32_t Fax_DownStream  (VINETIC_CHANNEL *pCh, const IFX_uint8_t* buf, IFX_int32_t count);
#endif /* (VIN_CFG_FEATURES & VIN_FEAT_FAX_T38) */
extern IFX_uint8_t getPacketPtr    (VINETIC_DEVICE *pDev, IFX_uint16_t *pPacketCmd,
                                    FIFO **pFifo, PACKET **pPacket);
extern IFX_void_t FifoPut          (VINETIC_DEVICE *pDev, IFX_uint16_t nCmd1, FIFO **pFifo,
                                    PACKET **pPacket);
#endif /* _DRV_VINETIC_STREAM_H */
