#include <jos.h>
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "msg_pub.h"

#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"
#include "layer21.h"
#include "bkmp3_resample.h"
#include "string.h"
//#include "BK3000_reg.h"
//#include "ff.h"
#include "app_fs.h"
#include "diskio.h"
#include "bt_mini_sched.h"
#if (CONFIG_APP_MP3PLAYER == 1)

#define MEDIA_ERR_CNT_THRE         15
enum
{
    APP_PLAYER_SCHEDULE_CMD_NEXT_SONG = 1,
    APP_PLAYER_SCHEDULE_CMD_PREV_SONG = 2,
    APP_PLAYER_SCHEDULE_CMD_NEXT_DIR = 3,
    APP_PLAYER_SCHEDULE_CMD_MODE_CHANGE = 4,
    APP_PLAYER_SCHEDULE_CMD_PREV_DIR = 5,
    APP_PLAYER_SCHEDULE_CMD_FIX_SONG = 8,
};

#define APP_PLAY_SCHEDULE_CMD_PLAY_FLAG      0x40000000

#define APP_PLAYER_FLAG_HW_ATTACH                0x1
#define APP_PLAYER_FLAG_PLAY_PAUSE               0x2
#define APP_PLAYER_FLAG_PLAY_CONTINOUS       0x4
#define APP_PLAYER_FLAG_PLAY_CYCLE               0x8
#define APP_PLAYER_FLAG_PLAY_END                   0x10

// not implemented yet
#define APP_PLAYER_FLAG_RANDOM_ORDER          0x2000

#define APP_PLAYER_HW_STANDBY_TIME                   100
//lianxue.liu
#define APP_PLAYER_WAVE_READ_BLOCK_NUM          4
// before setting audio corretly, we need wait for at least 4 corret 
// frame decode. 
#define APP_PLAYER_AUDIO_INITION_BEGIN          4
//#define APP_PLAYER_DEBUG
//#define APP_PLAYER_DEBUG_FOR_QUICK_PLAY

#define   BUF_CUTDOWN_DEBUG    os_printf

//extern RAM_CODE void Beken_Uart_Rx(void);
extern uint16 mp3filecount;
extern uint16 mp3queuecount;

//#define MUSIC_PREV_SONG
#ifdef MUSIC_PREV_SONG
uint8 flag_prev_song = 0;
#endif

#ifdef APP_PLAYER_DEBUG_FOR_QUICK_PLAY
int quick_play_flag = 0;
#endif

typedef struct _app_player_ctrl_s
{
    fat_file_info_t file_info;        // cur file info
    uint16          dir_index;
    int32           file_index;      // file handle
    uint32          block_played;

    uint32          player_flag;
    uint32          schedule_cmd; 
    uint8           media_err_status; //lianxue.liu  
    uint8           wav_header[44];
    uint8           wav_header_index;
    uint8           wav_read_flag;    
    uint32          wav_freq;
    uint32          wav_channel;
    uint32          wav_bytePsecd;
    uint8           adjust_playtime;
}app_player_ctrl;

static app_player_ctrl app_player;
wavInfo wav_info;

typedef struct _app_player_ctrl_backup_s
{
    uint32          file_index;      // file handle
    uint32          file_size_blks;
    uint32          block_played;
    uint16          dir_index;
}__PACKED_POST__ app_player_ctrl_backup;

app_player_ctrl_backup player_config_backup;

uint8 *readBuf;   
short *mp3_pcm_ptr; 
short *mp3_pcm_ptr_tmp; 
int pcm_size = 0;
volatile uint8 aud_buff_full = 0;
volatile uint32 buf_free = 0;
         
short  *aulawsmpl;
short  *alawtbl;
short  *ulawtbl;
uint8  *rbbuf;

HMP3Decoder hMP3Decoder;    //错误信息结构体
MP3FrameInfo mp3FrameInfo;     //mp3文件信息结构体
MP3DecInfo *mp3decinfo;
   
uint8 init=0;
uint8 mp3_play_flag = 0;  
short *last_half_data = NULL;
uint16_t resample_size = 0;
int t=0;

FIL *newFatfs;
static uint32 media_err_cnt = 0;
uint32 play_derect = 0;   // 1:next, 0 :previous
static int16 sblksize = 2048;
static int16 sbuffsize = 2048;

#ifdef CONFIG_APP_MP3_EQUANLIZER
static uint8 mp3_eq_index = 0;
#endif
uint8 appPlayerPlayMode=0;
static void app_player_play_pause( int play_pause  );
static int app_player_button_next( void );
static int app_player_button_prev( void );
static void app_player_mp3_file_end( void );
static void app_player_wav_file_end( void );
static int app_player_button_setvol_up( void );
static int app_player_button_setvol_down( void );
static int app_player_get_next_stream_file( void );
static int app_player_get_prev_stream_file( void );
static int app_player_to_first_file( void );
static void app_player_stop( void  );
static void app_player_wave_init(void);
static void app_player_mp3_init(void);
static void app_player_file_info_init( void );
static void wav_mem_uninit(void);
static void wav_mem_init(void);
static void mp3_mem_uninit(void);
static void mp3_mem_init(void);
extern RAM_CODE t_error BTms_Sched(u_int32 num_interations);

#ifdef APP_PLAYER_DEBUG
static int app_player_get_next_stream_file_test( void );
static int app_player_get_prev_stream_file_test( void );
#endif
#if 0
uint32 app_get_mp3_filenumber( void )
{
	return app_player.file_index;
}
uint32 app_get_mp3_state( void )
{
    if(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE)
        return 1;
    else
        return 0;
}
#endif
static void app_restore_breakpoint(void)
{
    uint8 item;

    if(get_cur_media_type()== 0/*DISK_TYPE_SD*/)
        item = ENV_SD_INFO;
    else
        item = ENV_UDISK_INFO;

    app_env_restore((void*)&player_config_backup,item);
    //os_printf("back info:diridex=%x,fileidx=%x,filesize=%x,blockplayed=%x\r\n",
    //player_config_backup.dir_index,player_config_backup.file_index,player_config_backup.file_size_blks,player_config_backup.block_played);
}

static void app_player_save_breakpoint( void )
{
    uint8 item;
    player_config_backup.file_index = app_player.file_index;
    player_config_backup.dir_index = app_player.dir_index;
    //    player_config_backup.cur_block = app_player.cur_block;
    player_config_backup.file_size_blks = app_player.file_info.file_blks;
    player_config_backup.block_played = newFatfs->fptr&0xfffffffc;
    if(get_cur_media_type()== 0/*DISK_TYPE_SD*/)
        item = ENV_SD_INFO;
    else
        item = ENV_UDISK_INFO;
    //os_printf("save breakpoint:index:%x,blockplayed:%x,filesize:%x\r\n",
    //player_config_backup.file_index,player_config_backup.block_played,player_config_backup.file_size_blks);

    app_env_save_task((void*)&player_config_backup,item);
	
}


static void app_player_shedule_cmd(int cmd )
{
    cmd |= APP_PLAY_SCHEDULE_CMD_PLAY_FLAG;
    app_player_play_pause(0);

#ifdef APP_PLAYER_DEBUG_FOR_QUICK_PLAY
    quick_play_flag = 0;
#endif
    app_player.schedule_cmd = cmd;
}

uint32 media_playing_flag = 0;//current meida playing status

static void resume_mp3_from_wav(void)
{
    if(media_playing_flag)
    {
        app_player_play_pause(1); //contiue playing after playing wav.
        media_playing_flag = 0;
    }
    else
    {
        aud_PAmute_oper(1);
    }
}
static int16 get_wav_buffsize(int16 blksize)
{
    int16 buffsize = blksize; // 512 * n 
    if(wav_info.ch == 1)
    {
        buffsize <<= 1;
    }
    return buffsize;
}

static void playwav_resumeMp3(uint32 fieldId)
{
    if(0 == check_wave_file_correct(fieldId) && 
        !bt_flag1_is_set( APP_FLAG_WAVE_PLAYING ))
    {
        if(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE)// bak up player_flag to contiue playing after playing wav.
        {
            media_playing_flag = 1;
            app_player_play_pause(0);// pause the current play
        }
        //the start of wav play
        start_wave_and_action(fieldId, resume_mp3_from_wav);
    }
}

void app_playwav_resumeMp3(uint32 fieldId)
{
    playwav_resumeMp3(fieldId);
}
#if 0
uint32 app_player_automatic_play_at_first_time(void)
{
	if(get_fat_ok_flag())
    {
        app_player_file_info_init();
        app_player_button_play_pause();
    }
    return 0;
}
#endif

static int app_player_button_setvol_up( void )
{
    if(player_vol_bt == AUDIO_VOLUME_MIN)
    {
        if(app_player.player_flag&APP_PLAYER_FLAG_PLAY_PAUSE)
            aud_PAmute_oper(0);        
    }
	
    if(++player_vol_bt >= AUDIO_VOLUME_MAX)
    {
        player_vol_bt=AUDIO_VOLUME_MAX; 
    }
  //  os_printf("vol_up:%d\r\n",player_vol_bt);
    
    if(!app_wave_playing())
        aud_volume_set(player_vol_bt);
    if(AUDIO_VOLUME_MAX == player_vol_bt)
    {
        playwav_resumeMp3(APP_WAVE_FILE_ID_VOL_MAX);
    }
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_MUSIC_VOL_INFO);
#endif
    return 0;
}

static int app_player_button_setvol_down( void )
{
    if(player_vol_bt != AUDIO_VOLUME_MIN)
    {
        player_vol_bt --;
    }
   // os_printf("vol_down:%d\r\n",player_vol_bt);

    if(!app_wave_playing())
        aud_volume_set(player_vol_bt);
    if(AUDIO_VOLUME_MIN == player_vol_bt)
    {
        playwav_resumeMp3(APP_WAVE_FILE_ID_VOL_MIN);
    }
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_MUSIC_VOL_INFO);
#endif
    return 0;
}

int app_player_button_play_pause( void )
{
    app_player_play_pause( !(app_player.player_flag &  APP_PLAYER_FLAG_PLAY_PAUSE ) );
    return 0;
}

static int app_player_button_next( void )
{
    app_player_shedule_cmd( APP_PLAYER_SCHEDULE_CMD_NEXT_SONG );
    return 0;
}
static int app_player_button_nextDir( void )
{
    app_player_shedule_cmd( APP_PLAYER_SCHEDULE_CMD_NEXT_DIR );
    return 0;
}

static int app_player_button_prev( void )
{
    app_player_shedule_cmd( APP_PLAYER_SCHEDULE_CMD_PREV_SONG );
    return 0;
}
static int app_player_button_fix( void )
{
    app_player_shedule_cmd( APP_PLAYER_SCHEDULE_CMD_FIX_SONG );
    return 0;
}

#if CALC_PLAY_TIME 
static void app_player_wave_get_bytePersecd(void)
{
    if( app_player.wav_header_index != 44 )
        return;

#if 0
    uint8 *buff = &app_player.wav_header[0];
    uint32 bytePsecd = buff[28] + (buff[29]<<8) + (buff[30]<<16) + (buff[31]<<24);
#else
    uint32 bytePsecd =  wav_info.fs * wav_info.ch * wav_info.bits / 8;
#endif
    app_player.wav_bytePsecd = bytePsecd;  
}

static void app_player_wave_calc_total_playtime(void)
{
    if( app_player.wav_header_index != 45 )
        return;
    
    uint32 total_time = (newFatfs->obj.objsize - 44) / app_player.wav_bytePsecd;
    MP3_Show_Play_Time(total_time);
}

uint32 app_player_wave_calc_cur_playtime(void)
{
    uint32 cur_time;
    if( app_player.wav_header_index != 45 )
        return 0;

    cur_time = (newFatfs->fptr - 44) / app_player.wav_bytePsecd;
    MP3_Show_Play_Time(cur_time);
    return cur_time;
}
#endif

static int app_player_wave_header_handler( uint8 *buff, uint16 len )
{
    if( app_player.wav_header_index >= 44 )
        return 0;

    if(!(((buff[0x00] == 'R') && (buff[0x01] == 'I')&& (buff[0x02] == 'F') && (buff[0x03] == 'F')) &&
         ((buff[0x08] == 'W') && (buff[0x09] == 'A')&& (buff[0x0A] == 'V') && (buff[0x0B] == 'E'))))
    {
        return 0;
    }

    memcpy(&app_player.wav_header[0], buff, 12);

    int32_t  s = len  - 12;
    uint8_t* p = buff + 12;
    uint32_t f = 1;

    while(s >= 8)
    {
        uint32_t id = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
        uint32_t sz = p[4] + (p[5] << 8) + (p[6] << 16) + (p[7] << 24);

        switch(id)
        {
        case 0x20746d66:
            memcpy(&app_player.wav_header[12], p, 24);
            f++;
            break;
        case 0x61746164:
            memcpy(&app_player.wav_header[36], p, 8);
            f++;
            break;
        default:
            break;
        }

        if(f == 3) break;

        s -= (8 + sz);
        p += (8 + sz);
    }

    if(f != 3) return 0;

    app_player.wav_header_index = 44;
    app_player_wave_init();
    s -= 8;
    p += 8;

    if(s>0)
    {
        uint32_t m = s % wav_info.block_align;
        if((24 ==wav_info.bits) && m )
        {
            f_SeekfromCurPos(newFatfs, 1, s);
        }
        else
        {
            wav_fill_buffer(p + 8, s - 8);
        }
    }
    return 1;
}

static void app_player_wave_init(void)
{
    app_player.wav_read_flag = 1;
    uint32 wave_actual_len=0;
    int set_audio_return = 10;
    if( app_player.wav_header_index != 45 )
    {
        media_err_cnt = 0;
        
        memcpy( (uint8 *)&wav_info.fmtTag, &app_player.wav_header[0x14], 2 );
        memcpy( (uint8 *)&wav_info.fs, &app_player.wav_header[0x18], 4 );
        memcpy( (uint8 *)&wav_info.ch, &app_player.wav_header[0x16], 2 );
        memcpy( (uint8 *)&wav_info.block_align, &app_player.wav_header[0x20], 2 );
        memcpy( (uint8 *)&wav_info.bits, &app_player.wav_header[0x22], 2 );

        app_player.wav_freq = wav_info.fs;
        app_player.wav_channel = wav_info.ch;

        os_printf("wave_type: %d, freq: %d, channel: %d, bits: %d\r\n", wav_info.fmtTag,
                  wav_info.fs, wav_info.ch, wav_info.bits );
        if( (wav_info.fmtTag != 0x01)  		// linear 8bits/16bits/32bits
            && (wav_info.fmtTag != 0x06)    // a-law  8bits
            &&(wav_info.fmtTag != 0x07))    // u-law  8bits
        {
            app_player.block_played = app_player.file_info.file_blks;
            return;
        }
        #if 0
        if(wav_info.bits == 24)				// Not Support 24 bits linear
        {
            app_player.block_played = app_player.file_info.file_blks;
            return;
        }
        #ifdef MUSIC_PREV_SONG
        flag_prev_song = 0;
        #endif
        #endif
    #if CALC_PLAY_TIME 
        app_player_wave_get_bytePersecd();
    #endif
        
        //计算audio ringbuff 半空 和SD卡的BLOCKSIZE之间的关系;
        sblksize = AUDIO_BUFF_LEN/2;
        if(8 == wav_info.bits) 
            sblksize >>= 1;
        do
        {
            sblksize >>= 1;
            sbuffsize = get_wav_buffsize(sblksize);
        }while(sbuffsize > 4096);
        if(8 == wav_info.bits) 
            sbuffsize <<= 1;
        sblksize /= wav_info.block_align;
        sblksize *= wav_info.block_align;
        os_printf("size = %d,%d\r\n",sbuffsize,sblksize);
        if(sblksize < 512)  // Not support!!!!
        {
            app_player.block_played = app_player.file_info.file_blks;
            return;
        }
        init=0;
        aud_close();
	 set_audio_return = aud_initial( wav_info.fs, wav_info.ch, 16 );

        //set_audio_return = aud_initial( wav_info.fs, wav_info.ch, 16 );
        //os_printf("set_audio_return=%d\r\n",set_audio_return);
        //os_printf("app_player.schedule_cmd=%x\r\n",app_player.schedule_cmd);
        if((-1) == set_audio_return)
        {
            /*if( app_player.schedule_cmd  & APP_PLAYER_SCHEDULE_CMD_PREV_SONG )
              app_player_button_prev();
              else
              app_player_button_next();*/
            os_printf("not over!!! need change!\r\n");
            app_player.block_played = app_player.file_info.file_blks;
            return ;
        }

        aud_volume_set(player_vol_bt);
        app_player.wav_header_index = 45;
        mp3_play_flag = 2;
        memcpy((uint8 *)&wave_actual_len,&app_player.wav_header[0x28],4);
        if(wave_actual_len < app_player.file_info.file_blks)
            app_player.block_played = app_player.file_info.file_blks - wave_actual_len;
        aud_open();
    }
}

static void app_player_mp3_init(void)
{
    mp3_mem_init();

    //aud_initial(48000, 2, 16 );  //temp setting
    init=0;
    t = 0;
#ifdef MUSIC_PREV_SONG    
    flag_prev_song = 0;
 #endif   
    aud_volume_set(player_vol_bt);

#ifdef JMALLOC_STATISTICAL
    os_printf("JMALLOC_STATISTICAL: MP3\r\n");
    memory_usage_show();
#endif
}

//void app_player_button_setting(void)
//{
//}

static void app_player_audio_init(void)
{
 //#ifndef CONFIG_AUDIO_DAC_ALWAYSOPEN
     if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
     BK3000_Ana_Dac_enable(1);
 //#endif
}

void app_player_init( void)
{
    FRESULT err = FR_OK;
    //app_player_button_setting();

    app_player.player_flag = 0;

    app_player_audio_init();

    err = fat_malloc_files_buffer();
    if(err == FR_INT_ERR)
    {
        os_printf("fat_malloc_files_buffer failed, please check\r\n");
        while(1);
    }
    //os_printf("fat_malloc_files_buffer is OK\r\n");
}

#if 0
void app_player_uninit( int mode )
{
    BK3000_PMU_PERI_PWDS |= bit_PMU_SDIO_PWD; // close sdcard

    if( mode == 1/*APP_WORK_MODE_SDCARD_PLAYER*/ )
    {
        gpio_enable_second_function(GPIO_FUNCTION_I2C_FM);
    }
}
#endif

int app_player_unload(uint32 workmode)
{
    //app_handle_t app_h = app_get_sys_handler(); 
    uint8 mediatype = 1;//DISK_TYPE_SD;
    FRESULT err = FR_OK;

    aud_close();

    if(workmode == SYS_WM_SD_MUSIC_MODE)
        mediatype = 0;//DISK_TYPE_SD;
    else
        mediatype = 1;//DISK_TYPE_UDISK;
 //   if( (app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH ))
    {
        disk_unmount(mediatype);
        //os_printf("media uninstall %d\r\n",get_fat_ok_flag());
    }


    app_player_save_breakpoint();
    //os_printf("END 1,mp3_play_flag=%x\r\n",mp3_play_flag);
    if(mp3_play_flag == 1)
    {
        app_player_mp3_file_end();
    }
    else if (mp3_play_flag == 2)
    {
        app_player_wav_file_end();
    }
    
    err = fat_free_files_buffer();
    if(err == FR_INT_ERR)
    {
        os_printf("fat_free_files_buffer failed, please check\r\n");
        while(1);
    }
    //os_printf("fat_free_files_buffer is OK\r\n");
    j_memset( &app_player, 0, sizeof(app_player_ctrl) );
    mp3filecount = 0;
    os_printf("app_player_unload\r\n");
    return 0;
}

static int app_player_to_first_file( void )
{
    FILE_INFO *pFile;
	
    uint32 player_flag = app_player.player_flag;
    uint32 schedule_cmd = app_player.schedule_cmd;
    memset( (uint8 *)&app_player, 0, sizeof( app_player_ctrl ) );
    //os_printf("app_player_to_first_file\r\n");
    app_player.player_flag = player_flag;
    app_player.schedule_cmd = schedule_cmd;
#if 0   //for new fat
    fat_change_to_root_dir();

    app_player_get_next_stream_file();

    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;

    if(app_player.file_index == 0)
    {
        os_printf("get to first file failed!!!\r\n");
        return -1;
    }

#ifdef APP_PLAYER_DEBUG
    os_printf("get first file app_player.file_index=%d\r\n",app_player.file_index);
#endif
#endif
    if( 0 == get_musicfile_count())
    {
        return -1;
    }

    app_player.dir_index = 0;
    app_player.file_index = 0;
    newFatfs = Get_Cur_Dir_file(app_player.dir_index,app_player.file_index);
    pFile = get_file_info();
	
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
    app_player.file_info.file_blks = newFatfs->obj.objsize;
    app_player.file_info.file_start_cluster = newFatfs->obj.sclust;

    app_player.block_played = 0;
    memcpy(app_player.file_info.ext_name,pFile->extname,3);
    memcpy(app_player.file_info.filename,pFile->fname,8);
    //os_printf("filename+++++++++++++++++++++:%s\r\n",app_player.file_info.filename);
    return 0;
}


static int app_player_change_play_mode()
{
    if(++appPlayerPlayMode==APP_PLAYER_MODE_END)
        appPlayerPlayMode=0;
    //os_printf("appPlayerPlayMode=%d\r\n",appPlayerPlayMode);
    return 0;
}
static int app_player_get_next_dir(void)
{
    FILE_INFO *pFile ;
    if(++app_player.dir_index>=mp3queuecount)
        app_player.dir_index=0;
    app_player.file_index = 0;
    os_printf("totalDir=%d,currentDir=%d,app_player.file_index=%d\r\n",mp3queuecount,app_player.dir_index,app_player.file_index);
    newFatfs = Get_Cur_Dir_file(app_player.dir_index,app_player.file_index);
    pFile = get_file_info();
	
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
	 app_player.file_info.file_blks = newFatfs->obj.objsize;
    app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
    app_player.block_played = 0;
    
    memcpy(app_player.file_info.ext_name,pFile->extname,3);
    memcpy(app_player.file_info.filename,pFile->fname,8);
    //os_printf("filename+++++++++++++++++++++:%s\r\n",app_player.file_info.filename);
    return 0;
}
static int app_player_get_next_stream_file( void )
{
    FILE_INFO *pFile ;
    uint16 musicfilecount;
    static unsigned long next=1;

    musicfilecount =get_curDirMusicfile_count(app_player.dir_index);
    if( 0 == musicfilecount)
    {
        app_player.file_index=0;
        app_player.dir_index++;
        if(app_player.dir_index>=mp3queuecount)
            app_player.dir_index=0;
    }
    if(appPlayerPlayMode==APP_PLAYER_MODE_PLAY_RANDOM)
    {
        next = next*1103515245+12345;
        app_player.dir_index = next%mp3queuecount;
        app_player.file_index = next%get_curDirMusicfile_count(app_player.dir_index);
    }
    else
    {
        app_player.file_index++;
        if(app_player.file_index >=musicfilecount)
        {
            app_player.file_index = 0;
            app_player.dir_index++;
            if(app_player.dir_index>=mp3queuecount)
                app_player.dir_index=0;
        }
    }
    newFatfs = Get_Cur_Dir_file(app_player.dir_index,app_player.file_index);
    os_printf("next_file.... app_player.dir_index=%d,app_player.file_index=%d\r\n",app_player.dir_index,app_player.file_index);
    pFile = get_file_info();
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
    app_player.file_info.file_blks = newFatfs->obj.objsize;
    app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
    app_player.block_played = 0;
    memcpy(app_player.file_info.ext_name,pFile->extname,3);
    memcpy(app_player.file_info.filename,pFile->fname,8);
    //os_printf("filename+++++++++++++++++++++:%s\r\n",app_player.file_info.filename);
    return 0;
}


static int app_player_get_prev_stream_file( void )
{
    FILE_INFO *pFile;
    uint16 musicfilecount;
    static unsigned long next=1;
    musicfilecount =get_curDirMusicfile_count(app_player.dir_index);
	
    if( 0 == musicfilecount)
    {
        app_player.file_index=0;
        if(app_player.dir_index>0)
            app_player.dir_index--;
        else
        {
            if(mp3queuecount>=1)
                app_player.dir_index=mp3queuecount-1;
            else
                return -1;
        }
    }
    if(appPlayerPlayMode==APP_PLAYER_MODE_PLAY_RANDOM)
    {
        next = next*1103515245+12345;
        app_player.dir_index = next%mp3queuecount;
        app_player.file_index = next%get_curDirMusicfile_count(app_player.dir_index);
    }
    else
    {
        if(app_player.file_index>0)
        {
            app_player.file_index--;
        }
        else
        {
            if(app_player.dir_index>0)
                app_player.dir_index--;
            else 
                app_player.dir_index = mp3queuecount-1;
            app_player.file_index = get_curDirMusicfile_count(app_player.dir_index)-1;
        }
    }
    newFatfs = Get_Cur_Dir_file(app_player.dir_index,app_player.file_index);
    os_printf("prev_file.... app_player.dir_index=%d,app_player.file_index=%d\r\n",app_player.dir_index,app_player.file_index);
    pFile = get_file_info();
	
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
    app_player.file_info.file_blks = newFatfs->obj.objsize;
    app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
    app_player.block_played = 0;
    
    memcpy(app_player.file_info.ext_name,pFile->extname,3);
    memcpy(app_player.file_info.filename,pFile->fname,8);
    //os_printf("filename+++++++++++++++++++++:%s\r\n",app_player.file_info.filename);
    return 0;
}
static int app_player_get_fix_stream_file( uint16  fileIndex )
{
    FILE_INFO *pFile ;
    uint16 musicfilecount =get_curDirMusicfile_count(app_player.dir_index);
	
    if( 0 == musicfilecount)
    {
        return -1;
    }
    
    app_player.file_index = fileIndex;
    if(app_player.file_index > (musicfilecount-1))
        app_player.file_index = 0;
        
    newFatfs = Get_Cur_Dir_file(app_player.dir_index,app_player.file_index);
    os_printf("fix_file  app_player.file_index=%d\r\n",app_player.file_index);
    pFile = get_file_info();
	
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
    app_player.file_info.file_blks = newFatfs->obj.objsize;
    app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
    app_player.block_played = 0;
    
    memcpy(app_player.file_info.ext_name,pFile->extname,3);
    memcpy(app_player.file_info.filename,pFile->fname,8);
    
    return 0;
}

static void app_player_file_info_init( void )
{
    int file_index;
    uint16 dir_index;
    FILE_INFO *pFile;
    if( 0 == get_musicfile_count())
    {
    //若无音频文件，则切换模式
        msg_put(MSG_CHANGE_MODE);
        return;
    }
		
    memset( (uint8 *)&app_player, 0, sizeof( app_player ) );
    app_player.player_flag |= (APP_PLAYER_FLAG_PLAY_CONTINOUS|APP_PLAYER_FLAG_PLAY_CYCLE|APP_PLAYER_FLAG_HW_ATTACH);
    //lianxue.liu
    app_restore_breakpoint();

    //if the previous player_vol_music is 0, need mute the system vol
//    if(player_vol_music == 0)
//        aud_volume_set(player_vol_music);
    dir_index = player_config_backup.dir_index;
    file_index = player_config_backup.file_index;
    //os_printf("file index :%x\r\n",file_index);
    //check the file is exist or not
    if((dir_index>=mp3queuecount)||(file_index >=get_curDirMusicfile_count(dir_index)) || (file_index < 0))
    {
        app_player_to_first_file();
    }
    else
    {
        newFatfs = Get_Cur_Dir_file(dir_index,file_index);
        pFile = get_file_info();
        app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
        app_player.file_info.file_blks = newFatfs->obj.objsize;
        app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
        app_player.block_played = 0;
        memcpy(app_player.file_info.ext_name,pFile->extname,3);
        memcpy(app_player.file_info.filename,pFile->fname,8);
        if(app_player.file_info.file_blks == player_config_backup.file_size_blks)
        {
            app_player.dir_index = player_config_backup.dir_index;
            app_player.file_index = player_config_backup.file_index;
            app_player.block_played = player_config_backup.block_played;
            if( j_strncmp(app_player.file_info.ext_name, "WAV", 3 ) == 0 )
            {
                app_player.block_played = 0;
                f_lseek(newFatfs,0);
            }
            else
            {
            #if CALC_PLAY_TIME 
                app_player.adjust_playtime = 1;
            #endif
                f_lseek(newFatfs, app_player.block_played);
            }
        }
        else
        {
            app_player_to_first_file();
        }
    }
}

#if 0
void app_player_print_file_info( void )
{
    os_printf("File: %s, start index : %d, file size: %d\r\n",FileInfo.fname,app_player.block_played,
              app_player.file_info.file_blks );

}
#endif

void app_player_play_pause_caller( int play_pause )
{
    app_player_play_pause( play_pause );
}

static void app_player_play_pause( int play_pause  )
{
    uint32 status;// = BK3000_SDIO_CMD_RSP_INT_SEL;
    uint32 readbytes = 0;
    app_handle_t app_h = app_get_sys_handler();

    if(get_musicfile_count() == 0 )
        return;
    os_printf("app_player_play_pause:%d,0x%x,player_vol_bt:%d\r\n",play_pause,app_player.player_flag,player_vol_bt);
    if((play_pause == 0) &&  (app_player.player_flag &  APP_PLAYER_FLAG_PLAY_PAUSE ))  // pause
    {
        os_printf("MP3 PAUSE\r\n");
    #if defined(CONFIG_APP_SDCARD) //&& defined(CONFIG_LINE_SD_SNIFF)
        if(SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
            sdcard_idle(1);
    #endif
        app_set_led_event_action(LED_EVENT_MUSIC_PAUSE);
        app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
        aud_PAmute_oper(1);
//        enable_audio_ldo_for_music_files();
        aud_close();
#ifdef APP_PLAYER_DEBUG_FOR_QUICK_PLAY
        quick_play_flag = 1;
#endif
    }
    else if( play_pause && !( app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE))
    {
        if(bt_flag1_is_set( APP_FLAG_WAVE_PLAYING ))
            return;
    #if defined(CONFIG_APP_SDCARD) //&& defined(CONFIG_LINE_SD_SNIFF)
        if(SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
            sdcard_idle(0);
    #endif
        os_printf("MP3 PLAY\r\n");
        app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
        if( j_strncmp( app_player.file_info.ext_name, "WAV", 3 ) == 0 )
        {
            app_player.wav_read_flag = 0;
            //app_player_print_file_info();
            if( app_player.wav_header_index == 45 )
            {
                init=0;
                os_printf("player_vol_bt = %d\r\n", player_vol_bt);
                aud_close();
                aud_initial( app_player.wav_freq, app_player.wav_channel, 16 );
                aud_volume_set(player_vol_bt);
                aud_open();            
/*
                status = f_read(newFatfs, rbbuf, sblksize, &readbytes);
                if(status > 0)
                {
                    app_player.block_played = app_player.file_info.file_blks;
                }
                wav_fill_buffer(rbbuf,readbytes);
                app_player.block_played += readbytes;*/
                app_player.wav_read_flag = 1;
            }
            else
            {
                //os_printf("wave start vol:%d \r\n",player_vol_bt);

                wav_mem_init();
                status = f_read(newFatfs, rbbuf, 2048, &readbytes);
                if(status > 0)
                {
                    app_player.block_played = app_player.file_info.file_blks;
                }
                else
                {
                    app_player.block_played += readbytes;
                    app_player_wave_header_handler(rbbuf,readbytes);
		      //os_printf("start wav: readbytes=%x\r\n",readbytes);
                    status = f_read(newFatfs, rbbuf, sblksize, &readbytes);
		      //os_printf("read wav: status=%x\r\n",status);
                    wav_fill_buffer(rbbuf, readbytes);
		      //os_printf("wav fill buffer: readbytes=%x\r\n",readbytes);
                    app_player.block_played += readbytes;
                #if CALC_PLAY_TIME 
                    app_player_wave_calc_total_playtime();
                #endif
                }
            }

        }
        else if( j_strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
        {
            //app_player_print_file_info();
            if(mp3_play_flag != 1)
            {
                //os_printf("MP3 start vol:%d \r\n",player_vol_bt);
                app_player_mp3_init();
                if(mp3decinfo) 
                {
                    mp3decinfo->decode_state = MP3_DECODE_FIND_ID3_INFO;

                #if CALC_PLAY_TIME
                    mp3decinfo->totalframes = newFatfs->obj.objsize;
                    mp3decinfo->curFramecnt = 0;
                #endif
                }
                else	
                {
                    os_printf("mp3 memery initial unsucssed\r\n");
                }

                BUF_CUTDOWN_DEBUG("mp3 initial is ok \r\n");
                mp3_play_flag = 1;
            }
            else
            {
                mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
                init = 0;
                t = 0;
                aud_buff_full = 0;
                BUF_CUTDOWN_DEBUG("continue playing mp3\r\n");
            }
        }
        else
        {
            msg_put(MSG_KEY_NEXT);//下一曲
        }
        app_player.player_flag |= APP_PLAYER_FLAG_PLAY_PAUSE;
        //		enable_audio_ldo();
        aud_PAmute_oper(0);
    }
}

static void app_player_stop( void  )
{
    if( app_player.file_index < 0 )
        return;

    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
    if( j_strncmp( app_player.file_info.ext_name, "WAV", 3 ) == 0 )
    {
 //       app_player.cur_block = app_player.start_block;
        app_player.block_played = 0;
        app_player.wav_header_index = 0;
        app_player.wav_read_flag = 0;
    }
    else if( j_strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
    {
//        app_player.cur_block = app_player.start_block;
        app_player.block_played = 0;
    }

}
static void app_player_process_file_end( void )
{
    if(app_player.media_err_status == FR_DISK_ERR)
    {
        app_player.media_err_status = FR_OK;
        msg_put(MSG_MEDIA_READ_ERR);
        return;
    }
    app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
    if(app_player.wav_header_index != 0)//for player bug
        app_player.wav_header_index = 0;
    if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_CONTINOUS )
    {
        int get_file_return = 0;
		
    //if(play_derect == 1)  //  自动下一曲只需一个方向
    //{
        if(appPlayerPlayMode==APP_PLAYER_MODE_PLAY_ONE)
            get_file_return = app_player_get_fix_stream_file(app_player.file_index); // get current file
        else
        {
        #ifdef MUSIC_PREV_SONG
            if(flag_prev_song) 
                get_file_return = app_player_get_prev_stream_file();                    // get prev file
            else
        #endif
                get_file_return = app_player_get_next_stream_file();                    // get next file
        }
    //}
    //else if(play_derect == 0)
    //{
    //    get_file_return = app_player_get_prev_stream_file();                    // get prev file
    //}
		
        //when switch to next/prev file, need to save the break point
//        app_player_save_breakpoint();
        app_player_play_pause(1);                                       // not the file list end, just play next file
        (void)get_file_return;
    }
    else // not continous play
    {
        if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_CYCLE )   // cycle , play last index
        {
            f_lseek(newFatfs,0);
            app_player.block_played = 0;
            app_player_play_pause(1);
        }
        else                                                                                // just close
        {
            app_player_stop();
            aud_close();
        }
    }
}

static void app_player_wav_file_end( void )
{
    app_player.wav_header_index = 0;
    mp3_play_flag = 0;   
    wav_mem_uninit();
    f_close(newFatfs);
}

static void app_player_mp3_file_end( void )
{
    t = 0;
    init=0;
    media_err_cnt = 0;
    mp3_play_flag = 0;
    aud_buff_full = 0;
    mp3_mem_uninit();
    rbbuf = NULL;
    f_close(newFatfs);
}

static void app_player_play_wave_file( void )
{
    uint32 status;
    uint32 readbytes;
    uint16 audsize = 0;

    if( app_player.block_played >= app_player.file_info.file_blks )   // play to end
    {
        app_player.wav_header_index = 0;
        //os_printf("END 2\r\n");
        app_player_wav_file_end();
        app_player_process_file_end();
    }
    //else if((aud_get_buffer_size() >= buffsize) && app_player.wav_read_flag && (app_player.wav_header_index == 45) )        // playing , refresh the audio data
    else if(app_player.wav_read_flag && (app_player.wav_header_index == 45) )
    {
        audsize = aud_get_buffer_size();
        //os_printf("audio size = %d\r\n",audsize);
        if(audsize >= sbuffsize)
        {
            //APP_PLAYER_WAVE_READ_BLOCK_NUM
            app_player.wav_read_flag = 0;
            status = f_wave_read(newFatfs, rbbuf, sblksize, &readbytes);
            app_player.wav_read_flag = 1;
			
            if(status > 0)
            {
                media_err_cnt++;
                if(media_err_cnt > MEDIA_ERR_CNT_THRE)
                {
                    media_err_cnt = 0;
                    app_player.block_played = app_player.file_info.file_blks;//end process
                    app_player.media_err_status = FR_DISK_ERR;
                }
                return;
            }
			
            if(readbytes == 0)
              app_player.block_played =  app_player.file_info.file_blks;
            media_err_cnt = 0;
            app_player.block_played += readbytes;
        }
    }
}

#if CALC_PLAY_TIME
uint32 app_player_show_playtime( void )
{
	uint32 time = 0;
    if(mp3_play_flag == 1)
        time = MP3_Calc_Current_Play_Time(hMP3Decoder); 
    else if(mp3_play_flag == 2) {
        time = app_player_wave_calc_cur_playtime();
    }
	return time;
}
#endif

static void MP3_CALL app_player_play_mp3_file( void )
{	
    int err; 
    static uint16 error_cnt = 0; 
    if(!hMP3Decoder && mp3_play_flag)
    {
        return ;
    }

    if(aud_buff_full)
    {    
        goto fill_aud_buff;
    }

    err = MP3Decode(hMP3Decoder, mp3_pcm_ptr, &pcm_size);
	
    if(err != ERR_MP3_NONE)
    {	
        switch(err)
        {
            case ERR_MP3_INDATA_UNDERFLOW:
            {
                if(f_EOF(newFatfs) == FR_FILE_END)
                {
                    app_player_mp3_file_end();
                    app_player_process_file_end();
                }
                else
                {
                    mp3decinfo->decode_state = MP3_DECODE_FIND_ID3_INFO;
                    os_printf("not the end of the file\r\n");
                }
                break;
            }	
            case ERR_MP3_READ_DATA_FAILED:
            {
                os_printf("**mp3 read error**:%d\r\n",error_cnt);
                if((error_cnt++ > 30) && Media_is_online())
                {
                    error_cnt = 0;
                    msg_put(MSG_CHANGE_MODE);
                    //防止读卡出错时，切换模式不重新初始化		
                    clr_sd_noinitial_flag();
                }
                break;
            }
            case ERR_MP3_FILE_END:
            {
                app_player_mp3_file_end();
                //os_printf("mmmm\r\n");
                app_player_process_file_end();
                //os_printf("nnnnn\r\n");

                if(error_cnt++ > 30)
                {
                    error_cnt = 0;
                    msg_put(MSG_CHANGE_MODE);
                }
                break;
            }
            case ERR_MP3_MAINDATA_UNDERFLOW:
            {
                mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
                break;
            }
            case ERR_MP3_DECODE_MAX_ERR:
                os_printf("---mp3 max error------\r\n");
                app_player_mp3_file_end();
                app_player_process_file_end();
                break;
            default:
                ClearMP3((MP3DecInfo *)hMP3Decoder);
                mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
                break;

        }
        os_printf("mp3 retrun err:%x,%x,%x\r\n",err,app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE,app_player.schedule_cmd & 0xFFFF );
        return;
    }  
    error_cnt = 0;

    // wait for at least 4 corret frame decode. 
    if((t > APP_PLAYER_AUDIO_INITION_BEGIN) && init == 0)
    {
        uint32 interrupts_info,mask;
    #if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
    #endif
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo); //获得解码
        os_printf("比特率%dkb/s, 原采样率%dHZ\r\n",
        (mp3FrameInfo.bitrate)/1000,mp3FrameInfo.samprate);
        os_printf("原声道数%d, MPAG:%d,层:%d\r\n",
        mp3FrameInfo.nChans,mp3FrameInfo.version,mp3FrameInfo.layer);
        os_printf("outputsamps:%d\r\n",mp3FrameInfo.outputSamps);
        os_printf("每帧数据:%d\r\n",mp3decinfo->framesize);

    #if CALC_PLAY_TIME
        if(app_player.adjust_playtime) {
        app_player.adjust_playtime = 0;
        mp3decinfo->curFramecnt = newFatfs->fptr / mp3decinfo->framesize;
        }
        MP3_Calc_Play_Time(hMP3Decoder, &mp3FrameInfo);
    #endif

        VICMR_disable_interrupts(&interrupts_info,&mask);
        aud_PAmute_oper(1);
    #if (CONFIG_NOUSE_RESAMPLE == 0)
    #ifdef  FIR_HARD_CORE
        if(mp3FrameInfo.samprate == 11025 || mp3FrameInfo.samprate == 12000)
        {
            fir_hardcore_config(UPSMPL4_1);
        }
        else
        {
            fir_hardcore_config(UPSMPL2_1);
        }
    #endif
        aud_close();
        if(mp3FrameInfo.samprate == 48000
            || mp3FrameInfo.samprate == 24000
            || mp3FrameInfo.samprate == 12000)
        {
            aud_initial(48000, 2, 16 );
        }
        else if(mp3FrameInfo.samprate == 32000 || mp3FrameInfo.samprate == 16000)
        {
            aud_initial(16000, 2, 16 );
        }
        else if(mp3FrameInfo.samprate == 8000)
        {
            aud_initial(8000, 2, 16 );
        }
        else
        {
            aud_initial(44100, 2, 16 );
        }
    #else
        aud_close();
        aud_initial(mp3FrameInfo.samprate, 2, 16 );
    #endif

        aud_open();
        aud_volume_set(player_vol_bt);
        aud_PAmute_oper(0);
        
        init=2;
        VICMR_restore_interrupts(interrupts_info,mask);
        init=1;	
    }
    else if(init == 0)
    {
        goto decode_exit;
    }
	
    if(err == ERR_MP3_NONE )
    {
        mp3_pcm_ptr_tmp = mp3_pcm_ptr;
        MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
        if(mp3FrameInfo.nChans == 1)
        {
            Convert_Mono(mp3_pcm_ptr,pcm_size);
            pcm_size <<= 1;
        }

    fill_aud_buff:
        buf_free = aud_get_buffer_size();
    
        if(buf_free > pcm_size*2)
        {
            //os_printf("PCM SIZE =%x,%x\r\n",pcm_size,buf_free);
            aud_fill_buffer((uint8*)mp3_pcm_ptr_tmp, pcm_size*2);   
            aud_buff_full = 0;
            pcm_size = 0;
        }
        /*at least 6 byte space in audbuffer, we can fill it*/
        else if(buf_free >= 6)
        {
            //we cann't fill audbuffer to full, should be 4bytes less 
            //see func: aud_fill_buffer
            buf_free = (buf_free&0xfffffffe)-4;
            aud_fill_buffer((uint8*)mp3_pcm_ptr_tmp, buf_free);
            mp3_pcm_ptr_tmp += (buf_free>>1);
            pcm_size -= (buf_free>>1);
            aud_buff_full = 1;
            return;
        }
        else
        {
            aud_buff_full = 1;
            return;
        }
    }

decode_exit:
    if(mp3decinfo->decode_state == MP3_DECODE_FIND_SYNC_WORD)
    {
        t++;
    }			
}

#if 0
int app_player_state_pause( void )
{
    return ( !(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE )
                    || (!(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH)));
}
#endif

static void app_player_process_schedule_cmd( void )
{
    switch( app_player.schedule_cmd & 0xFFFF )
    {
        case APP_PLAYER_SCHEDULE_CMD_MODE_CHANGE:
            //os_printf("app_player_process_schedule_cmd\r\n");
            aud_close();
            app_player.schedule_cmd = 0;
            break;
            
	case APP_PLAYER_SCHEDULE_CMD_FIX_SONG:
            if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
            {
                int get_next_return=0;

                //record the schedule cmd derection here.
                play_derect = 1;

                app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
                if(app_player.wav_header_index != 0) //for player bug
                    app_player.wav_header_index = 0;
                if(mp3_play_flag == 1)
                {
                    //os_printf("END 3\r\n");
                    app_player_mp3_file_end();
                }
                else if(mp3_play_flag == 2)
                {
                    //os_printf("END 3\r\n");
                    app_player_wav_file_end();
                }

                //os_printf("next_s %d\r\n", os_get_tick_counter());
                get_next_return = app_player_get_fix_stream_file(input_number-1); 				   // get next file
                input_number = 0;
                //os_printf("next_e %d\r\n", os_get_tick_counter());

                //when switch to next file, need to save the break point
                app_player_save_breakpoint();

                if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
                    app_player_play_pause(1);
                //app_player_print_file_info();
                (void)get_next_return;
            }
            app_player.schedule_cmd = 0;
            break;
            
        case APP_PLAYER_SCHEDULE_CMD_NEXT_DIR:
            if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
            {
                int get_next_return=0;

                app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
                if(app_player.wav_header_index != 0) //for player bug
                    app_player.wav_header_index = 0;
                if(mp3_play_flag == 1)
                {
                    //os_printf("END 3\r\n");
                    app_player_mp3_file_end();
                }
                else if(mp3_play_flag == 2)
                {
                    //os_printf("END 3\r\n");
                    app_player_wav_file_end();
                }
                //os_printf("next_s %d\r\n", os_get_tick_counter());
                get_next_return = app_player_get_next_dir();                    // get next dir
                //os_printf("next_e %d\r\n", os_get_tick_counter());
                app_player_save_breakpoint();
                if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
                    app_player_play_pause(1);
                (void)get_next_return;
            }
            app_player.schedule_cmd = 0;
            break;
            
        case APP_PLAYER_SCHEDULE_CMD_NEXT_SONG:
            if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
            {
                int get_next_return=0;

                //record the schedule cmd derection here.
                play_derect = 1;

                app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
                if(app_player.wav_header_index != 0) //for player bug
                    app_player.wav_header_index = 0;
                if(mp3_play_flag == 1)
                {
                    //os_printf("END 3\r\n");
                    app_player_mp3_file_end();
                }
                else if(mp3_play_flag == 2)
                {
                    //os_printf("END 3\r\n");
                    app_player_wav_file_end();
                }

                //os_printf("next_s %d\r\n", os_get_tick_counter());
                get_next_return = app_player_get_next_stream_file();                    // get next file
                //os_printf("next_e %d\r\n", os_get_tick_counter());

                //when switch to next file, need to save the break point
                app_player_save_breakpoint();

                if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
                    app_player_play_pause(1);
                //app_player_print_file_info();
                (void)get_next_return;
            }
            app_player.schedule_cmd = 0;
            break;

        case APP_PLAYER_SCHEDULE_CMD_PREV_SONG:
            if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
            {
                int get_pre_return=0;

                play_derect = 0;

                app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
                if(app_player.wav_header_index != 0) //for player bug
                    app_player.wav_header_index = 0;
                if(mp3_play_flag == 1)
                {
                    //os_printf("END 4\r\n");
                    app_player_mp3_file_end();
                }
                else if(mp3_play_flag == 2)
                {
                    //os_printf("END 4\r\n");
                    app_player_wav_file_end();
                }
                //os_printf("pre_s %d\r\n", os_get_tick_counter());
                get_pre_return = app_player_get_prev_stream_file();
                //os_printf("pre_e %d\r\n", os_get_tick_counter());
                //when switch to prev file, need to save the break point
                app_player_save_breakpoint();

                if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
                    app_player_play_pause(1);
                (void)get_pre_return;
            }
            app_player.schedule_cmd = 0;
            break;

        default:
            break;
    }

    return;
}

void MP3_CALL app_player_play_func( void )
{
    if( !(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH ))
    {
        app_player_process_schedule_cmd();
        return;
    }

    if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE )
    {
        if( j_strncmp( app_player.file_info.ext_name, "WAV", 3 ) == 0 )
        {
            app_player_play_wave_file();
        }
        else if( j_strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
        {
            app_player_play_mp3_file();   
        }

        CLEAR_SLEEP_TICK;
    }
    app_player_process_schedule_cmd();
}

int player_get_play_status(void)
{
    return (app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE);
}

static void wav_mem_uninit(void)
{
    memory_usage_show();
    if(rbbuf != NULL)
    {
        jfree(rbbuf);
        rbbuf = NULL;
        //os_printf("f 1\r\n");
    }
    if(aulawsmpl != NULL)
    {
        jfree(aulawsmpl);
        aulawsmpl = NULL;
        //os_printf("f 2\r\n");
    }

    if(alawtbl != NULL)
    {
        jfree(alawtbl);
        alawtbl = NULL;
        //os_printf("f 3\r\n");
    }

    if(ulawtbl != NULL)
    {
        jfree(ulawtbl);
        ulawtbl = NULL;
        //os_printf("f 4\r\n");
    }
    memory_usage_show();
}

static void wav_mem_init(void)
{
    memory_usage_show();
    rbbuf = (uint8 *) jmalloc(2048,0);
    aulawsmpl = (short *)jmalloc(2048*2,0);
    alawtbl = (short *)jmalloc(256*2,0);
    ulawtbl = (short *)jmalloc(256*2,0);

    memcpy(alawtbl,pcm_Alaw_table,256*2);
    memcpy(ulawtbl,pcm_Ulaw_table,256*2);
    memory_usage_show();
}

static void mp3_mem_uninit(void)
{
    os_printf("mp3_mem_uninit()\r\n");
    memory_usage_show();
    if(readBuf)
    {
        os_printf("mp3 mem free	1\r\n");
        //jfree(readBuf);
        readBuf = NULL;
        mp3decinfo->mainBuf = NULL;
    }

    if(hMP3Decoder)
    {
        os_printf("mp3 mem free	3\r\n");
        MP3FreeDecoder(hMP3Decoder);
//	 os_printf("mp3 mem free	4\r\n");
        hMP3Decoder = NULL;
        mp3decinfo = NULL;
    }
    memory_usage_show();
}

extern SAMPLE_ALIGN u_int8 g_sbc_priv[]; // 2644+4
static void mp3_mem_init(void)
{
    L2DecodeContext *l2i;
    os_printf("mp3_mem_init\r\n");
    memory_usage_show();
    if(NULL == hMP3Decoder)
    {
        hMP3Decoder = MP3InitDecoder();
    }
    if(NULL == hMP3Decoder)
    {
        os_printf("hMP3Decoder_malloc_fail\r\n");
        memory_usage_show();
        goto exit;
    }

    mp3decinfo = (MP3DecInfo*)hMP3Decoder;
#if 1
    readBuf = &g_sbc_priv;
    memset(readBuf, 0, 2644+4);
#else
    readBuf= (unsigned char*)jmalloc(READBUF_SIZE, M_ZERO);   //FOR READ BUF_CUTDOWN_DEBUG
#endif
    mp3decinfo->mainBuf = readBuf; 
    mp3decinfo->mainBuf_ptr = mp3decinfo->mainBuf + BIT_RESVOR_SIZE;
    mp3decinfo->mainBuf_len = 0;
    mp3decinfo->err_cnt = 0;

    mp3_pcm_ptr = (short*)mp3decinfo->HuffmanInfoPS;
	//mp3_pcm_ptr = (short*)jmalloc(4608, M_ZERO); 
	
    l2i = (L2DecodeContext *)(mp3decinfo->L2DecInfo);
    l2i->sb_samples = (int32_t *)mp3decinfo->SubbandInfoPS;
    l2i->synth_buf = (int16_t *)mp3decinfo->IMDCTInfoPS;


    BUF_CUTDOWN_DEBUG("mp3_mem_init %p,%p,%p\r\n", readBuf, mp3_pcm_ptr, hMP3Decoder);
    memory_usage_show();
    return;

 exit:
    while(3254)
    {
        os_printf("F");
    }
    return;
}

#define FAST_FORWARD 0
#define FAST_BACKWARD 1
static uint8 step_sector = 0;
static void music_ff_fb_function(uint8 ff_fb)
{
    FRESULT res;
    uint32 ff_fb_bytes;
    if(!player_get_play_status())
    {
        return;
    }

    app_player_play_pause(0);
	
    if( j_strncmp( app_player.file_info.ext_name, "WAV", 3 ) == 0 )
    {
        step_sector = 20;
        ff_fb_bytes = step_sector<<15;
        if(ff_fb==FAST_FORWARD)
            app_player.block_played += ff_fb_bytes;
    }
    else
    {
        step_sector = 20; 
	 ff_fb_bytes = step_sector << 12;
    }
	
    res = f_SeekfromCurPos( newFatfs,ff_fb,ff_fb_bytes);

    if(res==FR_FILE_END)
    {
        os_printf("file end\r\n");
    }
    else if(res == FR_FILE_BEGIN)
    {
        os_printf("file begin\r\n");
    }
    
#if CALC_PLAY_TIME  
    app_player.adjust_playtime = 1;
#endif

    app_player_play_pause(1);
}

#ifdef CONFIG_BLUETOOTH_COEXIST
static uint8 mp3_player_status = 0; 
static uint8 flag_coexist_mp3_player = 0;
static app_coexist_recover_mp3_player(void)
{
    app_handle_t app_h = app_get_sys_handler();
    os_printf("app_coexist_recover_mp3_player(%d)\r\n",mp3_player_status);
    
    app_button_type_set(BUTTON_TYPE_NON_HFP);
    app_wave_file_play_stop();
    sbc_mem_free();
    app_player_init();
    if(app_h->sys_work_mode == SYS_WM_SD_MUSIC_MODE)
        Media_Fs_Init(0);
    else
        Media_Fs_Init(1);
    
    if(get_fat_ok_flag())
    {
        app_player_file_info_init();
        app_player_button_play_pause();
        //app_player_play_pause_caller(mp3_player_status);
    }
}

void app_coexist_mp3_player_play_pause(uint8 flag) // 1:recover music play    0:stop music
{
    app_handle_t app_h = app_get_sys_handler();
    
    os_printf("app_coexist_mp3_player_play_pause(%d,%d)\r\n",flag,flag_coexist_mp3_player);
    //if(app_is_mp3_mode())
    {
        if(flag && (flag_coexist_mp3_player != 1))
        {
            jtask_stop(app_h->app_coexist_task);
            jtask_schedule(app_h->app_coexist_task,2000,app_coexist_recover_mp3_player,NULL);
            flag_coexist_mp3_player = 1;
        }
        else if((0 == flag) && (flag_coexist_mp3_player != 2))
        {
            jtask_stop(app_h->app_coexist_task);
            //mp3_player_status = player_get_play_status();
            app_change_mode_flag_set();
            app_player_play_pause_caller(0);
            Delay(1000);
            app_player_unload(app_h->sys_work_mode );
            app_wave_file_play_stop();
            app_change_mode_flag_clear();
            flag_coexist_mp3_player = 2;
        }
    }
}
#endif

void music_mode_msg_handler(void)
{
    int ret;
    uint8 exit_flag = 0;
    MSG_T msg;
    app_handle_t app_h = app_get_sys_handler();
    
    app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
#ifdef CONFIG_BLUETOOTH_COEXIST
    flag_coexist_mp3_player = 0;
    if(!hci_get_acl_link_count(app_h->unit))
        bt_unit_set_scan_enable(app_h->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);
    else
        bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
#else
#if defined(CONFIG_LINE_SD_SNIFF)
#ifdef CONFIG_APP_USB 
    app_env_handle_t  env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
    {
        bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
    }
    else
#endif
        bt_unit_set_scan_enable( app_h->unit, HCI_PAGE_SCAN_ENABLE); // HCI_NO_SCAN_ENABLE isr ceva: 20s  
#else
     bt_unit_set_scan_enable(app_h->unit,HCI_NO_SCAN_ENABLE);
#endif			
#endif

    if(pre_enter_mode_handle(app_h->sys_work_mode))
        return;
    os_printf("---enter music mode---\r\n");
    memory_usage_show();
    app_restore_volume(ENV_MUSIC_VOL_INFO);
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif

    enter_work_mode();
#ifdef CONFIG_APP_EQUANLIZER
    app_equalizer_a2dp_apply();
#endif	
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
#endif

    if(get_fat_ok_flag()
    #ifdef CONFIG_BLUETOOTH_COEXIST
        &&(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
    #endif
    )
    {
        app_player_file_info_init();
        app_player_button_play_pause();
    #ifdef CONFIG_BLUETOOTH_COEXIST
        flag_coexist_mp3_player = 1;
    #endif
    }
#if defined(CONFIG_BLUETOOTH_SPP)&& defined(SUPPORT_SPP_IN_MP3PLAYER)
    if(app_mp3player_ble_spp_is_enable()) 
    {
        bt_unit_set_scan_enable( app_h->unit, HCI_PAGE_SCAN_ENABLE); 
        //bt_unit_set_scan_enable( app_h->unit, HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE );
    }
#endif
    while(!exit_flag)
    {
        ret = msg_get(&msg);
        if(MSG_FAILURE_RET == ret)
        {
            msg.id = MSG_NULL;
        }
        else
        {
            switch(msg.id)
            {
                case MSG_KEY_PLAY:
                #ifdef CONFIG_BLUETOOTH_COEXIST
                    if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
                    {
                    
                    }
                    else
                #endif
                    {
                        app_player_play_pause(!(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE));
                    #if defined(CONFIG_LINE_SD_SNIFF)
                        sniffmode_wakeup_dly = 1000;
                        if(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE)
                        {
                            bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                            app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
                        }
                        else
                        {
                            //os_printf("sd attatch:%x \r\n",app_is_not_auto_reconnection());
                            bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,0);
                            app_set_led_event_action(LED_EVENT_MUSIC_PAUSE);
                        }
                    #endif		
                    }		
                    break;

                case MSG_KEY_NEXT:
                #if defined(CONFIG_LINE_SD_SNIFF)
                    sniffmode_wakeup_dly = 1000;
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                #endif
                #ifdef MUSIC_PREV_SONG
                    flag_prev_song = 0;
                #endif
                    app_player_button_next();
                    app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
                    break;
                    
                case MSG_KEY_PREV:
                #if defined(CONFIG_LINE_SD_SNIFF)
                    sniffmode_wakeup_dly = 1000;
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                #endif
                #ifdef MUSIC_PREV_SONG
                    flag_prev_song = 1;
                #endif
                    app_player_button_prev();
                    app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
                    break;
                    
                case MSG_KEY_VOL_DOWN:
                    app_player_button_setvol_down();
                    break;
                    
                case MSG_KEY_VOL_UP:
                    app_player_button_setvol_up();
                    break;	
                    
                case MSG_KEY_REWIND:
                #if defined(CONFIG_LINE_SD_SNIFF)
                    sniffmode_wakeup_dly = 1000;
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                #endif
                    music_ff_fb_function(FAST_BACKWARD);
                    app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
                    break;
                case MSG_KEY_FORWARD:
                #if defined(CONFIG_LINE_SD_SNIFF)
                    sniffmode_wakeup_dly = 1000;
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                #endif
                    music_ff_fb_function(FAST_FORWARD);
                    app_set_led_event_action(LED_EVENT_MUSIC_PLAY);
                    break;	
                case MSG_CHANGE_PLAY_MODE:
                    app_player_change_play_mode();
                    break;
                case MSG_CHANGE_DIR:
                    app_player_button_nextDir();     
                    break;
                case MSG_HALF_SECOND:
                    break;
            #ifdef CONFIG_BLUETOOTH_COEXIST
                case MSG_KEY_HFP_ACK:
                    app_button_hfack_action();
                    break;
                case MSG_KEY_HFP_REJECT:
                    app_button_reject_action();
                    break;
                case MSG_KEY_NUM_REDIAL:
                	app_button_redial_last_number();
                	break;
                case MSG_KEY_TRANSFER_TOGGLE:
                    app_button_hf_transfer_toggle();
                    break;
            #endif       
                default:
                    exit_flag = common_msg_handler(&msg);
                    break;
            }
        }
        if(app_wave_playing())
            app_wave_file_play();
        else
            app_player_play_func();
    
    #ifdef CONFIG_BLUETOOTH_COEXIST
    #ifdef CONFIG_APP_AEC
	 if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AEC_ENABLE))
            app_aec_swi();
    #endif
        BTms_Sched(2);//controller的处理
    #else
        BTms_Sched(1);//controller的处理
    #endif
        //Beken_Uart_Rx();//调试串口命令处理
        timer_clear_watch_dog();
    
    #ifdef CONFIG_BLUETOOTH_COEXIST
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
    #else
    #if (CONFIG_CPU_CLK_OPTIMIZATION == 1)&& defined(CONFIG_LINE_SD_SNIFF)
        if(( !bt_flag1_is_set( APP_FLAG_WAVE_PLAYING ) )&&( !bt_flag2_is_set( APP_FLAG2_LINEIN_SD_PLAYING ) ))
        {
            BK3000_set_clock(1, 0);
        }
    #endif
    #endif
    }
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,0);
#endif
    app_exit_sniff_mode();
#endif
	//set_wake_gpio(0);
    memory_usage_show();
    os_printf("---exit music mode---\r\n");
}
#endif


