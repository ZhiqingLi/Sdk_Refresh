/*******************************************************************************
 *                              US212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_main.c
 * \brief    manager的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"

void manager_mem_manager(uint32 app_id, uint32 exec_ap)
{
    if(app_id == APP_ID_BTPLAYEG)
    {
        if(exec_ap == TRUE)
        {
            if(g_app_info_state.stub_tools_type != STUB_PC_TOOL_ATT_MODE)
            {
                if(support_dev_num > 1)
                {
                    //避免使用send buffer空间 0x30800-0x34000
                    sys_enable_mem_use(DSP_CACHE_MEM_ADDR_L, DSP_CACHE_MEM_ADDR_L + DSP_CACHE_MEM_SIZE_L);
                
                    sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H);
                }
                else
                {
                    //需要注意这里使用了send bufer的空间
                    sys_enable_mem_use(DSP_CACHE_MEM_ADDR, DSP_CACHE_MEM_ADDR + DSP_CACHE_MEM_SIZE);
                
                    sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H); 
                }
            }
            else
            {
                //ATT工具使用了dsp的后16k空间
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR, DSP_CACHE_MEM_ADDR + DSP_CACHE_MEM_SIZE);
            
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H2, DSP_CACHE_MEM_ADDR_H2 + DSP_CACHE_MEM_SIZE_H2);
            }
        }
        else
        {
            sys_clear_temp_pool();
        }
    }
    else if(app_id == APP_ID_BTCALLEG)
    {
        if(exec_ap == TRUE)
        {
            if(g_app_info_state.stub_tools_type != STUB_PC_TOOL_ATT_MODE)
            {
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR2, DSP_CACHE_MEM_ADDR2 + DSP_CACHE_MEM_SIZE2);
                
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H);   
            }
            else
            {
                //ATT工具使用了dsp的后16k空间
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR2, DSP_CACHE_MEM_ADDR2 + DSP_CACHE_MEM_SIZE2);
            
                sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H2, DSP_CACHE_MEM_ADDR_H2 + DSP_CACHE_MEM_SIZE_H2);
            }

            sys_enable_mem_use(SBC_INPUT_BUFFER_LOW_ADDR, SBC_INPUT_BUFFER_LOW_ADDR + SBC_INPUT_BUFFER_LOW_SIZE);
        }
        else
        {
            sys_clear_temp_pool();
        }        
    }
    else if(app_id == APP_ID_MENGINE)
    {
        if(exec_ap == TRUE)
        {
            sys_set_mem_use_info(MEM_USE_MUSICPLAY);

            sys_enable_mem_use(SBC_INPUT_BUFFER_LOW_ADDR, SBC_INPUT_BUFFER_LOW_ADDR + SBC_INPUT_BUFFER_LOW_SIZE);
        }
        else
        {
            sys_set_mem_use_info(MEM_USE_NORMAL);
            sys_clear_temp_pool();    
        }      
    }
    else if(app_id == APP_ID_UENGINE)
    {
        if(exec_ap == TRUE)
        {
            sys_enable_mem_use(DSP_CACHE_MEM_ADDR2, DSP_CACHE_MEM_ADDR2 + DSP_CACHE_MEM_SIZE2);
            
            sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H);  

            sys_enable_mem_use(SBC_INPUT_BUFFER_LOW_ADDR, SBC_INPUT_BUFFER_LOW_ADDR + SBC_INPUT_BUFFER_LOW_SIZE);
        }
        else
        {
            sys_clear_temp_pool();
        }         
    }
    else if(app_id == APP_ID_LINEIN_EG)
    {
        if(exec_ap == TRUE)
        {   
#ifdef __SUPPORT_LINEIN_TWS  
            sys_disable_mem_use(0x3d000, 0x3e000);
            
            sys_disable_mem_use(0x3f800, 0x40000);

            sys_enable_mem_use(DSP_CACHE_MEM_ADDR, DSP_CACHE_MEM_ADDR + DSP_CACHE_MEM_SIZE);

            sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H);
            
#else       
            sys_enable_mem_use(DSP_CACHE_MEM_ADDR2, DSP_CACHE_MEM_ADDR2 + DSP_CACHE_MEM_SIZE2);

            sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H + DSP_CACHE_MEM_SIZE_H); 

            sys_enable_mem_use(SBC_INPUT_BUFFER_LOW_ADDR, SBC_INPUT_BUFFER_LOW_ADDR + SBC_INPUT_BUFFER_LOW_SIZE);
#endif        

        }
        else
        {
#ifdef __SUPPORT_LINEIN_TWS  
            sys_enable_mem_use(0x3d000, 0x3e000);
            
            sys_enable_mem_use(0x3f800, 0x40000);
#endif            
            sys_clear_temp_pool();            
        }
    }
    else if(app_id == APP_ID_FMENGINE)
    {
        if(exec_ap == TRUE)
        {
            sys_enable_mem_use(DSP_CACHE_MEM_ADDR2, DSP_CACHE_MEM_ADDR2 + DSP_CACHE_MEM_SIZE2);
            
            sys_enable_mem_use(DSP_CACHE_MEM_ADDR_H, DSP_CACHE_MEM_ADDR_H +DSP_CACHE_MEM_SIZE_H);

            sys_enable_mem_use(SBC_INPUT_BUFFER_LOW_ADDR, SBC_INPUT_BUFFER_LOW_ADDR + SBC_INPUT_BUFFER_LOW_SIZE);
        }   
        else
        {
            sys_clear_temp_pool();
        }
    }
    else if(app_id == APP_ID_UDISK)
    {
        if(exec_ap == TRUE)
        {
            sys_enable_mem_use(0x27000, 0x29000);    
        }
        else
        {
            sys_disable_mem_use(0x27000, 0x29000);
        }
    }
    else if(app_id == APP_ID_BTSTACK)
    {
        if(exec_ap == TRUE)
        {
            sys_disable_mem_use(0x27000, 0x27e00);    
        } 
        else
        {
            sys_enable_mem_use(0x27000, 0x27e00);    
        }         
    }
    else if(app_id == APP_ID_CONFIG)
    {
        if(exec_ap == FALSE)
        {   
            //在退出config应用场景下，不使用0x29000-0x3c000这一段临时缓冲区
            //所以在设置的时候，需要把这部分缓冲区的缓冲池删掉
            sys_clear_temp_pool();    
        }
    }
    else
    {
        ;//nothing for QAC
    }
}
