
/* scheduler.h - Definições para o escalonador do BitOS */
#ifndef SCHEDULER_H
#define SCHEDULER_H

/* Definições de constantes */
#define MAX_TASKS 10
#define STACK_SIZE 256  /* 256 words por tarefa (512 bytes) - valor mais realista para 16 bits */

/* Estados de processo */
#define TASK_UNUSED   0
#define TASK_READY    1
#define TASK_RUNNING  2
#define TASK_BLOCKED  3
#define TASK_ZOMBIE   4

/* Estrutura para o contexto da tarefa (registradores) */
struct Task_Context {
    unsigned int ax, bx, cx, dx;
    unsigned int si, di, bp;
    unsigned int ds, es;
    unsigned int ip, cs, flags;
};

/* Estrutura de controle de tarefas (TCB - Task Control Block) */
struct Task {
    int id;                  /* ID da tarefa */
    int state;               /* Estado atual da tarefa */
    int priority;            /* Prioridade da tarefa (menor é maior prioridade) */
    int quantum;             /* Quantum restante da tarefa */
    unsigned int stack_pointer;      /* Ponteiro da pilha */
    unsigned int stack[STACK_SIZE];  /* Pilha da tarefa */
};

/* Variáveis globais e protótipos de funções */
extern struct Task tasks[MAX_TASKS];
extern int current_task;
extern int next_task_id;

/* Inicializa o escalonador */
void scheduler_init(void);

/* Cria uma nova tarefa */
int create_task(void (*entry_point)(void), int priority);

/* Implementa a troca de contexto - chamada por timer_tick */
void schedule(void);

/* Função para ceder voluntariamente a CPU */
void yield(void);

/* Função que realiza a troca de contexto efetiva (implementada em assembly) */
void task_switch(int new_task);

/* Funções auxiliares */
void block_task(int task_id);
void unblock_task(int task_id);
void terminate_task(int task_id);

#endif /* SCHEDULER_H */
