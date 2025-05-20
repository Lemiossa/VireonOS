;
; arch/drivers/vga/set_cursor.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _set_cursor

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
