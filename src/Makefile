#
# Makefile - Main Makefile
# Created by Matheus Leme Da Silva
#

ifndef DEBUG
	DEBUG=false
endif

ifndef BIND
	BIND=build/bin
endif

ifndef OBJD
	OBJD=build/obj
endif

# programs
AS=nasm
LD=ld86
CC=bcc
AR=ar86

# kernel
ASFLAGS=-dOFM -f as86
ifeq ($(DEBUG),true)
	LDFLAGS=-d -s -M -N
else
	LDFLAGS=-d -s
endif
ARFLAGS=r
CFLAGS=-Dconst= -O -ansi -I. -I./include

# bootloader
ASFLAGSX86=-f bin

SRC=\
		x86/entry.asm \
		x86/runtime.asm \
		x86/panic.c \
		x86/panicx86.asm \
		utils/format_str.c \
		utils/kmemcpy.c \
		utils/console.c \
		utils/kputchar.c \
		utils/kputs.c \
		utils/kprintf.c \
		drivers/vga/screen.asm \
		drivers/vga/init.asm \
		drivers/vga/putc.asm \
		drivers/vga/putcat.c \
		drivers/vga/cursor.asm \
		drivers/keyboard/main.asm \
		drivers/disk/main.asm \
		drivers/disk/cdecl.asm \
		x86/hardware.asm \
		kernel.c

OBJ=$(patsubst %.asm,$(OBJD)/%.o,$(patsubst %.c,$(OBJD)/%.o,$(SRC)))
KERNEL?=virx86
BOOTLOADER?=bootloader

all: $(BOOTLOADER) $(KERNEL)

$(KERNEL): $(OBJ)
	@echo "  LD        $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJ)

$(BOOTLOADER):
	@echo "  AS86      $@"
	@$(AS) $(ASFLAGSX86) boot/mbr.asm -o $@

$(OBJD)/%.o: %.asm
	@echo "  AS        $@"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

$(OBJD)/%.o: %.c
	@echo "  CC        $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@echo "  RM        $(OBJ) $(KERNEL) $(BOOTLOADER)"
	@-rm -f $(OBJ) $(KERNEL) $(BOOTLOADER)

.PHONY: all clean
