# ==================================
# Makefile - main Makefile
# Created by Matheus Leme Da Silva
# ==================================

MAKEFLAGS += --no-print-directory

include config.mk

CC ?= gcc
CFLAGS := -Wall -Wextra -O2

# ===== Directories =====
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin

# ===== Files =====
IMAGE := $(OSI_OSNAME).img
BOOTLOADER_FILE := $(BIN_DIR)/bootloader.elf
KERNEL_FILE := $(BIN_DIR)/$(KERNEL_IMAGE)

# ==================================
# Targets
# ==================================

all: $(IMAGE)

prepare:
	@echo "  MKDIR     $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)

bootloader:
	@echo "[ BOOTLOADER ]"
	@$(MAKE) -C bootloader TARGET=../$(BOOTLOADER_FILE) DEBUG=$(DEBUG)

kernel:
	@echo "[  KERNEL   ]"
	@mkdir -p kernel/lib/include
	@#cp -f $(LIBC_FILE) kernel/lib/
	@#cp -rf libc/include kernel/lib
	@$(MAKE) -C kernel TARGET=../$(KERNEL_FILE) DEBUG=$(DEBUG)

$(IMAGE): prepare bootloader kernel
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
	@$(MAKE) -C bootloader clean
	@$(MAKE) -C kernel clean
	@rm -rf $(BUILD_DIR) $(IMAGE) fat

.PHONY: all clean bootloader kernel prepare libc img run run-ng debug install
