#include "task.h"

arr_t* tasks = NULL;
size_t current_task = 0;

void go_init(void) {
    task_t *main_task = malloc(sizeof(task_t));
    memset(main_task, 0, sizeof(task_t));
    main_task->stack.memory = NULL;
    main_task->stack.size = 0;

    arr_push(tasks, main_task);
}

void __attribute__((naked)) switch_context(task_t *from, task_t *to) {
    __asm__(
        "cbz x0, 1f\n"
        "stp x19, x20, [x0, #0]\n"
        "stp x21, x22, [x0, #16]\n"
        "stp x23, x24, [x0, #32]\n"
        "stp x25, x26, [x0, #48]\n"
        "stp x27, x28, [x0, #64]\n"
        "str x29, [x0, #80]\n"
        "str x30, [x0, #88]\n"
        "mov x9, sp\n"
        "str x9, [x0, #96]\n"
        
        "1:\n"
        "ldp x19, x20, [x1, #0]\n"
        "ldp x21, x22, [x1, #16]\n"
        "ldp x23, x24, [x1, #32]\n"
        "ldp x25, x26, [x1, #48]\n"
        "ldp x27, x28, [x1, #64]\n"
        "ldr x29, [x1, #80]\n"
        "ldr x30, [x1, #88]\n"
        "ldr x9, [x1, #96]\n"
        "mov sp, x9\n"
        "ret\n"
    );
}


static void __attribute__((naked)) task_trampoline(void) {
    asm volatile(
        "mov x0, x19\n"                 // x0 = data
        "mov x30, x21\n"                // in the end , will jump to handle_task_end
        "br x20\n"                     // call
        //"br x21\n" //add this line , if remove "move x30, x21\n" will work the same
    );
}


void go_task(task_t *task, void(*instr)(void*), void *data) {
    ctx_t *ctx = &task->ctx;
    memset(ctx, 0, sizeof(*ctx));
    
    ctx->x19 = (uint64_t)data;
    ctx->x20 = (uint64_t)instr;
    ctx->x21 = (uint64_t)handle_task_end;
    
    uint64_t stack_top = (uint64_t)(task->stack.memory + task->stack.size);
    stack_top = stack_top & ~15ULL;
    
    printf("[DEBUG] go_task: stack=%p, size=%zu, aligned_top=0x%llx\n",
           task->stack.memory, task->stack.size, stack_top);
    
    ctx->sp = stack_top;
    ctx->fp = stack_top;
    ctx->lr = (uint64_t)task_trampoline;
    
    arr_push(tasks, task);
}

void handle_task_end(void) {
    printf("=== handle_task_end func was call ===\n\n");
    task_t *finished_task = (task_t*)tasks->items[current_task];

    arr_remove(tasks, current_task);

    if (current_task >= tasks->count) {
        current_task = 0;
    }

    if (tasks->count == 0) {
        free(finished_task->stack.memory);
        free(finished_task);
        exit(0);
    }

    task_t *next_task = (task_t*)tasks->items[current_task];

    switch_context(NULL, next_task);
}

void yield(void) {
    size_t prev = current_task;
    current_task = (current_task + 1) % tasks->count;
    
    task_t *from = (task_t*)tasks->items[prev];
    task_t *to = (task_t*)tasks->items[current_task];
    
    switch_context(from, to);
}
