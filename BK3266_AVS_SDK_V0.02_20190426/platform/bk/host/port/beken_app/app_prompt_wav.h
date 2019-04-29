#ifndef _APP_PROMPT_WAV_H_
#define _APP_PROMPT_WAV_H_

//#define WAV_DEBUG

#ifdef WAV_DEBUG
    #define WAV_PRT      os_printf
#else
    #define WAV_PRT      os_null_printf
#endif
enum wav_error_codes
{
    WAV_NO_ERROR                = 0x00, 
    WAV_LOW_BATTERY_WHILE_SCO   = 0x01,
    WAV_FILE_ID_INVALID         = 0x02,
    WAV_FILE_TYPE_INVALID       = 0x03,
    WAV_POWER_OFF_WHILE_NOT_PWD = 0x04,
    WAV_FILE_PAGE_INDEX_INVALID = 0x05,
    WAV_FILE_LEN_INVALID        = 0x06,
    WAV_FILE_LINEIN_MODE        = 0x07,
    SBC_FILE_SYNCWORD_INVALID   = 0x08,
};
typedef struct _app_wave_play_ctrl_s
{
    uint8_t  flag_playing;
    uint8_t  type;
    uint8   page_buff[264];

    uint32_t  page_index;
    uint32_t  page_end;

    uint8_t   mute_tick;
    uint32_t  freq;
    uint16_t   channel;
    uint8_t   vol_s;
    uint8_t   vol;

    uint8_t   voice_num[32];
    uint8_t   voice_index;
    int       file_id;
    int       file_id_pending_saved;
    wav_cb_func func;
	#ifdef CONFIG_SBC_PROMPT
    uint8_t   status;
    uint8_t   block_size;
    uint8_t *sbc_code_ptr;
	#endif
}app_wave_play_ctrl_t;

typedef app_wave_play_ctrl_t * app_wave_handle_t;

void app_wave_file_volume_init( uint8 vol );
void app_wave_file_vol_s_init( uint8 vol );
void app_wave_file_aud_notify( uint32 freq, uint16 channel, uint8 vol );
void app_wave_file_voice_num_set( char * num );
uint8 app_wave_file_play_start( int file_id );
uint8 check_wave_file_correct(int file_id);
uint8 enter_mode_wave_and_action(uint32 new_mode, wav_cb_func cb);
void app_wave_file_play_stop( void );
void app_wave_file_play( void );
int app_wave_playing( void );
int app_get_wave_info(uint32 *freq_ptr, uint32 * channel_ptr, uint32 *vol_s_ptr);
void app_wave_file_play_init(void);
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
void app_wave_file_fill_buffer(uint8 *buff, uint16 size );
#endif
#ifdef CONFIG_SBC_PROMPT
int app_wave_is_sbc_playing(void);
int app_wave_check_type( uint8 type);
void app_wave_set_status( uint8 status);
int app_wave_check_status( uint8 status);
uint8_t app_wave_get_blocksize(void);
int app_wave_fill_sbc_node(uint8_t *obj_ptr);
#endif
#endif
