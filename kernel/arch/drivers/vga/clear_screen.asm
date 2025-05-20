;
; arch/drivers/vga/clear_screen.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _clear_screen

section .text

_clear_screen:
	mov ah, 0x02
	mov bh, 0x00
	mov dh, 0x00
	mov dl, 0x00

	mov ah, 0x06
	mov al, 0x00
	mov bh, 0x07
	mov cx, 0x0000
	mov dx, 0x314f
	int 0x10
ret
