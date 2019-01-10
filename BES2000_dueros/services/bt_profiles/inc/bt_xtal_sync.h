#ifndef __BT_XTAL_SYNC_H__
#define __BT_XTAL_SYNC_H__

#ifdef __cplusplus
extern "C" {
#endif

enum BT_XTAL_SYNC_MODE_T {
    BT_XTAL_SYNC_MODE_MUSIC = 1,
    BT_XTAL_SYNC_MODE_VOICE = 2,
};

void bt_xtal_sync(enum BT_XTAL_SYNC_MODE_T mode);
void bt_init_xtal_sync(enum BT_XTAL_SYNC_MODE_T mode);
void bt_term_xtal_sync(void);

#ifdef __cplusplus
}
#endif

#endif
