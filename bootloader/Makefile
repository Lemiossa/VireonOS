# ==================================
# Makefile - bootloader Makefile
# Created by Matheus Leme Da Silva
# ==================================
AS := nasm
LD := ld86
ASFLAGS := -O0 -f as86
LDFLAGS := -d -s -T0x0
SRC := bootsector/mbr.asm
OBJ := $(patsubst %.asm, build/obj/%.o, $(SRC))
TARGET ?= mbr.elf
all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "  LD        $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJ)

build/obj/%.o: %.asm
	@echo "  AS        $@"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	@echo "  RM        $(OBJ) $(TARGET)"
	@rm -f $(OBJ) $(TARGET)

.PHONY: all clean
