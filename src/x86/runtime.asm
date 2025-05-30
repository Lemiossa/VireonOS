;
; arch/x86/runtime.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]

global idiv_u
idiv_u:
	xor dx, dx
	div bx
ret

global imod
imod:
	cwd
	idiv bx
	mov ax, dx
ret

global imodu
imodu:
	xor dx, dx
	div bx
	mov ax, dx
ret

global lcmpul
lcmpul:
	cmp bx, dx
	ja .greeter
	jb .less
	cmp ax, cx
	ja .greeter
	jb .less
	xor ax, ax
ret
	.greeter:
		mov ax, 1
ret
	.less:
		mov ax, -1
ret

global lcmpl
lcmpl:
	cmp bx, dx
	jg .greeter
	jl .less
	cmp ax, cx
	ja .greeter
	jb .less
	xor ax, ax
ret
	.greeter:
		mov ax, 1
ret
	.less:
		mov ax, -1
ret

global lnegl
lnegl:
	neg word[bx]
	jnz .skip
	neg word[bx+2]
	inc word[bx+2]
	jmp .done
	.skip:
		neg word[bx+2]
	.done:
ret

global lmodl
lmodl:
	push cx
	push di
	call ldivl
	mov ax, cx
	mov bx, di
	pop di
	pop cx
ret

global laddl
laddl:
	add ax, [di]
	adc ax, [di+2]
ret

global ldivl
ldivl:
	push si
	push bp
	mov si, 0
	test bx, bx
	jns .check_divisor
	inc si
	neg bx
	neg ax
	sbb bx, 0
	.check_divisor:
		test dx, dx
		jns .do_divison
		inc si
		neg dx
		neg cx
		sbb dx, 0
	.do_divison:
		push si
		call ldivul
		pop si
		test si, 1
		jz .done
		neg bx
		neg ax
		sbb ax, 0
	.done:
		pop bp
		pop si
ret

global ldivul
ldivul:
	push si
	push bp
	mov si, cx
	mov bp, di
	xor di, di
	xor cx, cx
	mov word[.dividend], ax
	mov word[.dividend+2], bx
	mov word[.divisor], si
	mov word[.divisor+2], bp
	mov cx, 32
	.loop:
		shl word[.dividend], 1
		rcl word[.dividend+2], 1
		rcl word[.remainder], 1
		rcl word[.remainder+2], 1
		mov ax, word[.remainder]
		mov bx, word[.remainder+2]
		sub ax, word[.divisor]
		sbb bx, word[.divisor+2]
		jc .skip_sub
		mov word[.remainder], ax
		mov word[.remainder+2], bx
		inc word[.dividend]
	.skip_sub:
		loop .loop
		mov ax, word[.dividend]
		mov bx, word[.dividend+2]
		mov cx, word[.remainder]
		mov di, word[.remainder+2]
		pop bp
		pop si
ret
.dividend dd 0
.divisor dd 0
.remainder dd 0

global landul
landul:
	and ax, [di]
	and bx, [di+2]
ret

global laddul
laddul:
	and ax, [di]
	adc bx, [di+2]
ret

global lmodul
lmodul:
	push cx
	push di
	call ldivul
	mov ax, cx
	mov bx, di
	pop di
	pop cx
ret

global lsrul
lsrul:
	push cx
	mov cx, ax
	mov ax, [bx+2]
	.loop:
		test cx, cx
		jz .done
		shr ax, 1
		mov [bx+2], ax
		mov ax, [bx]
		rcr ax, 1
		mov [bx], ax
		dec cx
		jmp .loop
	.done:
		pop cx
ret

global lorul
lorul:
	or ax, [di],
	or bx, [di+2]
ret

global ltstl
ltstl:
	or ax, bx
	jnz .true
	xor ax, ax
ret
	.true:
		mov ax, 1
ret

global lsubul
lsubul:
	sub ax, [di]
	sbb bx, [di+2]
ret

global lincl
lincl:
	inc word [bx]
	jnz .done
	inc word [bx+2]
	.done:
ret
