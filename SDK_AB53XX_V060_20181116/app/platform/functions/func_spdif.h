#ifndef _FUNC_SPDIF_H
#define _FUNC_SPDIF_H

typedef struct {
    u8  disp_update :   1;
} func_spdif_t;
extern func_spdif_t f_spdif;

typedef struct {
    u8 channel;
    u8 res[3];
}spd_cfg_t;

extern spd_cfg_t spd_cfg;

void func_spdif_message(u16 msg);
bool spdif_is_online(void);
void spdif_init(void *cfg);
void spdif_exit(void);
void spdif_stop(void);
void spdif_start(void);
void spdif_play_pause(void);
void spdif_smp_detect(void);

#if (GUI_SELECT != GUI_NO)
void func_spdif_display(void);
void func_spdif_enter_display(void);
void func_spdif_exit_display(void);
#else
#define func_spdif_display()
#define func_spdif_enter_display()
#define func_spdif_exit_display()
#endif

#endif // _FUNC_SPDIF_H
