
    .syntax unified

    .extern cur_task_id
    .extern task_sp

    .text
    .align    2
    .thumb_func
    .globl    task_run
    .type    task_run, %function
/* void task_run(uint32_t task_id) */
task_run:

    ldr r1, =cur_task_id
    ldr r2, [r1]
    str r0, [r1]
    ldr r3, =task_sp
    push {r4-r12,lr}
    str sp, [r3, r2, lsl #2]
    ldr sp, [r3, r0, lsl #2]
    isb
    pop {r4-r12,lr}
    bx lr

    .pool
    .size    task_run, . - task_run

    .end
