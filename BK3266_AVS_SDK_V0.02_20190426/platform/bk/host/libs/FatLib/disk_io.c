//#include "Driver_sdcard.h"
#include "driver_beken_includes.h"
#include "diskio.h"
#if (CONFIG_APP_MP3PLAYER == 1)
enum
{
	DISK_TYPE_SD =0,
	DISK_TYPE_UDISK
};
static uint8 cur_disk_type = DISK_TYPE_SD;

DSTATUS disk_initialize (uint8 pdrv)
{
    int cnt = 5;
    cur_disk_type = pdrv;
    if(pdrv == DISK_TYPE_SD)
    {
    #ifdef CONFIG_APP_SDCARD
        while(cnt--)
        {
            if(SD_init() == SD_ERR_NONE)
                return RES_OK;     
            os_printf("SD init retry cnt = %d\r\n",cnt);
            Delay(50);
        }
    #endif
    }
    else
    {
    #ifdef CONFIG_APP_USB
        if(udisk_init()== USB_RET_OK)
            return RES_OK;
    #endif
    }
    return STA_NOINIT;
}

DSTATUS disk_status (BYTE pdrv)
{
	//test 
	return RES_OK;
}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	uint8* buff, 
	DWORD sector, 
	UINT count 
)
{
    int res;
    uint32 read_loop = 3;


    if(!Media_is_online())				
        return RES_NOMEDIA;
	
    if( cur_disk_type == DISK_TYPE_SD)
    {
    #ifdef CONFIG_APP_SDCARD
        while(read_loop--)
        {
            res = sd_rd_blk_sync(sector,count,buff);

            if(res == SD_ERR_NONE)
            {
                return RES_OK;	
            }
            else
            {
                Delay(1);
                continue;
            }
        }
    #endif
        return RES_ERROR;		
    }
    else
    {
    #ifdef CONFIG_APP_USB
        if(udisk_rd_blk_sync(sector,count,buff)!= USB_RET_OK)
    #endif
            return RES_ERROR;
    }
    return RES_OK;
}

DRESULT disk_unmount(uint8 pdrv)
{
    if(pdrv == DISK_TYPE_SD)
    {
    #ifdef CONFIG_APP_SDCARD
        sdcard_uninit();
    #endif
    }
    else
    {
    #ifdef CONFIG_APP_USB
        udisk_uninit();
    #endif
    }
    return RES_OK;
}

uint32 get_disk_size(void)
{
    if(cur_disk_type == DISK_TYPE_SD)
    {
    #ifdef CONFIG_APP_SDCARD
        return sdcard_get_size();
    #else
        return 0;
    #endif
    }
    else
    {
    #ifdef CONFIG_APP_USB
        return udisk_get_size();
    #else
        return 0;
    #endif
    }
}

uint8 Media_is_online(void)
{
    if(cur_disk_type == DISK_TYPE_SD)
    {
    #ifdef CONFIG_APP_SDCARD
        return sd_is_attached();
    #else
        return 0;
    #endif
    }
    else
    {
    #ifdef CONFIG_APP_USB
        return udisk_is_attached();
    #else
        return 0;
    #endif
    }
}
uint8 get_cur_media_type(void)
{
    return cur_disk_type;
}
#endif