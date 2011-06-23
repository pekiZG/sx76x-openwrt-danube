# Copyright (C) 2005 infineon <Qi-Ming.Wu@infineon.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = amazon_daa 
amazon_daa_SOURCES= \
		drv_api.h \
		drv_cpc5621_api.c \
		drv_cpc5621_api.h \
                drv_cpc5621_common.c \
                drv_cpc5621_interface.h \
                drv_cpc5621_io.c \
		drv_cpc5621_linux.c \
		drv_cpc5621_linux.h

amazon_daa_LDFLAGS = -r -nostdlib
amazon_daa_CFLAGS = -D@BOARD_NAME@ -D__KERNEL__ \
                    -DLINUX -DMODULE  -DMODVERSIONS -DEXPORT_SYMTAB \
		    -DCONFIG_DEVFS_FS -DDUSLIC -D__BYTE_ORDER=__BIG_ENDIAN
INCLUDES =-I@KERNEL_INCL_PATH@
INCLUDES+=-I@DUSLIC_INCL_PATH@ -I@DUSLIC_INCL_PATH@/common/src/ 
	   	 

EXTRA_DIST = \
           drv_cpc5621_noOS.c \
           drv_cpc5621_noOS.h \
           drv_cpc5621_vxworks_bsp.c \
           drv_cpc5621_vxworks.c  \
           drv_cpc5621_vxworks.h 
		   
		   

if MIPS
amazon_daa_CFLAGS += -Wall -Wstrict-prototypes -Wno-trigraphs -O2 \
 -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 \
 -mno-abicalls -fno-pic -pipe -mtune=r4600 -mips2 -Wa,--trap\
 -mlong-calls
endif   
		   
		
