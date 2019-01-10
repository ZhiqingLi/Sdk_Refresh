/***
 * besbt.h
 */

#ifndef BESBT_H
#define BESBT_H

enum BESBT_HOOK_USER_T {
    BESBT_HOOK_USER_0 = 0,
    BESBT_HOOK_USER_1,
    BESBT_HOOK_USER_2,    
    BESBT_HOOK_USER_3,
    BESBT_HOOK_USER_QTY
};

typedef void (*BESBT_HOOK_HANDLER)(void);

#ifdef __cplusplus
extern "C" {
#endif

void BesbtInit(void);
void BesbtThread(void const *argument);
int Besbt_get_sco_num(void);
int Besbt_hook_handler_set(enum BESBT_HOOK_USER_T user, BESBT_HOOK_HANDLER handler);
unsigned char *randaddrgen_get_bt_addr(void);
unsigned char *randaddrgen_get_ble_addr(void);
const char *randaddrgen_get_btd_localname(void);
const char *randaddrgen_get_btd_blename(void);

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
