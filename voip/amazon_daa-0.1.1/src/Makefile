ifeq (,$(findstring /$(LDS_ARCH)/build,$(CURDIR)))
  include $(LDS_MAKEFILE_DIR)/general/target.mk
else
VPATH=$(SRCDIR)

SRC_FILES= drv_cpc5621_api.c drv_cpc5621_common.c drv_cpc5621_io.c drv_cpc5621_linux.c

BOARD_NAME=AMAZON
CFLAGS += -D$(BOARD_NAME) -DDUSLIC -D__BYTE_ORDER=__BIG_ENDIAN -DCONFIG_DEVFS_FS -DHAVE_CONFIG_H -DLINUX -DPORTA -DVINETIC

CFLAGS +=-I$(LDS_KERNEL_DIR)/include \
	-I$(LDS_SRC_INFINEON_COMPONENTS_VINETIC_INC_DIR) \
	-I$(LDS_SRC_INFINEON_COMPONENTS_DUSLIC_INC_DIR) \
	-I$(LDS_SRC_INFINEON_COMPONENTS_DUSLIC_INC_DIR)/common/src

	
DRIVER_NAME=amazon_daa
OUTPUT_FILE=$(DRIVER_NAME).o

all: $(OUTPUT_FILE) install

include $(LDS_MAKEFILE_GENERAL_DIR)/common_driver_rules.mk

	

install:
	mkdir -p $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/
	install -m 755 $(OUTPUT_FILE) $(LDS_GEN_FS_DIR)/lib/modules/$(LDS_KERNEL_VERSION)/

$(OUTPUT_FILE): $(OBJ_FILES)
	@echo "  LD     $(notdir $@)"
	$(CC) $(LDFLAGS) -o  $@ $^

	

endif
