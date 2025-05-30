;
; hardware.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

zero_idt:
	dw 0
	dd 0

; global

; shutdown
global legacy_shutdown
global apm_shutdown
global _shutdown

; reboot
global legacy_reboot
global apm_reboot
global _reboot

_shutdown:
	call apm_shutdown
	call legacy_shutdown
	.done:
ret

_reboot:
	call legacy_reboot
ret

apm_shutdown:
	pushad
	mov ax, 0x5300
	xor bx, bx
	int 0x15
	jc apm_failed
	cmp bx, 0x504d
	jne apm_failed
	mov ax, 0x5301
	xor bx, bx
	int 0x15
	jc apm_failed
	mov ax, 0x530e
	xor bx, bx
	mov cx, 0x0102
	int 0x10
	jc try_apm_1_1

apm_execute:
	mov ax, 0x5307
	mov bx, 0x0001
	mov cx, 0x0001
	int 0x15
	mov ax, 0x5307
	mov bx, 0x0001
	mov cx, 0x0003
	int 0x15
	popad
	xor ax, ax
ret

try_apm_1_1:
	mov ax, 0x530e
	xor bx, bx
	mov cx, 0x0101
	int 0x15
	jc apm_failed
	jmp apm_execute

apm_failed:
	popad
	xor ax, ax
ret

legacy_shutdown:
	pushad
	mov dx, 0xcf9
	mov al, 0x0e
	out dx, al
	mov al, 0xfe
	out 0x64, al
	mov dx, 0x4004
	mov al, 0x00
	out dx, al
	mov dx, 0x2001
	mov al, 0x00
	out dx, al
	lidt [zero_idt]
	int 0x03
	popad
	xor ax, ax
ret

legacy_reboot:
	pushad
	mov al, 0xfe
	out 0x64, al
	mov al, 0x64
	out 0x43, al
	jmp short $+2
	mov al, 0xfe
	out 0x64, al
	mov al, 0x06
	mov dx, 0xcf9
	out dx, al
	lidt [zero_idt]
	int 0x03
	popad
ret
