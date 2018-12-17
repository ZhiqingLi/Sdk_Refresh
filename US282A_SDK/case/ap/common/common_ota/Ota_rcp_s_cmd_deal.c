/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：APK/APP OTA接口，包括获取版本号，下载固件，APK查询信息
         重启升级。
 * 作者：lizijian
 ********************************************************************************/
#include "Common_ota.h"
#include "vm_def.h"

/*APK向小机发送握手数据*/
app_result_e ota_receive_connect_data(uint32 data1, uint32 data2, void *data, uint16 data_len)
{    
    uint32 i;
    uint16 *buf = data ;
    uint8 *buf8 = data ; 
    uint16 checksum = *buf ;
    uint8 check_len;
    int ret;
    connect_message_t* connect_msg;
    ota_write_info_t *write_info;
    ota_write_info_t *write_info_ret;
    uint8 *temp_data_buffer;
    //ota_write_info_t *write_info_ret = g_write_info_buf;
    //libc_print("APK request to connect!!",0,0);

    temp_data_buffer = sys_malloc_large_data(512);

    write_info_ret = (ota_write_info_t *)temp_data_buffer;
    
    buf += 2;    

    ret = sys_vm_read(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));
    if ((ret != 0) || (g_ota_restore_data.dowmload_statue == OTA_DOWNLOAD_STATE_CLEAR))//没读到VM_OTA数据/清除数据
    {
        g_ota_restore_data.dowmload_statue = OTA_DOWNLOAD_STATE_NULL; 
    }
    connect_msg = (connect_message_t*)buf;
    if(g_ota_restore_data.dowmload_statue==OTA_DOWNLOAD_STATE_NULL)  //开始升级第一次握手
    {
        g_OTA_var->g_cru_pack_count = 0;
        g_OTA_var->g_cur_rec_state.cru_pack_count = 0;
        g_OTA_var->g_cur_rec_state.rec_data_state = 0;
       

        check_len = 8 + (connect_msg->wirte_info.part_total*sizeof(ota_write_item_t)+2)/2;
        //libc_print("check_len",check_len,2);

     
        if(CheckSum16(buf,check_len)==checksum)//检验成功
        {    
            //注意这里操作会引起write_info和write_info_ret指针的变化!!!
            write_info=(ota_write_info_t *)(buf + 8);//握手数据从16字节开始是信息分区表的内容
            //记录握手信息
            sys_vm_write(write_info,VM_OTA_CONNECT,2 + sizeof(ota_write_item_t) * write_info->part_total);
            //PRINTD_DATA(write_info,(check_len-4)*4);
           
            write_info_ret = base_ota_part_write_query(write_info); 
            if(write_info_ret == NULL)
            {
                g_OTA_var->g_connect_repy.connect_state = OTA_FW_DATA_TOO_BIG; //固件太大，不能下载
                libc_print("connect fail (FW too Big!)",0,0);

            }
            else
            {
                sys_vm_write(write_info_ret,VM_OTA_PART_INFO,2 + sizeof(ota_write_item_t) * write_info_ret->part_total);
                libc_memcpy(&g_OTA_var->g_cur_rec_state, buf, 8); 
                g_OTA_var->g_connect_repy.connect_state= OTA_CONNECT_SUCCESS; // 握手成功
                libc_memcpy(&g_ota_restore_data.cru_updata, &g_OTA_var->g_cur_rec_state.cru_updata, 8);
                g_ota_restore_data.pack_count = 0;
                g_ota_restore_data.part_id = connect_msg->wirte_info.part_info[0].part_id;
                //g_ota_restore_data.random_code = connect_msg->random_upg_val;   
                g_ota_restore_data.dowmload_statue = OTA_DOWNLOAD_STATE_START;  
                g_OTA_var->g_send_erro_count = 0;

                g_OTA_var->g_cur_rec_state.part_id = connect_msg->wirte_info.part_info[0].part_id;
                g_OTA_var->g_backup_part_addr = write_info_ret->part_info[0].part_offset;
                for(i = 0; i < write_info_ret->part_total; i++)
                {
                    if(g_OTA_var->g_cur_rec_state.part_id <16)
                    {   //判断OTA是否含有系统分区
                        g_ota_restore_data.ud_att = 1;
                        break;
                    }
                }
                  
                
                 /*for(i = 0; i < write_info_ret->part_total; i++)
                 {   //擦除各分区即将使用的块
                     base_ota_prepare(write_info_ret->part_info[i].part_offset,write_info_ret->part_info[i].part_size);
                 }*/

                 
                sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));
                 
                libc_print("connect success!!",0,0);
            }  
            
        }
        else
        {
            g_OTA_var->g_connect_repy.connect_state = OTA_CONNECT_DATA_ERROR; //握手数据有误
            libc_print("connect message ERRO!!",0,0);
        }
    }    
    else 
    {
        libc_memcpy(&g_OTA_var->g_cur_rec_state, buf, 8); 
        sys_vm_read(write_info_ret,VM_OTA_CONNECT,1);//读变长数组的大小
        sys_vm_read(write_info_ret,VM_OTA_CONNECT,2 + sizeof(ota_write_item_t) * write_info_ret->part_total);

        if( 0 != libc_memcmp(write_info_ret,RCP_BUFF_ADDR+20+16, 2 + sizeof(ota_write_item_t) \
            * write_info_ret->part_total))
        {
            g_OTA_var->g_connect_repy.connect_state = OTA_FW_DIFFERENT; //2次升级固件不是同一个
            libc_print("connect message is different",0,0);
            sys_free_large_data(temp_data_buffer);
            return RESULT_NULL;
        }
        
        //普通切应用或者前一个分区已经发完准备发送第二个分区
        if(g_OTA_var->g_connect_repy.connect_state == OTA_CONNECT_SUCCESS)  
        {       
            
            if(connect_msg->part_id==g_OTA_var->g_cur_rec_state.part_id)//切应用
            {
                change_app:
                libc_print("change applicant!!",0,0);
                sys_free_large_data(temp_data_buffer);
                return RESULT_NULL;
            }
            else//换分区
            {                     
                if(connect_msg->part_id==0xff)
                {
                    goto change_app;
                }
                g_OTA_var->g_cur_rec_state.part_id = connect_msg->part_id;
                
                
                sys_vm_read(write_info_ret,VM_OTA_PART_INFO,1);
                sys_vm_read(write_info_ret,VM_OTA_PART_INFO,2 + sizeof(ota_write_item_t) * write_info_ret->part_total);
               
                libc_print("g_cur_rec_state.part_id",g_OTA_var->g_cur_rec_state.part_id,2);

                g_OTA_var->g_cru_pack_count = 0;              
                g_OTA_var->g_cur_rec_state.cru_pack_count = 0;
                g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_FAIL;
    
                for(i = 0; i < write_info_ret->part_total; i++)
                {
                    if(g_OTA_var->g_cur_rec_state.part_id == write_info_ret->part_info[i].part_id)
                    {
                        g_OTA_var->g_backup_part_addr = write_info_ret->part_info[i].part_offset;
                        break;
                    }
                }
                sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));
                libc_print("g_backup_part_addr",g_OTA_var->g_backup_part_addr,2);
            }
                
        }
        else  //断电续传
        {
            sys_vm_read(write_info_ret,VM_OTA_PART_INFO,1);
            sys_vm_read(write_info_ret,VM_OTA_PART_INFO,2 + sizeof(ota_write_item_t) * write_info_ret->part_total);
            PRINTD_DATA(write_info_ret,2 + sizeof(ota_write_item_t) * connect_msg->wirte_info.part_total);

            for(i = 0; i < write_info_ret->part_total; i++)
            {
                if(g_ota_restore_data.part_id == write_info_ret->part_info[i].part_id)
                {
                    g_OTA_var->g_backup_part_addr = write_info_ret->part_info[i].part_offset;
                    break;
                }
            }

            libc_print("g_backup_part_addr ",g_OTA_var->g_backup_part_addr,2);
            g_OTA_var->g_connect_repy.connect_state = OTA_CONNECT_SUCCESS; // 断点续
            g_OTA_var->g_cru_pack_count = g_ota_restore_data.pack_count;
            g_OTA_var->g_cur_rec_state.part_id = g_ota_restore_data.part_id;
            g_OTA_var->g_cur_rec_state.cru_pack_count = g_ota_restore_data.pack_count;
            g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_FAIL; 
            g_OTA_var->g_send_erro_count = 0;
                
            g_OTA_var->g_reply_bkinfo.connect_state = OTA_BK_CONTINUE;
            g_OTA_var->g_reply_bkinfo.cru_pack_count = g_OTA_var->g_cru_pack_count;
            g_OTA_var->g_reply_bkinfo.part_id = g_OTA_var->g_cur_rec_state.part_id;
            //g_OTA_var->g_reply_bkinfo.random_upg_key = g_ota_restore_data.random_code;
            g_OTA_var->g_reply_bkinfo.rec_data_state = RECEIVE_DARA_FAIL;
            libc_print("g_cur_rec_state.part_id",g_OTA_var->g_cur_rec_state.part_id,2);
            //libc_print("restore breakpoint!!",0,0);
           
        } 
        if(g_ota_restore_data.dowmload_statue==OTA_DOWNLOAD_STATE_END)
        {
            g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_COMLETE; 
            libc_print("spe!!!~~~~",0,0);
        }   
            
    }    
    sys_free_large_data(temp_data_buffer);
    return RESULT_NULL;
}



/*APK向小机发送固件数据*/
app_result_e ota_receive_data(uint32 data1, uint32 data2, void *data, uint16 data_len)
{
    uint16 *buf =data ;
    uint16 checksum;
    uint16 apk_send_pak_num;
    int32 ret_val;
    
    checksum = *buf;
    apk_send_pak_num = *(buf+1);
    buf += 1;

    if(g_ota_restore_data.dowmload_statue == OTA_DOWNLOAD_STATE_CLEAR)
    {
        //手机发送清除命令后小机又收到fw包，如果不返回导致OTA状态错误
        libc_print("OTA state err!!",0,0);
        return RESULT_NULL;
    }
    
    if(CheckSum16(buf,514>>1)==checksum)//检验成功
    {    
        if(apk_send_pak_num!=g_OTA_var->g_cru_pack_count)
        {
            libc_print("pakg is not need!",g_OTA_var->g_cru_pack_count,2);
            g_OTA_var->g_send_erro_count++;
            return RESULT_NULL;
        }

        ret_val = (int32)fw_download_new();
        
    }
    else
    {    
        //libc_print(" FW data pack crc fail!",0,0);
        g_OTA_var->g_cur_rec_state.cru_pack_count = g_OTA_var->g_cru_pack_count;
        g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_FAIL;

    }

    return RESULT_NULL;

}

/*APK向小机发送VRAM数据*/
app_result_e ota_receive_vram_data(uint32 data1, uint32 data2, void *data, uint16 data_len)
{
    
 

    //libc_print("receive_vram_data",0,0);
    //sys_vm_write(&g_test_vram_led,vm_id,sizeof(test_change_vram_t));   //写VM  
    
    //discrete_led_set(led_sty->led_id, led_sty->led_mode, led_sty->led_duty);


    return RESULT_NULL;

}

uint32 fw_download_new(void)
{
    int32 ret_val;
    ota_write_param_t write_param;
    
    write_param.data_buffer = (uint8 *)(RCP_BUFF_ADDR+0x14);  //记录接收数据的地址
    write_param.data_len = 1;
    write_param.part_id = g_OTA_var->g_cur_rec_state.part_id;
    write_param.part_base_offset = g_OTA_var->g_backup_part_addr;
    write_param.part_write_offset = g_OTA_var->g_cru_pack_count;

    /*if(g_OTA_var->g_cru_pack_count%128 == 0)
    {
       if(g_tts_play_info.status == TTS_STATUS_PLAYING||g_tts_play_info.status == TTS_STATUS_READY)
       {     
          libc_print("pause to receive data for TTS",0,0);
          return 2;
       }
    }*/
 
    ret_val = base_ota_part_write(&write_param); 

    if (ret_val == -1)
    {
        
        return 0;
    }
    else
    {       
       
        g_OTA_var->g_cur_rec_state.cru_pack_count = g_OTA_var->g_cru_pack_count;      
        g_OTA_var->g_cur_rec_state.rec_data_state = RECEIVE_DARA_SUC;
        g_OTA_var->g_cru_pack_count+= 1;//每写一笔数据512BYTE ,记录一下                                  
        if(((g_OTA_var->g_cru_pack_count%128)==0)||(g_OTA_var->g_cru_pack_count==1))  //写一个BLOCK或到最后一包写一次VM
        {       
            sys_vm_read(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //读VM为了把版本读回来  
            g_ota_restore_data.dowmload_statue = OTA_DOWNLOAD_STATE_START ;  
            g_ota_restore_data.part_id = g_OTA_var->g_cur_rec_state.part_id;
            g_ota_restore_data.pack_count = g_OTA_var->g_cru_pack_count ;
            g_ota_restore_data.updata_statue = OTA_UPDATE_STATE_NULL;
            sys_vm_write(&g_ota_restore_data,VM_OTA,sizeof(restore_breakpoint_t));   //写VM     
        }

        return 1;
    }   

}


/*UINT32 SPINor_Part_Check(UINT32 part_addr ,UINT16 part_size , UINT32 part_checksum)
{
    UINT8 *buf;
    INT32 ret_val;
    UINT32 sector_num;
    uint32 sum,addr;
    unsigned short * pbuf;

    buf = (UINT8 *)LFI_ITEM_BUF;//这里用了LFI校验的BUF
    addr = part_addr;

    while(part_size > 0x10)
    {
        sector_num = 0x10;
        ret_val = base_fw_read(addr, buf, sector_num);  //一次将8k读出
        if(ret_val != 0)
        {
            return 1;
        }
        sector_num -= 0x10;
        part_addr += 0x10;
        sum += CheckSum32(buf,0x10<<7);    
    }
   
    sum += CheckSum32(buf,sector_num<<7);

    if(sum == part_checksum)
    {
        return 0;
    }
    else
    {
        return 1;
    }

    
}*/

UINT16 CheckSum16(UINT16 *buf, UINT32 len)
{
    UINT32 index;
    UINT16 sum = 0;

    for (index = 0; index < len; index++)
    {
        sum += buf[index];
    }

    return sum;
}

UINT32 CheckSum32(UINT32 *buf, UINT32 len)
{
    UINT32 index;
    UINT32 sum = 0;

    for (index = 0; index < len; index++)
    {
        sum += buf[index];
    }

    return sum;
}


