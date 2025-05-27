; ==================================
; mbr.asm - the main bootloader file
; Created by Matheus Leme Da Silva
; Locate: Brazil
; ==================================

[BITS 16]
[ORG 0x0000]

%define FILE_ADDR	0x0800
%define FILENAME	'VIRX86     '   ; set this correctly! !ONLY 11 BYTES!

global _start
jmp short _start
nop

OemID							db 'VireonOS'	; OEM identifier. !ONLY 8 BYTES!
BytesPerSector		dw 512		; for compability
SectorsPerCluster	db 8
ReservedSectors		dw 1		; size of bootloader??
FatCount					db 2		; only 2 copies
RootEntries				dw 512		; good size
TotalSectors			dw 32768		; if (TotalSectors > 65535) { TotalSectors = 0; }
MediaType					db 0xF8		; 0xf8 for hard drives(HDs), 0xf0 for floppy(FDs)
SectorsPerFat			dw 32		; 0x0080
SectorsPerTrack		dw 63		; 0x003F
Heads							dw 16		; 0x00FF
HiddenSectors			dd 0		; 0x00000000
LargeSectors			dd 0
DriveNumber				db 0x80  	; 0x00 - the BIOS set this
Reserved					db 0		; 0x00
Signature					db 0x29
VolID							dd 'EMEL'	; VireonOS created by matheus *LEME* da silva
VolLabel					db 'VIR-OS     '
FatType						db 'FAT16   '

; end of the header =========================
; ===========================================

_start:
	cli
	mov ax, 0x07C0
	mov ds, ax
	mov es, ax
	xor ax, ax
	mov ss, ax
	mov sp, 0xFFFF
	sti

	mov [DriveNumber], dl

	; print the main message
	mov si, BootMSG
	call PrintString

	; Calculate root directory start
	movzx ax, byte [FatCount]
	mul word [SectorsPerFat]	; AX = FATs * SectorsPerFat
	add ax, [ReservedSectors]	; Add ***ReservedSectors***
	mov [RootDirStart], ax

	; Calculate root directory size ***in sectors***
	mov ax, [RootEntries]
	mov cx, 32			; bytes per entry
	mul cx
	div word [BytesPerSector]
	test dx, dx
	jz .no_round
	inc ax

.no_round:
	mov [RootDirSectors], ax

	; calculate de data area start
	mov cx, ax
	add ax, [RootDirStart]
	mov [DataStart], ax

	; Load root dir
	mov ax, [RootDirStart]
	mov bx, Buffer
	call ReadSectors

	; Find the file
	mov cx, [RootDirStart]
	mov di, Buffer

.find_file:
	cmp byte [di], 0 ; End of Dir?
	je .not_found

	cmp byte [di], 0xE5 ; deleted entry?
	je .next

	mov si, filename ; compare filename
	push di
	mov cx, 0x0B
	repe cmpsb
	pop di
	je .found

.next:
	add di, 0x20 ; next entry
	dec word[counter]
	jnz .find_file

.not_found:
	mov si, FileNotFoundMSG
	call PrintString
	jmp Reboot

.found:
	; Get first cluster
	mov ax, [di+0x1A]
	mov [Cluster], ax

	; Load FAT
	mov ax, [ReservedSectors]
	mov cx, [SectorsPerFat]
	mov bx, FatBuffer
	call ReadSectors

	; Set up destination buffer
	mov ax, FILE_ADDR
	mov es, ax
	xor bx, bx

.load_loop:
	mov ax, [Cluster]
	cmp ax, 0x0002 ; Valid Cluster?
	jb .done
	cmp ax, 0xFFF8 ; End of File?
	jae .done

	; Convert cluster to LBA
	sub ax, 0x0002
	movzx cx, byte[SectorsPerCluster]
	mul cx
	add ax, [DataStart]

	; Read the cluster
	push bx
	movzx cx, byte [SectorsPerCluster]
	call ReadSectors
	pop bx

	; Update buffer position
	movzx ax, byte[SectorsPerCluster]
	mul word [BytesPerSector]
	add bx, ax

	jnc .no_adj

	; Adjust segment if overflow

	mov dx, es
	add dx, 0x1000
	mov es, dx
	xor bx, bx

.no_adj:
	; Get nest cluster
	mov ax, [Cluster]
	shl ax, 1 ; Multiply by 2 (FAT16 = 2 bytes per entry)
	add ax, FatBuffer
	mov si, ax
	mov ax, [si]
	mov [Cluster], ax

	jmp .load_loop

.done:
	xor dx, dx
	mov dl, [DriveNumber]
	jmp FILE_ADDR:0000



; UTILITY ===================================
; ===========================================

; Read Sectors FUNCTION
; @param AX - LBA
; @param CX - Sectors to read
; @param ES:BX - Buffer
ReadSectors:
	pusha
.read:
	push cx
	push ax

	; Convert LBA to CHS
	xor dx, dx
	div word [SectorsPerTrack]
	inc dl ; Sector = (LBA % SPT)+ 1
	mov cl, dl

	xor dx, dx
	div word [Heads]
	mov dh, dl ; Head
	mov ch, al ; Cylinder (low 8 Bits)

	shl ah, 6
	or cl, ah

	mov dl, [DriveNumber]
	mov ax, 0x0201 ; read 1 sector

	int 0x13
	jc .error ; if carry != 0; Error

	pop ax
	inc ax
	add bx, [BytesPerSector]

	pop cx
	loop .read

	popa
	ret

.error:
	mov si, ErrorMSG
	call PrintString
	jmp Reboot


; Print a string
; @param ES:SI - string ptr
PrintString:
	pusha
	.loop:
		lodsb
		test al, al
		jz .done
		mov ah, 0x0E
		int 0x10
		jmp .loop
	.done:
		popa
		ret

; Reboot the system
Reboot:
	mov si, RebMSG
	call PrintString
	xor ah, ah
	int 0x16
	int 0x19

; DATA ======================================
; ===========================================

; messages
BootMSG			db 'Booting...', 0x0D, 0x0A, 0
ErrorMSG		db 'ERROR!', 0x0D, 0x0A, 0
FileNotFoundMSG		db 'File not found!', 0x0A, 0x0D, 0
RebMSG			db 'Press any key to reboot...', 0x0D, 0x0A, 0

; others
filename		db FILENAME

RootDirStart		dw 0
RootDirSectors		dw 0
DataStart		dw 0
Cluster			dw 0
counter			dw 1024

; END =======================================
; ===========================================

TIMES 510 - ($ - $$) db 0
DW 0xAA55

Buffer:
FatBuffer:
