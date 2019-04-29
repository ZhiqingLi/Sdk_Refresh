/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _APP_LIST_H_
#define _APP_LIST_H_

#include <jos.h>

/* Auto-init for non usb drivers */
typedef struct {
    driver_init_t init;
    driver_uninit_t uninit;
    bool_t init_done;
} load_arr_t;

result_t bt_app_init(void);
void bt_app_uninit(void);

static load_arr_t app_list[] = {
#ifdef CONFIG_BLUETOOTH_APP
    { bt_app_init, bt_app_uninit, 0 },
#endif

    { NULL, NULL, 0 }
};

#endif

