#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

static app_wave_play_ctrl_t app_wave_ctrl;
#if (CONFIG_APP_MP3PLAYER == 1)
uint8 mp3_need_pause = 0; //若正在播放mp3, 有提示音时要先暂停mp3
#endif
#if (CONFIG_PROMPT_WAVE_AS_ALAW == 1)
CONST short prompt_Alaw_table[256] =  {
      -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,
      -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848,
      -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
      -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,
     -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,
     -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392,
     -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184,
     -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
       -43,    -41,    -47,    -45,    -35,    -33,    -39,    -37,
       -59,    -57,    -63,    -61,    -51,    -49,    -55,    -53,
       -11,     -9,    -15,    -13,     -3,     -1,     -7,     -5,
       -27,    -25,    -31,    -29,    -19,    -17,    -23,    -21,
      -172,   -164,   -188,   -180,   -140,   -132,   -156,   -148,
      -236,   -228,   -252,   -244,   -204,   -196,   -220,   -212,
       -86,    -82,    -94,    -90,    -70,    -66,    -78,    -74,
      -118,   -114,   -126,   -122,   -102,    -98,   -110,   -106,
       688,    656,    752,    720,    560,    528,    624,    592,
       944,    912,   1008,    976,    816,    784,    880,    848,
       344,    328,    376,    360,    280,    264,    312,    296,
       472,    456,    504,    488,    408,    392,    440,    424,
      2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,
      3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392,
      1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,
      1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696,
        43,     41,     47,     45,     35,     33,     39,     37,
        59,     57,     63,     61,     51,     49,     55,     53,
        11,      9,     15,     13,      3,      1,      7,      5,
        27,     25,     31,     29,     19,     17,     23,     21,
       172,    164,    188,    180,    140,    132,    156,    148,
       236,    228,    252,    244,    204,    196,    220,    212,
        86,     82,     94,     90,     70,     66,     78,     74,
       118,    114,    126,    122,    102,     98,    110,    106
};
#endif
void app_wave_file_play_init(void)
{
    memset( &app_wave_ctrl, 0, sizeof(app_wave_ctrl));
    memset( &app_wave_ctrl.voice_num, 0xFF, 32 );
    app_wave_ctrl.voice_index = 0xFF;
    app_wave_ctrl.file_id_pending_saved = -1;
    return;
}
static uint32 s_sbc_fill_cnt = 0;
static __inline app_wave_handle_t app_get_wave_handle( void )
{
    return &app_wave_ctrl;
}

int app_get_wave_info(uint32 *freq_ptr, uint32 * channel_ptr, uint32 *vol_s_ptr)
{
    if(freq_ptr)
    {
        *freq_ptr = app_wave_ctrl.freq;
    }

    if(channel_ptr)
    {
        *channel_ptr = app_wave_ctrl.channel;
    }

    if(vol_s_ptr)
    {
        *vol_s_ptr = app_wave_ctrl.vol_s;
    }
    return 0;
}

void app_wave_file_volume_init( uint8 vol )
{
    app_wave_ctrl.vol = vol;
}

void app_wave_file_vol_s_init( uint8 vol )
{
    app_wave_ctrl.vol_s = vol;
}

void app_wave_file_aud_notify( uint32 freq, uint16 channel, uint8 vol )
{
    app_wave_ctrl.freq = freq;
    app_wave_ctrl.channel = channel;
    app_wave_ctrl.vol_s = vol;
}

void app_wave_file_voice_num_set( char * num )
{
    int len;
    int i;
    int count = 0;

    memset( &app_wave_ctrl.voice_num, 0xFF, 32 );

    if( num == NULL )
    {
        app_wave_ctrl.voice_index = 0xFF;

        return;
    }

    len = j_strlen(num);

    for( i = 0; i < len; i++ )
    {
        if( num[i] >= '0' && num[i] <= '9' )
        {
            app_wave_ctrl.voice_num[count] = num[i] - '0';
            count++;
        }
        else
            continue;
    }

    if( count > 0 )
        app_wave_ctrl.voice_index = 0;
}


#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
void app_wave_file_fill_buffer(uint8 *buff, uint16 size )
{
    uint16 i = 0;
#if (CONFIG_PROMPT_WAVE_AS_ALAW == 1)
    uint8 *buff_st = jmalloc(size*4,M_ZERO);
    int16 smpl = 0;
#else
    uint8 *buff_st = jmalloc(size*2,M_ZERO);
#endif
    uint8 *src = (uint8 *)buff;

    if(buff_st == NULL)
    {
        os_printf("wavbuff_st jmalloc fail!!!\r\n");
        return;
    }
#if (CONFIG_PROMPT_WAVE_AS_ALAW == 1)
    while(i < size)
    {
        smpl = prompt_Alaw_table[(*(src + i)) ^ 0x80];
        *(buff_st + 4*i) = (smpl & 0x00ff);
        *(buff_st + 4*i + 1) = ((smpl&0xff00) >> 8);
        *(buff_st + 4*i + 2) = (smpl & 0x00ff);
        *(buff_st + 4*i + 3) = ((smpl&0xff00) >> 8);
        i++;
    }
#else
    while(i < size)
    {
        *(buff_st + 2*i) = *(src + i);
        *(buff_st + 2*i + 1) = *(src + i + 1);
        *(buff_st + 2*i + 2) = *(src + i);
        *(buff_st + 2*i + 3) = *(src + i + 1);
        i += 2;
    }
#endif
#if (CONFIG_PROMPT_WAVE_AS_ALAW == 1)
	aud_fill_buffer( buff_st, size*4);
#else
	aud_fill_buffer( buff_st, size*2);
#endif
    jfree(buff_st);
    buff_st = NULL;
}
#endif
uint8 app_wave_file_play_start( int file_id )
{
    static int last_file_id = -1,file_conn_flag=0;
    app_wave_handle_t wave_h = app_get_wave_handle();
    int len,size,frms;//i,
    int continue_flag = (APP_WAVE_FILE_ID_VOICE_CONTINUE == (file_id & APP_WAVE_FILE_ID_VOICE_CONTINUE));
    uint16 page_len = FLASH_PAGE_LEN;

    file_id &= ~APP_WAVE_FILE_ID_VOICE_CONTINUE;
    if(check_wave_file_correct(file_id))
    {
        return WAV_FILE_ID_INVALID;
    }

    os_printf("wave_start(%d),%d,play flag:%d\r\n",file_id,wave_h->type,wave_h->flag_playing);
#if (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)
	app_customer_hid_set_cnt(file_id);	
#endif

#if (CONFIG_CUSTOMER_1V2_CON_MODE == 1)
	app_customer_1v2_con_set_cnt(file_id);
#endif

    if((bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)) 
    	&&(file_id == APP_WAVE_FILE_ID_LOW_BATTERY))
        return WAV_LOW_BATTERY_WHILE_SCO;
    if(bt_flag1_is_set(APP_FLAG_POWERDOWN))
        return 1;

    if(file_id < 0 || file_id >= WAVE_EVENT)
    {
        return WAV_FILE_ID_INVALID;
    }
    if(bt_flag1_is_set(APP_FLAG_POWERDOWN) && (file_id!=APP_WAVE_FILE_ID_POWEROFF))
        return WAV_POWER_OFF_WHILE_NOT_PWD;

    if (get_Charge_state() && bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))     
        return WAV_POWER_OFF_WHILE_NOT_PWD;

    if (last_file_id == APP_WAVE_FILE_ID_POWEROFF)  
        return WAV_POWER_OFF_WHILE_NOT_PWD;		

#ifdef CONFIG_TWS
    if (bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
    {
        if (bt_flag1_is_set(APP_FLAG_MUSIC_PLAY)
        && (file_id!=APP_WAVE_FILE_ID_POWEROFF)) 
            return WAV_POWER_OFF_WHILE_NOT_PWD;
        if(bt_flag1_is_set(APP_FLAG_LINEIN))
            return WAV_FILE_LINEIN_MODE;
    }
#endif
    if(1 == wave_h->flag_playing)
    {
        if((file_id == APP_WAVE_FILE_ID_POWEROFF)&&(wave_h->file_id != APP_WAVE_FILE_ID_POWEROFF))//(IS_MODE_CHANGE_FILE_ID(file_id))
        {	// wave playing stop
            wave_h->file_id = INVALID_WAVE_EVENT;
            wave_h->func = NULL;
            app_wave_file_play_stop();
        }
        /*
        Modified for playing the following new wav_file_id, maybe same as the last one;
        */

        /*
        else
        {	// wave playing continue
        WAV_PRT("wav_playing_return\r\n");
        return 2;
        }
        */
        else if(last_file_id == file_id)
        {
            return WAV_FILE_ID_INVALID;
        }
        
        if((wave_h->file_id == APP_WAVE_FILE_ID_START) && (file_id != APP_WAVE_FILE_ID_POWEROFF))
        {
            wave_h->file_id_pending_saved = file_id;
            return WAV_NO_ERROR;
        }
    }

    if (file_id == APP_WAVE_FILE_ID_CONN)
        file_conn_flag = 1;

    //wave_h->type = app_env_get_wave_type(file_id);
    if(-1 == app_env_get_wave_type(file_id))
    {
        return WAV_FILE_TYPE_INVALID;
    }
    else
    {
         wave_h->type = app_env_get_wave_type(file_id);
    }
    wave_h->mute_tick = 0;

    if(!continue_flag)
    {
        app_sleep_func(0);
    }
#ifdef CONFIG_SBC_PROMPT
// 同一时间来两提示音，若两个提示音不同类型，要重新初始化audio
    if(app_env_get_wave_type(file_id)  !=app_env_get_wave_type(last_file_id))
    {
        app_wave_file_play_stop();
    }
#endif

    
#ifdef CONFIG_SBC_PROMPT
    if((INTER_WAV==wave_h->type)
        ||(INTER_SBC==wave_h->type))
#else
    if( wave_h->type == 0 )
#endif
    {
        wave_h->page_index = app_env_get_wave_page_index(file_id);
        if( wave_h->page_index == -1 )
            return WAV_FILE_PAGE_INDEX_INVALID;

		if (file_id == APP_WAVE_FILE_ID_DISCONN)
		{
			if ((file_conn_flag==0) && app_check_bt_mode(BT_MODE_1V1))
				return WAV_NO_ERROR; 
			file_conn_flag = 0;			
		}

        //if( !bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_MUSIC_PLAY))
        //    ;
#if(CONFIG_CPU_CLK_OPTIMIZATION == 1)
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
        aud_PAmute_oper(1);

        //enable_audio_ldo();

        memcpy( wave_h->page_buff,  (void *)(wave_h->page_index << 8 ), page_len );
        //flash_memcpy(wave_h->page_buff,  (uint8 *)(wave_h->page_index * FLASH_PAGE_LEN_CRC ), FLASH_PAGE_LEN_CRC,1);

        memcpy( &len, &wave_h->page_buff[0], 4 );

        if( len == 0xFFFFFFFF ) // not wav file
            return WAV_FILE_LEN_INVALID;

        last_file_id = file_id;    

        wave_h->page_end = wave_h->page_index + ( len/page_len);
#ifdef CONFIG_SBC_PROMPT
        if(INTER_SBC == wave_h->type)
        {
            if(0x9c != wave_h->page_buff[4])
				return SBC_FILE_SYNCWORD_INVALID;

            wave_h->sbc_code_ptr = (uint8_t *)(wave_h->page_index<<8) +4;
        }
        //WAV_PRT("wav_index:%d,sbc:%p,end:%d\r\n", wave_h->page_index,wave_h->sbc_code_ptr, wave_h->page_end);
#else
        //WAV_PRT("wav_index:%d end:%d\r\n", wave_h->page_index, wave_h->page_end);
#endif

        if( bt_flag1_is_set( APP_FLAG_LINEIN )
    #ifdef CONFIG_BLUETOOTH_COEXIST
        &&(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
    #endif
        )
        {
            //BK3000_Ana_Line_enable(0);
            linein_audio_close();
            aud_volume_mute(1);
        }
#if (CONFIG_APP_MP3PLAYER == 1)
        else if(app_is_mp3_mode() && player_get_play_status())
        {
            mp3_need_pause = 1;
            app_player_play_pause_caller(0);
            //app_player_button_play_pause();
        }
#endif

        if (!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
            aud_mic_open(0);

        //os_printf("wave_start, play flag:%d\r\n",wave_h->flag_playing);
        if(wave_h->flag_playing == 0)
        {
            bt_flag1_operate(APP_FLAG_WAVE_PLAYING, 1 );

            if(!continue_flag)
            {
                aud_close();
            }
            frms = ((AUDIO_BUFF_LEN >>1)/512-1);
#ifdef CONFIG_SBC_PROMPT
            if((INTER_WAV == wave_h->type)
            ||(EXT_WAV == wave_h->type))
            {
                if(!continue_flag)
                {
                    aud_initial(8000, 1, 16);
                }
            }
            else if(INTER_SBC == wave_h->type)
            {
                int frequency;
                s_sbc_fill_cnt = 0;
                switch(((wave_h->page_buff[5] >> 6) & 0x03))
                {
                    case 1:
                    frequency = 8000;
                    break;
                    case 2:
                    frequency = 44100;
                    break;
                    case 3:
                    frequency = 48000;
                    break;
                    case 0:
                    default:
                    frequency = 16000;
                    break;
                }
                if(!continue_flag)
                    aud_initial(frequency, 1, 16);
                wave_h->block_size = wave_h->page_buff[6]*2+8;

            }
#else
            if(!continue_flag)
            {
                aud_initial(8000, 1, 16);
            }
#endif

            aud_volume_set(wave_h->vol );
            //if(!continue_flag)
            {
                aud_open();
            }
        }
        else
        {
            size = aud_get_buffer_size();
            if(size > 2)
                size -= 2;
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
#if(CONFIG_PROMPT_WAVE_AS_ALAW == 1)
            frms = (size >> 2)/page_len;
#else
            frms = (size >> 1)/page_len;
#endif
#else
#if(CONFIG_PROMPT_WAVE_AS_ALAW == 1)
            frms = (size >> 1)/page_len;
#else
            frms = (size)/page_len;
#endif
#endif
        }
        
#ifdef CONFIG_SBC_PROMPT
        if( INTER_WAV == wave_h->type)
        {
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            app_wave_file_fill_buffer(	&wave_h->page_buff[8], page_len-8 );
#else
            aud_fill_buffer( &wave_h->page_buff[8], page_len - 8 );
#endif
            wave_h->page_index++;
            int i;
            for( i = 0; i < MIN(frms, wave_h->page_end - wave_h->page_index - 1); i++ )
            {
                memcpy( wave_h->page_buff,	(void *)(wave_h->page_index << 8 ), page_len );
                //flash_memcpy(wave_h->page_buff,  (uint8 *)(wave_h->page_index * FLASH_PAGE_LEN_CRC ), FLASH_PAGE_LEN_CRC,0);
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
                app_wave_file_fill_buffer(	wave_h->page_buff, page_len );
#else
                aud_fill_buffer(  wave_h->page_buff, page_len );
#endif
                wave_h->page_index++;
            }
        }
        else if(INTER_SBC == wave_h->type)
        {
        //		sbc_first_play_init(1,1);
            wave_h->status = PROMPT_START;
            //os_printf("wave_start: sbc\r\n");
        }
#else
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
        app_wave_file_fill_buffer(	&wave_h->page_buff[8], page_len-8 );
#else
        aud_fill_buffer( &wave_h->page_buff[8], page_len - 8 );
#endif
        wave_h->page_index++;
        int i;
        for( i = 0; i < MIN(frms, wave_h->page_end - wave_h->page_index - 1); i++ )
        {
            memcpy( wave_h->page_buff,  (void *)(wave_h->page_index << 8 ), page_len );
            //flash_memcpy(wave_h->page_buff,  (uint8 *)(wave_h->page_index * FLASH_PAGE_LEN_CRC ), FLASH_PAGE_LEN_CRC,0);
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            app_wave_file_fill_buffer(	wave_h->page_buff, page_len );
#else
            aud_fill_buffer(  wave_h->page_buff, page_len );
#endif
            wave_h->page_index++;
        }
#endif

        wave_h->flag_playing = 1;
        wave_h->file_id = file_id;

        //WAV_PRT("app_wave_file_play_start internal pa = %d,file_id=%d\n",IS_INTERNAL_PA_OPEN,file_id);
        return WAV_NO_ERROR;
    }
#ifdef CONFIG_DRIVER_DTMF
    else
    {
        driver_dtmf_tone_conf_t dtmf_conf;
        driver_dtmf_mode_t dtmf_mode;
        int err = 0;
        uint32 param1,param2;

        err = app_env_get_wave_param( file_id, &param1, &param2);
        if( err )
            return err;

        //if( !bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_MUSIC_PLAY))
        //    ;

        bt_flag1_is_set( APP_FLAG_WAVE_PLAYING, 1 );
        //enable_audio_ldo();

        dtmf_mode.tone_mode = (param2 >> 11) & 0x01;
        dtmf_mode.tone_patten = (param2 >> 10) & 0x01;
        dtmf_mode.tone_pause = (param2 >> 6 ) & 0x0f;
        dtmf_mode.tone_turation = (param2 >> 2 )& 0x0f;

        dtmf_mode_config(&dtmf_mode);

        dtmf_conf.fstep = param1 & 0xFFFF;
        dtmf_conf.gain = (param2 >> 12)& 0xf;
        dtmf_conf.enable = (param2 >> 1) & 0x01;

        dtmf_tone_config( 2, &dtmf_conf);

        dtmf_conf.fstep = ( param1 >> 16) & 0xFFFF;
        dtmf_conf.gain = (param2 >> 16 )& 0xf;
        dtmf_conf.enable = param2 & 0x01;

        dtmf_tone_config( 1, &dtmf_conf);

        wave_h->flag_playing = 1;

        wave_h->page_index = 0;
        wave_h->page_end = dtmf_mode.tone_turation + dtmf_mode.tone_pause;

        if( dtmf_mode.tone_mode == 1 )
        {
            wave_h->page_end = (wave_h->page_end) << 3;
        }

        aud_initial( 8000, 1, 16);
        aud_volume_set(wave_h->vol );

        dtmf_enable(1);
    }
#endif
    return WAV_NO_ERROR;
}

uint8 start_wave_and_action(uint32 file_id, wav_cb_func cb)
{
    uint8 ret;
    //WAV_PRT("start_wave_and_action ... \n");

    //file uncorrect, needn't play
    if(check_wave_file_correct(file_id))
    {
        //WAV_PRT("start_mode_change_wave no file!!!\n");
        if(cb)
        {
            cb();
        }
        return WAV_FILE_ID_INVALID;
    }

    //error status,needn't play
    ret = app_wave_file_play_start(file_id);
    //if(ret && (3 != ret)) /* ??????????????? */
    if(ret)
    {
        //WAV_PRT("start_wave ERROR STATUS!!!\n");
        if(cb)
        {
            cb();
        }

        return ret;
    }
    else
    {
        app_get_wave_handle()->func = cb;
    }
    return WAV_NO_ERROR;
}


int app_wave_playing( void )
{
    app_wave_handle_t wave_h = app_get_wave_handle();

    return wave_h->flag_playing;
}
#ifdef CONFIG_SBC_PROMPT
int app_wave_is_sbc_playing(void)
{
    app_wave_handle_t wave_h = app_get_wave_handle();
    return ((wave_h->type == INTER_SBC) || (wave_h->type == EXT_SBC));
}

int app_wave_check_type( uint8 type)
{
    app_wave_handle_t wave_h = app_get_wave_handle();
    if(wave_h->type==type)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int app_wave_check_status( uint8 status)
{
    app_wave_handle_t wave_h = app_get_wave_handle();
    if(wave_h->status==status)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void app_wave_set_status( uint8 status)
{
    app_wave_handle_t wave_h = app_get_wave_handle();
	wave_h->status=status;
}

uint8_t app_wave_get_blocksize(void)
{
    app_wave_handle_t wave_h = app_get_wave_handle();
	return wave_h->block_size;
}

int app_wave_fill_sbc_node(uint8_t *obj_ptr)
{
    uint8_t *wave_end;
    int ret=0;
    app_wave_handle_t wave_h = app_get_wave_handle();
    wave_end = (uint8_t *)(wave_h->page_end<<8);

    if((wave_end-wave_h->sbc_code_ptr)>=wave_h->block_size)
    {
        memcpy(obj_ptr,wave_h->sbc_code_ptr,wave_h->block_size);
        ret = 1;
        wave_h->sbc_code_ptr += wave_h->block_size;
        wave_h->status = PROMPT_WORKING;
    }
    else if(s_sbc_fill_cnt < 32)  // 32 * 16ms = 512ms
    {
        s_sbc_fill_cnt++;
        wave_h->status = PROMPT_FILL_ZERO;
    }
    else
        wave_h->status = PROMPT_EMPTY;

    return ret;
}
#endif


void app_wave_file_play( void )
{
    app_wave_handle_t wave_h = app_get_wave_handle();
    uint16 size = 0, i, frms;
    uint16 page_len = FLASH_PAGE_LEN;

    if( wave_h->flag_playing )
    {
#if(CONFIG_CPU_CLK_OPTIMIZATION == 1)
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
        CLEAR_SLEEP_TICK;
    #ifdef CONFIG_SBC_PROMPT
        if(INTER_SBC==wave_h->type)
        {
            app_wave_file_sbc_fill();
        }
        else if(INTER_WAV==wave_h->type)
    #else
        if( wave_h->type == 0 )
    #endif
        {
            size = aud_get_buffer_size();
            if(size > 2)
                size -= 2;
            if( wave_h->page_index < wave_h->page_end )
            {
            #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
                //frms = size/page_len/2;
                #if(CONFIG_PROMPT_WAVE_AS_ALAW == 1)
                    frms = (size >> 2)/page_len;
                #else
                    frms = (size >> 1)/page_len;
                #endif
            #else
                #if(CONFIG_PROMPT_WAVE_AS_ALAW == 1)
                    frms = (size >> 1)/page_len;
                #else
                    frms = (size)/page_len;
                #endif
            #endif
                frms = (frms > 8)?8:frms;

                for( i = 0; i < frms; i++)
                {
                    memcpy( wave_h->page_buff,  (void *)(wave_h->page_index << 8 ), page_len );
                    //flash_memcpy(wave_h->page_buff,  (uint8 *)(wave_h->page_index * FLASH_PAGE_LEN_CRC ), FLASH_PAGE_LEN_CRC,0);
				#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
					app_wave_file_fill_buffer(	wave_h->page_buff, page_len );
				#else
                    aud_fill_buffer(  wave_h->page_buff, page_len );
				#endif

                    wave_h->page_index++;
                    if( wave_h->page_index >= wave_h->page_end )
                        break;
                }
                size = aud_get_buffer_size();
                if( size >= (AUDIO_BUFF_LEN - 8 ))
                {
                    //WAV_PRT("app_wave_file_play, app_wave_file_play_stop 1\r\n");
                    app_wave_file_play_stop();
                    return;
                }
            }
            else
            {
                if( size >= (AUDIO_BUFF_LEN - 8 ))
                {
                    //WAV_PRT("app_wave_file_play,_stop2:%d,:%d,%d\r\n", size, wave_h->page_index, wave_h->page_end);
                    app_wave_file_play_stop();
                    return;
                }
            }
        }
        else
        {
            wave_h->page_index++;
            if( wave_h->page_index >= wave_h->page_end )
            {
                //WAV_PRT("_stop3\r\n");
                app_wave_file_play_stop();
                return;
            }
        }

        if( wave_h->mute_tick <= 10 )
        {
            wave_h->mute_tick++;
        }
        else
            aud_PAmute_oper(0);
    }
    return;
}

//correct--0, others--uncorrect
uint8 check_wave_file_correct(int file_id)
{
    uint32 page_index = 0;
    int len = 0;
    app_env_handle_t env_h = app_env_get_handle();

    if(1 != env_h->env_cfg.used) //no wave file have been configured
    {
        return 1;
    }

    page_index = app_env_get_wave_page_index(file_id);

    if(((-1) == page_index)||(0 == page_index))       //file not exist, needn't play mode switch wave
    {
        return 1;
    }

    memcpy(&len,  (void *)(page_index << 8), 4 );

    if(0xFFFFFFFF == len) // not correct wav file
    {
        return 1;
    }

    return 0;
}

void app_wave_file_play_stop( void )
{
    app_wave_handle_t wave_h = app_get_wave_handle();
    app_env_handle_t env_h = app_env_get_handle();
    app_handle_t app_h = app_get_sys_handler();

    //WAV_PRT("app_wave_file_play_stop, playing = %d\n",wave_h->flag_playing);
    os_printf("wav stop:%d,%d,flag_sm1:0x%x\r\n",wave_h->flag_playing,bt_flag1_is_set(APP_FLAG_WAVE_PLAYING)>0,app_h->flag_sm1);
    if(wave_h->flag_playing == 0)
    {
        clear_wave_playing();
        if (wave_h->file_id == APP_WAVE_FILE_ID_POWEROFF)
        {
            aud_PAmute_oper(1);
            aud_close();
            aud_volume_mute(1);	
        }
        else
            app_audio_restore();
		
        return;
    }

    wave_h->flag_playing = 0;
    wave_h->mute_tick = 0;

    if( wave_h->type == 0 )
    {
        wave_h->page_index = 0;
        wave_h->page_end = 0;
    }
#ifdef CONFIG_DRIVER_DTMF
    else
    {
        wave_h->page_index = 0;
        wave_h->page_end = 0;
        dtmf_enable(0);
    }
#endif
    if(!bt_flag1_is_set(APP_FLAG_MUSIC_PLAY|APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
        aud_PAmute_oper(1);
    if(bt_flag1_is_set(APP_FLAG_LINEIN))
        adcmute_cnt = 50;
		
#if 0//(CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(1, 0);
#endif
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if(bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
        && bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
	 BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif

    //clear_wave_playing();

    //WAV_PRT("app_wave_file_play_stop1\n");
    // voice num
    if(( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL )
	&&(bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
       &&(wave_h->file_id != APP_WAVE_FILE_ID_POWEROFF))
    {
        int i = 0,need_play_incoming_number = 0;
        while(!check_wave_file_correct(APP_WAVE_FILE_ID_VOICE_NUM_0+i))//check 0-9 wave file
        {
            i++;
            if(i > 9)
            {
                need_play_incoming_number = 1;
                break;
            }
        }
        if((0xFF != wave_h->voice_index)&&(need_play_incoming_number))
        {
            app_wave_file_play_start(APP_WAVE_FILE_ID_VOICE_CONTINUE + APP_WAVE_FILE_ID_VOICE_NUM_0 + wave_h->voice_num[wave_h->voice_index]);
            wave_h->voice_index++;
            if( 0xFF == wave_h->voice_num[wave_h->voice_index])
            {
                wave_h->voice_index = 0xFF;
            }

            return;
        }

    }

    clear_wave_playing();

    //WAV_PRT("app_wave_file_play_stop2\n");
#ifdef INCOMING_CALL_RING //continue internal ring if call not exist but just incoming status
    if(((APP_WAVE_FILE_ID_HFP_RING == wave_h->file_id)||(0xFF == wave_h->voice_index)) && (bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)))
    {
        app_bt_shedule_task((jthread_func)app_wave_file_play_start,
                            (void *)(APP_WAVE_FILE_ID_HFP_RING),1000 );
        return;
    }
#endif
    //WAV_PRT("app_wave_file_play_stop3\n");


    if (APP_WAVE_FILE_ID_POWEROFF == wave_h->file_id)
	{
        aud_PAmute_oper(1);
        aud_close();
        aud_volume_mute(1);	
    }
    else
    	app_audio_restore();

    if(wave_h->func)
    {
        (wave_h->func)();
    }
    wave_h->func = NULL;
    wave_h->file_id = WAVE_EVENT;

    if((-1) != wave_h->file_id_pending_saved)
    {
        app_wave_file_play_start( wave_h->file_id_pending_saved );
        wave_h->file_id_pending_saved = -1;
    }
#if ((CONFIG_CHARGE_EN == 1))
    app_charge_powerdown();
#endif
    return;
}

//0--check ok,play mode switch wave, others--check fail, needn't play mode switch wave
uint8 enter_mode_wave_and_action(uint32 new_mode, wav_cb_func cb)
{
    uint32 file_id;

    if(SYS_WM_NULL == new_mode)
    {
        new_mode = SYS_WM_BT_MODE;
    }

    switch(new_mode)
    {
        case SYS_WM_BT_MODE:
            file_id = APP_WAVE_FILE_ID_BT_MODE;
            break;
    #if (CONFIG_APP_MP3PLAYER == 1)
    #ifdef CONFIG_APP_SDCARD
        case SYS_WM_SD_MUSIC_MODE:
            file_id = APP_WAVE_FILE_ID_SDCARD_MODE;
            break;
    #endif
    #ifdef CONFIG_APP_USB
    	case SYS_WM_UDISK_MUSIC_MODE:
            file_id = APP_WAVE_FILE_ID_UDISK_MODE;
            break;
    #endif
    #ifdef FM_ENABLE
        case SYS_WM_FM_MODE:
            file_id = APP_WAVE_FILE_ID_FM_MODE;
            break;
    #endif
    #endif
        case SYS_WM_LINEIN_MODE:
            file_id = APP_WAVE_FILE_ID_LINEIN_MODE;
            break;
        default:               //SYS_LINEIN_MODE
            file_id = APP_WAVE_FILE_ID_LINEIN_MODE;
            break;
    }

    return start_wave_and_action(file_id, cb);
}

// EOF
