/*******************************************************************************
 *                         
 * WAVES PC工具和算法数据接口代码
 *
 *******************************************************************************/
#include <psp_includes.h>

const uint8 waves_dae_para_bin[WAVES_DAE_PARA_BIN_MAX_NUM][14] ={"wseparam.bin","wsepara1.bin","wsepara2.bin"};

//input参数表，通过按键音量来控制

const int16 w_input_table[32] ={-600, -525, -468, -425, -380, -368, -350, -324, \
                                -302, -287, -267, -252, -240, -230, -222, -211, \ 
                                -204, -199, -192, -185, -181, -173, -166, -158, \
                                -154, -147, -140, -132, -124, -117, -113, -77   
                               }; 

static uint32 dsp_to_mips_address(uint32 addr)
{
    //only for addr <= 0x30000
    return (((addr & 0x1ffff) << 1) + 0x9fc20000);
}

static uint32 mips_to_dsp_address(uint32 addr)
{
    //only for addr >= 0x9fc20000
    return ((((addr & 0x3ffff) - 0x20000) >> 1) + 0x20000);
}

static void read_waves_dae_para(uint8 bin_number)
{
    sd_handle p_waves_dae_para_bin_handle;
    
    int addr_mips = 0;

    if (bin_number >= WAVES_DAE_PARA_BIN_MAX_NUM)
    {    
        PRINT_INFO_INT("bin number error:",bin_number);
        bin_number = 0;//bin_number出错的话，强制加载第一个.bin音效文件
    }

    PRINT_DATA(waves_dae_para_bin[bin_number],14);
    p_waves_dae_para_bin_handle = sys_sd_fopen(waves_dae_para_bin[bin_number]);

    if (p_waves_dae_para_bin_handle == NULL)
    {       
        PRINT_INFO("open waves dae bin fail!!!");
        
        return;   
    }

    sys_sd_fread(p_waves_dae_para_bin_handle, (void*)WAVES_DAE_PARA_START_ADDR, WAVES_DAE_PARA_LENGTH);
    
    sys_sd_fclose(p_waves_dae_para_bin_handle);
}

static void write_waves_dae_para_vm(void)
{
    uint32 i = 0;
    uint8 block_num = 0;
    waves_dae_para_t* waves_dae_para = (waves_dae_para_t*)WAVES_DAE_PARA_START_ADDR;

    block_num = WAVES_DAE_PARA_LENGTH/VM_DATA_LENGTH + 1;

    for (i = 0; i < block_num; i++)
    {
        sys_vm_write(waves_dae_para + i, (VM_WAVES_AL_DATA1 + (i<<16)), VM_DATA_LENGTH);
    }   
}


static void read_waves_dae_para_vm(void)
{
    uint32 i = 0;
    uint8 block_num = 0;
    waves_dae_para_t* waves_dae_para = (waves_dae_para_t*)WAVES_DAE_PARA_START_ADDR;

    block_num = WAVES_DAE_PARA_LENGTH/VM_DATA_LENGTH + 1;

    for (i = 0; i < block_num; i++)
    {
        sys_vm_read(waves_dae_para + i, (VM_WAVES_AL_DATA1 + (i<<16)), VM_DATA_LENGTH);
    }   
}


int32 key_inner_set_waves_effect_param(uint32 set_type, void *param_ptr, void *null3)
{
    coeff_t *p_coeff = (coeff_t*)WAVES_ASET_COEFF_ADDR;
    coeff_property_t* p_coeff_property;

    uint32 cur_package_start_addr = 0;
    uint32 cur_package_length = 0;

    uint8  cur_vol = NULL;
    uint32 w_input_addr = NULL;
    int16  w_db = 0;
    int32  w_input_val  = 0;
    uint32 coff_storagef_start_dsp_addr;
    uint8  waves_dae_para_bin_number = 0;
     
    switch (set_type)
    {
        case SET_WAVES_COEFF_PROPERTY:
        p_coeff_property = (coeff_property_t*)param_ptr; 
        
        p_coeff->parameter_address = (int)p_coeff_property->memory_addr;    
        p_coeff->parameter_length  = (int)p_coeff_property->length;
             
        break;
        
        case SET_WAVES_EFFECT_PARAM:
            
        cur_package_start_addr = dsp_to_mips_address((uint32)p_coeff->parameter_address); 
        cur_package_length = (uint32)p_coeff->parameter_length; 
            
        libc_memcpy((void*)(cur_package_start_addr),(uint8*)param_ptr,cur_package_length);

        p_coeff->DAE_change_flag = 1;      
        break;

        case WAVES_DAE_WRITE_VRAM:
           
        write_waves_dae_para_vm();
        
        break;

        case WAVES_DAE_READ_VRAM:
           
        read_waves_dae_para_vm();
        
        break;

        
        case SET_WAVES_PARAM_FROM_SDFS:
           
        waves_dae_para_bin_number = (((*(uint8*)param_ptr)&BIN_NUMBER_MASK)>>BIN_NUMBER_SHIFT);
        PRINT_INFO_INT("waves_dae_para_bin_number:",waves_dae_para_bin_number);
        
        read_waves_dae_para(waves_dae_para_bin_number);
        
        break;

        case SET_WAVES_INPUT_PARAM:
        cur_vol = (uint8)(*(uint32*)param_ptr);
      
        w_db = w_input_table[cur_vol];
        w_input_val = exp_fast(w_db);
        PRINT_INFO_INT("w_input_val",w_input_val);
                      
        w_input_addr = WAVES_INPUT_PARA_START_ADDR;
        
        *((int32*)w_input_addr) = w_input_val;

        break;
        
        default:
        break;
    }
    return 0;
}


