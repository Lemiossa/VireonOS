;
; drivers/vga/screen.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global clear_screen
global _clear_screen

section .text

; clear screen with attr
; params:
;		BH - attr
clear_screen:
	push bx
	mov ah, 0x02
	mov bh, 0x00
	mov dh, 0x00
	mov dl, 0x00
	int 0x10
	pop bx
	mov ah, 0x06
	mov al, 0x00
	mov cx, 0x0000
	mov dx, 0x314f
	int 0x10
ret

_clear_screen:
	push bp
	mov bp, sp
	mov bh, [bp+4]
	call clear_screen
	pop bp
ret
