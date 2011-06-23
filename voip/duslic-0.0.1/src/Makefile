ifeq (,$(findstring /$(LDS_ARCH)/build,$(CURDIR)))
  include $(LDS_MAKEFILE_DIR)/general/target.mk
else
VPATH=$(SRCDIR)

SRC_FILES=drv_duslic_api.c \
          drv_duslic_linux.c \
          drv_duslic_main.c \
          drv_duslic_dwnld.c \
          drv_duslic_gpio.c  \
          drv_duslic_int.c   \
          drv_duslic_basic.c \
	  drv_duslic_misc.c  \
	  drv_duslic_amazon.c

BOARD_NAME=AMAZON
CFLAGS += -D$(BOARD_NAME) -DDUSLIC_SPI -DUSE_VINETIC_GPIO -DHAVE_CONFIG_H


CFLAGS +=-I$(LDS_KERNEL_DIR)/include \
	-I$(LDS_SRC_INFINEON_COMPONENTS_VINETIC_INC_DIR) \
	-I$(SRCDIR)/common/src

	
DRIVER_NAME=duslic
OUTPUT_FILE=$(DRIVER_NAME).o

all: $(OUTPUT_FILE) install

include $(LDS_MAKEFILE_GENERAL_DIR)/common_driver_rules.mk

install:
	mkdir -p $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/
	install -m 755 $(OUTPUT_FILE) $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/

$(OUTPUT_FILE):  $(OBJ_FILES)
	@echo "  LD     $(notdir $@)"
	$(CC) $(LDFLAGS) -o  $@ $^

endif
