ifeq (,$(findstring /$(LDS_ARCH)/build,$(CURDIR)))
  include $(LDS_MAKEFILE_DIR)/general/target.mk
else
VPATH=$(SRCDIR)

SRC_FILES= drv_vinetic_api.c \
    drv_vinetic_basic.c \
    drv_vinetic_cid.c \
    drv_vinetic_cram.c \
    drv_vinetic_dspconf.c \
    drv_vinetic_dwnld.c \
    drv_vinetic_gpio.c \
    drv_vinetic_int.c \
    drv_vinetic_main.c \
    drv_vinetic_misc.c \
    drv_vinetic_parallel.c \
    drv_vinetic_polled.c \
    drv_vinetic_stream.c \
    drv_vinetic_tapi.c \
    drv_vinetic_tone.c \
    drv_vinetic_lt.c \
    drv_vinetic_gr909.c \
    drv_vinetic_linux.c \
    drv_vinetic_con.c \
    drv_vinetic_serial.c \
    sys_drv_fifo.c \
    sys_drv_debug.c \
    drv_tapi_dial.c \
    drv_tapi_event.c \
    drv_tapi_linux.c \
    drv_tapi_main.c \
    drv_tapi_meter.c \
    drv_tapi_misc.c \
    drv_tapi_opcontrol.c \
    drv_tapi_pcm.c \
    drv_tapi_ring.c \
    drv_tapi_tone.c \
    drv_tapi_cid.c \
    drv_tapi_voice.c


CFLAGS += -DTAPI -DHAVE_CONFIG_H -DOLD_BOARD


CFLAGS +=-I$(LDS_KERNEL_DIR)/include \
	-I$(LDS_SRC_INFINEON_COMPONENTS_VINETIC_DIR) \
	-I$(SRCDIR)/common/src \
        -I$(SRCDIR)/tapi/src \
            -I$(SRCDIR)/lib/lib_fifo \
            -I$(SRCDIR)/bin_images \
            -I$(SRCDIR)/mpc/src \
            -I$(SRCDIR)/evaluation \
            -I$(SRCDIR)/testing    \
            -I$(SRCDIR)/../../drv_spi/src

	
DRIVER_NAME=drv_vinetic
OUTPUT_FILE=drv_vinetic.o

all: $(OUTPUT_FILE) install

include $(LDS_MAKEFILE_GENERAL_DIR)/common_driver_rules.mk


$(OUTPUT_FILE): $(OBJ_FILES)
	@echo "  LD     $(notdir $@)"
	$(CC) $(LDFLAGS) -o  $@ $^


install:
	mkdir -p $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/
	install -m 755 $(OUTPUT_FILE) $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/


endif
