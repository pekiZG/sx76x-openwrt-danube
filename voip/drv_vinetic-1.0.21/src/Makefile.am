## Process this file with automake to produce Makefile.

bin_PROGRAMS = drv_vinetic

## the driver itself
drv_vinetic_SOURCES = \
    @top_srcdir@/drv_version.h \
    @srcdir@/drv_api.h \
    @srcdir@/drv_vinetic.h \
    @srcdir@/drv_vinetic_api.c \
    @srcdir@/drv_vinetic_api.h \
    @srcdir@/drv_vinetic_basic.c \
    @srcdir@/drv_vinetic_basic.h \
    @srcdir@/drv_vinetic_cid.c \
    @srcdir@/drv_vinetic_cid.h \
    @srcdir@/drv_vinetic_cram.c \
    @srcdir@/drv_vinetic_cram.h \
    @srcdir@/drv_vinetic_dspconf.c \
    @srcdir@/drv_vinetic_dspconf.h \
    @srcdir@/drv_vinetic_dwnld.c \
    @srcdir@/drv_vinetic_dwnld.h \
    @srcdir@/drv_vinetic_errno.h \
    @srcdir@/drv_vinetic_gpio.c \
    @srcdir@/drv_vinetic_gpio.h \
    @srcdir@/drv_vinetic_gpio_user.h \
    @srcdir@/drv_vinetic_int.c \
    @srcdir@/drv_vinetic_int.h \
    @srcdir@/drv_vinetic_main.c \
    @srcdir@/drv_vinetic_main.h \
    @srcdir@/drv_vinetic_misc.c \
    @srcdir@/drv_vinetic_misc.h \
    @srcdir@/drv_vinetic_parallel.c \
    @srcdir@/drv_vinetic_parallel.h \
    @srcdir@/drv_vinetic_serial.c \
    @srcdir@/drv_vinetic_serial.h \
    @srcdir@/drv_vinetic_polled.c \
    @srcdir@/drv_vinetic_polled.h \
    @srcdir@/drv_vinetic_stream.c \
    @srcdir@/drv_vinetic_stream.h \
    @srcdir@/drv_vinetic_tapi.c \
    @srcdir@/drv_vinetic_tapi.h \
    @srcdir@/drv_vinetic_tone.c \
    @srcdir@/drv_vinetic_tone.h \
    @srcdir@/drv_vinetic_lt.c \
    @srcdir@/drv_vinetic_lt.h \
    @srcdir@/drv_vinetic_gr909.c \
    @srcdir@/drv_vinetic_gr909.h \
    @srcdir@/drv_vinetic_linux.c \
    @srcdir@/vinetic_io.h \
    @srcdir@/vinetic_tg_amp.h \
    @srcdir@/vinetic_tg_freq.h \
    @srcdir@/common/src/sys_drv_fifo.c \
    @srcdir@/common/src/sys_drv_fifo.h \
    @srcdir@/common/src/sys_drv_debug.c \
    @srcdir@/common/src/sys_drv_debug.h \
    @srcdir@/tapi/src/drv_tapi.h \
    @srcdir@/tapi/src/drv_tapi_io.h \
    @srcdir@/tapi/src/drv_tapi_dial.c \
    @srcdir@/tapi/src/drv_tapi_event.c \
    @srcdir@/tapi/src/drv_tapi_linux.c \
    @srcdir@/tapi/src/drv_tapi_linux.h \
    @srcdir@/tapi/src/drv_tapi_main.c \
    @srcdir@/tapi/src/drv_tapi_meter.c \
    @srcdir@/tapi/src/drv_tapi_misc.c \
    @srcdir@/tapi/src/drv_tapi_opcontrol.c \
    @srcdir@/tapi/src/drv_tapi_pcm.c \
    @srcdir@/tapi/src/drv_tapi_ring.c \
    @srcdir@/tapi/src/drv_tapi_tone.c \
    @srcdir@/tapi/src/drv_tapi_cid.c \
    @srcdir@/tapi/src/drv_tapi_voice.c \
    @srcdir@/drv_vinetic_con.c \
    @srcdir@/drv_vinetic_con.h

#   @srcdir@/lib/lib_fifo/lib_fifo.c

EXTRA_DIST = \
    @top_srcdir@/drv_vinetic.wpj \
    @srcdir@/drv_config_user.default.h \
    @srcdir@/drv_vinetic_vxworks.c \
    @srcdir@/drv_vinetic_vxworks.h \
    @srcdir@/bin_images/acdwl.h \
    @srcdir@/bin_images/dcdwl.h \
    @srcdir@/bin_images/dramdwl.h \
    @srcdir@/bin_images/pramdwl.h \
    @srcdir@/bin_images/phi_VIP.h \
    @srcdir@/bin_images/Embd_CTRL_2_14.c \
    @srcdir@/bin_images/Embd_CTRL_3_13.c \
    @srcdir@/bin_images/Embd_CTRL_4_04.c \
    @srcdir@/common/src/driverlib.h \
    @srcdir@/common/src/ifx_types.h \
    @srcdir@/common/src/sys_drv_defs.h \
    @srcdir@/common/src/sys_drv_delaylib_vxworks.c \
    @srcdir@/common/src/sys_drv_delaylib_vxworks.h \
    @srcdir@/common/src/sys_drv_ifxos.h \
    @srcdir@/common/src/sys_drv_kthread_linux.c \
    @srcdir@/common/src/sys_drv_kthread_linux.h \
    @srcdir@/common/src/sys_drv_noOS.h \
    @srcdir@/common/src/sys_drv_timerlib_vxworks.c \
    @srcdir@/common/src/sys_drv_timerlib_vxworks.h \
    @srcdir@/mpc/src/drv_mpc.h \
    @srcdir@/tapi/src/drv_tapi_noos.h \
    @srcdir@/tapi/src/drv_tapi_vxworks.c \
    @srcdir@/tapi/src/drv_tapi_vxworks.h \
    @srcdir@/lib/lib_fifo/lib_fifo.c \
    @srcdir@/lib/lib_fifo/lib_fifo.h 

INCLUDES =  -I@top_srcdir@ \
            -I@KERNEL_INCL_PATH@ \
            -I@srcdir@/common/src \
            -I@srcdir@/tapi/src \
            -I@srcdir@/lib/lib_fifo \
            -I@srcdir@/bin_images \
            -I@srcdir@/mpc/src \
            -I@srcdir@/evaluation \
            -I@srcdir@/testing    \
            -I@srcdir@/../../drv_spi/src

if WARNINGS
    AM_CFLAGS = -Wall -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment -Wparentheses -Wpointer-arith
else
    AM_CFLAGS = -Wall
endif

if DEBUG
    drv_debug_vinetic_CFLAGS = -O1 -g3
else
    drv_debug_vinetic_CFLAGS =
endif

drv_vinetic_CFLAGS =-D__LINUX__ -DLINUX -D__KERNEL__ -DMODULE \
                    -DTAPI $(AM_CFLAGS) $(drv_debug_vinetic_CFLAGS)

drv_vinetic_LDFLAGS = -r -nostdlib

