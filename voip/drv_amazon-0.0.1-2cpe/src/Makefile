ifeq (,$(findstring /$(LDS_ARCH)/build,$(CURDIR)))
  include $(LDS_MAKEFILE_DIR)/general/target.mk
else
VPATH=$(SRCDIR)

SRC_FILES= drv_amazon_api.c \
	drv_amazon_access.c \
	drv_amazon_common.c \
	drv_amazon_linux.c


CFLAGS +=-I$(LDS_KERNEL_DIR)/include \
	 -I$(SRCDIR)/ \
	-I$(LDS_SRC_INFINEON_COMPONENTS_VINETIC_INC_DIR) \
	-I$(SRCDIR)/common/src


CFLAGS += -DHAVE_CONFIG_H

DRIVER_NAME=drv_amazon
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
