;
; drivers/disk/main.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; extern
extern _bootdrive

; Driver constants
%define MAX_SECTORS			65535
%define SECTOR_SIZE			512
%define MAX_RETRIES			3

; Error codes
%define ERR_SUCCESS			0
%define ERR_INVALID			1
%define ERR_READ_FAIL		2
%define ERR_WRITE_FAIL	3
%define ERR_NOT_READY		4
%define ERR_TIMEOUT			5

section .data
	current_drive					db 0x00
	sectors_per_track			dw 63
	heads_count						dw 16
	last_error						dw 0
section .text


; Initialize disk driver
; Params:
;		DL = drive number
; Return:
;		AL Error code
global init_disk
init_disk:
	push dx
	push cx
	; store drive number
	mov [current_drive], dl
	; reset disk system
	mov ah, 0x00
	int 0x13
	jc .init_error
	; get drive parameters
	mov ah, 0x08
	mov dl, [current_drive]
	int 0x13
	jc .init_error
	; Store Geometry
	and cx, 0x3f
	mov [sectors_per_track], cx
	inc dh
	mov [heads_count], dx
	mov ax, ERR_SUCCESS
	mov [last_error], ax
	pop cx
	pop dx
ret

.init_error:
	mov ax, ERR_NOT_READY
	mov [last_error], ax
	pop cx
	pop dx
ret


