# ==================================
# Makefile - main Makefile
# Created by Matheus Leme Da Silva
# ==================================

ifndef DEBUG
	DEBUG := 0
endif

# -------------------
# Compilers & Tools
# -------------------
CC          := bcc
LD          := ld86
AS          := nasm
AR          := ar86

# -------------------------
# Compiler & Linker Flags
# -------------------------
CFLAGS      := -Dconst= -O -ansi -I./include -I./lib/include
ifeq ($(DEBUG),true)
	LDFLAGS     := -d -s -M -N
else
	LDFLAGS     := -d -s
endif
ASFLAGS     := -dOFM -f as86
ARFLAGS     := r

# ------------------------------
# Source Files & Directories
# ------------------------------
# Kernel source files
KERNEL        := \
				arch/x86/entry.asm \
				arch/x86/runtime.asm \
				\
				\
				\
				arch/console.c \
				print_utils.c \
				kvprintf.c \
				kprintf.c \
				\
				\
				\
				arch/drivers/vga/init.asm \
				arch/drivers/vga/clear_screen.asm \
				arch/drivers/vga/putc.asm \
				arch/drivers/vga/set_cursor.asm \
				\
				\
				\
				kernel.c

# External libraries
LIBS        :=

# ------------------------
# Object Files & Target
# ------------------------
# Object files derived from source files
KOBJ        := $(patsubst %.asm,build/obj/%.o,$(patsubst %.c,build/obj/%.o,$(KERNEL)))

# Kernel binary target
TARGET      ?= kernel.bin

# -----------------------
# Build Targets
# -----------------------

# Default target: Build the kernel
all: $(TARGET)

# Link the object files and create the final kernel binary
$(TARGET): $(KOBJ) $(LIBS)
	@echo "  LD        $@"
	@$(LD) $(LDFLAGS) -o $@ $(KOBJ) $(LIBS)

# --------------------------
# Compilation Rules
# --------------------------

# Assembly (ASM) files to object files
build/obj/%.o: %.asm
	@echo "  AS        $@"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

# C files to object files
build/obj/%.o: %.c
	@echo "  CC        $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<

# ---------------------
# Clean Target
# ---------------------
clean:
	@echo "  RM        $(KOBJ) $(TARGET)"
	@-rm -f $(KOBJ) $(TARGET)

# Declare phony targets (not actual files)
.PHONY: all clean libk
