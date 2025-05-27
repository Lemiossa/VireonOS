;
; arch/drivers/vga/putc.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _vgaputc
global _vga_putc

; print a character in the screen with a teletype
; @param AL - character
; @param BL - Attr
_vgaputc:
	push ax
	push bx
	push cx
	push dx
	push es

	mov ah, 0x03
	mov bh, 0x00
	int 0x10

	; DH - line
	; DL - column

	cmp al, 0x0a ; \n
	je .newline
	cmp al, 0x0d ; \r
	je .carriage_return
	cmp al, 0x09 ; \t
	je .tab
	cmp al, 0x08 ; \b
	je .backspace

	mov ah, 0x09
	mov cx, 1
	int 0x10

	inc dl
	cmp dl, 80
	jl .set_cursor
	; end of line: jump to new line
	mov dl, 0
	inc dh
	jmp .check_scroll

	.backspace:
		cmp dl, 0
		je .check_prev_line
		dec dl
		mov ah, 0x09
		mov al, ''
		mov cx, 1
		int 0x10
		jmp .set_cursor
	.check_prev_line:
		cmp dh, 0
		je .set_cursor
		dec dh
		mov dl, 79
		jmp .set_cursor
	.tab:
		push ax
		mov ah, 0x0e
		mov al, ' '
		int 0x10
		mov al, ' '
		int 0x10
		pop ax
		mov ah, 0x03
		mov bh, 0x00
		int 0x10
		jmp .set_cursor
	.newline:
		inc dh
		mov dl, 0
		jmp .check_scroll
	.carriage_return:
		mov dl, 0
		jmp .set_cursor
	.check_scroll:
		cmp dh, 50
		jl .set_cursor

		mov ah, 0x06
		mov al, 1
		mov bh, 0x07
		mov ch, 0x0000
		mov cl, 0x0000
		mov dh, 49
		mov dl, 79
		int 0x10

		mov dh, 49
		mov dl, 0
	.set_cursor:
		mov ah, 0x02
		mov bh, 0
		int 0x10

		pop es
		pop dx
		pop cx
		pop bx
		pop ax
ret

_vga_putc:
	push bp
	mov bp, sp
	mov al, [bp+4]
	mov bl, [bp+6]
	call _vgaputc
	pop bp
ret
