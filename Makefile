#
# Makefile - main Makefile
# Created by Matheus Leme Da Silva
#

MAKEFLAGS += --no-print-directory

include config.mk

CC ?= gcc
CFLAGS := -Wall -Wextra -O2

# ===== Directories =====
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

# ===== Files =====
IMAGE := $(OSI_OSNAME).img
BOOTLOADER_FILE := $(BIN_DIR)/bootloader.bin
KERNEL_FILE := $(BIN_DIR)/$(KERNEL_IMAGE)

# ==================================
# Targets
# ==================================

all: $(IMAGE)

prepare:
	@echo "  MKDIR     $(BIN_DIR) $(OBJ_DIR)"
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

compile: prepare
	@make -C src BOOTLOADER=../$(BOOTLOADER_FILE) \
		KERNEL=../$(KERNEL_FILE) \
		OBJD=../$(OBJ_DIR) \
		BIND=../$(BIN_DIR) \
		DEBUG=$(DEBUG)

$(IMAGE): prepare compile
	@echo "[   IMAGE   ] Creating $(IMAGE)"
	@dd if=/dev/zero of=$(IMAGE) bs=$(FS_SECTOR_SIZE) count=$(FS_TOTAL_SECTORS) status=none
	@mkfs.fat -F $(FS_FAT_TYPE) -n $(OSI_VOLUME_LABEL) $(IMAGE)
	@dd if=$(BOOTLOADER_FILE) of=$(IMAGE) conv=notrunc status=none
	@#mkdir -p image
	@#fusefat -o rw+,nonempty $(IMAGE) ./image
	@#sleep 0.5
	@#cp $(KERNEL_FILE) ./image/$(KERNEL_IMAGE)
	@#sync
	@#fusermount -u ./image
	@#rmdir image
	@mcopy -i $(IMAGE) $(KERNEL_FILE) "::/$(KERNEL_IMAGE)"

run:
	@echo "[   RUN     ] Running $(IMAGE) on QEMU"
	@qemu-system-i386 -drive format=raw,file=$(IMAGE) -m $(MACHINE_MEMORY) -boot order=$(MACHINE_BOOT_ORDER) -audiodev $(MACHINE_AUDIODEV),id=audio0 -machine pcspk-audiodev=audio0 -serial stdio

run-ng:
	@echo "[   RUN     ] Running $(IMAGE) on QEMU"
	@qemu-system-i386 -drive format=raw,file=$(IMAGE) -m $(MACHINE_MEMORY) -boot order=$(MACHINE_BOOT_ORDER) -audiodev $(MACHINE_AUDIODEV),id=audio0 -machine pcspk-audiodev=audio0 -nographic

install:
ifndef DEVICE
	$(error device not defined, use 'make install DEVICE=/dev/sdb'; this is example)
endif
	@echo "installing..."
	@sudo dd if="$(IMAGE)" of="$(DEVICE)"

debug:
	$(CC) $(CFLAGS) -o fat tools/fat.c
	./fat $(IMAGE)

clean:
	@echo "[ CLEANING  ]"
	@make -C src clean BOOTLOADER=../$(BOOTLOADER_FILE) \
		KERNEL=../$(KERNEL_FILE) \
		OBJD=../$(OBJ_DIR) \
		BIND=../$(BIN_DIR) \
		DEBUG../=$(DEBUG)
	@rm -rf $(BUILD_DIR) $(IMAGE) fat

.PHONY: all clean compile prepare img run run-ng debug install
