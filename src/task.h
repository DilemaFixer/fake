#ifndef TASK_H
#define TASK_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "arr.h"

#define DEFAULT_STACK_SIZE (2 * 1024)

typedef struct ctx_t {
    uint64_t x19, x20, x21, x22, x23, x24, x25, x26, x27, x28;
    uint64_t fp, lr, sp;
} ctx_t;

typedef struct task_stack_t {  // переименовано
    void* memory;
    size_t size;
} task_stack_t;

typedef struct task_t {
    ctx_t ctx;
    task_stack_t stack; 
    void *data;
    void (*instr)(void*);  // ← Добавляем это поле
} task_t;

extern arr_t* tasks;
extern size_t current_task;

void switch_context(task_t *from, task_t *to);
void go_task(task_t *task, void(*instr)(void*), void *data);
void handle_task_end(void);
void yield(void);
void go_init(void);
#endif
