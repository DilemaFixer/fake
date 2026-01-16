#include "task.h"
#include "arr.h"
#include <string.h>

extern arr_t* tasks;
extern size_t current_task;

 void task1(void *data) {
    char *name = (char*)data;
    for (int i = 0; i <= 5; i++) {
        printf("%s: iteration %d\n", name, i);
        yield();
    }
    printf("%s: done\n", name);
}

void task2(void *data) {
    char *name = (char*)data;
    for (int i = 0; i <= 5; i++) {
        printf("%s: iteration %d\n", name, i);
        yield();
    }
    printf("%s: done\n", name);
}

void make_task(void(*task)(void*), void* data) {
    task_t *t1 = malloc(sizeof(task_t));
    t1->stack.memory = malloc(DEFAULT_STACK_SIZE);
    t1->stack.size = DEFAULT_STACK_SIZE;
    go_task(t1, task, data);
}

int main(void) {
    printf("=== Task Scheduler Test ===\n\n");

    printf("[DEBUG] Creating tasks array\n");
    tasks = new_arr(1);

    printf("[DEBUG] Initializing main task\n");
    go_init();

    printf("[DEBUG] Creating task1\n");
    make_task(task1, "Task1");

    printf("[DEBUG] Creating task2\n");
    make_task(task2, "Task2");

    printf("Tasks initialized, starting...\n\n");

    while (tasks->count > 1) {
        printf("main : yield \n");
        yield();
    }

    printf("\n=== All tasks completed ===\n");
    return 0;
}
