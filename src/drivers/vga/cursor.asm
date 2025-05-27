;
; drivers/vga/cursor.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _set_cursor
global set_cursor
global _enable_cursor
global _disable_cursor
global enable_cursor
global disable_cursor
global get_cursor_position
global _get_cursor_x_position
global _get_cursor_y_position

_set_cursor:
	push bp
	mov bp, sp
	push ax
	push bx
	push dx
	mov dh, [bp+6]
	cmp dh, 49
	ja .lines_exceded
	jmp .done
	.lines_exceded:
		mov dh, 49
	.done:
		mov dl, [bp+4]
		mov ah, 0x02
		xor bh, bh
		int 0x10
		pop dx
		pop bx
		pop ax
		pop bp
ret

set_cursor:
	cmp dh, 49
	jbe .ok
	mov dh, 49
	.ok:
		mov ah, 0x02
		xor bh, bh
		int 0x10
ret

enable_cursor:
	mov dx, 0x3d4
	mov al, 0x0a
	out dx, al
	inc dx
	in al, dx
	and al, 0xc0
	or al, 0x06
	out dx, al
	mov dx, 0x3d4
	mov al, 0x0b
	out dx, al
	inc dx
	in al, dx
	and al, 0xe0
	or al, 0x07
	out dx, al
ret

disable_cursor:
	mov dx, 0x3d4
	mov al, 0x0a
	out dx, al
	inc dx
	mov al, 0x20
	out dx, al
ret

_enable_cursor:
	call enable_cursor
ret

_disable_cursor:
	call disable_cursor
ret

get_cursor_position:
	mov ah, 0x03
	xor bh, bh
	int 0x10
ret

_get_cursor_x_position:
	call get_cursor_position
	xor ax, ax
	mov al, dl
ret

_get_cursor_y_position:
	call get_cursor_position
	xor ax, ax
	mov al, dh
ret
