#ifndef _APP_IIC_FM_H_
#define _APP_IIC_FM_H_
#define FM_DEBUG

#ifdef FM_DEBUG
#define FM_PRT      os_printf
#else
#define FM_PRT
#endif

#define CHANNEL_INTERVAL_AT_SEEK_SUCCESS    (1000)

#define RET_FM_SUCCESS           (0)
#define RET_FM_EXCEPTIONAL       (-1)

#define READ                     1
#define WRITE                    0

#define MIN_FRE                  875
#define MAX_FRE                  1080
#define MAX_CHANNEL_COUNT        (MAX_FRE - MIN_FRE)
#define MAX_FRE_DEVIATION        (MAX_FRE - MIN_FRE)

void fm_uninstall(void);
void fm_install(void *P);
void fm_msg_handler(void);
void app_playwav_resumeFM(uint32 fieldID);
uint8 app_get_fm_vol(void);
void BK3252_FmEnMute(void );
void BK3252_FmDisMute(void );
void FM_IC_PowerDown(void);

#endif
