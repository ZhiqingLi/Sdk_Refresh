/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "includes.h"
#include "jinit.h"

result_t j_stack_init(uw_args_t *args)
{
    if (!args)
        args = get_uw_args();

    return jstart_stack(args);
}

/* Stack unload */
void j_stack_uninit(void)
{
    jstop_stack();
}

result_t os_port_init(void)
{
    timer_pwm1_pt2_init();
    compute_cpu_speed();
    return UWE_OK;
}

void os_port_uninit(void)
{
}
