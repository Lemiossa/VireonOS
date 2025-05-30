;
; drivers/vga/init.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _init_vga

; extern
extern clear_screen

section .text

_init_vga:

	mov ax, 0x0003
	int 0x10

	mov ax, 0x1112
	xor bl, bl
	int 0x10

	mov ax, 0x1200
	mov bl, 0x20
	int 0x10

	mov ax, 0x0040
	mov es, ax
	mov byte[es:0x0084], 49

	mov ah, 0x01
	mov cx, 0x0607
	int 0x10

	mov bh, 0x07
	call clear_screen
ret

