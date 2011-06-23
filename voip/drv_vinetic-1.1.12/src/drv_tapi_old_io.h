#ifndef DRV_TAPI_OLD_IO_H
#define DRV_TAPI_OLD_IO_H
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
 ****************************************************************************
   Module      : drv_tapi_old_io.h
   Desription  : Contains old TAPI I/O commands defines.
   Remarks     : This file is used for backward compatibility. Applications
                 using old ioctl defines should be compiled with -DTAPI_OLD_IO
*******************************************************************************/

/* ============================= */
/* Global defines                */
/* ============================= */

/*
   Error Codes
*/

#ifndef SUCCESS
#define SUCCESS   0
#endif
#ifndef ERR
#define ERR      -1
#endif

/*
   #ifndef <old_io_cmd>
   #define <old_io_cmd>    <new_io_cmd>
   #endif
*/

#ifndef PHONE_CAPABILITIES_CHECK
/** This service checks if a specific capability is supported.
  refer to \ref IFX_TAPI_CAP_CHECK */
#define PHONE_CAPABILITIES_CHECK                         IFX_TAPI_CAP_CHECK
#endif

#ifndef PHONE_CAPABILITIES_LIST
/** This service returns the capability lists.
  refer to \ref IFX_TAPI_CAP_LIST */
#define PHONE_CAPABILITIES_LIST                          IFX_TAPI_CAP_LIST
#endif

#ifndef PHONE_CAPABILITIES
/** This service returns the number of capabilities.
  refer to \ref IFX_TAPI_CAP_NR */
#define PHONE_CAPABILITIES                               IFX_TAPI_CAP_NR
#endif

#ifndef IFXPHONE_INIT
/** This service sets the default initialization of device and hardware.
  refer to \ref IFX_TAPI_CH_INIT */
#define IFXPHONE_INIT                                    IFX_TAPI_CH_INIT
#endif

#ifndef IFXPHONE_PHONE_STATUS
/** Query status information about the phone line.
  refer to \ref IFX_TAPI_CH_STATUS_GET */
#define IFXPHONE_PHONE_STATUS                            IFX_TAPI_CH_STATUS_GET
#endif

#ifndef IFXPHONE_GETCIDCONF
/** Read out the configuration of the CID transmitter.
  refer to \ref IFX_TAPI_CID_CFG_GET */
#define IFXPHONE_GETCIDCONF                              IFX_TAPI_CID_CFG_GET
#endif

#ifndef IFXPHONE_CIDCONF
/** Configures the CID transmitter.
  refer to \ref IFX_TAPI_CID_CFG_SET */
#define IFXPHONE_CIDCONF                              IFX_TAPI_CID_CFG_SET
#endif

#ifndef IFXPHONE_CID_CONF
/** Configures the CID transmitter.
  refer to \ref IFX_TAPI_CID_CFG_SET */
#define IFXPHONE_CID_CONF                             IFX_TAPI_CID_CFG_SET
#endif

#ifndef IFXPHONE_CIDRX_DATA
/** Reads CID Data collected since caller id receiver was started.
  refer to \ref IFX_TAPI_CID_RX_DATA_GET */
#define IFXPHONE_CIDRX_DATA                           IFX_TAPI_CID_RX_DATA_GET
#endif

#ifndef IFXPHONE_CIDRX_START
/** Setup the CID Receiver to start receiving CID Data.
  refer to \ref IFX_TAPI_CID_RX_START */
#define IFXPHONE_CIDRX_START                          IFX_TAPI_CID_RX_START
#endif

#ifndef IFXPHONE_CIDRX_STATUS
/** Retrieves the current status information of the CID Receiver.
  refer to \ref IFX_TAPI_CID_RX_STATUS_GET */
#define IFXPHONE_CIDRX_STATUS                      IFX_TAPI_CID_RX_STATUS_GET
#endif

#ifndef IFXPHONE_CIDRX_STOP
/** Stop the CID Receiver.
  refer to \ref IFX_TAPI_CID_RX_STOP */
#define IFXPHONE_CIDRX_STOP                        IFX_TAPI_CID_RX_STOP
#endif

#ifndef IFXPHONE_CID_INFO_TX
/** This interfaces transmits only CID presentation layer message.
  refer to \ref IFX_TAPI_CID_TX_INFO_START */
#define IFXPHONE_CID_INFO_TX                       IFX_TAPI_CID_TX_INFO_START
#endif

#ifndef IFXPHONE_CID_SEQ_TX
/** This interfaces transmits only CID presentation layer message.
  refer to \ref IFX_TAPI_CID_TX_INFO_START */
#define IFXPHONE_CID_SEQ_TX                        IFX_TAPI_CID_TX_SEQ_START
#endif

#ifndef IFXPHONE_REPORT_SET
/** Set the report levels if the driver is compiled with ENABLE_TRACE.
  refer to \ref IFX_TAPI_DEBUG_REPORT_SET */
#define IFXPHONE_REPORT_SET                        IFX_TAPI_DEBUG_REPORT_SET
#endif

#ifndef PHONE_PLAY_LEVEL
/** This service returns the level of the most recently played signal.
  refer to \ref IFX_TAPI_DEC_LEVEL_GET */
#define PHONE_PLAY_LEVEL                           IFX_TAPI_DEC_LEVEL_GET
#endif

#ifndef PHONE_PLAY_START
/** Starts the playout of data.
  refer to \ref IFX_TAPI_DEC_START */
#define PHONE_PLAY_START                           IFX_TAPI_DEC_START
#endif

#ifndef PHONE_PLAY_STOP
/** Stops the playout of data.
  refer to \ref IFX_TAPI_DEC_STOP */
#define PHONE_PLAY_STOP                            IFX_TAPI_DEC_STOP
#endif

#ifndef PHONE_PLAY_CODEC
/** Select a codec for the data channel playout.
  refer to \ref IFX_TAPI_DEC_TYPE_SET */
#define PHONE_PLAY_CODEC                           IFX_TAPI_DEC_TYPE_SET
#endif

#ifndef PHONE_PLAY_VOLUME
/** Sets the playout volume.
  refer to \ref IFX_TAPI_DEC_VOLUME_SET */
#define PHONE_PLAY_VOLUME                          IFX_TAPI_DEC_VOLUME_SET
#endif

#ifndef IFXPHONE_GETAGCCONF
/** Get the current AGC coefficients of a coder module.
  refer to \ref IFX_TAPI_ENC_AGC_CFG_GET */
#define IFXPHONE_GETAGCCONF                        IFX_TAPI_ENC_AGC_CFG_GET
#endif

#ifndef IFXPHONE_AGCCONF
/** Set the AGC coefficient for a coder module This implementation assumes that
    an index of a AGC resource is fix assigned to the related index of the
    coder module.
  refer to \ref IFX_TAPI_ENC_AGC_CFG_SET */
#define IFXPHONE_AGCCONF                           IFX_TAPI_ENC_AGC_CFG_SET
#endif

#ifndef IFXPHONE_AGC
/** Enable / Disable the AGC resource This implementation assumes that the AGC
    resouces inside a VINETIC are fixed assigned to the related Coder-Module.
  refer to \ref IFX_TAPI_ENC_AGC_SET */
#define IFXPHONE_AGC                               IFX_TAPI_ENC_AGC_SET
#endif

#ifndef IFXPHONE_GET_FRAME
/** This service gets the frame length for the audio packets.
  refer to \ref IFX_TAPI_ENC_FRAME_LEN_GET */
#define IFXPHONE_GET_FRAME                         IFX_TAPI_ENC_FRAME_LEN_GET
#endif

#ifndef PHONE_FRAME
/** This service sets the frame length for the audio packets.
  refer to \ref IFX_TAPI_ENC_FRAME_LEN_SET */
#define PHONE_FRAME                                IFX_TAPI_ENC_FRAME_LEN_SET
#endif

#ifndef PHONE_REC_LEVEL
/** This service returns the level of the most recently recorded signal.
  refer to \ref IFX_TAPI_ENC_LEVEL_SET */
#define PHONE_REC_LEVEL                            IFX_TAPI_ENC_LEVEL_SET
#endif

#ifndef PHONE_REC_START
/** Start recording on the data channel.
  refer to \ref IFX_TAPI_ENC_START */
#define PHONE_REC_START                            IFX_TAPI_ENC_START
#endif

#ifndef PHONE_REC_STOP
/** Start recording (generating packets) on this channel.
  refer to \ref IFX_TAPI_ENC_STOP */
#define PHONE_REC_STOP                             IFX_TAPI_ENC_STOP
#endif

#ifndef PHONE_REC_CODEC
/** Select a codec for the data channel.
  refer to \ref IFX_TAPI_ENC_TYPE_SET */
#define PHONE_REC_CODEC                            IFX_TAPI_ENC_TYPE_SET
#endif

#ifndef PHONE_VAD
/** Configures the voice activity detection and silence handling Voice
    Activity Detection (VAD) is a feature that allows the codec to determine
    when to send voice data or silence data.
    refer to \ref IFX_TAPI_ENC_VAD_CFG_SET */
#define PHONE_VAD                                  IFX_TAPI_ENC_VAD_CFG_SET
#endif

#ifndef PHONE_REC_VOLUME
/** Sets the recording volume.
  refer to \ref IFX_TAPI_ENC_VOLUME_SET */
#define PHONE_REC_VOLUME                           IFX_TAPI_ENC_VOLUME_SET
#endif

#ifndef PHONE_EXCEPTION
/** This service returns the current exception status.
  refer to \ref IFX_TAPI_EXCEPTION_GET */
#define PHONE_EXCEPTION                            IFX_TAPI_EXCEPTION_GET
#endif

#ifndef IFXPHONE_MASK_EXCEPTION
/** This service masks the reporting of the exceptions.
  refer to \ref IFX_TAPI_EXCEPTION_MASK */
#define IFXPHONE_MASK_EXCEPTION                    IFX_TAPI_EXCEPTION_MASK
#endif

#ifndef IFXPHONE_JB_CONF
/** Configures the jitter buffer.
  refer to \ref IFX_TAPI_JB_CFG_SET */
#define IFXPHONE_JB_CONF                           IFX_TAPI_JB_CFG_SET
#endif

#ifndef IFXPHONE_JB_STATISTICS
/** Reads out jitter buffer statistics.
  refer to \ref IFX_TAPI_JB_STATISTICS_GET */
#define IFXPHONE_JB_STATISTICS                     IFX_TAPI_JB_STATISTICS_GET
#endif

#ifndef IFXPHONE_JB_RESET
/** Resets the jitter buffer statistics.
  refer to \ref IFX_TAPI_JB_STATISTICS_RESET */
#define IFXPHONE_JB_RESET                          IFX_TAPI_JB_STATISTICS_RESET
#endif

#ifndef IFXPHONE_GETLECCONF_PCM
/** Get the LEC configuration for PCM.
  refer to \ref IFX_TAPI_LEC_PCM_CFG_GET */
#define IFXPHONE_GETLECCONF_PCM                    IFX_TAPI_LEC_PCM_CFG_GET
#endif

#ifndef IFXPHONE_LECCONF_PCM
/** Set the line echo canceller (LEC) configuration for PCM.
  refer to \ref IFX_TAPI_LEC_PCM_CFG_SET */
#define IFXPHONE_LECCONF_PCM                       IFX_TAPI_LEC_PCM_CFG_SET
#endif

#ifndef IFXPHONE_GETLECCONF
/** Get the LEC configuration.
  refer to \ref IFX_TAPI_LEC_PHONE_CFG_GET */
#define IFXPHONE_GETLECCONF                        IFX_TAPI_LEC_PHONE_CFG_GET
#endif

#ifndef IFXPHONE_LECCONF
/** Set the line echo canceller (LEC) configuration.
  refer to \ref IFX_TAPI_LEC_PHONE_CFG_SET */
#define IFXPHONE_LECCONF                           IFX_TAPI_LEC_PHONE_CFG_SET
#endif

#ifndef IFXPHONE_SET_LINEFEED
/** This service sets the line feeding mode.
  refer to \ref IFX_TAPI_LINE_FEED_SET */
#define IFXPHONE_SET_LINEFEED                      IFX_TAPI_LINE_FEED_SET
#endif

#ifndef PHONE_HOOKSTATE
/** This service reads the hook status from the driver.
  refer to \ref IFX_TAPI_LINE_HOOK_STATUS_GET */
#define PHONE_HOOKSTATE                            IFX_TAPI_LINE_HOOK_STATUS_GET
#endif

#ifndef IFXPHONE_VALIDATION_TIME
/** Specifies the time for hook, pulse digit and hook flash validation.
  refer to \ref IFX_TAPI_LINE_HOOK_VT_SET */
#define IFXPHONE_VALIDATION_TIME                   IFX_TAPI_LINE_HOOK_VT_SET
#endif

#ifndef IFXPHONE_HIGH_LEVEL
/** This service enables or disables a high level path of a phone channel.
  refer to \ref IFX_TAPI_LINE_LEVEL_SET */
#define IFXPHONE_HIGH_LEVEL                        IFX_TAPI_LINE_LEVEL_SET
#endif

#ifndef IFXPHONE_DATA_ADD
/** This interface adds the data channel to an analog phone device.
  refer to \ref IFX_TAPI_MAP_DATA_ADD */
#define IFXPHONE_DATA_ADD                          IFX_TAPI_MAP_DATA_ADD
#endif

#ifndef IFXPHONE_DATA_REMOVE
/** This interface removes a data channel from an analog phone device.
  refer to \ref IFX_TAPI_MAP_DATA_REMOVE */
#define IFXPHONE_DATA_REMOVE                       IFX_TAPI_MAP_DATA_REMOVE
#endif

#ifndef IFXPHONE_PCM_ADD
/** This interface adds the PCM channel to an analog phone device.
  refer to \ref IFX_TAPI_MAP_PCM_ADD */
#define IFXPHONE_PCM_ADD                           IFX_TAPI_MAP_PCM_ADD
#endif

#ifndef IFXPHONE_PCM_REMOVE
/** This interface removes the PCM channel to an analog phone device.
  refer to \ref IFX_TAPI_MAP_PCM_REMOVE */
#define IFXPHONE_PCM_REMOVE                        IFX_TAPI_MAP_PCM_REMOVE
#endif

#ifndef IFXPHONE_PHONE_ADD
/** This interface adds the phone channel to another analog phone channel.
  refer to \ref IFX_TAPI_MAP_PHONE_ADD */
#define IFXPHONE_PHONE_ADD                         IFX_TAPI_MAP_PHONE_ADD
#endif

#ifndef IFXPHONE_PHONE_REMOVE
/** This interface removes a phone channel from an analog phone device.
  refer to \ref IFX_TAPI_MAP_PHONE_REMOVE */
#define IFXPHONE_PHONE_REMOVE                      IFX_TAPI_MAP_PHONE_REMOVE
#endif

#ifndef IFXPHONE_METER_CHAR
/** This service sets the characteristic for the metering service.
  refer to \ref IFX_TAPI_METER_CFG_SET */
#define IFXPHONE_METER_CHAR                        IFX_TAPI_METER_CFG_SET
#endif

#ifndef IFXPHONE_METER_START
/** This service starts the metering.
  refer to \ref IFX_TAPI_METER_START */
#define IFXPHONE_METER_START                       IFX_TAPI_METER_START
#endif

#ifndef IFXPHONE_METER_STOP
/** This service stops the metering.
  refer to \ref IFX_TAPI_METER_STOP */
#define IFXPHONE_METER_STOP                        IFX_TAPI_METER_STOP
#endif

#ifndef IFXPHONE_GET_PCM_ACTIVATION
/** This service gets the activation status of the PCM timeslots configured for
    this channel.
  refer to \ref IFX_TAPI_PCM_ACTIVATION_GET */
#define IFXPHONE_GET_PCM_ACTIVATION                IFX_TAPI_PCM_ACTIVATION_GET
#endif

#ifndef IFXPHONE_SET_PCM_ACTIVATION
/** This service activate / deactivates the PCM timeslots configured for this
    channel.
  refer to \ref IFX_TAPI_PCM_ACTIVATION_SET */
#define IFXPHONE_SET_PCM_ACTIVATION                IFX_TAPI_PCM_ACTIVATION_SET
#endif

#ifndef IFXPHONE_GET_PCM_CONFIG
/** This service gets the configuration of the PCM interface.
  refer to \ref IFX_TAPI_PCM_CFG_GET */
#define IFXPHONE_GET_PCM_CONFIG                    IFX_TAPI_PCM_CFG_GET
#endif

#ifndef IFXPHONE_SET_PCM_CONFIG
/** This service sets the configuration of the PCM interface.
  refer to \ref IFX_TAPI_PCM_CFG_SET */
#define IFXPHONE_SET_PCM_CONFIG                    IFX_TAPI_PCM_CFG_SET
#endif

#ifndef IFXPHONE_PCM_VOLUME
/** Sets the PCM interface volume settings.
  refer to \ref IFX_TAPI_PCM_VOLUME_SET */
#define IFXPHONE_PCM_VOLUME                        IFX_TAPI_PCM_VOLUME_SET
#endif

#ifndef IFXPHONE_VOLUME
/** Sets the speaker phone and microphone volume settings.
  refer to \ref IFX_TAPI_PHONE_VOLUME_SET */
#define IFXPHONE_VOLUME                            IFX_TAPI_PHONE_VOLUME_SET
#endif

#ifndef IFXPHONE_AAL_CONF
/** This interface configures AAL fields for a new connection.
  refer to \ref IFX_TAPI_PKT_AAL_CFG_SET */
#define IFXPHONE_AAL_CONF                          IFX_TAPI_PKT_AAL_CFG_SET
#endif

#ifndef IFXPHONE_AAL_PROFILE
/** AAL profile configuration.
  refer to \ref IFX_TAPI_PKT_AAL_PROFILE_SET */
#define IFXPHONE_AAL_PROFILE                       IFX_TAPI_PKT_AAL_PROFILE_SET
#endif

#ifndef PHONE_DTMF_OOB
/** This service controls the RFC2833 envents sending mode inband or out of band
  refer to \ref IFX_TAPI_PKT_EV_OOB_SET */
#define PHONE_DTMF_OOB                             IFX_TAPI_PKT_EV_OOB_SET
#endif

#ifndef IFXPHONE_RTCP_STATISTICS
/** Retrieves RTCP statistics.
  refer to \ref IFX_TAPI_PKT_RTCP_STATISTICS_GET */
#define IFXPHONE_RTCP_STATISTICS                IFX_TAPI_PKT_RTCP_STATISTICS_GET
#endif

#ifndef IFXPHONE_RTCP_RESET
/** Resets the RTCP statistics.
  refer to \ref IFX_TAPI_PKT_RTCP_STATISTICS_RESET */
#define IFXPHONE_RTCP_RESET                  IFX_TAPI_PKT_RTCP_STATISTICS_RESET
#endif

#ifndef IFXPHONE_RTP_CONF
/** This interface configures RTP and RTCP fields for a new connection.
  refer to \ref IFX_TAPI_PKT_RTP_CFG_SET */
#define IFXPHONE_RTP_CONF                       IFX_TAPI_PKT_RTP_CFG_SET
#endif

#ifndef IFXPHONE_RTP_PTCONF
/** Change the payload type table.
  refer to \ref IFX_TAPI_PKT_RTP_PT_CFG_SET */
#define IFXPHONE_RTP_PTCONF                     IFX_TAPI_PKT_RTP_PT_CFG_SET
#endif

#ifndef IFXPHONE_GET_PULSE_ASCII
/** This service reads the ASCII character representing the pulse digit out of
    the receive buffer.
  refer to \ref IFX_TAPI_PULSE_ASCII_GET */
#define IFXPHONE_GET_PULSE_ASCII                IFX_TAPI_PULSE_ASCII_GET
#endif

#ifndef PHONE_GET_PULSE_ASCII
/** This service reads the ASCII character representing the pulse digit out of
    the receive buffer.
  refer to \ref IFX_TAPI_PULSE_ASCII_GET */
#define PHONE_GET_PULSE_ASCII                   IFX_TAPI_PULSE_ASCII_GET
#endif

#ifndef IFXPHONE_GET_PULSE
/** This service reads the dial pulse digit out of the receive buffer.
  refer to \ref IFX_TAPI_PULSE_GET */
#define IFXPHONE_GET_PULSE                      IFX_TAPI_PULSE_GET
#endif

#ifndef IFXPHONE_PULSE_READY
/** This service checks if a pulse digit was received.
  refer to \ref IFX_TAPI_PULSE_READY */
#define IFXPHONE_PULSE_READY                    IFX_TAPI_PULSE_READY
#endif

#ifndef PHONE_RING
/** This service rings the phone.
  refer to \ref IFX_TAPI_RING */
#define PHONE_RING                              IFX_TAPI_RING
#endif

#ifndef IFXPHONE_RING_CADENCE_HIGH_RES
/** This service sets the high resolution ring cadence for the non-blocking
    ringing services.
    refer to \ref IFX_TAPI_RING_CADENCE_HR_SET */
#define IFXPHONE_RING_CADENCE_HIGH_RES          IFX_TAPI_RING_CADENCE_HR_SET
#endif

#ifndef PHONE_RING_CADENCE
/** This service sets the ring cadence for the non-blocking ringing services.
  refer to \ref IFX_TAPI_RING_CADENCE_SET */
#define PHONE_RING_CADENCE                      IFX_TAPI_RING_CADENCE_SET
#endif

#ifndef IFXPHONE_RING_GET_CONFIG
/** This service gets the ring configuration for the non-blocking ringing
    services.
  refer to \ref IFX_TAPI_RING_CFG_GET */
#define IFXPHONE_RING_GET_CONFIG                IFX_TAPI_RING_CFG_GET
#endif

#ifndef IFXPHONE_RING_CONFIG
/** This service sets the ring configuration for the non-blocking ringing
   services.
  refer to \ref IFX_TAPI_RING_CFG_SET */
#define IFXPHONE_RING_CONFIG                    IFX_TAPI_RING_CFG_SET
#endif

#ifndef PHONE_MAXRINGS
/** This service sets the maximum number of rings before a blocking ringing is
    started with service PHONE_RING .
    refer to \ref IFX_TAPI_RING_MAX_SET */
#define PHONE_MAXRINGS                          IFX_TAPI_RING_MAX_SET
#endif

#ifndef PHONE_RING_START
/** This service starts the non-blocking ringing on the phone line.
  refer to \ref IFX_TAPI_RING_START */
#define PHONE_RING_START                        IFX_TAPI_RING_START
#endif

#ifndef PHONE_RING_STOP
/** This service stops non-blocking ringing on the phone line which was
    started before with service PHONE_RING_START .
  refer to \ref IFX_TAPI_RING_STOP */
#define PHONE_RING_STOP                         IFX_TAPI_RING_STOP
#endif

#ifndef IFXPHONE_DISABLE_SIGDETECT
/** Disables the signal detection for Fax or modem signals.
  refer to \ref IFX_TAPI_SIG_DETECT_DISABLE */
#define IFXPHONE_DISABLE_SIGDETECT              IFX_TAPI_SIG_DETECT_DISABLE
#endif

#ifndef IFXPHONE_ENABLE_SIGDETECT
/** Enables the signal detection for Fax or modem signals.
  refer to \ref IFX_TAPI_SIG_DETECT_ENABLE */
#define IFXPHONE_ENABLE_SIGDETECT               IFX_TAPI_SIG_DETECT_ENABLE
#endif

#ifndef IFXPHONE_FAX_SETDEMOD
/** This service configures and enables the demodulator for a T.38 fax session.
  refer to \ref IFX_TAPI_T38_DEMOD_START */
#define IFXPHONE_FAX_SETDEMOD                   IFX_TAPI_T38_DEMOD_START
#endif

#ifndef IFXPHONE_FAX_SETMOD
/** This service configures and enables the modulator for a T.38 fax session.
  refer to \ref IFX_TAPI_T38_MOD_START */
#define IFXPHONE_FAX_SETMOD                     IFX_TAPI_T38_MOD_START
#endif

#ifndef IFXPHONE_FAX_STATUS
/** This service provides the T.38 fax status on query.
  refer to \ref IFX_TAPI_T38_STATUS_GET */
#define IFXPHONE_FAX_STATUS                     IFX_TAPI_T38_STATUS_GET
#endif

#ifndef IFXPHONE_FAX_DISABLE
/** This service disables the T.38 fax data pump and activates the voice path
    again.
    refer to \ref IFX_TAPI_T38_STOP */
#define IFXPHONE_FAX_DISABLE                    IFX_TAPI_T38_STOP
#endif

#ifndef PHONE_BUSY
/** Pre-defined tone services for busy tone.
  refer to \ref IFX_TAPI_TONE_BUSY_PLAY */
#define PHONE_BUSY                              IFX_TAPI_TONE_BUSY_PLAY
#endif

#ifndef IFXPHONE_CPTD_START
/** Start the call progress tone detection based on a previously defined simple
    tone.
  refer to \ref IFX_TAPI_TONE_CPTD_START */
#define IFXPHONE_CPTD_START                     IFX_TAPI_TONE_CPTD_START
#endif

#ifndef IFXPHONE_CPTD_STOP
/** Stops the call progress tone detection.
  refer to \ref IFX_TAPI_TONE_CPTD_STOP */
#define IFXPHONE_CPTD_STOP                      IFX_TAPI_TONE_CPTD_STOP
#endif

#ifndef PHONE_DIALTONE
/** Pre-defined tone services for dial tone.
  refer to \ref IFX_TAPI_TONE_DIALTONE_PLAY */
#define PHONE_DIALTONE                          IFX_TAPI_TONE_DIALTONE_PLAY
#endif

#ifndef PHONE_GET_DTMF_ASCII
/** This service reads the ASCII character representing the DTMF digit out
    of the receive buffer.
  refer to \ref IFX_TAPI_TONE_DTMF_ASCII_GET */
#define PHONE_GET_DTMF_ASCII                    IFX_TAPI_TONE_DTMF_ASCII_GET
#endif

#ifndef PHONE_GET_DTMF
/** This service reads the DTMF digit out of the internal receive buffer.
  refer to \ref IFX_TAPI_TONE_DTMF_GET */
#define PHONE_GET_DTMF                          IFX_TAPI_TONE_DTMF_GET
#endif

#ifndef PHONE_DTMF_READY
/** This service checks if a DTMF digit was received.
  refer to \ref IFX_TAPI_TONE_DTMF_READY_GET */
#define PHONE_DTMF_READY                        IFX_TAPI_TONE_DTMF_READY_GET
#endif

#ifndef IFXPHONE_SET_TONE_LEVEL
/** This service sets the tone level of the tone currently played.
  refer to \ref IFX_TAPI_TONE_LEVEL_SET */
#define IFXPHONE_SET_TONE_LEVEL                 IFX_TAPI_TONE_LEVEL_SET
#endif

#ifndef PHONE_PLAY_TONE
/** Plays a tone, which is defined before.
  refer to \ref IFX_TAPI_TONE_LOCAL_PLAY */
#define PHONE_PLAY_TONE                         IFX_TAPI_TONE_LOCAL_PLAY
#endif

#ifndef IFXPHONE_TONE_PLAY_NET
/** Plays a tone to the network side, which is defined before.
  refer to \ref IFX_TAPI_TONE_NET_PLAY */
#define IFXPHONE_TONE_PLAY_NET                  IFX_TAPI_TONE_NET_PLAY
#endif

#ifndef PHONE_GET_TONE_OFF_TIME
/** This service gets the current off-time duration which was set by a
    former PHONE_SET_TONE_OFF_TIME .
  refer to \ref IFX_TAPI_TONE_OFF_TIME_GET */
#define PHONE_GET_TONE_OFF_TIME                 IFX_TAPI_TONE_OFF_TIME_GET
#endif

#ifndef PHONE_SET_TONE_OFF_TIME
/** This service sets the off-time for a tone with a specified duration.
  refer to \ref IFX_TAPI_TONE_OFF_TIME_SET */
#define PHONE_SET_TONE_OFF_TIME                 IFX_TAPI_TONE_OFF_TIME_SET
#endif

#ifndef PHONE_GET_TONE_ON_TIME
/** This service gets the current on-time duration which was set by a
    former PHONE_SET_TONE_ON_TIME .
  refer to \ref IFX_TAPI_TONE_ON_TIME_GET */
#define PHONE_GET_TONE_ON_TIME                  IFX_TAPI_TONE_ON_TIME_GET
#endif

#ifndef PHONE_SET_TONE_ON_TIME
/** This service sets the on-time for a tone with a specified duration.
  refer to \ref IFX_TAPI_TONE_ON_TIME_SET */
#define PHONE_SET_TONE_ON_TIME                  IFX_TAPI_TONE_ON_TIME_SET
#endif

#ifndef PHONE_RINGBACK
/** Pre-defined tone services for ring back tone.
  refer to \ref IFX_TAPI_TONE_RINGBACK_PLAY */
#define PHONE_RINGBACK                          IFX_TAPI_TONE_RINGBACK_PLAY
#endif

#ifndef PHONE_GET_TONE_STATE
/** This service gets the tone playing state.
  refer to \ref IFX_TAPI_TONE_STATUS_GET */
#define PHONE_GET_TONE_STATE                    IFX_TAPI_TONE_STATUS_GET
#endif

#ifndef PHONE_CPT_STOP
/** Stops playback of the current tone (call progress tone), or cadence.
  refer to \ref IFX_TAPI_TONE_STOP */
#define PHONE_CPT_STOP                          IFX_TAPI_TONE_STOP
#endif

#ifndef IFXPHONE_TONE_TABLE_CONF
/** Configures a tone based on simple or composed tones.
  refer to \ref IFX_TAPI_TONE_TABLE_CFG_SET */
#define IFXPHONE_TONE_TABLE_CONF                IFX_TAPI_TONE_TABLE_CFG_SET
#endif

#ifndef IFXPHONE_TONETABLE_CONF
/** Configures a tone based on simple or composed tones.
  refer to \ref IFX_TAPI_TONE_TABLE_CFG_SET */
#define IFXPHONE_TONETABLE_CONF                 IFX_TAPI_TONE_TABLE_CFG_SET
#endif

#ifndef IFXPHONE_VERSION_CHECK
/** Check the supported TAPI interface version.
  refer to \ref IFX_TAPI_VERSION_CHECK */
#define IFXPHONE_VERSION_CHECK                  IFX_TAPI_VERSION_CHECK
#endif

#ifndef IFXPHONE_GET_VERSION
/** Retrieves the TAPI version string.
  refer to \ref IFX_TAPI_VERSION_GET */
#define IFXPHONE_GET_VERSION                    IFX_TAPI_VERSION_GET
#endif

/* ============================= */
/* Global Constants              */
/* ============================= */

/*
   #define <old_constant>        <new_constant>
*/

/** Switch the phone off */
#define TAPI_PHONE_VOLCONF_OFF                  IFX_TAPI_LINE_VOLUME_OFF

/** Switch the volume to low, -24 dB */
#define TAPI_PHONE_VOLCONF_LOW                  IFX_TAPI_LINE_VOLUME_LOW

/** Switch the volume to medium, 0 dB */
#define TAPI_PHONE_VOLCONF_MEDIUM               IFX_TAPI_LINE_VOLUME_MEDIUM

/** Switch the volume to high, 24 dB */
#define TAPI_PHONE_VOLCONF_HIGH                 IFX_TAPI_LINE_VOLUME_HIGH

/** LEC delay line maximum length */
#define TAPI_LEC_MAX_LEN                        IFX_TAPI_LEC_LEN_MAX

/** LEC delay line minimum length */
#define TAPI_LEC_MIN_LEN                        IFX_TAPI_LEC_LEN_MIN

/** maximum number of simple tones wich can be played in one go */
#define TAPI_MAX_SIMPLE_TONES                   IFX_TAPI_TONE_SIMPLE_MAX

/** Maximum tone generation steps, also called cadences */
#define TAPI_TONE_MAXSTEPS                      IFX_TAPI_TONE_STEPS_MAX

/** Tone is played out on default source */
#define TAPI_TONE_SRC_DEFAULT                   IFX_TAPI_TONE_SRC_DEFAULT

/** Tone is played out on DSP, default, if available */
#define TAPI_TONE_SRC_DSP                       IFX_TAPI_TONE_SRC_DSP

/** Tone is played out on local tone generator in the analog part of the
 device. Default if DSP is not available */
#define TAPI_TONE_SRC_TG                        IFX_TAPI_TONE_SRC_TG

/** Tone minimum index which can be configured by user */
#define TAPI_TONE_INDEX_MIN                     IFX_TAPI_TONE_INDEX_MIN

/** Tone maximum index which can be configured by user */
#define TAPI_TONE_INDEX_MAX                     IFX_TAPI_TONE_INDEX_MAX

/** Dial Fifo Size */
#define TAPI_DIAL_FIFO_SIZE                     IFX_TAPI_PULSE_FIFO_SIZE

/** DTMF Fifo Size */
#define TAPI_DTMF_FIFO_SIZE                     IFX_TAPI_DTMF_FIFO_SIZE

/** CID Rx Fifo Size */
#define TAPI_CID_RX_FIFO_SIZE                   IFX_TAPI_CID_RX_FIFO_SIZE

/* maximum cid data */
/**
   CID Tx maximum buffer size.

   \remarks

   -  ETSI  :
      call setup cmd : 2,  cli : 22, date/time : 10, name : 52,
      redir num : 22, checksum : 1 => 109 Bytes max in CID buffer
   -  NTT :
      DLE : 3, SOH : 1, Header : 1, STX : 1, ETX : 1, DATA: 119, CRC : 2
      => 128 Bytes max in CID Buffer
*/
#define TAPI_MAX_CID_PARAMETER                  IFX_TAPI_CID_SIZE_MAX

/** Length of a CID message (in characters) */
#define TAPI_CID_MAX_MSG_LEN                    IFX_TAPI_CID_MSG_LEN_MAX

/** Maximum number of Codecs */
#define MAX_CODECS                              IFX_TAPI_ENC_TYPE_MAX

/** maximum cadences bytes */
#define TAPI_MAX_CADENCE_BYTES                  IFX_TAPI_RING_CADENCE_MAX_BYTES

/* ============================= */
/* Global Enums                  */
/* ============================= */

/*
   enum _<old_enum_name> { old_enum_1 = new_enum_1, ...};
   typedef enum _<new_enum_prefix>       <old_enum_name>;
*/

/** Lists the ports for the capability list. */
enum _TAPI_PHONE_PORTS
{
   pots = IFX_TAPI_CAP_PORT_POTS,
   pstn = IFX_TAPI_CAP_PORT_PSTN,
   handset = IFX_TAPI_CAP_PORT_HANDSET,
   speaker = IFX_TAPI_CAP_PORT_SPEAKER
};
typedef enum _IFX_TAPI_CAP_PORT           TAPI_PHONE_PORTS;

/** Lists the signal detectors for the capability list. */
enum _TAPI_PHONE_CAP_SIGDETECT
{
   /** Signal detection for CNG is available */
   cng = IFX_TAPI_CAP_SIG_DETECT_CNG,
   /** Signal detection for CED is available */
   ced = IFX_TAPI_CAP_SIG_DETECT_CED,
   /** Signal detection for DIS is available */
   dis = IFX_TAPI_CAP_SIG_DETECT_DIS,
   /** Signal detection for line power is available */
   power = IFX_TAPI_CAP_SIG_DETECT_POWER,
   /** Signal detection for CPT is available */
   cpt = IFX_TAPI_CAP_SIG_DETECT_CPT,
   /** Signal detection for V8.bis is available */
   v8bis = IFX_TAPI_CAP_SIG_DETECT_V8BIS
};
typedef enum _IFX_TAPI_CAP_SIG_DETECT     TAPI_PHONE_CAP_SIGDETECT;

/** Enumeration used for phone capabilities types. */
enum _TAPI_PHONE_CAP
{
   vendor = IFX_TAPI_CAP_TYPE_VENDOR,
   device = IFX_TAPI_CAP_TYPE_DEVICE,
   port = IFX_TAPI_CAP_TYPE_PORT,
   codec = IFX_TAPI_CAP_TYPE_CODEC,
   dsp = IFX_TAPI_CAP_TYPE_DSP,
   pcm = IFX_TAPI_CAP_TYPE_PCM,
   codecs = IFX_TAPI_CAP_TYPE_CODECS,
   phones = IFX_TAPI_CAP_TYPE_PHONES,
   sigdetect = IFX_TAPI_CAP_TYPE_SIGDETECT,
   t38 = IFX_TAPI_CAP_TYPE_T38
};
typedef enum _IFX_TAPI_CAP_TYPE           TAPI_PHONE_CAP;

/** List of ABSCLI/ABSNAME settings. */
enum _TAPI_CID_ABSREASON_t
{
   /** Unavailable */
   TAPI_CID_ABSREASON_UNAV = IFX_TAPI_CID_ABSREASON_UNAV,
   /** Private */
   TAPI_CID_ABSREASON_PRIV = IFX_TAPI_CID_ABSREASON_PRIV
};
typedef enum _IFX_TAPI_CID_ABSREASON      TAPI_CID_ABSREASON_t;

/** ETSI ALERTS */
enum _TAPI_CID_ETSI_ALERT_t
{
   /** First Ring Burst.
      Note: Defined only for CID transmission associated with ringing.
            Ringing timings are set via \ref TAPI_RING_CADENCE */
   TAPI_CID_ALERT_ETSI_FR = IFX_TAPI_CID_ALERT_ETSI_FR,
   /** DTAS. */
   TAPI_CID_ALERT_ETSI_DTAS = IFX_TAPI_CID_ALERT_ETSI_DTAS,
   /** Ring Pulse. */
   TAPI_CID_ALERT_ETSI_RP = IFX_TAPI_CID_ALERT_ETSI_RP,
   /** Line Reversal (alias Polarity Reversal) followed by DTAS. */
   TAPI_CID_ALERT_ETSI_LRDTAS = IFX_TAPI_CID_ALERT_ETSI_LRDTAS
};
typedef enum _IFX_TAPI_CID_ALERT_ETSI     TAPI_CID_ETSI_ALERT_t;

/** Caller ID transmission modes. */
enum _TAPI_CID_HOOK_MODE_t
{
   /** On-hook transmission. Applicable to CID type 1 and MWI */
   TAPI_CID_ONHOOK = IFX_TAPI_CID_HM_ONHOOK,
   /** Off-hook transmission. Applicable to CID type 2 and MWI */
   TAPI_CID_OFFHOOK = IFX_TAPI_CID_HM_OFFHOOK
};
typedef enum _IFX_TAPI_CID_HOOK_MODE      TAPI_CID_HOOK_MODE_t;

/** Caller ID message types (defined in ETSI EN 300 659-3). */
enum _TAPI_CID_MESSAGE_TYPE_t
{
   /** Call Set-up. Corresponds to Caller ID type 1 and type 2. */
   TAPI_CID_MT_CSUP = IFX_TAPI_CID_MT_CSUP,
   /** Message Waiting Indicator */
   TAPI_CID_MT_MWI = IFX_TAPI_CID_MT_MWI,
   /** Advice of Charge */
   TAPI_CID_MT_AOC = IFX_TAPI_CID_MT_AOC,
   /** Short Message Service */
   TAPI_CID_MT_SMS = IFX_TAPI_CID_MT_SMS,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES01 = IFX_TAPI_CID_MT_RES01,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES02 = IFX_TAPI_CID_MT_RES02,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES03 = IFX_TAPI_CID_MT_RES03,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES04 = IFX_TAPI_CID_MT_RES04,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES05 = IFX_TAPI_CID_MT_RES05,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES06 = IFX_TAPI_CID_MT_RES06,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES07 = IFX_TAPI_CID_MT_RES07,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES08 = IFX_TAPI_CID_MT_RES08,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES09 = IFX_TAPI_CID_MT_RES09,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0A = IFX_TAPI_CID_MT_RES0A,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0B = IFX_TAPI_CID_MT_RES0B,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0C = IFX_TAPI_CID_MT_RES0C,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0D = IFX_TAPI_CID_MT_RES0D,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0E = IFX_TAPI_CID_MT_RES0E,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0F = IFX_TAPI_CID_MT_RES0F
};
typedef enum _IFX_TAPI_CID_MSG_TYPE       TAPI_CID_MESSAGE_TYPE_t;

/** CID Receiver Errors. */
enum _TAPI_CIDRX_ERR
{
   /** No Error during CID Receiver operation */
   TAPI_CIDRX_ERR_NOERR = IFX_TAPI_CID_RX_ERROR_NONE,
   /** Reading error during CID Receiver operation */
   TAPI_CIDRX_ERR_READ = IFX_TAPI_CID_RX_ERROR_READ
};
typedef enum _IFX_TAPI_CID_RX_ERROR       TAPI_CIDRX_ERR;

/******/

/** CID Receiver Status. */
enum _TAPI_CIDRX_STAT
{
   /** CID Receiver is not active */
   TAPI_CIDRX_STAT_INACTIVE = IFX_TAPI_CID_RX_STATE_INACTIVE,
   /** CID Receiver is active */
   TAPI_CIDRX_STAT_ACTIVE = IFX_TAPI_CID_RX_STATE_ACTIVE,
   /** CID Receiver is just receiving data */
   TAPI_CIDRX_STAT_ONGOING = IFX_TAPI_CID_RX_STATE_ONGOING,
   /** CID Receiver is completed */
   TAPI_CIDRX_STAT_DATA_RDY = IFX_TAPI_CID_RX_STATE_DATA_READY
};
typedef enum _IFX_TAPI_CID_RX_STATE      TAPI_CIDRX_STAT;

/** Caller ID Services (defined in ETSI EN 300 659-3). */
enum _TAPI_CID_SERVICE_TYPE_t
{
   /** date and time presentation */
   TAPI_CID_ST_DATE = IFX_TAPI_CID_ST_DATE,
   /** calling line identity (mandatory) */
   TAPI_CID_ST_CLI = IFX_TAPI_CID_ST_CLI,
   /** called line identity */
   TAPI_CID_ST_CDLI = IFX_TAPI_CID_ST_CDLI,
   /** reason for absence of CLI */
   TAPI_CID_ST_ABSCLI = IFX_TAPI_CID_ST_ABSCLI,
   /** calling line name */
   TAPI_CID_ST_NAME = IFX_TAPI_CID_ST_NAME,
   /** reason for absence of name */
   TAPI_CID_ST_ABSNAME = IFX_TAPI_CID_ST_ABSNAME,
   /** visual indicator */
   TAPI_CID_ST_VISINDIC = IFX_TAPI_CID_ST_VISINDIC,
   /** Message Identification */
   TAPI_CID_ST_MSGIDENT = IFX_TAPI_CID_ST_MSGIDENT,
   /** Last Message CLI */
   TAPI_CID_ST_LMSGCLI = IFX_TAPI_CID_ST_LMSGCLI,
   /** Complementary Date and Time */
   TAPI_CID_ST_CDATE = IFX_TAPI_CID_ST_CDATE,
   /** complementary calling line identity */
   TAPI_CID_ST_CCLI = IFX_TAPI_CID_ST_CCLI,
   /** call type */
   TAPI_CID_ST_CT = IFX_TAPI_CID_ST_CT,
   /** first called line identity  */
   TAPI_CID_ST_FIRSTCLI = IFX_TAPI_CID_ST_FIRSTCLI,
   /** number of messages  */
   TAPI_CID_ST_MSGNR = IFX_TAPI_CID_ST_MSGNR,
   /** type of forwarded call  */
   TAPI_CID_ST_FWCT = IFX_TAPI_CID_ST_FWCT,
   /** type of calling user  */
   TAPI_CID_ST_USRT = IFX_TAPI_CID_ST_USRT,
   /** number redirection */
   TAPI_CID_ST_REDIR = IFX_TAPI_CID_ST_REDIR,
   /** charge */
   TAPI_CID_ST_CHARGE = IFX_TAPI_CID_ST_CHARGE,
   /** additional charge */
   TAPI_CID_ST_ACHARGE = IFX_TAPI_CID_ST_ACHARGE,
   /** duration of the call */
   TAPI_CID_ST_DURATION = IFX_TAPI_CID_ST_DURATION,
   /** network provider id */
   TAPI_CID_ST_NTID = IFX_TAPI_CID_ST_NTID,
   /** carrier identity */
   TAPI_CID_ST_CARID = IFX_TAPI_CID_ST_CARID,
   /** selection of terminal function */
   TAPI_CID_ST_TERMSEL = IFX_TAPI_CID_ST_TERMSEL,
   /** display information, used as INFO for DTMF */
   TAPI_CID_ST_DISP = IFX_TAPI_CID_ST_DISP,
   /** Service Information */
   TAPI_CID_ST_SINFO = IFX_TAPI_CID_ST_SINFO,
   /** extension for operator use */
   TAPI_CID_ST_XOPUSE = IFX_TAPI_CID_ST_XOPUSE
};
typedef enum _IFX_TAPI_CID_SERVICE_TYPE      TAPI_CID_SERVICE_TYPE_t;

/**  CID standards */
enum _TAPI_CID_STD_t
{
   /** Bellcore/Telcordia GR-30-CORE.
      Using Bell202 FSK coding of CID information. */
   TAPI_CID_STD_TELCORDIA = IFX_TAPI_CID_STD_TELCORDIA,
   /** ETSI 300-659-1/2/3 V1.3.1.
      Using V.23 FSK coding to transmit CID information. */
   TAPI_CID_STD_ETSI_FSK = IFX_TAPI_CID_STD_ETSI_FSK,
   /** ETSI 300-659-1/2/3 V1.3.1.
      Using DTMF transmission of CID information. */
   TAPI_CID_STD_ETSI_DTMF = IFX_TAPI_CID_STD_ETSI_DTMF,
   /** SIN 227 Issue 3.4.
      Using V.23 FSK coding of CID information. */
   TAPI_CID_STD_SIN = IFX_TAPI_CID_STD_SIN,
   /** NTT standard: TELEPHONE SERVICE INTERFACES, Edition 5.
      Using a modified V.23 FSK coding of CID information. */
   TAPI_CID_STD_NTT = IFX_TAPI_CID_STD_NTT
};
typedef enum _IFX_TAPI_CID_STD                  TAPI_CID_STD_t;

/** List of VMWI settings. */
enum _TAPI_CID_VMWI_t
{
   /** Message waiting disabled */
   TAPI_CID_VMWI_DIS = IFX_TAPI_CID_VMWI_DIS,
   /** Message waiting enabled */
   TAPI_CID_VMWI_EN = IFX_TAPI_CID_VMWI_EN
};
typedef enum _IFX_TAPI_CID_VMWI                 TAPI_CID_VMWI_t;

/** Enumeration for dial status events. */
enum _TAPI_DIALING
{
   /** DTMF sign detected */
   TAPI_DIALINGSTATUS_DTMF = IFX_TAPI_DIALING_STATUS_DTMF,
   /** pulse digit detected */
   TAPI_DIALINGSTATUS_PULSE = IFX_TAPI_DIALING_STATUS_PULSE
};
typedef enum _IFX_TAPI_DIALING_STATUS            TAPI_DIALING;

/** Specifies the Enable/Disable mode of the AGC resource. */
enum _TAPI_AGC_MODE
{
   TAPI_AGC_DISABLE = IFX_TAPI_ENC_AGC_MODE_DISABLE,
   TAPI_AGC_ENABLE = IFX_TAPI_ENC_AGC_MODE_ENABLE
};
typedef enum _IFX_TAPI_ENC_AGC_MODE       TAPI_AGC_MODE;

/** Possible codecs to select for PHONE_REC_CODEC and TAPI_AAL_PROFILE. */
enum _TAPI_PHONE_CODEC
{
   /** G723, 6.3 kBit/s */
   G723_63 = IFX_TAPI_ENC_TYPE_G723_63,
   /** G723, 5.3 kBit/s */
   G723_53 = IFX_TAPI_ENC_TYPE_G723_53,
   /** G728, 16 kBit/s */
   G728 = IFX_TAPI_ENC_TYPE_G728,
   /** G729 A and B, 8 kBit/s */
   G729 = IFX_TAPI_ENC_TYPE_G729,
   /** G711 u-Law, 64 kBit/s */
   MLAW = IFX_TAPI_ENC_TYPE_MLAW,
   /** G711 A-Law, 64 kBit/s */
   ALAW = IFX_TAPI_ENC_TYPE_ALAW,
   /** G726, 16 kBit/s */
   G726_16 = IFX_TAPI_ENC_TYPE_G726_16,
   /** G726, 24 kBit/s */
   G726_24 = IFX_TAPI_ENC_TYPE_G726_24,
   /** G726, 32 kBit/s */
   G726_32 = IFX_TAPI_ENC_TYPE_G726_32,
   /** G726, 40 kBit/s */
   G726_40 = IFX_TAPI_ENC_TYPE_G726_40,
   /** G729 E, 11.8 kBit/s */
   G729_E = IFX_TAPI_ENC_TYPE_G729_E,
};
typedef enum _IFX_TAPI_ENC_TYPE                       TAPI_PHONE_CODEC;

/** Enumeration used for ioctl PHONE_VAD. */
enum _TAPI_VAD
{
   /** No voice activity detection */
   TAPI_VAD_NOVAD = IFX_TAPI_ENC_VAD_NOVAD,
   /** Voice activity detection on, in this case also comfort noise and
      spectral information (nicer noise) is switched on */
   TAPI_VAD_VAD_ON = IFX_TAPI_ENC_VAD_ON,
   /** Voice activity detection on with comfort noise generation without
       spectral information */
   TAPI_VAD_G711 = IFX_TAPI_ENC_VAD_G711
};
typedef enum _IFX_TAPI_ENC_VAD                        TAPI_VAD;

/** Country selection for TAPI_Init. */
enum _TAPI_COUNTRY
{
   /** Default contry */
   TAPI_COUNTRY_DEFAULT = IFX_TAPI_INIT_COUNTRY_DEFAULT,
   /** Germany */
   TAPI_COUNTRY_DE = IFX_TAPI_INIT_COUNTRY_DE,
   /** USA */
   TAPI_COUNTRY_US = IFX_TAPI_INIT_COUNTRY_US,
   /** United Kingdom */
   TAPI_COUNTRY_UK = IFX_TAPI_INIT_COUNTRY_UK
};
typedef enum _IFX_TAPI_INIT_COUNTRY                   TAPI_COUNTRY;

/** TAPI Initialization modes, selection for target system. */
enum _TAPI_INIT_MODES
{
   /** Default initialization */
   TAPI_DEFAULT = 		IFX_TAPI_INIT_MODE_DEFAULT,
   /** Typical VoIP solution. Phone connected to a packet coder (data channel)
    with DSP features for signal detection */
   TAPI_VOICE_CODER = 	IFX_TAPI_INIT_MODE_VOICE_CODER,
   /** Phone to PCM using DSP features for signal detection */
   TAPI_PCM_DSP = 		IFX_TAPI_INIT_MODE_PCM_DSP,
   /** Phone to PCM connection without DSP features */
   TAPI_PCM_PHONE = 	IFX_TAPI_INIT_MODE_PCM_PHONE,
   /** No special initialization, for testing purposes */
   TAPI_NONE = 			IFX_TAPI_INIT_MODE_NONE
};
typedef enum _IFX_TAPI_INIT_MODE                     TAPI_INIT_MODES;

/** Jitter buffer adoption. */
enum _TAPI_LOCAL_ADAPT
{
   /** Local Adaptation OFF */
   LOCAL_ADAPT_OFF = IFX_TAPI_JB_LOCAL_ADAPT_OFF,
   /** Local adaptation ON */
   LOCAL_ADAPT_ON = IFX_TAPI_JB_LOCAL_ADAPT_ON,
   /** Local Adaptation ON
      with Sample Interpollation */
   LOCAL_ADAPT_SI_ON = IFX_TAPI_JB_LOCAL_ADAPT_SI_ON
};
typedef enum _IFX_TAPI_JB_LOCAL_ADAPT              TAPI_LOCAL_ADAPT;

/** Defines for linefeeding. */
enum _TAPI_LINEFEED
{
   /** Normal feeding mode for phone off hook */
   TAPI_LINEFEED_NORMAL = IFX_TAPI_LINE_FEED_ACTIVE,
   /** Normal feeding mode for phone off hook reversed */
   TAPI_LINEFEED_REVERSED = IFX_TAPI_LINE_FEED_ACTIVE_REV,
   /** Power down resistance = IFX_TAPI_LINE_FEED_resistance hook with hook detection */
   TAPI_LINEFEED_PARKED = IFX_TAPI_LINE_FEED_STANDBY,
   /** Switch off the line, but the device is able to test the line,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_HIGH_IMPEDANCE = IFX_TAPI_LINE_FEED_HIGH_IMPEDANCE,
   /** Switch off the line and the device */
   TAPI_LINEFEED_POWER_DOWN = IFX_TAPI_LINE_FEED_DISABLED,
   /** not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_GROUND_START = IFX_TAPI_LINE_FEED_GROUND_START,
   /** Thresholds for automatic battery switch are set via
   coefficient settings, not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_NORMAL_AUTO = IFX_TAPI_LINE_FEED_NORMAL_AUTO,
   /** Thresholds for automatic battery switch are set via coefficient
   settings reversed, not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_REVERSED_AUTO = IFX_TAPI_LINE_FEED_REVERSED_AUTO,
   /** feeding mode for phone off hook with low battery to save power */
   TAPI_LINEFEED_NORMAL_LOW = IFX_TAPI_LINE_FEED_NORMAL_LOW,
   /** feeding mode for phone off hook with low battery to save power
       and reserved polarity */
   TAPI_LINEFEED_REVERSED_LOW = IFX_TAPI_LINE_FEED_REVERSED_LOW,
   /** Reserved, needed for ring call back function */
   TAPI_LINEFEED_RING_BURST = IFX_TAPI_LINE_FEED_RING_BURST,
   /** Reserved, needed for ring call back function */
   TAPI_LINEFEED_RING_PAUSE = IFX_TAPI_LINE_FEED_RING_PAUSE,
   /** Reserved, needed for internal function,
       not supported for VINETIC 2CPE, Revision 2.1  */
   TAPI_LINEFEED_METER = IFX_TAPI_LINE_FEED_METER,
   /** Reserved, special test linemode,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_ACTIVE_LOW = IFX_TAPI_LINE_FEED_ACTIVE_LOW,
   /** Reserved, special test linemode,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_ACTIVE_BOOSTED = IFX_TAPI_LINE_FEED_ACTIVE_BOOSTED,
   /** Reserved, special linemode for S-MAX Slic,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_ACT_T = IFX_TAPI_LINE_FEED_ACT_T,
   /** Reserved, special linemode for S-MAX Slic,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_POWER_DOWN_RESISTIVE_SWITCH = IFX_TAPI_LINE_FEED_DISABLED_RESISTIVE_SWITCH,
   /** Power down resistance = IFX_TAPI_LINE_FEED_resistance hook with hook detection,
       not supported for VINETIC 2CPE, Revision 2.1 */
   TAPI_LINEFEED_PARKED_REVERSED = IFX_TAPI_LINE_FEED_PARKED_REVERSED
};
typedef enum _IFX_TAPI_LINE_FEED                      TAPI_LINEFEED;

/** Enumeration for hook status events. */
enum _TAPI_HOOKSTATUS
{
   /** Hook detected */
   TAPI_HOOKSTATUS_HOOK = IFX_TAPI_LINE_HOOK_STATUS_HOOK,
   /** Hook flash detected */
   TAPI_HOOKSTATUS_FLASH = IFX_TAPI_LINE_HOOK_STATUS_FLASH,
   /** Detected hook event is an Offhook */
   TAPI_HOOKSTATUS_OFFHOOK = IFX_TAPI_LINE_HOOK_STATUS_OFFHOOK
};
typedef enum _IFX_TAPI_LINE_HOOK_STATUS         TAPI_HOOKSTATUS;

/** Validation types used for structure TAPI_VALIDATION_TIMES. */
enum _TAPI_VALIDATION_TYPES
{
   /** Settings for hook validation, if the time matches between nMinTime and
      nMaxTime an exception is raised. */
   HOOKOFF_TIME = IFX_TAPI_LINE_HOOK_VT_HOOKOFF_TIME,
   /** Settings for hook validation, if the time matches between nMinTime and
       nMaxTime an exception is raised. */
   HOOKON_TIME = IFX_TAPI_LINE_HOOK_VT_HOOKON_TIME,
   /** Settings for hook flash validation also known as register recall.
       If the time matches between the time defined in the fields nMinTime
       and nMaxTime an exception is raised */
   HOOKFLASH_TIME = IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME,
   /** Settings for pulse digit low, open loop and make validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   DIGITLOW_TIME = IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME,
   /** Settings for pulse digit high, close loop and break validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   DIGITHIGH_TIME = IFX_TAPI_LINE_HOOK_VT_DIGITHIGH_TIME,
   /** Settings for pulse digit pause. The time must match the time defined
       in the fields nMinTime and nMaxTime to recognize it as
       pulse dialing event */
   INTERDIGIT_TIME = IFX_TAPI_LINE_HOOK_VT_INTERDIGIT_TIME
};
typedef enum _IFX_TAPI_LINE_HOOK_VALIDATION_TYPE      TAPI_VALIDATION_TYPES;

/** Specifies the Enable/Disable high level mode (for howler tone) */
enum _TAPI_HIGH_LEVEL
{
   TAPI_HIGH_LEVEL_DISABLE = IFX_TAPI_LINE_LEVEL_DISABLE,
   TAPI_HIGH_LEVEL_ENABLE  =  IFX_TAPI_LINE_LEVEL_ENABLE
};
typedef enum _IFX_TAPI_LINE_LEVEL         TAPI_HIGH_LEVEL;

/** Enumeration for phone line status information. */
enum _TAPI_LINESTATUS
{
   /** line is ringing */
   TAPI_LINESTATUS_RINGING = IFX_TAPI_LINE_STATUS_RINGING,
   /** ringing finished */
   TAPI_LINESTATUS_RINGFINISHED = IFX_TAPI_LINE_STATUS_RINGFINISHED,
   /** fax detcted -> is replaced by signal */
   TAPI_LINESTATUS_FAX = IFX_TAPI_LINE_STATUS_FAX,
   /** ground key detected */
   TAPI_LINESTATUS_GNDKEY = IFX_TAPI_LINE_STATUS_GNDKEY,
   /** ground key high detected */
   TAPI_LINESTATUS_GNDKEYHIGH = IFX_TAPI_LINE_STATUS_GNDKEYHIGH,
   /** overtemperature detected */
   TAPI_LINESTATUS_OTEMP = IFX_TAPI_LINE_STATUS_OTEMP,
   /** ground key polarity detected */
   TAPI_LINESTATUS_GNDKEYPOL = IFX_TAPI_LINE_STATUS_GNDKEYPOL,
   /** GR909 result is available and can be queried with the appropriate
       interface */
   TAPI_LINESTATUS_GR909RES = IFX_TAPI_LINE_STATUS_GR909RES,
   /** caller id received */
   TAPI_LINESTATUS_CIDRX = IFX_TAPI_LINE_STATUS_CIDRX,
   /** Event occurs when the line mode may be switched to
       TAPI_LINEFEED_NORMAL_LOW to save power */
   TAPI_LINESTATUS_FEEDLOWBATT = IFX_TAPI_LINE_STATUS_FEEDLOWBATT
};
typedef enum _IFX_TAPI_LINE_STATUS              TAPI_LINESTATUS;

/** Start/Stop information for data channel mapping. */
enum _TAPI_DATA_MAPPING_START_STOP
{
   /** Do not modify the status of the recorder */
   TAPI_DATA_MAPPING_UNCHANGED = IFX_TAPI_MAP_DATA_UNCHANGED,
   /** Recording is started */
   TAPI_DATA_MAPPING_START = IFX_TAPI_MAP_DATA_START,
   /** Recording is stopped */
   TAPI_DATA_MAPPING_STOP = IFX_TAPI_MAP_DATA_STOP
};
typedef enum _IFX_TAPI_MAP_DATA_START_STOP      TAPI_DATA_MAPPING_START_STOP;

/** Data channel destination types (conferencing). */
enum _TAPI_DATA_MAPTYPE
{
   TAPI_DATA_MAPTYPE_DEFAULT = IFX_TAPI_MAP_DATA_TYPE_DEFAULT,
   TAPI_DATA_MAPTYPE_CODER = IFX_TAPI_MAP_DATA_TYPE_CODER,
   TAPI_DATA_MAPTYPE_PCM = IFX_TAPI_MAP_DATA_TYPE_PCM,
   TAPI_DATA_MAPTYPE_PHONE = IFX_TAPI_MAP_DATA_TYPE_PHONE
};
typedef enum _IFX_TAPI_MAP_DATA_TYPE            TAPI_DATA_MAPTYPE;

/** Play out enabling and disabling information. */
enum _TAPI_MAPPLAY
{
   /** Do not modify playing status */
   TAPI_MAPPLAY_NOMODIF = IFX_TAPI_MAP_DEC_NONE,
   /** Start playing after mapping */
   TAPI_MAPPLAY_START = IFX_TAPI_MAP_DEC_START,
   /** Stop playing after mapping */
   TAPI_MAPPLAY_STOP = IFX_TAPI_MAP_DEC_STOP
};
typedef enum _IFX_TAPI_MAP_DEC                  TAPI_MAPPLAY;

/** Recording enabling and disabling information. */
enum _TAPI_MAPREC
{
   /** Do not modify recording status */
   TAPI_MAPREC_NOMODIF = IFX_TAPI_MAP_ENC_NONE,
   /** Start recording after mapping */
   TAPI_MAPREC_START = IFX_TAPI_MAP_ENC_START,
   /** Stop recording after mapping */
   TAPI_MAPREC_STOP = IFX_TAPI_MAP_ENC_STOP
};
typedef enum _IFX_TAPI_MAP_ENC                  TAPI_MAPREC;

/** Type channel for mapping. */
enum _TAPI_MAPTYPE
{
   /** Default. It depends on the device and configures the best applicable */
   TAPI_MAPTYPE_DEFAULT = IFX_TAPI_MAP_TYPE_DEFAULT,
   /** Type is a coder packet channel */
   TAPI_MAPTYPE_CODER = IFX_TAPI_MAP_TYPE_CODER,
   /** Type is a PCM channel */
   TAPI_MAPTYPE_PCM = IFX_TAPI_MAP_TYPE_PCM
};
typedef enum _IFX_TAPI_MAP_TYPE                 TAPI_MAPTYPE;

/** PCM resource configuration */
enum _PCM_CONFIG_RES
{
   /** A-Law 8 bit */
   A_LAW_8_BIT = IFX_TAPI_PCM_RES_ALAW_8BIT,
   /** u-Law 8 bit */
   U_LAW_8_BIT = IFX_TAPI_PCM_RES_ULAW_8BIT,
   /** Linear 16 bit */
   LINEAR_16_BIT = IFX_TAPI_PCM_RES_LINEAR_16BIT
};
typedef enum _IFX_TAPI_PCM_RES                  PCM_CONFIG_RES;

/** Used for TAPI_AAL_PROFILE in case one coder range. */
enum _TAPI_AAL_PROFILE_RANGE
{
   /** One range from 0 to 15 */
   RANGE_0_15 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_15,
   /** Range from 0 to 7 for a two range profile entry */
   RANGE_0_7 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7,
   /** Range from 8 to 15 for a two range profile entry */
   RANGE_8_15 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15,
   /** Range from 0 to 3 for a four range profile entry */
   RANGE_0_3 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_3,
   /** Range from 4 to 7 for a four range profile entry */
   RANGE_4_7 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7,
   /** Range from 8 to 11 for a four range profile entry */
   RANGE_8_11 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11,
   /** Range from 12 to 15 for a four range profile entry */
   RANGE_12_15 = IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15
};
typedef enum _IFX_TAPI_PKT_AAL_PROFILE_RANGE    TAPI_AAL_PROFILE_RANGE;

/** Enumeration for TAPI_RTP_CONF event setting. */
enum _TAPI_RTPEV
{
   /** Device default setting */
   TAPI_RTPEV_DEFAULT = IFX_TAPI_PKT_EV_OOB_DEFAULT,
   /** No events, only voice is transmitted */
   TAPI_RTPEV_NO = IFX_TAPI_PKT_EV_OOB_NO,
   /** Only events, no voice is transmitted. Not yet implemented */
   TAPI_RTPEV_ONLY = IFX_TAPI_PKT_EV_OOB_ONLY,
   /** Events and voice is transmitted */
   TAPI_RTPEV_ALL = IFX_TAPI_PKT_EV_OOB_ALL
};
typedef enum _IFX_TAPI_PKT_EV_OOB               TAPI_RTPEV;

/** Ringing configuration mode */
enum _RING_CONFIG_MODE
{
   /** Internal balanced */
   INTERNAL_BALANCED = IFX_TAPI_RING_CFG_MODE_INTERNAL_BALANCED,
   /** Internal unbalanced ROT */
   INTERNAL_UNBALANCED_ROT = IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROT,
   /** Internal unbalanced ROR */
   INTERNAL_UNBALANCED_ROR = IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROR,
   /** External SLIC current sense */
   EXTERNAL_IT_CS = IFX_TAPI_RING_CFG_MODE_EXTERNAL_IT_CS,
   /** External IO current sense */
   EXTERNAL_IO_CS = IFX_TAPI_RING_CFG_MODE_EXTERNAL_IO_CS
};
typedef enum _IFX_TAPI_RING_CFG_MODE RING_CONFIG_MODE;

/** Ringing configuration submode */
enum _RING_CONFIG_SUBMODE
{
   /** DC Ring Trip standard */
   DC_RNG_TRIP_STANDARD = IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_STANDARD,
   /** DC Ring Trip fast */
   DC_RNG_TRIP_FAST = IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_FAST,
   /** AC Ring Trip standard */
   AC_RNG_TRIP_STANDARD = IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_STANDARD,
   /** AC Ring Trip fast */
   AC_RNG_TRIP_FAST = IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_FAST
};
typedef enum _IFX_TAPI_RING_CFG_SUBMODE RING_CONFIG_SUBMODE;

/** List the tone detection options. */
enum _TAPI_SIGNAL
{
   /** no signal detected */
   TAPI_SIGNAL_NONE = IFX_TAPI_SIG_NONE,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      receive path */
   TAPI_SIGNAL_DISRX = IFX_TAPI_SIG_DISRX,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      transmit path */
   TAPI_SIGNAL_DISTX = IFX_TAPI_SIG_DISTX,
   /** V.21 Preamble Fax Tone in all path, Digital identification
       signal (DIS)  */
   TAPI_SIGNAL_DIS = IFX_TAPI_SIG_DIS,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, receive path */
   TAPI_SIGNAL_CEDRX = IFX_TAPI_SIG_CEDRX,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, transmit path */
   TAPI_SIGNAL_CEDTX = IFX_TAPI_SIG_CEDTX,
   /** V.25 2100 Hz (CED) Modem/Fax Tone in all paths */
   TAPI_SIGNAL_CED = IFX_TAPI_SIG_CED,
   /** CNG Fax Calling Tone (1100 Hz) receive path */
   TAPI_SIGNAL_CNGFAXRX = IFX_TAPI_SIG_CNGFAXRX,
   /** CNG Fax Calling Tone (1100 Hz) transmit path */
   TAPI_SIGNAL_CNGFAXTX = IFX_TAPI_SIG_CNGFAXTX,
   /** CNG Fax Calling Tone (1100 Hz) in all paths */
   TAPI_SIGNAL_CNGFAX = IFX_TAPI_SIG_CNGFAX,
   /** CNG Modem Calling Tone (1300 Hz) receive path */
   TAPI_SIGNAL_CNGMODRX = IFX_TAPI_SIG_CNGMODRX,
   /** CNG Modem Calling Tone (1300 Hz) transmit path  */
   TAPI_SIGNAL_CNGMODTX = IFX_TAPI_SIG_CNGMODTX,
   /** CNG Modem Calling Tone (1300 Hz) in all paths */
   TAPI_SIGNAL_CNGMOD = IFX_TAPI_SIG_CNGMOD,
   /** Phase reversal detection receive path
       \remarks Not supported phase reversal uses the same
       paths as CED detection. The detector for CED must be configured
       as well  */
   TAPI_SIGNAL_PHASEREVRX = IFX_TAPI_SIG_PHASEREVRX,
   /** Phase reversal detection transmit path
      \remarks Not supported phase reversal uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_PHASEREVTX = IFX_TAPI_SIG_PHASEREVTX,
   /** Phase reversal detection in all paths
       \remarks Phase reversals are detected at the
       end of an CED. If this signal is enabled also CED end detection
       is automatically enabled and reported if it occurs */
   TAPI_SIGNAL_PHASEREV = IFX_TAPI_SIG_PHASEREV,
   /** Amplitude modulation receive path

      \remarks Not supported amplitude modulation uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_AMRX = IFX_TAPI_SIG_AMRX,
   /** Amplitude modulation transmit path

   \remarks Not supported amplitude modulation uses the same
   paths as CED detection. The detector for CED must be configured
   as well  */
   TAPI_SIGNAL_AMTX = IFX_TAPI_SIG_AMTX,
   /** Amplitude modulation.

    \remarks Amplitude modulation is detected at the
    end of an CED. If this signal is enabled also CED end detection
    is automatically enabled and reported if it occurs.

    \note In case of AM detected the driver automatically switches
    to modem coefficients after the ending of CED */
   TAPI_SIGNAL_AM = IFX_TAPI_SIG_AM,
   /** Modem tone holding signal stopped receive path */
   TAPI_SIGNAL_TONEHOLDING_ENDRX = IFX_TAPI_SIG_TONEHOLDING_ENDRX,
   /** Modem tone holding signal stopped transmit path */
   TAPI_SIGNAL_TONEHOLDING_ENDTX = IFX_TAPI_SIG_TONEHOLDING_ENDTX,
   /** Modem tone holding signal stopped all paths */
   TAPI_SIGNAL_TONEHOLDING_END = IFX_TAPI_SIG_TONEHOLDING_END,
   /** End of signal CED detection receive path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_CEDENDRX = IFX_TAPI_SIG_CEDENDRX,
   /** End of signal CED detection transmit path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_CEDENDTX = IFX_TAPI_SIG_CEDENDTX,
   /** End of signal CED detection. This signal also includes
      information about phase reversals and amplitude modulation,
      if enabled */
   TAPI_SIGNAL_CEDEND = IFX_TAPI_SIG_CEDEND,
   /** Signals a call progress tone detection. This signal is enabled with
       the interface \ref IFXPHONE_CPTD_START and stopped with
       \ref IFXPHONE_CPTD_STOP. It can not be activate with
       \ref IFXPHONE_ENABLE_SIGDETECT */
   TAPI_SIGNAL_CPTD = IFX_TAPI_SIG_CPTD,
   /** Signals the V8bis detection on the receive path */
   TAPI_SIGNAL_V8BISRX = IFX_TAPI_SIG_V8BISRX,
   /** Signals the V8bis detection on the transmit path */
   TAPI_SIGNAL_V8BISTX = IFX_TAPI_SIG_V8BISTX,
   /** Signals that the Caller-ID transmission has finished */
   TAPI_SIGNAL_CIDENDTX = IFX_TAPI_SIG_CIDENDTX
};
typedef enum _IFX_TAPI_SIG                         TAPI_SIGNAL;

/** T38 Fax errors. */
enum _TAPI_FAX_T38_ERROR
{
   /** No Error */
   TAPI_FAX_T38_NO_ERR = IFX_TAPI_FAX_T38_ERROR_NONE,
   /** Error occured : Deactivate Datapump */
   TAPI_FAX_T38_ERR = IFX_TAPI_FAX_T38_ERROR_DATAPUMP,
   /** MIPS Overload */
   TAPI_FAX_T38_MIPS_OVLD = IFX_TAPI_FAX_T38_ERROR_MIPS_OVLD,
   /** Érror while reading data */
   TAPI_FAX_T38_READ_ERR = IFX_TAPI_FAX_T38_ERROR_READ,
   /** Error while writing data */
   TAPI_FAX_T38_WRITE_ERR = IFX_TAPI_FAX_T38_ERROR_WRITE,
   /** Error while setting up modulator or demodulator */
   TAPI_FAX_T38_DATA_ERR = IFX_TAPI_FAX_T38_ERROR_SETUP
};
typedef enum _IFX_TAPI_FAX_T38_ERROR               TAPI_FAX_T38_ERROR;

/** T38 Fax Datapump states. */
enum _TAPI_FAX_T38_STATUS
{
   /** Fax Datapump not active */
   TAPI_FAX_T38_DP_OFF = IFX_TAPI_FAX_T38_DP_OFF,
   /** Fax Datapump active */
   TAPI_FAX_T38_DP_ON = IFX_TAPI_FAX_T38_DP_ON,
   /** Fax transmission is active */
   TAPI_FAX_T38_TX_ON = IFX_TAPI_FAX_T38_TX_ON,
   /** Fax transmission is not active */
   TAPI_FAX_T38_TX_OFF = IFX_TAPI_FAX_T38_TX_OFF
};
typedef enum _IFX_TAPI_FAX_T38_STATUS              TAPI_FAX_T38_STATUS;

/** T38 Fax standards. */
enum _TAPI_FAX_T38_STD
{
   /** V.21 */
   TAPI_FAX_T38_V21_STD = IFX_TAPI_T38_STD_V21,
   /** V.27/2400 */
   TAPI_FAX_T38_V27_2400_STD = IFX_TAPI_T38_STD_V27_2400,
   /** V.27/4800 */
   TAPI_FAX_T38_V27_4800_STD = IFX_TAPI_T38_STD_V27_4800,
   /** V.29/7200 */
   TAPI_FAX_T38_V29_7200_STD = IFX_TAPI_T38_STD_V29_7200,
   /** V.29/9600 */
   TAPI_FAX_T38_V29_9600_STD = IFX_TAPI_T38_STD_V29_9600,
   /** V.17/7200 */
   TAPI_FAX_T38_V17_7200_STD = IFX_TAPI_T38_STD_V17_7200,
   /** V.17/9600 */
   TAPI_FAX_T38_V17_9600_STD = IFX_TAPI_T38_STD_V17_9600,
   /** V.17/12000 */
   TAPI_FAX_T38_V17_12000_STD = IFX_TAPI_T38_STD_V17_12000 ,
   /** V.17/14400 */
   TAPI_FAX_T38_V17_14400_STD = IFX_TAPI_T38_STD_V17_14400
};
typedef enum _IFX_TAPI_T38_STD                     TAPI_FAX_T38_STD;

/** Specifies the CPT signal for CPT detection. */
enum _TAPI_CPT_SIGNAL
{
   TAPI_CPT_SIGNALRX = IFX_TAPI_TONE_CPTD_DIRECTION_RX,
   TAPI_CPT_SIGNALTX = IFX_TAPI_TONE_CPTD_DIRECTION_TX
};
typedef enum _IFX_TAPI_TONE_CPTD_DIRECTION         TAPI_CPT_SIGNAL;

/** Frequency setting for a cadence step. */
enum _TAPI_TONE_FREQ
{
   TAPI_TONE_FREQNONE = IFX_TAPI_TONE_FREQNONE,
   TAPI_TONE_FREQA = IFX_TAPI_TONE_FREQA,
   TAPI_TONE_FREQB = IFX_TAPI_TONE_FREQB,
   TAPI_TONE_FREQC = IFX_TAPI_TONE_FREQC,
   TAPI_TONE_FREQD = IFX_TAPI_TONE_FREQD,
   TAPI_TONE_FREQALL = IFX_TAPI_TONE_FREQALL
};
typedef enum _IFX_TAPI_TONE_FREQ                   TAPI_TONE_FREQ;

/** Modulation setting for a cadence step. */
enum _TAPI_TONE_MODULATION
{
   TAPI_TONE_MODULATION_OFF = IFX_TAPI_TONE_MODULATION_OFF,
   TAPI_TONE_MODULATION_ON = IFX_TAPI_TONE_MODULATION_ON
};
typedef enum _IFX_TAPI_TONE_MODULATION             TAPI_TONE_MODULATION;

/** Defines the tone generator usage. */
enum _TAPI_TONE_TG
{
   /** Use tone generator 1 */
   TAPI_TONE_TG1 = IFX_TAPI_TONE_TG1,
   /** Use tone generator 2 */
   TAPI_TONE_TG2 = IFX_TAPI_TONE_TG2,
   /** Use all tone generators  */
   TAPI_TONE_TGALL = IFX_TAPI_TONE_TGALL
};
typedef enum _IFX_TAPI_TONE_TG TAPI_TONE_TG;

/** Tone types. */
enum _TAPI_TONE_TYPES
{
   TONE_SIMPLE = IFX_TAPI_TONE_TYPE_SIMPLE,
   TONE_COMPOSED = IFX_TAPI_TONE_TYPE_COMPOSED
};
typedef enum _IFX_TAPI_TONE_TYPE                TAPI_TONE_TYPES;

/** LEC gain levels */
enum _TAPI_LEC_GAIN
{
   /** Turn LEC off */
   TAPI_LEC_GAIN_OFF = IFX_TAPI_LEC_GAIN_OFF,
   /** Turn LEC on to low level*/
   TAPI_LEC_GAIN_LOW = IFX_TAPI_LEC_GAIN_LOW,
   /** Turn LEC on to normal level*/
   TAPI_LEC_GAIN_MEDIUM = IFX_TAPI_LEC_GAIN_MEDIUM,
   /** Turn LEC on to high level*/
   TAPI_LEC_GAIN_HIGH = IFX_TAPI_LEC_GAIN_HIGH
};
typedef enum _IFX_TAPI_LEC_GAIN                 TAPI_LEC_GAIN;

/** Jitter Buffer types */
enum _JB_TYPE
{
   FIXED_JB_TYPE   = IFX_TAPI_JB_TYPE_FIXED,
   ADAPTIVE_JB_TYPE = IFX_TAPI_JB_TYPE_ADAPTIVE
};
typedef enum _IFX_TAPI_JB_TYPE	               JB_TYPE;

/** Jitter buffer packet length adaption */
enum _JB_PKT_ADAPT
{
   PL_ADAPT_OFF = IFX_TAPI_JB_PKT_ADAPT_RES1,
   PL_ADAPT_ON  = IFX_TAPI_JB_PKT_ADAPT_RES2
};
typedef enum _IFX_TAPI_JB_PKT_ADAPT             JB_PKT_ADAPT;

/** Lec NLP */
enum _TAPI_LEC_NLP
{
   TAPI_LEC_NLPDEFAULT = IFX_TAPI_LEC_NLP_DEFAULT,
   TAPI_LEC_NLP_ON = IFX_TAPI_LEC_NLP_ON,
   TAPI_LEC_NLP_OFF = IFX_TAPI_LEC_NLP_OFF
};
typedef enum _IFX_TAPI_LEC_NLP                  TAPI_LEC_NLP;


/** Metering modes */
enum _TAPI_METER_MODE
{
   /** Normal TTX mode */
   TAPI_METER_MODE_TTX = IFX_TAPI_METER_MODE_TTX,
   /** Reverse polarity mode */
   TAPI_METER_MODE_REVPOL = IFX_TAPI_METER_MODE_REVPOL
};
typedef enum _IFX_TAPI_METER_MODE               TAPI_METER_MODE;

/** Metering frequency */
enum _TAPI_METER_FREQ
{
   /** Device default */
   TAPI_METER_FREQ_DEFAULT = 0,
   /** 12 kHz */
   TAPI_METER_FREQ_12_KHZ = 1,
   /** 16 kHz */
   TAPI_METER_FREQ_16_KHZ = 2
};
typedef enum _TAPI_METER_FREQ                   TAPI_METER_FREQ;

/** List the RFC2833 tone events that are detected from network side.
    */
enum _TAPI_RFC2833_EVENT
{
   /** RFC2833 Event number for DTMF tone  #0 */
   TAPI_RFC2833_EVENT_DTMF_0     = IFX_TAPI_PKT_EV_NUM_DTMF_0,
   /** RFC2833 Event number for DTMF tone  #1 */
   TAPI_RFC2833_EVENT_DTMF_1     = IFX_TAPI_PKT_EV_NUM_DTMF_1,
   /** RFC2833 Event number for DTMF tone  #2 */
   TAPI_RFC2833_EVENT_DTMF_2     = IFX_TAPI_PKT_EV_NUM_DTMF_2,
   /** RFC2833 Event number for DTMF tone  #3 */
   TAPI_RFC2833_EVENT_DTMF_3     = IFX_TAPI_PKT_EV_NUM_DTMF_3,
   /** RFC2833 Event number for DTMF tone  #4 */
   TAPI_RFC2833_EVENT_DTMF_4     = IFX_TAPI_PKT_EV_NUM_DTMF_4,
   /** RFC2833 Event number for DTMF tone  #5 */
   TAPI_RFC2833_EVENT_DTMF_5     = IFX_TAPI_PKT_EV_NUM_DTMF_5,
   /** RFC2833 Event number for DTMF tone  #6 */
   TAPI_RFC2833_EVENT_DTMF_6     = IFX_TAPI_PKT_EV_NUM_DTMF_6,
   /** RFC2833 Event number for DTMF tone  #7 */
   TAPI_RFC2833_EVENT_DTMF_7     = IFX_TAPI_PKT_EV_NUM_DTMF_7,
   /** RFC2833 Event number for DTMF tone  #8 */
   TAPI_RFC2833_EVENT_DTMF_8     = IFX_TAPI_PKT_EV_NUM_DTMF_8,
   /** RFC2833 Event number for DTMF tone  #9 */
   TAPI_RFC2833_EVENT_DTMF_9     = IFX_TAPI_PKT_EV_NUM_DTMF_9,
   /** RFC2833 Event number for DTMF tone STAR */
   TAPI_RFC2833_EVENT_DTMF_STAR  = IFX_TAPI_PKT_EV_NUM_DTMF_STAR,
   /** RFC2833 Event number for DTMF tone HASH */
   TAPI_RFC2833_EVENT_DTMF_HASH  = IFX_TAPI_PKT_EV_NUM_DTMF_HASH,
   /** RFC2833 Event number for ANS tone */
   TAPI_RFC2833_EVENT_ANS        = IFX_TAPI_PKT_EV_NUM_ANS,
   /** RFC2833 Event number for /ANS tone */
   TAPI_RFC2833_EVENT_NANS       = IFX_TAPI_PKT_EV_NUM_NANS,
   /** RFC2833 Event number for ANSam tone */
   TAPI_RFC2833_EVENT_ANSAM      = IFX_TAPI_PKT_EV_NUM_ANSAM,
   /** RFC2833 Event number for /ANSam tone */
   TAPI_RFC2833_EVENT_NANSAM     = IFX_TAPI_PKT_EV_NUM_NANSAM,
   /** RFC2833 Event number for CNG tone */
   TAPI_RFC2833_EVENT_CNG        = IFX_TAPI_PKT_EV_NUM_CNG,
   /** RFC2833 Event number for DIS tone */
   TAPI_RFC2833_EVENT_DIS        = IFX_TAPI_PKT_EV_NUM_DIS,
   /** no support RFC event received or no event received */
   TAPI_RFC2833_EVENT_NO_EVENT   = IFX_TAPI_PKT_EV_NUM_NO_EVENT
};
typedef enum _IFX_TAPI_PKT_EV_NUM              TAPI_RFC2833_EVENT;

/* Jitter Buffer Packet Configuration */
enum _JB_CONF_PCK
{
   JB_CONF_PCK_RES1  = IFX_TAPI_JB_PKT_ADAPT_RES1,
   JB_CONF_PCK_RES2  = IFX_TAPI_JB_PKT_ADAPT_RES2,
   JB_CONF_PCK_VOICE = IFX_TAPI_JB_PKT_ADAPT_VOICE,
   JB_CONF_PCK_DATA  = IFX_TAPI_JB_PKT_ADAPT_DATA
};
typedef enum _IFX_TAPI_JB_PKT_ADAPT               JB_CONF_PCK;


/** Out of band or in band definition */
enum _TAPI_OOB
{
   /** Tranmit in band and out of band */
   TAPI_OOB_INOUT = IFX_TAPI_PKT_EV_OOB_ALL,
   /** Tranmit only out of band */
   TAPI_OOB_OUT = IFX_TAPI_PKT_EV_OOB_ONLY,
   /** Tranmit only in band */
   TAPI_OOB_IN = IFX_TAPI_PKT_EV_OOB_NO
};
typedef enum _IFX_TAPI_PKT_EV_OOB	               TAPI_OOB;

/** Enumeration for phone line status information */
enum _TAPI_ERROR
{
   TAPI_ERROR_NONE = IFX_TAPI_RT_ERROR_NONE,
   TAPI_ERROR_RINGCADENCE_CIDTX = IFX_TAPI_RT_ERROR_RINGCADENCE_CIDTX,
   TAPI_ERROR_CIDTX_NOACK       = IFX_TAPI_RT_ERROR_CIDTX_NOACK
};
typedef enum _IFX_TAPI_RUNTIME_ERROR	            TAPI_ERROR;

/** Report Set */
enum _TAPI_REPORT_SET
{
   REPORT_SET_OFF = IFX_TAPI_DEBUG_REPORT_SET_OFF,
   REPORT_SET_LOW = IFX_TAPI_DEBUG_REPORT_SET_LOW,
   REPORT_SET_NORMAL = IFX_TAPI_DEBUG_REPORT_SET_NORMAL,
   REPORT_SET_HIGH = IFX_TAPI_DEBUG_REPORT_SET_HIGH
};
typedef enum _IFX_TAPI_DEBUG_REPORT_SET	         TAPI_REPORT_SET;

/* ============================= */
/* Global structures             */
/* ============================= */

/*
   typedef struct _<new_struct_prefix>       <old_struct_name>;
*/

typedef struct _IFX_TAPI_CAP                          TAPI_PHONE_CAPABILITY;
typedef struct _IFX_TAPI_CH_INIT	                     TAPI_INIT;
typedef struct _IFX_TAPI_CH_STATUS	                  TAPI_PHONE_STATUS;
typedef struct _IFX_TAPI_CID_CFG	                     TAPI_CID_CONF_t;
typedef struct _IFX_TAPI_CID_DTMF_CFG	               TAPI_CID_DTMF_CONF_t;
typedef struct _IFX_TAPI_CID_FSK_CFG	               TAPI_CID_FSK_CONF_t;
typedef struct _IFX_TAPI_CID_ABS_REASON         	   TAPI_CID_ABS_REASON_t;
typedef struct _IFX_TAPI_CID_MSG	                     TAPI_CID_INFO_t;
typedef struct _IFX_TAPI_CID_MSG_DATE	               TAPI_CID_MSG_DATE_t;
typedef union  _IFX_TAPI_CID_MSG_ELEMENT	            TAPI_CID_MSG_ELEMENT_t;
typedef struct _IFX_TAPI_CID_MSG_STRING	            TAPI_CID_MSG_STRING_t;
typedef struct _IFX_TAPI_CID_MSG_VALUE	               TAPI_CID_MSG_VALUE_t;
typedef struct _IFX_TAPI_CID_RX_DATA	               TAPI_CIDRX_DATA;
typedef struct _IFX_TAPI_CID_RX_STATUS	               TAPI_CIDRX_STATUS;
typedef struct _IFX_TAPI_CID_STD_ETSI_DTMF	         TAPI_CID_STD_ETSI_DTMF_t;
typedef struct _IFX_TAPI_CID_STD_ETSI_FSK	            TAPI_CID_STD_ETSI_FSK_t;
typedef struct _IFX_TAPI_CID_STD_NTT	               TAPI_CID_STD_NTT_t;
typedef struct _IFX_TAPI_CID_STD_SIN	               TAPI_CID_STD_SIN_t;
typedef struct _IFX_TAPI_CID_STD_TELCORDIA            TAPI_CID_STD_TELCORDIA_t;
typedef union  _IFX_TAPI_CID_STD_TYPE	               TAPI_CID_STD_TYPE_t;
typedef struct _IFX_TAPI_CID_TIMING	                  TAPI_CID_TIMING_t;
typedef struct _IFX_TAPI_JB_CFG	                     TAPI_JB_CONF;
typedef struct _IFX_TAPI_JB_STATISTICS	               TAPI_JB_DATA;
typedef struct _IFX_TAPI_LEC_CFG	                     TAPI_LECCONF;
typedef struct _IFX_TAPI_LINE_HOOK_VT	               TAPI_VALIDATION_TIMES;
typedef struct _IFX_TAPI_LINE_VOLUME	               TAPI_PHONE_VOLUME;
typedef struct _IFX_TAPI_MAP_DATA	                  TAPI_DATA_MAPPING;
typedef struct _IFX_TAPI_MAP_PCM	                     TAPI_PCM_MAPPING;
typedef struct _IFX_TAPI_MAP_PHONE	                  TAPI_PHONE_MAPPING;
typedef struct _IFX_TAPI_METER_CFG	                  TAPI_METER_CONFIG;
typedef struct _IFX_TAPI_PCK_AAL_CFG	               TAPI_AAL_CONF;
typedef struct _IFX_TAPI_PCK_AAL_PROFILE	            TAPI_AAL_PROFILE;
typedef struct _IFX_TAPI_PCM_CFG	                     TAPI_PCM_CONFIG;
typedef struct _IFX_TAPI_PKT_RTCP_STATISTICS	         TAPI_RTCP_DATA;
typedef struct _IFX_TAPI_PKT_RTP_CFG	               TAPI_RTP_CONF;
typedef struct _IFX_TAPI_PKT_RTP_PT_CFG	            TAPI_RTP_PT_CONF;
typedef struct _IFX_TAPI_RING_CADENCE	               TAPI_RING_CADENCE;
typedef struct _IFX_TAPI_RING_CFG	                  TAPI_RING_CONFIG;
typedef struct _IFX_TAPI_SIG_DETECTION	               TAPI_PHONE_SIGDETECTION;
typedef struct _IFX_TAPI_T38_DEMOD_DATA	            TAPI_FAX_DEMODDATA;
typedef struct _IFX_TAPI_T38_MOD_DATA	               TAPI_FAX_MODDATA;
typedef struct _IFX_TAPI_T38_STATUS	                  TAPI_FAX_STATUS;
typedef union  _IFX_TAPI_TONE	            TAPI_TONE;
typedef struct _IFX_TAPI_TONE_COMPOSED	               TAPI_COMPOSED_TONE;
typedef struct _IFX_TAPI_TONE_CPTD	                  TAPI_PHONE_CPTD;
typedef struct _IFX_TAPI_TONE_SIMPLE	               TAPI_SIMPLE_TONE;
typedef struct _IFX_TAPI_PREDEF_TONE_LEVEL      	   TAPI_TONE_LEVEL;
typedef struct _IFX_TAPI_VERSION	                     TAPI_PHONE_VERSION;
typedef struct _IFX_TAPI_EXCEPTION_BITS	            EXCEPTION_BITS;
typedef union  _IFX_TAPI_EXCEPTION	                  TAPI_EXCEPTION;

#endif /* DRV_TAPI_OLD_IO_H */

