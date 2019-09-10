/**
 **************************************************************************************
 * @file    media_play_api.c
 * @brief   
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-3-17 13:06:47$
 * 
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include "string.h"
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "gpio.h"
#include "irqn.h"
#include "gpio.h"
#include "clk.h"
#include "dac.h"
#include "otg_host_standard_enum.h"
#include "otg_detect.h"
#include "delay.h"
#include "rtos_api.h"
#include "freertos.h"
#include "ff.h"
#include "ffpresearch.h"
#include "sd_card.h"
#include "debug.h"
#include "dac_interface.h"
#include "media_play_api.h"
#include "audio_core_api.h"
#include "decoder_service.h"
#include "device_detect.h"
#include "remind_sound_service.h"
#include "breakpoint.h"
#include "mode_switch_api.h"
#include "main_task.h"
#include "recorder_service.h"
#include "string_convert.h"
#include "random.h"
#include "timeout.h"
#ifdef CFG_FUNC_LRC_EN
#include "lrc.h"
#endif


#if defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN) || defined(CFG_FUNC_RECORDER_EN)
extern uint8_t MediaPlayDevice(void);
extern uint32_t PlaySoneNum; ////only test
extern bool IsUDiskLink(void);//device_detect.c


bool MediaPlayerOpenSongFile(void);
bool MediaPlayerFileDecoderInit(void);

FRESULT f_open_recfile_by_num(FIL *filehandle, UINT number);
bool RecFilePlayerInit(void);
#ifdef CFG_FUNC_RECORD_FLASHFS
uint32_t FlashFsReadFile(void *buffer, uint32_t length); // 作为flashfs 回放callback
#endif

//快进快退 针对解码器有效性实施不同策略:
//解码器播控有效时，优先使用消息驱动解码器，依据文件时长判断进度和上下首
#define FF_FB_STEP                  2000
#define FF_PLAY_TIME_MAX			0x7FFFFFFF//快进封顶值。封顶后不再变更，等结束后设下一首
#define FB_PLAY_LAST				0xFFFFFFFF //快退封底为0，封底后再退即FB_PLAY_LAST，等结束后设上一首

MEDIA_PLAYER* gpMediaPlayer = NULL;

static char current_vol[8];//disk volume like 0:/, 1:/
uint8_t	file_longname[FF_LFN_BUF + 1];
FileType SongFileType;

#if defined(CFG_PARA_RECORDS_FOLDER) && !defined(CFG_FUNC_FOLDER_PLAY)
bool FolderExclude_callback(FILINFO *finfo)
{
	if(strlen(finfo->fname) != strlen(CFG_PARA_RECORDS_FOLDER))
	{
		return FALSE;
	}
	if(strcmp(finfo->fname, CFG_PARA_RECORDS_FOLDER))//长度相同时，字符串不同
	{
		return FALSE;
	}
	return TRUE;
}
#else //未定义录音文件夹时，不启用文件夹过滤
#define FolderExclude_callback 		NULL
#endif

//媒体源设备，硬件初始化和扫描
//目前支持TF卡/SD卡，U盘。注意挂载和卸载。
static bool HardwareInit(DEV_ID DevId)
{
	uint8_t Retry = 8;//挂载尝试次数。
	DBG("hardware init start.\n");
	switch(DevId)
	{
#ifdef CFG_RES_CARD_USE
		case DEV_ID_SD:
		{
			CardPortInit(CFG_RES_CARD_GPIO);
			if(SDCard_Init() != NONE_ERR)
			{
				DBG("Card init err.\n");
				return FALSE;
			}
			DBG("SDCard Init Success!\n");
			strcpy(current_vol, MEDIA_VOLUME_STR_C);

			while(f_mount(&gpMediaPlayer->gFatfs_sd, current_vol, 1) != FR_OK && --Retry);
			if(!Retry)
			{
				DBG("SD卡挂载 失败\n");
				return FALSE;
			}
			DBG("SD卡挂载  成功\n");
			return TRUE;
		}
#endif
#ifdef CFG_FUNC_UDISK_DETECT
		case DEV_ID_USB:
			//OTG_SetCurrentPort(UDISK_PORT_NUM);
			if(!IsUDiskLink())
			{
				DBG("UNLINK UDisk\n");
				return FALSE;
			}
			strcpy(current_vol, MEDIA_VOLUME_STR_U);
			do
			{
				if(!OTG_HostInit() == 1)
				{
					DBG("Host Init Disk Error\n");
					return FALSE;
				}

			}while(f_mount(&gpMediaPlayer->gFatfs_u, current_vol, 1) != FR_OK && Retry--);
			if(!Retry)
			{
				DBG("USB挂载  失败\n");
				return FALSE;
			}
			DBG("USB挂载  成功\n");
			return TRUE;
#endif
#ifdef CFG_FUNC_RECORD_FLASHFS
		case DEV_ID_FLASHFS:
			return TRUE;
#endif

		default:
			break;
	}
	return FALSE;
}


#ifdef CFG_FUNC_LRC_EN
static bool PlayerParseLrc(void)
{
	uint32_t SeekTime = gpMediaPlayer->CurPlayTime * 1000;
	static int32_t LrcStartTime = -1;

	if((gpMediaPlayer->LrcRow.MaxLrcLen = (int16_t)LrcTextLengthGet(SeekTime)) >= 0)
	{
		TEXT_ENCODE_TYPE CharSet;
		bool bCheckFlag = FALSE;
		
		if(gpMediaPlayer->LrcRow.MaxLrcLen > 128)
		{
			bCheckFlag = TRUE;
			gpMediaPlayer->LrcRow.MaxLrcLen = 128;
		}

		memset(gpMediaPlayer->LrcRow.LrcText, 0, 128);

		LrcInfoGet(&gpMediaPlayer->LrcRow, SeekTime, 0);
		if(gpMediaPlayer->LrcRow.StartTime + gpMediaPlayer->LrcRow.Duration == LrcStartTime)
		{
			return FALSE;
		}
		// ??? 下面的代码需要确认执行时间，如果时间较长，影响定时的精确度，需要移到外面执行
		// ??? 编码转换
		CharSet = LrcEncodeTypeGet();
		//DBG("CharSet = %d\n", CharSet);
		if(gpMediaPlayer->LrcRow.MaxLrcLen > 0 
		  && !(CharSet == ENCODE_UNKNOWN || CharSet == ENCODE_GBK || CharSet == ENCODE_ANSI))
		{
#ifdef  CFG_FUNC_STRING_CONVERT_EN
			uint32_t  ConvertType = UNICODE_TO_GBK;
			uint8_t* TmpStr = gpMediaPlayer->TempBuf2;

			if(CharSet == ENCODE_UTF8)
			{
				ConvertType = UTF8_TO_GBK;
			}
			else if(CharSet == ENCODE_UTF16_BIG)
			{
				ConvertType = UNICODE_BIG_TO_GBK;
			}
			//DBG("ConvertType = %d\n", ConvertType);
			memset(TmpStr, 0, 128);

			gpMediaPlayer->LrcRow.MaxLrcLen = (uint16_t)StringConvert(TmpStr,
			                                (uint32_t)gpMediaPlayer->LrcRow.MaxLrcLen,
			                                gpMediaPlayer->LrcRow.LrcText,
			                                (uint32_t)gpMediaPlayer->LrcRow.MaxLrcLen,
			                                ConvertType);

			memcpy(gpMediaPlayer->LrcRow.LrcText, TmpStr, gpMediaPlayer->LrcRow.MaxLrcLen);
			memset(TmpStr, 0, 128);

			memset((void*)(gpMediaPlayer->LrcRow.LrcText + gpMediaPlayer->LrcRow.MaxLrcLen),
			               0, 128 - gpMediaPlayer->LrcRow.MaxLrcLen);
#endif
			//gPlayContrl->LrcRow.LrcText[gPlayContrl->LrcRow.MaxLrcLen] = '\0';
		}

		// ??? 末尾汉字完整性检查
		if(bCheckFlag)
		{
			uint32_t i = 0;
			while(i < (uint32_t)gpMediaPlayer->LrcRow.MaxLrcLen)
			{
				if(gpMediaPlayer->LrcRow.LrcText[i] > 0x80)
				{
					i += 2;
				}
				else
				{
					i++;
				}
			}

			if(i >= (uint32_t)gpMediaPlayer->LrcRow.MaxLrcLen)
			{
				gpMediaPlayer->LrcRow.MaxLrcLen--;
				gpMediaPlayer->LrcRow.LrcText[gpMediaPlayer->LrcRow.MaxLrcLen] = '\0';
			}
		}

		APP_DBG("<%s>\r\n", gpMediaPlayer->LrcRow.LrcText);
		
		LrcStartTime = gpMediaPlayer->LrcRow.StartTime + gpMediaPlayer->LrcRow.Duration;
		return TRUE;
	}

	APP_DBG("gpMediaPlayer->LrcRow.MaxLrcLen = %d\n", gpMediaPlayer->LrcRow.MaxLrcLen);
	if(gpMediaPlayer->LrcRow.MaxLrcLen < 0)
	{
		gpMediaPlayer->LrcRow.MaxLrcLen = 0;
	}

	return FALSE;	
}

void SearchAndOpenLrcFile(void)
{
	int32_t i;
	int32_t Len = 0;
	bool ret;
	//FOLDER CurFolder;		

	{
		memset(gpMediaPlayer->TempBuf2, 0, sizeof(gpMediaPlayer->TempBuf2));
		
		//if(FR_OK != f_open_by_num(current_vol, &gpMediaPlayer->PlayerFile, gpMediaPlayer->CurFileIndex, (char*)file_longname))
		if(FR_OK != f_opendir_by_num(current_vol, &gpMediaPlayer->PlayerFolder, gpMediaPlayer->PlayerFile.dir_num, NULL))
		{
			DBG("folder open err!!\n");
		}

		if(file_longname[0] != 0)
		{
			i = strlen((char*)file_longname);
			while(--i)
			{
				if(file_longname[i] == '.')
				{
					memcpy(&gpMediaPlayer->TempBuf2[Len], file_longname, i + 1);
					{
						gpMediaPlayer->TempBuf2[i + 1 + Len] = (uint8_t)'l';
						gpMediaPlayer->TempBuf2[i + 2 + Len] = (uint8_t)'r';
						gpMediaPlayer->TempBuf2[i + 3 + Len] = (uint8_t)'c';
					}
					break;
				}
			}
			//DBG("lrc file long Name: %s\n", TempBuf2);
		}
		else
		{
			//memcpy(TempBuf2, (void*)gpMediaPlayer->PlayerFile.ShortName, 8);
			//memcpy(&gpMediaPlayer->TempBuf2[Len], f_audio_get_name(gpMediaPlayer->CurFileIndex), 8);//此处有问题，后续需要修改
			memcpy(&gpMediaPlayer->TempBuf2[Len], gpMediaPlayer->PlayerFile.fn, 8);
			memcpy(&gpMediaPlayer->TempBuf2[Len + 8], (void*)"lrc", 3);
			DBG("lrc file short Name: %s\n", gpMediaPlayer->TempBuf2);
		}

		//ret = f_open(&gpMediaPlayer->LrcFile, (TCHAR*)gpMediaPlayer->TempBuf2, FA_READ);
		ret = f_open_by_name_in_dir(&gpMediaPlayer->PlayerFolder, &gpMediaPlayer->LrcFile, (TCHAR*)gpMediaPlayer->TempBuf2);

		if(ret == FR_OK)
		{
			DBG("open lrc file ok\n");
			gpMediaPlayer->LrcRow.LrcText = gpMediaPlayer->TempBuf1; // 歌词内存区映射
			gpMediaPlayer->IsLrcRunning = TRUE;
			LrcInit(&gpMediaPlayer->LrcFile, gpMediaPlayer->ReadBuffer, sizeof(gpMediaPlayer->ReadBuffer), &gpMediaPlayer->LrcInfo);
		}
		else
		{
			gpMediaPlayer->IsLrcRunning = FALSE;
			DBG("No lrc\n");
		}
	}
}
#endif


//播放器初始化
bool MediaPlayerInitialize(DEV_ID DeviceIndex, int32_t FileIndex, uint32_t FolderIndex)
{
	//1、设备初始化
	//2、文件系统初始化
	//3、查找到文件进行decoder初始化
//	if(gpMediaPlayer != NULL)
//	{
//		APP_DBG("player is reopened\n");
//	}
//	else
//	{
//		gpMediaPlayer = osPortMalloc(sizeof(MEDIA_PLAYER));
//		if(gpMediaPlayer == NULL)
//		{
//			DBG("gpMediaPlayer malloc error\n");
//			return FALSE;
//		}
//	}
//
//	memset(gpMediaPlayer, 0, sizeof(MEDIA_PLAYER));

	SetMediaPlayerState(PLAYER_STATE_IDLE);
	if(!HardwareInit(DeviceIndex))
	{
		APP_DBG("Hardware initialize error\n");
		return FALSE;
	}
	if(GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay)
	{
#if defined(FUNC_MATCH_PLAYER_BP) && (defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN))
		DiskSongSearchBPInit();
		ffpresearch_init(DeviceIndex == DEV_ID_SD ? &gpMediaPlayer->gFatfs_sd : &gpMediaPlayer->gFatfs_u, DiskSongSearchBP, FolderExclude_callback);
#else
		ffpresearch_init(DeviceIndex == DEV_ID_SD ? &gpMediaPlayer->gFatfs_sd : &gpMediaPlayer->gFatfs_u, NULL, FolderExclude_callback);
#endif
		if(FR_OK != f_scan_vol(current_vol))
		{
			DBG("f_scan_vol err!");
			return FALSE;
		}

		APP_DBG("Hardware initialize success.\n");

		//默认使用全盘播放第一个文件
		gpMediaPlayer->CurFolderIndex = 1; // 根目录
#if defined(FUNC_MATCH_PLAYER_BP) && (defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN))
		gpMediaPlayer->CurFileIndex = BPDiskSongNumGet();
		DBG("CurFileIndex = %d\n", gpMediaPlayer->CurFileIndex);
		gpMediaPlayer->CurPlayTime = BPDiskSongPlayTimeGet();
		DBG("CurPlayTime = %d\n", (int)gpMediaPlayer->CurPlayTime);
		if(gpMediaPlayer->CurFileIndex == 0)
#endif
		{
			gpMediaPlayer->CurFileIndex = 1;
			gpMediaPlayer->CurPlayTime = 0;
		}

		gpMediaPlayer->TotalFileSumInDisk = f_file_real_quantity();
		gpMediaPlayer->ValidFolderSumInDisk = f_dir_with_song_real_quantity() + 1;//f_dir_real_quantity();
		if(!gpMediaPlayer->TotalFileSumInDisk)
		{
			return FALSE;
		}
		// 打开播放文件夹，默认全盘播放
		// 文件夹相关操作后续补充
	}
#ifdef CFG_FUNC_RECORDER_EN
	else //playback
	{
#ifdef CFG_FUNC_RECORD_UDISK_FIRST
		if(!RecFilePlayerInit())
		{
			return FALSE;
		}
#else //flashfs 回放
		if((gpMediaPlayer->FlashFsFile = Fopen(CFG_PARA_FLASHFS_FILE_NAME, "r")) == NULL)//flashfs无有效文件
		{
			DBG("open error");
			return FALSE;
		}
		Fclose(gpMediaPlayer->FlashFsFile);
		gpMediaPlayer->FlashFsFile = NULL;
		gpMediaPlayer->FlashFsMemHandle.addr = NULL;//不做缓冲，由解码器直接获取
		gpMediaPlayer->FlashFsMemHandle.mem_capacity = 0;
		gpMediaPlayer->FlashFsMemHandle.mem_len = 0;
		gpMediaPlayer->FlashFsMemHandle.p = 0;
		gpMediaPlayer->TotalFileSumInDisk = 1;//flashfs有效时只有单个录音文件。
#endif
		gpMediaPlayer->CurFolderIndex = 0xffff; // 不在加速器计算内
		gpMediaPlayer->CurFileIndex = 1;
		gpMediaPlayer->CurPlayTime = 0;
		gpMediaPlayer->CurPlayMode = PLAY_MODE_REPEAT_ALL;//回放录音只有循环(倒序)播放，不支持播放模式切换
	}
#endif
	uint16_t i;//首播歌曲出现故障时跳下一首，直到初始化解码器成功的歌曲
	for(i = 0; i < gpMediaPlayer->TotalFileSumInDisk; i++)
	{
		if(MediaPlayerFileDecoderInit())
		{
			break;
		}
		gpMediaPlayer->CurFileIndex = (gpMediaPlayer->CurFileIndex % gpMediaPlayer->TotalFileSumInDisk) + 1;
	}
	if(i == gpMediaPlayer->TotalFileSumInDisk)
	{
		return FALSE;//没有有效歌曲。
	}
	//刷新播放模式，文件夹播放需额外操作。

	MediaPlayerSwitchPlayMode(gpMediaPlayer->CurPlayMode, TRUE);

	SetMediaPlayerState(PLAYER_STATE_PLAYING);
	return TRUE;
}

void MediaPlayerDeinitialize(void)
{
	if(gpMediaPlayer != NULL)
	{
#ifdef CFG_FUNC_RECORD_UDISK_FIRST
		if(gpMediaPlayer->RecFileList != NULL)
		{
			osPortFree(gpMediaPlayer->RecFileList);
		}
#endif
	    osPortFree(gpMediaPlayer);
	    gpMediaPlayer = NULL;
	}
}

//文件解析 歌曲信息log
static void MediaPlayerSongInfoLog(void)
{
	SongInfo* PlayingSongInfo = audio_decoder_get_song_info();

	APP_DBG("PlayCtrl:MediaPlayerSongInfoLog\n");
	if(PlayingSongInfo == NULL)
	{
		return ;
	}

#ifdef CFG_FUNC_LRC_EN
    APP_DBG("LRC:%d\n", gpMediaPlayer->IsLrcRunning);
#else
    APP_DBG("LRC:0\n");
#endif
	APP_DBG("----------TAG Info----------\n");

	APP_DBG("CharSet:");
	switch(PlayingSongInfo->char_set)
	{
		case CHAR_SET_ISO_8859_1:
			APP_DBG("CHAR_SET_ISO_8859_1\n");
			break;
		case CHAR_SET_UTF_16:
			APP_DBG("CHAR_SET_UTF_16\n");
			break;
		case CHAR_SET_UTF_8:
			APP_DBG("CHAR_SET_UTF_8\n");
			break;
		default:
			APP_DBG("CHAR_SET_UNKOWN\n");
			break;
	}
#ifdef CFG_FUNC_STRING_CONVERT_EN
    if(PlayingSongInfo->char_set == CHAR_SET_UTF_8)
    {
        StringConvert(PlayingSongInfo->title,	  MAX_TAG_LEN, PlayingSongInfo->title,	   MAX_TAG_LEN, UTF8_TO_GBK);
        StringConvert(PlayingSongInfo->artist,	  MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, UTF8_TO_GBK);
        StringConvert(PlayingSongInfo->album,	  MAX_TAG_LEN, PlayingSongInfo->album,	   MAX_TAG_LEN, UTF8_TO_GBK);
        StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, UTF8_TO_GBK);
        StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, UTF8_TO_GBK);
    }
    else if(PlayingSongInfo->char_set == CHAR_SET_UTF_16)
    {
        StringConvert(PlayingSongInfo->title,	  MAX_TAG_LEN, PlayingSongInfo->title,	   MAX_TAG_LEN, UNICODE_TO_GBK);
        StringConvert(PlayingSongInfo->artist,	  MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, UNICODE_TO_GBK);
        StringConvert(PlayingSongInfo->album,	  MAX_TAG_LEN, PlayingSongInfo->album,	   MAX_TAG_LEN, UNICODE_TO_GBK);
        StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, UNICODE_TO_GBK);
        StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, UNICODE_TO_GBK);
    }
    else if((PlayingSongInfo->char_set & 0xF0000000) == CHAR_SET_DIVERSE)
    {
        uint32_t type = PlayingSongInfo->char_set & 0xF;
        if(type > 1)  
        {
            StringConvert(PlayingSongInfo->title,	  MAX_TAG_LEN, PlayingSongInfo->title,	   MAX_TAG_LEN, (type - 2) * 2); 
        }
        type = (audio_decoder->song_info->char_set >> 4)  & 0xF;
        if(type > 1)  
        {
            StringConvert(PlayingSongInfo->artist,	  MAX_TAG_LEN, PlayingSongInfo->artist,    MAX_TAG_LEN, (type - 2) * 2);
        }
        type = (audio_decoder->song_info->char_set >> 8)  & 0xF;
        if(type > 1)  
        {
            StringConvert(PlayingSongInfo->album,	  MAX_TAG_LEN, PlayingSongInfo->album,	   MAX_TAG_LEN, (type - 2) * 2);
        }
        type = (audio_decoder->song_info->char_set >> 12) & 0xF;
        if(type > 1)  
        {
            StringConvert(PlayingSongInfo->comment,   MAX_TAG_LEN, PlayingSongInfo->comment,   MAX_TAG_LEN, (type - 2) * 2);
        }
        type = (audio_decoder->song_info->char_set >> 16) & 0xF;
        if(type > 1)  
        {
            StringConvert(PlayingSongInfo->genre_str, MAX_TAG_LEN, PlayingSongInfo->genre_str, MAX_TAG_LEN, (type - 2) * 2);
        }
    }
#endif
    
    APP_DBG("title: %s\n", PlayingSongInfo->title);
    APP_DBG("artist: %s\n", PlayingSongInfo->artist);
    APP_DBG("Album: %s\n", PlayingSongInfo->album);
    APP_DBG("comment: %s\n", PlayingSongInfo->comment);
    APP_DBG("genre: %d %s\n", PlayingSongInfo->genre, PlayingSongInfo->genre_str);
    APP_DBG("year: %s\n", PlayingSongInfo->year);
    
    APP_DBG("\n");
    APP_DBG("----------------------------\n");
    APP_DBG("**********Song Info*********\n");
    APP_DBG("SongType:");
    switch(PlayingSongInfo->stream_type)
    {
        case STREAM_TYPE_MP2:
            APP_DBG("MP2");
            break;
        case STREAM_TYPE_MP3:
            APP_DBG("MP3");
            break;
        case STREAM_TYPE_WMA:
            APP_DBG("WMA");
            break;
        case STREAM_TYPE_SBC:
            APP_DBG("SBC");
            break;
        case STREAM_TYPE_PCM:
            APP_DBG("PCM");
            break;
        case STREAM_TYPE_ADPCM:
            APP_DBG("ADPCM");
            break;
        case STREAM_TYPE_FLAC:
            APP_DBG("FLAC");
            break;
        case STREAM_TYPE_AAC:
            APP_DBG("AAC");
            break;
        default:
            APP_DBG("UNKNOWN");
            break;
    }
    APP_DBG("\n");
    APP_DBG("Chl Num:%d\n", (int)PlayingSongInfo->num_channels);
    APP_DBG("SampleRate:%d\n", (int)PlayingSongInfo->sampling_rate);
    APP_DBG("BitRate:%d\n", (int)PlayingSongInfo->bitrate);
    APP_DBG("File Size:%d\n", (int)PlayingSongInfo->file_size);
    APP_DBG("TotalPlayTime:%dms\n", (int)PlayingSongInfo->duration);
    APP_DBG("CurPlayTime:%dms\n", (int)gpMediaPlayer->CurPlayTime);
    APP_DBG("IsVBR:%d\n", (int)PlayingSongInfo->vbr_flag);
    APP_DBG("MpegVer:");

    switch(audio_decoder_get_mpeg_version())
    {
        case MPEG_VER2d5:
            APP_DBG("MPEG_2_5");
            break;
        case MPEG_VER1:
            APP_DBG("MPEG_1");
            break;
        case MPEG_VER2:
            APP_DBG("MPEG_2");
            break;
        default:
            APP_DBG("MPEG_UNKNOWN");
            break;
    }
    APP_DBG("\n");
    APP_DBG("Id3Ver:%d\n", (int)audio_decoder_get_id3_version());

    APP_DBG("**************************\n");
    return ;
}

// 根据媒体参数，更新audiocore声道
//根据媒体文件参数，调整Dac频率和采样率
static void MediaPlayerUpdataDecoderSourceNum(void)
{
	AudioCoreSourceEnable(DecoderSourceNumGet());
#ifndef CFG_FUNC_MIXER_SRC_EN
	SongInfo* PlayingSongInfo = audio_decoder_get_song_info();
#ifdef CFG_RES_AUDIO_DACX_EN
	AudioDAC_SampleRateChange(ALL, PlayingSongInfo->sampling_rate);
#else
	AudioDAC_SampleRateChange(DAC0, PlayingSongInfo->sampling_rate);
#endif
	DBG("DAC Sample rate = %d\n", (int)AudioDAC_SampleRateGet(DAC0));
#endif
}

//仅打开文件
bool MediaPlayerOpenSongFile(void)
{
    memset(&gpMediaPlayer->PlayerFile, 0, sizeof(gpMediaPlayer->PlayerFile));
#ifdef CFG_FUNC_RECORD_UDISK_FIRST
	if(GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeCardPlayBack)
	{
		if(FR_OK != f_open_recfile_by_num(&gpMediaPlayer->PlayerFile, gpMediaPlayer->CurFileIndex))
		{
			DBG(("FileOpenByNum() error!\n"));
			return FALSE;
		}
	}
	else
#elif defined(CFG_FUNC_RECORD_FLASHFS)
	if(GetSystemMode() == AppModeFlashFsPlayBack)
	{
		DBG("open flashfs");
		if((gpMediaPlayer->FlashFsFile = Fopen(CFG_PARA_FLASHFS_FILE_NAME, "r")) == NULL)
		{
			return FALSE;
		}
	}
	else
#endif
	{
		DBG("CurFileIndex = %d\n", gpMediaPlayer->CurFileIndex);
		if(FR_OK != f_open_by_num(current_vol, &gpMediaPlayer->PlayerFolder, &gpMediaPlayer->PlayerFile, gpMediaPlayer->CurFileIndex, (char*)file_longname))
	    {
	        DBG(("FileOpenByNum() error!\n"));
	        return FALSE;
	    }
		if(file_longname[0] != 0)
		{
			DBG("Song file long Name: %s\n", file_longname);
			SongFileType = get_audio_type((TCHAR*)file_longname);
		}
		else
		{
			DBG("Song Name: %s\n", gpMediaPlayer->PlayerFile.fn);
			SongFileType = get_audio_type((TCHAR *)gpMediaPlayer->PlayerFile.fn);
		}

#ifdef CFG_FUNC_LRC_EN
		SearchAndOpenLrcFile();
#endif
		//DBG("Song type: %d\n", SongFileType);
	}

	return TRUE;
}

bool MediaPlayerDecoderInit(void)
{
	gpMediaPlayer->DecoderSync = DecoderStateDeinitializing;//设 初始化前状态
#ifdef CFG_FUNC_RECORD_SD_UDISK
	if(GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeCardPlayBack)
	{
		if(DecoderInit(&gpMediaPlayer->PlayerFile, (int32_t)IO_TYPE_FILE, (int32_t)FILE_TYPE_MP3) != RT_SUCCESS)
		{
			DBG("Decoder Init Err\n");
			return FALSE;
		}
	}
	else 
#elif defined(CFG_FUNC_RECORD_FLASHFS)
	if(GetSystemMode() == AppModeFlashFsPlayBack)
	{
 
		mv_mread_callback_set(FlashFsReadFile);

		if(DecoderInit(&gpMediaPlayer->FlashFsMemHandle, (uint32_t)IO_TYPE_MEMORY, (uint32_t)MP3_DECODER) != RT_SUCCESS)
		{
			DBG("Decoder Init Err\n");
			return FALSE;
		}

	}	
	else
#endif
	{
		if(DecoderInit(&gpMediaPlayer->PlayerFile, (int32_t)IO_TYPE_FILE, (int32_t)SongFileType) != RT_SUCCESS)
		{
			return FALSE;
		}
	    DBG("Open File ok! i=%d, ptr=%d, filesize=%uKB\n", (int)gpMediaPlayer->CurFileIndex, (int)f_tell(&gpMediaPlayer->PlayerFile), (uint16_t)(f_size(&gpMediaPlayer->PlayerFile)/1024));

	}
	gpMediaPlayer->DecoderSync = DecoderStateInitialized;//设 初始化状态
	return TRUE;
}

//打开文件和解码器设置
bool MediaPlayerFileDecoderInit(void)
{
	if(!MediaPlayerOpenSongFile())
	{
		return FALSE;
	}

	if(MediaPlayerDecoderInit() == FALSE)
	{
		return FALSE;
	}
#ifdef FUNC_MATCH_PLAYER_BP

	//DBG("999 CurPlayTime=%ld\n",gpMediaPlayer->CurPlayTime);
	if(gpMediaPlayer->CurPlayTime != 0)// && (GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeCardPlayBack))
	{
	
		//DBG("gggg\n");

		if(DecoderSeek(gpMediaPlayer->CurPlayTime) != TRUE) //bkd find break point play
		{
		
			//DBG("pppp\n");
			return FALSE;
		}
	}
#endif

	MediaPlayerSongInfoLog();

	MediaPlayerUpdataDecoderSourceNum();
    return TRUE;
}


//关闭播放文件
void MediaPlayerCloseSongFile(void)
{
#ifdef CFG_FUNC_RECORD_FLASHFS
    if(GetSystemMode() == AppModeFlashFsPlayBack && gpMediaPlayer->FlashFsFile)
    {
		DBG("Closefs");
    	Fclose(gpMediaPlayer->FlashFsFile);
		mv_mread_callback_unset();
		gpMediaPlayer->FlashFsFile = NULL;
    }
	else
#endif
	{
		f_close(&gpMediaPlayer->PlayerFile);
#ifdef CFG_FUNC_LRC_EN
		f_close(&gpMediaPlayer->LrcFile);

#endif
	}
}

//播放器 下一首 设定
void MediaPlayerNextSong(bool IsSongPlayEnd)
{
#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext	msgSend;
#endif
	APP_DBG("PlayCtrl:MediaPlayerNextSong\n");
//	gpMediaPlayer->CurPlayMode = PLAY_MODE_REPEAT_ALL;
	switch(gpMediaPlayer->CurPlayMode)
	{
		case PLAY_MODE_RANDOM_ALL:
			gpMediaPlayer->CurFileIndex = GetRandomNum((uint16_t)GetSysTick1MsCnt(), gpMediaPlayer->TotalFileSumInDisk);
			break;
		case PLAY_MODE_RANDOM_FOLDER:
			if(!gpMediaPlayer->PlayerFolder.FileNumLen
					|| gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex
					|| gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
			{//合法性检查
				DBG("No file in dir or file Index out dir!\n ");
			}
			gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex + GetRandomNum((uint16_t)GetSysTick1MsCnt(), gpMediaPlayer->PlayerFolder.FileNumLen) - 1;
			DBG("Cur File Num = %d in Folder %d\n", gpMediaPlayer->CurFileIndex - gpMediaPlayer->PlayerFolder.FirstFileIndex + 1 , gpMediaPlayer->PlayerFolder.FolderIndex);
			break;

		case PLAY_MODE_REPEAT_FOLDER:
			if(!gpMediaPlayer->PlayerFolder.FileNumLen
					|| gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex
					|| gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
			{//合法性检查
				DBG("No file in dir or file Index out dir!\n ");
			}
			gpMediaPlayer->CurFileIndex++;
			if(gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
			{
				gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex;
			}
			DBG("Cur File Num = %d in Folder %d\n", gpMediaPlayer->CurFileIndex - gpMediaPlayer->PlayerFolder.FirstFileIndex + 1 , gpMediaPlayer->PlayerFolder.FolderIndex);
			break;
		case PLAY_MODE_REPEAT_ONE:
			if(IsSongPlayEnd)
			{
				break;
			}
		case PLAY_MODE_REPEAT_ALL:
//		case PLAY_MODE_PREVIEW_PLAY:
			gpMediaPlayer->CurFileIndex++;
			if(gpMediaPlayer->CurFileIndex > gpMediaPlayer->TotalFileSumInDisk)
			{
				gpMediaPlayer->CurFileIndex = 1;
			}
			DBG("Cur File Num = %d\n", gpMediaPlayer->CurFileIndex);
			break;
			
		//case PLAY_MODE_REPEAT_ONE:
		default:
			break;
	}
	gpMediaPlayer->CurPlayTime = 0;
	gpMediaPlayer->SongSwitchFlag = 0;
	SetMediaPlayerState(PLAYER_STATE_PLAYING);

#ifdef CFG_FUNC_DISPLAY_EN
	msgSend.msgId = MSG_DISPLAY_SERVICE_FILE_NUM;
	MessageSend(GetDisplayMessageHandle(), &msgSend);
#endif
}

//播放器 上一首 设定
void MediaPlayerPreSong(void)
{
#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext	msgSend;
#endif
	APP_DBG("PlayCtrl:MediaPlayerPreSong\n");
	switch(gpMediaPlayer->CurPlayMode)
	{
		case PLAY_MODE_RANDOM_ALL:
			gpMediaPlayer->CurFileIndex = GetRandomNum((uint16_t)GetSysTick1MsCnt(), gpMediaPlayer->TotalFileSumInDisk);
			break;
		case PLAY_MODE_RANDOM_FOLDER:
			if(!gpMediaPlayer->PlayerFolder.FileNumLen
					|| gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex
					|| gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
			{//合法性检查
				DBG("No file in dir or file Index out dir!\n ");
			}
			gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex + GetRandomNum((uint16_t)GetSysTick1MsCnt(), gpMediaPlayer->PlayerFolder.FileNumLen) - 1;
			DBG("Cur File Num = %d in Folder %d\n", gpMediaPlayer->CurFileIndex - gpMediaPlayer->PlayerFolder.FirstFileIndex + 1 , gpMediaPlayer->PlayerFolder.FolderIndex);
			break;

		case PLAY_MODE_REPEAT_FOLDER:
			if(!gpMediaPlayer->PlayerFolder.FileNumLen
					|| gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex
					|| gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
			{//合法性检查
				DBG("No file in dir or file Index out dir!\n ");
			}
			gpMediaPlayer->CurFileIndex--;
			if(gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex)
			{
				gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex  + gpMediaPlayer->PlayerFolder.FileNumLen - 1;
			}
			DBG("Cur File Num = %d in Folder %d\n", gpMediaPlayer->CurFileIndex - gpMediaPlayer->PlayerFolder.FirstFileIndex + 1 , gpMediaPlayer->PlayerFolder.FolderIndex);
			break;

		case PLAY_MODE_REPEAT_ALL:
		case PLAY_MODE_REPEAT_ONE:
//		case PLAY_MODE_PREVIEW_PLAY:
			gpMediaPlayer->CurFileIndex--;
			if(gpMediaPlayer->CurFileIndex < 1)
			{
				gpMediaPlayer->CurFileIndex = gpMediaPlayer->TotalFileSumInDisk;
				DBG("gpMediaPlayer->CurFileIndex = %d\n", gpMediaPlayer->CurFileIndex);
			}			
			break;
		//case PLAY_MODE_REPEAT_ONE:
		default:
			break;
	}	
	gpMediaPlayer->CurPlayTime = 0;
	gpMediaPlayer->SongSwitchFlag = 1;
	SetMediaPlayerState(PLAYER_STATE_PLAYING);

#ifdef CFG_FUNC_DISPLAY_EN
	msgSend.msgId = MSG_DISPLAY_SERVICE_FILE_NUM;
	MessageSend(GetDisplayMessageHandle(), &msgSend);
#endif
}

//播放器 暂停和播放 设定
void MediaPlayerPlayPause(void)
{
	switch(GetMediaPlayerState())
	{
		case PLAYER_STATE_PLAYING:
			SetMediaPlayerState(PLAYER_STATE_PAUSE);
			if(!SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
			{
				DecoderPause();
			}
			break;
		case PLAYER_STATE_STOP:
		case PLAYER_STATE_IDLE:
			SetMediaPlayerState(PLAYER_STATE_PLAYING);
			if(!SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
			{
				MediaPlayerSongRefresh();
			}
			break;
		default:
			SetMediaPlayerState(PLAYER_STATE_PLAYING);
			if(!SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
			{
				DecoderResume();
			}
			break;
	}	
}

// 播放器 停止态设定
void MediaPlayerStop(void)
{
	APP_DBG("PlayCtrl:MediaPlayerStop\n");	
	gpMediaPlayer->CurPlayTime = 0;
	SetMediaPlayerState(PLAYER_STATE_IDLE);
	MediaPlayerDecoderStop();
}

//播放器状态 快进设定
void MediaPlayerFastForward(void)
{
	APP_DBG("PlayCtrl:MediaPlayerFastForward\n");
	if(SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
	{
		if(gpMediaPlayer->CurPlayTime >= FF_PLAY_TIME_MAX - FF_FB_STEP / 1000)
		{
			gpMediaPlayer->CurPlayTime = FF_PLAY_TIME_MAX;
		}
		else
		{
			gpMediaPlayer->CurPlayTime += FF_FB_STEP / 1000;
		}
	}
	else //直接调整解码器
	{
		if((GetMediaPlayerState() == PLAYER_STATE_IDLE)	// 停止状态下，禁止快进、快退
			|| (GetMediaPlayerState() == PLAYER_STATE_STOP)
			|| (GetMediaPlayerState() == PLAYER_STATE_PAUSE))
		{
			return;
		}
		if(GetDecoderState() == 0)//decoder end 应该禁止此操作
		{
			MediaPlayerCloseSongFile();
			
			MediaPlayerNextSong(TRUE);
			if(!MediaPlayerFileDecoderInit())
			{
				;//文件错误处理
			}
			//SetMediaPlayerState(PLAYER_STATE_PLAYING);

			return;
		}
		DecoderFF(FF_FB_STEP);
		SetMediaPlayerState(PLAYER_STATE_FF);
	}
}

//播放器状态 快退
void MediaPlayerFastBackward(void)
{
	APP_DBG("PlayCtrl:MediaPlayerFastBackward\n");

	if(SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
	{
		if(gpMediaPlayer->CurPlayTime == 0 || gpMediaPlayer->CurPlayTime == FB_PLAY_LAST)
		{
			gpMediaPlayer->CurPlayTime = FB_PLAY_LAST;
		}
		else if(gpMediaPlayer->CurPlayTime <= FF_FB_STEP / 1000)
		{
			gpMediaPlayer->CurPlayTime = 0;
		}
		else
		{
			gpMediaPlayer->CurPlayTime -= FF_FB_STEP / 1000;
		}
	}
	else //播控有效
	{
		if((GetMediaPlayerState() == PLAYER_STATE_IDLE)	// 停止状态，禁止快进、快退
			|| (GetMediaPlayerState() == PLAYER_STATE_STOP)
			|| (GetMediaPlayerState() == PLAYER_STATE_PAUSE))
		{
			return;
		}
		if(GetDecoderState() == 0)//解码器空闲，未初始化过，播放上一个文件。应该禁止此操作
		{
			MediaPlayerCloseSongFile();
			MediaPlayerPreSong();

			if(!MediaPlayerFileDecoderInit())
			{
				;//错误处理
			}
			//SetMediaPlayerState(PLAYER_STATE_PLAYING);
			return;
		}

		DecoderFB(FF_FB_STEP);//由解码器获取文件时长，判断seek和判断上下首
		SetMediaPlayerState(PLAYER_STATE_FB);
	}
}

//播放器状态 快进快退停止设定
void MediaPlayerFFFBEnd(void)
{
	APP_DBG("PlayCtrl:MediaPlayerFFFBEnd\n");
	if(SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))//解码器被占用
	{
		if(gpMediaPlayer->CurPlayTime == FB_PLAY_LAST)
		{
			MediaPlayerPreSong();
		}
		else if(gpMediaPlayer->CurPlayTime == FF_PLAY_TIME_MAX)
		{
			MediaPlayerNextSong(TRUE);
		}
		SetMediaPlayerState(PLAYER_STATE_PLAYING);
	}
	else //直接调整解码器
	{
		if((GetMediaPlayerState() == PLAYER_STATE_IDLE) // 停止状态，禁止快进、快退
		|| (GetMediaPlayerState() == PLAYER_STATE_STOP)
		|| (GetMediaPlayerState() == PLAYER_STATE_PAUSE))
		{
			return;
		}

		DecoderResume();
		SetMediaPlayerState(PLAYER_STATE_PLAYING);
	}
}

//指定歌曲文件夹播放准备，
//文件夹未打开时，Refresh 必须为TRUE。已打开的文件夹参数len不一定完备。
//不支持文件夹号直接打开
void PrepareFolderPlay(bool Refresh)
{
	if(gpMediaPlayer->CurFileIndex == 0 || gpMediaPlayer->CurFileIndex > gpMediaPlayer->TotalFileSumInDisk)
	{
		DBG("FileIndex error!\n");
		return ;
	}
	if(!Refresh	//不刷新时，文件夹必须已打开，但是len未必赋值。
		&& gpMediaPlayer->PlayerFolder.FirstFileIndex//容错处理
		&& gpMediaPlayer->PlayerFolder.FileNumLen != FILE_NUM_LEN_INVALID
		&& gpMediaPlayer->PlayerFolder.FileNumLen != 0
		&& gpMediaPlayer->CurFileIndex >= gpMediaPlayer->PlayerFolder.FirstFileIndex
		&& gpMediaPlayer->CurFileIndex <= gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1)
	{
			return ;//有效文件。
	}
	else
	{
		//非当前folder 必须重新打开文件号，get文件夹号
		if(FR_OK != f_open_by_num(current_vol, &gpMediaPlayer->PlayerFolder, &gpMediaPlayer->PlayerFile, gpMediaPlayer->CurFileIndex, (char*)file_longname))
		{
			DBG(("FileOpenByNum() error!\n"));
			return;
		}
	}

	//文件夹尚未扫描到结尾，或未设置
	if(gpMediaPlayer->PlayerFolder.FileNumLen == FILE_NUM_LEN_INVALID)
	{
		f_file_count_in_dir(&gpMediaPlayer->PlayerFolder);
	}//确认此文件夹 歌曲数量
	if(!gpMediaPlayer->PlayerFolder.FileNumLen
			|| gpMediaPlayer->CurFileIndex < gpMediaPlayer->PlayerFolder.FirstFileIndex
			|| gpMediaPlayer->CurFileIndex > gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen -1)
	{
		DBG("No file in folder or file Index out!\n ");
	}
}
//play模式切换,指定模式/或循环  ; refresh TRUE:刷新, FALSE:基于当前参数，不刷新，(主要是文件夹播放)
void MediaPlayerSwitchPlayMode(uint8_t PlayMode, bool Refresh)
{
	if(GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay)
	{
		APP_DBG("PlayCtrl:MediaPlayerSwitchPlayMode\n");
		//if(GetSystemMode() != AppModeUDiskPlayBack && GetSystemMode() != AppModeCardPlayBack)
		{
			if(PlayMode < PLAY_MODE_SUM)
			{
				gpMediaPlayer->CurPlayMode = PlayMode;
			}
			else
			{
				gpMediaPlayer->CurPlayMode++;
				gpMediaPlayer->CurPlayMode %= PLAY_MODE_SUM;
			}
			APP_DBG("mode = %d\n", gpMediaPlayer->CurPlayMode);
#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_PLAYER_INFO);
#endif
			if(gpMediaPlayer->CurPlayMode == PLAY_MODE_REPEAT_FOLDER || gpMediaPlayer->CurPlayMode == PLAY_MODE_RANDOM_FOLDER)//需要确认文件夹歌曲数量，以便显示。
			{//如果不是当前播放的文件夹，需要先停止播放，再找到文件夹。
				PrepareFolderPlay(Refresh);
			}
		}
		//从全盘切到当前目录播放时，如果要显示歌曲总数，需先打开文件夹(scan)。歌曲较多时存在读盘压力
	}
}


//播放器 上一文件夹 设定
void MediaPlayerPreFolder(void)
{
	if((GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay)
			&& (gpMediaPlayer->CurPlayMode == PLAY_MODE_REPEAT_FOLDER || gpMediaPlayer->CurPlayMode == PLAY_MODE_RANDOM_FOLDER))
	{
		APP_DBG("PlayCtrl:MediaPlayerPreFolder\n");

		if(!gpMediaPlayer->PlayerFolder.FileNumLen //当前文件夹内文件数量不能为0
				|| gpMediaPlayer->PlayerFolder.FileNumLen == gpMediaPlayer->TotalFileSumInDisk)//仅有当前一个有效文件夹
		{
			DBG("No file in last or Only one folder!\n ");
			return;
		}
		MediaPlayerDecoderStop();
		if(gpMediaPlayer->PlayerFolder.FirstFileIndex == 1)//第一个有效文件夹，切换最后一个文件夹，先播最后一个
		{
			gpMediaPlayer->CurFileIndex = gpMediaPlayer->TotalFileSumInDisk;
		}
		else //换前一个文件夹最后一首
		{
			gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex - 1;
		}
		PrepareFolderPlay(TRUE);
		gpMediaPlayer->CurPlayTime = 0;
		gpMediaPlayer->SongSwitchFlag = 1;
	}
}
//播放器 下一文件夹 设定
void MediaPlayerNextFolder(void)
{
	if((GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay)
			&& (gpMediaPlayer->CurPlayMode == PLAY_MODE_REPEAT_FOLDER || gpMediaPlayer->CurPlayMode == PLAY_MODE_RANDOM_FOLDER))
	{
		APP_DBG("PlayCtrl:MediaPlayerNextFolder\n");

		if(!gpMediaPlayer->PlayerFolder.FileNumLen //当前文件夹内文件数量不能为0
			|| gpMediaPlayer->PlayerFolder.FileNumLen == gpMediaPlayer->TotalFileSumInDisk)//仅有当前一个有效文件夹
		{
			DBG("No file in last or Only one folder!\n ");
			return;
		}
		MediaPlayerDecoderStop();
		if(gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen - 1 == gpMediaPlayer->TotalFileSumInDisk)//最后一个有效文件夹，切换第一个文件夹，先播第一首
		{
			gpMediaPlayer->CurFileIndex = 1;
		}
		else //换下一个文件夹第一
		{
			gpMediaPlayer->CurFileIndex = gpMediaPlayer->PlayerFolder.FirstFileIndex + gpMediaPlayer->PlayerFolder.FileNumLen;
		}
		PrepareFolderPlay(TRUE);
		gpMediaPlayer->CurPlayTime = 0;
		gpMediaPlayer->SongSwitchFlag = 0;
	}
}


void MediaPlayerPreviewPlay(void)
{
}

void MediaPlayerRepeatAB(void)
{
}

//播放实时信息更新 log
void MediaPlayerTimeUpdate(void)
{
	if(SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))
	{
		return;
	}
	if((GetMediaPlayerState() == PLAYER_STATE_PLAYING)
		|| (GetMediaPlayerState() == PLAYER_STATE_FF)
		|| (GetMediaPlayerState() == PLAYER_STATE_FB))
	{
		gpMediaPlayer->CurPlayTime = DecoderServicePlayTimeGet();//更新播放时间
		if(MediaPlayDevice() == DEV_ID_USB)
		{
			APP_DBG("USB ");
		}
		else if(MediaPlayDevice() == DEV_ID_SD)
		{
			APP_DBG("SD ");
		}
		else if(MediaPlayDevice() == DEV_ID_FLASHFS)
		{
			APP_DBG("Flash ");
		}
		if((gpMediaPlayer->CurPlayMode == PLAY_MODE_RANDOM_FOLDER)
		|| (gpMediaPlayer->CurPlayMode == PLAY_MODE_REPEAT_FOLDER))
		{
			APP_DBG("F(%d/%d, %d/%d) ",
					//(int)gpMediaPlayer->PlayerFolder.FolderIndex,
					(int)(f_dir_with_song_real_quantity_cur()),
					(int)gpMediaPlayer->ValidFolderSumInDisk,
					(int)(gpMediaPlayer->CurFileIndex - gpMediaPlayer->PlayerFolder.FirstFileIndex + 1),
					(int)gpMediaPlayer->PlayerFolder.FileNumLen);
		}
		else
		{
			APP_DBG("D(%d/%d, %d/%d) ",
					//(int)gpMediaPlayer->PlayerFolder.FolderIndex,
					(int)(f_dir_with_song_real_quantity_cur()),
					(int)gpMediaPlayer->ValidFolderSumInDisk,
					(int)gpMediaPlayer->CurFileIndex,
					(int)gpMediaPlayer->TotalFileSumInDisk);
		}
#ifdef	CFG_FUNC_RECORD_SD_UDISK
		if(GetSystemMode() == AppModeUDiskPlayBack || GetSystemMode() == AppModeCardPlayBack)
		{
			TCHAR NameStr[FILE_PATH_LEN];
			IntToStrMP3Name(NameStr, gpMediaPlayer->RecFileList[gpMediaPlayer->CurFileIndex - 1]);
			APP_DBG("%s, %02d:%02d ",
					NameStr,
					(int)(gpMediaPlayer->CurPlayTime ) / 60,
					(int)(gpMediaPlayer->CurPlayTime ) % 60);

		}
		else
#elif defined(CFG_FUNC_RECORD_FLASHFS)
		if(GetSystemMode() == AppModeFlashFsPlayBack)
		{

			APP_DBG("%s, %02d:%02d ",
					CFG_PARA_FLASHFS_FILE_NAME,
					(int)(gpMediaPlayer->CurPlayTime ) / 60,
					(int)(gpMediaPlayer->CurPlayTime ) % 60);

		}
		else
#endif
		{
			APP_DBG("%s, %02d:%02d ",
					gpMediaPlayer->PlayerFile.fn,
					(int)(gpMediaPlayer->CurPlayTime ) / 60,
					(int)(gpMediaPlayer->CurPlayTime ) % 60);
		}
		switch(gpMediaPlayer->CurPlayMode)
		{
			case PLAY_MODE_REPEAT_ONE:
				APP_DBG("RP_ONE ");
				break;
			case PLAY_MODE_REPEAT_ALL:
				APP_DBG("RP_ALL ");
				break;
			case PLAY_MODE_REPEAT_FOLDER:
				APP_DBG("RP_FOLDER ");
				break;
			case PLAY_MODE_RANDOM_FOLDER:
				APP_DBG("RDM_FOLDER ");
				break;
			case PLAY_MODE_RANDOM_ALL:
				APP_DBG("RDM_ALL ");
				break;
//			case PLAY_MODE_PREVIEW_PLAY:
//				APP_DBG("PREVIRW ");
//				break;
			default:
				break;
		}
		APP_DBG("\n");
#ifdef BP_PART_SAVE_TO_NVM
		if((GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay))
		{
			BackupInfoUpdata(BACKUP_PLAYER_INFO_2NVM);
		}
#endif
	}
#ifdef CFG_FUNC_LRC_EN
	if((GetSystemMode() == AppModeUDiskAudioPlay || GetSystemMode() == AppModeCardAudioPlay) && gpMediaPlayer->IsLrcRunning)
	{
		PlayerParseLrc();
	}
#endif // FUNC_LRC_EN
}

//刷新解码配置
bool MediaPlayerDecoderRefresh(uint32_t SeekTime)
{
	SoftFlagDeregister(SoftFlagDecoderMask & ~SoftFlagDecoderApp);//清理非App对解码器的登记
	if(!MediaPlayerDecoderInit())
	{
		return FALSE;
	}
	if(SeekTime != 0)
	{
		DecoderSeek(SeekTime);
	}
	MediaPlayerSongInfoLog();
	MediaPlayerUpdataDecoderSourceNum();
    return TRUE;
}

//播放器状态参数，打开文件，刷新播放器/解码器，解码器必须已停止
//依据播控分离，播放器重新获得解码服务
bool MediaPlayerSongRefresh(void)
{
	SoftFlagDeregister(SoftFlagDecoderMask & ~SoftFlagDecoderApp);//清理非App对解码器的登记
	if(gpMediaPlayer->CurPlayTime == FF_PLAY_TIME_MAX)
	{
		MediaPlayerNextSong(TRUE);
	}
	else if(gpMediaPlayer->CurPlayTime == FB_PLAY_LAST)
	{
		MediaPlayerPreSong();
	}
	MediaPlayerCloseSongFile();//存在重复关闭情况，需兼容。

	if(!MediaPlayerOpenSongFile())
	{
		return FALSE;//文件错误
	}
	//解码初始化失败(无效文件)，保持依据历史操作，切换上下首
	if(!MediaPlayerDecoderRefresh(gpMediaPlayer->CurPlayTime))
	{
		DBG("Refresh fail\n");
		if(gpMediaPlayer->SongSwitchFlag == TRUE)
		{
			MessageContext		msgSend;

			msgSend.msgId		= MSG_MEDIA_PLAY_PRE;
			MessageSend(GetAppMessageHandle(), &msgSend);
		}
		else
		{
			MessageContext		msgSend;

			msgSend.msgId		= MSG_MEDIA_PLAY_NEXT;
			MessageSend(GetAppMessageHandle(), &msgSend);
		}
		return FALSE;
	}
	switch(GetMediaPlayerState())
	{
		case PLAYER_STATE_PLAYING:
		case PLAYER_STATE_FF:
		case PLAYER_STATE_FB:
			SetMediaPlayerState(PLAYER_STATE_PLAYING);//
			DecoderPlay();
			break;
		case PLAYER_STATE_IDLE:
		case PLAYER_STATE_PAUSE:
			DBG("Pause\n");
			SetMediaPlayerState(PLAYER_STATE_PAUSE);
			DecoderPause();
			break;
		case PLAYER_STATE_STOP:
			DBG("STATE_STOP\n");
			break;
	}
	//此处是否判断快进超过歌曲长度，改为下一首，当前是解码器自动。
#ifdef CFG_FUNC_BREAKPOINT_EN
	if(GetSystemMode() == AppModeCardAudioPlay || GetSystemMode() == AppModeUDiskAudioPlay)
	{
		BackupInfoUpdata(BACKUP_SYS_INFO);
		BackupInfoUpdata(BACKUP_PLAYER_INFO);
	}
#endif
	return TRUE;
}

//播放器 停止解码服务
void MediaPlayerDecoderStop(void)
{//除App外，其他解码器没有复用，且解码器非停止态
	if(!SoftFlagGet(SoftFlagDecoderMask & ~SoftFlagDecoderApp))// && gpMediaPlayer->DecoderSync != DecoderStateStop)
	{
		if(gpMediaPlayer->DecoderSync != DecoderStateStop && gpMediaPlayer->DecoderSync != DecoderStateNone)
		{
			gpMediaPlayer->DecoderSync = DecoderStateStop;//设 app发起停止解码器 状态
			DecoderStop();
		}
	}
}

void MediaPlaySetDecoderState(DecoderState State)
{
	gpMediaPlayer->DecoderSync = State;
}

DecoderState MediaPlayGetDecoderState(void)
{
	return gpMediaPlayer->DecoderSync;
}

//播放器 获取其状态
uint8_t GetMediaPlayerState(void)
{
	if(gpMediaPlayer != NULL)
	{
		return gpMediaPlayer->CurPlayState;
	}

	return PLAYER_STATE_IDLE;
}

//播放器 状态设置
void SetMediaPlayerState(uint8_t state)
{
	if(gpMediaPlayer != NULL)
	{
		if(gpMediaPlayer->CurPlayState != state)
		{
			APP_DBG("PlayCtrl:SetMediaPlayerState %d\n", state);
			gpMediaPlayer->CurPlayState = state;
		}
	}
}

#ifdef CFG_FUNC_RECORD_SD_UDISK
//录音文件夹 检索排序
bool RecFilePlayerInit(void)
{
	FRESULT res;
	TCHAR PathStr[16];
	uint16_t j, k, i = 0;
	uint16_t	Backup;
		
	strcpy(PathStr, current_vol);
	strcat(PathStr, CFG_PARA_RECORDS_FOLDER);
	f_chdrive(current_vol);
	res = f_opendir(&gpMediaPlayer->Dir, PathStr);
	if(res != FR_OK)
	{
		DBG("f_opendir failed: %s ret:%d\n", PathStr, res);
		return FALSE;
	}
	
	if(gpMediaPlayer->RecFileList == NULL)
	{
		gpMediaPlayer->RecFileList = (uint16_t*)osPortMalloc(FILE_INDEX_MAX * FILE_NAME_VALUE_SIZE);//录音文件的最大数量。
		if(gpMediaPlayer->RecFileList == NULL)
		{
			return FALSE;
		}
		memset(gpMediaPlayer->RecFileList, 0, FILE_INDEX_MAX * FILE_NAME_VALUE_SIZE);
	}

	while(((res = f_readdir(&gpMediaPlayer->Dir, &gpMediaPlayer->FileInfo)) == FR_OK) && gpMediaPlayer->FileInfo.fname[0])// && i != FILE_INDEX_MAX)
	{
		if(gpMediaPlayer->FileInfo.fattrib & AM_ARC && gpMediaPlayer->FileInfo.fsize)//跳过非存档文件、长度为0的文件。
		{
			k = RecFileIndex(gpMediaPlayer->FileInfo.fname);//
			if(k)
			{
				for(j = 0; j <= i && j < FILE_INDEX_MAX; j++)//排序，检查,正常录音时顺序无需排序。
				{
					if(gpMediaPlayer->RecFileList[j] < k)
					{
						Backup = gpMediaPlayer->RecFileList[j];
						gpMediaPlayer->RecFileList[j] = k;
						k = Backup;
					}
				}
				i++;
			}
		}
	}
	gpMediaPlayer->TotalFileSumInDisk = i < FILE_INDEX_MAX ? i : FILE_INDEX_MAX;
	if(gpMediaPlayer->TotalFileSumInDisk == 0)
	{
		DBG("No Rec File\n");
		return FALSE;
	}
	DBG("RecList:%d\n", gpMediaPlayer->TotalFileSumInDisk);
	f_closedir(&gpMediaPlayer->Dir);
	return TRUE;
}

//依据编号打开 录音文件
FRESULT f_open_recfile_by_num(FIL *filehandle, UINT Index)
{
	FRESULT ret;
	TCHAR PathStr[25];
	
	if(Index > 255 || Index > gpMediaPlayer->TotalFileSumInDisk || Index == 0)
	{
		return FR_NO_FILE;
	}
	Index--;
	strcpy(PathStr, current_vol);
	strcat(PathStr, CFG_PARA_RECORDS_FOLDER);
	strcat(PathStr,"/");
	IntToStrMP3Name(PathStr + strlen(PathStr), gpMediaPlayer->RecFileList[Index]);
	DBG("%s", PathStr);

	ret = f_open(filehandle, PathStr, FA_READ);
	return ret;
}
#endif
#ifdef CFG_FUNC_RECORD_FLASHFS
uint32_t FlashFsReadFile(void *buffer, uint32_t length) // 作为flashfs 回放callback
{
	uint8_t ret = 0;
	if(length == 0)
	{
		return 0;
	}
    if(gpMediaPlayer->FlashFsFile)
    {
		 ret = Fread(buffer, length, 1, gpMediaPlayer->FlashFsFile);
    }
    if(ret)
    {
		//DBG("R:%d\n",length);
    	return length;
    }
    else
    {
		DecoderStop();	
    	DBG("Read fail\n");
    	return 0;
    }
}

#endif

#ifdef DEL_REC_FILE_EN
void DelRecFile(void)// bkd add 
{

	char FilePath[FILE_PATH_LEN];
	uint32_t i_count=0;

/*
#ifdef CFG_FUNC_RECORD_SD_UDISK
			f_close(&gpMediaPlayer->PlayerFile);
#elif defined(CFG_FUNC_RECORD_FLASHFS) 
			if(RecorderCt->RecordFile)
			{
				Fclose(RecorderCt->RecordFile);
				RecorderCt->RecordFile = NULL;
			}
#endif

*/

#ifdef CFG_FUNC_RECORD_SD_UDISK
	strcpy(FilePath, current_vol);
	strcat(FilePath, CFG_PARA_RECORDS_FOLDER);
	strcat(FilePath,"/");
	IntToStrMP3Name(FilePath + strlen(FilePath), gpMediaPlayer->RecFileList[gpMediaPlayer->CurFileIndex-1]);
	f_unlink(FilePath);
	
	if(gpMediaPlayer->CurFileIndex==gpMediaPlayer->TotalFileSumInDisk)gpMediaPlayer->CurFileIndex=1;
	else	
	{
	
	for(i_count=0;i_count<(gpMediaPlayer->TotalFileSumInDisk-gpMediaPlayer->CurFileIndex);i_count++)
		gpMediaPlayer->RecFileList[gpMediaPlayer->CurFileIndex-1+i_count]=gpMediaPlayer->RecFileList[gpMediaPlayer->CurFileIndex+i_count];
	
	}
	
	gpMediaPlayer->TotalFileSumInDisk--;
	
	DBG("Del %s.mp3\n",FilePath);
#elif defined(CFG_FUNC_RECORD_FLASHFS) 
	Remove(CFG_PARA_FLASHFS_FILE_NAME);
#endif			
	DBG("Media:MSG_DEVICE_SERVICE_REC_DEL\n");

}
#endif
#endif
