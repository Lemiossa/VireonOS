/* scheduler.c - Implementação do escalonador do BitOS */
#include "sched.h"
#include <types.h>

/* Variáveis globais */
struct Task tasks[MAX_TASKS];
int current_task = -1;    /* Nenhuma tarefa rodando ainda */
int next_task_id = 0;
int ticks_per_quantum = 10; /* 10 ticks = 100ms a 100Hz */

extern uint ticks;

/* Inicializa o escalonador */
void scheduler_init(void) {
    int i;
    
    /* Inicializa as estruturas de tarefas */
    for (i = 0; i < MAX_TASKS; i++) {
        tasks[i].id = -1;
        tasks[i].state = TASK_UNUSED;
    }
    
    /* O timer já está configurado, não precisamos inicializá-lo aqui */
}

/* Cria uma nova tarefa */
int create_task(void (*entry_point)(void), int priority) {
    int i;
    unsigned int *stack_top;
    
    /* Procura um slot livre */
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_UNUSED) {
            break;
        }
    }
    
    /* Verifica se atingiu o limite de tarefas */
    if (i == MAX_TASKS) {
        return -1;  /* Não foi possível criar a tarefa */
    }
    
    /* Inicializa a estrutura da tarefa */
    tasks[i].id = next_task_id++;
    tasks[i].state = TASK_READY;
    tasks[i].priority = priority;
    tasks[i].quantum = ticks_per_quantum;
    
    /* Configura a pilha da tarefa */
    stack_top = &tasks[i].stack[STACK_SIZE - 1];
    
    /* Salva os valores iniciais dos registradores na pilha 
       Simula uma interrupção que ao retornar irá para o entry_point */
    
    /* Flags - Interrupções habilitadas */
    *--stack_top = 0x0200;
    
    /* CS - Segmento de código do ponto de entrada */
    *--stack_top = 0x0800;  /* Mesmo segmento do kernel */
    
    /* IP - Offset do ponto de entrada */
    *--stack_top = (unsigned int)entry_point;
    
    /* Salva registradores de uso geral com valores iniciais */
    *--stack_top = 0;  /* AX */
    *--stack_top = 0;  /* BX */
    *--stack_top = 0;  /* CX */
    *--stack_top = 0;  /* DX */
    *--stack_top = 0;  /* SI */
    *--stack_top = 0;  /* DI */
    *--stack_top = 0;  /* BP */
    
    /* Registradores de segmento */
    *--stack_top = 0x0800;  /* DS - Segmento de dados do kernel */
    *--stack_top = 0x0800;  /* ES - Segmento de dados do kernel */
    
    /* Salva o ponteiro da pilha para restaurar quando a tarefa for executada */
    tasks[i].stack_pointer = (unsigned int)stack_top;
    
    return tasks[i].id;
}

/* Encontra a próxima tarefa para executar - algoritmo round-robin com prioridade */
int find_next_task(void) {
    int i;
    int start = (current_task + 1) % MAX_TASKS;
    int lowest_priority = 999;
    int best_task = -1;
    
    /* Primeiro passo: procura a tarefa pronta com a maior prioridade (menor valor) */
    for (i = 0; i < MAX_TASKS; i++) {
        int task_index = (start + i) % MAX_TASKS;
        
        if (tasks[task_index].state == TASK_READY && tasks[task_index].priority < lowest_priority) {
            lowest_priority = tasks[task_index].priority;
            best_task = task_index;
        }
    }
    
    /* Se encontrou uma tarefa pronta, retorna ela */
    if (best_task != -1) {
        return best_task;
    }
    
    /* Se não encontrou nenhuma tarefa pronta, verifica se a atual ainda está em execução */
    if (current_task != -1 && tasks[current_task].state == TASK_RUNNING) {
        return current_task;
    }
    
    /* Se não houver tarefas prontas ou em execução, retorna -1 */
    return -1;
}

/* Função chamada a cada tick do timer (100Hz) - você já a implementou */
void timer_tick(void) {
	ticks++;
    /* Se nenhuma tarefa estiver em execução, tenta iniciar */
    if (current_task == -1) {
        schedule();
        return;
    }
    
    /* Decrementa o quantum da tarefa atual */
    if (--tasks[current_task].quantum <= 0) {
        /* Quantum expirou, redefine e escalona */
        tasks[current_task].quantum = ticks_per_quantum;
        schedule();
    }
}

/* Função principal de escalonamento */
void schedule(void) {
    int next;
    
    /* Se nenhuma tarefa estiver em execução, encontre a primeira tarefa pronta */
    if (current_task == -1) {
        next = find_next_task();
        if (next == -1) {
            return;  /* Nenhuma tarefa pronta */
        }
    } else {
        /* Encontra a próxima tarefa para executar */
        next = find_next_task();
        
        /* Se não houver mudança, retorna */
        if (next == current_task) {
            return;
        }
        
        /* Se a tarefa atual estava em execução, muda para pronta */
        if (tasks[current_task].state == TASK_RUNNING) {
            tasks[current_task].state = TASK_READY;
        }
    }
    
    /* Atualiza o estado da próxima tarefa */
    if (next != -1) {
        tasks[next].state = TASK_RUNNING;
        
        /* Efetua a troca de contexto */
        task_switch(next);
    }
}

/* Bloqueia uma tarefa específica */
void block_task(int task_id) {
    int i;
    
    /* Procura a tarefa pelo ID */
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id == task_id) {
            tasks[i].state = TASK_BLOCKED;
            
            /* Se estiver bloqueando a tarefa atual, faça um escalonamento */
            if (i == current_task) {
                yield();
            }
            return;
        }
    }
}

/* Desbloqueia uma tarefa específica */
void unblock_task(int task_id) {
    int i;
    
    /* Procura a tarefa pelo ID */
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id == task_id && tasks[i].state == TASK_BLOCKED) {
            tasks[i].state = TASK_READY;
            return;
        }
    }
}

/* Termina uma tarefa específica */
void terminate_task(int task_id) {
    int i;
    
    /* Procura a tarefa pelo ID */
    for (i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id == task_id) {
            tasks[i].state = TASK_ZOMBIE;
            
            /* Se estiver terminando a tarefa atual, faça um escalonamento */
            if (i == current_task) {
                yield();
            }
            return;
        }
    }
}

/* Função para ceder voluntariamente a CPU */
void yield(void) {
    schedule();
}
