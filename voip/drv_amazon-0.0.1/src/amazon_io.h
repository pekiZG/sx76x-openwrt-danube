#ifndef _AMAZON_IO_H
#define _AMAZON_IO_H
/****************************************************************************
       Copyright (c) 2004, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
 ****************************************************************************
   Module      : amazon_io.h
   Date        : 2004-10-25
   Description : This io file contains structures, typedefs and defines shared
                 which other software modules (application code, other drivers,
                 ...).
*******************************************************************************/

/** \file
    This io file contains board driver related structures, typedefs and defines
    shared which other software modules (application code, other drivers, ...).
*/
/** \mainpage Vinetic Board Driver.

    \section s_one 1) Generalities

     This is the documentation of the board driver template for vinetic systems.
     The source code is written in a portable way and script files to generate
     the source code for a specific system are provided. In this case, only the
     low level access (drv_amazon_access.c) must be adapted to the system used,
     unless the developer needs new ioctls which aren't implemented.

    \section s_two 2) Concept and Architecture

     The idea of implementing a board driver is to lower the complexity of the
     proper vinetic driver by separating it from board related stuff, so that
     it can be reusable with \e very \e less \e portability \e efforts.

     As the vinetic chip is used on several, very different boards,
     the following concept applies:

     \li Only one Vinetic driver \e freed from Board Support and containing
         \e only Vinetic Intelligence
     \li One Board Driver supporting the plattform used and interacting with
         the vinetic driver, containing the board intelligence.
     \li User interfaces playing the mediator between the both drivers

      The following figure outlines the overall vinetic software system
      architecture.\n\n

      \image html SofwareSystemArch.emf

    \section s_three 3) Operating systems supported

    -# LINUX
    -# VXWORKS
*/

/** \defgroup AMAZON_INTERFACE Vinetic amazon Board Driver Interface
    Lists the entire interface to the Board driver
@{  */

/** \defgroup AMAZON_INTERFACE_CONFIG amazon Configuration Interfaces */
/** \defgroup AMAZON_INTERFACE_INIT amazon Initialization Interfaces */
/** \defgroup AMAZON_INTERFACE_REGACCESS amazon Register Access Interfaces */
/** \defgroup AMAZON_INTERFACE_MISC amazon Driver miscelaneous interfaces */
/** \defgroup AMAZON_INTERFACE_OPERATIONAL amazon Driver operational interfaces */


/* ============================= */
/* Global Defines                */
/* ============================= */

/* magic number */
#define AMAZON_IOC_MAGIC 'X'

/** This interface sets the trace/log level.

   \param int The parameter points to the trace/log level to set

   \return none

   \ingroup AMAZON_INTERFACE_MISC

   \code
    // trace level normal
    ioctl(fd, FIO_AMAZON_DEBUGLEVEL, DBG_LEVEL_NORMAL);
   \endcode */
#define FIO_AMAZON_DEBUGLEVEL                       _IO(AMAZON_IOC_MAGIC, 1)

/** This interface provides the amazon Driver version string on request.

   \param char* The parameter points to the version character string

   \return 0 if successful, otherwise -1 in case of an error

   \ingroup AMAZON_INTERFACE_MISC

   \code
   char Version[80];
   int ret;

   ret = ioctl(fd, FIO_AMAZON_GET_VERSION, (int)&Version);
   printf(\namazon Driver Version:%s\n, Version);
   \endcode */
#define FIO_AMAZON_GET_VERSION                      _IO(AMAZON_IOC_MAGIC, 2)

/** This interface stores the configuration needed for initialization
    (access mode, irq mode, clock rate).

   \param amazon_Config_t* The parameter points to a
   \ref amazon_Config_t structure.

   \return 0 if successful, otherwise -1 in case of an error

   \remarks This interface can be called by the user application to change the
            default configuration. Otherwise, the initialization can be done
            directly using \ref FIO_AMAZON_INIT.

   \ingroup AMAZON_INTERFACE_CONFIG

   \code
     // This example shows how to do a configuration prior to board
     // initialization.
     amazon_Config_t param;
     memset (&param, 0, sizeof(amazon_Config_t));
     // set access mode
     param.nAccessMode = AMAZON_ACCESS_16BIT_MOTOROLA;
     // set clock rate
     param.nClkRate    = AMAZON_CLK_4096_KHZ;
     // set irq line mode
     param.nIrqMode    = AMAZON_LEVEL_IRQ;
     ret = ioctl(fd, FIO_AMAZON_CONFIG, &param);
   \endcode

   \code
     // This example shows how to set back the default configuration if another
     // configuration was done before.
     ret = ioctl(fd, FIO_AMAZON_CONFIG, 0);
   \endcode */
#define FIO_AMAZON_CONFIG                           _IO(AMAZON_IOC_MAGIC, 3)

/** This interface reads the actual configuration

   \param amazon_Config_t* The parameter points to a
   \ref amazon_Config_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be used to read the configuration stored with
           \ref FIO_AMAZON_CONFIG.

   \ingroup AMAZON_INTERFACE_CONFIG

   \code
     amazon_Config_t param;
     memset (&param, 0, sizeof(amazon_Config_t));
     ret = ioctl(fd, FIO_AMAZON_GETCONFIG, &param);
   \endcode */
#define FIO_AMAZON_GETCONFIG                        _IO(AMAZON_IOC_MAGIC, 4)

/** This interface initializes the board (chip select, access mode, clock rate)

   \param int This interface expects no parameter. It should be set to 0.

   \return number of Vinetic chips onboard if successful, otherwise -1 in
           case of an error.

   \remark This interface must be called before the board can be used. In case
           the default configuration is not suitable,
           use \ref FIO_AMAZON_CONFIG to store the configuration parameters.

   \ingroup AMAZON_INTERFACE_INIT

   \code
    ret = ioctl(fd, FIO_AMAZON_INIT, 0);
   \endcode */
#define FIO_AMAZON_INIT                             _IO(AMAZON_IOC_MAGIC, 5)

/** This interface resets the vinetic

   \param amazon_Reset_t * The parameter points to a
   \ref amazon_Reset_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be called to reset a specific vinetic chip.

   \ingroup AMAZON_INTERFACE_INIT

   \code
   // complete reset of vinetic chip 0
    amazon_Reset_t param;

    param.nChipNum = 0;
    param.nResetMode = AMAZON_RESET_ACTIVE_DEACTIVE;
    ret = ioctl(fd, FIO_AMAZON_RESETCHIP, &param);
   \endcode */
#define FIO_AMAZON_RESETCHIP                        _IO(AMAZON_IOC_MAGIC, 6)

/** This interface provides chip key information (base address, irq line) on
    request according to chip number.

   \param amazon_GetDevParam_t* The parameter points to a
   \ref amazon_GetDevParam_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be called only after the initialization was
           succesfully done using \ref FIO_AMAZON_INIT.

   \ingroup AMAZON_INTERFACE_INIT

   \code
     amazon_GetDevParam_t boardParam;
     int chipNum = 0, i;

     chipNum = ioctl(fd, FIO_AMAZON_INIT, 0);
     if (chipNum > 0)
     {
        for (i = 0; i < chipNum; i++)
        {
           boardParam.nChipNum = i;
           ret = ioctl(fd, FIO_AMAZON_GETBOARDPARAMS, &boardParam);
           if (ret == IFX_SUCESS)
           {
              initilize_vinetic_driver (&boardParam);
           }
           else
              break;
        }
     }
   \endcode */
#define FIO_AMAZON_GETBOARDPARAMS                     _IO(AMAZON_IOC_MAGIC, 7)

/** This interface reads a board register according to specified family.

   \param amazon_Reg_t* The parameter points to a
   \ref amazon_Reg_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be called only after the initialization was
           succesfully done using \ref FIO_AMAZON_INIT.

   \ingroup AMAZON_INTERFACE_REGACCESS

   \code

   \endcode */
#define FIO_AMAZON_XYZREG_READ                       _IO(AMAZON_IOC_MAGIC, 8)

/** This interface writes a board register according to specified family.

   \param amazon_Reg_t* The parameter points to a
   \ref amazon_Reg_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be called only after the initialization was
           succesfully done using \ref FIO_AMAZON_INIT.

   \ingroup AMAZON_INTERFACE_REGACCESS

*/
#define FIO_AMAZON_XYZREG_WRITE                      _IO(AMAZON_IOC_MAGIC, 9)

/** This interface modifies a board register according to specified family.

   \param amazon_Reg_t* The parameter points to a
   \ref amazon_Reg_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \remark This interface can be called only after the initialization was
           succesfully done using \ref FIO_AMAZON_INIT.

   \ingroup AMAZON_INTERFACE_REGACCESS
*/
#define FIO_AMAZON_XYZREG_MODIFY                    _IO(AMAZON_IOC_MAGIC, 10)

/** This interface sets the specified led.

   \param amazon_Led_t* points to a
   \ref amazon_Led_t structure.

   \return 0 if successful, otherwise -1 in case of an error.

   \ingroup AMAZON_INTERFACE_OPERATIONAL

   \code
    // This example sets led 0 on.
    amazon_Led_t param
    param.nLedNum = 0;
    param.bLedState = 1:
    ret = ioctl(fd, FIO_AMAZON_SETLED, &param);
   \endcode */
#define FIO_AMAZON_SETLED                          _IO(AMAZON_IOC_MAGIC, 11)

/** This interface reads the board version.

   \param int*  points to the board version.

   \return 0 if successful, otherwise -1 in case of an error.

   \ingroup AMAZON_INTERFACE_OPERATIONAL

   \code
    // This example reads the board version.
    IFX_int32_t board_vers;
    ret = ioctl(fd, FIO_AMAZON_GET_BOARDVERS, &board_vers);
   \endcode */
#define FIO_AMAZON_GET_BOARDVERS                   _IO(AMAZON_IOC_MAGIC, 12)


/** This interface sets the access mode with appropriate chip select.

   \param int points to a
   \ref amazon_AccessMode_e enum.

   \return 0 if successful, otherwise -1 in case of an error.

   \ingroup AMAZON_INTERFACE_OPERATIONAL

   \code
    // This example sets 16 bit motorola mode.
    ret = ioctl(fd, FIO_AMAZON_SET_ACCESSMODE, amazon_ACCESS_16BIT_MOTOROLA);
   \endcode */
#define FIO_AMAZON_SET_ACCESSMODE                    _IO(AMAZON_IOC_MAGIC, 14)

/** This interface sets the clock rate.

   \param int points to a
   \ref amazon_ClockRate_e enum.

   \return 0 if successful, otherwise -1 in case of an error.

   \ingroup amazon_INTERFACE_OPERATIONAL

   \code
    // This example sets a clockrate of 2 MHz.
    ret = ioctl(fd, FIO_AMAZON_SET_CLOCKRATE, amazon_CLK_2048_KHZ);
   \endcode */
#define FIO_AMAZON_SET_CLOCKRATE                     _IO(AMAZON_IOC_MAGIC, 15)

#define FIO_AMAZON_DEACTIVATE_RESET                  _IO(AMAZON_IOC_MAGIC, 16)


/** @} */

/* ============================= */
/* Global enums                  */
/* ============================= */

/** amazon Board Access Modes
   \ingroup amazon_INTERFACE_INIT
   \ingroup amazon_INTERFACE_OPERATIONAL
*/
typedef enum
{
   /** 16-bit Motorola parallel access */
   AMAZON_ACCESS_16BIT_MOTOROLA = 0,
   /** 16-bit Intel Mux parallel access */
   AMAZON_ACCESS_16BIT_INTELMUX = 1,
   /** 16-bit Intel Demux parallel access */
   AMAZON_ACCESS_16BIT_INTELDEMUX = 2,
   /** 8-bit Motorola parallel access */
   AMAZON_ACCESS_8BIT_MOTOROLA = 3,
   /** 8-bit Intel Mux parallel access */
   AMAZON_ACCESS_8BIT_INTELMUX = 4,
   /** 8-bit Intel Demux parallel access */
   AMAZON_ACCESS_8BIT_INTELDEMUX = 5,
   /** SPI access */
   AMAZON_ACCESS_SPI = 6
} amazon_AccessMode_e;

/** amazon Clock rates
   \ingroup amazon_INTERFACE_INIT
   \ingroup amazon_INTERFACE_OPERATIONAL
*/
typedef enum
{
   /** Clock = 8192 KHz */
   amazon_CLK_8192_KHZ = 0,
   /** Clock = 4096 KHz */
   amazon_CLK_4096_KHZ = 1,
   /** Clock = 3072 KHz */
   amazon_CLK_3072_KHZ = 2,
   /** Clock = 2048 KHz */
   amazon_CLK_2048_KHZ = 3,
   /** Clock = 1536 KHz */
   amazon_CLK_1536_KHZ = 4,
   /** Clock = 1024 KHz */
   amazon_CLK_1024_KHZ = 5,
   /** Clock = 512 KHz */
   amazon_CLK_512_KHZ = 6
} amazon_ClockRate_e;

/** amazon Reset Modes
   \ingroup amazon_INTERFACE_INIT
*/
typedef enum
{
   /** Reset active */
   amazon_RESET_ACTIVE = 0,
   /** Reset not active */
   amazon_RESET_DEACTIVE = 1,
   /** Reset active and deactive */
   amazon_RESET_ACTIVE_DEACTIVE = 2
} amazon_ResetMode_e;



/* ============================= */
/* Global Structures             */
/* ============================= */

/** amazon Reset
   \ingroup amazon_INTERFACE_INIT
*/
typedef struct
{
   /** Vinetic chip number for which
       the reset is done */
   unsigned int         nChipNum;
   /** Reset Mode according to enum
      \ref amazon_ResetMode_e */
   amazon_ResetMode_e nResetMode;
} amazon_Reset_t;

/** amazon Board minimal configuration
   \ingroup amazon_INTERFACE_CONFIG
*/
typedef struct
{
   /** Chip access Mode according to enum
      \ref amazon_AccessMode_e */
   amazon_AccessMode_e nAccessMode;
   /** Clock rate according to enum
      \ref amazon_ClockRate_e */
   amazon_ClockRate_e  nClkRate;
} amazon_Config_t;

/** amazon Board Information.
   \remark
      Board Information is available only after initialization
   \ingroup amazon_INTERFACE_INIT
*/
typedef struct
{
   /** Vinetic chip number for which
       information will be retrieved, in */
   unsigned int  nChipNum;
   /** Chip access Mode according to enum
      \ref amazon_AccessMode_e, out */
   amazon_AccessMode_e nAccessMode;
   /** Clock rate according to enum
      \ref amazon_ClockRate_e, out */
   amazon_ClockRate_e  nClkRate;
   /** Physical Base address to access
       the chip, out */
   unsigned long nBaseAddrPhy;
   /** Irq Line on which an interrupt
       handler will be installed, out.

       \remark
        In case -1 is set, polling mode
        will be assumed. */
   int           nIrqNum;
} amazon_GetDevParam_t;

/** amazon Board Register access
   \ingroup amazon_INTERFACE_REGACCESS
*/
typedef struct
{

   /** Register offset of register to access */
   unsigned char nRegOffset;
   /** Register mask if modify */
   unsigned char nRegMask;
   /** Register value to write / read */
   unsigned char nRegVal;
} amazon_Reg_t;

/** amazon Led control
    \ingroup amazon_INTERFACE_OPERATIONAL
*/
typedef struct
{
   /** Number of Led to set */
   unsigned int nLedNum;
   /** Led state to set

   - 0 : Led Off
   - 1 : Led  On */
   unsigned int bLedState;
} amazon_Led_t;

/* ============================= */
/* Exported Functions            */
/* ============================= */

#endif /* _amazon_IO_H */

