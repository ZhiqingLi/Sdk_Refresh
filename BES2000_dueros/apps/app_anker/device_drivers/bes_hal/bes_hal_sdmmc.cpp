#include "cmsis_os.h"
#include "hal_trace.h"
//#include "hal_sdmmc.h"
#include "bes_hal_sdmmc.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "app_utils.h"
#include "SDFileSystem.h"

static uint8_t isSDFileOpened = 0;
#if 0
const char sd_label[] = "sd";
SDFileSystem *sdfsa = NULL;//("sd");
#endif
enum HAL_CMU_FREQ_T curSysFreq = HAL_CMU_FREQ_32K;
int32_t bes_hal_sdmmc_init(void)
{
    int32_t ret = 0;
    if (isSDFileOpened == 0)
    {
        curSysFreq = hal_sysfreq_get();
        ret = app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_208M);
        #if 0
        if(!sdfsa )
        {
            sdfsa = new SDFileSystem(sd_label);
            TRACE("app_sd_handle_msg_new end\n");
        }
        #endif
        TRACE("\n%s: init succeed!\n",__func__);
        isSDFileOpened = 1;
    }  
    return ret;
}

int32_t bes_hal_sdmmc_deinit(void)
{
    int32_t ret = 0;
    if(curSysFreq < HAL_CMU_FREQ_QTY)
        ret = app_sysfreq_req(APP_SYSFREQ_USER_APP_0, (enum APP_SYSFREQ_FREQ_T)curSysFreq);
    isSDFileOpened = 0;
    TRACE("\n%s: deinit succeed!\n",__func__);
    return ret;
}

int32_t bes_hal_sdmmc_open_file(FIL* fp, uint8_t* path, uint8_t readFlag)
{
    FRESULT fpRes;

    uint8_t opsFlag = 0;
    if(readFlag)
        opsFlag = FA_OPEN_EXISTING | FA_READ;
    else
        opsFlag = FA_CREATE_ALWAYS | FA_WRITE;
    fpRes = f_open(fp,(const TCHAR*)path, (BYTE)opsFlag);

    if(fpRes != FR_OK)    
    {
        TRACE("\n%s: open %s failed!\n",__func__,path);
        return -1;
    }
    TRACE("\n%s: open %s succeed!\n",__func__,path);
    return 0;
}

int32_t bes_hal_sdmmc_close_file(FIL* fp)
{
    FRESULT fpRes;
    fpRes = f_close(fp);
    if(fpRes != FR_OK)    
    {
        TRACE("\n%s: close file failed!\n",__func__);
        return -1;
    }
    TRACE("\n%s: close file failed!\n",__func__);
    return 0;
}

int32_t bes_hal_sdmmc_write_file(FIL* fp, const void* buf, uint32_t size, uint32_t* plen)
{
    FRESULT fpRes;
    fpRes = f_write(fp, buf, (UINT)size, (UINT*)plen);
    if(fpRes != FR_OK)    
    {
        TRACE("\n%s: write file %d bytes failed, real write %d bytes!\n",__func__,size,*plen);
        return -1;
    }
    TRACE("\n%s: write file %d bytes failed, real write %d bytes!\n",__func__,size,*plen);
    return 0;
}

int32_t bes_hal_sdmmc_read_file(FIL* fp, void* buf, uint32_t size, uint32_t* plen)
{
    FRESULT fpRes;
    fpRes = f_read(fp, buf, (UINT)size, (UINT*)plen);
    if(fpRes != FR_OK)    
    {
        TRACE("\n%s: read file %d bytes failed, real read %d bytes!\n",__func__,size,*plen);
        return -1;
    }
    TRACE("\n%s: read file %d bytes failed, real read %d bytes!\n",__func__,size,*plen);
    return 0;
}