# Copyright (C) 2005 infineon <Qi-Ming.Wu@infineon.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([amazon_daa],
        [0.0.1],
        [infineon infineon@infineon.com],
        [amazon_daa])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

AC_PROG_CC
AC_PROG_INSTALL
AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_STDC_HEADERS
AC_PROG_RANLIB

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   m4/Makefile
   src/Makefile
   test/Makefile
])


DEFAULT_BOARD_NAME='AMAZON'
BOARD_NAME='AMAZON'
AC_ARG_ENABLE(boardname,
   AC_HELP_STRING(
      [--enable-boardname=x],
      [Set the target board]
   ),
   [
    if test -n $enableval; then
       echo Set the board name $enableval
       AC_SUBST([BOARD_NAME],[$enableval])
    else
       echo Set the default BOARD_NAME $DEFAULT_BOARD_NAME
       AC_SUBST([BOARD_NAME],[$DEFAULT_BOARD_NAME])
    fi
   ]
)

DEFAULT_KERNEL_INCL_PATH='/cvs/kernel/opensource/linux_2_4_20/include'
AC_SUBST([KERNEL_INCL_PATH],[$DEFAULT_KERNEL_INCL_PATH])

AC_ARG_ENABLE(kernelincl,
   AC_HELP_STRING(
      [--enable-kernelincl=x],
      [Set the target kernel include path]
   ),
   [
    if test -n $enableval; then
       echo Set the LINUX kernel include path $enableval
       AC_SUBST([KERNEL_INCL_PATH],[$enableval])
    else
       echo Set the default LINUX kernel include path $DEFAULT_KERNEL_INCL_PATH
       AC_SUBST([KERNEL_INCL_PATH],[$DEFAULT_KERNEL_INCL_PATH])
    fi
   ]
)



DEFAULT_DUSLIC_INCL_PATH='/opt/qiming/duslic/src'
AC_SUBST([DUSLIC_INCL_PATH],[$DEFAULT_DUSLIC_INCL_PATH])

AC_ARG_ENABLE(duslicincl,
   AC_HELP_STRING(
      [--enable-duslicincl=x],
      [Set the target duslic include path]
   ),
   [
    if test -n $enableval; then
       echo Set the duslic include path $enableval
       AC_SUBST([DUSLIC_INCL_PATH],[$enableval])
    else
       echo Set the default duslic include path $DEFAULT_DUSLIC_INCL_PATH
       AC_SUBST([DUSLIC_INCL_PATH],[$DEFAULT_DUSLIC_INCL_PATH])
    fi
   ]
)



#AM_CONDITIONAL(AMAZON,false)

case "${BOARD_NAME}" in
    AMAZON)
       AM_CONDITIONAL(AMAZON, true)
    ;;
esac

case "${host}" in
   mips-linux) 
      CFLAGS=""      
      AM_CONDITIONAL(MIPS, true)
      ;;
esac



AC_OUTPUT
