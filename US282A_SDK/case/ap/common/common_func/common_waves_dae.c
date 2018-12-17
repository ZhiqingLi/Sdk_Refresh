#include "psp_includes.h"
#include "common_func.h"

//动态切换waves音效参数
void dynamic_switch_waves_bin_para(uint8 bin_number)
{
    if (bin_number >= WAVES_DAE_PARA_BIN_MAX_NUM)
    {    
        PRINT_INFO_INT("set waves bin number error:",bin_number);
        return;
    }
    
    g_app_info_state_all.bin_number = bin_number;
    g_app_info_state_all.waves_dae_para_update_flag = 1;      
}


