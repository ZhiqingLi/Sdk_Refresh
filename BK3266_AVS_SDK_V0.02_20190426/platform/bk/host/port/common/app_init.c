/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "app_list.h"

void uw_app_uninit(void)
{
    int_t i;

    DBG_I(DJOS_DRIVER, ("JDRV: Starting to un-load non usb drivers\n"));

    /* Find the end of the list */
    for (i = 0; app_list[i].init; i++)
        ;

    /* Uninitialize in reverse order to the initialization */
    while (i--)
    {
        if(app_list[i].init_done)
            app_list[i].uninit();

        app_list[i].init_done = 0;
    }
}

result_t uw_app_init(void)
{
    result_t rc;
    int_t i;

    DBG_I(DJOS_DRIVER, ("JDRV: Starting to load Non-USB drivers\n"));

    for (i = 0; app_list[i].init; i++)
    {
        rc = app_list[i].init();
        if(rc)
            goto Error;

        app_list[i].init_done = 1;
    }

    return UWE_OK;

Error:
    uw_app_uninit();
    return rc;
}

