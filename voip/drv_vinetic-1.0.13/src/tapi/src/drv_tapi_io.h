#ifndef DRV_TAPI_IO_H
#define DRV_TAPI_IO_H
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
   Module      : drv_tapi_io.h
   Desription  : Contains TAPI I/O defines, enums and structures
                 according to LTAPI specification.
   \file TAPI ioctl defines
   \note Changes:
   TAPI_DATA_MAPPING nPhoneCh changed to nDstCh because it can also be a PCM.
   TAPI_DATA_MAPPING nDataChType changed to nChType for destination type
   IFXPHONE_PCM_ADD and REMOVE added
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */

/* ============================= */
/* Global defines                */
/* ============================= */

#ifndef TAPI_VERSION2
#define TAPI_VERSION2
#endif

#define TAPI_MAX_CADENCE_BYTES   40
#define TAPI_DIAL_FIFO_SIZE      20
#define TAPI_DTMF_FIFO_SIZE      40
#define TAPI_CIDRX_FIFO_SIZE     10
#define TAPI_MAX_SIMPLE_TONES    7
/* maximum cid data */
/*
   ETSI  :
   -------
   call setup cmd : 2,  cli : 22, date/time : 10, name : 52,
   redir num : 22, checksum : 1 => 109 Bytes max in CID buffer
   NTT :
   -------
   DLE : 3, SOH : 1, Header : 1, STX : 1, ETX : 1, DATA: 119, CRC : 2
   => 128 Bytes max in CID Buffer
*/
#define TAPI_MAX_CID_PARAMETER   128 /* because of NTT support */

/* #define USA_RING_CADENCE         0xC0C0 */

/** magic number for ioctls*/
#define TAPI_IOC_MAGIC 'q'

/** \defgroup TAPI_INTERFACE TAPI Driver Interface
    Lists the entire interface to the TAPI
  @{  */

/**
   \defgroup TAPI_INTERFACE_INIT Initialization service
   This service sets the default initialization of device and hardware.
   All default settings are linked as fixed tables to the source code
   of the TAPI. These tables can be easily replaced by user defined tables.  */

/** \defgroup TAPI_INTERFACE_OP Operation Control Service
    Modifies the operation of the device. All operation control services
    apply to phone channels except otherwise stated.  */

/** \defgroup TAPI_INTERFACE_METER Metering Service
    Contains services for metering.
    All metering services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_TONE Tone Control Service
    All tone services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_DIAL Dial Service
    Contains services for dialing.
    All dial services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_SIGNAL Signal Detection Services
   The application handles the different states of the detection status
   like depicted in the image \image html DataChSwitching.emf */

/** \defgroup TAPI_INTERFACE_CID CID Features
   Defines the service used for CID display according to ETSI EN 300 659-3.
   CLI is set to zero cause it is a mandatory field */

/** \defgroup TAPI_INTERFACE_CON Connection Control Service
    Contains all services used for RTP or AAL connection. It also contains
    services for conferencing.
    Apply to data channels except otherwise stated.  */

/** \defgroup TAPI_INTERFACE_MISC Miscellaneous Service
    Contains services for status and version information.
    Apply to phone channels except otherwise stated.  */

/** \defgroup TAPI_INTERFACE_RINGING Ringing Service */

/** \defgroup TAPI_INTERFACE_PCM PCM Service
    Contains services for PCM configuration.
    Apply to phone channels except otherwise stated.  */

/** \defgroup TAPI_INTERFACE_FAX Fax T.38 services.
   The FAX services switch the  corresponding data channel from
   voice to T.38 data pump.
   Also the data interface changes. So the FAX data is sent and received via
   the read and write interface of the drivers, once the or has been called
   successfully. A call to switches the driver and the device back to voice
    processing mode and the functions read and write will service voice data
    (RTP or AAL packets).
    All fax services apply to phone channels except otherwise stated. */

/*@}*/

/** Retrieves the TAPI version string

   \param char* Pointer to version character string

   \return 0 if successful, otherwise -1 in case of an error

   \code
   char Version[80];
   int ret;

   ret = ioctl(fd, IFXPHONE_GET_VERSION, (int)&Version);
   printf("\nVersion:%s\n", Version);
   \endcode

   \ingroup TAPI_INTERFACE_MISC */
#define  IFXPHONE_GET_VERSION             _IO(TAPI_IOC_MAGIC, 0x00)

/** This service sets the line feeding mode

   \param int The parameter represents an mode value.

   - 0: TAPI_LINEFEED_NORMAL, Normal feeding mode for phone off hook
   - 1: TAPI_LINEFEED_REVERSED, reversed feeding mode for phone off hook
   - 2: TAPI_LINEFEED_PARKED, power down resistance = on hook with
   hook detection
   - 3: TAPI_LINEFEED_HIGH_IMPEDANCE, Switch off the line, but the
   device is able to test the line
   - 4: TAPI_LINEFEED_POWER_DOWN, Switch off the line and the device
   - 5: TAPI_LINEFEED_GROUND_START, ground start mode
   - 6: TAPI_LINEFEED_NORMAL_AUTO, normal with automatic battery switch.
   Thresholds for automatic battery switch are set via coefficient settings
   - 7: TAPI_LINEFEED_REVERSED_AUTO, reversed with automatic battery switch
   - 8: TAPI_LINEFEED_NORMAL_LOW, feeding mode for phone off hook with
        low battery to save power
   - 9: TAPI_LINEFEED_REVERSED_LOW, feeding mode for phone off hook with
        low battery to save power and reserved polarity

   \remarks For all battery switching modes the hardware must be able to
   support it for example by programming coefficients.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // set line mode to power down
   ioctl(fd, IFXPHONE_SET_LINEFEED, TAPI_LINEFEED_POWER_DOWN);
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_SET_LINEFEED            _IO(TAPI_IOC_MAGIC, 0x01)

/** This service sets the ring configuration for the non-blocking
    ringing services.
   This service is non-blocking.

   \param TAPI_RING_CONFIG* The parameter points to a
          \ref TAPI_RING_CONFIG structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The configuration has to be set before ringing starts using the
    interface \ref PHONE_RING_START

   \code
     TAPI_RING_CONFIG param;
     memset (&param, 0, sizeof(TAPI_RING_CONFIG));
     param.mode = 0;
     param.submode = 1;
     ret = ioctl(fd, IFXPHONE_RING_CONFIG, &param)
   \endcode
   \ingroup TAPI_INTERFACE_RINGING */
#define  IFXPHONE_RING_CONFIG             _IO(TAPI_IOC_MAGIC, 0x02)

/**
   This service sets the high resolution ring cadence for the non-blocking
   ringing services. The cadence value has to be set before ringing starts
   (\ref PHONE_RING_START).This service is non-blocking

   \param TAPI_RING_CADENCE* The parameter points to a
   \ref TAPI_RING_CADENCE structure.

   \return 0 if successful, otherwise -1 in case of an error.
   The number of ring bursts can be counted via exception
   “PSTN line is ringing” of service \ref PHONE_EXCEPTION.

   \code
   // pattern of 3 sec. ring and 1 sec. pause
   char data[10] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00 };
   // initial 1 sec. ring and 600 ms non ringing signal before ringing
   char initial[4] =0xFF,0xFF,0xF0,0x0};
   TAPI_RING_CADENCE Cadence;
   memset (&Cadence, 0, sizeof(TAPI_RING_CADENCE));
   memcpy (&Cadence.data, data, sizeof (data));
   Cadence.nr = sizeof(data) * 8;
   // set size in bits
   memcpy (&Cadence.initial, initial, sizeof (initial));
   Cadence.initialNr = 4 * 8;
   // set the cadence sequence
   ioctl(fd, IFXPHONE_RING_CADENCE_HIGH_RES, &Cadence);
   \endcode

   \ingroup TAPI_INTERFACE_RINGING */
#define  IFXPHONE_RING_CADENCE_HIGH_RES   _IO(TAPI_IOC_MAGIC, 0x03)

/** \addtogroup TAPI_INTERFACE_PCM PCM Service */
/*@{*/

/** This service sets the configuration of the PCM interface.
   \param TAPI_PCM_CONFIG* The parameter points to a
   \ref TAPI_PCM_CONFIG structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The parameter rate must be set to the PCM rate, which is
   applied to the device, otherwise error is returned.

   \code
   TAPI_PCM_CONFIG Pcm;
   memset (&Pcm, 0, sizeof(TAPI_PCM_CONFIG));
   Pcm.nTimeslotRX = 5;
   Pcm.nTimeslotTX = 5;
   Pcm.nHighway = 1;
   // 16 bit resolution
   Pcm.nResolution = LINEAR_16_BIT;
   // 2048 kHz sample rate
   Pcm.nRate = 2048;
   // configure PCM interface
   ioctl(fd, IFXPHONE_SET_PCM_CONFIG, &Pcm);
   \endcode   */
#define  IFXPHONE_SET_PCM_CONFIG          _IO(TAPI_IOC_MAGIC, 0x04)

/** This service gets the configuration of the PCM interface.

   \param TAPI_PCM_CONFIG* The parameter points to a
   \ref TAPI_PCM_CONFIG structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PCM_CONFIG Pcm;
   memset (&Pcm, 0, sizeof(TAPI_PCM_CONFIG));
   ioctl(fd, IFXPHONE_GET_PCM_CONFIG, &Pcm);
   \endcode   */
#define  IFXPHONE_GET_PCM_CONFIG          _IO(TAPI_IOC_MAGIC, 0x05)

/** This service activate / deactivates the PCM timeslots configured for this channel
   \param int The parameter defines the activation status
       - 0 deactivate the timeslot
       - 1 activate the timeslot

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // activate the PCM timeslot
   ioctl(fd, IFXPHONE_SET_PCM_ACTIVATION, 1);
\endcode   */
#define  IFXPHONE_SET_PCM_ACTIVATION      _IO(TAPI_IOC_MAGIC, 0x06)

/** This service gets the activation status of the PCM timeslots configured
   for this channel

   \param int The parameter points to an integer which returns the status
          - 0: The timeslot is deactive
          - 1: The timeslot is active

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int bAct, ret;
   ret = ioctl(fd, IFXPHONE_GET_PCM_ACTIVATION, &bAct);
   if (ret == 0 && bAct) print ("Activated\n");
\endcode   */
#define  IFXPHONE_GET_PCM_ACTIVATION      _IO(TAPI_IOC_MAGIC, 0x07)

/*@}*/

/**
   This service sets the tone level of the tone currently played.
   \param TAPI_TONE_LEVEL* The parameter points to a
   \ref TAPI_TONE_LEVEL structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks

   \code
   TAPI_TONE_LEVEL Tone;
   memset (&Tone, 0, sizeof(TAPI_TONE_LEVEL));
   Tone.generator = 0xFF;
   // all tone generators
   Tone.level = 0x100;
   // -38.97 dB level
   // playlevel in dB can be calculated via formula:
   // playlevel_in_dB = +3.17 + 20 log10 (nPlayLevel/32767)
   ioctl(fd, IFXPHONE_SET_TONE_LEVEL, &Tone);
   \endcode
   \ingroup  TAPI_INTERFACE_TONE */
#define  IFXPHONE_SET_TONE_LEVEL          _IO(TAPI_IOC_MAGIC, 0x08)

/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/
/** This service checks if a pulse digit was received.

   \param int* The parameter points to the status. It returns the
   following values:

   - 0: No pulse digit is in the receive queue
   - 1: pulse digit is in the receive queue

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFXPHONE_PHONE_STATUS can be used upon exception. If the
   event reporting mask is not set (see \ref IFXPHONE_MASK_EXCEPTION),
   \ref IFXPHONE_PHONE_STATUS reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int bReady;
   //  check the pulse dialing status
   ioctl(fd, IFXPHONE_PULSE_READY, &bReady);
   if (bReady == 1)
   {
      //  digit arrived
   }
   else
   {
      //  no digit in the buffer
   }
   \endcode   */
#define  IFXPHONE_PULSE_READY             _IO(TAPI_IOC_MAGIC, 0x09)

/**
   This service reads the dial pulse digit out of the receive buffer. After
   reading the digit is removed from the receive buffer. The receive buffer
   contains 20 digits.
   \param int* The parameter points to the detected digit.
   It returns the following values:

   - 0: no digit detected
   - 1: Digit 1
   - ...
   - 9: Digit 9
   - B: 0

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFXPHONE_PHONE_STATUS can be used upon exception. If the
   event reporting mask is not set (see \ref IFXPHONE_MASK_EXCEPTION),
   \ref IFXPHONE_PHONE_STATUS reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int nDigit, ret;
   //  get pulse digit
   ret = ioctl(fd, IFXPHONE_GET_PULSE, &nDigit);
   \endcode   */
#define  IFXPHONE_GET_PULSE               _IO(TAPI_IOC_MAGIC, 0x0A)

/** This service reads the ASCII character representing the pulse digit
   out of the receive buffer. After reading the digit is removed from the
   receive buffer.
   The parameter points to int nDigit. It returns the ASCII representation
   of the detected digit.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int32 nDigit, ret;
   //  get DTMF digit
   ret = ioctl(fd, PHONE_GET_PULSE_ASCII, &nDigit);
   \endcode   */
#define  IFXPHONE_GET_PULSE_ASCII         _IO(TAPI_IOC_MAGIC, 0x0B)
/*@}*/

/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** This service sets the characteristic for the metering service.

   \param TAPI_METER_CONFIG* The parameter points to a
   \ref TAPI_METER_CONFIG structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If burst_cnt is not zero the burst number is defined by
   burst_cnt and stopping is not necessary. If burst_cnt is
   set to zero the bursts are sent out until a IFXPHONE_METER_STOP
   ioctl is called.

   \code
   TAPI_METER_CONFIG Metering;
   memset (&Metering, 0, sizeof(TAPI_METER_CONFIG));
   //  Metering mode is already set to 0
   // 100ms burst length
   Metering.burst_len = 100;
   // 2 min. burst distance
   Metering.burst_dist = 120;
   // send out 2 bursts
   Metering.burst_cnt = 2;
   //  set metering characteristic
   ioctl(fd, IFXPHONE_METER_CHAR, &Metering);
   \endcode   */
#define  IFXPHONE_METER_CHAR              _IO(TAPI_IOC_MAGIC, 0x0C)

/** This service starts the metering.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Before this service can be used,
   the metering characteristic must be set (service \ref IFXPHONE_METER_CHAR)
   and the line mode must be set to normal
   (service \ref IFXPHONE_SET_LINEFEED).

   \code
   ioctl(fd, IFXPHONE_METER_START, 0);
   \endcode   */
#define  IFXPHONE_METER_START             _IO(TAPI_IOC_MAGIC, 0x0D)

/** This service stops the metering.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If the metering has not been started before this service
   returns an error

   \code
   ioctl(fd, IFXPHONE_METER_STOP, 0);
   \endcode   */
#define  IFXPHONE_METER_STOP              _IO(TAPI_IOC_MAGIC, 0x0E)
/*@}*/

/** This service sets the default initialization of device and hardware.

   \param TAPI_INIT* The parameter points to a \ref TAPI_INIT structure

   \code
   TAPI_INIT Init;
   Init.nMode = 0;
   Init.nCountry = 2;
   Init.pProc = &DevInitStruct;
   ioctl(fd, IFXPHONE_INIT, &Init);
   \endcode

   \ingroup TAPI_INTERFACE_INIT */
#define  IFXPHONE_INIT                    _IO(TAPI_IOC_MAGIC, 0x0F)

/** This service masks the reporting of the exceptions.
   Set the mask to 1 disables the reporting of the corresponding event.

   \param int The parameter defines the exception event bits mask,
   defined in \ref EXCEPTION_BITS

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If digit reporting is enabled, digits are also read out in
   \ref IFXPHONE_PHONE_STATUS

   \code
   // mask reporting of exceptions
   ioctl(fd, IFXPHONE_MASK_EXCEPTION, nException);
   \endcode
   \ingroup TAPI_INTERFACE_MISC */
#define  IFXPHONE_MASK_EXCEPTION          _IO(TAPI_IOC_MAGIC, 0x10)

/** Configures the CID transmitter

   \param TAPI_CID_CONFIG* The parameter points to a
   \ref TAPI_CID_CONFIG structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The delay must be programmed in that way, that the CID data still fits
   between the ring burst in case of appearance mode 1 and 2, otherwise the
   ioctl \ref PHONE_RING_START may return an error.
   CID is stopped when the ringing is stopped or the phone goes off hook.

   \code
   TAPI_CID_CONFIG param;
   int ret;
   memset (&param, 0, sizeof(TAPI_CID_CONFIG));
   // set spec to V23
   param.nSpec = 1;
   ret = ioctl(fd, IFXPHONE_CIDCONF, &param);
   \endcode

   \ingroup TAPI_INTERFACE_CID */
#define  IFXPHONE_CIDCONF                 _IOW(TAPI_IOC_MAGIC, 0x11, int)

typedef enum
{
   /** */
   REPORT_SET_OFF,
   /** */
   REPORT_SET_LOW,
   /** */
   REPORT_SET_NORMAL,
   /** */
   REPORT_SET_HIGH
}TAPI_REPORT_SET;

/** Set the report levels if the driver is compiled with
    ENABLE_TRACE

    \param int Valid arguments are

    - 0: REPORT_SET_OFF, report is switched off
    - 1: REPORT_SET_LOW, report is switched to high output
    - 2: REPORT_SET_NORMAL, report is switched to general information and warnings
    - 3: REPORT_SET_HIGH, report is switched to report only errors
    \ingroup TAPI_INTERFACE_MISC     */
#define  IFXPHONE_REPORT_SET              _IO(TAPI_IOC_MAGIC, 0x12)

/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** This interface configures RTP and RTCP fields for a new connection.

   \param TAPI_RTP_CONF* The parameter points to a
   \ref TAPI_RTP_CONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_RTP_CONF param;
   memset (&param, 0, sizeof(TAPI_RTP_CONF));
   param.nSeqNr = 0x1234;
   ret = ioctl(fd, IFXPHONE_RTP_CONF, &param);
   \endcode   */
#define  IFXPHONE_RTP_CONF                _IOW(TAPI_IOC_MAGIC, 0x13, int)

/** Change the payload type table

   \param TAPI_RTP_PT_CONF* The parameter points to a
   \ref TAPI_RTP_PT_CONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The requested payload type should have been negotiated with the peer prior
   to the connection set-up. Event payload types are negotiated during
   signaling phase and the driver and the device can be programmed accordingly
   at the time of starting the media session. Event payload types shall not be
   modified when the session is in progress.

   \code
   TAPI_RTP_PT_CONF param;
   memset (&param, 0, sizeof(TAPI_RTP_PTCONF));
   param.nPT[G728] = 0x5;
   param.nPT[G726_16] = 0x4;
   ret = ioctl(fd, IFXPHONE_RTP_PTCONF, &param);
   \endcode   */
#define  IFXPHONE_RTP_PTCONF              _IOW(TAPI_IOC_MAGIC, 0x14, int)

/** Retrieves RTCP statistics

   \param TAPI_RTCP_DATA* Pointer to a \ref TAPI_RTCP_DATA structure
   according RFC 3550/3551 for a sender report.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The structure refers to the RFC 3550 / 3551. Not all fields may
   be supported by the TAPI, but must be filled by the calling application.

   \code
   TAPI_RTCP_DATA param;
   ret = ioctl(fd, IFXPHONE_RTCP_STATISTICS, (int)&param);
   \endcode   */
#define  IFXPHONE_RTCP_STATISTICS         _IOR(TAPI_IOC_MAGIC, 0x15, int)

/** Resets the RTCP statistics.
   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFXPHONE_RTCP_RESET, 0);
   \endcode   */
#define  IFXPHONE_RTCP_RESET              _IOW(TAPI_IOC_MAGIC, 0x16, int)

/** Configures the jitter buffer
   \param TAPI_JB_CONF* The parameter points to a
   \ref TAPI_JB_CONF structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Select fixed or adaptive jitter buffer and set
   parameters. Modifies the JB settings during run-time.

   \code
   TAPI_JB_CONF param;
   memset (&param, 0, sizeof(TAPI_JB_CONF));
   // set to adaptive jitter buffer type
   param.nJbType = 2;
   ret = ioctl(fd, IFXPHONE_JB_CONF, param);
   \endcode */
#define  IFXPHONE_JB_CONF                 _IOW(TAPI_IOC_MAGIC, 0x17, int)

/** Reads out jitter buffer statistics.
   \param TAPI_JB_DATA* The parameter points to a
   \ref TAPI_JB_DATA structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_JB_DATA param;
   memset (&param, 0, sizeof(TAPI_JB_DATA));
   ret = ioctl(fd, IFXPHONE_JB_STATISTICS, param);
   ...
   \endcode */
#define  IFXPHONE_JB_STATISTICS           _IOR(TAPI_IOC_MAGIC, 0x18, int)

/** Resets the jitter buffer statistics.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFXPHONE_JB_RESET, 0);
   \endcode   */
#define  IFXPHONE_JB_RESET                _IOW(TAPI_IOC_MAGIC, 0x19, int)

/** This interface configures AAL fields for a new connection.

   \param TAPI_AAL_CONF* The parameter points to a
   \ref TAPI_AAL_CONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_AAL_CONFIG param;
   memset (&param, 0, sizeof(TAPI_AAL_CONFIG));
   param.nTimestamp = 0x1234;
   ret = ioctl(fd, IFXPHONE_AAL_CONF, &param)
   \endcode   */
#define  IFXPHONE_AAL_CONF                _IOW(TAPI_IOC_MAGIC, 0x1E, int)
/*@}*/

/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** This service configures and enables the modulator for a T.38 fax session.

   \param TAPI_FAX_MODDATA* The parameter points to a
   \ref TAPI_FAX_MODDATA structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service deactivates the voice path and configures the channel
    for a fax session.

   \ingroup TAPI_INTERFACE_FAX

   \code
     TAPI_FAX_MODDATA param;
     memset (&param, 0, sizeof(TAPI_FAX_MODDATA));
     // set V.21 standard
     param.nStandard = 0x01;
     ret = ioctl(fd, IFXPHONE_FAX_SETMOD, &param);
   \endcode */
#define  IFXPHONE_FAX_SETMOD              _IOW(TAPI_IOC_MAGIC, 0x1A, int)

/** This service configures and enables the demodulator for a T.38 fax session.

   \param TAPI_FAX_DEMODDATA* The parameter points to a
   \ref TAPI_FAX_DEMODDATA structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service deactivates the voice path and configures the driver
    for a fax session.

   \ingroup TAPI_INTERFACE_FAX

   \code
     TAPI_FAX_DEMODDATA param;
     memset (&param, 0, sizeof(TAPI_FAX_DEMODDATA));
     // set V.21 standard as standard used for fax
     param.nStandard1 = 0x01;
     // set V.17/14400 as alternative standard
     param.nStandard2 = 0x09;
     ret = ioctl(fd, IFXPHONE_FAX_SETDEMOD, &param);
   \endcode */
#define  IFXPHONE_FAX_SETDEMOD            _IOW(TAPI_IOC_MAGIC, 0x1B, int)

/** This service disables the T.38 fax data pump and activates the voice
    path again.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup TAPI_INTERFACE_FAX

   \code
     ret = ioctl(fd, IFXPHONE_FAX_DISABLE, 0);
   \endcode */
#define  IFXPHONE_FAX_DISABLE             _IOW(TAPI_IOC_MAGIC, 0x1C, int)

/** This service provides the T.38 fax status on query.

   \param TAPI_FAX_STATUS* The parameter points to a
   \ref TAPI_FAX_STATUS structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This interface must be used when a fax exception has occurred to know
    the status. An error or a change of status will be indicated with TAPI
    exceptions, refer to \ref TAPI_EXCEPTION.

   \ingroup TAPI_INTERFACE_FAX

   \code
     TAPI_FAX_STATUS param;
     memset (&param, 0, sizeof(TAPI_FAX_STATUS));
     // request status
     ret = ioctl(fd, IFXPHONE_FAX_STATUS, &param);
   \endcode */
#define  IFXPHONE_FAX_STATUS              _IOR(TAPI_IOC_MAGIC, 0x1D, int)
/*@}*/

/** This service gets the ring configuration for the non-blocking
    ringing services.

   \param TAPI_RING_CONFIG* The parameter points to a
          \ref TAPI_RING_CONFIG structure

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup TAPI_INTERFACE_RINGING */
#define  IFXPHONE_RING_GET_CONFIG         _IO(TAPI_IOC_MAGIC, 0x1F)

/** Read out the configuration of the CID transmitter.

   \param TAPI_CID_CONFIG* The parameter points to a
   \ref TAPI_CID_CONFIG structure

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup TAPI_INTERFACE_CID */
#define  IFXPHONE_GETCIDCONF              _IOR(TAPI_IOC_MAGIC, 0x20, int)

/** Configures the DTMF CID Sending parameters according ETS 300 659-1.

   \param TAPI_DTMFCID* The parameter points to a
   \ref TAPI_DTMFCID structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_DTMFCID dtmfCid;
   memset (&dtmfCid, 0, sizeof (dtmfCid));
   dtmfCid.ackTone = 'A';
   dtmfCid.startTone = 'A';
   dtmfCid.stopTone = 'C';
   dtmfCid.infoStartTone = 'B';
   dtmfCid.redirStartTone = 'D';
   dtmfCid.digitTime = 50;
   dtmfCid.interDigitTime = 50;
   ioctl (gFd[ch], IFXPHONE_DTMFCID_CONF, &dtmfCid);
   \endcode

   \ingroup TAPI_INTERFACE_CID */
#define  IFXPHONE_DTMFCID_CONF            _IOW(TAPI_IOC_MAGIC, 0x21, int)

/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** AAL profile configuration.

   \param TAPI_AAL_PROFILE* The parameter points to a
   \ref TAPI_AAL_PROFILE structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
    Sets each row in a profile. A maximum number of 10 rows can be set up.
    This interface is called on initialization to program the selected AAL
    profile.
    The following example programms the ATMF profiles 3, 4 and 5

   \code
   TAPI_AAL_PROFILE Profile;
   switch (nProfile)
   {
      case  3:
      case  4:
      // ATMF profile 3 and 4
      Profile.rows = 1;
      Profile.codec[0] = MLAW;
      Profile.len[0] = 43;
      Profile.nUUI[0] = RANGE_0_15;
      break;
      case  5:
      // ATMF profile 5
      Profile.rows = 2;
      Profile.codec[0] = MLAW;
      Profile.len[0] = 43;
      Profile.nUUI[0] = RANGE_0_7;
      Profile.codec[1] = G726_32;
      Profile.len[1] = 43;
      Profile.nUUI[1] = RANGE_8_15;
      break;
   }
   ret = ioctl (fd, IFXPHONE_AAL_PROFILE, (INT)&Profile);
   \endcode */
#define  IFXPHONE_AAL_PROFILE             _IOW(TAPI_IOC_MAGIC, 0x23, int)

/** This interface adds the data channel to an analog phone device.
   To set up a conference this interfaces is used on more data channels.

   \param TAPI_DATA_MAPPING* The parameter points to a
   \ref TAPI_DATA_MAPPING structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Applies to a data file descriptor.The host has to take care about
   the resource management. It has to count the data channel (codec) resource
   usage and maintains a list, which data channel is mapped to which phone
   channel. The available resources can be queried by
   \ref PHONE_CAPABILITIES_CHECK.

   \code
   TAPI_DATA_MAPPING datamap;
   memset (&datamap, 0, sizeof(TAPI_DATA_MAPPING));

   // add data 0 to phone 0
   ioctl (fd0, IFXPHONE_DATA_ADD, &datamap);
   datamap.nDstCh = 1;
   // add data 0 to phone 1
   ioctl (fd0, IFXPHONE_DATA_ADD, &datamap);
   \endcode   */
#define  IFXPHONE_DATA_ADD                _IOW(TAPI_IOC_MAGIC, 0x24, int)

/** This interface removes a data channel from an analog phone device.

   \param TAPI_DATA_MAPPING* The parameter points to a
   \ref TAPI_DATA_MAPPING structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Applies to a data file descriptor.The host has to take care about
   the resource management. It has to count the data channel (codec) resource
   usage and maintains a list, which data channel is mapped to which phone
   channel. The available resources can be queried by
   \ref PHONE_CAPABILITIES_CHECK.

   \code
   TAPI_DATA_MAPPING datamap;
   memset (&datamap, 0, sizeof(TAPI_DATA_MAPPING));

   // add data 0 to phone 0
   ioctl (fd0, IFXPHONE_DATA_ADD, &datamap);
   datamap.nDstCh = 1;
   // add data 0 to phone 1
   ioctl (fd0, IFXPHONE_DATA_ADD, &datamap);
   \endcode   */
#define  IFXPHONE_DATA_REMOVE             _IOW(TAPI_IOC_MAGIC, 0x25, int)

/** This interface adds the PCM channel to an analog phone device.
   To set up a conference this interfaces is used on more PCM channels.

   \param TAPI_PCM_MAPPING* The parameter points to a
   \ref TAPI_PCM_MAPPING structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PCM_MAPPING param;
   memset (&param, 0, sizeof(TAPI_PCM_MAPPING));
   // PCM channel 1 is removed from the phone channel 2
   param.nDstCh = 2;
   // fd1 represents PCM channel 1
   ret = ioctl(fd1, IFXPHONE_PCM_REMOVE, &param)
   \endcode   */
#define  IFXPHONE_PCM_ADD                _IOW(TAPI_IOC_MAGIC, 0x43, int)

/** This interface removes the PCM channel to an analog phone device.
   To set up a conference this interfaces is used on more PCM channels.

   \param TAPI_PCM_MAPPING* The parameter points to a
   \ref TAPI_PCM_MAPPING structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PCM_MAPPING param;
   memset (&param, 0, sizeof(TAPI_PCM_MAPPING));
   // PCM channel 1 is removed from the phone channel 2
   param.nDstCh = 2;
   // fd1 represents PCM channel 1
   ret = ioctl(fd1, IFXPHONE_PCM_REMOVE, &param)
   \endcode   */
#define  IFXPHONE_PCM_REMOVE              _IOW(TAPI_IOC_MAGIC, 0x44, int)

#define  IFXPHONE_PLAY_HOLD               _IOW(TAPI_IOC_MAGIC, 0x26, int)
#define  IFXPHONE_REC_HOLD                _IOW(TAPI_IOC_MAGIC, 0x27, int)
/*@}*/

/** Set the line echo canceller (LEC) configuration

   \param TAPI_LECCONF* The parameter points to a \ref TAPI_LECCONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_LECCONF param;
   memset (&param, 0, sizeof(TAPI_LECCONF));
   param.nGainTx = TAPI_LEC_GAIN_MEDIUM;
   param.nGainRx = TAPI_LEC_GAIN_MEDIUM;
   param.nLen = 16; // 16ms
   param.bNlp = TAPI_LEC_NLPDEFAULT;
   ret = ioctl(fd, IFXPHONE_LECCONF, &param)
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_LECCONF                 _IOW(TAPI_IOC_MAGIC, 0x28, int)

/** Get the LEC configuration.

   \param TAPI_LECCONF* The parameter points to a \ref TAPI_LECCONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_GETLECCONF              _IOR(TAPI_IOC_MAGIC, 0x29, int)


/** Set the line echo canceller (LEC) configuration for PCM

   \param TAPI_LECCONF* The parameter points to a \ref TAPI_LECCONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_LECCONF param;
   memset (&param, 0, sizeof(TAPI_LECCONF));
   param.nGainTx = TAPI_LEC_GAIN_MEDIUM;
   param.nGainRx = TAPI_LEC_GAIN_MEDIUM;
   param.nLen = 16; // 16ms
   param.bNlp = TAPI_LEC_NLPDEFAULT;
   ret = ioctl(fd, IFXPHONE_LECCONF_PCM, &param)
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_LECCONF_PCM             _IOW(TAPI_IOC_MAGIC, 0x50, int)

/** Get the LEC configuration for PCM.

   \param TAPI_LECCONF* The parameter points to a \ref TAPI_LECCONF structure.

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_GETLECCONF_PCM          _IOR(TAPI_IOC_MAGIC, 0x51, int)


/** Set the AGC coefficient for a coder module
    This implementation assumes that an index of a AGC resource
    is fix assigned to the related index of the coder module.

   \param TAPI_PHONE_AGC* The parameter points to a
   \ref TAPI_PHONE_AGC structure.

   \return 0 if successful, otherwise -1 in case of an error

    \code
   TAPI_PHONE_AGC param;
   memset (&param, 0, sizeof(TAPI_PHONE_AGC));
   param.COM = 2;
   param.GAIN = 2;
   ret = ioctl(fd, IFXPHONE_AGCCONF, &param)
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_AGCCONF                 _IOW(TAPI_IOC_MAGIC, 0x35, int)

/** Get the current AGC coefficients of a coder module.

   \param TAPI_PHONE_AGC* The parameter points to a
   \ref TAPI_PHONE_AGC structure.

   \return 0 if successful, otherwise -1 in case of an error

    \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_GETAGCCONF              _IOR(TAPI_IOC_MAGIC, 0x33, int)

/** Enable / Disable the AGC resource
    This implementation assumes that the AGC resouces inside a VINETIC are
    fixed assigned to the related Coder-Module

   \param TAPI_AGC_MODE The following values (see \ref TAPI_AGC_MODE enum)
   are valid:
      - 0x0: TAPI_AGC_DISABLE, Disable AGC
      - 0x1: TAPI_AGC_ENABLE, Enable AGC

   \return 0 if successful, otherwise -1 in case of an error

    \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_AGC                     _IOW(TAPI_IOC_MAGIC, 0x34, int)

/** This interface adds the phone channel to another analog phone channel.
   To set up a conference with device internal phone this interfaces is used.

   \param TAPI_PHONE_MAPPING* The parameter points to a
   \ref TAPI_PHONE_MAPPING structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Applies to a phone channel file descriptor.
   The host has to take care about the resource management.

   \code
   TAPI_PHONE_MAPPING param;
   memset (&param, 0, sizeof(TAPI_PHONE_MAPPING));
   param.nPhoneCh = 1;
   ioctl (fd0, IFXPHONE_PHONE_ADD, &param);
   \endcode
   \ingroup TAPI_INTERFACE_CON */
#define  IFXPHONE_PHONE_ADD               _IOW(TAPI_IOC_MAGIC, 0x2A, int)

/** This interface removes a phone channel from an analog phone device.

   \param TAPI_PHONE_MAPPING* The parameter points to a
   \ref TAPI_PHONE_MAPPING structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Applies to a phone channel file descriptor.

   \code
   TAPI_PHONE_MAPPING param;
   memset (&param, 0, sizeof(TAPI_PHONE_MAPPING));
   param.nPhoneCh = 1;
   ioctl (fd0, IFXPHONE_PHONE_REMOVE, &param);
   \endcode
   \ingroup TAPI_INTERFACE_CON */
#define  IFXPHONE_PHONE_REMOVE            _IOW(TAPI_IOC_MAGIC, 0x2B, int)

/** Query status information about the phone line.

   \param TAPI_PHONE_STATUS* The parameter points to a
   \ref TAPI_PHONE_STATUS array.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If the exception bits reporting of digits is enabled
   (see \ref IFXPHONE_MASK_EXCEPTION), the digit fifo is read out once and the
   information is passed to the status. It may occur that more than one
   digit is located in the fifo. Therefore the application must check with
   \ref PHONE_DTMF_READY or \ref IFXPHONE_PULSE_READY if further data is
   contained in the fifos.
   This interface replaces the exception bits.

   \code
   // This example shows the call of the status information of one channel
   #define TAPI_STATUS_DTMF   0x01
   #define TAPI_STATUS_PULSE  0x02
   TAPI_PHONE_STATUS status[1];
   int fd;
   fd = open ("/dev/vin11", O_RDWR);
   status[0].channels = 0;
   // get the status
   ioctl(fd, IFXPHONE_PHONE_STATUS, status);
   if (status[0].dialing & TAPI_STATUS_DTMF ||
       status[0].dialing & TAPI_STATUS_PULSE)
      printf ("Dial key %d detected\n", status.digit);
   \endcode

   \code
   // This example shows a select waiting on one fd for exceptions and on
   // two others for data. In one call the status information of all two
   // channels are queried from the driver.
   TAPI_PHONE_STATUS stat[4];
   int fdcfg, fd[2], i;
   fd_set rfds;
   FD_ZERO (&rfds);
   fdcfg = open ("/dev/vin10", O_RDWR);
   FD_SET (fdcfg, &rfds);
   width = fdcfg;
   fd[0] = open ("/dev/vin11", O_RDWR);
   FD_SET (fd[0], &rfds);
   if (width < fd[0])
      width = fd[0];
   fd[1] = open ("/dev/vin12", O_RDWR);
   FD_SET (fd[1], &rfds);
   if (width < fd[1])
      width = fd[1];
   ret = select(width + 1, &rfds, NULL, NULL, NULL);
   // select woke up from exception on fdcfg or data on fd[x]
   if (FD_ISSET(fdcfg, &rfds))
   {
      // exception occurred
      status[0].channels = 2;
      // get the status
      ioctl(fdcfg, IFXPHONE_PHONE_STATUS, stat);
      for (i = 0; i < 2; i++)
      {
         if (stat[i].hook || stat[i].dialing ||
             stat[i].line || stat[i].digit || stat[i].signal)
            nState = HandleException (pCon, &stat[i]);
      }
   }
   for (i = 0; i < 2; i++)
   {
      if (FD_ISSET(fd[i], &rfds))
         HandleData (fd[i]);
   }
   // go on with state handling
   \endcode

   \code
   // This example shows a select waiting on channel fds for GR909 measurements
   // exceptions, assuming that the low level driver provides io interfaces to
   // start measurements and read results.
   TAPI_PHONE_STATUS stat[4];
   int fdcon, fd[MAX_CHANNELS], i;
   fd_set rfds;
   FD_ZERO (&rfds);
   fdcon = open ("/dev/vin10", O_RDWR);
   FD_SET (fdcon, &rfds);
   //get channel fds
   for (i = 0; i < MAX_CHANNELS, i++)
   {
      fd[i] = open (achDev[i], O_RDWR);
      if (fd [i] <= 0)
         return (-1);
      // initialize channel
      init_channel (fd[i]);
      //start first measurement
      start_measurement (fd[i], VOLTAGE);
   }
   while (1)
   {
      select(fdcon + 1, &rfds, NULL, NULL, NULL);
      // select woke up from exception on fdcon
      if (FD_ISSET(fdcon, &rfds))
      {
         // exception occurred
         status[0].channels = MAX_CHANNELS;
         // get the status
         ioctl(fdcfg, IFXPHONE_PHONE_STATUS, stat);
         for (i = 0; i < MAX_CHANNELS, i++)
         {
            if (status[i].line & TAPI_LINESTATUS_GR909RES)
            {
               //read result
               ret = read_gr909_result(fd[i], &result_struct);
               if (ret == 0)
               {
                  switch (result_struct.measurement)
                  {
                  case VOLTAGE:
                     ret = check_voltages(&result_struct);
                     if (ret == 0)
                        start_measurement (fd[i], RESISTIVE_FAULT);
                     break;
                  case RESISTIVE_FAULT:
                     ret = check_resistive_fault(&result_struct);
                     if (ret == 0)
                        start_measurement (fd[i], RECEIVER_OFFHOOK);
                     break;
                  case RECEIVER_OFFHOOK:
                     ret = check_receiver_offhook (&result_struct);
                     if (ret == 0)
                        start_measurement (fd[i], RINGER);
                     break;
                  case RINGER:
                     ret = check_ringer (&result_struct);
                     // all measurements were ok. Stop machine now.
                     if (ret == 0)
                        return (0);
                  }
                  //error: values out of allowed range. Stop machine now.
                  if (ret == -1)
                     return (-1);
               }
            }
         }
      }
   }
   \endcode

   \ingroup TAPI_INTERFACE_MISC */
#define  IFXPHONE_PHONE_STATUS            _IO (TAPI_IOC_MAGIC, 0x2C)

/** This service gets the frame length for the audio packets

   \param int* Pointer to the length of frames in milliseconds

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nFrameLength;
   ioctl(fd, IFXPHONE_GET_FRAME, &nFrameLength);
   \endcode
   \ingroup TAPI_INTERFACE_CON   */
#define  IFXPHONE_GET_FRAME               _IO(TAPI_IOC_MAGIC, 0x2D)

/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Specifies the time for hook, pulse digit and hook flash validation

   \param TAPI_VALIDATION_TIMES* The parameter points to a
   \ref TAPI_VALIDATION_TIMES structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The following conditions must be met:
      - DIGIT_LOW_TIME min and max < HOOKFLASH_TIME min and max
      - HOOKFLASH_TIME min and max < HOOKON_TIME min and max

   \code
   TAPI_VALIDATION_TIMES param;
   memset (&param, 0, sizeof(TAPI_VALIDATION_TIMES));
   // set pulse dialing
   param.nType = DIGITLOW_TIME;
   param.nMinTime = 40;
   param.nMaxTime = 60;
   ioctl(fd, IFXPHONE_VALIDATION_TIME, (int)&param);
   param.nType = DIGITHIGH_TIME;
   param.nMinTime = 40;
   param.nMaxTime = 60;
   ioctl(fd, IFXPHONE_VALIDATION_TIME, (int)&param);
   \endcode */
#define  IFXPHONE_VALIDATION_TIME         _IOW(TAPI_IOC_MAGIC, 0x2E, int)

/** Configures a tone based on simple or composed tones

   \param TAPI_TONE* The parameter points to a
   \ref TAPI_TONE structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   A simple tone specifies a tone sequence composed of several single frequency
   tones or dual frequency tones. The sequence can be transmitted only one
   time, several times or until the transmission is stopped by client.
   This interface can add a simple tone to the internal table with maximum
   222 entries, starting from TAPI_TONE_MIN_INDEX (32).
   At least on cadence must be defined otherwise this interface returns an
   error. The tone table provides all tone frequencies in 5 Hz steps with a 5%
   tolerance and are defined in RFC 2833.
   For composed tones the loopcount of each simple tone must be
   different from 0.

   \code
   int fd;
   TAPI_INIT tapi;
   TAPI_TONE tone;

   memset (&tapi,0, sizeof(TAPI_INIT));
   memset (&tone,0, sizeof(TAPI_TONE));
   fd = open (dev_nodes[1], 0);
   ioctl (fd, IFXPHONE_INIT, (int)&tapi);
   tone.simple.format = TONE_SIMPLE;
   tone.simple.index = 71;
   tone.simple.freqA = 480;
   tone.simple.freqB = 620;
   tone.simple.levelA = -300;
   tone.simple.levelB = -300;
   tone.simple.levelC = -300;
   tone.simple.cadence[0] = 2000;
   tone.simple.cadence[1] = 2000;
   tone.simple.frequencies[0] = TAPI_TONE_FREQA | TAPI_TONE_FREQB;
   tone.simple.loop = 2;
   tone.simple.pause = 200;
   ioctl (fd, IFXPHONE_TONETABLE_CONF, (int)&tone);
   memset (&tone,0, sizeof(TAPI_TONE));
   tone.composed.format = TONE_COMPOSED;
   tone.composed.index = 100;
   tone.composed.count = 2;
   tone.composed.tones[0] = 71;
   tone.composed.tones[1] = 71;
   ioctl (fd, IFXPHONE_TONETABLE_CONF, (int)&tone);
   // now start playing the simple tone
   ioctl (fd, PHONE_PLAY_TONE, (int)71);
   // stop playing tone
   ioctl (fd, PHONE_CPT_STOP, (int)0);
   \endcode
   \ingroup TAPI_INTERFACE_TONE  */
#define  IFXPHONE_TONETABLE_CONF          _IOW(TAPI_IOC_MAGIC, 0x36, int)
/*@}*/

/** Check the supported TAPI interface version

   \param TAPI_PHONE_VERSION* The parameter points to a
   \ref TAPI_PHONE_VERSION structure.

   \remarks
   Since an application is always build against one specific TAPI interface
   version it should check if it is supported. If not the application should
   abort. This interface checks if the current TAPI version supports a
   particular version. For example the TAPI versions 2.1 will support TAPI 2.0.
   But version 3.0 might not support 2.0.

   \code
   TAPI_PHONE_VERSION version;
   int ret;

   version.major = 2;
   version.minor = 1;
   if (ioctl(fd, IFXPHONE_VERSION_CHECK, (int)&version) == 0)
      printf("\nVersion 2.1 supported\n");
   \endcode

   \ingroup TAPI_INTERFACE_MISC */
#define  IFXPHONE_VERSION_CHECK           _IOW(TAPI_IOC_MAGIC, 0x39, int)

/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** Setup the CID Receiver to start receiving CID Data.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This command must be sent so that the driver can start collecting CID Data.
   Only type 1 CID FSK is supported.

   \code
   ret = ioctl(fd, IFXPHONE_CIDRX_START, 0);
   \endcode   */
#define  IFXPHONE_CIDRX_START             _IO(TAPI_IOC_MAGIC, 0x3A)

/** Stop the CID Receiver

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFXPHONE_CIDRX_STOP, 0);
   \endcode   */
#define  IFXPHONE_CIDRX_STOP              _IO(TAPI_IOC_MAGIC, 0x3B)

/** Retrieves the current status information of the CID Receiver.

   \param TAPI_CIDRX_STATUS* The parameter points to a
   \ref TAPI_CIDRX_STATUS structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Once the CID receiver is activated it signals the status with an exception.
   The exception is raised if data has been received completly or an error
   occured. Afterwards this interface is used to determine if data has been
   received. In that case it can be read with the interface
   \ref IFXPHONE_CIDRX_DATA .
   This interface can also be used without exception to determine the status
   of the receiver while reception.

   \code
   TAPI_CIDRX_STATUS cidStatus;
   memset (&cidStatus, 0, sizeof (TAPI_CIDRX_STATUS));
   ret = ioctl(fd, IFXPHONE_CIDRX_STATUS, (int)&cidStatus);
   // check if cid information are available for reading
   if ((ret == 0) && (cidStatus.nError == 0) && cidStatus.nStatus == 0x3))
   {
      printf ("Data is ready for reading\n\r");
   }
   \endcode   */
#define  IFXPHONE_CIDRX_STATUS            _IOR(TAPI_IOC_MAGIC, 0x3C, int)

/** Reads CID Data collected since caller id receiver was started.

   \param TAPI_CIDRX_DATA* The parameter points to a
   \ref TAPI_CIDRX_DATA structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This request can be sent after the CID Receiver status
   signaled that data is ready for reading or that data collection
   is ongoing. In the last case, the number of data read correspond to the
   number of data received since cid receiver was started. Once the data
   is read after the status signaled that data is ready, new data can be read
   only if CID Receiver detects new data.

   \code
   TAPI_CIDRX_STATUS cidStatus;
   TAPI_CIDRX_DATA cidData;
   memset (&cidStatus, 0, sizeof (TAPI_CIDRX_STATUS));
   memset (&cidData, 0, sizeof (TAPI_CIDRX_DATA));
   // read actual status
   ret = ioctl(fd, IFXPHONE_CIDRX_STATUS, (int)&cidStatus);
   // check if cid data are available for reading
   if ((ret == 0) && (cidStatus.nError == 0) && cidStatus.nStatus == 0x03))
      ret = ioctl(fd, IFXPHONE_CIDRX_DATA, (int)&cidData);
   \endcode   */
#define  IFXPHONE_CIDRX_DATA              _IOR(TAPI_IOC_MAGIC, 0x3D, int)



/**
   This interfaces transmits CID message. The message data and type information
   are given by TAPI_CID_INFO_t.

   \param  TAPI_CID_INFO_t*  Pointer to a \ref TAPI_CID_INFO_t,
   containing the CID / MWI information to be transmitted.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Before issuing this service, CID engine must be configured with
   \ref IFXPHONE_CIDCONF at least one time after boot. This is required to
   configure country specific settings.

   \code
   TAPI_CID_INFO_t Cid_Info;
   TAPI_CID_MSG_ELEMENT_t Msg_El[2];
   // reset and fill the caller id structure
   memset (&Cid_Info, 0, sizeof(Cid_Info));
   memset (&Msg_El, 0, sizeof(Msg_El));
   Cid_Info.txMode = TAPI_CID_ONHOOK;
   Cid_Info.messageType = TAPI_CID_MT_MWI;  // Message Waiting
   Cid_Info.nParam = 2;
   Cid_Info.pMessage = &Msg_El;
   // Mandatory for Message Waiting: set Visual Indicator on
   Msg_El[0].value.elementType = TAPI_CID_ST_VISINDIC;
   Msg_El[0].value.data = TAPI_CID_VMWI_EN;
   // Add optional CLI (number) element
   Msg_El[1].string.elementType = TAPI_CID_ST_CLI;
   Msg_El[1].string.len = ret;
   strncpy(Msg_El[1].string.data, number, sizeof(Msg_El[1].string.data));
   // transmit the caller id
   ioctl(fd, IFXPHONE_CID_INFO_TX, &Cid_Info);
   \endcode   */
#define  IFXPHONE_CID_INFO_TX             _IOW (TAPI_IOC_MAGIC, 0x3E, int)

/*@}*/

/** \addtogroup TAPI_INTERFACE_SIGNAL */
/* @{*/

/** Enables the signal detection for Fax or modem signals.

   \param TAPI_PHONE_SIGDETECTION* The parameter points to a
   \ref TAPI_PHONE_SIGDETECTION structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks check the capability list which detection service is
    available and how many detectors are available
   Signal events are reported via exception. The information is
   queried with \ref IFXPHONE_PHONE_STATUS.
   In case of AM detected the driver may automatically switches to
   modem coefficients after the ending of CED
   No every combination is possible. It depends on the under lying
   device and firmwar4e.

   \code
   TAPI_PHONE_SIGDETECTION detect;
   TAPI_PHONE_STATUS param;
   memset (&param, 0, sizeof(TAPI_PHONE_STATUS));
   memset (&detect, 0, sizeof(TAPI_PHONE_SIGDETECTION));
   detect.sig = TAPI_SIGNAL_CEDTX | TAPI_SIGNAL_PHASEREVRX;
   ioctl (fd, IFXPHONE_ENABLE_SIGDETECT, &detect);

   ioctl (fd, IFXPHONE_PHONE_STATUS, &param);
   if (param.signal & TAPI_SIGNAL_CED)
    print ("got CED from receive\n");
   \endcode  */
#define  IFXPHONE_ENABLE_SIGDETECT        _IOW(TAPI_IOC_MAGIC, 0x40, int)

/** Disables the signal detection for Fax or modem signals.

   \param TAPI_PHONE_SIGDETECTION* The parameter points to a
   \ref TAPI_PHONE_SIGDETECTION structure.

   \return 0 if successful, otherwise -1 in case of an error */
#define  IFXPHONE_DISABLE_SIGDETECT       _IOW(TAPI_IOC_MAGIC, 0x41, int)

/** Start the call progress tone detection based on a previously defined
   simple tone.

   \param TAPI_PHONE_CPTD* The parameter points to a
   \ref TAPI_PHONE_CPTD structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PHONE_CPTD param;
   memset (&param, 0, sizeof(TAPI_PHONE_CPTD));
   param.tone   = 74;
   param.signal = TAPI_CPT_SIGNALTX;
   ioctl (fd, IFXPHONE_CPTD_START, &param);
   \endcode   */
#define  IFXPHONE_CPTD_START              _IOW(TAPI_IOC_MAGIC, 0xC3, int)

/** Stops the call progress tone detection

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error
   */
#define  IFXPHONE_CPTD_STOP               _IOW(TAPI_IOC_MAGIC, 0xC4, int)

/*@}*/

/** Sets the speaker phone and microphone volume settings.

   \param TAPI_PHONE_VOLUME* The parameter points to a
   \ref TAPI_PHONE_VOLUME structure.

   This interface
    expects a structure of type \ref TAPI_PHONE_VOLUME

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PHONE_VOLUME param;
   memset (&param, 0, sizeof(TAPI_PHONE_VOLUME));
   param.nTx = 24;
   param.nRx = 0;
   ret = ioctl(fd, IFXPHONE_VOLUME, &param);
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_VOLUME                  _IOW(TAPI_IOC_MAGIC, 0x42, int)

/** Sets the PCM interface volume settings.

   \param TAPI_PHONE_VOLUME* The parameter points to a
   \ref TAPI_PHONE_VOLUME structure.

   This interface
    expects a structure of type \ref TAPI_PHONE_VOLUME

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PHONE_VOLUME param;
   memset (&param, 0, sizeof(TAPI_PHONE_VOLUME));
   param.nTx = 24;
   param.nRx = 0;
   ret = ioctl(fd, IFXPHONE_PCM_VOLUME, &param);
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_PCM_VOLUME               _IOW(TAPI_IOC_MAGIC, 0x45, int)

/** This service enables or disables a high level path of a phone channel.
   It is required to enable output signals with level higher than 3.14 dBm.

   \param int The parameter represent a boolean value of \ref TAPI_HIGH_LEVEL.

   - 0: TAPI_HIGH_LEVEL_DISABLE, disable the high level path.
   - 1: TAPI_HIGH_LEVEL_ENABLE, enable the high level path.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service is intended for phone channels only and must be used
   in combination with \ref IFXPHONE_VOLUME or \ref IFXPHONE_PCM_VOLUME to set
   the max. level (\ref TAPI_PHONE_VOLCONF_HIGH) or to restore level

   \remarks Only the order of calls with the parameters TAPI_HIGH_LEVEL_ENABLE
   and then TAPI_HIGH_LEVEL_DISABLE is supported

   \code
   ret = ioctl(fd, IFXPHONE_HIGH_LEVEL, TAPI_HIGH_LEVEL_ENABLE);
   // <play out some high level tones or samples>
   ret = ioctl(fd, IFXPHONE_HIGH_LEVEL, TAPI_HIGH_LEVEL_DISABLE);
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  IFXPHONE_HIGH_LEVEL              _IOW(TAPI_IOC_MAGIC, 0x46, int)

/*standard TAPI IOCTL Cmd (s. linux/include/telephony.h)*/
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** This service returns the number of capabilities

   \param int* Pointer to the number of capabilities which are returned

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // get the cap list size
   int ret, nCapNr;
   ret = ioctl(fd, PHONE_CAPABILITIES, &nCapNr);
   \endcode   */
#define  PHONE_CAPABILITIES               _IO (TAPI_IOC_MAGIC, 0x80)

/** This service returns the capability lists.

   \param TAPI_PHONE_CAPABILITY* Pointer to the capability list

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_PHONE_CAPABILITY *pCapList;

   // get the cap list size
   ioctl(fd, PHONE_CAPABILITIES, &nCapNr);
   pCapList = malloc (nCapNr * sizeof(TAPI_PHONE_CAPABILITY));

   // get the cap list
   ioctl(fd, PHONE_CAPABILITIES_LIST, pCapList);
   for (i=0; i < nCapSize; i++)
   {
      switch (pCapList[i].captype)
      {
           case codec:
               printf ("Codec: %s\n\r", pCapList[i].desc);
               break;
           case pcm:
              printf ("PCM: %d\n\r", pCapList[i].cap);
              break;
           case codecs:
              printf ("CODER: %d\n\r", pCapList[i].cap);
              break;
           case phones:
              printf ("PHONES: %d\n\r", pCapList[i].cap);
              break;
           default:
               break;
      }
   }
   // free the allocated memory
   free(pCapList);
   \endcode   */
#define  PHONE_CAPABILITIES_LIST          _IO (TAPI_IOC_MAGIC, 0x81)

/** This service checks if a specific capability is supported

   \param TAPI_PHONE_CAP* The parameter points to a
   \ref TAPI_PHONE_CAP structure

   \return
      - 0:  If not supported
      - 1:  If supported
      - -1: In case of an error

   \code
   TAPI_PHONE_CAPABILITY CapList;

   memset (&CapList, 0, sizeof(TAPI_PHONE_CAPABILITY));
   // check if G726, 16 kBit/s is supported
   CapList.captype = codec;
   CapList.cap = G726_16;
   ret = ioctl(fd, PHONE_CAPABILITIES_CHECK, &CapList);
   if (ret > 0)
      printf("G726_16 supported\n");
   // check how many data channels are supported
   CapList.captype = codecs;
   ret = ioctl(fd, PHONE_CAPABILITIES_CHECK, &CapList);
   if (ret > 0)
      printf("%d data channels supported\n", CapList.cap);
   // check if POTS port is available
   CapList.captype = port;
   CapList.cap = pots;
   ret = ioctl(fd, PHONE_CAPABILITIES_CHECK, &CapList);
   if (ret > 0)
      printf("POTS port supported\n");
   \endcode   */
#define  PHONE_CAPABILITIES_CHECK         _IO (TAPI_IOC_MAGIC, 0x82)
/*@}*/

/**  This service reads the hook status from the driver.

   \param int* The parameter is a pointer to the status.
   - 0: no hook detected
   - 1: hook detected

   \return 0 if successful, otherwise -1 in case of an error

   \note “off hook” corresponds to “ground start” depending on line feed mode.

   \code
   int nHook;
   ret = ioctl(fd, PHONE_HOOKSTATE, &nHook);
   switch(nHook)
   {
      case 0:
      // on hook
      break;

      case 1:
      // off hook
      break;

      default:
      // unknown state
      break;
   \endcode
   \ingroup TAPI_INTERFACE_OP */
#define  PHONE_HOOKSTATE                  _IO (TAPI_IOC_MAGIC, 0x84)

/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** This service rings the phone. The maximum number of rings has to be set by
   \ref PHONE_MAXRINGS. The service will return after the maximum number of
   rings or if the phone answered (blocking call).

   \param int This interface expects no parameter. It should be set to 0.

   \return The execution status
      - 0 if number of rings reached
      - 1 if phone was hooked off
      - -1 in case of an error

   \code
   int nMaxRing = 3, ret;
   // set the maximum rings
   ioctl(fd, PHONE_MAXRINGS, nMaxRing);
   // ring the phone
   ret = ioctl(fd, PHONE_RING, 0);
   if (ret == 0)
   {
      // no answer, maximum number of rings reached
    }
    else if (ret == 1)
    {
      // phone hooked off
    }
   \endcode   */
#define  PHONE_RING                       _IO (TAPI_IOC_MAGIC, 0x83)

/** This service sets the maximum number of rings before a blocking ringing
   is started with service \ref PHONE_RING. This service itself is non-blocking.

   \param int The parameter defines the maximum ring number

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nMaxRing = 3;
   ioctl(fd, PHONE_MAXRINGS, nMaxRing);
   \endcode   */
#define  PHONE_MAXRINGS                   _IOW (TAPI_IOC_MAGIC, 0x85, char)

/** This service sets the ring cadence for the non-blocking ringing services.
   The cadence value has to be set before ringing starts
   (\ref PHONE_RING_START). This service is non-blocking.

   \param int The parameter defines the cadence. This value contains
   the encoded cadence sequence. One bit represents ring cadence voltage
   for 0.5 sec.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The number of ring bursts can be counted via exception
   “PSTN line is ringing” of service \ref PHONE_EXCEPTION .

   \code
   // pattern of 3 sec. ring and 1 sec. pause : 1111 1100 1111 1100 ...
   WORD32 nCadence = 0xFCFCFCFC;
   // set the cadence sequence
   ioctl(fd, PHONE_RING_CADENCE, nCadence);
   \endcode   */
#define  PHONE_RING_CADENCE               _IOW (TAPI_IOC_MAGIC, 0x86, short)

/** This service starts the non-blocking ringing on the phone line and sends
   out CID.

   \param  TAPI_PHONE_CID* If the parameter is not NULL but a pointer to
   \ref TAPI_PHONE_CID CID is send out. If this structure is passed, the
   service for the CID must be set, otherwise the interface returns an
   error and no ringing is started.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The CID data must fit in the programmed appearance area otherwise an
   run time error occurs and no ringing is done. For service
   currently only CIDCS_CLI, CIDCS_DATE, CIDCS_REDIR, CIDCS_ABSCLI,
   CIDCS_NAME, CIDCS_INFO and CIDCS_TRANSPARENT are implemented.
   Either CIDCS_CLI or CIDCS_ABSCLI is mandatory.
   The ringing must be stopped with \ref PHONE_RING_STOP. A second call to
   \ref PHONE_RING_START while the phone rings returns an error.
   The ringing can be configured with the interfaces \ref IFXPHONE_CIDCONF,
   \ref IFXPHONE_DTMFCID_CONF, \ref IFXPHONE_RING_CONFIG,
   \ref  PHONE_RING_CADENCE and \ref IFXPHONE_RING_CADENCE_HIGH_RES before
   this interface is called.

   \code
   TAPI_PHONE_CID Cid;
   // fill the caller id structure
   memset (&Cid, 0, sizeof(TAPI_PHONE_CID));
   sprintf(Cid.month,"%d",nMonth);
   sprintf(Cid.day, "%d",nDay);
   sprintf(Cid.hour, "%d", nHour);
   sprintf(Cid.min, "%d", nMin);
   Cid.numlen = min(strlen(pNumber), 11);
   strncpy(Cid.number, pNumber, 11);
   Cid.namelen = min(strlen(pName),80);
   strncpy(Cid.name, pName, 80);
   // present name and date
   Cid.service = CIDCS_CLI | CIDCS_NAME | CIDCS_DATE;
   // start the ringing and transmit the caller id
   ioctl(fd, PHONE_RING_START, &Cid);
   \endcode   */
#define  PHONE_RING_START                 _IOW (TAPI_IOC_MAGIC, 0x87, int)

/** This service stops non-blocking ringing on the phone line which was
   started before with service \ref PHONE_RING_START. This service is
   non-blocking.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, PHONE_RING_STOP, 0);
   \endcode   */
#define  PHONE_RING_STOP                  _IO (TAPI_IOC_MAGIC, 0x88)
/*@}*/

/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Select a codec for the data channel

   \param TAPI_PHONE_CODEC The parameter specifies the codec as defined
   in \ref TAPI_PHONE_CODEC. Default codec is G711, u-Law

   \return error code
      - 0 if successful,
      - -1 in case of an error

   \code
   // set the codec
   ioctl(fd, PHONE_REC_CODEC, G726_16);
   \endcode  */
#define  PHONE_REC_CODEC                  _IOW (TAPI_IOC_MAGIC, 0x89,int)

/** Start recording on the data channel. Enables the encoder.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The voice is serviced with the drivers read and write interface.
   The data format is dependent on the selected setup (coder, chip setup,
   etc.). For example, a RTP setup will receive RTP packets. Read and
   write are always non-blocking.
   If the codec has not been set before with \ref PHONE_REC_CODEC the
   encoder is started with G711

   \code
   ioctl(fd, PHONE_REC_START, 0);
   \endcode */
#define  PHONE_REC_START                  _IO (TAPI_IOC_MAGIC, 0x8A)

/** Start recording (generating packets) on this channel

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, PHONE_REC_STOP, 0);
   \endcode   */
#define  PHONE_REC_STOP                   _IO (TAPI_IOC_MAGIC, 0x8B)

#define  PHONE_REC_DEPTH                  _IOW (TAPI_IOC_MAGIC, 0x8C, int)

/** This service sets the frame length for the audio packets

   \param int Length of frames in milliseconds

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nFrameLength = 10;
   ioctl(fd, PHONE_FRAME, nFrameLength);
   \endcode   */
#define  PHONE_FRAME                      _IOW (TAPI_IOC_MAGIC, 0x8D, int)

/** Sets the recording volume

   \param int Recording volume, default 0x100

   \return 0 if successful, otherwise -1 in case of an error

   \note Not supported

   \code
   ioctl(fd, PHONE_REC_VOLUME, 0x100 * 2);
   \endcode   */
#define  PHONE_REC_VOLUME                 _IOW (TAPI_IOC_MAGIC, 0x8E, int)
#define  PHONE_REC_VOLUME_LINEAR          _IOW (TAPI_IOC_MAGIC, 0xDB, int)

/** This service returns the level of the most recently recorded signal.
   Value 0x100 describes the default record level of -38.79 dB. Maximum
   value is 0x7FFF which corresponds to a record level of approximately 3 dB.

   \param int* Pointer to an integer for the level

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not implemented!

   \code
   int nRecLevel;
   //  get the record level
   ret = ioctl(fd, PHONE_REC_LEVEL, &nRecLevel);
   // output level in dB can be calculated via formula:
   // outlvl_in_dB = +3.17 + 20 log10 (nRecLevel/32767)
   \endcode   */
#define  PHONE_REC_LEVEL                  _IO (TAPI_IOC_MAGIC, 0x8F)

/** Select a codec for the data channel playout

   \param \ref TAPI_PHONE_CODEC The parameter specifies the codec, default G711

   \return error code
      - 0 if successful,
      - -1 in case of an error
   \remarks This interface is currently not supported, because the codec
   is determined by the payload type of the received packets */
#define  PHONE_PLAY_CODEC                 _IOW (TAPI_IOC_MAGIC, 0x90, int)

/** Starts the playout of data

   \note Currently not implemented!

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, PHONE_PLAY_START, 0);
   \endcode   */
#define  PHONE_PLAY_START                 _IO (TAPI_IOC_MAGIC, 0x91)

/** Stops the playout of data

   \note Currently not implemented!

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, PHONE_PLAY_STOP, 0);
   \endcode   */
#define  PHONE_PLAY_STOP                  _IO (TAPI_IOC_MAGIC, 0x92)

#define  PHONE_PLAY_DEPTH                 _IOW (TAPI_IOC_MAGIC, 0x93, int)

/** Sets the playout volume

   \param int Playout volume, default 0x100

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not supported

   \code
   ioctl(fd, PHONE_PLAY_VOLUME, 0x100 * 2);
   \endcode   */
#define  PHONE_PLAY_VOLUME                _IOW (TAPI_IOC_MAGIC, 0x94, int)
#define  PHONE_PLAY_VOLUME_LINEAR         _IOW (TAPI_IOC_MAGIC, 0xDC, int)

/** This service returns the level of the most recently played signal.
   Value 0x100 describes the default record level of -38.79 dB. Maximum
   value is 0x7FFF which corresponds to a record level of approximately 3 dB.

   \param int* Pointer to an integer for the level

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not implemented!

   \code
   int nLevel;
   //  get the record level
   ret = ioctl(fd, PHONE_PLAY_LEVEL, &nLevel);
   // output level in dB can be calculated via formula:
   // outlvl_in_dB = +3.17 + 20 log10 (nRecLevel/32767)
   \endcode   */
#define  PHONE_PLAY_LEVEL                 _IO (TAPI_IOC_MAGIC, 0x95)
/*@}*/

/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/
/** This service checks if a DTMF digit was received.

   \param int* Pointer to an integer for the status information.
   It returns the following values:

   - 0: No DTMF digit is in the receive queue
   - 1: DTMF digit is in the receive queue

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFXPHONE_PHONE_STATUS can be used upon exception. If the
   event reporting mask is not set (see \ref IFXPHONE_MASK_EXCEPTION),
   \ref IFXPHONE_PHONE_STATUS reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int bReady;
   //  check the DTMF status
   ioctl(fd, PHONE_DTMF_READY, &bReady);
   if (bReady == 1)
   {
      //  digit arrived
   }
   else
   {
      //  no digit in the buffer
   }
   \endcode   */
#define  PHONE_DTMF_READY                 _IOR (TAPI_IOC_MAGIC, 0x96, int)

/** This service reads the DTMF digit out of the internal receive buffer.
   After reading the digit is removed from the internal receive buffer.
   The receive buffer contains 20 digits.
   \param int* The parameter points to the detected digit.
   It returns the following values:

   - 0: no digit detected
   - 1: Digit 1
   - ...
   - 9: Digit 9
   - A: * (star)
   - B: 0
   - C: # (pound)
   - 1C: A
   - 1D: B
   - 1E: C
   - 1F: D

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFXPHONE_PHONE_STATUS can be used upon exception. If the
   event reporting mask is not set (see \ref IFXPHONE_MASK_EXCEPTION),
   \ref IFXPHONE_PHONE_STATUS reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int nDigit, ret;
   //  get DTMF digit
   ret = ioctl(fd, PHONE_GET_DTMF, &nDigit);
   \endcode   */
#define  PHONE_GET_DTMF                   _IOR (TAPI_IOC_MAGIC, 0x97, int)

/** This service reads the ASCII character representing the DTMF digit out of
   the receive buffer. After reading the digit is removed from the
   receive buffer.
   \param int* The parameter points to the detected digit in ASCII
   representation.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int32 nDigit, ret;
   //  get DTMF digit
   ret = ioctl(fd, PHONE_GET_DTMF_ASCII, &nDigit);
   \endcode   */
#define  PHONE_GET_DTMF_ASCII             _IOR (TAPI_IOC_MAGIC, 0x98,int)

/** This service controls the DTMF sending mode. The DTMF may be sent
   out-of-band (OOB) or in-band.
   \param int The parameter has the following values:

   - 0: TAPI_OOB_INOUT, DTMF is send in-band and out-of-band
   - 1: TAPI_OOB_OUT, DTMF is send only out-of band
   - 2: TAPI_OOB_IN, DTMF is send only in-band

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The DTMF out of band controls how events are reported. If switched
   on (TAPI_OOB_OUT or TAPI_OOB_INOUT) events are reported according to
   RFC 2833. If switched off (TAPI_OOB_IN) the events are coded as voice.

   \code
   //  set the DTMF sending mode to out of band
   ioctl(fd, PHONE_DTMF_OOB, TAPI_OOB_OUT);
   \endcode   */
#define  PHONE_DTMF_OOB                   _IOW (TAPI_IOC_MAGIC, 0x99,int)
/*@}*/

/**
   This service returns the current exception status.

   \param TAPI_EXCEPTION* The parameter points to a
   \ref TAPI_EXCEPTION structure.

   \remarks
   The user application must poll cidrx_supdate, once the CID Receiver was
   started with IFXPHONE_CIDRX_START. If it is set the application
   calls IFXPHONE_CIDRX_STATUS in order to know if CID data is
   available for reading or if an error occured during CID Reception.

   \code
   TAPI_EXCEPTION nException;
   //  get the exception
   nException.Status = ioctl(fd, PHONE_EXCEPTION, 0);
   if (nException.Bits.dtmf_ready)
      printf ("DTMF detected\n");
   \endcode
   \ingroup TAPI_INTERFACE_MISC */
#define  PHONE_EXCEPTION                  _IOR (TAPI_IOC_MAGIC, 0x9A, int)

/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Tone is played out on default source */
#define TAPI_TONE_SRC_DEFAULT 0
/** Tone is played out on DSP, default, if available */
#define TAPI_TONE_SRC_DSP 0x4000
/** Tone is played out on local tone generator in the analog part of the
 device. Default if DSP is not available */
#define TAPI_TONE_SRC_TG 0x8000

#define TAPI_TONE_MIN_INDEX 32
#define TAPI_TONE_MAX_INDEX 255

/** Plays a tone, which is defined before.

   \param int The parameter is the index of the tone to the predefined tone
   table (range 1 - 31) or custom tones added previously
   (index TAPI_TONE_MIN_INDEX - TAPI_TONE_MAX_INDEX).
   Index 0 means tone stop. Using the upper bits modify the
   default tone playing source:

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This can be a predefined, simple or a composed tone.
   The tone codes are assigned previously on system start.
   Index 1 - 31 is predefined by the driver and covers the
   original TAPI. A timing must be specified with
   \ref PHONE_SET_TONE_ON_TIME and \ref PHONE_SET_TONE_OFF_TIME
   before.
   All other indicies can be custom defined by
   \ref IFXPHONE_TONETABLE_CONF.

   \code
   // play tone index 34
   ioctl(fd, PHONE_PLAY_TONE, 34);
   \endcode   */
#define PHONE_PLAY_TONE                  _IOW (TAPI_IOC_MAGIC, 0x9B, char)
#define IFXPHONE_TONE_PLAY                PHONE_PLAY_TONE

/** Plays a tone to the network side, which is defined before. Applies
   to data file descriptor only.

   \param int The parameter is the index of the tone to the predefined tone
   table (range 1 - 31) or custom tones added previously
   (index TAPI_TONE_MIN_INDEX - TAPI_TONE_MAX_INDEX).
   Index 0 means tone stop. Using the upper bits modify the
   default tone playing source:

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This can be a predefined, simple or a composed tone.
   The tone codes are assigned previously on system start.
   Index 1 - 31 is predefined by the driver and covers the
   original TAPI. A timing must be specified with
   \ref PHONE_SET_TONE_ON_TIME and \ref PHONE_SET_TONE_OFF_TIME
   before.
   All other indicies can be custom defined by
   \ref IFXPHONE_TONETABLE_CONF.

   \code
   // play tone index 34
   ioctl(fd, IFXPHONE_TONE_PLAY_NET, 34);
   \endcode   */
#define IFXPHONE_TONE_PLAY_NET              _IOW (TAPI_IOC_MAGIC, 0xC5, char)

/**
   This service sets the on-time for a tone with a specified duration.
   After setting tone on/off time, the tone is started with service
   \ref PHONE_PLAY_TONE.

   \param int The parameter is equal to the duration of the tone in
   0.25 ms steps. The maximum value is 65535.

   - 0: not valid (default)
   - 1: 0.25 ms
   - 2: 0.5 ms
   - ...
   - 65535: 16383.75 ms

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This interface must be called before using tone services.

   \code
   int nDuration = 0x190;
   // set tone on-time with a 100 ms duration
   ioctl(fd, PHONE_SET_TONE_ON_TIME, nDuration);
   \endcode   */
#define  PHONE_SET_TONE_ON_TIME           _IOW (TAPI_IOC_MAGIC, 0x9C, int)

/**
   This service sets the off-time for a tone with a specified duration.
   After setting tone on/off time, the tone is started with service
   \ref PHONE_PLAY_TONE.

   \param int The parameter is equal to the duration of the tone in
   0.25 ms steps.

   - 0: not valid (default)
   - 1: 0.25 ms
   - 2: 0.5 ms
   - ...
   - 65535: 16383.75 ms

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nDuration = 0x190;
   // set tone off-time with a 100 ms duration
   ioctl(fd, PHONE_SET_TONE_OFF_TIME, nDuration);
   \endcode   */
#define  PHONE_SET_TONE_OFF_TIME          _IOW (TAPI_IOC_MAGIC, 0x9D, int)
/**
   This service gets the current on-time duration which was set by a
   former \ref PHONE_SET_TONE_ON_TIME

   \param int The parameter points to the  duration of the tone
   in 0.25 ms steps.

   - 0: not valid (default)
   - 1: 0.25 ms
   - 2: 0.5 ms
   - ...
   - 65535: 16383.75 ms

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nDuration, ret;
   // get on-time duration
   ret = ioctl(fd, PHONE_GET_TONE_ON_TIME, &nDuration);
   \endcode   */
#define  PHONE_GET_TONE_ON_TIME           _IO (TAPI_IOC_MAGIC, 0x9E)
/**
   This service gets the current off-time duration which was set by a
   former \ref PHONE_SET_TONE_OFF_TIME

   \param int The parameter is equal to the duration of the tone in
   0.25 ms steps. The maximum value is 65535.

   - 0: not valid (default)
   - 1: 0.25 ms
   - 2: 0.5 ms
   - ...
   - 65535: 16383.75 ms

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nDuration, ret;
   // get on-time duration
   ret = ioctl(fd, PHONE_GET_TONE_OFF_TIME, &nDuration);
   \endcode   */
#define  PHONE_GET_TONE_OFF_TIME          _IO (TAPI_IOC_MAGIC, 0x9F)

/** This service gets the tone playing state.

   \param int* The parameter points to the tone state

   - 0: no tone is played
   - 1: tone is played (tone is within on-time)
   - 2: silence (tone is within off-time)

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nToneState;
   // get the tone state
   ret = ioctl(fd, PHONE_GET_TONE_STATE, &nToneState);
   \endcode   */
#define  PHONE_GET_TONE_STATE             _IO (TAPI_IOC_MAGIC, 0xA0)

/** Pre-defined tone services for busy tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, PHONE_BUSY, 0);
   \endcode   */
#define  PHONE_BUSY                       _IO (TAPI_IOC_MAGIC, 0xA1)

/** Pre-defined tone services for ring back tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, PHONE_BUSY, 0);
   \endcode   */
#define  PHONE_RINGBACK                   _IO (TAPI_IOC_MAGIC, 0xA2)
/**
   Pre-defined tone services for dial tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, PHONE_BUSY, 0);
   \endcode   */
#define  PHONE_DIALTONE                   _IO (TAPI_IOC_MAGIC, 0xA3)
/**
   Stops playback of the current tone (call progress tone), or cadence.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Stopping tone can also be done with \ref PHONE_PLAY_TONE and
   index 0

   \code
   // start dial tone
   ioctl(fd, PHONE_DIALTONE, 0);
   // stop playing tone
   ioctl(fd, PHONE_CPT_STOP, 0);
   \endcode   */
#define  PHONE_CPT_STOP                   _IO (TAPI_IOC_MAGIC, 0xA4)
#define IFXPHONE_TONE_STOP                PHONE_CPT_STOP

/*@}*/
#define  PHONE_PSTN_SET_STATE             _IOW (TAPI_IOC_MAGIC, 0xA4, int)
#define  PHONE_PSTN_GET_STATE             _IO (TAPI_IOC_MAGIC, 0xA5)
#define  PHONE_WINK_DURATION              _IOW (TAPI_IOC_MAGIC, 0xA6, int)
#define  PHONE_WINK                       _IOW (TAPI_IOC_MAGIC, 0xAA, int)
/* ToDo: Include after implementation */
/* #define  PHONE_QUERY_CODEC             _IOWR (TAPI_IOC_MAGIC, 0xA7, struct phone_codec_data *) */
#define  PHONE_QUERY_CODEC                _IO (TAPI_IOC_MAGIC, 0xA7)
#define  PHONE_PSTN_LINETEST              _IO (TAPI_IOC_MAGIC, 0xA8)

/** Configures the voice activity detection and silence handling
   Voice Activity Detection (VAD) is a feature that allows the
   codec to determine when to send voice data or silence data.
   When the record level falls below a certain threshold, the audio frames are
   replaced by comfort noise frames. These comfort noise frames contain less
   significant data, which means that the application can use less network
   bandwidth when sending the data across the network. For other codecs,
   this must be done in software by analyzing the record level.

   \param TAPI_VAD The following values are valid:

   - 0: TAPI_VAD_NOVAD, voice activity detection
   - 1: TAPI_VAD_VAD_ON, voice activity detection switched on
   - 2: TAPI_VAD_G711, voice activity detection switched on and comfort
    noise generation activated, applies to G.711

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Codecs G.723.1 and G.729A/B have a built in comfort noise generation (CNG).
   Explicitly switching on the CNG is not necessary. Voice activity detection
   is not available for G.728 and G.729E.

   \code
   // set the VAD on
   ioctl(fd, PHONE_VAD, TAPI_VAD_ON);
   \endcode
   \ingroup TAPI_INTERFACE_CON */
#define  PHONE_VAD                        _IOW (TAPI_IOC_MAGIC, 0xA9, int)

/** \addtogroup TAPI_INTERFACE_CID  */
/*@{*/

/** Set the CID type 2 configuration
   \param TAPI_CID2_CONFIG* The parameter points to a
   \ref TAPI_CID2_CONFIG structure.

   \return 0 if successful, otherwise -1 in case of an error.
   An error occurs if the parameter out of range or the CID2 ongoing on
   the channel

   \code
   TAPI_CID2_CONFIG param;
   int ret;
   param.stdSelect = TAPI_CID2_TIA;
   param.std.defaultS.ackTone = 'D';
   param.std.defaultS.casDuration = 80;
   param.std.defaultS.cas2ackTime = 100;
   param.std.defaultS.ack2fskOutTime = 500;
   ret = ioctl (fd, IFXPHONE_CID2_CONF, (int)&param);
   \endcode */
#define  IFXPHONE_CID2_CONF               _IOW(TAPI_IOC_MAGIC, 0xC0, int)

/** Get the CID type 2 configuration

 \param TAPI_CID2_CONFIG* The parameter points to a
 \ref TAPI_CID2_CONFIG structure.

 \return 0 if successful, otherwise -1 in case of an error.
 An error occurs if the parameter out of range or the CID2 ongoing on
 the channel  */
#define  IFXPHONE_CID2_GETCONF            _IO(TAPI_IOC_MAGIC, 0xC1)

/** Send CID type 2 data

   \param TAPI_PHONE_CID* The parameter points to a
   \ref TAPI_PHONE_CID structure.

   \return 0 if successful, otherwise -1 in case of an error.
   An error occurs if the parameter out of range or the CID2 ongoing on
   the channel

   \remarks
   The call to this ioctl is non blocking. Timers are used to check the
   time requirements. The CID receiver device must issue an acknowledge
   signal before FSK data is sent out.
   The CID receiver device used MUST support CID2 (Call Waiting feature).
   The maximum length of the data buffer is 255 bytes.

   \code
   TAPI_PHONE_CID cidData;
   int ret;
   char pValidFskData [] = {  0x80, 0x0A, 0x01, 0x08, 0x30, 0x37, 0x33,
                              0x30, 0x30, 0x38, 0x32, 0xD6 };
   // map transparent pointer to valid FSK buffer
   cidData.pData =
      (char*)pValidFskData;cidData.datalen = sizeof (pValidFskData);
   ret = ioctl (fd, IFXPHONE_CID2_SEND, (int)&cidData);
   \endcode */
#define  IFXPHONE_CID2_SEND               _IOW(TAPI_IOC_MAGIC, 0xC2, int)

/**  calling line identity (mandatory) */
#define  CIDCS_CLI        0x00000001
/** date and time presentation */
#define  CIDCS_DATE       0x00000002
/** called line identity */
#define  CIDCS_CDLI       0x00000004
/** reason for absence of CLI */
#define  CIDCS_ABSCLI     0x00000008
/** complementary calling line identity */
#define  CIDCS_CCLI       0x00000010
/** call type */
#define  CIDCS_CT         0x00000020
/** first called line identity  */
#define  CIDCS_FIRSTCLI   0x00000040
/** number of messages  */
#define  CIDCS_MSGNR      0x00000080
/** type of forwarded call  */
#define  CIDCS_FWCT       0x00000100
/** type of calling user  */
#define  CIDCS_USRT       0x00000200
/** number redirection, service 1AH */
#define  CIDCS_REDIR      0x00000400
/** network provider id */
#define  CIDCS_NTID       0x00000800
/** carrier identity */
#define  CIDCS_CARID      0x00001000
/** selection of terminal function */
#define  CIDCS_TERMSEL    0x00002000
/** display information */
#define  CIDCS_DISP       0x00004000
/** extension for operator use */
#define  CIDCS_XOPUSE     0x00008000
/** calling line name */
#define  CIDCS_NAME       0x00010000
/** reason for absence of name */
#define  CIDCS_ABSNAME    0x00020000
/** information values for DTMF*/
#define  CIDCS_INFO       0x00040000
/** the CID data is passed transparently in pData, no interpretation is done */
#define  CID_TRANSPARENT  0x80000000
/*@}*/

/* ============================= */
/* Tapi initialization modes     */
/* ============================= */
/** Tapi Initialization modes, selection for target system.
   If different modes are supported by the implementation, this parameter
   specifies which mode should be set up. The meaning of the mode is dependent
   on the implementation.
   \ingroup TAPI_INTERFACE_INIT */
typedef enum
{
   /** Default initialization */
   TAPI_DEFAULT = 0,
   /** Typical VoIP solution. Phone connected to a packet coder (data channel)
    with DSP features for signal detection */
   TAPI_VOICE_CODER = 1,
   /** Phone to PCM using DSP features for signal detection */
   TAPI_PCM_DSP = 2,
   /** Phone to PCM connection without DSP features */
   TAPI_PCM_PHONE = 3,
   /** No special initialization, for testing purposes */
   TAPI_NONE = 0xff
}TAPI_INIT_MODES;

/** Country selection for TAPI_Init. For future purposes, not yet used.
   If different countries are supported by the implementation, this parameter
   specifies which one.
   \ingroup TAPI_INTERFACE_INIT */
typedef enum
{
   /** Default contry */
   TAPI_COUNTRY_DEFAULT = 0,
   /** Germany */
   TAPI_COUNTRY_DE = 1,
   /** USA */
   TAPI_COUNTRY_US = 2,
   /** United Kingdom */
   TAPI_COUNTRY_UK = 3
}TAPI_COUNTRY;

/** Defines for linefeeding */
typedef enum
{
   /** Normal feeding mode for phone off hook */
   TAPI_LINEFEED_NORMAL = 0,
   /** Normal feeding mode for phone off hook reversed */
   TAPI_LINEFEED_REVERSED = 1,
   /** Power down resistance = on hook with hook detection */
   TAPI_LINEFEED_PARKED = 2,
   /** Switch off the line, but the device is able to test the line */
   TAPI_LINEFEED_HIGH_IMPEDANCE = 3,
   /** Switch off the line and the device */
   TAPI_LINEFEED_POWER_DOWN = 4,
   /** */
   TAPI_LINEFEED_GROUND_START = 5,
   /** Thresholds for automatic battery switch are set via
   coefficient settings */
   TAPI_LINEFEED_NORMAL_AUTO = 6,
   /** Thresholds for automatic battery switch are set via coefficient
   settings reversed */
   TAPI_LINEFEED_REVERSED_AUTO = 7,
   /** feeding mode for phone off hook with low battery to save power */
   TAPI_LINEFEED_NORMAL_LOW = 8,
   /** feeding mode for phone off hook with low battery to save power
       and reserved polarity */
   TAPI_LINEFEED_REVERSED_LOW = 9,
   /** Reserved, needed for ring call back function */
   TAPI_LINEFEED_RING_BURST = 10,
   /** Reserved, needed for ring call back function */
   TAPI_LINEFEED_RING_PAUSE = 11,
   /** Reserved, needed for internal function */
   TAPI_LINEFEED_METER = 12,
   /** Reserved, special test linemode */
   TAPI_LINEFEED_ACTIVE_LOW = 13,
   /** Reserved, special test linemode */
   TAPI_LINEFEED_ACTIVE_BOOSTED = 14,
   /** Reserved, special linemode for S-MAX Slic */
   TAPI_LINEFEED_ACT_T = 15,
   /** Reserved, special linemode for S-MAX Slic */
   TAPI_LINEFEED_POWER_DOWN_RESISTIVE_SWITCH = 16,
   /** Power down resistance = on hook with hook detection */
   TAPI_LINEFEED_PARKED_REVERSED = 17
}TAPI_LINEFEED;

/*======================*/
/* Defines for Metering */
/*======================*/

/** Metering modes */
typedef enum
{
   /** Normal TTX mode */
   TAPI_METER_MODE_TTX = 0,
   /** Reverse polarity mode */
   TAPI_METER_MODE_REVPOL = 1
}TAPI_METER_MODE;

/** Metering frequency */
typedef enum
{
   /** Device default */
   TAPI_METER_FREQ_DEFAULT = 0,
   /** 12 kHz */
   TAPI_METER_FREQ_12_KHZ = 1,
   /** 16 kHz */
   TAPI_METER_FREQ_16_KHZ = 2
}TAPI_METER_FREQ;

/*====================*/
/* Defines for Codecs */
/*====================*/

/** Possible codecs to select for \ref PHONE_REC_CODEC and \ref TAPI_AAL_PROFILE
   \ingroup TAPI_INTERFACE_CON */
typedef enum
{
   /** G723, 6.3 kBit/s */
   G723_63  = 1,
   /** G723, 5.3 kBit/s */
   G723_53  = 2,
   /** G728, 16 kBit/s */
   G728     = 6,
   /** G729 A and B, 8 kBit/s */
   G729     = 7,
   /** G711 u-Law, 64 kBit/s */
   MLAW     = 8,
   /** G711 A-Law, 64 kBit/s */
   ALAW     = 9,
   /** G726, 16 kBit/s */
   G726_16  = 12,
   /** G726, 24 kBit/s */
   G726_24  = 13,
   /** G726, 32 kBit/s */
   G726_32  = 14,
   /** G726, 40 kBit/s */
   G726_40  = 15,
   /** G729 E, 11.8 kBit/s */
   G729_E   = 16
}TAPI_PHONE_CODEC;

#define MAX_CODECS (G729_E + 1)

/*====================*/
/* Defines for Phone  */
/* Mapping            */
/*====================*/

/** Type channel for mapping */
typedef enum
{
   /** Default. It depends on the device and configures the best applicable */
   TAPI_MAPTYPE_DEFAULT = 0,
   /** Type is a coder packet channel */
   TAPI_MAPTYPE_CODER = 1,
   /** Type is a PCM channel */
   TAPI_MAPTYPE_PCM = 2
}TAPI_MAPTYPE;

/** Recording enabling and disabling information */
typedef enum
{
   /** Do not modify recording status */
   TAPI_MAPREC_NOMODIF = 0,
   /** Start recording after mapping */
   TAPI_MAPREC_START = 1,
   /** Stop recording after mapping */
   TAPI_MAPREC_STOP = 2
}TAPI_MAPREC;

/** Play out enabling and disabling information */
typedef enum
{
   /** Do not modify playing status */
   TAPI_MAPPLAY_NOMODIF = 0,
   /** Start playing after mapping */
   TAPI_MAPPLAY_START = 1,
   /** Stop playing after mapping */
   TAPI_MAPPLAY_STOP = 2
}TAPI_MAPPLAY;

/*======================*/
/* Defines for Voice    */
/* activation Detection */
/*======================*/

/** Enumeration used for ioctl \ref PHONE_VAD
 \ingroup TAPI_INTERFACE_CON */
typedef enum
{
   /** No voice activity detection */
   TAPI_VAD_NOVAD = 0,
   /** Voice activity detection on, in this case also comfort noise and
      spectral information (nicer noise) is switched on */
   TAPI_VAD_VAD_ON = 1,
   /** Voice activity detection on with comfort noise generation without
       spectral information */
   TAPI_VAD_G711 = 2
}TAPI_VAD;

/*==================*/
/* Defines for lec */
/*==================*/

/** LEC gain levels */
typedef enum
{
   /** Turn LEC off */
   TAPI_LEC_GAIN_OFF = 0,
   /** Turn LEC on to low level*/
   TAPI_LEC_GAIN_LOW = 1,
   /** Turn LEC on to normal level*/
   TAPI_LEC_GAIN_MEDIUM = 2,
   /** Turn LEC on to high level*/
   TAPI_LEC_GAIN_HIGH = 3
}TAPI_LEC_GAIN;

/* LEC delay line max/min length */
#define TAPI_LEC_MIN_LEN         4  /* ms, ref : Tapi Spec*/
#define TAPI_LEC_MAX_LEN         16 /* ms, ref : Tapi Spec */

/*====================*/
/* Defines for Jitter */
/*   Buffer Types     */
/*====================*/
enum
{
   FIXED_JB_TYPE   = 0x1,
   ADAPTIVE_JB_TYPE
};

/* ============================= */
/* Defines for CID config        */
/* ============================= */

/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** CID Specifications */
typedef enum
{
   /** CID BELLCORE (USA)*/
   TAPI_CID_BELLCORE = 0,
   /** CID1 ETSI (Europe)*/
   TAPI_CID_V23 = 1,
   /** CID NTT (Japan)*/
   TAPI_CID_NTT = 2
} TAPI_CID_SPEC;

/** CID level  */
typedef enum
{
   /** Low level CID generation */
   TAPI_CID_LOWLEVEL = 0,
   /** High level CID generation */
   TAPI_CID_HIGHLEVEL = 1
} TAPI_CID_LEVEL;

/** CID generation modes */
typedef enum
{
   /** CID is sent with FSK */
   TAPI_CID_FSK_MODE = 0,
   /** CID is sent with DTMF */
   TAPI_CID_DTMF_MODE = 1
} TAPI_CID_MODE;

/** CID appearance */
typedef enum
{
   /** CID is sent before ring bursts */
   CID_BEFORE_RINGBURST = 0,
   /** CID is sent between periodical ring bursts */
   CID_BETWEEN_RINGBURST = 1,
   /** CID is sent after initial ringing */
   CID_AFTER_DELAY = 2
} TAPI_CID_APPEARANCE;

/** CID2 Standards */
typedef enum
{
   /** CID2 TIA Standard */
   TAPI_CID2_TIA = 0,
   /** CID2 ETSI Standard */
   TAPI_CID2_ETSI = 1,
   /** CID2 BT Standard */
   TAPI_CID2_BT = 2,
   /** CID2 Japan Standard */
   TAPI_CID2_NTT = 3
   /* expand list of standards here */
} TAPI_CID2_STD;

/** Caller ID transmission modes */
typedef enum
{
   /** On-hook transmission. Applicable to CID type 1 and MWI */
   TAPI_CID_ONHOOK   = 0x00,
   /** Off-hook transmission. Applicable to CID type 2 and MWI */
   TAPI_CID_OFFHOOK  = 0x01
} TAPI_CID_HOOK_MODE_t;

/** Caller ID message types (defined in ETSI EN 300 659-3) */
typedef enum
{
   /** Call Set-up. Corresponds to Caller ID type 1 and type 2. */
   TAPI_CID_MT_CSUP  = 0x80,
   /** Message Waiting Indicator */
   TAPI_CID_MT_MWI   = 0x82,
   /** Advice of Charge */
   TAPI_CID_MT_AOC   = 0x86,
   /** Short Message Service */
   TAPI_CID_MT_SMS   = 0x89,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES01 = 0xF1,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES02 = 0xF2,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES03 = 0xF3,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES04 = 0xF4,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES05 = 0xF5,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES06 = 0xF6,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES07 = 0xF7,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES08 = 0xF8,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES09 = 0xF9,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0A = 0xFA,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0B = 0xFB,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0C = 0xFC,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0D = 0xFD,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0E = 0xFE,
   /** Reserved for Network Operator Use */
   TAPI_CID_MT_RES0F = 0xFF
} TAPI_CID_MESSAGE_TYPE_t;

/** Caller ID Services (defined in ETSI EN 300 659-3) */
typedef enum
{
   /** date and time presentation */
   TAPI_CID_ST_DATE        = 0x01,
   /** calling line identity (mandatory) */
   TAPI_CID_ST_CLI         = 0x02,
   /** called line identity */
   TAPI_CID_ST_CDLI        = 0x03,
   /** reason for absence of CLI */
   TAPI_CID_ST_ABSCLI      = 0x04,
   /** calling line name */
   TAPI_CID_ST_NAME        = 0x07,
   /** reason for absence of name */
   TAPI_CID_ST_ABSNAME     = 0x08,
   /** visual indicator */
   TAPI_CID_ST_VISINDIC    = 0x0B,
   /** Message Identification */
   TAPI_CID_ST_MSGIDENT    = 0x0D,
   /** Last Message CLI */
   TAPI_CID_ST_LMSGCLI     = 0x0E,
   /** Complementary Date and Time */
   TAPI_CID_ST_CDATE       = 0x0F,
   /** complementary calling line identity */
   TAPI_CID_ST_CCLI        = 0x10,
   /** call type */
   TAPI_CID_ST_CT          = 0x11,
   /** first called line identity  */
   TAPI_CID_ST_FIRSTCLI    = 0x12,
   /** number of messages  */
   TAPI_CID_ST_MSGNR       = 0x13,
   /** type of forwarded call  */
   TAPI_CID_ST_FWCT        = 0x15,
   /** type of calling user  */
   TAPI_CID_ST_USRT        = 0x16,
   /** number redirection */
   TAPI_CID_ST_REDIR       = 0x1A,
   /** charge */
   TAPI_CID_ST_CHARGE      = 0x20,
   /** additional charge */
   TAPI_CID_ST_ACHARGE     = 0x21,
   /** duration of the call */
   TAPI_CID_ST_DURATION    = 0x23,
   /** network provider id */
   TAPI_CID_ST_NTID        = 0x30,
   /** carrier identity */
   TAPI_CID_ST_CARID       = 0x31,
   /** selection of terminal function */
   TAPI_CID_ST_TERMSEL     = 0x40,
   /** display information, used as INFO for DTMF */
   TAPI_CID_ST_DISP        = 0x50,
   /** Service Information */
   TAPI_CID_ST_SINFO       = 0x55,
   /** extension for operator use */
   TAPI_CID_ST_XOPUSE      = 0xE0
} TAPI_CID_SERVICE_TYPE_t;

/** List of VMWI settings */
typedef enum
{
   /** Message waiting disabled */
   TAPI_CID_VMWI_DIS = 0x00,
   /** Message waiting enabled */
   TAPI_CID_VMWI_EN  = 0xFF
} TAPI_CID_VMWI_t;

/** List of ABSCLI/ABSNAME settings */
typedef enum
{
   /** Unavailable */
   TAPI_CID_ABSREASON_UNAV = 0x4F,
   /** Private */
   TAPI_CID_ABSREASON_PRIV = 0x50
} TAPI_CID_ABSREASON_t;


/* Defines for CID Receiver */

/** CID Receiver Status */
typedef enum
{
   /** CID Receiver is not active */
   TAPI_CIDRX_STAT_INACTIVE = 0,
   /** CID Receiver is active */
   TAPI_CIDRX_STAT_ACTIVE = 1,
   /** CID Receiver is just receiving data */
   TAPI_CIDRX_STAT_ONGOING = 2,
   /** CID Receiver is completed */
   TAPI_CIDRX_STAT_DATA_RDY = 3
} TAPI_CIDRX_STAT;

/** CID Receiver Errors */
typedef enum
{
   /** No Error during CID Receiver operation */
   TAPI_CIDRX_ERR_NOERR = 0,
   /** Reading error during CID Receiver operation */
   TAPI_CIDRX_ERR_READ = 1
} TAPI_CIDRX_ERR;

/*@}*/

/*====================*/
/* Defines for Jitter */
/* Buffer Adaptation  */
/*====================*/

/** Jitter buffer adoption */
typedef enum
{
   /** Local Adaptation OFF */
   LOCAL_ADAPT_OFF = 0,
   /** Local adaptation ON */
   LOCAL_ADAPT_ON = 1,
   /** Local Adaptation ON
      with Sample Interpollation */
   LOCAL_ADAPT_SI_ON = 2
}TAPI_LOCAL_ADAPT;

/*========================*/
/* Defines for Fax Status */
/*========================*/

/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/
/** T38 Fax Datapump states */
typedef enum
{
   /** Fax Datapump not active */
   TAPI_FAX_T38_DP_OFF = 0x1,
   /** Fax Datapump active */
   TAPI_FAX_T38_DP_ON  = 0x2,
   /** Fax transmission is active */
   TAPI_FAX_T38_TX_ON  = 0x4,
   /** Fax transmission is not active */
   TAPI_FAX_T38_TX_OFF = 0x8
} TAPI_FAX_T38_STATUS;

/*========================*/
/* Defines for Fax Errors */
/*========================*/

/** T38 Fax errors */
typedef enum
{
   /** No Error */
   TAPI_FAX_T38_NO_ERR    = 0,
   /** Error occured : Deactivate Datapump */
   TAPI_FAX_T38_ERR       = 1,
   /** MIPS Overload */
   TAPI_FAX_T38_MIPS_OVLD = 2,
   /** Érror while reading data */
   TAPI_FAX_T38_READ_ERR  = 3,
   /** Error while writing data */
   TAPI_FAX_T38_WRITE_ERR = 4,
   /** Error while setting up modulator or demodulator */
   TAPI_FAX_T38_DATA_ERR  = 5
} TAPI_FAX_T38_ERROR;

/*===========================*/
/* Defines for Fax Standards */
/*===========================*/
/** T38 Fax standards */
typedef enum
{
   /** V.21 */
   TAPI_FAX_T38_V21_STD       = 0x1,
   /** V.27/2400 */
   TAPI_FAX_T38_V27_2400_STD  = 0x2,
   /** V.27/4800 */
   TAPI_FAX_T38_V27_4800_STD  = 0x3,
   /** V.29/7200 */
   TAPI_FAX_T38_V29_7200_STD  = 0x4,
   /** V.29/9600 */
   TAPI_FAX_T38_V29_9600_STD  = 0x5,
   /** V.17/7200 */
   TAPI_FAX_T38_V17_7200_STD  = 0x6,
   /** V.17/9600 */
   TAPI_FAX_T38_V17_9600_STD  = 0x7,
   /** V.17/12000 */
   TAPI_FAX_T38_V17_12000_STD = 0x8 ,
   /** V.17/14400 */
   TAPI_FAX_T38_V17_14400_STD = 0x9
} TAPI_FAX_T38_STD;
/*@}*/

/* ============================= */
/*         Tapi Init             */
/* ============================= */

/** TAPI initialization structure used for IFXPHONE_INIT
   \ingroup TAPI_INTERFACE_INIT */
typedef struct
{
   /** Application mode selection, VoIp, PCM etc

   - 0: TAPI_DEFAULT, default initialization see device driver documentation.
        This value is used for compatibility reasons
   - 1: TAPI_VOICE_CODER, Phone connected to a packet coder (data channel)
        with DSP features for signal detection
   - 2: TAPI_PCM_DSP, Phone to PCM using DSP features for signal detection
   - 3: TAPI_PCM_PHONE, Phone to PCM connection without DSP features
   - 255: TAPI_NONE, no configuration is done. This is used only for testing */
   unsigned char nMode;
   /** Country selection. For future purposes, not yet used */
   unsigned char nCountry;
   /** Low level Device initialization pointer. Please refer to device driver
   documentation */
   void *pProc;
} TAPI_INIT;

/******************************************************************************
*
* The capabilities ioctls can inform you of the capabilities of each phone
* device installed in your system.  The PHONECTL_CAPABILITIES ioctl
* returns an integer value indicating the number of capabilities the
* device has.  The PHONECTL_CAPABILITIES_LIST will fill an array of
* capability structs with all of it's capabilities.  The
* PHONECTL_CAPABILITIES_CHECK takes a single capability struct and returns
* a TRUE if the device has that capability, otherwise it returns FALSE.
*
******************************************************************************/

/** \addtogroup TAPI_INTERFACE_MISC  */
/*@{*/
/** Enumeration used for phone capabilities types */
typedef enum {
   vendor = 0,
   device = 1,
   port = 2,
   codec = 3,
   dsp = 4,
   pcm = 5,
   codecs = 6,
   phones = 7,
   sigdetect = 8,
   t38 = 9
} TAPI_PHONE_CAP;

/** Capability structure */
typedef struct {
   /** Description of the capability */
   char desc[80];
   /** Defines the capability type

   - 0:  vendor, string representation of the vendor
   - 1:  device, string representation of the underlying device
   - 2:  port, gives information about the available ports, see
         \ref TAPI_PHONE_PORTS for port types
   - 3:  codec, gives information about the available codecs as
         listed in \ref TAPI_PHONE_CODEC
   - 4:  dsp, 1 if DSP is available, otherwise 0
   - 5:  pcm, cap defines how many pcm channels are available
   - 6:  codecs, cap defines how many codec channels are available
   - 7:  phones, cap defines how many phone channels are available
   - 8:  T.38 stack support.
   - 9: sigdetect, signal detectors out of \ref TAPI_PHONE_CAP_SIGDETECT */
   TAPI_PHONE_CAP captype;
   /** Defines if, what or how many is available. The definition of cap
       depends on the type. See captype */
   int cap;
   /** The number of this capability */
   int handle;
} TAPI_PHONE_CAPABILITY;

/** Lists the ports for the capability list */
typedef enum {
   pots = 0,
   pstn = 1,
   handset = 2,
   speaker = 3
} TAPI_PHONE_PORTS;

/** Lists the signal detectors for the capability list */
typedef enum {
   /** Signal detection for CNG is available */
   cng = 0,
   /** Signal detection for CED is available */
   ced = 1,
   /** Signal detection for DIS is available */
   dis = 2,
   /** Signal detection for line power is available */
   power = 3,
   /** Signal detection for CPT is available */
   cpt = 4,
   /** Signal detection for V8.bis is available */
   v8bis = 5
} TAPI_PHONE_CAP_SIGDETECT;
/*@}*/

/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Defines the tone generator usage */
typedef enum
{
   /** Use tone generator 1 */
   TAPI_TONE_TG1 = 1,
   /** Use tone generator 2 */
   TAPI_TONE_TG2 = 2,
   /** Use all tone generators  */
   TAPI_TONE_TGALL = 0xff
}TAPI_TONE_TG;

/** Structure used for tone level configuration */
typedef struct
{
   /** Defines the tone generator number whose level has to be modified.
   - 0x1: TAPI_TONE_TG1, generator one is used
   - 0x2: TAPI_TONE_TG2, generator two is used
   - 0xFF: TAPI_TONE_TGALL, all tone generator are affected. */
   unsigned char      nGenerator;
   /** Defines the tone level: value 0x100 describes the default record
   level of -38.79 dB. Maximum value is 0x7FFF which corresponds to a
   record level of approximately 3 dB.
   The tone level in dB can be calculated via formula:
   level_in_dB = +3.17 + 20 log10 (level / 32767) */
   unsigned long      nLevel;
}TAPI_TONE_LEVEL;
/*@}*/

/** \addtogroup TAPI_INTERFACE_OP  */
/*@{*/

/** TAPI phone volume control */
   /** Switch the phone off */
#define TAPI_PHONE_VOLCONF_OFF      (0xFF)
   /** Switch the volume to low, -24 dB */
#define TAPI_PHONE_VOLCONF_LOW      (-24)
   /** Switch the volume to medium, 0 dB */
#define TAPI_PHONE_VOLCONF_MEDIUM   (0)
   /** Switch the volume to high, 24 dB */
#define TAPI_PHONE_VOLCONF_HIGH     (24)

/** Phone speaker phone and microphone volume settings used for
    ioctl \ref IFXPHONE_VOLUME
    \ingroup TAPI_INTERFACE_OP */
typedef struct
{
   /** Volume setting for the receiving path, speakerphone
       The value is given in dB with the range (-24dB ... 24dB) */
   int nGainRx;
   /** Volume setting for the transmitting path, microphone
       The value is given in dB with the range (-24dB ... 24dB) */
   int nGainTx;
}TAPI_PHONE_VOLUME;


/** Specifies the Enable/Disable mode of the high level */
typedef enum
{
   TAPI_HIGH_LEVEL_DISABLE = 0x0,
   TAPI_HIGH_LEVEL_ENABLE = 0x1
}TAPI_HIGH_LEVEL;
/*@}*/


/** Structure for ring cadence used in \ref IFXPHONE_RING_CADENCE_HIGH_RES  */
typedef struct
{
   /** Pointer to data bytes which contain the encoded cadence sequence.
   One bit represents ring cadence voltage for 50 ms. A maximum of
   40 bytes (320 bits) are allowed.  */
   char      data[TAPI_MAX_CADENCE_BYTES];
   /** Number of data bits of cadence sequence. A maximum number of
   320 data bits is possible which corresponds to a maximum cadence
   duration of 16 seconds.  */
   int       nr;
   /** Pointer to data bytes which contain the encoded cadence sequence
   for a first non periodic initial ringing. One bit represents ring cadence
   voltage for 50ms. A maximum of 40 bytes (320 bits) are allowed. */
   char      initial [TAPI_MAX_CADENCE_BYTES];
   /** Number of data bits of cadence sequence. A maximum number of 320 data
   bits is possible which corresponds to a maximum cadence duration
   of 16 seconds. */
   int       initialNr;
}TAPI_RING_CADENCE;


/** Ring Configuration Mode */
typedef enum
{
   /** Internal balanced */
   INTERNAL_BALANCED = 0,
   /** Internal unbalanced ROT */
   INTERNAL_UNBALANCED_ROT = 1,
   /** Internal unbalanced ROR */
   INTERNAL_UNBALANCED_ROR = 2,
   /** External SLIC current sense */
   EXTERNAL_IT_CS = 3,
   /** External IO current sense */
   EXTERNAL_IO_CS = 4
}RING_CONFIG_MODE;

/** Ring Configuration SubMode */
typedef enum
{
   /** DC Ring Trip standard */
   DC_RNG_TRIP_STANDARD = 0,
   /** DC Ring Trip fast */
   DC_RNG_TRIP_FAST = 1,
   /** AC Ring Trip standard */
   AC_RNG_TRIP_STANDARD = 2,
   /** AC Ring Trip fast */
   AC_RNG_TRIP_FAST = 3
}RING_CONFIG_SUBMODE;


/** Ringing configuration structure used for
    ioctl \ref IFXPHONE_RING_CONFIG
    \ingroup TAPI_INTERFACE_RINGING */
typedef struct
{
   /** Configures the ringing mode

   - 0: INTERNAL_BALANCED, internal balanced
   - 1: INTERNAL_UNBALANCED_ROT, internal unbalanced ROT
   - 2: INTERNAL_UNBALANCED_ROR, internal unbalanced ROR
   - 3: EXTERNAL_IT_CS, external SLIC current sense
   - 4: EXTERNAL_IO_CS, external IO current sense */
   unsigned char       nMode;
   /** Configures the ringing submode

   - 0: DC_RNG_TRIP_STANDARD, DC Ring Trip standard
   - 1: DC_RNG_TRIP_FAST, DC Ring Trip fast
   - 2: AC_RNG_TRIP_STANDARD, AC Ring Trip standard
   - 3: AC_RNG_TRIP_FAST, AC Ring Trip fast */
   unsigned char       nSubmode;
}TAPI_RING_CONFIG;

/** PCM Resolutions */
typedef enum
{
   /** A-Law 8 bit */
   A_LAW_8_BIT = 0,
   /** u-Law 8 bit */
   U_LAW_8_BIT = 1,
   /** Linear 16 bit */
   LINEAR_16_BIT = 2
}PCM_CONFIG_RES;

/** Structure for PCM configuration  */
typedef struct
{
   /** PCM timeslot for the receive direction */
   unsigned long             nTimeslotRX;
   /** PCM timeslot for the transmit direction */
   unsigned long             nTimeslotTX;
   /** Defines the PCM highway number which is connected to the channel */
   unsigned long             nHighway;
   /** Defines the PCM interface coding

   - 0: A_LAW_8_BIT, 8 bit A law
   - 1: U_LAW_8_BIT, 8 bit u Law
   - 2: LINEAR_16_BIT, 16 bit linear */
   unsigned long             nResolution;
   /** Defines the PCM sample rate in kHz. */
   unsigned long             nRate;
} TAPI_PCM_CONFIG;


/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Structure for metering config */
typedef struct
{
   /** Metering mode

   - 0: TAPI_METER_MODE_TTX, TTX mode
   - 1: TAPI_METER_MODE_REVPOL, reverse polarity  */
   unsigned char             mode;
   /** Metering frequency

   - 0: TAPI_METER_FREQ_DEFAULT, device default
   - 1: TAPI_METER_FREQ_12_KHZ, 12 kHz
   - 2: TAPI_METER_FREQ_16_KHZ, 16 kHz  */
   unsigned char             freq;
   /** Length of metering burst in ms. burst_len must be greater than zero. */
   unsigned long            burst_len;
   /** Distance between the metering burst in sec */
   unsigned long            burst_dist;
   /** Defines the number of bursts */
   unsigned long            burst_cnt;
} TAPI_METER_CONFIG;

typedef enum
{
   TAPI_LEC_NLPDEFAULT = 0,
   TAPI_LEC_NLP_ON = 1,
   TAPI_LEC_NLP_OFF = 2
}TAPI_LEC_NLP;

/** Line echo canceller (LEC) configuration */
typedef struct
{
   /** Gain for input or LEC off

    - 0: TAPI_LEC_GAIN_OFF, LEC Off
    - 1: TAPI_LEC_GAIN_LOW, Low Gain
    - 2: TAPI_LEC_GAIN_MEDIUM,  Medium Gain (only supported for VINETIC)
    - 3: TAPI_LEC_GAIN_HIGH, High Gain    */
   char nGainIn;
   /** Gain for ouput or LEC off

   - 0: TAPI_LEC_GAIN_OFF, LEC Off
   - 1: TAPI_LEC_GAIN_LOW, Low Gain
   - 2: TAPI_LEC_GAIN_MEDIUM, Medium Gain (only supported for VINETIC)
   - 3: TAPI_LEC_GAIN_HIGH, High Gain    */
   char nGainOut;
   /** LEC tail length in milli seconds. nLen is currently not supported
       and can be set to zero */
   char nLen;
   /** Switch the NLP on or off

   - 0: TAPI_LEC_NLPDEFAULT, NLP is default
   - 1: TAPI_LEC_NLP_ON, NLP is on
   - 2: TAPI_LEC_NLP_OFF, NLP is off   */
   char bNlp;
} TAPI_LECCONF;
/*@}*/

/**
  * \addtogroup TAPI_INTERFACE_CID
  */
/*@{*/

/** Structure used for CID configuration */
typedef struct
{
   /** Auto deactivation, no longer used  */
   unsigned char nAutoDeac;
   /** High Level CID Generation Mode

   - 0: TAPI_CID_LOWLEVEL, Low Level CID generation mode (simple FSK modulator)
   - 1: TAPI_CID_HIGHLEVEL, High level CID generation mode
                           (with automatic framing)  */
   unsigned char nHlev;
   /** Used standard

   - 0: TAPI_CID_BELLCORE, The Bellcore specification (Bel202) is used
                           for the CID Sender, default
   - 1: TAPI_CID_V23, The ITU-T V.23 specification is used for the CID Sender
   - 2: TAPI_CID_NTT, The NTT specification is used for the CID Sender */
   unsigned char nSpec;
   /** CID generation mode

   - 0: TAPI_CID_FSK_MODE, CID is generated with FSK, default
   - 1: TAPI_CID_DTMF_MODE, CID is generated with DTMF, use
        \ref IFXPHONE_DTMFCID_CONF for configuration. */
   unsigned char nMode;
   /** Defines the caller id appearance mode

   - 0: CID_BEFORE_RINGBURST, before ring burst
   - 1: CID_BETWEEN_RINGBURST, between ring bursts, default
   - 2: CID_AFTER_DELAY, between first non periodic ring and periodic ring bursts.
          The delay field in the CID configuration interface will be used for
          delay the CID presentation after the first ring burst   */
   long nAppearance;
   /** Delay for apperance.
    Defines the delay of the caller id signal in ms. For appearance mode 0
    (before ring burst) this is the time after the ringing is started. For
    appearance mode 1 (between ring bursts) this is the time after the first
    ringing burst is finished. Default is 500 ms. */
   unsigned int nDelay;
   /** Number of periodical bursts before sending CID. This applies only
       in case CID is to send between periodical ring bursts */
   unsigned int nBurstCount;
} TAPI_CID_CONFIG;

/** DTMF CID configuration */
typedef struct
{
   /** Time for the DTMF tone in ms. Currently not used. */
   unsigned long toneTime;
   /** Time of the SAS tone in ms. Currently not used. */
   unsigned long sasTime;
   /** Tone to indicate ack. Currently not used. */
   char   ackTone;
   /** DTMF start tone in ASCII presentation. Default is ’A’. */
   char   startTone;
   /** DTMF stop tone in ASCII presentation. Default is ’C’. */
   char   stopTone;
   /** DTMF info tone in ASCII presentation. Default is ’B’. */
   char   infoStartTone;
   /** DTMF start redirection tone in ASCII presentation. Default is ’D’. */
   char   redirStartTone;
   /** Time for DTMF digit duration in ms. Default is 50 ms. */
   unsigned long digitTime;
   /** Time between DTMF digit in ms. Default is 50 ms. */
   unsigned long interDigitTime;
} TAPI_DTMFCID;

/** CID2 Configuration structure for ITU standard */
typedef struct
{
   /** Defines the CAS duration in ms. Default is 80 ms */
   unsigned long casDuration;
   /** Tone used as acknowledge tone as ASCII value.
      Valid values are ’0’ .. ’9’, ’A’, ’B’, ’C’, ’D’. Default is ’D’ */
   char ackTone;
   /** Time to wait for acknowledge after CAS. Default is 100 ms. */
   unsigned long cas2ackTime;
   /** Time before sending FSK data after ACK. Default is 45 ms. */
   unsigned long ack2fskOutTime;
   /** Code of CAS (simple) Tone to be sent when starting CID2 processing,
       look at \ref TAPI_SIMPLE_TONE for details */
   unsigned int casToneIndex;
} TAPI_CID2_DEFAULT_CONFIG;

/** CID2 Configuration structure for JAPAN standard */
typedef struct
{
   /** Time to wait after alert tone before sending FSK.
       Default is 300 ms */
   unsigned long at2fskOutTime;
   /** Time to wait after FSK is sent before restoring the line.
       Default is 100 ms */
   unsigned long fskOut2restoreTime;
   /** Code of Alert Signal to be sent after line is muted, look at
       \ref TAPI_SIMPLE_TONE for details */
   unsigned int alertSignalIndex;
} TAPI_CID2_NTT_CONFIG;

/** CID2 standards configurations structures */
typedef union
{
   /** CID2 Configuration according to common standards (TIA, ETSI, BT), refer
       to \ref TAPI_CID2_DEFAULT_CONFIG */
   TAPI_CID2_DEFAULT_CONFIG defaultS;
   /** CID2 Configuration according to NTT standard, refer to
       \ref TAPI_CID2_NTT_CONFIG */
   TAPI_CID2_NTT_CONFIG nttS;
   /* add here for other standards different from the common one */
} TAPI_CID2_STD_CONFIG;

/** CID Type 2 configuration structure used for \ref IFXPHONE_CID2_CONF */
typedef struct
{
   /** Select Standard according to \ref TAPI_CID2_STD

   - 0: TAPI_CID2_TIA, TIA  Standard, default
   - 1: TAPI_CID2_ETSI, ETSI Standard, same principle as TIA, other timings
   - 2: TAPI_CID2_BT, BT Standard, same principle as TIA, other timings
   - 3: TAPI_CID2_NTT,  NTT Standard */
   TAPI_CID2_STD  stdSelect;
   /** Standard configuration according to \ref TAPI_CID2_CONFIG */
   TAPI_CID2_STD_CONFIG std;
} TAPI_CID2_CONFIG;

/* ============================= */
/* Structure for CID TX/RX       */
/* ============================= */

/** Structure for CID display according to ETSI EN 300 659-3.

   \remarks Either the service CIDCS_CLI or CIDCS_ABSCLI and the
   corresponding information must be set.
   To display a number for example the service is set to
   CIDCS_CDLI | CIDCS_CLI. The number is set up in number.
   The length must be also set in numlen
   For ETSI service 1AH (redirecting number) the information
   is passed to redirno. The length is set in numrdno. The service
   is set to CIDCS_REDIR | CIDCS_CLI.
   For service 4 (reason for absence of calling line number) the service
   is set to CIDCS_ABSCLI. The information is passed to the first byte of
   the parameter number. */
typedef struct
{
   /** Month to display for 0x01 date and time service */
   unsigned int month;
   /** Day to display for 0x01 date and time service */
   unsigned int day;
   /** Hour to display for 0x01 date and time service */
   unsigned int hour;
   /** Minutes to display for 0x01 date and time service */
   unsigned int mn;
   /** Length of the number */
   unsigned int numlen;
   /** Number to display for 0x02 calling line identity service */
   char number[20];
   /** Length of the name */
   unsigned int namelen;
   /** Name for the 0x07 service calling party name */
   char name[50];
   /** Length of the redirecting number */
   unsigned int numrdno;
   /** Number to display for 1AH redirecting number service */
   char redirno[20];
   /** Length of the information */
   unsigned int infolen;
   /** Information to display */
   char info[20];
   /** Size of data buffer */
   unsigned int datalen;
   /** Transparent data in case of CID_TRANSPARENT */
   char *pData;
   /** Defines the service used for CID display according to ETSI EN 300 659-3.
      It must be a combination of the following bits:

      - 0x1: CIDCS_CLI,  calling line identity
      - 0x2: CIDCS_DATE, display date and time
      - 0x4: CIDCS_CDLI, called line identity
      - 0x8: CIDCS_ABSCLI, reason for absence of CLI located in number[0]
      - 0x10: CIDCS_CCLI, complementary calling line identity
      - 0x20: CIDCS_CT, call type
      - 0x40: CIDCS_FIRSTCLI, first called line identity
      - 0x80: CIDCS_MSGNR, number of messages
      - 0x100: CIDCS_FWCT, type of forwarded call
      - 0x200: CIDCS_USRT, type of calling user
      - 0x400: CIDCS_REDIR, number redirection, ETSI service 1AH
      - 0x800: CIDCS_NTID, network provider id
      - 0x1000: CIDCS_CARID, carrier identiy
      - 0x2000: CIDCS_TERMSEL, selection of terminal function
      - 0x4000: CIDCS_DISP, display information
      - 0x8000: CIDCS_XOPUSE, extension for operator use
      - 0x10000: CIDCS_NAME, calling line name
      - 0x20000: CIDCS_ABSNAME, reason for absence of name located
      in name[0]
      - 0x40000: CIDCS_INFO, information values for DTMF
      - 0x80000000: CID_TRANSPARENT, the CID data is passed transparently
      in pData, no interpretation is done       */
   unsigned long service;
} TAPI_PHONE_CID;

#define TAPI_CID_MAX_MSG_LEN  50

/** Structure for element types (\ref TAPI_CID_SERVICE_TYPE_t)
   representing a date (like TAPI_CID_ST_DATE or TAPI_CID_ST_CDATE)
*/
typedef struct
{
   /** Set to TAPI_CID_ST_DATE and TAPI_CID_ST_CDATE (display date and time) */
   TAPI_CID_SERVICE_TYPE_t elementType;
   /** Month data */
   unsigned int month;
   /** Day data */
   unsigned int day;
   /** Hour data */
   unsigned int hour;
   /** Minutes data */
   unsigned int mn;
} TAPI_CID_MSG_DATE_t;

/** Structure for element types (\ref TAPI_CID_SERVICE_TYPE_t)
   with dynamic length (like numbers or names) */
typedef struct
{
   /** Possible values are:
   - TAPI_CID_ST_MSGIDENT
   - TAPI_CID_ST_DURATION
   - TAPI_CID_ST_XOPUSE
   - TAPI_CID_ST_CHARGE
   - TAPI_CID_ST_ACHARGE
   - TAPI_CID_ST_CLI
   - TAPI_CID_ST_CDLI
   - TAPI_CID_ST_LMSGCLI
   - TAPI_CID_ST_CCLI
   - TAPI_CID_ST_FIRSTCLI
   - TAPI_CID_ST_REDIR
   - TAPI_CID_ST_NTID
   - TAPI_CID_ST_CARID
   - TAPI_CID_ST_TERMSEL
   - TAPI_CID_ST_NAME
   - TAPI_CID_ST_DISP   */
   TAPI_CID_SERVICE_TYPE_t elementType;
   /** Defines the length of the string element */
   unsigned int len;
   /** Element data */
   unsigned char element[TAPI_CID_MAX_MSG_LEN];
} TAPI_CID_MSG_STRING_t;

/** Structure for element types (\ref TAPI_CID_SERVICE_TYPE_t)
   with one value (length 1). */
typedef struct
{
   /** Possible values are:
   - TAPI_CID_ST_ABSCLI
   - TAPI_CID_ST_ABSNAME
   - TAPI_CID_ST_VISINDIC
   - TAPI_CID_ST_CT
   - TAPI_CID_ST_MSGNR
   - TAPI_CID_ST_FWCT
   - TAPI_CID_ST_USRT
   - TAPI_CID_ST_SINFO */
   TAPI_CID_SERVICE_TYPE_t elementType;
   /** Element data */
   unsigned char element;
} TAPI_CID_MSG_VALUE_t;

/** Union for structures representing different type of information to
    be displayed for CID. */
typedef union
{
   /** Date and time */
   TAPI_CID_MSG_DATE_t        date;
   /** Numbers, name or similar */
   TAPI_CID_MSG_STRING_t   string;
   /** CID Value. Must be used for
    - TAPI_CID_ST_ABSCLI
    - TAPI_CID_ST_ABSNAME
    - TAPI_CID_ST_VISINDIC
    - TAPI_CID_ST_CT
    - TAPI_CID_ST_MSGNR
    - TAPI_CID_ST_FWCT
    - TAPI_CID_ST_USRT
    - TAPI_CID_ST_SINFO */
   TAPI_CID_MSG_VALUE_t       value;
} TAPI_CID_MSG_ELEMENT_t;

/** Structure containing the CID message type and content as well as
information about transmission mode. This structure contains all
information required by \ref IFXPHONE_CID_INFO_TX to start CID generation. */
typedef struct
{
   /** Define the Transmission Mode. Default TAPI_CID_ONHOOK. */
   TAPI_CID_HOOK_MODE_t    txMode;
   /** Define the Message Type to be displayed. Default TAPI_CID_MT_CSUP. */
   TAPI_CID_MESSAGE_TYPE_t messageType;
   /** Number of elements of the message array. */
   unsigned int nMsgElements;
   /** Pointer to message array */
   TAPI_CID_MSG_ELEMENT_t* message;
} TAPI_CID_INFO_t;


/** Structure for caller id receiver status */
typedef struct
{
   /** Caller id receiver actual status using \ref TAPI_CIDRX_STAT

   - 0: TAPI_CIDRX_STAT_INACTIVE
   - 1: TAPI_CIDRX_STAT_ACTIVE
   - 2: TAPI_CIDRX_STAT_ONGOING
   - 3: TAPI_CIDRX_STAT_DATA_RDY
   */
   unsigned char nStatus;
   /** Caller id receiver actual error code using \ref TAPI_CIDRX_ERR

   - 0: TAPI_CIDRX_ERR_NOERR
   - 1: TAPI_CIDRX_ERR_READ
   */
   unsigned char nError;
} TAPI_CIDRX_STATUS;

/** structure for caller id receiver data */
typedef struct
{
   /** Caller id receiver data */
   unsigned char data [TAPI_MAX_CID_PARAMETER];
   /** Caller id receiver datasize in bytes */
   unsigned int nSize;
} TAPI_CIDRX_DATA;
/* @}*/

/** Out of band or in band definition */
typedef enum
{
   /** Tranmit in band and out of band */
   TAPI_OOB_INOUT,
   /** Tranmit only out of band */
   TAPI_OOB_OUT,
   /** Tranmit only in band */
   TAPI_OOB_IN
}TAPI_OOB;

/* ============================= */
/* Structure for exception byte  */
/* ============================= */

/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
/** Exception bits */
typedef struct
{
   unsigned int dtmf_ready:1;
   unsigned int hookstate:1;
   unsigned int flash_hook:1;
   unsigned int pstn_ring:1;
   unsigned int callerid_ready:1;
   unsigned int cidrx_supdate:1;
   unsigned int ground_key:1;
   unsigned int pulse_digit_ready:1;
   unsigned int fax_cng:1;
   unsigned int fax_ced:1;
   unsigned int fax_dis:1;
   unsigned int fax_supdate:1;
   unsigned int fault:1;
   unsigned int ground_key_high:1;
   unsigned int ground_key_polarity:1;
   unsigned int otemp:1;
   /* to get with TAPI_PHONE_STATUS in signal */
   unsigned int signal:1;
   unsigned int device:1;
   unsigned int gr909result:1;
   unsigned int fax_cng_net:1;
   unsigned int fax_ced_net:1;
   /** RFC 2833 event playout detection */
   unsigned int eventDetect:1;
   /** initial ringing finished */
   unsigned int ring_finished:1;
   /** runtime errors during processing of a tapi command */
   unsigned int runtime_error:1;
   /** Event occurs when the line mode may be switched to
       TAPI_LINEFEED_NORMAL_LOW to save power */
   unsigned int feedLowBatt:1;
   unsigned int reserved2:7;
} EXCEPTION_BITS;
#elif (__BYTE_ORDER == __BIG_ENDIAN)
/** Exception bits */
typedef struct
{
   unsigned int reserved2:7;
   /** Event occurs when the line mode may be switched to
       TAPI_LINEFEED_NORMAL_LOW to save power */
   unsigned int feedLowBatt:1;
   /** runtime errors during processing of a tapi command */
   unsigned int runtime_error:1;
   /** initial ringing finished */
   unsigned int ring_finished:1;
   /** RFC 2833 event playout detection */
   unsigned int eventDetect:1;
   /** Reserved */
   unsigned int fax_ced_net:1;
   /** Reserved */
   unsigned int fax_cng_net:1;
   /** GR909 result is available and can be queried with the appropriate
       interface */
   unsigned int gr909result:1;
   /** Low level device exception. The information can be queried from
       the underlying driver */
   unsigned int device:1;
   /** Signal detected. The information is retrieved with
       \ref IFXPHONE_PHONE_STATUS */
   unsigned int signal:1;
   /** Overtemperature detected */
   unsigned int otemp:1;
   /** Ground key polarity detected */
   unsigned int ground_key_polarity:1;
   /** Ground key high detected */
   unsigned int ground_key_high:1;
   /** Fault condition detected, query information via
       \ref IFXPHONE_PHONE_STATUS */
   unsigned int fault:1;
   /** FAX status update, query information via \ref IFXPHONE_PHONE_STATUS */
   unsigned int fax_supdate:1;
   /** FAX DIS received (no longer supported) */
   unsigned int fax_dis:1;
   /** FAX CED received (no longer supported) */
   unsigned int fax_ced:1;
   /** FAX CNG received (no longer supported) */
   unsigned int fax_cng:1;
   /** Pulse dial digit is ready */
   unsigned int pulse_digit_ready:1;
   /** Ground key detected */
   unsigned int ground_key:1;
   /** Caller id receiver event, query information
   with IFXPHONE_CIDRX_STATUS */
   unsigned int cidrx_supdate:1;
   /** Caller id data is ready (no longer supported) */
   unsigned int callerid_ready:1;
   /** Reserved */
   unsigned int pstn_ring:1;
   /** Flash hook detected */
   unsigned int flash_hook:1;
   /** Hook state changed */
   unsigned int hookstate:1;
   /** DTMF digit is ready */
   unsigned int dtmf_ready:1;
} EXCEPTION_BITS;
#else
#error Please define endian mode
#endif

/** Contains TAPI exception information */
typedef union
{
   /** Specifies the exception */
   EXCEPTION_BITS Bits;
   /** Contains the complete status */
   unsigned long         Status;
}TAPI_EXCEPTION;
/*@}*/

/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Enumeration for \ref TAPI_RTP_CONF event setting */
typedef enum
{
   /** Device default setting */
   TAPI_RTPEV_DEFAULT = 0,
   /** No events, only voice is transmitted */
   TAPI_RTPEV_NO = 1,
   /** Only events, no voice is transmitted. Not yet implemented */
   TAPI_RTPEV_ONLY = 2,
   /** Events and voice is transmitted */
   TAPI_RTPEV_ALL = 3
}TAPI_RTPEV;

/** Structure for RTP Configuration     */
typedef struct
{
   /** Start value for the sequence nr  */
   unsigned short nSeqNr;
   /** Synchronization source value for the voice and SID packets */
   unsigned long nSsrc;
   /** Start value for the timestamp (resolution of 125 µs) */
   unsigned long nTimestamp;
   /** Defines how events are transmitted

   - 0: TAPI_RTPEV_DEFAULT, Default
   - 1: TAPI_RTPEV_NO, No events, only voice
   - 2: TAPI_RTPEV_ONLY, No voice, only events
   - 3: TAPI_RTPEV_ALL, Events and voice */
   unsigned char  nEvents;
   /** Event payload type */
   unsigned char  nEventPT;
} TAPI_RTP_CONF;

/** Structure for RTP payload configuration */
typedef struct
{
   /** Table with all payload types, the coder type \ref TAPI_PHONE_CODEC
   is used as index. */
   unsigned char nPT [MAX_CODECS];
} TAPI_RTP_PT_CONF;

/** Structure for RTCP Statistics. It refers to the RFC3550/3551 */
typedef struct
{
   /** Sender generating this report */
   unsigned long ssrc;
   /** NTP timestamp higher 32 bits */
   unsigned long ntp_sec;
   /** NTP timestamp lower 32 bits */
   unsigned long ntp_frac;
   /** RTP time stamp */
   unsigned long   rtp_ts;
   /** Sent packet count */
   unsigned long   psent;
   /** Sent octets count */
   unsigned long   osent;
   /** Data source */
   unsigned long   rssrc;
   /** Receivers fraction loss */
   unsigned int   fraction:8;
   /** Receivers packet lost */
   int      lost:24;
   /** Extended last seq nr received*/
   unsigned long   last_seq;
   /** Receivers interarrival jitter */
   unsigned long   jitter;
   /** Last sender report */
   unsigned long   lsr;
   /** Delay since last sender report */
   unsigned long   dlsr;
} TAPI_RTCP_DATA;

typedef enum
{
   JB_CONF_PCK_RES1 = 0,
   JB_CONF_PCK_RES2 = 1,
   JB_CONF_PCK_VOICE = 2,
   JB_CONF_PCK_DATA = 3
}JB_CONF_PCK;

/** Structure for jitter buffer configuration used by \ref IFXPHONE_JB_CONF
   \remarks This structure may be changed in the future */
typedef struct
{
   /** jitter buffer type

      - 1: fixed
      - 2: adaptive (default) */
   unsigned char    nJbType;
   /** Packet length adoption

   - 0: JB_CONF_RES1, The jitter buffer size depends on the estimated
   network jitter. No longer supported
   - 1: JB_CONF_RES2, The jitter buffer size depends on the Packet Length.
   In this case the jitter buffer size is packet length multiplied with
   nScaling. No longer supported
   - 2: JB_CONF_PCK_VOICE, Packet adoption is optimized for voice.
   Reduced adjustment
   speed and packet repetition is off.
   - 3: JB_CONF_PCK_DATA, Packet adoption is optimized for data
   (modem or fax inband).
   Reduced adjustment speed and packet repetition is on.   */
   char     nPckAdpt;
   /** Local adoption. Only of relevance if nJbType = 2

   - 0: LOCAL_ADAPT_OFF, Local adaptation off.
   Speech gaps which are detected by the far end side are used for the jitter
   buffer adaptations
   - 1: LOCAL_ADAPT_ON, Local adaptation on.
   Jitter buffer adaptation via local and far end speech detection. This means
   that the jitter buffer adaptation doesn't need the far end silence
   compression feature but will use it if the far end side has detected a
   silence period. Thus a jitter buffer adaptation can occur when the far end
   side has detected silence or when a speech gap was detected in the
   downstream direction.
   - 2: LOCAL_ADAPT_SI_ON, Local adaptation on with sample interpolation
   A jitter buffer adaptation can be done via sample interpolation. The
   advantage is that not a whole frame has to be interpolated or discarded.
   Thus no major distortion should be heard. */
   char     nLocalAdpt;
   /** This factor influences the play out delay.
      If nPckAdpt is 1, nScaling multiplied by the packet length determines the
      play out delay. If nPckAdpt is 0, the play out delay is calculated by
      the multiplication of the estimated network jitter and nScaling.
      nScaling can be chosen between 0 and 16. Default: 8 */
   char     nScaling;
   /** Initial size of the jitter buffer in timestamps of 125 µs in case of
       an adaptive jitter buffer  */
   unsigned short   nInitialSize;
   /** Maximum size of the jitter buffer in timestamps of 125 µs in case of
      an adaptive jitter buffer */
   unsigned short   nMaxSize;
   /** Minimum size of the jitter buffer in timestamps of 125 µs in case of
      an adaptive jitter buffer */
   unsigned short   nMinSize;
}TAPI_JB_CONF;

/** Structure for Jitter Buffer statistics used by
    ioctl \ref IFXPHONE_JB_STATISTICS */
typedef struct
{
   /** jitter buffer type

      - 1: fixed
      - 2: adaptive */
   unsigned char    nType;
   /** Incoming time high word total time in timestamp units for all packets
      since the start of theconnection which could be played out correctly.
      Not supported anymore  */
   unsigned long   nInTime;
   /**  Comfort noise generation. Not supported anymore */
   unsigned long   nCNG;
   /**  Bad frame interpolation. Not supported anymore  */
   unsigned long   nBFI;
   /** Current jitter buffer size */
   unsigned short nBufSize;
   /** Maximum estimated jitter buffer size */
   unsigned short nMaxBufSize;
   /** Minimum estimated jitter buffer size */
   unsigned short nMinBufSize;
   /** Maximum packet delay. Not supported anymore  */
   unsigned short   nMaxDelay;
   /**  Minimum packet delay. Not supported anymore */
   unsigned short   nMinDelay;
   /** Network jitter value. Not supported anymore */
   unsigned short   nNwJitter;
   /**  play out delay */
   unsigned short   nPODelay;
   /**  Maximum play out delay */
   unsigned short   nMaxPODelay;
   /**  Minimum play out delay */
   unsigned short   nMinPODelay;
   /**  Received packet number  */
   unsigned long   nPackets;
   /** Lost packets number. Not supported anymore */
   unsigned short   nLost;
   /**  Invalid packet number */
   unsigned short   nInvalid;
   /** Duplication packet number. Not supported anymore */
   unsigned short   nDuplicate;
   /**  Late packets number */
   unsigned short   nLate;
   /**  Early packets number */
   unsigned short   nEarly;
   /**  Resynchronizations number */
   unsigned short   nResync;
   /** Total number of injected samples since the beginning
      of the connection or since the last
      statistic reset due to jitter buffer underflows */
   unsigned long nIsUnderflow;
   /** Total number of injected samples since the beginning of the
      connection or since the last statistic reset in case of normal jitter
      buffer operation, which means when there isn’t a jitter buffer
      underflow. */
   unsigned long nIsNoUnderflow;
   /** Total number of injected samples since the beginning of the
      connection or since the last statistic reset in case of jitter buffer
      increments  */
   unsigned long nIsIncrement;
   /** Total number of skipped lost samples since the beginning of the
      connection or since the last statistic reset in case of jitter
      buffer decrements */
   unsigned long nSkDecrement;
   /** Total number of dropped samples since the beginning of the
      connection or since the last statistic reset in case of jitter buffer
      decrements. */
   unsigned long nDsDecrement;
   /** Total number of dropped samples since the beginning of the
      connection or since the last statistic reset in case of jitter buffer
      overflows. */
   unsigned long nDsOverflow;
   /** Total number of comfort noise samples since the beginning of the
       connection or since the last statistic reset. */
   unsigned long nSid;
} TAPI_JB_DATA;

/** structure used for ioctl IFXPHONE_AAL_CONF */
typedef struct
{
   /** Connection identifier */
   unsigned short nCid;
   /** Start value for the timestamp (resolution of 125 µs) */
   unsigned short nTimestamp;
   /** Connection Identifier for CPS events */
   unsigned short nCpsCid;
} TAPI_AAL_CONF;

/** Used for TAPI_AAL_PROFILE in case one coder range.
   The range information is specified as "UUI Codepoint Range" in the
   specification The ATM Forum, AF-VMOA-0145.000 or ITU-T I.366.2 */
typedef enum
{
   /** One range from 0 to 15 */
   RANGE_0_15 = 0,
   /** Range from 0 to 7 for a two range profile entry */
   RANGE_0_7 = 1,
   /** Range from 8 to 15 for a two range profile entry */
   RANGE_8_15 = 2,
   /** Range from 0 to 3 for a four range profile entry */
   RANGE_0_3 = 3,
   /** Range from 4 to 7 for a four range profile entry */
   RANGE_4_7 = 4,
   /** Range from 8 to 11 for a four range profile entry */
   RANGE_8_11 = 5,
   /** Range from 12 to 15 for a four range profile entry */
   RANGE_12_15 = 6
}TAPI_AAL_PROFILE_RANGE;

/** AAL profile setup structure used for \ref IFXPHONE_AAL_PROFILE */
typedef struct
{
   /** Amount of rows to program */
   char rows;
   /** Length of packet in bytes - 1 */
   char len[10];
   /** UUI codepoint range indicator, see \ref TAPI_AAL_PROFILE_RANGE

      - 0: RANGE_0_15, UUI codepoint range 0 -15
      - 1: RANGE_0_7, UUI codepoint range 0 -7
      - 2: RANGE_8_15, UUI codepoint range 8 -15
      - 3: RANGE_0_3, UUI codepoint range 0 - 3
      - 4: RANGE_4_7, UUI codepoint range 4 - 7
      - 5: RANGE_8_11, UUI codepoint range 8 - 11
      - 6: RANGE_12_15, UUI codepoint range 12 - 15 */
   char nUUI[10];
   /** Codec as listed for \ref TAPI_PHONE_CODEC  */
   char codec[10];
} TAPI_AAL_PROFILE;
/*@}*/

/** \addtogroup TAPI_INTERFACE_CON  */
/*@{*/
/** Data channel destination types (conferencing) */
typedef enum
{
   TAPI_DATA_MAPTYPE_DEFAULT = 0,
   TAPI_DATA_MAPTYPE_CODER = 1,
   TAPI_DATA_MAPTYPE_PCM = 2,
   TAPI_DATA_MAPTYPE_PHONE = 3
} TAPI_DATA_MAPTYPE;

/** Start/Stop information for data channel mapping */
typedef enum
{
   /** Do not modify the status of the recorder */
   TAPI_DATA_MAPPING_UNCHANGED = 0,
   /** Recording is started */
   TAPI_DATA_MAPPING_START = 1,
   /** Recording is stopped */
   TAPI_DATA_MAPPING_STOP = 2
} TAPI_DATA_MAPPING_START_STOP;

/** Phone channel mapping structure used for \ref IFXPHONE_DATA_ADD and
\ref IFXPHONE_DATA_REMOVE */
typedef struct
{
   /** Phone channel number to which this channel should be mapped.
   Phone channels numbers start from 0. */
   unsigned char                             nDstCh;
   /** Type of the destination channel.

   - 0: TAPI_MAPTYPE_DEFAULT, Default selected (phone channel)
   - 1: TAPI_MAPTYPE_CODER, not supported
   - 2: TAPI_MAPTYPE_PCM, type is PCM
   - 3: TAPI_MAPTYPE_PHONE, type is phone channel  */
   TAPI_DATA_MAPTYPE           nChType;
   /** Enables or disables the recording service or leaves as it is

   - 0: TAPI_DATA_MAPPING_UNCHANGED, not modify the status of the recorder
   - 1: TAPI_DATA_MAPPING_START, Recording is started,
        same as \ref PHONE_REC_START
   - 2: TAPI_DATA_MAPPING_STOP, Recording is stopped,
        same as \ref PHONE_REC_STOP */
   TAPI_DATA_MAPPING_START_STOP              nRecStart;
   /** Enables or disables the play service or leaves as it is

   - 0: TAPI_DATA_MAPPING_UNCHANGED, Do not modify the status of the recorder
   - 1: TAPI_DATA_MAPPING_START, Playing is started,
        same as \ref PHONE_PLAY_START
   - 2: TAPI_DATA_MAPPING_STOP, Playing is stopped,
        same as \ref PHONE_PLAY_STOP */
   TAPI_DATA_MAPPING_START_STOP              nPlayStart;
} TAPI_DATA_MAPPING;

/** Phone channel mapping structure used for \ref IFXPHONE_PHONE_ADD and
\ref IFXPHONE_PHONE_REMOVE */
typedef struct
{
   /** Phone channel number to which this channel should be mapped.
   Phone channels numbers start from 0. */
   unsigned char                             nPhoneCh;
} TAPI_PHONE_MAPPING;

/** Phone channel mapping structure used for \ref IFXPHONE_PCM_ADD and
\ref IFXPHONE_PCM_REMOVE */
typedef struct
{
   /** Channel number to which this channel should be mapped.
   Channels numbers start from 0. */
   unsigned char                             nDstCh;
} TAPI_PCM_MAPPING;
/*@}*/

/* ============================= */
/* Structure for fax status      */
/* ============================= */

/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** Structure to setup the modulator for T.38 fax and used for
    \ref IFXPHONE_FAX_SETMOD*/
typedef struct
{
   /** Selects the standard used for Fax T.38

   - 0x00: Silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000
   - 0x09: V.17/14400
   - 0x0A: CNG
   - 0x0B: CED
   */
   unsigned char    nStandard;
   /** Signal duration in ms. Used for the tonals signals (CED, CNG)
       and silence only. 1 < nSigLen < 100 [ms]*/
   unsigned short   nSigLen;
   /** Sets the transmit gain for the downstream direction */
   unsigned short   nGainTx;
   /** Desired output signal power in -dBm */
   unsigned char    nDbm;
   /** TEP Generation flag, used by V.27, V.29 and V.17 only,
       ignored in all other cases.

   - 0: no TEP generation
   - 1: TEP generation */
   unsigned char    nTEP;
   /** Training sequence flag, used by V.17 only, ignored in all other cases.

   - 0: Short training sequence
   - 1: Long training sequence */
   unsigned char    nTraining;
   /** Level required before the modulation starts.*/
   unsigned short   nMobsm;
   /**Level required before the modulation requests more data.*/
   unsigned short   nMobrd;
} TAPI_FAX_MODDATA;

/** Structure to setup the demodulator for T.38 fax and used for
    \ref IFXPHONE_FAX_SETDEMOD*/
typedef struct
{
   /** Selects the standard used for Fax T.38 using \ref TAPI_FAX_T38_STD

   - 0x00: Silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000 */
   unsigned char    nStandard1;
   /** Selects the alternative standard used for Fax T.38 uisng
       \ref TAPI_FAX_T38_STD

   - 0x00: Silence
   - 0x01: V.21
   - 0x02: V.27/2400
   - 0x03: V.27/4800
   - 0x04: V.29/7200
   - 0x05: V.29/9600
   - 0x06: V.17/7200
   - 0x07: V.17/9600
   - 0x08: V.17/12000
   - 0x09: V.17/14400
   - 0xFF: Use only standard 1. */
   unsigned char    nStandard2;
   /** Signal duration in ms. Used for the tonals signals (CED, CNG)
       and silence only.
       1 < nSigLen < 100 [ms]*/
   unsigned short   nSigLen;
   /** Sets the receive gain for the upstream direction */
   unsigned short   nGainRx;
   /** Equalizer configuration flag

   - 0: Reset the equalizer
   - 1: Reuse the equalizer coefficients */
   unsigned char    nEqualizer;
   /** Training sequence flag, used by V.17 only, ignored in all other cases.

   - 0: Short training sequence
   - 1: Long training sequence */
   unsigned char    nTraining;
   /** Level required before the demodulator sends data.*/
   unsigned short   nDmbsd;
} TAPI_FAX_DEMODDATA;

/** Structure to read the T.38 fax status and used for
    \ref IFXPHONE_FAX_STATUS*/
typedef struct
{
   /** T.38 fax status, refer to \ref TAPI_FAX_T38_STATUS

   - 0x01: Data pump is not active
   - 0x02: Data pump is active
   - 0x04: Transmission is active
   - 0x08: Transmission is finished
   */
   unsigned char nStatus;
   /** T.38 fax error, refer to \ref TAPI_FAX_T38_ERROR

   - 0x00: No error occurred
   - 0x01: FAX error occurred, the FAX data pump should be deactivated
   - 0x02: MIPS overload
   - 0x03: Error while reading data
   - 0x04: Error while writing data
   - 0x05: Error while setting up the modulator or demodulator
   */
   unsigned char nError;
} TAPI_FAX_STATUS;
/*@}*/

/* ============================= */
/* Structure for operation       */
/* control data                  */
/* ============================= */

typedef struct
{
   /* interrupt routine save hookstatus here */
   unsigned char              bHookState;
   /* polarity status from the line; 1 = reversed; 0 = normal */
   unsigned char             nPolarity;
   /* automatic battery switch; 1 = automatic; 0 = normal */
   unsigned char             nBatterySw;
   /* last line feed mode; is set after ringing stops */
   unsigned char             nLineMode;
   /* set when the fault condition occurs of the device. It will be reseted
      when the line mode is modified */
   unsigned char             bFaulCond;
}TAPI_OPCONTROL_DATA;

/*=====================================*/
/* Defines for validation time types   */
/*=====================================*/

/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Validation types used for structure \ref TAPI_VALIDATION_TIMES
   \remarks
   The default values are as follows:

   - 80 ms  <= flash time      <= 200 ms
   - 30 ms  <= digit low time  <= 80 ms
   - 30 ms  <= digit high time <= 80 ms
   - interdigit time =     300 ms
   - off hook time   =      40 ms
   - on hook time    =     400 ms
   !!! open: only min time is validated and pre initialized */
typedef enum
{
   /** Settings for hook validation, if the time matches between nMinTime and
      nMaxTime an exception is raised. */
   HOOKOFF_TIME     = 0x0,
   /** Settings for hook validation, if the time matches between nMinTime and
       nMaxTime an exception is raised. */
   HOOKON_TIME      = 0x1,
   /** Settings for hook flash validation also known as register recall.
       If the time matches between the time defined in the fields nMinTime
       and nMaxTime an exception is raised */
   HOOKFLASH_TIME   = 0x2,
   /** Settings for pulse digit low, open loop and make validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   DIGITLOW_TIME    = 0x4,
   /** Settings for pulse digit high, close loop and break validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   DIGITHIGH_TIME   = 0x8,
   /** Settings for pulse digit pause. The time must match the time defined
       in the fields nMinTime and nMaxTime to recognize it as
       pulse dialing event */
   INTERDIGIT_TIME  = 0x10
} TAPI_VALIDATION_TYPES;

/* ============================= */
/* Structure for validation      */
/* time settings                 */
/* ============================= */

/** Structure used for validation times of hook, hook flash and pulse dialing */
typedef struct
{
   /** Type of validation time setting */
   TAPI_VALIDATION_TYPES nType;
   /** Minimum time for validation in ms */
   unsigned long         nMinTime;
   /** maximum time for validation in ms */
   unsigned long         nMaxTime;
}TAPI_VALIDATION_TIMES;
/*@}*/

/* ============================= */
/* types for status request      */
/* ============================= */

/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Enumeration for hook status events */
typedef enum
{
   /** Hook detected */
   TAPI_HOOKSTATUS_HOOK    = 0x01,
   /** Hook flash detected */
   TAPI_HOOKSTATUS_FLASH   = 0x02,
   /** Detected hook event is an Offhook */
   TAPI_HOOKSTATUS_OFFHOOK = 0x04
}TAPI_HOOKSTATUS;

/** Enumeration for dial status events */
typedef enum
{
   /** DTMF sign detected */
   TAPI_DIALINGSTATUS_DTMF  = 0x01,
   /** pulse digit detected */
   TAPI_DIALINGSTATUS_PULSE = 0x02
}TAPI_DIALING;

/** Enumeration for phone line status information */
typedef enum
{
   /** line is ringing */
   TAPI_LINESTATUS_RINGING = 0x1,
   /** ringing finished */
   TAPI_LINESTATUS_RINGFINISHED = 0x02,
   /** fax detcted -> is replaced by signal */
   TAPI_LINESTATUS_FAX = 0x04,
   /** ground key detected */
   TAPI_LINESTATUS_GNDKEY = 0x10,
   /** ground key high detected */
   TAPI_LINESTATUS_GNDKEYHIGH = 0x20,
   /** overtemperature detected */
   TAPI_LINESTATUS_OTEMP = 0x40,
   /** ground key polarity detected */
   TAPI_LINESTATUS_GNDKEYPOL = 0x80,
   /** GR909 result is available and can be queried with the appropriate
       interface */
   TAPI_LINESTATUS_GR909RES = 0x100,
   /** caller id received */
   TAPI_LINESTATUS_CIDRX    = 0x200,
   /** Event occurs when the line mode may be switched to
       TAPI_LINEFEED_NORMAL_LOW to save power */
   TAPI_LINESTATUS_FEEDLOWBATT = 0x400
}TAPI_LINESTATUS;

/** Enumeration for phone line status information */
typedef enum
{
   /** no error */
   TAPI_ERROR_NONE = 0x0,
   /** ring cadence settings error in cid tx */
   TAPI_ERROR_RINGCADENCE_CIDTX = 0x1
}  TAPI_ERROR;

/** Structure used for IFXPHONE_PHONE_STATUS to query the phone status of one or
    more channels    */
typedef struct
{
   /** Size of the list. If 0 only the status of this channel is queried. If
       channels not equal to 0 all channels starting with 0 till the value
       of channels are retrieved.
       As the return value it defines the channel number */
   unsigned char channels;
   /** Lists the hook status events

   - 0x01: TAPI_HOOKSTATUS_HOOK, Hook detected
   - 0x02: TAPI_HOOKSTATUS_FLASH, Hook flash detected
   - 0x04: TAPI_HOOKSTATUS_OFFHOOK, Detected hook event is an off hook. May
           be set additionally to TAPI_HOOKSTATUS_HOOK */
   unsigned char hook;
   /** Lists the dial status events */
   unsigned char dialing;
   /** Contains the digit in case of an dial event */
   unsigned char digit;
   /** Signals the line status and fault conditions

      - 0x1: TAPI_LINESTATUS_RINGING, PSTN line is ringing
      - 0x2: TAPI_LINESTATUS_RINGFINISHED, ringing finished on PSTN line
      - 0x4: TAPI_LINESTATUS_FAX, FAX status change
      - 0x10: TAPI_LINESTATUS_GNDKEY, Ground key detected
      - 0x20: TAPI_LINESTATUS_GNDKEYHIGH, Ground key high detected
      - 0x40: TAPI_LINESTATUS_OTEMP, Overtemperature detected
      - 0x80: TAPI_LINESTATUS_GNDKEYPOL, Ground key polarity detected
      - 0x100: TAPI_LINESTATUS_GR909RES,
      - 0x200: TAPI_LINESTATUS_CIDRX, Cid Receiver event occured
      - 0x400: TAPI_LINESTATUS_FEEDLOWBATT, Event occurs when the line mode
        may be switched to TAPI_LINEFEED_NORMAL_LOW to save power */
   unsigned int line;
   /** Signals the detection of events.
      The signals/events detected are reported in this field and represented
      according to the definition of \ref TAPI_SIGNAL. Following description
      is taken from the definition of \ref TAPI_SIGNAL. Please check it out
      for further information.

      - 0x0: TAPI_SIGNAL_NONE,     no signal detected
      - 0x1: TAPI_SIGNAL_DISRX, V.21 Preamble Fax Tone, Digital Identification
         Signal (DIS), receive path
      - 0x2: TAPI_SIGNAL_DISTX, V.21 Preamble Fax Tone, Digital Identification
         Signal (DIS), transmit path
      - 0x4: TAPI_SIGNAL_DIS, V.21 Preamble Fax Tone in all path,
         Digital Identification Signal (DIS)
      - 0x8: TAPI_SIGNAL_CEDRX, V.25 2100 Hz (CED) Modem/Fax Tone, receive path
      - 0x10: TAPI_SIGNAL_CEDTX, V.25 2100 Hz (CED) Modem/Fax Tone, transmit path
      - 0x20: TAPI_SIGNAL_CED, V.25 2100 Hz (CED) Modem/Fax Tone in all paths
      - 0x40: TAPI_SIGNAL_CNGFAXRX, CNG Fax Calling Tone (1100 Hz) receive path
      - 0x80: TAPI_SIGNAL_CNGFAXTX, CNG Fax Calling Tone (1100 Hz) transmit path
      - 0x100: TAPI_SIGNAL_CNGFAX, CNG Fax Calling Tone (1100 Hz) in all paths
      - 0x200: TAPI_SIGNAL_CNGMODRX, CNG Modem Calling Tone (1300 Hz)
         receive path
      - 0x400: TAPI_SIGNAL_CNGMODTX, CNG Modem Calling Tone (1300 Hz)
         transmit path
      - 0x800: TAPI_SIGNAL_CNGMOD, CNG Modem Calling Tone (1300 Hz) in all paths
      - 0x1000: TAPI_SIGNAL_PHASEREVRX, Phase reversal detection receive path
      - 0x2000: TAPI_SIGNAL_PHASEREVTX, Phase reversal detection transmit path
      - 0x4000: TAPI_SIGNAL_PHASEREV, Phase reversal detection in all paths
      - 0x8000: TAPI_SIGNAL_AMRX, Amplitude modulation receive path
      - 0x10000: TAPI_SIGNAL_AMTX, Amplitude modulation transmit path
      - 0x20000: TAPI_SIGNAL_AM, Amplitude modulation.
      - 0x40000: TAPI_SIGNAL_TONEHOLDING_ENDRX, Modem tone holding signal
         stopped receive path
      - 0x80000: TAPI_SIGNAL_TONEHOLDING_ENDTX, Modem tone holding signal
         stopped transmit path
      - 0x100000: TAPI_SIGNAL_TONEHOLDING_END, Modem tone holding signal
         stopped all paths
      - 0x200000: TAPI_SIGNAL_CEDENDRX, End of signal CED detection receive path
      - 0x400000: TAPI_SIGNAL_CEDENDTX, End of signal CED detection transmit path
      - 0x800000: TAPI_SIGNAL_CEDEND, End of signal CED detection.
      - 0x1000000: TAPI_SIGNAL_CPTD, Call progress tone detected.
      - 0x8000000: TAPI_SIGNAL_CIDENDTX, Caller ID transmission finished. */
   unsigned long signal;
   /** device specific status information */
   unsigned long device;
   /** RFC 2833 event playout information. This field contains the last event,
       that was received from network side.

      - 0:  TAPI_RFC2833_EVENT_DTMF_0, RFC2833 Event number for DTMF tone #0
      - 1:  TAPI_RFC2833_EVENT_DTMF_1, RFC2833 Event number for DTMF tone #1
      - 2:  TAPI_RFC2833_EVENT_DTMF_2, RFC2833 Event number for DTMF tone #2
      - 3:  TAPI_RFC2833_EVENT_DTMF_3, RFC2833 Event number for DTMF tone #3
      - 4:  TAPI_RFC2833_EVENT_DTMF_4, RFC2833 Event number for DTMF tone #4
      - 5:  TAPI_RFC2833_EVENT_DTMF_5, RFC2833 Event number for DTMF tone #5
      - 6:  TAPI_RFC2833_EVENT_DTMF_6, RFC2833 Event number for DTMF tone #6
      - 7:  TAPI_RFC2833_EVENT_DTMF_7, RFC2833 Event number for DTMF tone #7
      - 8:  TAPI_RFC2833_EVENT_DTMF_8, RFC2833 Event number for DTMF tone #8
      - 9:  TAPI_RFC2833_EVENT_DTMF_9, RFC2833 Event number for DTMF tone #9
      - 10: TAPI_RFC2833_EVENT_DTMF_STAR, RFC2833 Event number for DTMF
            tone STAR
      - 11: TAPI_RFC2833_EVENT_DTMF_HASH, RFC2833 Event number for DTMF
            tone HASH
      - 32: TAPI_RFC2833_EVENT_ANS, RFC2833 Event number for ANS tone
      - 33: TAPI_RFC2833_EVENT_NANS, RFC2833 Event number for /ANS tone
      - 34: TAPI_RFC2833_EVENT_ANSAM, RFC2833 Event number for ANSam tone
      - 35: TAPI_RFC2833_EVENT_NANSAM, RFC2833 Event number for /ANSam tone
      - 36: TAPI_RFC2833_EVENT_CNG, RFC2833 Event number for CNG tone
      - 54: TAPI_RFC2833_EVENT_DIS, RFC2833 Event number for DIS tone */
   unsigned long event;
   /** Set of runtime errors which happens during the processing of a
       tapi command.

       - 0x0: TAPI_ERROR_NONE, No runtime error
       - 0x1: TAPI_ERROR_RINGCADENCE_CIDTX, Ring cadence cofiguration error
              for CID transmission   */
   unsigned long error;
}TAPI_PHONE_STATUS;
/*@}*/

/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/
/* ============================= */
/* Structures for simple and     */
/* complex tone settings         */
/* ============================= */

/** Maximum tone generation steps, also called cadences */
#define TAPI_TONE_MAXSTEPS 6

/** Frequency setting for a cadence step */
typedef enum
{
   TAPI_TONE_FREQNONE = 0,
   TAPI_TONE_FREQA = 0x1,
   TAPI_TONE_FREQB = 0x2,
   TAPI_TONE_FREQC = 0x4,
   TAPI_TONE_FREQD = 0x8,
   TAPI_TONE_FREQALL = 0xF
}TAPI_TONE_FREQ;

/** Modulation setting for a cadence step */
typedef enum
{
   TAPI_TONE_MODULATION_OFF = 0,
   TAPI_TONE_MODULATION_ON = 1
}TAPI_TONE_MODULATION;

/** Tone types */
typedef enum
{
   TONE_SIMPLE = 1,
   TONE_COMPOSED = 2
}TAPI_TONE_TYPES;

/** Structure for definition of simple tones */
typedef struct
{
    /** Indicate the type of the tone descriptor:

    - 1: TONE_SIMPLE, The tone descriptor describe a simple tone
    - 2: TONE_COMPOSED, The tone descriptor describes a composed tone. */
    unsigned int format;
    /** Name of the simple tone */
    char name[30];
    /** Tone code ID from TAPI_TONE_MIN_INDEX up to max TAPI_TONE_MAX_INDEX */
    unsigned int index;
    /** Number of times to repeat the simple tone sequence plus 1.
        The loop count if not equal 0 defines the time of the entire sequence:
         tone_seq = loop * (onA + offA + onB + offB + onC + offC + pause)

    - 0: for infinite
    - 1: play it once
    - 2: play it twice (repeat)
    ...
    - 8: play it eight times (repeat seven times) */
    unsigned int loop;
     /** Power level for frequency A in 0.1 dB with -300 < levelA < 0 */
    int levelA;
     /** Power level for frequency B in 0.1 dB with -300 < levelB < 0 */
    int levelB;
     /** Power level for frequency C in 0.1 dB with -300 < levelC < 0 */
    int levelC;
    /** Power level for frequency D in 0.1 dB with -300 < levelD < 0 */
    int levelD;
    /** Tone frequency A in Hz where 1 < Hz < 4000 */
    unsigned int freqA;
    /** Tone frequency B in Hz where 0 < Hz < 4000 */
    unsigned int freqB;
    /** Tone frequency C in Hz where 0 < Hz < 4000 */
    unsigned int freqC;
    /** Tone frequency D in Hz where 0 < Hz < 4000 */
    unsigned int freqD;
    /** Time in ms of the cadence step with a granularity of 2ms  */
    unsigned int cadence[TAPI_TONE_MAXSTEPS];
    /** Active frequencies for the cadence step.
      Each frequencies is a four bit binary value. Each bit of the value
      represent one frequency. All active frequencies are summed together.
      The value for each frequencies can be a combination of the following

      - 0x0:  TAPI_TONE_FREQNONE, all frequencies are inactive - pause
      - 0x01: TAPI_TONE_FREQA, play frequency A
      - 0x02: TAPI_TONE_FREQB, play frequency B
      - 0x04: TAPI_TONE_FREQC, play frequency C
      - 0x08: TAPI_TONE_FREQD, play frequency D
      - 0x0F: TAPI_TONE_FREQALL, play all frequencies */
    unsigned int frequencies[TAPI_TONE_MAXSTEPS];
    /** Modulation of the frequency A with frequency B for the cadence step

    - 0x0: TAPI_TONE_MODULATION_OFF, Modulation of frequency A with
           frequency B is disabled
    - 0x1: TAPI_TONE_MODULATION_ON, Modulation of frequency A with
           frequency B is enabled */
    unsigned int modulation[TAPI_TONE_MAXSTEPS];
    /** Time for additional pause time between simple tones
    in ms (0 <= ms <= 32000).
    If pause is not equal 0, loop must indicate infinite repetitions (0) or
    at least more than one repetition.
    Pause is the time between each tone sequence. Each secuence is composed
    by several frecuency tones. */
    unsigned int pause;
} TAPI_SIMPLE_TONE;

/** Structure for definition of composed tones */
typedef struct
{
    /** Indicate the type of the tone descriptor:

    - 1: TONE_SIMPLE, The tone descriptor describe a simple tone
    - 2: TONE_COMPOSED, The tone descriptor describes a composed tone. */
    unsigned int format;
    /** name of the composed tone */
    char name[30];
    /** tone code ID up to max TAPI_TONE_MAX_INDEX */
    unsigned int index;
    /** number of times to repeat the composed tone sequence */
    unsigned int loop;
    /** time for voice path to be active (reestablished) in ms
    (0 <= ms <= 64000) */
    unsigned int alternatVoicePath;
    /** total number of simple tones to be played */
    unsigned int count;
    /** simple tone code IDs to be played */
    unsigned int tones[TAPI_MAX_SIMPLE_TONES];
}TAPI_COMPOSED_TONE;

/** Tone descriptor */
typedef union
{
      TAPI_SIMPLE_TONE simple;
      TAPI_COMPOSED_TONE composed;
}TAPI_TONE;
/*@}*/

/** Structure used for the TAPI version support check
   \ingroup TAPI_INTERFACE_MISC  */
typedef struct
{
   /** major version number supported */
   unsigned char major;
   /** minor version number supported */
   unsigned char minor;
}TAPI_PHONE_VERSION;

/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/
/** List the tone detection options. Some application maybe not interessted
    whether the signal came from the receive or transmit path. Therefore
    for each signal a mask exists, that includes receive and transmit path.
    */
typedef enum
{
   /** no signal detected */
   TAPI_SIGNAL_NONE        = 0x0,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      receive path */
   TAPI_SIGNAL_DISRX       = 0x1,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      transmit path */
   TAPI_SIGNAL_DISTX       = 0x2,
   /** V.21 Preamble Fax Tone in all path, Digital identification
       signal (DIS)  */
   TAPI_SIGNAL_DIS         = 0x4,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, receive path */
   TAPI_SIGNAL_CEDRX       = 0x8,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, transmit path */
   TAPI_SIGNAL_CEDTX       = 0x10,
   /** V.25 2100 Hz (CED) Modem/Fax Tone in all paths */
   TAPI_SIGNAL_CED         = 0x20,
   /** CNG Fax Calling Tone (1100 Hz) receive path */
   TAPI_SIGNAL_CNGFAXRX    = 0x40,
   /** CNG Fax Calling Tone (1100 Hz) transmit path */
   TAPI_SIGNAL_CNGFAXTX    = 0x80,
   /** CNG Fax Calling Tone (1100 Hz) in all paths */
   TAPI_SIGNAL_CNGFAX      = 0x100,
   /** CNG Modem Calling Tone (1300 Hz) receive path */
   TAPI_SIGNAL_CNGMODRX    = 0x200,
   /** CNG Modem Calling Tone (1300 Hz) transmit path  */
   TAPI_SIGNAL_CNGMODTX    = 0x400,
   /** CNG Modem Calling Tone (1300 Hz) in all paths */
   TAPI_SIGNAL_CNGMOD      = 0x800,
   /** Phase reversal detection receive path
       \remarks Not supported phase reversal uses the same
       paths as CED detection. The detector for CED must be configured
       as well  */
   TAPI_SIGNAL_PHASEREVRX  = 0x1000,
   /** Phase reversal detection transmit path
      \remarks Not supported phase reversal uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_PHASEREVTX  = 0x2000,
   /** Phase reversal detection in all paths
       \remarks Phase reversals are detected at the
       end of an CED. If this signal is enabled also CED end detection
       is automatically enabled and reported if it occurs */
   TAPI_SIGNAL_PHASEREV    = 0x4000,
   /** Amplitude modulation receive path

      \remarks Not supported amplitude modulation uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_AMRX                 = 0x8000,
   /** Amplitude modulation transmit path

   \remarks Not supported amplitude modulation uses the same
   paths as CED detection. The detector for CED must be configured
   as well  */
   TAPI_SIGNAL_AMTX                 = 0x10000,
   /** Amplitude modulation.

    \remarks Amplitude modulation is detected at the
    end of an CED. If this signal is enabled also CED end detection
    is automatically enabled and reported if it occurs.

    \note In case of AM detected the driver automatically switches
    to modem coefficients after the ending of CED */
   TAPI_SIGNAL_AM                   = 0x20000,
   /** Modem tone holding signal stopped receive path */
   TAPI_SIGNAL_TONEHOLDING_ENDRX    = 0x40000,
   /** Modem tone holding signal stopped transmit path */
   TAPI_SIGNAL_TONEHOLDING_ENDTX    = 0x80000,
   /** Modem tone holding signal stopped all paths */
   TAPI_SIGNAL_TONEHOLDING_END      = 0x100000,
   /** End of signal CED detection receive path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_CEDENDRX          = 0x200000,
   /** End of signal CED detection transmit path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   TAPI_SIGNAL_CEDENDTX          = 0x400000,
   /** End of signal CED detection. This signal also includes
      information about phase reversals and amplitude modulation,
      if enabled */
   TAPI_SIGNAL_CEDEND            = 0x800000,
   /** Signals a call progress tone detection. This signal is enabled with
       the interface \ref IFXPHONE_CPTD_START and stopped with
       \ref IFXPHONE_CPTD_STOP. It can not be activate with
       \ref IFXPHONE_ENABLE_SIGDETECT */
   TAPI_SIGNAL_CPTD              = 0x1000000,
   /** Signals the V8bis detection on the receive path */
   TAPI_SIGNAL_V8BISRX           = 0x2000000,
   /** Signals the V8bis detection on the transmit path */
   TAPI_SIGNAL_V8BISTX           = 0x4000000,
   /** Signals that the Caller-ID transmission has finished */
   TAPI_SIGNAL_CIDENDTX          = 0x8000000
}TAPI_SIGNAL;

/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/
/** List the RFC2833 tone events that are detected from network side.
    */
typedef enum
{
   /** RFC2833 Event number for DTMF tone  #0 */
   TAPI_RFC2833_EVENT_DTMF_0     = 0,
   /** RFC2833 Event number for DTMF tone  #1 */
   TAPI_RFC2833_EVENT_DTMF_1     = 1,
   /** RFC2833 Event number for DTMF tone  #2 */
   TAPI_RFC2833_EVENT_DTMF_2     = 2,
   /** RFC2833 Event number for DTMF tone  #3 */
   TAPI_RFC2833_EVENT_DTMF_3     = 3,
   /** RFC2833 Event number for DTMF tone  #4 */
   TAPI_RFC2833_EVENT_DTMF_4     = 4,
   /** RFC2833 Event number for DTMF tone  #5 */
   TAPI_RFC2833_EVENT_DTMF_5     = 5,
   /** RFC2833 Event number for DTMF tone  #6 */
   TAPI_RFC2833_EVENT_DTMF_6     = 6,
   /** RFC2833 Event number for DTMF tone  #7 */
   TAPI_RFC2833_EVENT_DTMF_7     = 7,
   /** RFC2833 Event number for DTMF tone  #8 */
   TAPI_RFC2833_EVENT_DTMF_8     = 8,
   /** RFC2833 Event number for DTMF tone  #9 */
   TAPI_RFC2833_EVENT_DTMF_9     = 9,
   /** RFC2833 Event number for DTMF tone STAR */
   TAPI_RFC2833_EVENT_DTMF_STAR  = 10,
   /** RFC2833 Event number for DTMF tone HASH */
   TAPI_RFC2833_EVENT_DTMF_HASH  = 11,
   /** RFC2833 Event number for ANS tone */
   TAPI_RFC2833_EVENT_ANS        = 32,
   /** RFC2833 Event number for /ANS tone */
   TAPI_RFC2833_EVENT_NANS       = 33,
   /** RFC2833 Event number for ANSam tone */
   TAPI_RFC2833_EVENT_ANSAM      = 34,
   /** RFC2833 Event number for /ANSam tone */
   TAPI_RFC2833_EVENT_NANSAM     = 35,
   /** RFC2833 Event number for CNG tone */
   TAPI_RFC2833_EVENT_CNG        = 36,
   /** RFC2833 Event number for DIS tone */
   TAPI_RFC2833_EVENT_DIS        = 54,
   /** no support RFC event received or no event received */
   TAPI_RFC2833_EVENT_NO_EVENT   = 0xFFFFFFFF
}TAPI_RFC2833_EVENT;

/** Structure used for enable and disable signal detection. See
   \ref IFXPHONE_ENABLE_SIGDETECT and \ref IFXPHONE_DISABLE_SIGDETECT
   The signals are reported via an TAPI exception service. The status
   information is queried with \ref IFXPHONE_PHONE_STATUS. */
typedef struct
{
   /** Signals to detect. Can be any combination of \ref TAPI_SIGNAL */
   unsigned long sig;
}TAPI_PHONE_SIGDETECTION;

/** Specifies the CPT signal for CPT detection */
typedef enum
{
   TAPI_CPT_SIGNALRX = 0x1,
   TAPI_CPT_SIGNALTX = 0x2
}TAPI_CPT_SIGNAL;

/** Structure used for \ref IFXPHONE_CPTD_START and \ref IFXPHONE_CPTD_STOP */
typedef struct
{
   /** The index of the tone to detect. The tone index must be programmed
   with IFXPHONE_ADD_SIMPLETONE */
   int tone;
   /** The specification of the signal

      - 1: TAPI_CPT_SIGNALRX, receive direction of the programmed CPT tone
      - 2: TAPI_CPT_SIGNALTX, transmit direction of the programmed CPT tone */
   int signal;
}TAPI_PHONE_CPTD;
/*@}*/

/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Specifies the Enable/Disable mode of the AGC resouce */
typedef enum
{
   TAPI_AGC_DISABLE = 0x0,
   TAPI_AGC_ENABLE  = 0x1
}TAPI_AGC_MODE;
/*@}*/

#endif  /* DRV_TAPI_IO_H */

