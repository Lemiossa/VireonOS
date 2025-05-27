# =======================================
# config.mk - main Makefile config
# Created by Matheus Leme Da Silva
# =======================================

DEBUG := 0 # 1 = True, 0 = False
FUSER := 0

# ===== Operating system image =====
OSI_OSNAME := VireonOS
OSI_VOLUME_LABEL := VIREON-OS

# ===== Filesystem configuration =====
FS_FAT_TYPE := 16
FS_SECTOR_SIZE := 512
FS_TOTAL_SECTORS := 32768

# ===== Virtual machine configuration =====
MACHINE_MEMORY := 3M
MACHINE_BOOT_ORDER := a
MACHINE_AUDIODEV := alsa

# ===== Others =====
KERNEL_IMAGE := VIRX86
