;
; drivers/keyboard/main.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

; global
global _getch
global _kbhit
global _getkey
global _getsc
global getch
global kbhit
global getkey
global getsc

; int kbhit(void);
_kbhit:
	call kbhit
ret

; char getch(void);
_getch:
	call getch
ret

; uchar getsc(void);
_getsc:
	call getsc
ret

; uint getkey(void);
_getkey:
	call getkey
ret

; kbhit
; return:
;		AX - 1 if true, 0 if false
kbhit:
	mov ah, 0x01
	int 0x16
	jz .no_key
	mov ax, 1
	jmp .done
	.no_key:
		xor ax, ax
	.done:
ret

; getch
getch:
	xor ah, ah
	int 0x16
	xor ah, ah
ret

; get scancode
getsc:
	xor ah, ah
	int 0x16
	mov al, ah
	xor ah, ah
ret

; getkey
getkey:
	xor ah, ah
	int 0x16
ret
