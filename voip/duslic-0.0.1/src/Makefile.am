# Copyright (C) 2005 Wu Qi Ming <Qi-Ming.Wu@infineon.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


bin_PROGRAMS = drv_duslic
drv_duslic_SOURCES =  \
          drv_api.h \
          drv_duslic.h \
          drv_duslic_api.h \
          drv_duslic_api.c \
          drv_duslic_linux.c \
          drv_duslic_main.c \
          drv_duslic_main.h \
          drv_duslic_dwnld.c \
	  drv_duslic_dwnld.h \
          drv_duslic_gpio.c  \
          drv_duslic_gpio.h  \
	  drv_duslic_errno.h \
          drv_duslic_int.h   \
          drv_duslic_int.c   \
          drv_duslic_basic.c \
          drv_duslic_basic.h \
	  drv_duslic_misc.c  \
	  drv_duslic_misc.h
drv_duslic_LDFLAGS = -r -nostdlib
drv_duslic_CFLAGS = -D@BOARD_NAME@ -D__KERNEL__ \
                    -DLINUX -DMODULE  -DMODVERSIONS -DEXPORT_SYMTAB \
		    -DCONFIG_DEVFS_FS -DDUSLIC_SPI
INCLUDES =-I@KERNEL_INCL_PATH@ -I@VINETIC_INCL_PATH@
INCLUDES +=-Icommon/src

EXTRA_DIST = \
   drv_board.h \
   duslic_io.h \
   common/src/sys_drv_delaylib_vxworks.c \
   common/src/sys_drv_debug.c \
   common/src/sys_drv_fifo.c \
   common/src/sys_drv_kthread_linux.c \
   common/src/sys_drv_timerlib_vxworks.c \
   common/src/driverlib.h \
   common/src/ifx_types.h \
   common/src/sys_drv_debug.h \
   common/src/sys_drv_defs.h \
   common/src/sys_drv_delaylib_vxworks.h \
   common/src/sys_drv_fifo.h \
   common/src/sys_drv_ifxos.h \
   common/src/sys_drv_kthread_linux.h \
   common/src/sys_drv_linux.h \
   common/src/sys_drv_noOS.h \
   common/src/sys_drv_ose.h \
   common/src/sys_drv_psos.h \
   common/src/sys_drv_timerlib_vxworks.h 
    
if AMAZON 
EXTRA_DIST +=\
       drv_duslic_amazon.h
drv_duslic_SOURCES += \
       drv_duslic_amazon.c
endif

if MIPS
drv_duslic_CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -O2 \
 -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
 -mno-abicalls -fno-pic -pipe -mtune=r4600 -mips2 -Wa,--trap\
 -mlong-calls
endif


drv_duslic_CFLAGS +=-DUSE_VINETIC_GPIO

