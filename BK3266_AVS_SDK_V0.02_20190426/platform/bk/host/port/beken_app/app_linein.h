#ifndef _APP_LINEIN_H_
#define _APP_LINEIN_H_

#define LINEIN_DEBUG

#ifdef LINEIN_DEBUG
    #define LINEIN_PRT      os_printf
#else
    #define LINEIN_PRT 
#endif

#define LINEIN_BUTTON_CONFIG

#define LINEIN_STATUS_ON                    1
#define LINEIN_STATUS_OFF                   0
#define LINEIN_GPIO_LOW_VALID               0

#define LINEIN_DEFAULT_GPIO             9// 2
#define LINEIN_GPIO_DEBOUNCE_COUNT          40

#define LINEIN_MAX_VOL                      28
#define LINEIN_MIN_VOL                      2
#define LINEIN_VOL_STEP                     2

typedef void (*LINEIN_DETECT)(void);

typedef struct _linein_cfg_s
{
    uint8 enable;
    uint8 high_flag;
    uint8 pin;
    uint8 status;
    LINEIN_DETECT detect_func;
    uint32 plugin_count;
    uint32 pullout_count;
}linein_cfg_t;
void app_playwav_resumelinein(uint32 fieldId);

void linein_audio_close(void);
void linein_audio_open(void);
void app_linein_enter(void* arg);
void app_linein_exit(void);
void app_linein_scanning( void );
//int app_linein_vol_up( void );
//int app_linein_vol_down( void );
//int app_linein_mute_unmute(void);
void app_linein_init(void);
uint8 linein_is_attached(void);
uint8 linein_get_play_status(void);
void linein_mode_msg_handler(void);
int app_linein_powerup_check(void);
#ifdef CONFIG_BLUETOOTH_COEXIST
void app_coexist_linein_play_pause(uint8 flag);
#endif
#endif
// EOF
