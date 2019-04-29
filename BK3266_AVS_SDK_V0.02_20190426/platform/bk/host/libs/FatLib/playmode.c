#include "includes.h"
#include "ff.h"
#include "diskio.h"		/* Declarations of low level disk I/O functions */

#if (CONFIG_APP_MP3PLAYER == 1)
#define MAX_DIR_SUPPORT 1024
#define	MAX_SONG_DIR	64       // 128
#define	MAX_DIR_LEVEL	6

FILE_INFO FileInfo;
CONST char mp3_ext[] = ".MP3";
CONST char wav_ext[] = ".WAV";
const char *MP3_Ext = &mp3_ext[0];
const char *WAV_Ext = &wav_ext[0];
FAT_DIR_INFO* file_queue = NULL;
uint16 mp3queuecount = 0; //有MP3文件的文件夹数目
uint16 mp3filecount = 0;//	MP3歌曲总的数目
FATFS *Fatfs_buf;
FIL *fsFD;
DIR_QUEUE *point_front;//队头指针
DIR_QUEUE *point_rear;//队尾指针
static uint16 rear_cnt = 0;
extern void mem_set (void* dst, int val, uint32 cnt);
extern FRESULT chk_mounted_con (FATFS *rfs, uint8 chk_wp);

int beken_strcmpi (const void* dst, const void* src, int cnt) {
	const char *d = (const char *)dst, *s = (const char *)src;
	int r = 0;
	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}

char * strrchr_con(char * str,char ch)
{
   char *p = (char *)str;
   while (*str) str++;
   while ((str-- != p) && (*str != ch));
   if (*str == ch) 
        return( (char *)str );
   return NULL;
}
static void Convert2UpperCase(char *str,UINT len)
{
	UINT i;	
	for(i=0;i<len;i++)
		((str[i]<='z')&&(str[i]>='a'))?(str[i]-=0x20):str[i];
}


FRESULT f_open_con (
	FIL *fp,			/* Pointer to the blank file object */
	PFILINFOADD finfo,
	BYTE mode			/* Access mode and file open mode flags */
)
{
			
	fp->obj.sclust = finfo->fcluster;/* File start cluster */
	fp->obj.objsize = finfo->fsize;	/* File size */
	
	fp->obj.fs = Fatfs_buf;
	fp->obj.id = Fatfs_buf->id;	/* Owner file system object of the file */
	fp->flag = mode;				/* File access mode */
	fp->err = 0;			/* Clear error flag */
	fp->sect = 0;			/* Invalidate current data sector */
	fp->fptr = 0;			/* Set file pointer top of the file */
	if(fp->obj.fs->fs_type == FS_EXFAT)
	{
		fp->obj.n_frag = 0;
		fp->obj.stat = finfo->ExNoChainFlag;
	}
	return FR_OK;
}

/*input:
i -- directory index
number -- music file idx in  directory i */
static FIL	*GetFile_From_NumInDir(WORD  i, WORD number)
{
	WORD  n = 0,brootdir = 0,music_count = 0;
	FRESULT res;
	FILINFOADD fno;
	DIR dj_con;
	char *Ext = NULL;
	FIL *fio = fsFD;
    	FAT_DIR_INFO* file = file_queue + i;
	
	dj_con.obj.fs = Fatfs_buf;
	brootdir = file->broot_dir;

	res=init_buf(Fatfs_buf);
	if(res != FR_OK)
		return NULL;
	
	if(brootdir)
		dj_con.obj.sclust = 0;	
	else
		dj_con.obj.sclust = file->first_cluster;

	music_count = 0;
	number += 1;
	res = Beken_dir_sdi(&dj_con, 0);
	if (res != FR_OK) 
	{
		fio =NULL;
		goto ret_free;
	}

	while(res == FR_OK) 
	{
		res = Beken_dir_read(&dj_con, 0);
		if (res != FR_OK) 
			break;
		/* Get the directory name and push it to the buffer */
		Beken_get_fileinfo(&dj_con, &fno);
		if((fno.fattrib & 0x20)!=0)/*对只读、隐藏、系统等文件的属性支持*/
		{
			for (n = 0; fno.fname[n]; n++);
			if((n > 4)&&(n <= FF_MAX_LFN))
			{	
				Convert2UpperCase(&fno.fname[n-3],3);
				Ext = &fno.fname[n-4];
				if ( (!beken_strcmpi((const void*)Ext, (const void*)MP3_Ext, 4) )||
					(!beken_strcmpi((const void*)Ext, (const void*)WAV_Ext, 4) ) )
					music_count++;
			}
		}
		if (music_count == number)
		{
			beken_mem_cpy(FileInfo.fname,fno.fname,12);
			beken_mem_cpy(FileInfo.extname,fno.fname+n-3,3);
			break;
		}
		else
			res = Beken_dir_next(&dj_con, 0);
	}
	
	FileInfo.fcluster = fno.fcluster;
	f_open_con(fio, &fno, FA_READ);
	
ret_free:
	uninit_buf(Fatfs_buf);
	return fio;
	
}


/*
dir_num:目录序号(从0开始)
song_idx:当前目录歌曲号
*/
FIL *Get_Cur_Dir_file(WORD  dir_num,WORD  song_idx)
{
	FIL *fhFile;
	FAT_DIR_INFO* file = NULL;

	if(dir_num >= mp3queuecount)
		dir_num = 0;

	file = file_queue + dir_num;	
	if(song_idx > file->music_total)
		song_idx = 0;
	//os_printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
	os_printf("total_dir:%d,cur_dir:%d,cur_dir_song_num:%d,cur_dir_song_index:%d\r\n",mp3queuecount,dir_num,file->music_total,song_idx);
	//os_printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
	fhFile = GetFile_From_NumInDir(dir_num, song_idx);
	return fhFile;
	
}


/*input:number -- 0~mp3filecount-1 */
//Playlist_GetSongFileInfo
FIL *Get_File_From_Number(WORD number)
{
	WORD i;
	FIL *fhFile;
    	FAT_DIR_INFO* file = NULL;

	for(i=0; i<MAX_SONG_DIR; i++)
	{
	    file = file_queue + i;
		if(number >= file->music_total)
		{
			number = number - file->music_total;
			continue;
		}
		else
			break;
	}
	if(i == MAX_SONG_DIR)
		return NULL;
	fhFile = GetFile_From_NumInDir(i, number);
	return fhFile;
}

static FRESULT  get_curdir_info(DIR_QUEUE *p_front)
{
	WORD n;
	
	DIR dj;
	FILINFOADD fno;
	unsigned short  cur_musicfile = 0;
	char *Ext = NULL; 
       FAT_DIR_INFO* file = NULL;
	FRESULT res = FR_OK;
	DIR_QUEUE* front;
	DIR_QUEUE* next;

	dj.obj.fs = Fatfs_buf;
	dj.obj.stat = p_front->ExNoChainFlag;
	
	res=init_buf(Fatfs_buf);
	if(res != FR_OK)
		return res;
	
	if(p_front->broot_dir)
		dj.obj.sclust = 0;//for Fat16	
	else
	{
		dj.obj.sclust = p_front ->cluster_number;
		dj.obj.stat = p_front ->ExNoChainFlag;
	}
	
	front = p_front;
	next = p_front->next;
	
	res = Beken_dir_sdi(&dj, 0);
	while(res == FR_OK) 
	{
		res = Beken_dir_read(&dj, 0);
		if (res != FR_OK) 
			break;
		
		/* Get the directory name and push it to the buffer */
		Beken_get_fileinfo(&dj, &fno);
		if(((fno.fattrib & 0x10) !=0) &&(front->dirlevel + 1 < MAX_DIR_LEVEL))
		{
			//printf("dir:%s\r\n",fno.fname);
			p_front->next = point_rear;
			p_front       = point_rear;
			point_rear->cluster_number = fno.fcluster;
			point_rear->dirlevel       = front->dirlevel + 1;
			point_rear->broot_dir = 0;
			point_rear->ExNoChainFlag = fno.ExNoChainFlag;
			point_rear->next           = next;
			if(++rear_cnt > MAX_DIR_SUPPORT-1)
			    break;
			point_rear++;
		}
		if((fno.fattrib & 0x20)!=0)
		{
			for (n = 0; fno.fname[n]; n++);
			if((n > 4)&&(n <= FF_MAX_LFN))
			{	
				Convert2UpperCase(&fno.fname[n-3],3);
				Ext = &fno.fname[n-4];
				
				if ( (!beken_strcmpi((const void*)Ext, (const void*)MP3_Ext, 4) )||
					(!beken_strcmpi((const void*)Ext, (const void*)WAV_Ext, 4) ) )
					cur_musicfile++;
			}
		}
		res = Beken_dir_next(&dj, 0);
	}

	if (cur_musicfile)
	{
	    	file = file_queue + mp3queuecount;
		file->first_cluster = front->cluster_number;
		file->music_total = cur_musicfile;
		file->broot_dir 	= front->broot_dir;
		file->ExNoChainFlag = front->ExNoChainFlag;
//		dirSongMax[mp3queuecount]=cur_musicfile;
        	//printf("+++++++++++dir[%d]=%d,file=0x%x,file->broot_dir=%d\r\n",mp3queuecount,file->music_total,file,file->broot_dir);
		mp3queuecount++;
		mp3filecount += cur_musicfile;
	}

	uninit_buf(Fatfs_buf);
	return FR_OK;
}


/*scan whole disk, get total music file number and store the DIRs info
which have music files
*/
static uint32 initfatsystem(void)
{

	UINT count = 0;
	DIR_QUEUE *dir_buf;

	rear_cnt = 0;
	CLEAR_WDT;
	dir_buf=(DIR_QUEUE*)ff_memalloc(MAX_DIR_SUPPORT*sizeof(DIR_QUEUE));//最大支持的文件夹
	if(!dir_buf)
	{
		return 1;
	}
		
	beken_mem_set(file_queue, 0, MAX_SONG_DIR* sizeof(FAT_DIR_INFO));
	mp3filecount = 0;
	mp3queuecount = 0;
	point_front = point_rear = dir_buf;
		
	point_front ->cluster_number = Fatfs_buf->dirbase;
	point_front ->dirlevel = 0;
	point_front ->ExNoChainFlag = 0;
	point_front->next           = NULL;
		
		
	if ((Fatfs_buf->fs_type == FS_FAT12)||(Fatfs_buf->fs_type == FS_FAT16))
		point_front ->broot_dir = 1;
	else
		point_front ->broot_dir = 0;
		
	point_rear++;
	rear_cnt++;
	while(1)
	{
		if ((mp3queuecount<MAX_SONG_DIR) && point_front && (count<MAX_DIR_SUPPORT))
		{
	        	if(rear_cnt > MAX_DIR_SUPPORT-1)
				break;
				
			get_curdir_info(point_front);
			count++;
			point_front = point_front->next;
		}
		else
			break;
	}
	os_printf("dir cnt=%d,  mp3 dir cnt=%d  mp3filecount=%d,rear_cnt =%d\r\n",count,mp3queuecount,mp3filecount,rear_cnt);
	
	ff_memfree(dir_buf);

	return 0;
}

/*Sd card & filesystem initialization*/
//uint32 SD_Fs_Init(void)
uint32 Media_Fs_Init(uint8 type)
{
    FRESULT res ;
    uint32 ret = 1;
    FileInfo.fat_ok_flag = 0;
    if(type == 0)
        res = f_mount(Fatfs_buf,"0:",1);
    else
        res = f_mount(Fatfs_buf,"1:",1);
    if( res == FR_OK)
    {
        if(initfatsystem() == FR_OK)
        {
            FileInfo.fat_ok_flag = 1;
            ret = 0;
        }
    }
    CLEAR_SLEEP_TICK;
    return ret;
}
uint8 get_fat_ok_flag(void)
{
    return FileInfo.fat_ok_flag;
}

uint16 get_musicfile_count(void)
{
	return mp3filecount;
}
uint16 get_curDirMusicfile_count(uint16 curDir)
{
    FAT_DIR_INFO* file = NULL;
    //os_printf("get_curDirMusicfile_count curDir=%d,\r\n",curDir);
    if(curDir >= mp3queuecount)
        curDir = 0;
    file = file_queue + curDir;
    os_printf("get_curDirMusicfile_file=0x%x,file->music_total=%d\r\n",file,file->music_total);
    return file->music_total;
}
FILE_INFO *get_file_info(void)
{
	return (&FileInfo); 
}

BYTE get_disk_type(void)
{
	return Fatfs_buf->pdrv;
}

//  modify thoes structs such as fsFD Fatfs_buf file_queue to malloc mode
//  can save ram space: 548+544+512=1604-3*4=1592 byte
FRESULT fat_malloc_files_buffer(void)
{ 
    fsFD = (FIL*)jmalloc(sizeof(FIL), 1);
    Fatfs_buf = (FATFS*)jmalloc(sizeof(FATFS), 0);
    file_queue = (FAT_DIR_INFO*)jmalloc(MAX_SONG_DIR*sizeof(FAT_DIR_INFO),0);
    
    if(!fsFD || !Fatfs_buf || !file_queue){
        goto failed;
    }

    return FR_OK;
    
failed:
    if(file_queue)
        jfree(file_queue);
    if(Fatfs_buf)
        jfree(Fatfs_buf);     
    if(fsFD)
        jfree(fsFD);   
    file_queue = NULL;
    Fatfs_buf = NULL;
    fsFD = NULL;
    return FR_INT_ERR;
}

FRESULT fat_free_files_buffer(void)
{ 
    if(file_queue)
        jfree(file_queue);
    if(Fatfs_buf)
        jfree(Fatfs_buf);     
    if(fsFD)
        jfree(fsFD);   
    
    file_queue = NULL;
    Fatfs_buf = NULL;
    fsFD = NULL;
    return FR_OK;
}
#endif
