;
; drivers/disk/cdecl.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; extern
extern init_disk


; void init_disk(uint drive_num);
global _init_disk
_init_disk:
	push bp
	mov bp, sp
	mov dl, [bp+4]
	call init_disk
	pop bp
ret
