;
; x86/panicx86.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]
section .text
; global
global _capture_registers
global _capture_stack_trace

; void capture_registers(regs_t regs);
_capture_registers:
	push bp
	mov bp, sp
	push di
	mov di, [bp+4]
	mov [di+0], ax
	mov [di+2], bx
	mov [di+4], cx
	mov [di+6], dx
	mov [di+8], si
	mov ax, [bp-2]
	mov [di+10], ax
	mov [di+12], bp
	mov ax, sp
	add ax, 6
	mov [di+14], ax
	mov ax, cs
	mov [di+16], ax
	mov ax, ds
	mov [di+18], ax
	mov ax, es
	mov [di+20], ax
	mov ax, ss
	mov [di+22], ax
	mov ax, [bp+2]
	mov [di+24], ax
	pushf
	pop ax
	mov [di+26], ax
	pop di
	pop bp
ret

; void capture_stack_trace(stack_frame_t *frame, u8 count);
_capture_stack_trace:
	push bp
	mov bp, sp
	push di
	push si
	push cx
	mov di, [bp+4]
	mov si, [bp+6]
	mov byte[si], 0
	mov cx, cs
	mov [di], cx
	mov cx, [bp+2]
	mov [di+2], cx
	inc byte[si]
	add di, 4
	mov bp, [bp+0]
	mov cx, 1
	.trace_loop:
		cmp cx, 10
		jae .done
		cmp bp, 0
		je .done
		mov ax, [bp+2]
		cmp ax, 0
		je .done
		mov ax, cs
		mov [di], ax
		mov ax, [bp+2]
		mov [di+2], ax
		inc byte[si]
		inc cx
		add di, 4
		mov bp, [bp]
		jmp .trace_loop
	.done:
		pop cx
		pop si
		pop di
		pop bp
ret
