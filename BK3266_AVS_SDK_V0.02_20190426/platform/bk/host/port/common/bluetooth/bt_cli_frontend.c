/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bluetooth.h>
#include "bt_app_internal.h"

result_t frontend_init(void)
{
    return UWE_OK;
}

void frontend_uninit(void)
{
}

#ifdef BEKEN_DEBUG
result_t frontend_notify(const char *data, unsigned int len)
{
    /* Print out whatever we couldn't parse */
    jprintf("[%s]\r\n", data);

    return UWE_OK;
}
#endif
