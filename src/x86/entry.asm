;
; arch/x86/entry.asm
; Created by Matheus Leme Da Silva
;

[BITS 16]

%define DEBUG 0
%define TIMER 1

; global
global _start
global _ticks
global _bootdrive

; extern
extern _kmain
;extern

KSEG		equ 0x0800
KSTK		equ 0x9000

section .text

_start:
	mov ax, KSEG
	mov ds, ax
	mov es, ax
	cli
	mov ss, ax
	mov ax, KSTK
	mov sp, ax
	sti

	mov [_bootdrive], dl

%if TIMER = 1
	cli
	mov ax, 0
	mov es, ax ; IVT segment

	mov ax, [es:0x08*4]
	mov [old_timer_offset], ax
	mov ax, [es:0x08*4+2]
	mov [old_timer_segment], ax

	mov word[es:0x08*4], IRQ0_handler
	mov word[es:0x08*4+2], KSEG

	sti
%endif

	call _kmain

%if TIMER = 1
	cli
	mov ax, 0
	mov es, ax
	mov ax, [old_timer_offset]
	mov [es:0x08*4], ax
	mov ax, [old_timer_segment]
	mov [es:0x08*4+2], ax
	sti
%endif

	mov si, kendmsg
	call print_string

	xor ah, ah
	int 0x16
	int 0x19

	jmp $

print_string:
	pusha
	.loop:
		lodsb
		test al, al
		jz .done
		mov ah, 0x0e
		int 0x10
		jmp .loop
	.done:
		popa
ret

IRQ0_handler:
	pusha
	push ds

	mov ax, KSEG
	mov ds, ax

	inc word[_ticks]
	.done:
		%if DEBUG = 1
			push ax
			mov ah, 0x0e
			mov al, '.'
			int 0x10
			pop ax
		%endif

		mov al, 0x20
		out 0x20, al

		pop ds
		popa
iret

_ticks dw 0x0000
_tick_overflows dw 0x0000
_bootdrive db 0x00
section .data
%if TIMER = 1
	old_timer_offset dw 0x0000
	old_timer_segment dw 0x0000
%endif
	kendmsg	db 'kernel ended! press any key to reboot...', 0x00
