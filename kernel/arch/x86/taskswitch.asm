; taskswitch.asm - Implementação em NASM do mecanismo de troca de contexto
; Este arquivo contém as funções de baixo nível necessárias para o escalonador do BitOS

BITS 16

section .text

; Símbolos externos importados do C
extern _tasks         ; Array de estruturas Task
extern _current_task  ; Índice da tarefa atual

; Símbolos exportados para C
global _task_switch

; Constantes - offsets na estrutura Task
TASK_SIZE            equ 524   ; Tamanho da estrutura Task em bytes (sizeof(struct Task))
STACK_POINTER_OFFSET equ 8     ; Offset para o campo stack_pointer na estrutura Task

; Função para trocar para uma tarefa específica
; void task_switch(int new_task);
_task_switch:
    push    bp
    mov     bp, sp
    
    ; Salva os registradores
    push    ax
    push    bx
    push    cx
    push    dx
    push    si
    push    di
    push    ds
    push    es
    
    ; Obtém o parâmetro new_task
    mov     bx, [bp+4]      ; new_task é o primeiro parâmetro (bp+4)
    
    ; Verifica se já há uma tarefa em execução
    mov     ax, [_current_task]
    cmp     ax, -1          ; Verifica se há tarefa em execução
    je      .no_current_task
    
    ; Salva o contexto da tarefa atual
    mov     di, ax                  ; DI = índice da tarefa atual
    mov     ax, TASK_SIZE
    mul     di                      ; AX = DI * tamanho da estrutura Task
    add     ax, _tasks              ; AX = endereço base da tarefa atual
    mov     di, ax
    
    ; Salva o ponteiro da pilha atual na estrutura da tarefa
    mov     [di + STACK_POINTER_OFFSET], sp
    
.no_current_task:
    ; Atualiza a tarefa atual
    mov     [_current_task], bx
    
    ; Calcula o endereço da nova tarefa
    mov     di, bx                  ; DI = índice da nova tarefa
    mov     ax, TASK_SIZE
    mul     di                      ; AX = DI * tamanho da estrutura Task
    add     ax, _tasks              ; AX = endereço base da nova tarefa
    mov     di, ax
    
    ; Restaura o ponteiro da pilha da nova tarefa
    mov     sp, [di + STACK_POINTER_OFFSET]
    
    ; Restaura os registradores
    pop     es
    pop     ds
    pop     di
    pop     si
    pop     dx
    pop     cx
    pop     bx
    pop     ax
    
    ; Restaura o frame pointer e retorna
    pop     bp
    ret
