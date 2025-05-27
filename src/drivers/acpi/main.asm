;
; arch/drivers/acpi/main.asm
; Created by Matheus Leme Da Silva
;
[BITS 16]
section .data
	; signatures
	RSDP_SIG db 'RSD PTR ', 0
	FACP_SIG db 'FACP', 0
	DSDT_SIG db 'DSDT', 0

	; dynamic data structures
	acpi_pm1a_cnt_port dw 0
	acpi_pm1b_cnt_port dw 0
	acpi_rst_reg db 0
	acpi_rst_addr db 0
	acpi_rst_val db 0
	acpi_reset_reg_addr dq 0
	acpi_slp_typa dw 0
	acpi_slp_typb dw 0
	acpi_s3_typa dw 0
	acpi_s3_typa dw 0
	acpi_worked db 0
	acpi_version db 0

section .text
; global
global _acpi_init
global _acpi_shutdown
global _acpi_reboot
global _acpi_sleep
global _apm_shutdown
global _legacy_shutdown
global _legacy_reboot
global _shutdown
global _reboot

; initialize ACPI
; no parameters
; void acpi_init(void);
_acpi_init:
	push bp
	mov bp, sp
	pushad
	call find_acpi_tables
	xor ax, ax
	cmp byte[acpi_worked], 1
	jne .done
	mov ax, 1
	.done:
		popad
		mov ax, [acpi_worked]
		pop bp
ret


; shutdown the computer - tries all methods
; no porameters;
; void shutdown(void);
_shutdown:
	push bp
	mov bp, sp
	pushad
	call _acpi_shutdown
	call _apm_shutdown
	call _legacy_shutdown
	.done:
		popad
		pop bp
ret

; reboot the computer - tries all mathods
; no parameters
; void reboot(void);
_reboot:

