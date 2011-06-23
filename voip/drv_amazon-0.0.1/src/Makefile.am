## Process this file with automake to produce the Makefile.

bin_PROGRAMS = drv_amazon

## the driver itself
drv_amazon_SOURCES = \
   @srcdir@/amazon_io.h \
   @srcdir@/drv_amazon_api.c \
   @srcdir@/drv_amazon_api.h \
   @srcdir@/drv_amazon_access.c \
   @srcdir@/drv_amazon_common.c \
   @srcdir@/drv_amazon_linux.c

EXTRA_DIST = \
   @srcdir@/drv_amazon_vxworks.c \
   @srcdir@/common/src/sys_drv_ifxos.h \
   @srcdir@/common/src/ifx_types.h \
   @srcdir@/common/src/sys_drv_debug.h

INCLUDES = -I@srcdir@ -I@srcdir@/common/src -I@KERNEL_INCL_PATH@ -I@VINETIC_INCL_PATH@

if DEBUG
   drv_amazon_CFLAGS = -O1 -g3
else
   drv_amazon_CFLAGS = -O2
endif

#drv_amazon_CFLAGS += -DLINUX -D__KERNEL__ -DMODULE -DCONFIG_DEVFS_FS -DENABLE_TRACE
drv_amazon_CFLAGS += -DLINUX -D__KERNEL__ -DMODULE -DENABLE_TRACE

drv_amazon_LDFLAGS = -r -nostdlib

