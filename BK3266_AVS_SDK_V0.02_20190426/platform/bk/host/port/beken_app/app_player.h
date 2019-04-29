#ifndef _APP_PLAYER_H_
#define APP_PLAYER_H_

#include "ff.h"
#include "mp3common.h"
#if (CONFIG_APP_MP3PLAYER == 1)
enum
    {
        APP_PLAYER_MODE_PLAY_ALL_CYCLE = 0, // 
        APP_PLAYER_MODE_PLAY_ONE = 2,        //
        APP_PLAYER_MODE_PLAY_RANDOM = 3,     //
        APP_PLAYER_MODE_END
    };

enum
    {
        BUTTON_PLAYER_NONE = 0,
        BUTTON_PLAYER_PLAY_PAUSE,
        BUTTON_PLAYER_NEXT,
        BUTTON_PLAYER_PREV,
        BUTTON_PLAYER_VOL_P,
        BUTTON_PLAYER_VOL_M,
        BUTTON_PLAYER_VOL_MUTE,

#if (defined(CONFIG_BLUETOOTH_HFP))
        BUTTON_MP3_HFP_ACK,
        BUTTON_MP3_HFP_REJECT,
#endif

        BUTTON_PLAYER_PLAY_MODE_SET,
        BUTTON_PLAYER_MODE_CHANGE,
        BUTTON_PLAYER_END
    };

#define MAX_DIR_DEPTH             6  // no large than 7, otherwise the break point can not restroe correctly.(see driver_flash.c for detail)

void app_player_button_setting(void);
void app_player_init( void);
//void app_player_uninit( int mode );
//void app_player_file_info_init( void );
//int app_player_state_pause( void );
void MP3_CALL app_player_play_func( void );
//void player_start_first_running(void);

uint32 app_check_mp3_music_type(void);

void app_player_play_pause_caller( int play_pause);
int player_get_play_status( void );
int app_player_button_play_pause( void );
void app_playwav_resumeMp3(uint32 fieldId);
//void playwav_resumeMp3(uint32 fieldId);
uint8 get_fat_ok_flag(void);
uint8 get_disk_type(void);
FIL *Get_File_From_Number(uint16 number);
FIL *Get_Cur_Dir_file(uint16 dir_num,uint16 song_idx);

//uint32 app_player_automatic_play_at_first_time(void);
void MP3_CALL Convert_Mono(short *buffer, int outputSamps);
void ClearMP3(MP3DecInfo *mp3DecInfo);
int app_player_unload(uint32 workmode);


void music_mode_msg_handler(void);
#if CALC_PLAY_TIME
uint32 app_player_show_playtime( void );
#endif
#ifdef CONFIG_BLUETOOTH_COEXIST
void app_coexist_mp3_player_play_pause(uint8 flag);
#endif

#endif

#endif
