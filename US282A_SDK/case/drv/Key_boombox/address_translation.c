/*******************************************************************************
 *                         
 * M4KºÍDSPµØÖ·×ª»»:BASE_CPU+0x00000 - BASE_CPU+0x40000
 *                  BASE_DSP+0x00000 - BASE_CPU+0x20000
 *******************************************************************************/
#include "address_translation.h"

static int32 dsp_to_mips_address(uint32* p_val)
{
    uint32* p_temp_val = NULL;
    uint32  temp_val = 0;
    int32   ret_val = 0;
    
    p_temp_val = p_val;
    temp_val = (*p_temp_val) & 0x1ffff;
    
    if (temp_val <= 0x20000)
    {
        *p_val = ((temp_val << 1) + 0x20000);
    }
    else
    {
        *p_val = ((temp_val << 1) - 0x20000);
    }
        
    return  ret_val;
}


static int32 mips_to_dsp_address(uint32* p_val)
{
    uint32* p_temp_val = NULL;
    uint32  temp_val = 0;
    int32   ret_val = 0;
    
    p_temp_val = p_val;
    temp_val = (*p_temp_val) & 0x3ffff;
    
    if (temp_val >= 0x20000)
    {
        *p_val = (((temp_val - 0x20000) >> 1));
    }
    else
    {
        *p_val = (((temp_val + 0x20000) >> 1));
    }
        
    return  ret_val;
}


int32 key_inner_address_translation(uint32 type, void *param_ptr, void *null3)
{
    int32   ret_val = 0;

    switch (type)
    {
        case MIPS_TO_DSP:

        ret_val = mips_to_dsp_address(param_ptr);
        
        break;
        
        case DSP_TO_MIPS:
            
        ret_val = dsp_to_mips_address(param_ptr);
        
        break;
        
        
        default:
            
        PRINT_INFO("!!!!!!:address translation type is error");
        ret_val = -1;
        
        break;
    }
    return ret_val;
}


