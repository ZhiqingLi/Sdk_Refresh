#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "app_key.h"
#include "hal_trace.h"

#define TEST_KEY_UP     HAL_KEY_CODE_FN3
#define TEST_KEY_DOWN   HAL_KEY_CODE_FN4

#define TEST_KEY_UP_VALUE      0x12
#define TEST_KEY_DOWN_VALUE    0x34

extern void register_ble_key_handle_cb(void (*cb)(APP_KEY_STATUS));

extern "C" {
    extern void app_ble_key_notify(uint8_t len, uint8_t* value);
}

void app_ble_key_handle(APP_KEY_STATUS ble_key);

uint32_t app_ble_key_init()
{
    register_ble_key_handle_cb(app_ble_key_handle);
}

void ble_key_handle_up_key(enum APP_KEY_EVENT_T event)
{
    uint8_t value = TEST_KEY_UP_VALUE;
    switch(event) {
        case APP_KEY_EVENT_UP:
            app_ble_key_notify(sizeof(value), &value);
            break;
        case APP_KEY_EVENT_CLICK:
            //app_ble_key_notify(sizeof(value), &value);
            break;
        case APP_KEY_EVENT_DOUBLECLICK:
            break;
        default:
            TRACE("unregister up key event=%x",event);
    }
}

void ble_key_handle_down_key(enum APP_KEY_EVENT_T event)
{
    uint8_t value = TEST_KEY_DOWN_VALUE;
    switch(event) {
        case APP_KEY_EVENT_UP:
            app_ble_key_notify(sizeof(value), &value);
            break;
        case APP_KEY_EVENT_CLICK:
            //app_ble_key_notify(sizeof(value), &value);
            break;
        case APP_KEY_EVENT_DOUBLECLICK:
            break;
        default:
            TRACE("unregister up key event=%x",event);
    }
}

void app_ble_key_handle(APP_KEY_STATUS ble_key)
{
    if(ble_key.code != 0xff) {
        TRACE("app_ble_key_handle code:%x event:%x",ble_key.code, ble_key.event);
        switch (ble_key.code) {
            case TEST_KEY_UP:
                ble_key_handle_up_key((enum APP_KEY_EVENT_T)ble_key.event);
                break;
            case TEST_KEY_DOWN:
                ble_key_handle_down_key((enum APP_KEY_EVENT_T)ble_key.event);
                break;
            default:
                TRACE("app_ble_key_handle  undefined key");
                break;
        }
    }
}
