#ifndef DRV_TAPI_IO_H
#define DRV_TAPI_IO_H
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
   Module      : drv_tapi_io.h
   Desription  : Contains TAPI I/O defines, enums and structures
                 according to LTAPI specification.
   \file TAPI ioctl defines
   \remarks
      The file is divided in sections ioctl commands, constants, enumerations,
      structures for the following groups :
      - TAPI_INTERFACE_INIT
      - TAPI_INTERFACE_OP
      - TAPI_INTERFACE_METER
      - TAPI_INTERFACE_TONE
      - TAPI_INTERFACE_DIAL
      - TAPI_INTERFACE_SIGNAL
      - TAPI_INTERFACE_CID
      - TAPI_INTERFACE_CON
      - TAPI_INTERFACE_MISC
      - TAPI_INTERFACE_RINGING
      - TAPI_INTERFACE_PCM
      - TAPI_INTERFACE_FAX
   \note Changes:
   IFX_TAPI_MAP_DATA_t nPhoneCh changed to nDstCh because it can also be a PCM.
   IFX_TAPI_MAP_DATA_t nDataChType changed to nChType for destination type
   IFX_TAPI_MAP_PCM_ADD and REMOVE added
*******************************************************************************/

#ifndef TAPI_VERSION2
#define TAPI_VERSION2
#endif

/** \defgroup TAPI_INTERFACE TAPI Driver Interface
    Lists the entire interface to the TAPI */
/*@{*/

/**
   \defgroup TAPI_INTERFACE_INIT Initialization service
   This service sets the default initialization of device and hardware.
   All default settings are linked as fixed tables to the source code
   of the TAPI. These tables can be easily replaced by user defined tables.  */

/** \defgroup TAPI_INTERFACE_OP Operation Control Service
    Modifies the operation of the device.
    All operation control services apply to phone channels except otherwise
    stated.  */

/** \defgroup TAPI_INTERFACE_METER Metering Service
    Contains services for metering.
    All metering services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_TONE Tone Control Service
    All tone services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_DIAL Dial Service
    Contains services for dialing.
    All dial services apply to phone channels except otherwise stated. */

/** \defgroup TAPI_INTERFACE_SIGNAL Signal Services
   The application handles the different states of the detection status
   like depicted in the image \image html DataChSwitching.emf. */

/** \defgroup TAPI_INTERFACE_CID Caller ID Features
   Interface for configuration, sending and receiving CID
   (different types and standards) */

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

/** \defgroup TAPI_INTERFACE_TEST Testing Service
    Contains services for system tests like hook generation and loops. */

/*@}*/

/* ========================================================================== */
/*                     TAPI Interface Ioctl Commands                          */
/* ========================================================================== */

/** magic number for ioctls*/
#define IFX_TAPI_IOC_MAGIC 'q'

/* ======================================================================== */
/* TAPI Initialization Services, ioctl commands (Group TAPI_INTERFACE_INIT) */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** This service sets the default initialization of device and hardware.

   \param IFX_TAPI_CH_INIT_t* The parameter points to a
          \ref IFX_TAPI_CH_INIT_t structure

   \code
   IFX_TAPI_CH_INIT_t Init;
   Init.nMode = 0;
   Init.nCountry = 2;
   Init.pProc = &DevInitStruct;
   ioctl(fd, IFX_TAPI_CH_INIT, &Init);
   \endcode
*/
#define  IFX_TAPI_CH_INIT                          _IO(IFX_TAPI_IOC_MAGIC, 0x0F)

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ========================================================================= */
/* TAPI Operation Control Services, ioctl commands (Group TAPI_INTERFACE_OP) */
/* ========================================================================= */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** This service sets the line feeding mode

   \param int The parameter represents an mode value.

   - 0: IFX_TAPI_LINE_FEED_ACTIVE, Normal feeding mode for phone off hook
   - 1: IFX_TAPI_LINE_FEED_ACTIVE_REV, reversed feeding mode for phone off hook
   - 2: IFX_TAPI_LINE_FEED_STANDBY, power down resistance = on hook with
   hook detection
   - 3: IFX_TAPI_LINE_FEED_HIGH_IMPEDANCE, Switch off the line, but the
   device is able to test the line
   - 4: IFX_TAPI_LINE_FEED_DISABLED, Switch off the line and the device
   - 5: IFX_TAPI_LINE_FEED_GROUND_START, ground start mode
   - 6: IFX_TAPI_LINE_FEED_NORMAL_AUTO, normal with automatic battery switch.
   Thresholds for automatic battery switch are set via coefficient settings
   - 7: IFX_TAPI_LINE_FEED_REVERSED_AUTO, reversed with automatic battery
        switch
   - 8: IFX_TAPI_LINE_FEED_NORMAL_LOW, feeding mode for phone off hook with
        low battery to save power
   - 9: IFX_TAPI_LINE_FEED_REVERSED_LOW, feeding mode for phone off hook with
        low battery to save power and reserved polarity

   \remarks For all battery switching modes the hardware must be able to
   support it for example by programming coefficients.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // set line mode to power down
   ioctl(fd, IFX_TAPI_LINE_FEED_SET, IFX_TAPI_LINE_FEED_DISABLED);
   \endcode
*/
#define  IFX_TAPI_LINE_FEED_SET                    _IO(IFX_TAPI_IOC_MAGIC, 0x01)

/** Set the line echo canceller (LEC) configuration

   \param IFX_TAPI_LEC_CFG_t* The parameter points to a
          \ref IFX_TAPI_LEC_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_LEC_CFG_t param;
   memset (&param, 0, sizeof(IFX_TAPI_LEC_CFG_t));
   param.nType   = IFX_TAPI_LEC_TYPE_NLEC;
   param.nGainTx = IFX_TAPI_LEC_GAIN_MEDIUM;
   param.nGainRx = IFX_TAPI_LEC_GAIN_MEDIUM;
   param.nLen = 16; // 16ms
   param.bNlp = IFX_TAPI_LEC_NLP_DEFAULT;
   ret = ioctl(fd, IFX_TAPI_LEC_PHONE_CFG_SET, &param)
   \endcode
*/
#define  IFX_TAPI_LEC_PHONE_CFG_SET          _IOW(IFX_TAPI_IOC_MAGIC, 0x28, int)

/** Get the LEC configuration.

   \param IFX_TAPI_LEC_CFG_t* The parameter points to a
          \ref IFX_TAPI_LEC_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

*/
#define  IFX_TAPI_LEC_PHONE_CFG_GET          _IOR(IFX_TAPI_IOC_MAGIC, 0x29, int)


/** Set the line echo canceller (LEC) configuration for PCM

   \param IFX_TAPI_LEC_CFG_t* The parameter points to a
          \ref IFX_TAPI_LEC_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_LEC_CFG_t param;
   memset (&param, 0, sizeof(IFX_TAPI_LEC_CFG_t));
   param.nType   = IFX_TAPI_LEC_TYPE_NLEC;
   param.nGainTx = IFX_TAPI_LEC_GAIN_MEDIUM;
   param.nGainRx = IFX_TAPI_LEC_GAIN_MEDIUM;
   param.nLen = 16; // 16ms
   param.bNlp = IFX_TAPI_LEC_NLP_DEFAULT;
   ret = ioctl(fd, IFX_TAPI_LEC_PCM_CFG_SET, &param)
   \endcode
*/
#define  IFX_TAPI_LEC_PCM_CFG_SET            _IOW(IFX_TAPI_IOC_MAGIC, 0x50, int)

/** Get the LEC configuration for PCM.

   \param IFX_TAPI_LEC_CFG_t* The parameter points to a
          \ref IFX_TAPI_LEC_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

*/
#define  IFX_TAPI_LEC_PCM_CFG_GET            _IOR(IFX_TAPI_IOC_MAGIC, 0x51, int)

/** Specifies the time for hook, pulse digit and hook flash validation

   \param IFX_TAPI_LINE_HOOK_VT_t* The parameter points to a
   \ref IFX_TAPI_LINE_HOOK_VT_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The following conditions must be met:
      - IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME
        min and max < IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME min and max
      - IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME
        min and max < IFX_TAPI_LINE_HOOK_VT_HOOKON_TIME min and max

   \code
   IFX_TAPI_LINE_HOOK_VT_t param;
   memset (&param, 0, sizeof(IFX_TAPI_LINE_HOOK_VT_t));
   // set pulse dialing
   param.nType = IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME;
   param.nMinTime = 40;
   param.nMaxTime = 60;
   ioctl(fd, IFX_TAPI_LINE_HOOK_VT_SET, (int)&param);
   param.nType = IFX_TAPI_LINE_HOOK_VT_DIGITHIGH_TIME;
   param.nMinTime = 40;
   param.nMaxTime = 60;
   ioctl(fd, IFX_TAPI_LINE_HOOK_VT_SET, (int)&param);
   \endcode */
#define  IFX_TAPI_LINE_HOOK_VT_SET           _IOW(IFX_TAPI_IOC_MAGIC, 0x2E, int)


/** Sets the speaker phone and microphone volume settings.

   \param IFX_TAPI_LINE_VOLUME_t* The parameter points to a
   \ref IFX_TAPI_LINE_VOLUME_t structure.

   This interface
    expects a structure of type \ref IFX_TAPI_LINE_VOLUME_t

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_LINE_VOLUME_t param;
   memset (&param, 0, sizeof(IFX_TAPI_LINE_VOLUME_t));
   param.nTx = 24;
   param.nRx = 0;
   ret = ioctl(fd, IFX_TAPI_PHONE_VOLUME_SET, &param);
   \endcode
*/
#define  IFX_TAPI_PHONE_VOLUME_SET           _IOW(IFX_TAPI_IOC_MAGIC, 0x42, int)

/** Sets the PCM interface volume settings.

   \param IFX_TAPI_LINE_VOLUME_t* The parameter points to a
   \ref IFX_TAPI_LINE_VOLUME_t structure.

   This interface
    expects a structure of type \ref IFX_TAPI_LINE_VOLUME_t

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_LINE_VOLUME_t param;
   memset (&param, 0, sizeof(IFX_TAPI_LINE_VOLUME_t));
   param.nTx = 24;
   param.nRx = 0;
   ret = ioctl(fd, IFX_TAPI_PCM_VOLUME_SET, &param);
   \endcode
*/
#define  IFX_TAPI_PCM_VOLUME_SET             _IOW(IFX_TAPI_IOC_MAGIC, 0x45, int)

/** This service enables or disables a high level path of a phone channel.
   It is required to enable output signals with level higher than 3.14 dBm.

   \param int The parameter represent a boolean value of
          \ref IFX_TAPI_LINE_LEVEL_t.

   - 0: IFX_TAPI_LINE_LEVEL_DISABLE, disable the high level path.
   - 1: IFX_TAPI_LINE_LEVEL_ENABLE, enable the high level path.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service is intended for phone channels only and must be used
   in combination with \ref IFX_TAPI_PHONE_VOLUME_SET or
   \ref IFX_TAPI_PCM_VOLUME_SET to set the max. level
   (\ref IFX_TAPI_LINE_VOLUME_HIGH) or to restore level

   \remarks Only the order of calls with the parameters
    IFX_TAPI_LINE_LEVEL_ENABLE and then IFX_TAPI_LINE_LEVEL_DISABLE
    is supported

   \code
   ret = ioctl(fd, IFX_TAPI_LINE_LEVEL_SET, IFX_TAPI_LINE_LEVEL_ENABLE);
   // <play out some high level tones or samples>
   ret = ioctl(fd, IFX_TAPI_LINE_LEVEL_SET, IFX_TAPI_LINE_LEVEL_DISABLE);
   \endcode
*/
#define  IFX_TAPI_LINE_LEVEL_SET             _IOW(IFX_TAPI_IOC_MAGIC, 0x46, int)

/**  This service reads the hook status from the driver.

   \param int* The parameter is a pointer to the status.
   - 0: no hook detected
   - 1: hook detected

   \return 0 if successful, otherwise -1 in case of an error

   \note “off hook” corresponds to “ground start” depending on line feed mode.

   \code
   int nHook;
   ret = ioctl(fd, IFX_TAPI_LINE_HOOK_STATUS_GET, &nHook);
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
*/
#define  IFX_TAPI_LINE_HOOK_STATUS_GET            _IO (IFX_TAPI_IOC_MAGIC, 0x84)

/*@}*/ /* TAPI_INTERFACE_OP */

/* =================================================================== */
/* TAPI Metering Services, ioctl commands (Group TAPI_INTERFACE_METER) */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** This service sets the characteristic for the metering service.

   \param IFX_TAPI_METER_CFG_t* The parameter points to a
   \ref IFX_TAPI_METER_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If burst_cnt is not zero the burst number is defined by
   burst_cnt and stopping is not necessary. If burst_cnt is
   set to zero the bursts are sent out until a IFX_TAPI_METER_STOP
   ioctl is called.

   \code
   IFX_TAPI_METER_CFG_t Metering;
   memset (&Metering, 0, sizeof(IFX_TAPI_METER_CFG_t));
   //  Metering mode is already set to 0
   // 100ms burst length
   Metering.burst_len = 100;
   // 2 min. burst distance
   Metering.burst_dist = 120;
   // send out 2 bursts
   Metering.burst_cnt = 2;
   //  set metering characteristic
   ioctl(fd, IFX_TAPI_METER_CFG_SET, &Metering);
   \endcode   */
#define  IFX_TAPI_METER_CFG_SET                    _IO(IFX_TAPI_IOC_MAGIC, 0x0C)

/** This service starts the metering.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Before this service can be used,
   the metering characteristic must be set (service \ref IFX_TAPI_METER_CFG_SET)
   and the line mode must be set to normal
   (service \ref IFX_TAPI_LINE_FEED_SET).

   \code
   ioctl(fd, IFX_TAPI_METER_START, 0);
   \endcode   */
#define  IFX_TAPI_METER_START                      _IO(IFX_TAPI_IOC_MAGIC, 0x0D)

/** This service stops the metering.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If the metering has not been started before this service
   returns an error

   \code
   ioctl(fd, IFX_TAPI_METER_STOP, 0);
   \endcode   */
#define  IFX_TAPI_METER_STOP                       _IO(IFX_TAPI_IOC_MAGIC, 0x0E)

/*@}*/ /* TAPI_INTERFACE_METER */

/* ======================================================================= */
/* TAPI Tone Control Services, ioctl commands (Group TAPI_INTERFACE_TONE)  */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/**
   This service sets the tone level of the tone currently played.
   \param IFX_TAPI_PREDEF_TONE_LEVEL_t* The parameter points to a
   \ref IFX_TAPI_PREDEF_TONE_LEVEL_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks

   \code
   IFX_TAPI_PREDEF_TONE_LEVEL_t Tone;
   memset (&Tone, 0, sizeof(IFX_TAPI_PREDEF_TONE_LEVEL_t));
   Tone.generator = 0xFF;
   // all tone generators
   Tone.level = 0x100;
   // -38.97 dB level
   // playlevel in dB can be calculated via formula:
   // playlevel_in_dB = +3.17 + 20 log10 (nPlayLevel/32767)
   ioctl(fd, IFX_TAPI_TONE_LEVEL_SET, &Tone);
   \endcode
*/
#define  IFX_TAPI_TONE_LEVEL_SET                   _IO(IFX_TAPI_IOC_MAGIC, 0x08)

/** Configures a tone based on simple or composed tones

   \param IFX_TAPI_TONE_t* The parameter points to a
   \ref IFX_TAPI_TONE_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   A simple tone specifies a tone sequence composed of several single frequency
   tones or dual frequency tones. The sequence can be transmitted only one
   time, several times or until the transmission is stopped by client.
   This interface can add a simple tone to the internal table with maximum
   222 entries, starting from IFX_TAPI_TONE_INDEX_MIN (32).
   At least on cadence must be defined otherwise this interface returns an
   error. The tone table provides all tone frequencies in 5 Hz steps with a 5%
   tolerance and are defined in RFC 2833.
   For composed tones the loopcount of each simple tone must be
   different from 0.

   \code
   int fd;
   IFX_TAPI_CH_INIT_t tapi;
   IFX_TAPI_TONE_t tone;

   memset (&tapi,0, sizeof(IFX_TAPI_CH_INIT_t));
   memset (&tone,0, sizeof(IFX_TAPI_TONE_t));
   fd = open (dev_nodes[1], 0);
   ioctl (fd, IFX_TAPI_CH_INIT, (int)&tapi);
   tone.simple.format = IFX_TAPI_TONE_TYPE_SIMPLE;
   tone.simple.index = 71;
   tone.simple.freqA = 480;
   tone.simple.freqB = 620;
   tone.simple.levelA = -300;
   tone.simple.levelB = -300;
   tone.simple.levelC = -300;
   tone.simple.cadence[0] = 2000;
   tone.simple.cadence[1] = 2000;
   tone.simple.frequencies[0] = IFX_TAPI_TONE_FREQA | IFX_TAPI_TONE_FREQB;
   tone.simple.loop = 2;
   tone.simple.pause = 200;
   ioctl (fd, IFX_TAPI_TONE_TABLE_CFG_SET, (int)&tone);
   memset (&tone,0, sizeof(IFX_TAPI_TONE_t));
   tone.composed.format = IFX_TAPI_TONE_TYPE_COMPOSED;
   tone.composed.index = 100;
   tone.composed.count = 2;
   tone.composed.tones[0] = 71;
   tone.composed.tones[1] = 71;
   ioctl (fd, IFX_TAPI_TONE_TABLE_CFG_SET, (int)&tone);
   // now start playing the simple tone
   ioctl (fd, IFX_TAPI_TONE_LOCAL_PLAY, (int)71);
   // stop playing tone
   ioctl (fd, IFX_TAPI_TONE_STOP, (int)0);
   \endcode
*/
#define  IFX_TAPI_TONE_TABLE_CFG_SET         _IOW(IFX_TAPI_IOC_MAGIC, 0x36, int)

/** Start the call progress tone detection based on a previously defined
   simple tone.

   \param IFX_TAPI_TONE_CPTD_t* The parameter points to a
   \ref IFX_TAPI_TONE_CPTD_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_TONE_CPTD_t param;
   memset (&param, 0, sizeof(IFX_TAPI_TONE_CPTD_t));
   param.tone   = 74;
   param.signal = IFX_TAPI_TONE_CPTD_DIRECTION_TX;
   ioctl (fd, IFX_TAPI_TONE_CPTD_START, &param);
   \endcode   */
#define  IFX_TAPI_TONE_CPTD_START            _IOW(IFX_TAPI_IOC_MAGIC, 0xC3, int)

/** Stops the call progress tone detection

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error
   */
#define  IFX_TAPI_TONE_CPTD_STOP             _IOW(IFX_TAPI_IOC_MAGIC, 0xC4, int)

/** Plays a tone, which is defined before.

   \param int The parameter is the index of the tone to the predefined tone
   table (range 1 - 31) or custom tones added previously
   (index IFX_TAPI_TONE_INDEX_MIN - IFX_TAPI_TONE_INDEX_MAX).
   Index 0 means tone stop. Using the upper bits modify the
   default tone playing source:

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This can be a predefined, simple or a composed tone.
   The tone codes are assigned previously on system start.
   Index 1 - 31 is predefined by the driver and covers the
   original TAPI. A timing must be specified with
   \ref IFX_TAPI_TONE_ON_TIME_SET and \ref IFX_TAPI_TONE_OFF_TIME_SET
   before.
   All other indicies can be custom defined by
   \ref IFX_TAPI_TONE_TABLE_CFG_SET.

   \code
   // play tone index 34
   ioctl(fd, IFX_TAPI_TONE_LOCAL_PLAY, 34);
   \endcode   */
#define  IFX_TAPI_TONE_LOCAL_PLAY          _IOW (IFX_TAPI_IOC_MAGIC, 0x9B, char)

/** Plays a tone to the network side, which is defined before. Applies
   to data file descriptor only.

   \param int The parameter is the index of the tone to the predefined tone
   table (range 1 - 31) or custom tones added previously
   (index IFX_TAPI_TONE_INDEX_MIN - IFX_TAPI_TONE_INDEX_MAX).
   Index 0 means tone stop. Using the upper bits modify the
   default tone playing source:

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This can be a predefined, simple or a composed tone.
   The tone codes are assigned previously on system start.
   Index 1 - 31 is predefined by the driver and covers the
   original TAPI. A timing must be specified with
   \ref IFX_TAPI_TONE_ON_TIME_SET and \ref IFX_TAPI_TONE_OFF_TIME_SET
   before.
   All other indicies can be custom defined by
   \ref IFX_TAPI_TONE_TABLE_CFG_SET.

   \code
   // play tone index 34
   ioctl(fd, IFX_TAPI_TONE_NET_PLAY, 34);
   \endcode   */
#define  IFX_TAPI_TONE_NET_PLAY            _IOW (IFX_TAPI_IOC_MAGIC, 0xC5, char)

/**
   This service sets the on-time for a tone with a specified duration.
   After setting tone on/off time, the tone is started with service
   \ref IFX_TAPI_TONE_LOCAL_PLAY.

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
   ioctl(fd, IFX_TAPI_TONE_ON_TIME_SET, nDuration);
   \endcode   */
#define  IFX_TAPI_TONE_ON_TIME_SET          _IOW (IFX_TAPI_IOC_MAGIC, 0x9C, int)

/**
   This service sets the off-time for a tone with a specified duration.
   After setting tone on/off time, the tone is started with service
   \ref IFX_TAPI_TONE_LOCAL_PLAY.

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
   ioctl(fd, IFX_TAPI_TONE_OFF_TIME_SET, nDuration);
   \endcode   */
#define  IFX_TAPI_TONE_OFF_TIME_SET         _IOW (IFX_TAPI_IOC_MAGIC, 0x9D, int)

/**
   This service gets the current on-time duration which was set by a
   former \ref IFX_TAPI_TONE_ON_TIME_SET

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
   ret = ioctl(fd, IFX_TAPI_TONE_ON_TIME_GET, &nDuration);
   \endcode   */
#define  IFX_TAPI_TONE_ON_TIME_GET                _IO (IFX_TAPI_IOC_MAGIC, 0x9E)

/**
   This service gets the current off-time duration which was set by a
   former \ref IFX_TAPI_TONE_OFF_TIME_SET

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
   ret = ioctl(fd, IFX_TAPI_TONE_OFF_TIME_GET, &nDuration);
   \endcode   */
#define  IFX_TAPI_TONE_OFF_TIME_GET               _IO (IFX_TAPI_IOC_MAGIC, 0x9F)

/** This service gets the tone playing state.

   \param int* The parameter points to the tone state

   - 0: no tone is played
   - 1: tone is played (tone is within on-time)
   - 2: silence (tone is within off-time)

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nToneState;
   // get the tone state
   ret = ioctl(fd, IFX_TAPI_TONE_STATUS_GET, &nToneState);
   \endcode   */
#define  IFX_TAPI_TONE_STATUS_GET                 _IO (IFX_TAPI_IOC_MAGIC, 0xA0)

/** Pre-defined tone services for busy tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, IFX_TAPI_TONE_BUSY_PLAY, 0);
   \endcode   */
#define  IFX_TAPI_TONE_BUSY_PLAY                  _IO (IFX_TAPI_IOC_MAGIC, 0xA1)

/** Pre-defined tone services for ring back tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, IFX_TAPI_TONE_RINGBACK_PLAY, 0);
   \endcode   */
#define  IFX_TAPI_TONE_RINGBACK_PLAY              _IO (IFX_TAPI_IOC_MAGIC, 0xA2)

/**
   Pre-defined tone services for dial tone.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks These tones are defined by frequencies inside the TAPI for USA.

   \code
   ioctl(fd, IFX_TAPI_TONE_BUSY_PLAY, 0);
   \endcode   */
#define  IFX_TAPI_TONE_DIALTONE_PLAY              _IO (IFX_TAPI_IOC_MAGIC, 0xA3)

/**
   Stops playback of the current tone (call progress tone), or cadence.

   \param int Index of the tone to be stoppped

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Stopping tone can also be done with \ref IFX_TAPI_TONE_LOCAL_PLAY
    or IFX_TAPI_TONE_NET_PLAY and index 0

   \code
   // start dial tone
   ioctl(fd, IFX_TAPI_TONE_DIALTONE_PLAY, 0);
   // stop playing tone
   ioctl(fd, IFX_TAPI_TONE_STOP, 62);
   \endcode   */
#define  IFX_TAPI_TONE_STOP                       _IO (IFX_TAPI_IOC_MAGIC, 0xA4)

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ======================================================================= */
/* TAPI Dial Services, ioctl commands (Group TAPI_INTERFACE_DIAL)          */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/

/** This service checks if a pulse digit was received.

   \param int* The parameter points to the status. It returns the
   following values:

   - 0: No pulse digit is in the receive queue
   - 1: pulse digit is in the receive queue

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFX_TAPI_CH_STATUS_GET can be used upon exception. If the
   event reporting mask is not set (see \ref IFX_TAPI_EXCEPTION_MASK),
   \ref IFX_TAPI_CH_STATUS_GET reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int bReady;
   //  check the pulse dialing status
   ioctl(fd, IFX_TAPI_PULSE_READY, &bReady);
   if (bReady == 1)
   {
      //  digit arrived
   }
   else
   {
      //  no digit in the buffer
   }
   \endcode   */
#define  IFX_TAPI_PULSE_READY                      _IO(IFX_TAPI_IOC_MAGIC, 0x09)

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
   The command \ref IFX_TAPI_CH_STATUS_GET can be used upon exception. If the
   event reporting mask is not set (see \ref IFX_TAPI_EXCEPTION_MASK),
   \ref IFX_TAPI_CH_STATUS_GET reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int nDigit, ret;
   //  get pulse digit
   ret = ioctl(fd, IFX_TAPI_PULSE_GET, &nDigit);
   \endcode   */
#define  IFX_TAPI_PULSE_GET                        _IO(IFX_TAPI_IOC_MAGIC, 0x0A)

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
#define  IFX_TAPI_PULSE_ASCII_GET                  _IO(IFX_TAPI_IOC_MAGIC, 0x0B)


/** This service checks if a DTMF digit was received.

   \param int* Pointer to an integer for the status information.
   It returns the following values:

   - 0: No DTMF digit is in the receive queue
   - 1: DTMF digit is in the receive queue

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The command \ref IFX_TAPI_CH_STATUS_GET can be used upon exception. If the
   event reporting mask is not set (see \ref IFX_TAPI_EXCEPTION_MASK),
   \ref IFX_TAPI_CH_STATUS_GET reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int bReady;
   //  check the DTMF status
   ioctl(fd, IFX_TAPI_TONE_DTMF_READY_GET, &bReady);
   if (bReady == 1)
   {
      //  digit arrived
   }
   else
   {
      //  no digit in the buffer
   }
   \endcode   */
#define  IFX_TAPI_TONE_DTMF_READY_GET       _IOR (IFX_TAPI_IOC_MAGIC, 0x96, int)

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
   The command \ref IFX_TAPI_CH_STATUS_GET can be used upon exception. If the
   event reporting mask is not set (see \ref IFX_TAPI_EXCEPTION_MASK),
   \ref IFX_TAPI_CH_STATUS_GET reads out the digit. It is contained in the
   status information (field digit and dialing). It is then not available
   with this command.
   This approach minimizes the number of ioctls used to query all status
   information. No additional request to get the digit is necessary.

   \code
   int nDigit, ret;
   //  get DTMF digit
   ret = ioctl(fd, IFX_TAPI_TONE_DTMF_GET, &nDigit);
   \endcode   */
#define  IFX_TAPI_TONE_DTMF_GET             _IOR (IFX_TAPI_IOC_MAGIC, 0x97, int)

/** This service reads the ASCII character representing the DTMF digit out of
   the receive buffer. After reading the digit is removed from the
   receive buffer.
   \param int* The parameter points to the detected digit in ASCII
   representation.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int32 nDigit, ret;
   //  get DTMF digit
   ret = ioctl(fd, IFX_TAPI_TONE_DTMF_ASCII_GET, &nDigit);
   \endcode   */
#define  IFX_TAPI_TONE_DTMF_ASCII_GET        _IOR (IFX_TAPI_IOC_MAGIC, 0x98,int)

/** This service controls the DTMF sending mode. The DTMF may be sent
   out-of-band (OOB) or in-band.
   \param int The parameter has the following values:

   - 0: IFX_TAPI_PKT_EV_OOB_ALL, DTMF is send in-band and out-of-band
   - 1: IFX_TAPI_PKT_EV_OOB_ONLY , DTMF is send only out-of band
   - 2: IFX_TAPI_PKT_EV_OOB_NO, DTMF is send only in-band

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The DTMF out of band controls how events are reported. If switched
   on (IFX_TAPI_PKT_EV_OOB_ONLY  or  IFX_TAPI_PKT_EV_OOB_ALL) events are reported
   according to RFC 2833.
   If switched off (IFX_TAPI_PKT_EV_OOB_NO) the events are coded as voice.

   \code
   //  set the DTMF sending mode to out of band
   ioctl(fd, IFX_TAPI_PKT_EV_OOB_SET, IFX_TAPI_PKT_EV_OOB_ONLY );
   \endcode   */
#define  IFX_TAPI_PKT_EV_OOB_SET             _IOW (IFX_TAPI_IOC_MAGIC, 0x99,int)

/*@}*/ /* TAPI_INTERFACE_DIAL */

/* ======================================================================= */
/* TAPI Signal Services, ioctl commands (Group TAPI_INTERFACE_SIGNAL)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/** Enables the signal detection for Fax or modem signals.

   \param IFX_TAPI_SIG_DETECTION_t* The parameter points to a
   \ref IFX_TAPI_SIG_DETECTION_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks For firmware which provides ATD/UTD please
   check the capability list which detection service is
    available and how many detectors are available.
   Signal events are reported via exception. The information is
   queried with \ref IFX_TAPI_CH_STATUS_GET.
   In case of AM detected the driver may automatically switches to
   modem coefficients after the ending of CED
   No every combination is possible. It depends on the under lying
   device and firmware.
   For firmware providing MFTD there are no limitations in the
   combination of signals that can be detected. When reprogramming
   the detectors the driver has to stop the detector for a short
   moment.

   \code
   IFX_TAPI_SIG_DETECTION_t detect;
   IFX_TAPI_CH_STATUS_t param;
   memset (&param, 0, sizeof(IFX_TAPI_CH_STATUS_t));
   memset (&detect, 0, sizeof(IFX_TAPI_SIG_DETECTION_t));
   detect.sig = IFX_TAPI_SIG_CEDTX | IFX_TAPI_SIG_PHASEREVRX;
   ioctl (fd, IFX_TAPI_SIG_DETECT_ENABLE, &detect);

   ioctl (fd, IFX_TAPI_CH_STATUS_GET, &param);
   if (param.signal & IFX_TAPI_SIG_CED)
    print ("got CED from receive\n");
   \endcode  */
#define  IFX_TAPI_SIG_DETECT_ENABLE          _IOW(IFX_TAPI_IOC_MAGIC, 0x40, int)

/** Disables the signal detection for Fax or modem signals.

   \param IFX_TAPI_SIG_DETECTION_t* The parameter points to a
   \ref IFX_TAPI_SIG_DETECTION_t structure.

   \return 0 if successful, otherwise -1 in case of an error */
#define  IFX_TAPI_SIG_DETECT_DISABLE         _IOW(IFX_TAPI_IOC_MAGIC, 0x41, int)

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ======================================================================= */
/* TAPI Caller ID Services, ioctl commands  (Group TAPI_INTERFACE_CID)     */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** Setup the CID Receiver to start receiving CID Data.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   This command must be sent so that the driver can start collecting CID Data.
   Only type 1 CID FSK is supported.

   \code
   ret = ioctl(fd, IFX_TAPI_CID_RX_START, 0);
   \endcode   */
#define  IFX_TAPI_CID_RX_START                     _IO(IFX_TAPI_IOC_MAGIC, 0x3A)

/** Stop the CID Receiver

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFX_TAPI_CID_RX_STOP, 0);
   \endcode   */
#define  IFX_TAPI_CID_RX_STOP                      _IO(IFX_TAPI_IOC_MAGIC, 0x3B)

/** Retrieves the current status information of the CID Receiver.

   \param IFX_TAPI_CID_RX_STATUS_t* The parameter points to a
   \ref IFX_TAPI_CID_RX_STATUS_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Once the CID receiver is activated it signals the status with an exception.
   The exception is raised if data has been received completly or an error
   occured. Afterwards this interface is used to determine if data has been
   received. In that case it can be read with the interface
   \ref IFX_TAPI_CID_RX_DATA_GET .
   This interface can also be used without exception to determine the status
   of the receiver while reception.

   \code
   IFX_TAPI_CID_RX_STATUS_t cidStatus;
   memset (&cidStatus, 0, sizeof (IFX_TAPI_CID_RX_STATUS_t));
   ret = ioctl(fd, IFX_TAPI_CID_RX_STATUS_GET, (int)&cidStatus);
   // check if cid information are available for reading
   if ((ret == 0) && (cidStatus.nError == 0) && cidStatus.nStatus == 0x3))
   {
      printf ("Data is ready for reading\n\r");
   }
   \endcode   */
#define  IFX_TAPI_CID_RX_STATUS_GET          _IOR(IFX_TAPI_IOC_MAGIC, 0x3C, int)

/** Reads CID Data collected since caller id receiver was started.

   \param IFX_TAPI_CID_RX_DATA_t* The parameter points to a
   \ref IFX_TAPI_CID_RX_DATA_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This request can be sent after the CID Receiver status
   signaled that data is ready for reading or that data collection
   is ongoing. In the last case, the number of data read correspond to the
   number of data received since cid receiver was started. Once the data
   is read after the status signaled that data is ready, new data can be read
   only if CID Receiver detects new data.

   \code
   IFX_TAPI_CID_RX_STATUS_t cidStatus;
   IFX_TAPI_CID_RX_DATA_t cidData;
   memset (&cidStatus, 0, sizeof (IFX_TAPI_CID_RX_STATUS_t));
   memset (&cidData, 0, sizeof (IFX_TAPI_CID_RX_DATA_t));
   // read actual status
   ret = ioctl(fd, IFX_TAPI_CID_RX_STATUS_GET, (int)&cidStatus);
   // check if cid data are available for reading
   if ((ret == 0) && (cidStatus.nError == 0) && cidStatus.nStatus == 0x03))
      ret = ioctl(fd, IFX_TAPI_CID_RX_DATA_GET, (int)&cidData);
   \endcode   */
#define  IFX_TAPI_CID_RX_DATA_GET            _IOR(IFX_TAPI_IOC_MAGIC, 0x3D, int)

/**
   This interfaces transmits CID message. The message data and type information
   are given by IFX_TAPI_CID_MSG_t. Only the pure sending of FSK or DTMF data is
   done, no alert or ringing.

   \param  IFX_TAPI_CID_MSG_t*  Pointer to a \ref IFX_TAPI_CID_MSG_t,
   containing the CID / MWI information to be transmitted.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   It is recommended to use \ref IFX_TAPI_CID_CFG_SET once after boot to
   configure country specifig settings. Otherwise the default settings
   wil be used.

   \code
   IFX_TAPI_CID_MSG_t cid_info;
   IFX_TAPI_CID_MSG_ELEMENT_t message[3];
   // reset and fill the caller id structure
   memset (&cid_info, 0, sizeof(cid_info));
   memset (&message, 0, sizeof(message));
   cid_info.txMode = IFX_TAPI_CID_HM_ONHOOK;
   cid_info.messageType = IFX_TAPI_CID_MT_MWI;  // Message Waiting
   cid_info.nMsgElements = 3;
   cid_info.pMessage = &message[0];
   // Mandatory for Message Waiting: set Visual Indicator on
   message[0].value.elementType = IFX_TAPI_CID_ST_VISINDIC;
   message[0].value.data = IFX_TAPI_CID_VMWI_EN;
   // Add optional CLI (number) element
   message[1].string.elementType = IFX_TAPI_CID_ST_CLI;
   message[1].string.len = ret;
   strncpy(message[1].string.data, number, sizeof(message[1].string.data));
   // Add Reason for absence of name element
   message[2].value.elementType = IFX_TAPI_CID_ST_ABSNAME;
   message[2].value.element = IFX_TAPI_CID_ABSREASON_PRIV;
   // transmit the caller id
   ioctl(fd, IFX_TAPI_CID_TX_INFO_START, &cid_info);
   \endcode   */
#define  IFX_TAPI_CID_TX_INFO_START         _IOW (IFX_TAPI_IOC_MAGIC, 0xB1, int)

/**
   This interfaces transmits CID message with handling of the complete sequence
   (alert, ack and ringing). The message data and type information are given by
   IFX_TAPI_CID_MSG_t.

   \param  IFX_TAPI_CID_MSG_t*  Pointer to a \ref IFX_TAPI_CID_MSG_t,
   containing the CID / MWI information to be transmitted.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   It is recommended to use \ref IFX_TAPI_CID_CFG_SET once after boot to
   configure country specifig settings. Otherwise the default settings wil
   be used.

   \code
   IFX_TAPI_CID_MSG_t cid_info;
   IFX_TAPI_CID_MSG_ELEMENT_t message[3];
   // reset and fill the caller id structure
   memset (&cid_info, 0, sizeof(cid_info));
   memset (&message, 0, sizeof(message));
   cid_info.txMode = IFX_TAPI_CID_HM_ONHOOK;
   cid_info.messageType = IFX_TAPI_CID_MT_MWI;  // Message Waiting
   cid_info.nMsgElements = 3;
   cid_info.pMessage = &message[0];
   // Mandatory for Message Waiting: set Visual Indicator on
   message[0].value.elementType = IFX_TAPI_CID_ST_VISINDIC;
   message[0].value.data = IFX_TAPI_CID_VMWI_EN;
   // Add optional CLI (number) element
   message[1].string.elementType = IFX_TAPI_CID_ST_CLI;
   message[1].string.len = ret;
   strncpy(message[1].string.data, number, sizeof(message[1].string.data));
   // Add Reason for absence of name element
   message[2].value.elementType = IFX_TAPI_CID_ST_ABSNAME;
   message[2].value.element = IFX_TAPI_CID_ABSREASON_PRIV;
   // transmit the caller id
   ioctl(fd, IFX_TAPI_CID_TX_SEQ_START, &cid_info);
   \endcode
   \code
   IFX_TAPI_CID_MSG_ELEMENT_t message;
   IFX_TAPI_CID_MSG_t cid_info;
   // FSK string to present the number "08923403330"
   IFX_uint8_t data [16] = {0x80, 0x0D, 0x02, 0x0B, 0x30, 0x38, 0x39, 0x32,
                         0x33, 0x34, 0x30, 0x33, 0x33, 0x33, 0x30, 0x33};
   // setup message
   message.transparent.elementType = IFX_TAPI_CID_ST_TRANSPARENT;
   message.transparent.len  = sizeof (data);
   message.transparent.data = data;
   // setup cid info
   cid_info.txMode = IFX_TAPI_CID_HM_ONHOOK;
   cid_info.messageType = IFX_TAPI_CID_MT_CSUP;
   cid_info.nMsgElements = 1;
   cid_info.message = &message;
   // sent sequence
   ioctl (fdch, IFX_TAPI_CID_TX_SEQ_START, &cid_info);
   \endcode

   */
#define  IFX_TAPI_CID_TX_SEQ_START          _IOW (IFX_TAPI_IOC_MAGIC, 0xB2, int)

/**
   This service configures country specific settings: selection of CID standard
   to be used and tuning of some parameters, such us timing, FSK and DTMF
   configuration.

   \param  IFX_TAPI_CID_CFG_t*  Pointer to a \ref IFX_TAPI_CID_CFG_t,
   containing CID / MWI configuration information.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   In case selected CID standard uses FSK, seizure and mark size for the FSK
   engine are set at runtime by calling \ref IFX_TAPI_CID_TX_INFO_START or \ref
   IFX_TAPI_CID_TX_SEQ_START. In fact, those parameters depend upon
   transmission type (on-hook or off-hook).

   \code
   IFX_TAPI_CID_CFG_t cid_conf;
   // reset and fill the config structure
   memset (&cid_conf, 0, sizeof(cid_conf));
   cid_conf.nStandard = IFX_TAPI_CID_STD_TELCORDIA;
   cid_conf.cfg.telcordia.nAlertToneOffhook = 26; // FIXME
   cid_conf.cfg.telcordia.ackTone = 'D';
   // set the configuration
   ioctl(fd, IFX_TAPI_CID_CFG_SET, &cid_conf);
   \endcode   */
#define  IFX_TAPI_CID_CFG_SET               _IOW (IFX_TAPI_IOC_MAGIC, 0xB0, int)

/*@}*/ /* TAPI_INTERFACE_CID */

/* ======================================================================= */
/* TAPI Connection Services, ioctl commands (Group TAPI_INTERFACE_CON)     */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** This interface configures RTP and RTCP fields for a new connection.

   \param IFX_TAPI_PKT_RTP_CFG_t* The parameter points to a
   \ref IFX_TAPI_PKT_RTP_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_PKT_RTP_CFG_t param;
   memset (&param, 0, sizeof(IFX_TAPI_PKT_RTP_CFG_t));
   param.nSeqNr = 0x1234;
   ret = ioctl(fd, IFX_TAPI_PKT_RTP_CFG_SET, &param);
   \endcode   */
#define  IFX_TAPI_PKT_RTP_CFG_SET            _IOW(IFX_TAPI_IOC_MAGIC, 0x13, int)

/** Change the payload type table

   \param IFX_TAPI_PKT_RTP_PT_CFG_t* The parameter points to a
   \ref IFX_TAPI_PKT_RTP_PT_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The requested payload type should have been negotiated with the peer prior
   to the connection set-up. Event payload types are negotiated during
   signaling phase and the driver and the device can be programmed accordingly
   at the time of starting the media session. Event payload types shall not be
   modified when the session is in progress.

   \code
   IFX_TAPI_PKT_RTP_PT_CFG_t param;
   memset (&param, 0, sizeof(TAPI_RTP_PTCONF));
   param.nPTup[IFX_TAPI_ENC_TYPE_G728] = 0x5;
   param.nPTup[IFX_TAPI_ENC_TYPE_G726_16] = 0x4;
   param.nPTdown[IFX_TAPI_ENC_TYPE_G728] = 0x7;
   param.nPTdown[IFX_TAPI_ENC_TYPE_G726_16] = 0x6;
   ret = ioctl(fd, IFX_TAPI_PKT_RTP_PT_CFG_SET, &param);
   \endcode   */
#define  IFX_TAPI_PKT_RTP_PT_CFG_SET         _IOW(IFX_TAPI_IOC_MAGIC, 0x14, int)

/** Retrieves RTCP statistics

   \param IFX_TAPI_PKT_RTCP_STATISTICS_t* Pointer to a
          \ref IFX_TAPI_PKT_RTCP_STATISTICS_t structure according
                RFC 3550/3551 for a sender report.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The structure refers to the RFC 3550 / 3551. Not all fields may
   be supported by the TAPI, but must be filled by the calling application.

   \code
   IFX_TAPI_PKT_RTCP_STATISTICS_t param;
   ret = ioctl(fd, IFX_TAPI_PKT_RTCP_STATISTICS_GET, (int)&param);
   \endcode   */
#define  IFX_TAPI_PKT_RTCP_STATISTICS_GET    _IOR(IFX_TAPI_IOC_MAGIC, 0x15, int)

/** Resets the RTCP statistics.
   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFX_TAPI_PKT_RTCP_STATISTICS_RESET, 0);
   \endcode   */
#define  IFX_TAPI_PKT_RTCP_STATISTICS_RESET  _IOW(IFX_TAPI_IOC_MAGIC, 0x16, int)

/** Configures the jitter buffer
   \param IFX_TAPI_JB_CFG_t* The parameter points to a
   \ref IFX_TAPI_JB_CFG_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Select fixed or adaptive jitter buffer and set
   parameters. Modifies the JB settings during run-time.

   \code
   IFX_TAPI_JB_CFG_t param;
   memset (&param, 0, sizeof(IFX_TAPI_JB_CFG_t));
   // set to adaptive jitter buffer type
   param.nJbType = 2;
   ret = ioctl(fd, IFX_TAPI_JB_CFG_SET, param);
   \endcode */
#define  IFX_TAPI_JB_CFG_SET                 _IOW(IFX_TAPI_IOC_MAGIC, 0x17, int)

/** Reads out jitter buffer statistics.
   \param IFX_TAPI_JB_STATISTICS_t* The parameter points to a
   \ref IFX_TAPI_JB_STATISTICS_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_JB_STATISTICS_t param;
   memset (&param, 0, sizeof(IFX_TAPI_JB_STATISTICS_t));
   ret = ioctl(fd, IFX_TAPI_JB_STATISTICS_GET, param);
   ...
   \endcode */
#define  IFX_TAPI_JB_STATISTICS_GET          _IOR(IFX_TAPI_IOC_MAGIC, 0x18, int)

/** Resets the jitter buffer statistics.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ret = ioctl(fd, IFX_TAPI_JB_STATISTICS_RESET, 0);
   \endcode   */
#define  IFX_TAPI_JB_STATISTICS_RESET        _IOW(IFX_TAPI_IOC_MAGIC, 0x19, int)

/** This interface configures AAL fields for a new connection.

   \param IFX_TAPI_PCK_AAL_CFG_t* The parameter points to a
   \ref IFX_TAPI_PCK_AAL_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   TAPI_AAL_CONFIG param;
   memset (&param, 0, sizeof(TAPI_AAL_CONFIG));
   param.nTimestamp = 0x1234;
   ret = ioctl(fd, IFX_TAPI_PKT_AAL_CFG_SET, &param)
   \endcode   */
#define  IFX_TAPI_PKT_AAL_CFG_SET            _IOW(IFX_TAPI_IOC_MAGIC, 0x1E, int)


/** This interface adds the phone channel to another analog phone channel.
   To set up a conference with device internal phone this interfaces is used.

   \param IFX_TAPI_MAP_PHONE_t* The parameter points to a
   \ref IFX_TAPI_MAP_PHONE_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Applies to a phone channel file descriptor.
   The host has to take care about the resource management.

   \code
   IFX_TAPI_MAP_PHONE_t param;
   memset (&param, 0, sizeof(IFX_TAPI_MAP_PHONE_t));
   param.nPhoneCh = 1;
   ioctl (fd0, IFX_TAPI_MAP_PHONE_ADD, &param);
   \endcode
*/
#define  IFX_TAPI_MAP_PHONE_ADD              _IOW(IFX_TAPI_IOC_MAGIC, 0x2A, int)

/** This interface removes a phone channel from an analog phone device.

   \param IFX_TAPI_MAP_PHONE_t* The parameter points to a
   \ref IFX_TAPI_MAP_PHONE_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Applies to a phone channel file descriptor.

   \code
   IFX_TAPI_MAP_PHONE_t param;
   memset (&param, 0, sizeof(IFX_TAPI_MAP_PHONE_t));
   param.nPhoneCh = 1;
   ioctl (fd0, IFX_TAPI_MAP_PHONE_REMOVE, &param);
   \endcode
*/
#define  IFX_TAPI_MAP_PHONE_REMOVE           _IOW(IFX_TAPI_IOC_MAGIC, 0x2B, int)

/** AAL profile configuration.

   \param IFX_TAPI_PCK_AAL_PROFILE_t* The parameter points to a
   \ref IFX_TAPI_PCK_AAL_PROFILE_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
    Sets each row in a profile. A maximum number of 10 rows can be set up.
    This interface is called on initialization to program the selected AAL
    profile.
    The following example programms the ATMF profiles 3, 4 and 5

   \code
   IFX_TAPI_PCK_AAL_PROFILE_t Profile;
   switch (nProfile)
   {
      case  3:
      case  4:
      // ATMF profile 3 and 4
      Profile.rows = 1;
      Profile.codec[0] = IFX_TAPI_ENC_TYPE_MLAW;
      Profile.len[0] = 43;
      Profile.nUUI[0] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_15;
      break;
      case  5:
      // ATMF profile 5
      Profile.rows = 2;
      Profile.codec[0] = IFX_TAPI_ENC_TYPE_MLAW;
      Profile.len[0] = 43;
      Profile.nUUI[0] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7;
      Profile.codec[1] = IFX_TAPI_ENC_TYPE_G726_32;
      Profile.len[1] = 43;
      Profile.nUUI[1] = IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15;
      break;
   }
   ret = ioctl (fd, IFX_TAPI_PKT_AAL_PROFILE_SET, (INT)&Profile);
   \endcode */
#define  IFX_TAPI_PKT_AAL_PROFILE_SET        _IOW(IFX_TAPI_IOC_MAGIC, 0x23, int)

/** This interface adds the data channel to an analog phone device.
   To set up a conference this interfaces is used on more data channels.

   \param IFX_TAPI_MAP_DATA_t* The parameter points to a
   \ref IFX_TAPI_MAP_DATA_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Applies to a data file descriptor.The host has to take care about
   the resource management. It has to count the data channel (codec) resource
   usage and maintains a list, which data channel is mapped to which phone
   channel. The available resources can be queried by
   \ref IFX_TAPI_CAP_CHECK.

   \code
   IFX_TAPI_MAP_DATA_t datamap;
   memset (&datamap, 0, sizeof(IFX_TAPI_MAP_DATA_t));

   // add data 0 to phone 0
   ioctl (fd0, IFX_TAPI_MAP_DATA_ADD, &datamap);
   datamap.nDstCh = 1;
   // add data 0 to phone 1
   ioctl (fd0, IFX_TAPI_MAP_DATA_ADD, &datamap);
   \endcode   */
#define  IFX_TAPI_MAP_DATA_ADD               _IOW(IFX_TAPI_IOC_MAGIC, 0x24, int)

/** This interface removes a data channel from an analog phone device.

   \param IFX_TAPI_MAP_DATA_t* The parameter points to a
   \ref IFX_TAPI_MAP_DATA_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks Applies to a data file descriptor.The host has to take care about
   the resource management. It has to count the data channel (codec) resource
   usage and maintains a list, which data channel is mapped to which phone
   channel. The available resources can be queried by
   \ref IFX_TAPI_CAP_CHECK.

   \code
   IFX_TAPI_MAP_DATA_t datamap;
   memset (&datamap, 0, sizeof(IFX_TAPI_MAP_DATA_t));

   // add data 0 to phone 0
   ioctl (fd0, IFX_TAPI_MAP_DATA_ADD, &datamap);
   datamap.nDstCh = 1;
   // add data 0 to phone 1
   ioctl (fd0, IFX_TAPI_MAP_DATA_ADD, &datamap);
   \endcode   */
#define  IFX_TAPI_MAP_DATA_REMOVE            _IOW(IFX_TAPI_IOC_MAGIC, 0x25, int)

/** This interface adds the PCM channel to an analog phone device.
   To set up a conference this interfaces is used on more PCM channels.

   \param IFX_TAPI_MAP_PCM_t* The parameter points to a
   \ref IFX_TAPI_MAP_PCM_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_MAP_PCM_t param;
   memset (&param, 0, sizeof(IFX_TAPI_MAP_PCM_t));
   // PCM channel 1 is removed from the phone channel 2
   param.nDstCh = 2;
   // fd1 represents PCM channel 1
   ret = ioctl(fd1, IFX_TAPI_MAP_PCM_REMOVE, &param)
   \endcode   */
#define  IFX_TAPI_MAP_PCM_ADD                _IOW(IFX_TAPI_IOC_MAGIC, 0x43, int)

/** This interface removes the PCM channel to an analog phone device.
   To set up a conference this interfaces is used on more PCM channels.

   \param IFX_TAPI_MAP_PCM_t* The parameter points to a
   \ref IFX_TAPI_MAP_PCM_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_MAP_PCM_t param;
   memset (&param, 0, sizeof(IFX_TAPI_MAP_PCM_t));
   // PCM channel 1 is removed from the phone channel 2
   param.nDstCh = 2;
   // fd1 represents PCM channel 1
   ret = ioctl(fd1, IFX_TAPI_MAP_PCM_REMOVE, &param)
   \endcode   */
#define  IFX_TAPI_MAP_PCM_REMOVE             _IOW(IFX_TAPI_IOC_MAGIC, 0x44, int)

/** This service gets the frame length for the audio packets

   \param int* Pointer to the length of frames in milliseconds

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nFrameLength;
   ioctl(fd, IFX_TAPI_ENC_FRAME_LEN_GET, &nFrameLength);
   \endcode
*/
#define  IFX_TAPI_ENC_FRAME_LEN_GET                _IO(IFX_TAPI_IOC_MAGIC, 0x2D)

/** Select a codec for the data channel

   \param IFX_TAPI_ENC_TYPE_t The parameter specifies the codec as defined
   in \ref IFX_TAPI_ENC_TYPE_t. Default codec is G711, u-Law

   \return error code
      - 0 if successful,
      - -1 in case of an error

   \code
   // set the codec
   ioctl(fd, IFX_TAPI_ENC_TYPE_SET, IFX_TAPI_ENC_TYPE_G726_16);
   \endcode  */
#define  IFX_TAPI_ENC_TYPE_SET               _IOW (IFX_TAPI_IOC_MAGIC, 0x89,int)

/** Start recording on the data channel. Enables the encoder.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The voice is serviced with the drivers read and write interface.
   The data format is dependent on the selected setup (coder, chip setup,
   etc.). For example, a RTP setup will receive RTP packets. Read and
   write are always non-blocking.
   If the codec has not been set before with \ref IFX_TAPI_ENC_TYPE_SET the
   encoder is started with G711

   \code
   ioctl(fd, IFX_TAPI_ENC_START, 0);
   \endcode */
#define  IFX_TAPI_ENC_START                       _IO (IFX_TAPI_IOC_MAGIC, 0x8A)

/** Start recording (generating packets) on this channel

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, IFX_TAPI_ENC_STOP, 0);
   \endcode   */
#define  IFX_TAPI_ENC_STOP                        _IO (IFX_TAPI_IOC_MAGIC, 0x8B)

/** This service sets the frame length for the audio packets

   \param int Length of frames in milliseconds

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nFrameLength = 10;
   ioctl(fd, IFX_TAPI_ENC_FRAME_LEN_SET, nFrameLength);
   \endcode   */
#define  IFX_TAPI_ENC_FRAME_LEN_SET         _IOW (IFX_TAPI_IOC_MAGIC, 0x8D, int)

/** Sets the recording volume

   \param int Recording volume, default 0x100

   \return 0 if successful, otherwise -1 in case of an error

   \note Not supported

   \code
   ioctl(fd, IFX_TAPI_ENC_VOLUME_SET, 0x100 * 2);
   \endcode   */
#define  IFX_TAPI_ENC_VOLUME_SET            _IOW (IFX_TAPI_IOC_MAGIC, 0x8E, int)

/** This service returns the level of the most recently recorded signal.
   Value 0x100 describes the default record level of -38.79 dB. Maximum
   value is 0x7FFF which corresponds to a record level of approximately 3 dB.

   \param int* Pointer to an integer for the level

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not implemented!

   \code
   int nRecLevel;
   //  get the record level
   ret = ioctl(fd, IFX_TAPI_ENC_LEVEL_SET, &nRecLevel);
   // output level in dB can be calculated via formula:
   // outlvl_in_dB = +3.17 + 20 log10 (nRecLevel/32767)
   \endcode   */
#define  IFX_TAPI_ENC_LEVEL_SET                   _IO (IFX_TAPI_IOC_MAGIC, 0x8F)

/** Set the AGC coefficient for a coder module
    This implementation assumes that an index of a AGC resource
    is fix assigned to the related index of the coder module.

   \param IFX_TAPI_ENC_AGC_CFG_t* The parameter points to a
   \ref IFX_TAPI_ENC_AGC_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

    \code
   IFX_TAPI_ENC_AGC_CFG_t param;
   memset (&param, 0, sizeof(IFX_TAPI_ENC_AGC_CFG_t));
   param.COM = 2;
   param.GAIN = 2;
   ret = ioctl(fd, IFX_TAPI_ENC_AGC_CFG, &param)
   \endcode
*/
#define  IFX_TAPI_ENC_AGC_CFG                _IOW(IFX_TAPI_IOC_MAGIC, 0x96, int)

/** Enable / Disable the AGC resource
    This implementation assumes that the AGC resouces inside a VINETIC are
    fixed assigned to the related Coder-Module

   \param IFX_TAPI_ENC_AGC_MODE_t The following values
    (see \ref IFX_TAPI_ENC_AGC_MODE_t enum) are valid:

      - 0x0: IFX_TAPI_ENC_AGC_MODE_DISABLE, Disable AGC
      - 0x1: IFX_TAPI_ENC_AGC_MODE_ENABLE, Enable AGC

   \return 0 if successful, otherwise -1 in case of an error
*/
#define  IFX_TAPI_ENC_AGC_ENABLE             _IOW(IFX_TAPI_IOC_MAGIC, 0x97, int)


/** Select a codec for the data channel playout

   \param \ref IFX_TAPI_ENC_TYPE_t The parameter specifies the codec,
          default G711

   \return error code
      - 0 if successful,
      - -1 in case of an error
   \remarks This interface is currently not supported, because the codec
   is determined by the payload type of the received packets */
#define  IFX_TAPI_DEC_TYPE_SET              _IOW (IFX_TAPI_IOC_MAGIC, 0x90, int)

/** Starts the playout of data

   \note Currently not implemented!

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, IFX_TAPI_DEC_START, 0);
   \endcode   */
#define  IFX_TAPI_DEC_START                       _IO (IFX_TAPI_IOC_MAGIC, 0x91)

/** Stops the playout of data

   \note Currently not implemented!

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, IFX_TAPI_DEC_STOP, 0);
   \endcode   */
#define  IFX_TAPI_DEC_STOP                        _IO (IFX_TAPI_IOC_MAGIC, 0x92)

/** Sets the playout volume

   \param int Playout volume, default 0x100

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not supported

   \code
   ioctl(fd, IFX_TAPI_DEC_VOLUME_SET, 0x100 * 2);
   \endcode   */
#define  IFX_TAPI_DEC_VOLUME_SET            _IOW (IFX_TAPI_IOC_MAGIC, 0x94, int)

/** This service returns the level of the most recently played signal.
   Value 0x100 describes the default record level of -38.79 dB. Maximum
   value is 0x7FFF which corresponds to a record level of approximately 3 dB.

   \param int* Pointer to an integer for the level

   \return 0 if successful, otherwise -1 in case of an error

   \note Currently not implemented!

   \code
   int nLevel;
   //  get the record level
   ret = ioctl(fd, IFX_TAPI_DEC_LEVEL_GET, &nLevel);
   // output level in dB can be calculated via formula:
   // outlvl_in_dB = +3.17 + 20 log10 (nRecLevel/32767)
   \endcode   */
#define  IFX_TAPI_DEC_LEVEL_GET                   _IO (IFX_TAPI_IOC_MAGIC, 0x95)

/** Configures the voice activity detection and silence handling
   Voice Activity Detection (VAD) is a feature that allows the
   codec to determine when to send voice data or silence data.
   When the record level falls below a certain threshold, the audio frames are
   replaced by comfort noise frames. These comfort noise frames contain less
   significant data, which means that the application can use less network
   bandwidth when sending the data across the network. For other codecs,
   this must be done in software by analyzing the record level.

   \param IFX_TAPI_ENC_VAD_t The following values are valid:

   - 0: IFX_TAPI_ENC_VAD_NOVAD, voice activity detection
   - 1: IFX_TAPI_ENC_VAD_ON, voice activity detection switched on
   - 2: IFX_TAPI_ENC_VAD_G711, voice activity detection switched on and comfort
        noise generation activated, applies to G.711
   - 3: IFX_TAPI_ENC_VAD_CNG_ONLY, Voice activity detection on with comfort
        noise generation without silence compression
   - 4: IFX_TAPI_ENC_VAD_SC_ONLY, Voice activity detection on with silence
        compression without comfort noise generation

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   Codecs G.723.1 and G.729A/B have a built in comfort noise generation (CNG).
   Explicitly switching on the CNG is not necessary. Voice activity detection
   is not available for G.728 and G.729E.

   \code
   // set the VAD on
   ioctl(fd, IFX_TAPI_ENC_VAD_CFG_SET, IFX_TAPI_ENC_VAD_ON);
   \endcode
*/
#define  IFX_TAPI_ENC_VAD_CFG_SET           _IOW (IFX_TAPI_IOC_MAGIC, 0xA9, int)

/** \todo: uncatalogued ioctl command */
#define  IFXPHONE_PLAY_HOLD                  _IOW(IFX_TAPI_IOC_MAGIC, 0x26, int)
/** \todo: uncatalogued ioctl command */
#define  IFXPHONE_REC_HOLD                   _IOW(IFX_TAPI_IOC_MAGIC, 0x27, int)
/** \todo: uncatalogued ioctl command */
#define  PHONE_REC_DEPTH                    _IOW (IFX_TAPI_IOC_MAGIC, 0x8C, int)
/** \todo: uncatalogued ioctl command */
#define  PHONE_PLAY_DEPTH                   _IOW (IFX_TAPI_IOC_MAGIC, 0x93, int)
/** \todo: uncatalogued ioctl command */
#define  PHONE_REC_VOLUME_LINEAR            _IOW (IFX_TAPI_IOC_MAGIC, 0xDB, int)
/** \todo: uncatalogued ioctl command */
#define  PHONE_PLAY_VOLUME_LINEAR           _IOW (IFX_TAPI_IOC_MAGIC, 0xDC, int)

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, ioctl commands (Group TAPI_INTERFACE_MISC)  */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Retrieves the TAPI version string

   \param char* Pointer to version character string

   \return 0 if successful, otherwise -1 in case of an error

   \code
   char Version[80];
   int ret;

   ret = ioctl(fd, IFX_TAPI_VERSION_GET, (int)&Version);
   printf("\nVersion:%s\n", Version);
   \endcode

*/
#define  IFX_TAPI_VERSION_GET                      _IO(IFX_TAPI_IOC_MAGIC, 0x00)

/** This service masks the reporting of the exceptions.
   Set the mask to 1 disables the reporting of the corresponding event.

   \param int The parameter defines the exception event bits mask,
   defined in \ref IFX_TAPI_EXCEPTION_BITS_t

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If digit reporting is enabled, digits are also read out in
   \ref IFX_TAPI_CH_STATUS_GET

   \code
   // mask reporting of exceptions
   IFX_TAPI_EXCEPTION_t    exceptionMask;
   memset (&exceptionMask, 0, sizeof (exceptionMask));
   // mask dtmf ready event
   exceptionMask.Bits.dtmf_ready = 1;
   // mask hook state event
   exceptionMask.Bits.hookstate  = 1;
   ioctl(fd, IFX_TAPI_EXCEPTION_MASK, exceptionMask.Status);
   \endcode
*/
#define  IFX_TAPI_EXCEPTION_MASK                   _IO(IFX_TAPI_IOC_MAGIC, 0x10)

/** Set the report levels if the driver is compiled with
    ENABLE_TRACE

    \param int Valid arguments are

    - 0: IFX_TAPI_DEBUG_REPORT_SET_OFF, report is switched off
    - 1: IFX_TAPI_DEBUG_REPORT_SET_LOW, report is switched to high output
    - 2: IFX_TAPI_DEBUG_REPORT_SET_NORMAL, report is switched to general
         information and warnings
    - 3: IFX_TAPI_DEBUG_REPORT_SET_HIGH, report is switched to report
         only errors
*/
#define  IFX_TAPI_DEBUG_REPORT_SET                 _IO(IFX_TAPI_IOC_MAGIC, 0x12)

/** Query status information about the phone line.

   \param IFX_TAPI_CH_STATUS_t* The parameter points to a
   \ref IFX_TAPI_CH_STATUS_t array.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks If the exception bits reporting of digits is enabled
   (see \ref IFX_TAPI_EXCEPTION_MASK), the digit fifo is read out once and the
   information is passed to the status. It may occur that more than one
   digit is located in the fifo. Therefore the application must check with
   \ref IFX_TAPI_TONE_DTMF_READY_GET or \ref IFX_TAPI_PULSE_READY if further
   data is contained in the fifos.
   This interface replaces the exception bits.

   \code
   // This example shows the call of the status information of one channel
   #define TAPI_STATUS_DTMF   0x01
   #define TAPI_STATUS_PULSE  0x02
   IFX_TAPI_CH_STATUS_t status[1];
   int fd;
   fd = open ("/dev/vin11", O_RDWR);
   status[0].channels = 0;
   // get the status
   ioctl(fd, IFX_TAPI_CH_STATUS_GET, status);
   if (status[0].dialing & TAPI_STATUS_DTMF ||
       status[0].dialing & TAPI_STATUS_PULSE)
      printf ("Dial key %d detected\n", status.digit);
   \endcode

   \code
   // This example shows a select waiting on one fd for exceptions and on
   // two others for data. In one call the status information of all two
   // channels are queried from the driver.
   IFX_TAPI_CH_STATUS_t stat[4];
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
      ioctl(fdcfg, IFX_TAPI_CH_STATUS_GET, stat);
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
   IFX_TAPI_CH_STATUS_t stat[4];
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
         ioctl(fdcfg, IFX_TAPI_CH_STATUS_GET, stat);
         for (i = 0; i < MAX_CHANNELS, i++)
         {
            if (status[i].line & IFX_TAPI_LINE_STATUS_GR909RES)
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
*/
#define  IFX_TAPI_CH_STATUS_GET                   _IO (IFX_TAPI_IOC_MAGIC, 0x2C)

/** Check the supported TAPI interface version

   \param IFX_TAPI_VERSION_t* The parameter points to a
   \ref IFX_TAPI_VERSION_t structure.

   \remarks
   Since an application is always build against one specific TAPI interface
   version it should check if it is supported. If not the application should
   abort. This interface checks if the current TAPI version supports a
   particular version. For example the TAPI versions 2.1 will support TAPI 2.0.
   But version 3.0 might not support 2.0.

   \code
   IFX_TAPI_VERSION_t version;
   int ret;

   version.major = 2;
   version.minor = 1;
   if (ioctl(fd, IFX_TAPI_VERSION_CHECK, (int)&version) == 0)
      printf("\nVersion 2.1 supported\n");
   \endcode

*/
#define  IFX_TAPI_VERSION_CHECK              _IOW(IFX_TAPI_IOC_MAGIC, 0x39, int)

/** This service returns the number of capabilities

   \param int* Pointer to the number of capabilities which are returned

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // get the cap list size
   int ret, nCapNr;
   ret = ioctl(fd, IFX_TAPI_CAP_NR, &nCapNr);
   \endcode   */
#define  IFX_TAPI_CAP_NR                          _IO (IFX_TAPI_IOC_MAGIC, 0x80)

/** This service returns the capability lists.

   \param IFX_TAPI_CAP_t* Pointer to the capability list

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_CAP_t *pCapList;

   // get the cap list size
   ioctl(fd, IFX_TAPI_CAP_NR, &nCapNr);
   pCapList = malloc (nCapNr * sizeof(IFX_TAPI_CAP_t));

   // get the cap list
   ioctl(fd, IFX_TAPI_CAP_LIST, pCapList);
   for (i=0; i < nCapSize; i++)
   {
      switch (pCapList[i].captype)
      {
           case IFX_TAPI_CAP_TYPE_CODEC:
               printf ("Codec: %s\n\r", pCapList[i].desc);
               break;
           case IFX_TAPI_CAP_TYPE_PCM:
              printf ("PCM: %d\n\r", pCapList[i].cap);
              break;
           case IFX_TAPI_CAP_TYPE_CODECS:
              printf ("CODER: %d\n\r", pCapList[i].cap);
              break;
           case IFX_TAPI_CAP_TYPE_PHONES:
              printf ("PHONES: %d\n\r", pCapList[i].cap);
              break;
           default:
               break;
      }
   }
   // free the allocated memory
   free(pCapList);
   \endcode   */
#define  IFX_TAPI_CAP_LIST                        _IO (IFX_TAPI_IOC_MAGIC, 0x81)

/** This service checks if a specific capability is supported

   \param IFX_TAPI_CAP_t* The parameter points to a
   \ref IFX_TAPI_CAP_t structure

   \return
      - 0:  If not supported
      - 1:  If supported
      - -1: In case of an error

   \code
   IFX_TAPI_CAP_t CapList;

   memset (&CapList, 0, sizeof(IFX_TAPI_CAP_t));
   // check if G726, 16 kBit/s is supported
   CapList.captype = codec;
   CapList.cap = IFX_TAPI_ENC_TYPE_G726_16;
   ret = ioctl(fd, IFX_TAPI_CAP_CHECK, &CapList);
   if (ret > 0)
      printf("IFX_TAPI_ENC_TYPE_G726_16 supported\n");
   // check how many data channels are supported
   CapList.captype = codecs;
   ret = ioctl(fd, IFX_TAPI_CAP_CHECK, &CapList);
   if (ret > 0)
      printf("%d data channels supported\n", CapList.cap);
   // check if POTS port is available
   CapList.captype = IFX_TAPI_CAP_TYPE_PORT;
   CapList.cap = IFX_TAPI_CAP_PORT_POTS;
   ret = ioctl(fd, IFX_TAPI_CAP_CHECK, &CapList);
   if (ret > 0)
      printf("POTS port supported\n");
   \endcode   */
#define  IFX_TAPI_CAP_CHECK                       _IO (IFX_TAPI_IOC_MAGIC, 0x82)

/**
   This service returns the current exception status.

   \param IFX_TAPI_EXCEPTION_t* The parameter points to a
   \ref IFX_TAPI_EXCEPTION_t structure.

   \remarks
   The user application must poll cidrx_supdate, once the CID Receiver was
   started with IFX_TAPI_CID_RX_START. If it is set the application
   calls IFX_TAPI_CID_RX_STATUS_GET in order to know if CID data is
   available for reading or if an error occured during CID Reception.

   \code
   IFX_TAPI_EXCEPTION_t nException;
   //  get the exception
   nException.Status = ioctl(fd, IFX_TAPI_EXCEPTION_GET, 0);
   if (nException.Bits.dtmf_ready)
      printf ("DTMF detected\n");
   \endcode
*/
#define  IFX_TAPI_EXCEPTION_GET             _IOR (IFX_TAPI_IOC_MAGIC, 0x9A, int)

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ===================================================================== */
/* TAPI Ringing Services, ioctl commands (Group TAPI_INTERFACE_RINGING)  */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/


/** This service sets the ring configuration for the non-blocking
    ringing services.
   This service is non-blocking.

   \param IFX_TAPI_RING_CFG_t* The parameter points to a
          \ref IFX_TAPI_RING_CFG_t structure

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The configuration has to be set before ringing starts using the
    interface \ref IFX_TAPI_RING_START

   \code
     IFX_TAPI_RING_CFG_t param;
     memset (&param, 0, sizeof(IFX_TAPI_RING_CFG_t));
     param.mode = 0;
     param.submode = 1;
     ret = ioctl(fd, IFX_TAPI_RING_CFG_SET, &param)
   \endcode
*/
#define  IFX_TAPI_RING_CFG_SET                     _IO(IFX_TAPI_IOC_MAGIC, 0x02)

/**
   This service sets the high resolution ring cadence for the non-blocking
   ringing services. The cadence value has to be set before ringing starts
   (\ref IFX_TAPI_RING_START).This service is non-blocking

   \param IFX_TAPI_RING_CADENCE_t* The parameter points to a
   \ref IFX_TAPI_RING_CADENCE_t structure.

   \return 0 if successful, otherwise -1 in case of an error.
   The number of ring bursts can be counted via exception
   “PSTN line is ringing” of service \ref IFX_TAPI_EXCEPTION_GET.

   \code
   // pattern of 3 sec. ring and 1 sec. pause
   char data[10] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00 };
   // initial 1 sec. ring and 600 ms non ringing signal before ringing
   char initial[4] =0xFF,0xFF,0xF0,0x0};
   IFX_TAPI_RING_CADENCE_t Cadence;
   memset (&Cadence, 0, sizeof(IFX_TAPI_RING_CADENCE_t));
   memcpy (&Cadence.data, data, sizeof (data));
   Cadence.nr = sizeof(data) * 8;
   // set size in bits
   memcpy (&Cadence.initial, initial, sizeof (initial));
   Cadence.initialNr = 4 * 8;
   // set the cadence sequence
   ioctl(fd, IFX_TAPI_RING_CADENCE_HR_SET, &Cadence);
   \endcode
*/
#define  IFX_TAPI_RING_CADENCE_HR_SET              _IO(IFX_TAPI_IOC_MAGIC, 0x03)

/** This service gets the ring configuration for the non-blocking
    ringing services.

   \param IFX_TAPI_RING_CFG_t* The parameter points to a
          \ref IFX_TAPI_RING_CFG_t structure

   \return 0 if successful, otherwise -1 in case of an error

*/
#define  IFX_TAPI_RING_CFG_GET                     _IO(IFX_TAPI_IOC_MAGIC, 0x1F)

/** This service rings the phone. The maximum number of rings has to be set by
   \ref IFX_TAPI_RING_MAX_SET. The service will return after the maximum number
   of rings or if the phone answered (blocking call).

   \param int This interface expects no parameter. It should be set to 0.

   \return The execution status
      - 0 if number of rings reached
      - 1 if phone was hooked off
      - -1 in case of an error

   \code
   int nMaxRing = 3, ret;
   // set the maximum rings
   ioctl(fd, IFX_TAPI_RING_MAX_SET, nMaxRing);
   // ring the phone
   ret = ioctl(fd, IFX_TAPI_RING, 0);
   if (ret == 0)
   {
      // no answer, maximum number of rings reached
    }
    else if (ret == 1)
    {
      // phone hooked off
    }
   \endcode   */
#define  IFX_TAPI_RING                            _IO (IFX_TAPI_IOC_MAGIC, 0x83)

/** This service sets the maximum number of rings before a blocking ringing
   is started with service \ref IFX_TAPI_RING. This service itself is
   non-blocking.

   \param int The parameter defines the maximum ring number

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int nMaxRing = 3;
   ioctl(fd, IFX_TAPI_RING_MAX_SET, nMaxRing);
   \endcode   */
#define  IFX_TAPI_RING_MAX_SET             _IOW (IFX_TAPI_IOC_MAGIC, 0x85, char)

/** This service sets the ring cadence for the non-blocking ringing services.
   The cadence value has to be set before ringing starts
   (\ref IFX_TAPI_RING_START). This service is non-blocking.

   \param int The parameter defines the cadence. This value contains
   the encoded cadence sequence. One bit represents ring cadence voltage
   for 0.5 sec.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   The number of ring bursts can be counted via exception
   “PSTN line is ringing” of service \ref IFX_TAPI_EXCEPTION_GET .

   \code
   // pattern of 3 sec. ring and 1 sec. pause : 1111 1100 1111 1100 ...
   WORD32 nCadence = 0xFCFCFCFC;
   // set the cadence sequence
   ioctl(fd, IFX_TAPI_RING_CADENCE_SET, nCadence);
   \endcode   */
#define  IFX_TAPI_RING_CADENCE_SET        _IOW (IFX_TAPI_IOC_MAGIC, 0x86, short)

/** This service starts the non-blocking ringing on the phone line without CID.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks
   If ringing with Caller ID wished, \ref IFX_TAPI_CID_TX_SEQ_START should be
   used! The ringing must be stopped with \ref IFX_TAPI_RING_STOP. A second call
   to \ref IFX_TAPI_RING_START while the phone rings returns an error.
   The ringing can be configured with the interfaces \ref IFX_TAPI_RING_CFG_SET,
   \ref IFX_TAPI_RING_CADENCE_SET and \ref IFX_TAPI_RING_CADENCE_HR_SET before
   this interface is called.

   \code
   // start the ringing
   ioctl(fd, IFX_TAPI_RING_START, 0);
   \endcode   */
#define  IFX_TAPI_RING_START                      _IO (IFX_TAPI_IOC_MAGIC, 0x87)

/** This service stops non-blocking ringing on the phone line which was started
   before with service \ref IFX_TAPI_RING_START or
   \ref IFX_TAPI_CID_TX_SEQ_START. This service is non-blocking.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   ioctl(fd, IFX_TAPI_RING_STOP, 0);
   \endcode   */
#define  IFX_TAPI_RING_STOP                       _IO (IFX_TAPI_IOC_MAGIC, 0x88)

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ==================================================================== */
/* TAPI PCM Services, ioctl commands (Group TAPI_INTERFACE_PCM)         */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** This service sets the configuration of the PCM interface.
   \param IFX_TAPI_PCM_CFG_t* The parameter points to a
   \ref IFX_TAPI_PCM_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The parameter rate must be set to the PCM rate, which is
   applied to the device, otherwise error is returned.

   \code
   IFX_TAPI_PCM_CFG_t Pcm;
   memset (&Pcm, 0, sizeof(IFX_TAPI_PCM_CFG_t));
   Pcm.nTimeslotRX = 5;
   Pcm.nTimeslotTX = 5;
   Pcm.nHighway = 1;
   // 16 bit resolution
   Pcm.nResolution = IFX_TAPI_PCM_RES_LINEAR_16BIT;
   // 2048 kHz sample rate
   Pcm.nRate = 2048;
   // configure PCM interface
   ioctl(fd, IFX_TAPI_PCM_CFG_SET, &Pcm);
   \endcode   */
#define  IFX_TAPI_PCM_CFG_SET                      _IO(IFX_TAPI_IOC_MAGIC, 0x04)

/** This service gets the configuration of the PCM interface.

   \param IFX_TAPI_PCM_CFG_t* The parameter points to a
   \ref IFX_TAPI_PCM_CFG_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \code
   IFX_TAPI_PCM_CFG_t Pcm;
   memset (&Pcm, 0, sizeof(IFX_TAPI_PCM_CFG_t));
   ioctl(fd, IFX_TAPI_PCM_CFG_GET, &Pcm);
   \endcode   */
#define  IFX_TAPI_PCM_CFG_GET                      _IO(IFX_TAPI_IOC_MAGIC, 0x05)

/** This service activate / deactivates the PCM timeslots configured for
    this channel.
   \param int The parameter defines the activation status
       - 0 deactivate the timeslot
       - 1 activate the timeslot

   \return 0 if successful, otherwise -1 in case of an error

   \code
   // activate the PCM timeslot
   ioctl(fd, IFX_TAPI_PCM_ACTIVATION_SET, 1);
\endcode   */
#define  IFX_TAPI_PCM_ACTIVATION_SET               _IO(IFX_TAPI_IOC_MAGIC, 0x06)

/** This service gets the activation status of the PCM timeslots configured
   for this channel

   \param bAct The parameter points to an integer which returns the status
          - 0: The timeslot is deactive
          - 1: The timeslot is active

   \return 0 if successful, otherwise -1 in case of an error

   \code
   int bAct, ret;
   ret = ioctl(fd, IFX_TAPI_PCM_ACTIVATION_GET, &bAct);
   if (ret == 0 && bAct) print ("Activated\n");
\endcode   */
#define  IFX_TAPI_PCM_ACTIVATION_GET               _IO(IFX_TAPI_IOC_MAGIC, 0x07)

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================= */
/* TAPI Fax T.38 Services, ioctl commands (Group TAPI_INTERFACE_FAX)       */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** This service configures and enables the modulator for a T.38 fax session.

   \param IFX_TAPI_T38_MOD_DATA_t* The parameter points to a
   \ref IFX_TAPI_T38_MOD_DATA_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service deactivates the voice path and configures the channel
    for a fax session.

   \code
     IFX_TAPI_T38_MOD_DATA_t param;
     memset (&param, 0, sizeof(IFX_TAPI_T38_MOD_DATA_t));
     // set V.21 standard
     param.nStandard = 0x01;
     ret = ioctl(fd, IFX_TAPI_T38_MOD_START, &param);
   \endcode */
#define  IFX_TAPI_T38_MOD_START              _IOW(IFX_TAPI_IOC_MAGIC, 0x1A, int)

/** This service configures and enables the demodulator for a T.38 fax session.

   \param IFX_TAPI_T38_DEMOD_DATA_t* The parameter points to a
   \ref IFX_TAPI_T38_DEMOD_DATA_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This service deactivates the voice path and configures the driver
    for a fax session.

   \code
     IFX_TAPI_T38_DEMOD_DATA_t param;
     memset (&param, 0, sizeof(IFX_TAPI_T38_DEMOD_DATA_t));
     // set V.21 standard as standard used for fax
     param.nStandard1 = 0x01;
     // set V.17/14400 as alternative standard
     param.nStandard2 = 0x09;
     ret = ioctl(fd, IFX_TAPI_T38_DEMOD_START, &param);
   \endcode */
#define  IFX_TAPI_T38_DEMOD_START            _IOW(IFX_TAPI_IOC_MAGIC, 0x1B, int)

/** This service disables the T.38 fax data pump and activates the voice
    path again.

   \param int This interface expects no parameter. It should be set to 0.

   \return 0 if successful, otherwise -1 in case of an error

   \code
     ret = ioctl(fd, IFX_TAPI_T38_STOP, 0);
   \endcode */
#define  IFX_TAPI_T38_STOP                   _IOW(IFX_TAPI_IOC_MAGIC, 0x1C, int)

/** This service provides the T.38 fax status on query.

   \param IFX_TAPI_T38_STATUS_t* The parameter points to a
   \ref IFX_TAPI_T38_STATUS_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This interface must be used when a fax exception has occurred to
    know the status. An error or a change of status will be indicated with TAPI
    exceptions, refer to \ref IFX_TAPI_EXCEPTION_t.

   \code
     IFX_TAPI_T38_STATUS_t param;
     memset (&param, 0, sizeof(IFX_TAPI_T38_STATUS_t));
     // request status
     ret = ioctl(fd, IFX_TAPI_T38_STATUS_GET, &param);
   \endcode */
#define  IFX_TAPI_T38_STATUS_GET             _IOR(IFX_TAPI_IOC_MAGIC, 0x1D, int)

/*@}*/ /* TAPI_INTERFACE_FAX */

/* ======================================================================= */
/* TAPI Test Services, ioctl commands (Group TAPI_INTERFACE_TEST)          */
/* ======================================================================= */

/** \addtogroup TAPI_INTERFACE_TEST */
/*@{*/
/** This service generates an on or off hook event for the low level driver.

   \param 0 - on hook, if not already in on hook
          1 - off hook, if not already in off hook

   \return 0 if successful, otherwise -1 in case of an error

   \remarks The hook event then gets to the hook state machine for
    validation. Depending on the timing of calling this interface
    also hook flash and pulse dialing can be verified.
   The example shows the generation of a flash hook with a timing of
   100 ms. The flash hook can be queried afterwards by the ioctl
   IFX_TAPI_CH_STATUS_GET.

   \code
   // generate on hook
   ret = ioctl(fd, IFX_TAPI_TEST_HOOKGEN, 0);
   // generate off hook for 100 ms
   ret = ioctl(fd, IFX_TAPI_TEST_HOOKGEN, 1);
   sleep (100);
   ret = ioctl(fd, IFX_TAPI_TEST_HOOKGEN, 0);
   \endcode */
#define  IFX_TAPI_TEST_HOOKGEN           _IOW(IFX_TAPI_IOC_MAGIC, 0x3E, int)

/** This service switches a specified loop in the device or the driver.

   \param IFX_TAPI_TEST_LOOP_t* The parameter points to a
   \ref IFX_TAPI_TEST_LOOP_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks

   \code
   IFX_TAPI_TEST_LOOP_t parm;
   memset (&param, 0, sizeof(IFX_TAPI_TEST_LOOP_t));
   param.bAnalog = 1;
   ret = ioctl(fd, IFX_TAPI_TEST_LOOP, &param);
   \endcode */
#define  IFX_TAPI_TEST_LOOP                _IOW(IFX_TAPI_IOC_MAGIC, 0x3F, int)

/*@}*/ /* TAPI_INTERFACE_TEST */

/** \todo uncatalogued ioctl command */
#define  PHONE_PSTN_SET_STATE               _IOW (IFX_TAPI_IOC_MAGIC, 0xA4, int)
/** \todo uncatalogued ioctl command */
#define  PHONE_PSTN_GET_STATE                     _IO (IFX_TAPI_IOC_MAGIC, 0xA5)
/** \todo uncatalogued ioctl command */
#define  PHONE_WINK_DURATION                _IOW (IFX_TAPI_IOC_MAGIC, 0xA6, int)
/** \todo uncatalogued ioctl command */
#define  PHONE_WINK                         _IOW (IFX_TAPI_IOC_MAGIC, 0xAA, int)
/** \todo uncatalogued ioctl command */
#define  PHONE_QUERY_CODEC                        _IO (IFX_TAPI_IOC_MAGIC, 0xA7)
/** \todo uncatalogued ioctl command */
#define  PHONE_PSTN_LINETEST                      _IO (IFX_TAPI_IOC_MAGIC, 0xA8)

/* ========================================================================= */
/*                     TAPI Interface Constants                              */
/* ========================================================================= */

/* ======================================================================== */
/* TAPI Initialization Services, constants (Group TAPI_INTERFACE_INIT)      */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================= */
/* TAPI Operation Control Services, constants (Group TAPI_INTERFACE_OP)    */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** TAPI phone volume control */

/** Switch the phone off */
#define IFX_TAPI_LINE_VOLUME_OFF                (0xFF)
/** Switch the volume to low, -24 dB */
#define IFX_TAPI_LINE_VOLUME_LOW                (-24)
/** Switch the volume to medium, 0 dB */
#define IFX_TAPI_LINE_VOLUME_MEDIUM             (0)
/** Switch the volume to high, 24 dB */
#define IFX_TAPI_LINE_VOLUME_HIGH               (24)

/** TAPI Lec control */

/** LEC delay line maximum length */
#define IFX_TAPI_LEC_LEN_MAX                      (16)
/** LEC delay line minimum length */
#define IFX_TAPI_LEC_LEN_MIN                      (4)

/*@}*/ /* TAPI_INTERFACE_OP */

/* ================================================================ */
/* TAPI Metering Services, constants (Group TAPI_INTERFACE_METER)   */
/* ================================================================ */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_METER */

/* ====================================================================== */
/* TAPI Tone Services, constants (Group TAPI_INTERFACE_TONE)              */
/* ====================================================================== */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** maximum number of simple tones wich can be played in one go */
#define IFX_TAPI_TONE_SIMPLE_MAX               (7)

/** Maximum tone generation steps, also called cadences */
#define IFX_TAPI_TONE_STEPS_MAX                  (6)

/** Tone minimum index which can be configured by user */
#define IFX_TAPI_TONE_INDEX_MIN                 (32)

/** Tone maximum index which can be configured by user */
#define IFX_TAPI_TONE_INDEX_MAX                 (255)

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ===================================================================== */
/* TAPI Dial Services, constants (Group TAPI_INTERFACE_DIAL)             */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/

/** Dial Fifo Size */
#define IFX_TAPI_PULSE_FIFO_SIZE                  (20)
/** DTMF Fifo Size */
#define IFX_TAPI_DTMF_FIFO_SIZE                  (40)

/*@}*/ /* TAPI_INTERFACE_DIAL */

/* =================================================================== */
/* TAPI Signal Services, constants (Group TAPI_INTERFACE_SIGNAL)       */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ===================================================================== */
/* TAPI Caller ID Services, constants (Group TAPI_INTERFACE_CID)         */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** CID Rx Fifo Size */
#define IFX_TAPI_CID_RX_FIFO_SIZE               (10)

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
#define IFX_TAPI_CID_SIZE_MAX                  (258)

/* #define USA_RING_CADENCE         0xC0C0 */

/** Length of a CID message (in characters) */
#define IFX_TAPI_CID_MSG_LEN_MAX               (50)

/*@}*/ /* TAPI_INTERFACE_CID */

/* =================================================================== */
/* TAPI Connection Services, constants (Group TAPI_INTERFACE_CON)      */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================= */
/* TAPI Miscellaneous Services, constants (Group TAPI_INTERFACE_MISC)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ======================================================================= */
/* TAPI Ringing Services, constants (Group TAPI_INTERFACE_RINGING)         */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Maximum number of cadence bytes */
#define IFX_TAPI_RING_CADENCE_MAX_BYTES                 (40)

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ======================================================================= */
/* TAPI PCM Services, constants (Group TAPI_INTERFACE_PCM)                 */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================= */
/* TAPI Fax T.38 Services, constants (Group TAPI_INTERFACE_FAX)            */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_FAX */

/* ========================================================================= */
/*                      TAPI Interface Enumerations                          */
/* ========================================================================= */


/* ======================================================================== */
/* TAPI Initialization Services, enumerations (Group TAPI_INTERFACE_INIT)   */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** Tapi Initialization modes, selection for target system.
   If different modes are supported by the implementation, this parameter
   specifies which mode should be set up. The meaning of the mode is dependent
   on the implementation. */
typedef enum _IFX_TAPI_INIT_MODE
{
   /** Default initialization */
   IFX_TAPI_INIT_MODE_DEFAULT = 0,
   /** Typical VoIP solution. Phone connected to a packet coder (data channel)
    with DSP features for signal detection */
   IFX_TAPI_INIT_MODE_VOICE_CODER = 1,
   /** Phone to PCM using DSP features for signal detection */
   IFX_TAPI_INIT_MODE_PCM_DSP = 2,
   /** Phone to PCM connection without DSP features */
   IFX_TAPI_INIT_MODE_PCM_PHONE = 3,
   /** No special initialization, for testing purposes */
   IFX_TAPI_INIT_MODE_NONE = 0xff
} IFX_TAPI_INIT_MODE_t;

/** Country selection for TAPI_Init. For future purposes, not yet used.
   If different countries are supported by the implementation, this parameter
   specifies which one. */
typedef enum _IFX_TAPI_INIT_COUNTRY
{
   /** Default contry */
   IFX_TAPI_INIT_COUNTRY_DEFAULT = 0,
   /** Germany */
   IFX_TAPI_INIT_COUNTRY_DE = 1,
   /** USA */
   IFX_TAPI_INIT_COUNTRY_US = 2,
   /** United Kingdom */
   IFX_TAPI_INIT_COUNTRY_UK = 3
} IFX_TAPI_INIT_COUNTRY_t;

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================== */
/* TAPI Operation Control Services, enumerations (Group TAPI_INTERFACE_OP)  */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Defines for linefeeding. */
typedef enum _IFX_TAPI_LINE_FEED
{
   /** Normal feeding mode for phone off hook */
   IFX_TAPI_LINE_FEED_ACTIVE = 0,
   /** Normal feeding mode for phone off hook reversed */
   IFX_TAPI_LINE_FEED_ACTIVE_REV = 1,
   /** Power down resistance = on hook with hook detection */
   IFX_TAPI_LINE_FEED_STANDBY = 2,
   /** Switch off the line, but the device is able to test the line,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_HIGH_IMPEDANCE = 3,
   /** Switch off the line and the device */
   IFX_TAPI_LINE_FEED_DISABLED = 4,
   /** not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_GROUND_START = 5,
   /** Thresholds for automatic battery switch are set via
   coefficient settings, not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_NORMAL_AUTO = 6,
   /** Thresholds for automatic battery switch are set via coefficient
   settings reversed, not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_REVERSED_AUTO = 7,
   /** feeding mode for phone off hook with low battery to save power */
   IFX_TAPI_LINE_FEED_NORMAL_LOW = 8,
   /** feeding mode for phone off hook with low battery to save power
       and reserved polarity */
   IFX_TAPI_LINE_FEED_REVERSED_LOW = 9,
   /** Reserved, needed for ring call back function */
   IFX_TAPI_LINE_FEED_RING_BURST = 10,
   /** Reserved, needed for ring call back function */
   IFX_TAPI_LINE_FEED_RING_PAUSE = 11,
   /** Reserved, needed for internal function,
       not supported for VINETIC 2CPE, Revision 2.1  */
   IFX_TAPI_LINE_FEED_METER = 12,
   /** Reserved, special test linemode,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_ACTIVE_LOW = 13,
   /** Reserved, special test linemode,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_ACTIVE_BOOSTED = 14,
   /** Reserved, special linemode for S-MAX Slic,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_ACT_TESTIN = 15,
   /** Reserved, special linemode for S-MAX Slic,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_DISABLED_RESISTIVE_SWITCH = 16,
   /** Power down resistance = on hook with hook detection,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_PARKED_REVERSED = 17,
   /** High resistive feeding mode with offhook sensing ability and
		 normal polarity */
   IFX_TAPI_LINE_FEED_ACTIVE_RES_NORMAL = 18,
   /** High resistive feeding mode with offhook sensing ability and
		 reversed polarity */
   IFX_TAPI_LINE_FEED_ACTIVE_RES_REVERSED = 19,
   /** Reserved, special linemode for SLIC LCP,
       not supported for VINETIC 2CPE, Revision 2.1 */
   IFX_TAPI_LINE_FEED_ACT_TEST = 20
} IFX_TAPI_LINE_FEED_t;

/** LEC gain levels */
typedef enum _IFX_TAPI_LEC_GAIN
{
   /** Turn LEC off */
   IFX_TAPI_LEC_GAIN_OFF = 0,
   /** Turn LEC on to low level*/
   IFX_TAPI_LEC_GAIN_LOW = 1,
   /** Turn LEC on to normal level*/
   IFX_TAPI_LEC_GAIN_MEDIUM = 2,
   /** Turn LEC on to high level*/
   IFX_TAPI_LEC_GAIN_HIGH = 3
} IFX_TAPI_LEC_GAIN_t;

/** LEC Type Configuration */
typedef enum _IFX_TAPI_LEC_TYPE
{
   /** No LEC, turn off */
   IFX_TAPI_LEC_TYPE_OFF  = 0,
   /** LEC Type is NLEC */
   IFX_TAPI_LEC_TYPE_NLEC = 1,
   /** LEC Type is WLEC */
   IFX_TAPI_LEC_TYPE_WLEC = 2
} IFX_TAPI_LEC_TYPE_t;

/** Validation types used for structure \ref IFX_TAPI_LINE_HOOK_VT_t
   \remarks
   The default values are as follows:

   - 80 ms  <= flash time      <= 200 ms
   - 30 ms  <= digit low time  <= 80 ms
   - 30 ms  <= digit high time <= 80 ms
   - interdigit time =     300 ms
   - off hook time   =      40 ms
   - on hook time    =     400 ms
   !!! open: only min time is validated and pre initialized */
typedef enum _IFX_TAPI_LINE_HOOK_VALIDATION_TYPE
{
   /** Settings for hook validation, if the time matches between nMinTime and
      nMaxTime an exception is raised. */
   IFX_TAPI_LINE_HOOK_VT_HOOKOFF_TIME     = 0x0,
   /** Settings for hook validation, if the time matches between nMinTime and
       nMaxTime an exception is raised. */
   IFX_TAPI_LINE_HOOK_VT_HOOKON_TIME      = 0x1,
   /** Settings for hook flash validation also known as register recall.
       If the time matches between the time defined in the fields nMinTime
       and nMaxTime an exception is raised */
   IFX_TAPI_LINE_HOOK_VT_HOOKFLASH_TIME   = 0x2,
   /** Settings for pulse digit low, open loop and make validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   IFX_TAPI_LINE_HOOK_VT_DIGITLOW_TIME    = 0x4,
   /** Settings for pulse digit high, close loop and break validation.
       The time must match between the time defined in the fields nMinTime and
       nMaxTime to recognize it as pulse dialing event */
   IFX_TAPI_LINE_HOOK_VT_DIGITHIGH_TIME   = 0x8,
   /** Settings for pulse digit pause. The time must match the time defined
       in the fields nMinTime and nMaxTime to recognize it as
       pulse dialing event */
   IFX_TAPI_LINE_HOOK_VT_INTERDIGIT_TIME  = 0x10
} IFX_TAPI_LINE_HOOK_VALIDATION_TYPE_t;

/** Lec NLP Settings */
typedef enum _IFX_TAPI_LEC_NLP
{
   /** Use default NLP setting */
   IFX_TAPI_LEC_NLP_DEFAULT = 0,
   /** Switch on NLP */
   IFX_TAPI_LEC_NLP_ON = 1,
   /** Switch off NLP */
   IFX_TAPI_LEC_NLP_OFF = 2
} IFX_TAPI_LEC_NLP_t;

/** Specifies the Enable/Disable mode of the high level */
typedef enum _IFX_TAPI_LINE_LEVEL
{
   /** Disable line level */
   IFX_TAPI_LINE_LEVEL_DISABLE = 0x0,
   /** Enable line level */
   IFX_TAPI_LINE_LEVEL_ENABLE = 0x1
} IFX_TAPI_LINE_LEVEL_t;

/** Specifies the Enable/Disable mode of the AGC resouce */
typedef enum _IFX_TAPI_ENC_AGC_MODE
{
   IFX_TAPI_ENC_AGC_MODE_DISABLE = 0x0,
   IFX_TAPI_ENC_AGC_MODE_ENABLE  = 0x1
} IFX_TAPI_ENC_AGC_MODE_t;

/** Structure used for \ref IFX_TAPI_ENC_AGC_CFG_SET  */
typedef struct _IFX_TAPI_ENC_AGC_CFG
{
   /** "Compare Level", this is the target level in 'dB'.
       Range: -50dB ... 0dB */
   int   com;
   /** "Maximum Gain", maximum gain that we'll be applied to the signal in 'dB'.
       Range: 0dB ... 48dB */
   int   gain;
   /** "Maximum Attenuation for AGC", maximum attenuation that we'll be
       applied to the signal in 'dB'.
       Range: -42dB ... 0dB */
   int   att;
   /** "Minimum Input Level", signals below this threshold won't be
       processed by AGC in 'dB'.
       Range: -60dB ... -25dB */
   int   lim;
} IFX_TAPI_ENC_AGC_CFG_t;

/*@}*/ /* TAPI_INTERFACE_OP */

/* ======================================================================= */
/* TAPI Metering Services, enumerations (Group TAPI_INTERFACE_METER)       */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** Metering modes. */
typedef enum _IFX_TAPI_METER_MODE
{
   /** Normal TTX mode */
   IFX_TAPI_METER_MODE_TTX = 0,
   /** Reverse polarity mode */
   IFX_TAPI_METER_MODE_REVPOL = 1
} IFX_TAPI_METER_MODE_t;

/*@}*/ /* TAPI_INTERFACE_METER */

/* ======================================================================= */
/* TAPI Tone Control Services, enumerations (Group TAPI_INTERFACE_TONE)    */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Defines the tone generator usage */
typedef enum _IFX_TAPI_TONE_TG
{
   /** Use tone generator 1 */
   IFX_TAPI_TONE_TG1 = 1,
   /** Use tone generator 2 */
   IFX_TAPI_TONE_TG2 = 2,
   /** Use all tone generators  */
   IFX_TAPI_TONE_TGALL = 0xff
} IFX_TAPI_TONE_TG_t;

/** Tone Sources */
typedef enum _IFX_TAPI_TONE_SRC
{
   /** Tone is played out on default source */
   IFX_TAPI_TONE_SRC_DEFAULT = 0,
   /** Tone is played out on DSP, default, if available */
   IFX_TAPI_TONE_SRC_DSP     = 0x4000,
   /** Tone is played out on local tone generator in
      the analog part of the device.
      Default if DSP is not available */
   IFX_TAPI_TONE_SRC_TG = 0x8000
} IFX_TAPI_TONE_SRC_t;

/** Frequency setting for a cadence step */
typedef enum _IFX_TAPI_TONE_FREQ
{
   /** No Frequency */
   IFX_TAPI_TONE_FREQNONE = 0,
   /** Frequency A */
   IFX_TAPI_TONE_FREQA = 0x1,
   /** Frequency B */
   IFX_TAPI_TONE_FREQB = 0x2,
   /** Frequency C */
   IFX_TAPI_TONE_FREQC = 0x4,
   /** Frequency D */
   IFX_TAPI_TONE_FREQD = 0x8,
   /** All Frequencies */
   IFX_TAPI_TONE_FREQALL = 0xF
} IFX_TAPI_TONE_FREQ_t;

/** Modulation setting for a cadence step */
typedef enum _IFX_TAPI_TONE_MODULATION
{
   /** Modulation off */
   IFX_TAPI_TONE_MODULATION_OFF = 0,
   /** Modulation on */
   IFX_TAPI_TONE_MODULATION_ON = 1
} IFX_TAPI_TONE_MODULATION_t;

/** Tone types */
typedef enum _IFX_TAPI_TONE_TYPE
{
   /** Simple tone */
   IFX_TAPI_TONE_TYPE_SIMPLE = 1,
   /** Composed tone */
   IFX_TAPI_TONE_TYPE_COMPOSED = 2
} IFX_TAPI_TONE_TYPE_t;

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ==================================================================== */
/* TAPI Dial Services, enumerations (Group TAPI_INTERFACE_DIAL)         */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/

/** Enumeration for dial status events */
typedef enum _IFX_TAPI_DIALING_STATUS
{
   /** DTMF sign detected */
   IFX_TAPI_DIALING_STATUS_DTMF  = 0x01,
   /** pulse digit detected */
   IFX_TAPI_DIALING_STATUS_PULSE = 0x02
} IFX_TAPI_DIALING_STATUS_t;

/*@}*/ /* TAPI_INTERFACE_DIAL */

/* ==================================================================== */
/* TAPI Signal Services, enumerations (Group TAPI_INTERFACE_SIGNAL)     */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/** List the tone detection options. Some application maybe not interessted
    whether the signal came from the receive or transmit path. Therefore
    for each signal a mask exists, that includes receive and transmit path.
    */
typedef enum _IFX_TAPI_SIG
{
   /** no signal detected */
   IFX_TAPI_SIG_NONE        = 0x0,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      receive path */
   IFX_TAPI_SIG_DISRX       = 0x1,
   /** V.21 Preamble Fax Tone, Digital idetification signal (DIS),
      transmit path */
   IFX_TAPI_SIG_DISTX       = 0x2,
   /** V.21 Preamble Fax Tone in all path, Digital identification
       signal (DIS)  */
   IFX_TAPI_SIG_DIS         = 0x4,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, receive path */
   IFX_TAPI_SIG_CEDRX       = 0x8,
   /** V.25 2100 Hz (CED) Modem/Fax Tone, transmit path */
   IFX_TAPI_SIG_CEDTX       = 0x10,
   /** V.25 2100 Hz (CED) Modem/Fax Tone in all paths */
   IFX_TAPI_SIG_CED         = 0x20,
   /** CNG Fax Calling Tone (1100 Hz) receive path */
   IFX_TAPI_SIG_CNGFAXRX    = 0x40,
   /** CNG Fax Calling Tone (1100 Hz) transmit path */
   IFX_TAPI_SIG_CNGFAXTX    = 0x80,
   /** CNG Fax Calling Tone (1100 Hz) in all paths */
   IFX_TAPI_SIG_CNGFAX      = 0x100,
   /** CNG Modem Calling Tone (1300 Hz) receive path */
   IFX_TAPI_SIG_CNGMODRX    = 0x200,
   /** CNG Modem Calling Tone (1300 Hz) transmit path  */
   IFX_TAPI_SIG_CNGMODTX    = 0x400,
   /** CNG Modem Calling Tone (1300 Hz) in all paths */
   IFX_TAPI_SIG_CNGMOD      = 0x800,
   /** Phase reversal detection receive path
       \remarks Not supported phase reversal uses the same
       paths as CED detection. The detector for CED must be configured
       as well  */
   IFX_TAPI_SIG_PHASEREVRX  = 0x1000,
   /** Phase reversal detection transmit path
      \remarks Not supported phase reversal uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   IFX_TAPI_SIG_PHASEREVTX  = 0x2000,
   /** Phase reversal detection in all paths
       \remarks Phase reversals are detected at the
       end of an CED. If this signal is enabled also CED end detection
       is automatically enabled and reported if it occurs */
   IFX_TAPI_SIG_PHASEREV    = 0x4000,
   /** Amplitude modulation receive path

      \remarks Not supported amplitude modulation uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   IFX_TAPI_SIG_AMRX         = 0x8000,
   /** Amplitude modulation transmit path

   \remarks Not supported amplitude modulation uses the same
   paths as CED detection. The detector for CED must be configured
   as well  */
   IFX_TAPI_SIG_AMTX                 = 0x10000,
   /** Amplitude modulation.

    \remarks Amplitude modulation is detected at the
    end of an CED. If this signal is enabled also CED end detection
    is automatically enabled and reported if it occurs.

    \note In case of AM detected the driver automatically switches
    to modem coefficients after the ending of CED */
   IFX_TAPI_SIG_AM                   = 0x20000,
   /** Modem tone holding signal stopped receive path */
   IFX_TAPI_SIG_TONEHOLDING_ENDRX    = 0x40000,
   /** Modem tone holding signal stopped transmit path */
   IFX_TAPI_SIG_TONEHOLDING_ENDTX    = 0x80000,
   /** Modem tone holding signal stopped all paths */
   IFX_TAPI_SIG_TONEHOLDING_END      = 0x100000,
   /** End of signal CED detection receive path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   IFX_TAPI_SIG_CEDENDRX          = 0x200000,
   /** End of signal CED detection transmit path

      \remarks Not supported. CED end detection uses the same
      paths as CED detection. The detector for CED must be configured
      as well  */
   IFX_TAPI_SIG_CEDENDTX          = 0x400000,
   /** End of signal CED detection. This signal also includes
      information about phase reversals and amplitude modulation,
      if enabled */
   IFX_TAPI_SIG_CEDEND            = 0x800000,
   /** Signals a call progress tone detection. This signal is enabled with
       the interface \ref IFX_TAPI_TONE_CPTD_START and stopped with
       \ref IFX_TAPI_TONE_CPTD_STOP. It cannot be activated with
       \ref IFX_TAPI_SIG_DETECT_ENABLE */
   IFX_TAPI_SIG_CPTD              = 0x1000000,
   /** Signals the V8bis detection on the receive path */
   IFX_TAPI_SIG_V8BISRX           = 0x2000000,
   /** Signals the V8bis detection on the transmit path */
   IFX_TAPI_SIG_V8BISTX           = 0x4000000,
   /** Signals that the Caller-ID transmission has finished
       This event cannot be activated with \ref IFX_TAPI_SIG_DETECT_ENABLE.
       Please use IFX_TAPI_CID_RX_START and IFX_TAPI_CID_RX_STOP instead. */
   IFX_TAPI_SIG_CIDENDTX          = 0x8000000,
   /** Enables DTMF reception on locally connected analog line */
   IFX_TAPI_SIG_DTMFTX            = 0x10000000,
   /** Enables DTMF reception on remote connected line */
   IFX_TAPI_SIG_DTMFRX            = 0x20000000
} IFX_TAPI_SIG_t;

/** Extended tone detection options */
typedef enum _IFX_TAPI_SIG_EXT
{
   /** no signal detected */
   IFX_TAPI_SIG_EXT_NONE          = 0x0,
   /** 980 Hz single tone (V.21L mark sequence) receive path */
   IFX_TAPI_SIG_EXT_V21LRX        = 0x1,
   /** 980 Hz single tone (V.21L mark sequence) transmit path */
   IFX_TAPI_SIG_EXT_V21LTX        = 0x2,
   /** 980 Hz single tone (V.21L mark sequence) all paths */
   IFX_TAPI_SIG_EXT_V21L          = 0x4,
   /** 1400 Hz single tone (V.18A mark sequence) receive path */
   IFX_TAPI_SIG_EXT_V18ARX        = 0x8,
   /** 1400 Hz single tone (V.18A mark sequence) transmit path */
   IFX_TAPI_SIG_EXT_V18ATX        = 0x10,
   /** 1400 Hz single tone (V.18A mark sequence) all paths */
   IFX_TAPI_SIG_EXT_V18A          = 0x20,
   /** 1800 Hz single tone (V.27, V.32 carrier) receive path */
   IFX_TAPI_SIG_EXT_V27RX         = 0x40,
   /** 1800 Hz single tone (V.27, V.32 carrier) transmit path */
   IFX_TAPI_SIG_EXT_V27TX         = 0x80,
   /** 1800 Hz single tone (V.27, V.32 carrier) all paths */
   IFX_TAPI_SIG_EXT_V27           = 0x100,
   /** 2225 Hz single tone (Bell answering tone) receive path */
   IFX_TAPI_SIG_EXT_BELLRX        = 0x200,
   /** 2225 Hz single tone (Bell answering tone) transmit path */
   IFX_TAPI_SIG_EXT_BELLTX        = 0x400,
   /** 2225 Hz single tone (Bell answering tone) all paths */
   IFX_TAPI_SIG_EXT_BELL          = 0x800,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) receive path */
   IFX_TAPI_SIG_EXT_V22RX         = 0x1000,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) transmit path */
   IFX_TAPI_SIG_EXT_V22TX         = 0x2000,
   /** 2250 Hz single tone (V.22 unscrambled binary ones) all paths */
   IFX_TAPI_SIG_EXT_V22           = 0x4000,
   /** 2225 Hz or 2250 Hz single tone, not possible to distinguish
       receive path */
   IFX_TAPI_SIG_EXT_V22ORBELLRX   = 0x8000,
   /** 2225 Hz or 2250 Hz single tone, not possible to distinguish
       transmit path */
   IFX_TAPI_SIG_EXT_V22ORBELLTX   = 0x10000,
   /** 2225 Hz or 2250 Hz single tone, not possible to distinguish all paths */
   IFX_TAPI_SIG_EXT_V22ORBELL     = 0x20000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) receive path */
   IFX_TAPI_SIG_EXT_V32ACRX       = 0x40000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) transmit path */
   IFX_TAPI_SIG_EXT_V32ACTX       = 0x80000,
   /** 600 Hz + 300 Hz dual tone (V.32 AC) all paths */
   IFX_TAPI_SIG_EXT_V32AC         = 0x100000
} IFX_TAPI_SIG_EXT_t;

/** List the RFC2833 tone events that are detected from network side. */
typedef enum _IFX_TAPI_PKT_EV_NUM
{
   /** RFC2833 Event number for DTMF tone  #0 */
   IFX_TAPI_PKT_EV_NUM_DTMF_0     = 0,
   /** RFC2833 Event number for DTMF tone  #1 */
   IFX_TAPI_PKT_EV_NUM_DTMF_1     = 1,
   /** RFC2833 Event number for DTMF tone  #2 */
   IFX_TAPI_PKT_EV_NUM_DTMF_2     = 2,
   /** RFC2833 Event number for DTMF tone  #3 */
   IFX_TAPI_PKT_EV_NUM_DTMF_3     = 3,
   /** RFC2833 Event number for DTMF tone  #4 */
   IFX_TAPI_PKT_EV_NUM_DTMF_4     = 4,
   /** RFC2833 Event number for DTMF tone  #5 */
   IFX_TAPI_PKT_EV_NUM_DTMF_5     = 5,
   /** RFC2833 Event number for DTMF tone  #6 */
   IFX_TAPI_PKT_EV_NUM_DTMF_6     = 6,
   /** RFC2833 Event number for DTMF tone  #7 */
   IFX_TAPI_PKT_EV_NUM_DTMF_7     = 7,
   /** RFC2833 Event number for DTMF tone  #8 */
   IFX_TAPI_PKT_EV_NUM_DTMF_8     = 8,
   /** RFC2833 Event number for DTMF tone  #9 */
   IFX_TAPI_PKT_EV_NUM_DTMF_9     = 9,
   /** RFC2833 Event number for DTMF tone STAR */
   IFX_TAPI_PKT_EV_NUM_DTMF_STAR  = 10,
   /** RFC2833 Event number for DTMF tone HASH */
   IFX_TAPI_PKT_EV_NUM_DTMF_HASH  = 11,
   /** RFC2833 Event number for ANS tone */
   IFX_TAPI_PKT_EV_NUM_ANS        = 32,
   /** RFC2833 Event number for /ANS tone */
   IFX_TAPI_PKT_EV_NUM_NANS       = 33,
   /** RFC2833 Event number for ANSam tone */
   IFX_TAPI_PKT_EV_NUM_ANSAM      = 34,
   /** RFC2833 Event number for /ANSam tone */
   IFX_TAPI_PKT_EV_NUM_NANSAM     = 35,
   /** RFC2833 Event number for CNG tone */
   IFX_TAPI_PKT_EV_NUM_CNG        = 36,
   /** RFC2833 Event number for DIS tone */
   IFX_TAPI_PKT_EV_NUM_DIS        = 54,
   /** no support RFC event received or no event received */
   IFX_TAPI_PKT_EV_NUM_NO_EVENT   = 0xFFFFFFFF
} IFX_TAPI_PKT_EV_NUM_t;

/** Specifies the CPT signal for CPT detection */
typedef enum _IFX_TAPI_TONE_CPTD_DIRECTION
{
   /** Receive direction */
   IFX_TAPI_TONE_CPTD_DIRECTION_RX = 0x1,
   /** Transmit direction */
   IFX_TAPI_TONE_CPTD_DIRECTION_TX = 0x2
} IFX_TAPI_TONE_CPTD_DIRECTION_t;

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ======================================================================= */
/* TAPI Caller ID Services, enumerations (Group TAPI_INTERFACE_CID)        */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/

/** List of ETSI Alerts. */
typedef enum _IFX_TAPI_CID_ALERT_ETSI
{
   /** First Ring Burst.
      \remark
         - Defined only for CID transmission associated with ringing.
         - Ringing timings are set via \ref IFX_TAPI_RING_CADENCE_t */
   IFX_TAPI_CID_ALERT_ETSI_FR = 0x0,
   /** DTAS. */
   IFX_TAPI_CID_ALERT_ETSI_DTAS = 0x1,
   /** Ring Pulse. */
   IFX_TAPI_CID_ALERT_ETSI_RP = 0x2,
   /** Line Reversal (alias Polarity Reversal) followed by DTAS. */
   IFX_TAPI_CID_ALERT_ETSI_LRDTAS = 0x3
} IFX_TAPI_CID_ALERT_ETSI_t;

/** List of CID standards */
typedef enum _IFX_TAPI_CID_STD
{
   /** Bellcore/Telcordia GR-30-CORE.
      Using Bell202 FSK coding of CID information. */
   IFX_TAPI_CID_STD_TELCORDIA = 0x0,
   /** ETSI 300-659-1/2/3 V1.3.1.
      Using V.23 FSK coding to transmit CID information. */
   IFX_TAPI_CID_STD_ETSI_FSK = 0x1,
   /** ETSI 300-659-1/2/3 V1.3.1.
      Using DTMF transmission of CID information. */
   IFX_TAPI_CID_STD_ETSI_DTMF = 0x2,
   /** SIN 227 Issue 3.4.
      Using V.23 FSK coding of CID information. */
   IFX_TAPI_CID_STD_SIN = 0x3,
   /** NTT standard: TELEPHONE SERVICE INTERFACES, Edition 5.
      Using a modified V.23 FSK coding of CID information. */
   IFX_TAPI_CID_STD_NTT = 0x4
} IFX_TAPI_CID_STD_t;

/** Caller ID transmission modes */
typedef enum _IFX_TAPI_CID_HOOK_MODE
{
   /** On-hook transmission. Applicable to CID type 1 and MWI */
   IFX_TAPI_CID_HM_ONHOOK   = 0x00,
   /** Off-hook transmission. Applicable to CID type 2 and MWI */
   IFX_TAPI_CID_HM_OFFHOOK  = 0x01
} IFX_TAPI_CID_HOOK_MODE_t;

/** Caller ID message types (defined in ETSI EN 300 659-3) */
typedef enum _IFX_TAPI_CID_MSG_TYPE
{
   /** Call Set-up. Corresponds to Caller ID type 1 and type 2. */
   IFX_TAPI_CID_MT_CSUP  = 0x80,
   /** Message Waiting Indicator */
   IFX_TAPI_CID_MT_MWI   = 0x82,
   /** Advice of Charge */
   IFX_TAPI_CID_MT_AOC   = 0x86,
   /** Short Message Service */
   IFX_TAPI_CID_MT_SMS   = 0x89,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES01 = 0xF1,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES02 = 0xF2,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES03 = 0xF3,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES04 = 0xF4,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES05 = 0xF5,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES06 = 0xF6,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES07 = 0xF7,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES08 = 0xF8,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES09 = 0xF9,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0A = 0xFA,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0B = 0xFB,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0C = 0xFC,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0D = 0xFD,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0E = 0xFE,
   /** Reserved for Network Operator Use */
   IFX_TAPI_CID_MT_RES0F = 0xFF
} IFX_TAPI_CID_MSG_TYPE_t;

/** Caller ID Services (defined in ETSI EN 300 659-3) */
typedef enum _IFX_TAPI_CID_SERVICE_TYPE
{
   /** date and time presentation */
   IFX_TAPI_CID_ST_DATE        = 0x01,
   /** calling line identity (mandatory) */
   IFX_TAPI_CID_ST_CLI         = 0x02,
   /** called line identity */
   IFX_TAPI_CID_ST_CDLI        = 0x03,
   /** reason for absence of CLI */
   IFX_TAPI_CID_ST_ABSCLI      = 0x04,
   /** calling line name */
   IFX_TAPI_CID_ST_NAME        = 0x07,
   /** reason for absence of name */
   IFX_TAPI_CID_ST_ABSNAME     = 0x08,
   /** visual indicator */
   IFX_TAPI_CID_ST_VISINDIC    = 0x0B,
   /** Message Identification */
   IFX_TAPI_CID_ST_MSGIDENT    = 0x0D,
   /** Last Message CLI */
   IFX_TAPI_CID_ST_LMSGCLI     = 0x0E,
   /** Complementary Date and Time */
   IFX_TAPI_CID_ST_CDATE       = 0x0F,
   /** complementary calling line identity */
   IFX_TAPI_CID_ST_CCLI        = 0x10,
   /** call type */
   IFX_TAPI_CID_ST_CT          = 0x11,
   /** first called line identity  */
   IFX_TAPI_CID_ST_FIRSTCLI    = 0x12,
   /** number of messages  */
   IFX_TAPI_CID_ST_MSGNR       = 0x13,
   /** type of forwarded call  */
   IFX_TAPI_CID_ST_FWCT        = 0x15,
   /** type of calling user  */
   IFX_TAPI_CID_ST_USRT        = 0x16,
   /** number redirection */
   IFX_TAPI_CID_ST_REDIR       = 0x1A,
   /** charge */
   IFX_TAPI_CID_ST_CHARGE      = 0x20,
   /** additional charge */
   IFX_TAPI_CID_ST_ACHARGE     = 0x21,
   /** duration of the call */
   IFX_TAPI_CID_ST_DURATION    = 0x23,
   /** network provider id */
   IFX_TAPI_CID_ST_NTID        = 0x30,
   /** carrier identity */
   IFX_TAPI_CID_ST_CARID       = 0x31,
   /** selection of terminal function */
   IFX_TAPI_CID_ST_TERMSEL     = 0x40,
   /** display information, used as INFO for DTMF */
   IFX_TAPI_CID_ST_DISP        = 0x50,
   /** Service Information */
   IFX_TAPI_CID_ST_SINFO       = 0x55,
   /** extension for operator use */
   IFX_TAPI_CID_ST_XOPUSE      = 0xE0,
   /** transparent transmission  */
   IFX_TAPI_CID_ST_TRANSPARENT = 0xFF
} IFX_TAPI_CID_SERVICE_TYPE_t;

/** List of VMWI settings */
typedef enum _IFX_TAPI_CID_VMWI
{
   /** Message waiting disabled */
   IFX_TAPI_CID_VMWI_DIS = 0x00,
   /** Message waiting enabled */
   IFX_TAPI_CID_VMWI_EN  = 0xFF
} IFX_TAPI_CID_VMWI_t;

/** List of ABSCLI/ABSNAME settings */
typedef enum _IFX_TAPI_CID_ABSREASON
{
   /** Unavailable */
   IFX_TAPI_CID_ABSREASON_UNAV = 0x4F,
   /** Private */
   IFX_TAPI_CID_ABSREASON_PRIV = 0x50
} IFX_TAPI_CID_ABSREASON_t;

/* Defines for CID Receiver */

/** CID Receiver Status */
typedef enum _IFX_TAPI_CID_RX_STATE
{
   /** CID Receiver is not active */
   IFX_TAPI_CID_RX_STATE_INACTIVE = 0,
   /** CID Receiver is active */
   IFX_TAPI_CID_RX_STATE_ACTIVE   = 1,
   /** CID Receiver is just receiving data */
   IFX_TAPI_CID_RX_STATE_ONGOING = 2,
   /** CID Receiver is completed */
   IFX_TAPI_CID_RX_STATE_DATA_READY = 3
} IFX_TAPI_CID_RX_STATE_t;

/** CID Receiver Errors */
typedef enum _IFX_TAPI_CID_RX_ERROR
{
   /** No Error during CID Receiver operation */
   IFX_TAPI_CID_RX_ERROR_NONE = 0,
   /** Reading error during CID Receiver operation */
   IFX_TAPI_CID_RX_ERROR_READ = 1
} IFX_TAPI_CID_RX_ERROR_t;

/*@}*/ /* TAPI_INTERFACE_CID */

/* ========================================================================= */
/* TAPI Connection Services, enumerations (Group TAPI_INTERFACE_CON)         */
/* ========================================================================= */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Possible codecs to select for \ref IFX_TAPI_ENC_TYPE_SET
   and \ref IFX_TAPI_PCK_AAL_PROFILE_t. */
typedef enum _IFX_TAPI_ENC_TYPE
{
   /** G723, 6.3 kBit/s */
   IFX_TAPI_ENC_TYPE_G723_63  = 1,
   /** G723, 5.3 kBit/s */
   IFX_TAPI_ENC_TYPE_G723_53  = 2,
   /** G728, 16 kBit/s */
   IFX_TAPI_ENC_TYPE_G728     = 6,
   /** G729 A and B, 8 kBit/s */
   IFX_TAPI_ENC_TYPE_G729     = 7,
   /** G711 u-Law, 64 kBit/s */
   IFX_TAPI_ENC_TYPE_MLAW     = 8,
   /** G711 A-Law, 64 kBit/s */
   IFX_TAPI_ENC_TYPE_ALAW     = 9,
   /** G726, 16 kBit/s */
   IFX_TAPI_ENC_TYPE_G726_16  = 12,
   /** G726, 24 kBit/s */
   IFX_TAPI_ENC_TYPE_G726_24  = 13,
   /** G726, 32 kBit/s */
   IFX_TAPI_ENC_TYPE_G726_32  = 14,
   /** G726, 40 kBit/s */
   IFX_TAPI_ENC_TYPE_G726_40  = 15,
   /** G729 E, 11.8 kBit/s */
   IFX_TAPI_ENC_TYPE_G729_E   = 16,
   /** iLBC, 13.3 kBit/s */
   IFX_TAPI_ENC_TYPE_ILBC_133 = 17,
   /** iLBC, 15.2 kBit/s */
   IFX_TAPI_ENC_TYPE_ILBC_152 = 18,
   /** Maximum number of Codecs,
       used by \ref IFX_TAPI_PKT_RTP_PT_CFG_t  */
   IFX_TAPI_ENC_TYPE_MAX
} IFX_TAPI_ENC_TYPE_t;

/** Type channel for mapping. */
typedef enum _IFX_TAPI_MAP_TYPE
{
   /** Default. It depends on the device and configures the best applicable */
   IFX_TAPI_MAP_TYPE_DEFAULT = 0,
   /** Type is a coder packet channel */
   IFX_TAPI_MAP_TYPE_CODER = 1,
   /** Type is a PCM channel */
   IFX_TAPI_MAP_TYPE_PCM = 2,
   /** Type is a phone channel */
   IFX_TAPI_MAP_TYPE_PHONE = 3
} IFX_TAPI_MAP_TYPE_t;

/** Recording enabling and disabling information. */
typedef enum _IFX_TAPI_MAP_ENC
{
   /** Do not modify recording status */
   IFX_TAPI_MAP_ENC_NONE = 0,
   /** Start recording after mapping */
   IFX_TAPI_MAP_ENC_START = 1,
   /** Stop recording after mapping */
   IFX_TAPI_MAP_ENC_STOP = 2
} IFX_TAPI_MAP_ENC_t;

/** Play out enabling and disabling information. */
typedef enum _IFX_TAPI_MAP_DEC
{
   /** Do not modify playing status */
   IFX_TAPI_MAP_DEC_NONE = 0,
   /** Start playing after mapping */
   IFX_TAPI_MAP_DEC_START = 1,
   /** Stop playing after mapping */
   IFX_TAPI_MAP_DEC_STOP = 2
} IFX_TAPI_MAP_DEC_t;

/** Data channel destination types (conferencing). */
typedef enum _IFX_TAPI_MAP_DATA_TYPE
{
   IFX_TAPI_MAP_DATA_TYPE_DEFAULT = 0,
   IFX_TAPI_MAP_DATA_TYPE_CODER = 1,
   IFX_TAPI_MAP_DATA_TYPE_PCM = 2,
   IFX_TAPI_MAP_DATA_TYPE_PHONE = 3
} IFX_TAPI_MAP_DATA_TYPE_t;

/** Start/Stop information for data channel mapping. */
typedef enum _IFX_TAPI_MAP_DATA_START_STOP
{
   /** Do not modify the status of the recorder */
   IFX_TAPI_MAP_DATA_UNCHANGED = 0,
   /** Recording is started */
   IFX_TAPI_MAP_DATA_START = 1,
   /** Recording is stopped */
   IFX_TAPI_MAP_DATA_STOP = 2
} IFX_TAPI_MAP_DATA_START_STOP_t;

/** Jitter buffer adaption */
typedef enum _IFX_TAPI_JB_LOCAL_ADAPT
{
   /** Local Adaptation OFF */
   IFX_TAPI_JB_LOCAL_ADAPT_OFF = 0,
   /** Local adaptation ON */
   IFX_TAPI_JB_LOCAL_ADAPT_ON = 1,
   /** Local Adaptation ON
      with Sample Interpollation */
   IFX_TAPI_JB_LOCAL_ADAPT_SI_ON = 2
} IFX_TAPI_JB_LOCAL_ADAPT_t;

/** Enumeration used for ioctl \ref IFX_TAPI_ENC_VAD_CFG_SET. */
typedef enum _IFX_TAPI_ENC_VAD
{
   /** No voice activity detection */
   IFX_TAPI_ENC_VAD_NOVAD = 0,
   /** Voice activity detection on, in this case also comfort noise and
      spectral information (nicer noise) is switched on */
   IFX_TAPI_ENC_VAD_ON = 1,
   /** Voice activity detection on with comfort noise generation without
       spectral information */
   IFX_TAPI_ENC_VAD_G711 = 2,
   /** Voice activity detection on with comfort noise generation without
       silence compression */
   IFX_TAPI_ENC_VAD_CNG_ONLY = 3,
   /** Voice activity detection on with silence compression without
       comfort noise generation */
   IFX_TAPI_ENC_VAD_SC_ONLY = 4
} IFX_TAPI_ENC_VAD_t;

/** Used for IFX_TAPI_PCK_AAL_PROFILE_t in case one coder range.
   The range information is specified as "UUI Codepoint Range" in the
   specification The ATM Forum, AF-VMOA-0145.000 or ITU-T I.366.2 */
typedef enum _IFX_TAPI_PKT_AAL_PROFILE_RANGE
{
   /** One range from 0 to 15 */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_15 = 0,
   /** Range from 0 to 7 for a two range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7 = 1,
   /** Range from 8 to 15 for a two range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15 = 2,
   /** Range from 0 to 3 for a four range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_3 = 3,
   /** Range from 4 to 7 for a four range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7 = 4,
   /** Range from 8 to 11 for a four range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11 = 5,
   /** Range from 12 to 15 for a four range profile entry */
   IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15 = 6
} IFX_TAPI_PKT_AAL_PROFILE_RANGE_t;

/** Jitter buffer types */
typedef enum _IFX_TAPI_JB_TYPE
{
   /** Fix Jitter Buffer */
   IFX_TAPI_JB_TYPE_FIXED   = 0x1,
   /** Adaptive Jitter Buffer */
   IFX_TAPI_JB_TYPE_ADAPTIVE = 0x2
} IFX_TAPI_JB_TYPE_t;

/** Jitter buffer packet adaption */
typedef enum _IFX_TAPI_JB_PKT_ADAPT
{
   /** reserved */
   IFX_TAPI_JB_PKT_ADAPT_RES1 = 0,
   /** reserved */
   IFX_TAPI_JB_PKT_ADAPT_RES2 = 1,
   /** Packet adaption voice */
   IFX_TAPI_JB_PKT_ADAPT_VOICE = 2,
   /** Packet adaption data */
   IFX_TAPI_JB_PKT_ADAPT_DATA = 3
} IFX_TAPI_JB_PKT_ADAPT_t;

/** Enumeration for \ref IFX_TAPI_PKT_RTP_CFG_t event setting */
typedef enum _IFX_TAPI_PKT_EV_OOB
{
   /** Device default setting */
   IFX_TAPI_PKT_EV_OOB_DEFAULT = 0,
   /** No event packets, DTMF inband. */
   IFX_TAPI_PKT_EV_OOB_NO = 1,
   /** Event packets, auto suppression of dtmf tones.*/
   IFX_TAPI_PKT_EV_OOB_ONLY = 2,
   /** Event packets, no auto suppression of dtmf tones. */
   IFX_TAPI_PKT_EV_OOB_ALL = 3
} IFX_TAPI_PKT_EV_OOB_t;

/** Enumeration for \ref IFX_TAPI_PKT_RTP_CFG_t event playout setting */
typedef enum
{
   /** Device default setting */
   IFX_TAPI_PKT_EV_OOBPLAY_DEFAULT = 0,
   /** All RFC2833 event packet coming from the net are played out */
   IFX_TAPI_PKT_EV_OOBPLAY_PLAY = 1,
   /** All RFC2833 event packet coming from the net are discarded */
   IFX_TAPI_PKT_EV_OOBPLAY_MUTE = 2
}IFX_TAPI_PKT_EV_OOBPLAY_t;

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, enumerations (Group TAPI_INTERFACE_MISC)    */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Debug trace levels */
typedef enum _IFX_TAPI_DEBUG_REPORT_SET
{
   /** report off */
   IFX_TAPI_DEBUG_REPORT_SET_OFF = 0,
   /** low level report */
   IFX_TAPI_DEBUG_REPORT_SET_LOW = 1,
   /** normal level report */
   IFX_TAPI_DEBUG_REPORT_SET_NORMAL = 2,
   /** high level report */
   IFX_TAPI_DEBUG_REPORT_SET_HIGH = 3
} IFX_TAPI_DEBUG_REPORT_SET_t;

/** Enumeration for hook status events */
typedef enum _IFX_TAPI_LINE_HOOK_STATUS
{
   /** Hook detected */
   IFX_TAPI_LINE_HOOK_STATUS_HOOK    = 0x01,
   /** Hook flash detected */
   IFX_TAPI_LINE_HOOK_STATUS_FLASH   = 0x02,
   /** Detected hook event is an Offhook */
   IFX_TAPI_LINE_HOOK_STATUS_OFFHOOK = 0x04
} IFX_TAPI_LINE_HOOK_STATUS_t;

/** Enumeration for phone line status information */
typedef enum _IFX_TAPI_LINE_STATUS
{
   /** line is ringing */
   IFX_TAPI_LINE_STATUS_RINGING = 0x1,
   /** ringing finished */
   IFX_TAPI_LINE_STATUS_RINGFINISHED = 0x02,
   /** fax detcted -> is replaced by signal */
   IFX_TAPI_LINE_STATUS_FAX = 0x04,
   /** ground key detected */
   IFX_TAPI_LINE_STATUS_GNDKEY = 0x10,
   /** ground key high detected */
   IFX_TAPI_LINE_STATUS_GNDKEYHIGH = 0x20,
   /** overtemperature detected */
   IFX_TAPI_LINE_STATUS_OTEMP = 0x40,
   /** ground key polarity detected */
   IFX_TAPI_LINE_STATUS_GNDKEYPOL = 0x80,
   /** GR909 result is available and can be queried with the appropriate
       interface */
   IFX_TAPI_LINE_STATUS_GR909RES = 0x100,
   /** caller id received */
   IFX_TAPI_LINE_STATUS_CIDRX    = 0x200,
   /** Event occurs when the line mode may be switched to
       IFX_TAPI_LINE_FEED_NORMAL_LOW to save power */
   IFX_TAPI_LINE_STATUS_FEEDLOWBATT = 0x400
} IFX_TAPI_LINE_STATUS_t;

/** Enumeration for runtime errors */
typedef enum _IFX_TAPI_RUNTIME_ERROR
{
   /** no error */
   IFX_TAPI_RT_ERROR_NONE = 0x0,
   /** ring cadence settings error in cid tx */
   IFX_TAPI_RT_ERROR_RINGCADENCE_CIDTX = 0x1,
   /** No acknowledge during CID sequence */
   IFX_TAPI_RT_ERROR_CIDTX_NOACK = 0x2
} IFX_TAPI_RUNTIME_ERROR_t;

/** Enumeration used for phone capabilities types */
typedef enum _IFX_TAPI_CAP_TYPE
{
   /** vendor */
   IFX_TAPI_CAP_TYPE_VENDOR = 0,
   /** device */
   IFX_TAPI_CAP_TYPE_DEVICE = 1,
   /** port */
   IFX_TAPI_CAP_TYPE_PORT = 2,
   /** codec */
   IFX_TAPI_CAP_TYPE_CODEC = 3,
   /** dsp */
   IFX_TAPI_CAP_TYPE_DSP = 4,
   /** pcm */
   IFX_TAPI_CAP_TYPE_PCM = 5,
   /** codecs */
   IFX_TAPI_CAP_TYPE_CODECS = 6,
   /** phones */
   IFX_TAPI_CAP_TYPE_PHONES = 7,
   /** sigdetect */
   IFX_TAPI_CAP_TYPE_SIGDETECT = 8,
   /** t38 */
   IFX_TAPI_CAP_TYPE_T38 = 9
} IFX_TAPI_CAP_TYPE_t;

/** Lists the ports for the capability list */
typedef enum _IFX_TAPI_CAP_PORT
{
   /** pots */
   IFX_TAPI_CAP_PORT_POTS    = 0,
   /** pstn */
   IFX_TAPI_CAP_PORT_PSTN    = 1,
   /** handset */
   IFX_TAPI_CAP_PORT_HANDSET = 2,
   /** speaker */
   IFX_TAPI_CAP_PORT_SPEAKER = 3
} IFX_TAPI_CAP_PORT_t;

/** Lists the signal detectors for the capability list */
typedef enum _IFX_TAPI_CAP_SIG_DETECT
{
   /** Signal detection for CNG is available */
   IFX_TAPI_CAP_SIG_DETECT_CNG = 0,
   /** Signal detection for CED is available */
   IFX_TAPI_CAP_SIG_DETECT_CED = 1,
   /** Signal detection for DIS is available */
   IFX_TAPI_CAP_SIG_DETECT_DIS = 2,
   /** Signal detection for line power is available */
   IFX_TAPI_CAP_SIG_DETECT_POWER = 3,
   /** Signal detection for CPT is available */
   IFX_TAPI_CAP_SIG_DETECT_CPT = 4,
   /** Signal detection for V8.bis is available */
   IFX_TAPI_CAP_SIG_DETECT_V8BIS = 5
} IFX_TAPI_CAP_SIG_DETECT_t;

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ======================================================================= */
/* TAPI Ringing Services, enumerations (Group TAPI_INTERFACE_RINGING)      */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Ring Configuration Mode */
typedef enum _IFX_TAPI_RING_CFG_MODE
{
   /** Internal balanced */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_BALANCED = 0,
   /** Internal unbalanced ROT */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROT = 1,
   /** Internal unbalanced ROR */
   IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROR = 2,
   /** External SLIC current sense */
   IFX_TAPI_RING_CFG_MODE_EXTERNAL_IT_CS = 3,
   /** External IO current sense */
   IFX_TAPI_RING_CFG_MODE_EXTERNAL_IO_CS = 4
} IFX_TAPI_RING_CFG_MODE_t;

/** Ring Configuration SubMode */
typedef enum _IFX_TAPI_RING_CFG_SUBMODE
{
   /** DC Ring Trip standard */
   IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_STANDARD = 0,
   /** DC Ring Trip fast */
   IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_FAST = 1,
   /** AC Ring Trip standard */
   IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_STANDARD = 2,
   /** AC Ring Trip fast */
   IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_FAST = 3
} IFX_TAPI_RING_CFG_SUBMODE_t;

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ==================================================================== */
/* TAPI PCM Services, enumerations (Group TAPI_INTERFACE_PCM)           */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** PCM Resolutions */
typedef enum _IFX_TAPI_PCM_RES
{
   /** A-Law 8 bit */
   IFX_TAPI_PCM_RES_ALAW_8BIT = 0,
   /** u-Law 8 bit */
   IFX_TAPI_PCM_RES_ULAW_8BIT = 1,
   /** Linear 16 bit */
   IFX_TAPI_PCM_RES_LINEAR_16BIT = 2
} IFX_TAPI_PCM_RES_t;

/*@}*/ /* TAPI_INTERFACE_PCM */

/* =================================================================== */
/* TAPI Fax T.38 Services, enumerations (Group TAPI_INTERFACE_FAX)     */
/* =================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** T38 Fax Datapump states */
typedef enum _IFX_TAPI_FAX_T38_STATUS
{
   /** Fax Datapump not active */
   IFX_TAPI_FAX_T38_DP_OFF = 0x1,
   /** Fax Datapump active */
   IFX_TAPI_FAX_T38_DP_ON  = 0x2,
   /** Fax transmission is active */
   IFX_TAPI_FAX_T38_TX_ON  = 0x4,
   /** Fax transmission is not active */
   IFX_TAPI_FAX_T38_TX_OFF = 0x8
} IFX_TAPI_FAX_T38_STATUS_t;

/** T38 Fax errors */
typedef enum _IFX_TAPI_FAX_T38_ERROR
{
   /** No Error */
   IFX_TAPI_FAX_T38_ERROR_NONE = 0,
   /** Error occured : Deactivate Datapump */
   IFX_TAPI_FAX_T38_ERROR_DATAPUMP = 1,
   /** MIPS Overload */
   IFX_TAPI_FAX_T38_ERROR_MIPS_OVLD = 2,
   /** Érror while reading data */
   IFX_TAPI_FAX_T38_ERROR_READ = 3,
   /** Error while writing data */
   IFX_TAPI_FAX_T38_ERROR_WRITE = 4,
   /** Error while setting up modulator or demodulator */
   IFX_TAPI_FAX_T38_ERROR_SETUP = 5
} IFX_TAPI_FAX_T38_ERROR_t;

/** T38 Fax standards */
typedef enum _IFX_TAPI_T38_STD
{
   /** V.21 */
   IFX_TAPI_T38_STD_V21       = 0x1,
   /** V.27/2400 */
   IFX_TAPI_T38_STD_V27_2400  = 0x2,
   /** V.27/4800 */
   IFX_TAPI_T38_STD_V27_4800  = 0x3,
   /** V.29/7200 */
   IFX_TAPI_T38_STD_V29_7200  = 0x4,
   /** V.29/9600 */
   IFX_TAPI_T38_STD_V29_9600  = 0x5,
   /** V.17/7200 */
   IFX_TAPI_T38_STD_V17_7200  = 0x6,
   /** V.17/9600 */
   IFX_TAPI_T38_STD_V17_9600  = 0x7,
   /** V.17/12000 */
   IFX_TAPI_T38_STD_V17_12000 = 0x8,
   /** V.17/14400 */
   IFX_TAPI_T38_STD_V17_14400 = 0x9
} IFX_TAPI_T38_STD_t;

/*@}*/ /* TAPI_INTERFACE_FAX */

/* ========================================================================= */
/*                      TAPI Interface Structures                            */
/* ========================================================================= */

/* ======================================================================== */
/* TAPI Initialization Services, structures (Group TAPI_INTERFACE_INIT)     */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_INIT */
/*@{*/

/** TAPI initialization structure used for IFX_TAPI_CH_INIT_t */
typedef struct _IFX_TAPI_CH_INIT
{
   /** Application mode selection, VoIp, PCM etc

   - 0: IFX_TAPI_INIT_MODE_DEFAULT, default initialization see device driver
        documentation. This value is used for compatibility reasons
   - 1: IFX_TAPI_INIT_MODE_VOICE_CODER, Phone connected to a packet coder
        (data channel) with DSP features for signal detection
   - 2: IFX_TAPI_INIT_MODE_PCM_DSP, Phone to PCM using DSP features for signal
        detection
   - 3: IFX_TAPI_INIT_MODE_PCM_PHONE, Phone to PCM connection without DSP
        features
   - 255: IFX_TAPI_INIT_MODE_NONE, no configuration is done.
          This is used only for testing */
   unsigned char nMode;
   /** Country selection. For future purposes, not yet used */
   unsigned char nCountry;
   /** Low level Device initialization pointer. Please refer to device driver
   documentation */
   void *pProc;
} IFX_TAPI_CH_INIT_t;

/*@}*/ /* TAPI_INTERFACE_INIT */

/* ======================================================================== */
/* TAPI Operation Control Services, structures (Group TAPI_INTERFACE_OP)    */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_OP */
/*@{*/

/** Phone speaker phone and microphone volume settings used for
    ioctl \ref IFX_TAPI_PHONE_VOLUME_SET */
typedef struct _IFX_TAPI_LINE_VOLUME
{
   /** Volume setting for the receiving path, speakerphone
       The value is given in dB with the range (-24dB ... 24dB) */
   int nGainRx;
   /** Volume setting for the transmitting path, microphone
       The value is given in dB with the range (-24dB ... 24dB) */
   int nGainTx;
} IFX_TAPI_LINE_VOLUME_t;

/** Structure used for validation times of hook, hook flash and pulse dialing */
typedef struct _IFX_TAPI_LINE_HOOK_VT
{
   /** Type of validation time setting */
   IFX_TAPI_LINE_HOOK_VALIDATION_TYPE_t nType;
   /** Minimum time for validation in ms */
   unsigned long         nMinTime;
   /** maximum time for validation in ms */
   unsigned long         nMaxTime;
} IFX_TAPI_LINE_HOOK_VT_t;

/** Line echo canceller (LEC) configuration */
typedef struct _IFX_TAPI_LEC_CFG
{
#if 0
   /** Configure LEC Type

    - 0: IFX_TAPI_LEC_TYPE_OFF
    - 1: IFX_TAPI_LEC_TYPE_NLEC
    - 2: IFX_TAPI_LEC_TYPE_WLEC
   */
   IFX_TAPI_LEC_TYPE_t nType;
#endif /* 0 */
   /** Gain for input or LEC off

    - 0: IFX_TAPI_LEC_GAIN_OFF, LEC Off
    - 1: IFX_TAPI_LEC_GAIN_LOW, Low Gain
    - 2: IFX_TAPI_LEC_GAIN_MEDIUM,  Medium Gain (only supported for VINETIC)
    - 3: IFX_TAPI_LEC_GAIN_HIGH, High Gain    */
   char nGainIn;
   /** Gain for ouput or LEC off

   - 0: IFX_TAPI_LEC_GAIN_OFF, LEC Off
   - 1: IFX_TAPI_LEC_GAIN_LOW, Low Gain
   - 2: IFX_TAPI_LEC_GAIN_MEDIUM, Medium Gain (only supported for VINETIC)
   - 3: IFX_TAPI_LEC_GAIN_HIGH, High Gain    */
   char nGainOut;
   /** LEC tail length in milli seconds. nLen is currently not supported
       and can be set to zero */
   char nLen;
   /** Switch the NLP on or off

   - 0: IFX_TAPI_LEC_NLP_DEFAULT, NLP is default
   - 1: IFX_TAPI_LEC_NLP_ON, NLP is on
   - 2: IFX_TAPI_LEC_NLP_OFF, NLP is off   */
   char bNlp;
} IFX_TAPI_LEC_CFG_t;

/*@}*/ /* TAPI_INTERFACE_OP */

/* ===================================================================== */
/* TAPI Metering Services, structures (Group TAPI_INTERFACE_METER)       */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_METER */
/*@{*/

/** Structure for metering config */
typedef struct _IFX_TAPI_METER_CFG
{
   /** Metering mode

   - 0: IFX_TAPI_METER_MODE_TTX, TTX mode
   - 1: IFX_TAPI_METER_MODE_REVPOL, reverse polarity  */
   unsigned char             mode;
   /** Metering frequency, currently not used */
   unsigned char             freq;
   /** Length of metering burst in ms. burst_len must be greater than zero. */
   unsigned long            burst_len;
   /** Distance between the metering burst in sec */
   unsigned long            burst_dist;
   /** Defines the number of bursts */
   unsigned long            burst_cnt;
} IFX_TAPI_METER_CFG_t;

/*@}*/ /* TAPI_INTERFACE_METER */

/* ==================================================================== */
/* TAPI Tone Control Services, structures (Group TAPI_INTERFACE_TONE)   */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_TONE */
/*@{*/

/** Structure used for tone level configuration */
typedef struct _IFX_TAPI_PREDEF_TONE_LEVEL
{
   /** Defines the tone generator number whose level has to be modified.
   - 0x1: IFX_TAPI_TONE_TG1, generator one is used
   - 0x2: IFX_TAPI_TONE_TG2, generator two is used
   - 0xFF: IFX_TAPI_TONE_TGALL, all tone generator are affected. */
   unsigned char      nGenerator;
   /** Defines the tone level: value 0x100 describes the default record
   level of -38.79 dB. Maximum value is 0x7FFF which corresponds to a
   record level of approximately 3 dB.
   The tone level in dB can be calculated via formula:
   level_in_dB = +3.17 + 20 log10 (level / 32767) */
   unsigned long      nLevel;
} IFX_TAPI_PREDEF_TONE_LEVEL_t;

/** Structure for definition of simple tones */
typedef struct _IFX_TAPI_TONE_SIMPLE
{
    /** Indicate the type of the tone descriptor:
    - 1: IFX_TAPI_TONE_TYPE_SIMPLE, The tone descriptor describe a simple tone
    - 2: IFX_TAPI_TONE_TYPE_COMPOSED, The tone descriptor describes a
         composed tone. */
    unsigned int format;
    /** Name of the simple tone */
    char name[30];
    /** Tone code ID from IFX_TAPI_TONE_INDEX_MIN up to
        max IFX_TAPI_TONE_INDEX_MAX */
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
    unsigned int cadence[IFX_TAPI_TONE_STEPS_MAX];
    /** Active frequencies for the cadence step.
      Each frequencies is a four bit binary value. Each bit of the value
      represent one frequency. All active frequencies are summed together.
      The value for each frequencies can be a combination of the following

      - 0x0:  IFX_TAPI_TONE_FREQNONE, all frequencies are inactive - pause
      - 0x01: IFX_TAPI_TONE_FREQA, play frequency A
      - 0x02: IFX_TAPI_TONE_FREQB, play frequency B
      - 0x04: IFX_TAPI_TONE_FREQC, play frequency C
      - 0x08: IFX_TAPI_TONE_FREQD, play frequency D
      - 0x0F: IFX_TAPI_TONE_FREQALL, play all frequencies */
    unsigned int frequencies[IFX_TAPI_TONE_STEPS_MAX];
    /** Modulation of the frequency A with frequency B for the cadence step

    - 0x0: IFX_TAPI_TONE_MODULATION_OFF, Modulation of frequency A with
           frequency B is disabled
    - 0x1: IFX_TAPI_TONE_MODULATION_ON, Modulation of frequency A with
           frequency B is enabled */
    unsigned int modulation[IFX_TAPI_TONE_STEPS_MAX];
    /** Time for additional pause time between simple tones
    in ms (0 <= ms <= 32000).
    If pause is not equal 0, loop must indicate infinite repetitions (0) or
    at least more than one repetition.
    Pause is the time between each tone sequence. Each secuence is composed
    by several frecuency tones. */
    unsigned int pause;
} IFX_TAPI_TONE_SIMPLE_t;

/** Structure for definition of composed tones */
typedef struct _IFX_TAPI_TONE_COMPOSED
{
    /** Indicate the type of the tone descriptor:
    - 1: IFX_TAPI_TONE_TYPE_SIMPLE, The tone descriptor describe a simple tone
    - 2: IFX_TAPI_TONE_TYPE_COMPOSED, The tone descriptor describes a composed
        tone. */
    unsigned int format;
    /** name of the composed tone */
    char name[30];
    /** tone code ID up to max IFX_TAPI_TONE_INDEX_MAX */
    unsigned int index;
    /** number of times to repeat the composed tone sequence */
    unsigned int loop;
    /** time for voice path to be active (reestablished) in ms
    (0 <= ms <= 64000) */
    unsigned int alternatVoicePath;
    /** total number of simple tones to be played */
    unsigned int count;
    /** simple tone code IDs to be played */
    unsigned int tones [IFX_TAPI_TONE_SIMPLE_MAX];
} IFX_TAPI_TONE_COMPOSED_t;

/** Tone descriptor */
typedef union _IFX_TAPI_TONE
{
   /** Descriptor for simple tone */
   IFX_TAPI_TONE_SIMPLE_t   simple;
   /** Descriptor for composed tone */
   IFX_TAPI_TONE_COMPOSED_t composed;
} IFX_TAPI_TONE_t;

/*@}*/ /* TAPI_INTERFACE_TONE */

/* ======================================================================= */
/* TAPI Dial Services, structures (Group TAPI_INTERFACE_DIAL)              */
/* ======================================================================= */
/** \addtogroup TAPI_INTERFACE_DIAL */
/*@{*/

/*@}*/ /* TAPI_INTERFACE_DIAL */

/* ======================================================================== */
/* TAPI Signal Services, structures (Group TAPI_INTERFACE_SIGNAL)           */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_SIGNAL */
/*@{*/

/** Structure used for enable and disable signal detection. See
   \ref IFX_TAPI_SIG_DETECT_ENABLE and \ref IFX_TAPI_SIG_DETECT_DISABLE
   The signals are reported via an TAPI exception service. The status
   information is queried with \ref IFX_TAPI_CH_STATUS_GET. */
typedef struct _IFX_TAPI_SIG_DETECTION
{
   /** Signals to detect. Can be any combination of \ref IFX_TAPI_SIG_t */
   unsigned long sig;
   /** Signals to detect. Can be any combination of \ref IFX_TAPI_SIG_EXT_t */
   unsigned long sig_ext;
} IFX_TAPI_SIG_DETECTION_t;

/** Structure used for \ref IFX_TAPI_TONE_CPTD_START and
    \ref IFX_TAPI_TONE_CPTD_STOP */
typedef struct _IFX_TAPI_TONE_CPTD
{
   /** The index of the tone to detect. The tone index must be programmed
   with IFX_TAPI_TONE_TABLE_CFG_SET */
   int tone;
   /** The specification of the signal

      - 1: IFX_TAPI_TONE_CPTD_DIRECTION_RX,
           receive direction of the programmed CPT tone
      - 2: IFX_TAPI_TONE_CPTD_DIRECTION_TX,
           transmit direction of the programmed CPT tone */
   int signal;
} IFX_TAPI_TONE_CPTD_t;

/*@}*/ /* TAPI_INTERFACE_SIGNAL */

/* ==================================================================== */
/* TAPI Caller ID Services, structures (Group TAPI_INTERFACE_CID)       */
/* ==================================================================== */
/** \addtogroup TAPI_INTERFACE_CID */
/*@{*/


/** Structure containing the timing for CID transmission. */
typedef struct _IFX_TAPI_CID_TIMING
{
   /** Time to wait before data transmission, in ms.
      Default 300 ms. */
   unsigned int beforeData;
   /** Time to wait after data transmission, in ms, for onhook services.
      Default 300 ms. */
   unsigned int dataOut2restoreTimeOnhook;
   /** Time to wait after data transmission, in ms, for offhook services.
      Default 60 ms. */
   unsigned int dataOut2restoreTimeOffhook;
   /** Time to wait after ack detection, in ms.
      Default 55 ms. */
   unsigned int ack2dataOutTime;
   /** Time-out for ack detection, in ms.
      Default 200 ms. */
   unsigned int cas2ackTime;
   /** Time to wait after ACK time-out, in ms.
      Default 0 ms. */
   unsigned int afterAckTimeout;
   /** Time to wait after first ring, in ms, typically before data transmission.
      Default 600 ms. */
   unsigned int afterFirstRing;
   /** Time to wait after ring pulse, in ms, typically before data transmission.
      Default 500 ms. */
   unsigned int afterRingPulse;
   /** Time to wait after DTAS, in ms, typically before data transmission.
      Default 45 ms. */
   unsigned int afterDTASOnhook;
   /** Time to wait after line reversal in ms, typically before data transmission.
      Default 100 ms. */
   unsigned int afterLineReversal;
   /** Time to wait after OSI signal, in ms.
      Default 300 ms. */
   unsigned int afterOSI;
} IFX_TAPI_CID_TIMING_t;

/** Structure containing the configuration information for FSK transmitter and
   receiver. */
typedef struct _IFX_TAPI_CID_FSK_CFG
{
   /** Signal level for FSK transmission in 0.1 dB. Default -14 dB. */
   int            levelTX;
   /** Minimum signal level for FSK reception in 0.1 dB. Default -15 dB. */
   int            levelRX;
   /** Number of seizure bits for FSK transmit. Default 300 bits. */
   unsigned int   seizureTX;
   /** Minimum number of seizure bits for FSK transmit. Default 300 bits. */
   unsigned int   seizureRX;
   /** Number of mark bits for onhook FSK receive. Default 180 bits. */
   unsigned int   markTXOnhook;
   /** Number of mark bits for offhook FSK receive. Default 80 bits. */
   unsigned int   markTXOffhook;
   /** Minimum number of mark bits for onhook FSK receive. Default 180 bits. */
   unsigned int   markRXOnhook;
   /** Minimum number of mark bits for offhook FSK receive. Default 80 bits. */
   unsigned int   markRXOffhook;
} IFX_TAPI_CID_FSK_CFG_t;

/** Structure containing the configuration information for DTMF CID. */
typedef struct _IFX_TAPI_CID_DTMF_CFG
{
   /** Tone id for starting tone. Default is DTMF 'A'. */
   char           startTone;
   /** Tone id for stop tone. Default is DTMF 'C'. */
   char           stopTone;
   /** Tone id for starting information tone. Default is DTMF 'B'. */
   char           infoStartTone;
   /** Tone id for starting redirection tone. Default is DTMF 'D'. */
   char           redirStartTone;
   /** Time for DTMF digit duration. Default is 50 ms. */
   unsigned int   digitTime;
   /** Time between DTMF digits in ms. Default is 50 ms. */
   unsigned int   interDigitTime;
} IFX_TAPI_CID_DTMF_CFG_t;

/** Structure containing parameters for the reason of absence. */
typedef struct _IFX_TAPI_CID_ABS_REASON
{
   /** length of reason of absence buffer */
   unsigned int    len;
   /** buffer for reason 'unavailable' */
   unsigned char   unavailable[IFX_TAPI_CID_MSG_LEN_MAX];
   /** buffer for reason 'private' */
   unsigned char   priv[IFX_TAPI_CID_MSG_LEN_MAX];
} IFX_TAPI_CID_ABS_REASON_t;

/** Structure containing CID configuration for Telcordia standard. */
typedef struct _IFX_TAPI_CID_STD_TELCORDIA
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t    *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_FSK_CFG_t  *pFSKConf;
   /** Usage of OSI for offhook transmission. Default “no use”. */
   unsigned int         OSIoffhook;
   /** Lenght of the OSI signal in ms. Default 200 ms. */
   unsigned int         OSItime;
   /** Tone table index for the alert tone to be used.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int         nAlertToneOnhook;
   unsigned int         nAlertToneOffhook;
   /** DTMF ACK after CAS, used for offhook transmission. Default DTMF 'D'. */
   char                 ackTone;
} IFX_TAPI_CID_STD_TELCORDIA_t;

/** Structure for the configuration of ETSI FSK standard */
typedef struct _IFX_TAPI_CID_STD_ETSI_FSK
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t       *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_FSK_CFG_t     *pFSKConf;
   /** Type of ETSI Alert of onhook services associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_FR */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertRing;
   /** Type of ETSI Alert of onhook services not associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_RP. */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertNoRing;
   /** Tone table index for the alert tone to be used.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int            nAlertToneOnhook;
   unsigned int            nAlertToneOffhook;
   /** Ring Pulse time interval. Default is 500 ms */
   unsigned int            ringPulseTime;
   /** DTMF ACK after CAS, used for offhook transmission. Default DTMF 'D'. */
   char                    ackTone;
} IFX_TAPI_CID_STD_ETSI_FSK_t;

/** Structure for the configuration of ETSI DTMF standard */
typedef struct _IFX_TAPI_CID_STD_ETSI_DTMF
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t       *pCIDTiming;
   /** Pointer to a structure containing DTMF configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_DTMF_CFG_t     *pDTMFConf;
   /** Pointer to a structure containing DTMF Absence of CLI parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_ABS_REASON_t   *pABSCLICode;
   /** Type of ETSI Alert of onhook services associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_FR */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertRing;
   /** Type of ETSI Alert of onhook services not associated to ringing.
      Default IFX_TAPI_CID_ALERT_ETSI_RP. */
   IFX_TAPI_CID_ALERT_ETSI_t   nETSIAlertNoRing;
   /** Tone table index for the alert tone to be used for OnHook CID.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int                nAlertToneOnhook;
   /** Tone table index for the alert tone to be used for OffHook CID. */
   unsigned int                nAlertToneOffhook;
   /** Ring Pulse time interval. Default is 500 ms */
   unsigned int                ringPulseTime;
   /** DTMF ACK after CAS, used for offhook transmission. Default DTMF 'D'. */
   char                        ackTone;
} IFX_TAPI_CID_STD_ETSI_DTMF_t;

/** Structure for the configuration of SIN standard */
typedef struct _IFX_TAPI_CID_STD_SIN
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t    *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_FSK_CFG_t  *pFSKConf;
   /** Tone table index for the alert tone to be used for Onhook CID Tx.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int          nAlertToneOnhook;
   /** Tone table index for the alert tone to be used for Offhook CID Tx.
       Default XXXXXd. */
   unsigned int          nAlertToneOffhook;
   /** DTMF ACK after CAS, used for offhook transmission. Default DTMF 'D'. */
   char                 ackTone;
} IFX_TAPI_CID_STD_SIN_t;

/** Structure for the configuration of NTT standard */
typedef struct _IFX_TAPI_CID_STD_NTT
{
   /** Pointer to a structure containing timing information.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_TIMING_t   *pCIDTiming;
   /** Pointer to a structure containing FSK configuration parameters.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_FSK_CFG_t     *pFSKConf;
   /** Tone table index for the alert tone to be used.
      Required for automatic CID/MWI generation. Default XXXXXd. */
   unsigned int            nAlertToneOnhook;
   unsigned int            nAlertToneOffhook;
   /** Ring Pulse time interval. Default is 500 ms */
   unsigned int            ringPulseTime;
   /** Number of Ring Pulse Loops in defined time interval. Default is 5 */
   unsigned int            ringPulseLoop;
} IFX_TAPI_CID_STD_NTT_t;

/** Union for CID standards configuration */
typedef union _IFX_TAPI_CID_STD_TYPE
{
   /** Configuration for TELECORDIA */
   IFX_TAPI_CID_STD_TELCORDIA_t   telcordia;
   /** Configuration for ETSI FSK */
   IFX_TAPI_CID_STD_ETSI_FSK_t    etsiFSK;
   /** Configuration for ETSI DTMF */
   IFX_TAPI_CID_STD_ETSI_DTMF_t   etsiDTMF;
   /** Configuration for SIN */
   IFX_TAPI_CID_STD_SIN_t         sin;
   /** Configuration for NTT */
   IFX_TAPI_CID_STD_NTT_t         ntt;
} IFX_TAPI_CID_STD_TYPE_t;

/** Structure containing CID configuration possibilities. */
typedef struct _IFX_TAPI_CID_CFG
{
   /** Standard used. Default IFX_TAPI_CID_STD_TELCORDIA */
   IFX_TAPI_CID_STD_t       nStandard;
   /** Pointer to union with parameters for the different standards.
      If the parameter is NULL, default values will be used. */
   IFX_TAPI_CID_STD_TYPE_t  *cfg;
} IFX_TAPI_CID_CFG_t;

/** Structure for element types (\ref IFX_TAPI_CID_SERVICE_TYPE_t)
   representing a date (like IFX_TAPI_CID_ST_DATE or IFX_TAPI_CID_ST_CDATE)
*/
typedef struct _IFX_TAPI_CID_MSG_DATE
{
   /** element type */
   IFX_TAPI_CID_SERVICE_TYPE_t elementType;
   /** month */
   unsigned int            month;
   /** day */
   unsigned int            day;
   /** hour */
   unsigned int            hour;
   /** mn */
   unsigned int            mn;
} IFX_TAPI_CID_MSG_DATE_t;

/** Structure for element types (\ref IFX_TAPI_CID_SERVICE_TYPE_t)
   with dynamic length (like numbers or names)
*/
typedef struct _IFX_TAPI_CID_MSG_STRING
{
   /** element type */
   IFX_TAPI_CID_SERVICE_TYPE_t elementType;
   /** element length */
   unsigned int            len;
   /** element buffer */
   unsigned char           element[IFX_TAPI_CID_MSG_LEN_MAX];
} IFX_TAPI_CID_MSG_STRING_t;

/** Structure for element types (\ref IFX_TAPI_CID_SERVICE_TYPE_t)
   with one value (length 1)
*/
typedef struct _IFX_TAPI_CID_MSG_VALUE
{
   /** element type */
   IFX_TAPI_CID_SERVICE_TYPE_t elementType;
   /** element */
   unsigned char           element;
} IFX_TAPI_CID_MSG_VALUE_t;

/** Structure for service type transparent (\ref IFX_TAPI_CID_SERVICE_TYPE_t)
*/
typedef struct _IFX_TAPI_CID_MSG_TRANSPARENT
{
   /** element type */
   IFX_TAPI_CID_SERVICE_TYPE_t elementType;
   /** element length */
   unsigned int            len;
   /** element buffer */
   unsigned char           *data;
} IFX_TAPI_CID_MSG_TRANSPARENT_t;

/** Structure for CID elements */
typedef union _IFX_TAPI_CID_MSG_ELEMENT
{
   /** date and time */
   IFX_TAPI_CID_MSG_DATE_t     date;
   /** numbers, name or similar */
   IFX_TAPI_CID_MSG_STRING_t   string;
   /** value */
   IFX_TAPI_CID_MSG_VALUE_t    value;
   /** transparent */
   IFX_TAPI_CID_MSG_TRANSPARENT_t transparent;
} IFX_TAPI_CID_MSG_ELEMENT_t;


/** Structure for CID transmission */
typedef struct _IFX_TAPI_CID_MSG
{
   /** Define the Transmission Mode. Default IFX_TAPI_CID_HM_ONHOOK. */
   IFX_TAPI_CID_HOOK_MODE_t    txMode;
   /** Define the Message Type to be displayed. Default IFX_TAPI_CID_MT_CSUP.*/
   IFX_TAPI_CID_MSG_TYPE_t messageType;
   /** Number of elements of the message array. */
   unsigned int            nMsgElements;
   /** Pointer to message array */
   IFX_TAPI_CID_MSG_ELEMENT_t  *message;
} IFX_TAPI_CID_MSG_t;

/** Structure for caller id receiver status */
typedef struct _IFX_TAPI_CID_RX_STATUS
{
   /** Caller id receiver actual status using \ref IFX_TAPI_CID_RX_STATUS_t

   - 0: IFX_TAPI_CID_RX_STATE_INACTIVE
   - 1: IFX_TAPI_CID_RX_STATE_ACTIVE
   - 2: IFX_TAPI_CID_RX_STATE_ONGOING
   - 3: IFX_TAPI_CID_RX_STATE_DATA_READY
   */
   unsigned char nStatus;
   /** Caller id receiver actual error code using \ref IFX_TAPI_CID_RX_ERROR_t

   - 0: IFX_TAPI_CID_RX_ERROR_NONE
   - 1: IFX_TAPI_CID_RX_ERROR_READ
   */
   unsigned char nError;
} IFX_TAPI_CID_RX_STATUS_t;

/** structure for caller id receiver data */
typedef struct _IFX_TAPI_CID_RX_DATA
{
   /** Caller id receiver data */
   unsigned char data [IFX_TAPI_CID_SIZE_MAX];
   /** Caller id receiver datasize in bytes */
   unsigned int nSize;
} IFX_TAPI_CID_RX_DATA_t;

/*@}*/ /* TAPI_INTERFACE_CID */

/* ======================================================================== */
/* TAPI Connection Services, structures (Group TAPI_INTERFACE_CON)          */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_CON */
/*@{*/

/** Structure for RTP Configuration     */
typedef struct _IFX_TAPI_PKT_RTP_CFG
{
   /** Start value for the sequence nr  */
   unsigned short nSeqNr;
   /** Synchronization source value for the voice and SID packets */
   unsigned long nSsrc;
   /** Start value for the timestamp (resolution of 125 µs) */
   unsigned long nTimestamp;
   /** Defines how events are transmitted

   - 0: IFX_TAPI_PKT_EV_OOB_DEFAULT, Default
   - 1: IFX_TAPI_PKT_EV_OOB_NO,
        No event packets, DTMF inband.
   - 2: IFX_TAPI_PKT_EV_OOB_ONLY,
        Event packets, auto suppression of dtmf tones.
   - 3: IFX_TAPI_PKT_EV_OOB_ALL,
        Event packets, no auto suppression of dtmf tones. */
   unsigned char  nEvents;
   /** Defines how received RTC event packets are treated

   - 0: IFX_TAPI_PKT_EV_OOBPLAY_DEFAULT, Default
   - 1: IFX_TAPI_PKT_EV_OOBPLAY_PLAY, Play out RFC2833 packets coming from the net
   - 2: IFX_TAPI_PKT_EV_OOBPLAY_MUTE, Mute RFC2833 packets coming from the net */
   unsigned char  nPlayEvents;

   /** Event payload type */
   unsigned char  nEventPT;
} IFX_TAPI_PKT_RTP_CFG_t;

/** Structure for RTP payload configuration */
typedef struct _IFX_TAPI_PKT_RTP_PT_CFG
{
   /** Table with all payload types for the upstream direction,
      the coder type \ref IFX_TAPI_ENC_TYPE_t is used as index. */
   unsigned char nPTup [IFX_TAPI_ENC_TYPE_MAX];
   /** Table with all payload types for the downstream direction,
       to use in case of dynamic payload type configuration.
       the coder type \ref IFX_TAPI_ENC_TYPE_t is used as index. */
   unsigned char nPTdown [IFX_TAPI_ENC_TYPE_MAX];
} IFX_TAPI_PKT_RTP_PT_CFG_t;

/** Structure for RTCP Statistics. It refers to the RFC3550/3551 */
typedef struct _IFX_TAPI_PKT_RTCP_STATISTICS
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
} IFX_TAPI_PKT_RTCP_STATISTICS_t;

/** Structure for jitter buffer configuration used by \ref IFX_TAPI_JB_CFG_SET
   \remarks This structure may be changed in the future */
typedef struct _IFX_TAPI_JB_CFG
{
   /** jitter buffer type

      - 1: fixed
      - 2: adaptive (default) */
   unsigned char    nJbType;
   /** Packet length adaption

   - 0: IFX_TAPI_JB_PKT_ADAPT_RES1, The jitter buffer size depends on the
     estimated network jitter. No longer supported
   - 1: IFX_TAPI_JB_PKT_ADAPT_RES2, The jitter buffer size depends on the
     Packet Length. In this case the jitter buffer size is packet length
     multiplied with nScaling. No longer supported
   - 2: IFX_TAPI_JB_PKT_ADAPT_VOICE, Packet adoption is optimized for voice.
   Reduced adjustment
   speed and packet repetition is off.
   - 3: IFX_TAPI_JB_PKT_ADAPT_DATA, Packet adoption is optimized for data
   (modem or fax inband).
   Reduced adjustment speed and packet repetition is on.   */
   char     nPckAdpt;
   /** Local adoption. Only of relevance if nJbType = 2

   - 0: IFX_TAPI_JB_LOCAL_ADAPT_OFF, Local adaptation off.
   Speech gaps which are detected by the far end side are used for the jitter
   buffer adaptations
   - 1: IFX_TAPI_JB_LOCAL_ADAPT_ON, Local adaptation on.
   Jitter buffer adaptation via local and far end speech detection. This means
   that the jitter buffer adaptation doesn't need the far end silence
   compression feature but will use it if the far end side has detected a
   silence period. Thus a jitter buffer adaptation can occur when the far end
   side has detected silence or when a speech gap was detected in the
   downstream direction.
   - 2: IFX_TAPI_JB_LOCAL_ADAPT_SI_ON, Local adaptation on with sample
   interpolation. A jitter buffer adaptation can be done via sample
   interpolation. The advantage is that not a whole frame has to be
   interpolated or discarded. Thus no major distortion should be heard. */
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
} IFX_TAPI_JB_CFG_t;

/** Structure for Jitter Buffer statistics used by
    ioctl \ref IFX_TAPI_JB_STATISTICS_GET */
typedef struct _IFX_TAPI_JB_STATISTICS
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
   /** number of received bytes high part including event packets */
   unsigned long  nRecBytesH;
   /** number of received bytes low part including event packets */
   unsigned long  nRecBytesL;
} IFX_TAPI_JB_STATISTICS_t;

/** structure used for ioctl IFX_TAPI_PKT_AAL_CFG_SET */
typedef struct _IFX_TAPI_PCK_AAL_CFG
{
   /** Connection identifier */
   unsigned short nCid;
   /** Start value for the timestamp (resolution of 125 µs) */
   unsigned short nTimestamp;
   /** Connection Identifier for CPS events */
   unsigned short nCpsCid;
} IFX_TAPI_PCK_AAL_CFG_t;

/** AAL profile setup structure used for \ref IFX_TAPI_PKT_AAL_PROFILE_SET */
typedef struct _IFX_TAPI_PCK_AAL_PROFILE
{
   /** Amount of rows to program */
   char rows;
   /** Length of packet in bytes - 1 */
   char len[10];
   /** UUI codepoint range indicator, see \ref IFX_TAPI_PKT_AAL_PROFILE_RANGE_t

      - 0: IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_15, UUI codepoint range 0 -15
      - 1: IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_7, UUI codepoint range 0 -7
      - 2: IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_15, UUI codepoint range 8 -15
      - 3: IFX_TAPI_PKT_AAL_PROFILE_RANGE_0_3, UUI codepoint range 0 - 3
      - 4: IFX_TAPI_PKT_AAL_PROFILE_RANGE_4_7, UUI codepoint range 4 - 7
      - 5: IFX_TAPI_PKT_AAL_PROFILE_RANGE_8_11, UUI codepoint range 8 - 11
      - 6: IFX_TAPI_PKT_AAL_PROFILE_RANGE_12_15, UUI codepoint range 12 - 15 */
   char nUUI[10];
   /** Codec as listed for \ref IFX_TAPI_ENC_TYPE_t  */
   char codec[10];
} IFX_TAPI_PCK_AAL_PROFILE_t;

/** Data channel mapping structure used for \ref IFX_TAPI_MAP_DATA_ADD and
\ref IFX_TAPI_MAP_DATA_REMOVE */
typedef struct _IFX_TAPI_MAP_DATA
{
   /** Phone channel or PCM channel number to which this channel should be
   mapped. Channel numbers start from 0. */
   unsigned char                             nDstCh;
   /** Type of the destination channel.

   - 0: IFX_TAPI_MAP_TYPE_DEFAULT, Default selected (phone channel)
   - 1: IFX_TAPI_MAP_TYPE_CODER, not supported
   - 2: IFX_TAPI_MAP_TYPE_PCM, type is PCM
   - 3: IFX_TAPI_MAP_TYPE_PHONE, type is phone channel  */
   IFX_TAPI_MAP_DATA_TYPE_t           nChType;
   /** Enables or disables the recording service or leaves as it is

   - 0: IFX_TAPI_MAP_DATA_UNCHANGED, not modify the status of the recorder
   - 1: IFX_TAPI_MAP_DATA_START, Recording is started,
        same as \ref IFX_TAPI_ENC_START
   - 2: IFX_TAPI_MAP_DATA_STOP, Recording is stopped,
        same as \ref IFX_TAPI_ENC_STOP */
   IFX_TAPI_MAP_DATA_START_STOP_t              nRecStart;
   /** Enables or disables the play service or leaves as it is

   - 0: IFX_TAPI_MAP_DATA_UNCHANGED, Do not modify the status of the recorder
   - 1: IFX_TAPI_MAP_DATA_START, Playing is started,
        same as \ref IFX_TAPI_DEC_START
   - 2: IFX_TAPI_MAP_DATA_STOP, Playing is stopped,
        same as \ref IFX_TAPI_DEC_STOP */
   IFX_TAPI_MAP_DATA_START_STOP_t              nPlayStart;
} IFX_TAPI_MAP_DATA_t;

/** Phone channel mapping structure used for \ref IFX_TAPI_MAP_PHONE_ADD and
\ref IFX_TAPI_MAP_PHONE_REMOVE */
typedef struct _IFX_TAPI_MAP_PHONE
{
   /** Phone channel or PCM channel number to which this channel should be
   mapped. Channel numbers start from 0. */
   unsigned char                             nPhoneCh;
   /** Type of the destination channel.

   - 0: IFX_TAPI_MAP_TYPE_DEFAULT, Default selected (phone channel)
   - 1: IFX_TAPI_MAP_TYPE_CODER, not supported
   - 2: IFX_TAPI_MAP_TYPE_PCM, type is PCM
   - 3: IFX_TAPI_MAP_TYPE_PHONE, type is phone channel  */
   IFX_TAPI_MAP_TYPE_t           nChType;
} IFX_TAPI_MAP_PHONE_t;

/** Phone channel mapping structure used for \ref IFX_TAPI_MAP_PCM_ADD and
\ref IFX_TAPI_MAP_PCM_REMOVE */
typedef struct _IFX_TAPI_MAP_PCM
{
   /** Phone channel or PCM channel number to which this channel should be
   mapped. Channel numbers start from 0. */
   unsigned char                             nDstCh;
   /** Type of the destination channel.

   - 0: IFX_TAPI_MAP_TYPE_DEFAULT, Default selected (phone channel)
   - 1: IFX_TAPI_MAP_TYPE_CODER, not supported
   - 2: IFX_TAPI_MAP_TYPE_PCM, type is PCM
   - 3: IFX_TAPI_MAP_TYPE_PHONE, type is phone channel  */
   IFX_TAPI_MAP_TYPE_t           nChType;
} IFX_TAPI_MAP_PCM_t;

/*@}*/ /* TAPI_INTERFACE_CON */

/* ======================================================================== */
/* TAPI Miscellaneous Services, structures (Group TAPI_INTERFACE_MISC)      */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_MISC */
/*@{*/

/** Capability structure.
   \remarks
   The capabilities ioctls can inform you of the capabilities of each phone
   device installed in your system.  The PHONECTL_CAPABILITIES ioctl
   returns an integer value indicating the number of capabilities the
   device has.  The PHONECTL_CAPABILITIES_LIST will fill an array of
   capability structs with all of it's capabilities.  The
   PHONECTL_CAPABILITIES_CHECK takes a single capability struct and returns
   a TRUE if the device has that capability, otherwise it returns FALSE.
 */
typedef struct _IFX_TAPI_CAP
{
   /** Description of the capability */
   char desc[80];
   /** Defines the capability type, see \ref IFX_TAPI_CAP_TYPE_t.

   - 0:  IFX_TAPI_CAP_TYPE_VENDOR, string representation of the vendor
   - 1:  IFX_TAPI_CAP_TYPE_DEVICE, string representation of the underlying
         device
   - 2:  IFX_TAPI_CAP_TYPE_PORT, gives information about the available ports,
         see \ref IFX_TAPI_CAP_PORT_t for port types
   - 3:  IFX_TAPI_CAP_TYPE_CODEC, gives information about the available codecs
         as listed in \ref IFX_TAPI_ENC_TYPE_t
   - 4:  IFX_TAPI_CAP_TYPE_DSP, 1 if DSP is available, otherwise 0
   - 5:  IFX_TAPI_CAP_TYPE_PCM, cap defines how many pcm channels are available
   - 6:  IFX_TAPI_CAP_TYPE_CODECS, cap defines how many codec channels are
         available
   - 7:  IFX_TAPI_CAP_TYPE_PHONES, cap defines how many phone channels are
         available
   - 8:  IFX_TAPI_CAP_TYPE_SIGDETECT, signal detectors out of
         \ref IFX_TAPI_CAP_TYPE_SIGDETECT
   - 9:  T.38 stack support.*/
   IFX_TAPI_CAP_TYPE_t captype;
   /** Defines if, what or how many is available. The definition of cap
       depends on the type. See captype */
   int cap;
   /** The number of this capability */
   int handle;
} IFX_TAPI_CAP_t;

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
/** Exception bits */
typedef struct _IFX_TAPI_EXCEPTION_BITS
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
   /* to get with IFX_TAPI_CH_STATUS_t in signal */
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
       IFX_TAPI_LINE_FEED_NORMAL_LOW to save power */
   unsigned int feedLowBatt:1;
   unsigned int reserved2:7;
} IFX_TAPI_EXCEPTION_BITS_t;
#elif (__BYTE_ORDER == __BIG_ENDIAN)
/** Exception bits */
typedef struct _IFX_TAPI_EXCEPTION_BITS
{
   unsigned int reserved2:7;
   /** Event occurs when the line mode may be switched to
       IFX_TAPI_LINE_FEED_NORMAL_LOW to save power */
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
       \ref IFX_TAPI_CH_STATUS_GET */
   unsigned int signal:1;
   /** Overtemperature detected */
   unsigned int otemp:1;
   /** Ground key polarity detected */
   unsigned int ground_key_polarity:1;
   /** Ground key high detected */
   unsigned int ground_key_high:1;
   /** Fault condition detected, query information via
       \ref IFX_TAPI_CH_STATUS_GET */
   unsigned int fault:1;
   /** FAX status update, query information via \ref IFX_TAPI_CH_STATUS_GET */
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
   with IFX_TAPI_CID_RX_STATUS_GET */
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
} IFX_TAPI_EXCEPTION_BITS_t;
#else
#error Please define endian mode
#endif

/** Contains TAPI exception information */
typedef union _IFX_TAPI_EXCEPTION
{
   /** Specifies the exception */
   IFX_TAPI_EXCEPTION_BITS_t Bits;
   /** Contains the complete status */
   unsigned long             Status;
} IFX_TAPI_EXCEPTION_t;

/** Structure used for IFX_TAPI_CH_STATUS_GET to query the phone status of one
    or more channels    */
typedef struct _IFX_TAPI_CH_STATUS
{
   /** Size of the list. If 0 only the status of this channel is queried. If
       channels not equal to 0 all channels starting with 0 till the value
       of channels are retrieved.
       As the return value it defines the channel number */
   unsigned char channels;
   /** Lists the hook status events

   - 0x01: IFX_TAPI_LINE_HOOK_STATUS_HOOK, Hook detected
   - 0x02: IFX_TAPI_LINE_HOOK_STATUS_FLASH, Hook flash detected
   - 0x04: IFX_TAPI_LINE_HOOK_STATUS_OFFHOOK, Detected hook event is an
           off hook.
           May be set additionally to IFX_TAPI_LINE_HOOK_STATUS_HOOK */
   unsigned char hook;
   /** Lists the dial status events */
   unsigned char dialing;
   /** Contains the digit in case of an dial event */
   unsigned char digit;
   /** Signals the line status and fault conditions

      - 0x1: IFX_TAPI_LINE_STATUS_RINGING, PSTN line is ringing
      - 0x2: IFX_TAPI_LINE_STATUS_RINGFINISHED, ringing finished on PSTN line
      - 0x4: IFX_TAPI_LINE_STATUS_FAX, FAX status change
      - 0x10: IFX_TAPI_LINE_STATUS_GNDKEY, Ground key detected
      - 0x20: IFX_TAPI_LINE_STATUS_GNDKEYHIGH, Ground key high detected
      - 0x40: IFX_TAPI_LINE_STATUS_OTEMP, Overtemperature detected
      - 0x80: IFX_TAPI_LINE_STATUS_GNDKEYPOL, Ground key polarity detected
      - 0x100: IFX_TAPI_LINE_STATUS_GR909RES,
      - 0x200: IFX_TAPI_LINE_STATUS_CIDRX, Cid Receiver event occured
      - 0x400: IFX_TAPI_LINE_STATUS_FEEDLOWBATT, Event occurs when the line mode
        may be switched to IFX_TAPI_LINE_FEED_NORMAL_LOW to save power */
   unsigned int line;
   /** Signals the detection of events.
      The signals/events detected are reported in this and the signal_ext
      field. The signals here are represented according to the definition of
      \ref IFX_TAPI_SIG_t. The following description is taken from the
      definition of \ref IFX_TAPI_SIG_t. Please look there for further
      information.

      - 0x0: IFX_TAPI_SIG_NONE,     no signal detected
      - 0x1: IFX_TAPI_SIG_DISRX, V.21 Preamble Fax Tone, Digital
             Identification Signal (DIS), receive path
      - 0x2: IFX_TAPI_SIG_DISTX, V.21 Preamble Fax Tone,
             Digital Identification Signal (DIS), transmit path
      - 0x4: IFX_TAPI_SIG_DIS, V.21 Preamble Fax Tone in all path,
         Digital Identification Signal (DIS)
      - 0x8: IFX_TAPI_SIG_CEDRX, V.25 2100 Hz (CED) Modem/Fax Tone,
             receive path
      - 0x10: IFX_TAPI_SIG_CEDTX, V.25 2100 Hz (CED) Modem/Fax Tone,
              transmit path
      - 0x20: IFX_TAPI_SIG_CED, V.25 2100 Hz (CED) Modem/Fax Tone in
              all paths
      - 0x40: IFX_TAPI_SIG_CNGFAXRX, CNG Fax Calling Tone (1100 Hz)
              receive path
      - 0x80: IFX_TAPI_SIG_CNGFAXTX, CNG Fax Calling Tone (1100 Hz)
              transmit path
      - 0x100: IFX_TAPI_SIG_CNGFAX, CNG Fax Calling Tone (1100 Hz) in
               all paths
      - 0x200: IFX_TAPI_SIG_CNGMODRX, CNG Modem Calling Tone (1300 Hz)
         receive path
      - 0x400: IFX_TAPI_SIG_CNGMODTX, CNG Modem Calling Tone (1300 Hz)
         transmit path
      - 0x800: IFX_TAPI_SIG_CNGMOD, CNG Modem Calling Tone (1300 Hz) in all
               paths
      - 0x1000: IFX_TAPI_SIG_PHASEREVRX, Phase reversal detection
                receive path
      - 0x2000: IFX_TAPI_SIG_PHASEREVTX, Phase reversal detection
                transmit path
      - 0x4000: IFX_TAPI_SIG_PHASEREV, Phase reversal detection in all paths
      - 0x8000: IFX_TAPI_SIG_AMRX, Amplitude modulation receive path
      - 0x10000: IFX_TAPI_SIG_AMTX, Amplitude modulation transmit path
      - 0x20000: IFX_TAPI_SIG_AM, Amplitude modulation.
      - 0x40000: IFX_TAPI_SIG_TONEHOLDING_ENDRX, Modem tone holding signal
         stopped receive path
      - 0x80000: IFX_TAPI_SIG_TONEHOLDING_ENDTX, Modem tone holding signal
         stopped transmit path
      - 0x100000: IFX_TAPI_SIG_TONEHOLDING_END, Modem tone holding signal
         stopped all paths
      - 0x200000: IFX_TAPI_SIG_CEDENDRX, End of signal CED detection
                  receive path
      - 0x400000: IFX_TAPI_SIG_CEDENDTX, End of signal CED detection
                  transmit path
      - 0x800000: IFX_TAPI_SIG_CEDEND, End of signal CED detection.
      - 0x1000000: IFX_TAPI_SIG_CPTD, Call progress tone detected.
      - 0x8000000: IFX_TAPI_SIG_CIDENDTX, Caller ID transmission finished.
      - 0x10000000: IFX_TAPI_SIG_DTMFTX Enables DTMF reception on locally connected analog line
      - 0x20000000: IFX_TAPI_SIG_DTMFRX, Enables DTMF reception on remote connected line  */
   unsigned long signal;

   /** Signals the detection of events.
      The signals/events detected are reported in this and the signal
      field. The signals here are represented according to the definition of
      \ref IFX_TAPI_SIG_EXT_t. The following description is taken from the
      definition of \ref IFX_TAPI_SIG_EXT_t. Please look there for further
      information.

      - 0x0: IFX_TAPI_SIG_EXT_NONE,     no signal detected
      - 0x1: IFX_TAPI_SIG_EXT_V21LRX,
             980 Hz single tone (V.21L mark sequence) receive path
      - 0x2: IFX_TAPI_SIG_EXT_V21LTX,
             980 Hz single tone (V.21L mark sequence) transmit path
      - 0x4: IFX_TAPI_SIG_EXT_V21L,
             980 Hz single tone (V.21L mark sequence) all paths
      - 0x8: IFX_TAPI_SIG_EXT_V18ARX,
             1400 Hz single tone (V.18A mark sequence) receive path
      - 0x10: IFX_TAPI_SIG_EXT_V18ATX,
             1400 Hz single tone (V.18A mark sequence) transmit path
      - 0x20: IFX_TAPI_SIG_EXT_V18A,
             1400 Hz single tone (V.18A mark sequence) all paths
      - 0x40: IFX_TAPI_SIG_EXT_V27RX,
             1800 Hz single tone (V.27, V.32 carrier) receive path
      - 0x80: IFX_TAPI_SIG_EXT_V27TX,
             1800 Hz single tone (V.27, V.32 carrier) transmit path
      - 0x100: IFX_TAPI_SIG_EXT_V27,
             1800 Hz single tone (V.27, V.32 carrier) all paths
      - 0x200: IFX_TAPI_SIG_EXT_BELLRX,
             2225 Hz single tone (Bell answering tone) receive path
      - 0x400: IFX_TAPI_SIG_EXT_BELLTX,
             2225 Hz single tone (Bell answering tone) transmit path
      - 0x800: IFX_TAPI_SIG_EXT_BELL,
             2225 Hz single tone (Bell answering tone) all paths
      - 0x1000: IFX_TAPI_SIG_EXT_V22RX,
             2250 Hz single tone (V.22 unscrambled binary ones) receive path
      - 0x2000: IFX_TAPI_SIG_EXT_V22TX,
             2250 Hz single tone (V.22 unscrambled binary ones) transmit path
      - 0x4000: IFX_TAPI_SIG_EXT_V22,
             2250 Hz single tone (V.22 unscrambled binary ones) all paths
      - 0x8000: IFX_TAPI_SIG_EXT_V22ORBELLRX, 2225 Hz or 2250 Hz single tone,
             not possible to distinguish
      - 0x10000: IFX_TAPI_SIG_EXT_V22ORBELLTX, 2225 Hz or 2250 Hz single tone,
             not possible to distinguish
      - 0x20000: IFX_TAPI_SIG_EXT_V22ORBELL, 2225 Hz or 2250 Hz single tone,
             not possible to distinguish all paths
      - 0x40000: IFX_TAPI_SIG_EXT_V32ACRX,
             600 Hz + 300 Hz dual tone (V.32 AC) receive path
      - 0x80000: IFX_TAPI_SIG_EXT_V32ACTX,
             600 Hz + 300 Hz dual tone (V.32 AC) transmit path
      - 0x100000: IFX_TAPI_SIG_EXT_V32AC,
             600 Hz + 300 Hz dual tone (V.32 AC) all paths   */
   unsigned long signal_ext;
   /** device specific status information */
   unsigned long device;
   /** RFC 2833 event playout information. This field contains the last event,
       that was received from network side.

      - 0:  IFX_TAPI_PKT_EV_NUM_DTMF_0,
            RFC2833 Event number for DTMF tone #0
      - 1:  IFX_TAPI_PKT_EV_NUM_DTMF_1,
            RFC2833 Event number for DTMF tone #1
      - 2:  IFX_TAPI_PKT_EV_NUM_DTMF_2,
            RFC2833 Event number for DTMF tone #2
      - 3:  IFX_TAPI_PKT_EV_NUM_DTMF_3,
            RFC2833 Event number for DTMF tone #3
      - 4:  IFX_TAPI_PKT_EV_NUM_DTMF_4,
            RFC2833 Event number for DTMF tone #4
      - 5:  IFX_TAPI_PKT_EV_NUM_DTMF_5,
            RFC2833 Event number for DTMF tone #5
      - 6:  IFX_TAPI_PKT_EV_NUM_DTMF_6,
            RFC2833 Event number for DTMF tone #6
      - 7:  IFX_TAPI_PKT_EV_NUM_DTMF_7,
            RFC2833 Event number for DTMF tone #7
      - 8:  IFX_TAPI_PKT_EV_NUM_DTMF_8,
            RFC2833 Event number for DTMF tone #8
      - 9:  IFX_TAPI_PKT_EV_NUM_DTMF_9,
            RFC2833 Event number for DTMF tone #9
      - 10: IFX_TAPI_PKT_EV_NUM_DTMF_STAR, RFC2833 Event number for DTMF
            tone STAR
      - 11: IFX_TAPI_PKT_EV_NUM_DTMF_HASH, RFC2833 Event number for DTMF
            tone HASH
      - 32: IFX_TAPI_PKT_EV_NUM_ANS, RFC2833 Event number for ANS tone
      - 33: IFX_TAPI_PKT_EV_NUM_NANS, RFC2833 Event number for /ANS tone
      - 34: IFX_TAPI_PKT_EV_NUM_ANSAM, RFC2833 Event number for ANSam tone
      - 35: IFX_TAPI_PKT_EV_NUM_NANSAM, RFC2833 Event number for /ANSam tone
      - 36: IFX_TAPI_PKT_EV_NUM_CNG, RFC2833 Event number for CNG tone
      - 54: IFX_TAPI_PKT_EV_NUM_DIS, RFC2833 Event number for DIS tone */
   unsigned long event;
   /** Set of runtime errors which happens during the processing of a
       tapi command.

       - 0x0: IFX_TAPI_RT_ERROR_NONE, No runtime error
       - 0x1: IFX_TAPI_RT_ERROR_RINGCADENCE_CIDTX, Ring cadence cofiguration
              error for CID transmission
       - 0x2: IFX_TAPI_RT_ERROR_CIDTX_NOACK, No acknowledge during CID sequence.
   */
   unsigned long error;
} IFX_TAPI_CH_STATUS_t;

/** Structure used for the TAPI version support check */
typedef struct _IFX_TAPI_VERSION
{
   /** major version number supported */
   unsigned char major;
   /** minor version number supported */
   unsigned char minor;
} IFX_TAPI_VERSION_t;

/*@}*/ /* TAPI_INTERFACE_MISC */

/* ===================================================================== */
/* TAPI Ringing Services, structures (Group TAPI_INTERFACE_RINGING)      */
/* ===================================================================== */
/** \addtogroup TAPI_INTERFACE_RINGING */
/*@{*/

/** Structure for ring cadence used in \ref IFX_TAPI_RING_CADENCE_HR_SET  */
typedef struct _IFX_TAPI_RING_CADENCE
{
   /** Pointer to data bytes which contain the encoded cadence sequence.
   One bit represents ring cadence voltage for 50 ms. A maximum of
   40 bytes (320 bits) are allowed.  */
   char      data[IFX_TAPI_RING_CADENCE_MAX_BYTES];
   /** Number of data bits of cadence sequence. A maximum number of
   320 data bits is possible which corresponds to a maximum cadence
   duration of 16 seconds.  */
   int       nr;
   /** Pointer to data bytes which contain the encoded cadence sequence
   for a first non periodic initial ringing. One bit represents ring cadence
   voltage for 50ms. A maximum of 40 bytes (320 bits) are allowed. */
   char      initial [IFX_TAPI_RING_CADENCE_MAX_BYTES];
   /** Number of data bits of cadence sequence. A maximum number of 320 data
   bits is possible which corresponds to a maximum cadence duration
   of 16 seconds. */
   int       initialNr;
} IFX_TAPI_RING_CADENCE_t;

/** Ringing configuration structure used for
    ioctl \ref IFX_TAPI_RING_CFG_SET */
typedef struct _IFX_TAPI_RING_CFG
{
   /** Configures the ringing mode

   - 0: IFX_TAPI_RING_CFG_MODE_INTERNAL_BALANCED, internal balanced
   - 1: IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROT, internal unbalanced ROT
   - 2: IFX_TAPI_RING_CFG_MODE_INTERNAL_UNBALANCED_ROR, internal unbalanced ROR
   - 3: IFX_TAPI_RING_CFG_MODE_EXTERNAL_IT_CS, external SLIC current sense
   - 4: IFX_TAPI_RING_CFG_MODE_EXTERNAL_IO_CS, external IO current sense */
   unsigned char       nMode;
   /** Configures the ringing submode

   - 0: IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_STANDARD, DC Ring Trip standard
   - 1: IFX_TAPI_RING_CFG_SUBMODE_DC_RNG_TRIP_FAST, DC Ring Trip fast
   - 2: IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_STANDARD, AC Ring Trip standard
   - 3: IFX_TAPI_RING_CFG_SUBMODE_AC_RNG_TRIP_FAST, AC Ring Trip fast */
   unsigned char       nSubmode;
} IFX_TAPI_RING_CFG_t;

/*@}*/ /* TAPI_INTERFACE_RINGING */

/* ============================================================ */
/* TAPI PCM Services, structures (Group TAPI_INTERFACE_PCM)     */
/* ============================================================ */
/** \addtogroup TAPI_INTERFACE_PCM */
/*@{*/

/** Structure for PCM configuration  */
typedef struct _IFX_TAPI_PCM_CFG
{
   /** PCM timeslot for the receive direction */
   unsigned long             nTimeslotRX;
   /** PCM timeslot for the transmit direction */
   unsigned long             nTimeslotTX;
   /** Defines the PCM highway number which is connected to the channel */
   unsigned long             nHighway;
   /** Defines the PCM interface coding

   - 0: IFX_TAPI_PCM_RES_ALAW_8BIT, 8 bit A law
   - 1: IFX_TAPI_PCM_RES_ULAW_8BIT, 8 bit u Law
   - 2: IFX_TAPI_PCM_RES_LINEAR_16BIT, 16 bit linear */
   unsigned long             nResolution;
   /** Defines the PCM sample rate in kHz. */
   unsigned long             nRate;
} IFX_TAPI_PCM_CFG_t;

/*@}*/ /* TAPI_INTERFACE_PCM */

/* ======================================================================== */
/* TAPI Fax T.38 Services, structures (Group TAPI_INTERFACE_FAX)            */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_FAX */
/*@{*/

/** Structure to setup the modulator for T.38 fax and used for
    \ref IFX_TAPI_T38_MOD_START*/
typedef struct _IFX_TAPI_T38_MOD_DATA
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
} IFX_TAPI_T38_MOD_DATA_t;

/** Structure to setup the demodulator for T.38 fax and used for
    \ref IFX_TAPI_T38_DEMOD_START */
typedef struct _IFX_TAPI_T38_DEMOD_DATA
{
   /** Selects the standard used for Fax T.38 using \ref IFX_TAPI_T38_STD_t

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
       \ref IFX_TAPI_T38_STD_t

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
} IFX_TAPI_T38_DEMOD_DATA_t;

/** Structure to read the T.38 fax status and used for
    \ref IFX_TAPI_T38_STATUS_GET */
typedef struct _IFX_TAPI_T38_STATUS
{
   /** T.38 fax status, refer to \ref IFX_TAPI_FAX_T38_STATUS_t

   - 0x01: Data pump is not active
   - 0x02: Data pump is active
   - 0x04: Transmission is active
   - 0x08: Transmission is finished
   */
   unsigned char nStatus;
   /** T.38 fax error, refer to \ref IFX_TAPI_FAX_T38_ERROR_t

   - 0x00: No error occurred
   - 0x01: FAX error occurred, the FAX data pump should be deactivated
   - 0x02: MIPS overload
   - 0x03: Error while reading data
   - 0x04: Error while writing data
   - 0x05: Error while setting up the modulator or demodulator
   */
   unsigned char nError;
} IFX_TAPI_T38_STATUS_t;

/*@}*/ /* TAPI_INTERFACE_FAX */

/* ======================================================================== */
/* TAPI Test Services, structures (Group TAPI_INTERFACE_TEST)               */
/* ======================================================================== */
/** \addtogroup TAPI_INTERFACE_TEST */
/*@{*/

/** structure for switching loops for testing */
typedef struct _IFX_TAPI_TEST_LOOP
{
   /** Switch an analog loop in the device. If switched on, signals that are
      played to the subscriber are looped back to the receiving side.
      - 0x0: Analog loop off
      - 0x1: Analog loop on */
   unsigned char bAnalog;
} IFX_TAPI_TEST_LOOP_t;

/*@}*/ /* TAPI_INTERFACE_TEST */

/* ========================================================================= */
/*                      TAPI Old Interface Adaptations                       */
/* ========================================================================= */

#ifdef TAPI_OLD_IO
/* old TAPI ioctl commands for backward compatibility */
#include "drv_tapi_old_io.h"
#endif /* TAPI_OLD_IO */

#endif  /* DRV_TAPI_IO_H */

