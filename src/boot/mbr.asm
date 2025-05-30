;
; mbr.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]
[ORG 0x7c00]

%define FILE_ADDR			0x0800
%define FILENAME			'VIRX86     '

%strlen FILENAMELEN FILENAME
%if FILENAMELEN != 11
	%error "FILENAME not have 11 bytes"
%endif

jmp short main
nop

OemID								db 'VireonOS'			; 8 bytes
BytesPerSector			dw 512						; FOR COMPABILITIE
SectorsPerCluster		db 8
ReservedSectors			dw 1
FatCount						db 2
RootEntries					dw 512
TotalSectors				dw 32768
MediaType						db 0xf8
SectorsPerFat				dw 32
SectorsPerTrack			dw 63
Heads								dw 16
HiddenSectors				dd 0
LargeSectors				dd 0
DriveNumber					db 0x80
Reserved						db 0
Signature						db 0x29
VolID								dd 'EMEL'
VolLabel						db 'VIR-OS     '			; 11 bytes
FatType							db 'FAT16   '			; 8 bytes

main:
	xor ax, ax
	cli
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	mov [DriveNumber], dl

	mov si, BootMSG
	call PrintString

	; calculate start of root dir
	movzx ax, byte[FatCount]
	mul word[SectorsPerFat]
	add ax, [ReservedSectors]
	mov [RootDirStart], ax

	; calculate size of root dir
	mov ax, [RootEntries]
	shr ax, 4
	mov [RootDirSectors], ax

	add ax, [RootDirStart]
	mov [DataStart], ax

	; load root dir
	mov ax, [RootDirStart]
	mov bx, Buffer
	mov cx, 1
	call ReadSectors

	mov di, Buffer
	mov cx, 16

	.find_file:
		cmp byte[di], 0 ; valid file?
		je .not_found
		cmp byte[di], 0xe5 ; deleted?
		je .next

		push di
		push cx
		mov si, filename
		mov cx, 11
		repe cmpsb
		pop cx
		pop di
		je .found

	.next:
		add di, 32
		loop .find_file
		inc word[RootDirStart]
		dec word[RootDirSectors]
		jnz .load_next_root

	.not_found:
		mov si, FileNotFoundMSG
		call PrintString
		jmp Reboot

	.load_next_root:
		mov ax, [RootDirStart]
		mov bx, Buffer
		mov cx, 1
		call ReadSectors
		mov di, Buffer
		mov cx, 16
		jmp .find_file

	.found:
		mov ax, [di+26] ; first cluster
		mov [Cluster], ax

		; Load FAT
		mov ax, [ReservedSectors]
		mov cx, [SectorsPerFat]
		mov bx, 0x7e00
		call ReadSectors

		; Configure the buffer
		mov ax, FILE_ADDR
		mov es, ax
		xor di, di

	.load_loop:
		mov ax, [Cluster]
		cmp ax, 0x0002
		jb .done
		cmp ax, 0xfff8
		jae .done

		; Convert cluster to LBA
		sub ax, 2
		movzx cx, byte[SectorsPerCluster]
		mul cx
		add ax, [DataStart]

		; Read the cluster
		push di
		push es
		mov bx, di
		movzx cx, byte[SectorsPerCluster]
		call ReadSectors
		pop es
		pop di

		; Update buffer position
		movzx ax, byte[SectorsPerCluster]
		shl ax, 9 ; multiply per 512
		add di, ax
		jnc .no_overflow

		mov ax, es
		add ax, 0x100
		mov es, ax
		xor di, di

	.no_overflow:
		; Get next cluster
		mov ax, [Cluster]
		shl ax, 1
		add ax, 0x7e00
		mov si, ax
		mov ax, [si]
		mov [Cluster], ax
		jmp .load_loop

	.done:
		push ds
		pop es
		mov dl, [DriveNumber]
		jmp FILE_ADDR:0000

; ReadSectors
; Params:
;		AX - LBA
;		CX - Sector count
;		ES:BX - Buffer
ReadSectors:
	pusha
	push cx
	push ax

	xor dx, dx
	div word [SectorsPerTrack]
	inc dl
	mov cl, dl ; Sector
	xor dx, dx
	div word [Heads]
	mov dh, dl ; Head
	mov ch, al ; Cylinder

	shl ah, 6
	or cl, ah

	mov dl, [DriveNumber]
	mov ax, 0x0200
	add ax, cx ; Number of sectors to read
	int 0x13
	jc .error

	pop ax
	add ax, cx
	mov dx, cx
	shl dx, 9
	add bx, dx
	jnc .no_seg_adjust

	mov dx, es
	add dx, 0x100 ; Increment the segment
	mov es, dx
	xor bx, bx

	.no_seg_adjust:
		pop cx
		popa
		ret

	.error:
		mov si, ErrorMSG
		call PrintString
		jmp Reboot

; PrintString
;		ES:SI - Pointer of string
PrintString:
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

; Reboot
Reboot:
	mov si, RebMSG
	call PrintString
	xor ah, ah
	int 0x16
	int 0x19


; DATA

; DATA.messages
	BootMSG							db 'Boot...', 0
	ErrorMSG						db 'ERROR!', 0
	FileNotFoundMSG			db 'Not found!', 0
	RebMSG							db '[...]', 0
	filename						db FILENAME

; DATA.others
	RootDirStart				dw 0
	RootDirSectors			dw 0
	DataStart						dw 0
	Cluster							dw 0

; END

TIMES 510-($-$$) db 0
DW 0xAA55

Buffer:

