#include "uart_recordkey.h"

#ifdef SUPPORT_UART_SIM_KEY

key_record_t record_key_val;
uint32 key_val_20ms = 0;               /* 按键操作定时计数 */
int32 interval_time = 0;               /* 定义按键运行的时间 */ 
bool connect_flag = FALSE;
bool keyplay_flag = FALSE;
uint32 g_record_key_type = 0;
uint32 g_record_key_val = 0;
uint32 g_record_key_time = 0;
uint32 time_cnt = 0;
uint32 g_set_reg;
uint32 g_get_reg;
uint8 g_set_reg_flag;
uint32 g_set_reg_val;

extern uint8 g_oldkey_val;

void get_keyval(uint8 keyval)
{
   g_record_key_time = 0;
   g_record_key_type = 1;
   g_record_key_val = keyval;
   libc_print("keyval:",g_record_key_val,2);
}
void get_keytime(void)
{
     g_record_key_time++;
     if (g_record_key_time >= HOLD_KEY_TIMER*5)
     {
        g_record_key_type = 2;
     }
}

void reocrdkey_print(void)
{
    int32 key_data = 0;
    if (connect_flag == TRUE)
    {
        key_data = g_record_key_type;
        key_data = (key_data << 8)+g_record_key_val;
        key_data = (key_data << 16)+(g_record_key_time*20+60+60);
        libc_print("<KEY>:",key_data,2);
        g_record_key_type = 1;
        g_record_key_time = 0;
    }
}

//分割字符串并转换成整形数据
uint32 atoitok(const char *src, int addr, int lenth)
{
    int total = 0;
    int temp_value;
    while (addr != 0)
    {
      src++;
      addr--;
    }
    while (lenth != 0)
    {
        if ((*src >= 'a') && (*src <= 'f'))
        {
            temp_value = 10 + *src - 'a';
        }
        else if ((*src >= 'A') && (*src <= 'F'))
        {
            temp_value = 10 + *src - 'A';
        }
        else
        {
            temp_value = *src - '0';
        }
        total = total * 16 + temp_value;
        src++;
        lenth--;
    }
    return total;
}

uint8 record_key_play(uint8 key_val)
{
    if (keyplay_flag == TRUE)
    {
        if ((key_val_20ms >= 0) && (key_val_20ms++ < (record_key_val.record_key_time)/20))
        {
            key_val = record_key_val.record_key_id;         // 读取长按键值
        }
        else
        {
             key_val = NO_KEY;
        }
            
        interval_time = interval_time - 20;
    
        if ((interval_time <= 0))
        {         
           key_val_20ms = 0;
           keyplay_flag = FALSE;
           libc_print("<SUCCESS>",0,0);
        }    
    }
    return key_val;
}

void key_data_check(uint8 rx_buf[17])
{
  //  libc_print(rx_buf,0,0);
    
    if (libc_strncmp(rx_buf,"keyval:",7) == 0)
    {
        if (connect_flag == TRUE)
        {
            interval_time = 0;
            key_val_20ms = 0;
            keyplay_flag = FALSE;
            libc_memset(record_key_val.record_key_id,0,sizeof(record_key_val));
            libc_memset(record_key_val.record_key_type,0,sizeof(record_key_val));
            libc_memset(record_key_val.record_key_time,0,sizeof(record_key_val));
            record_key_val.record_key_type = atoitok(rx_buf,7,2);
            record_key_val.record_key_id   = atoitok(rx_buf,9,2);
            record_key_val.record_key_time = atoitok(rx_buf,11,4);        
            interval_time = (int32)record_key_val.record_key_time;
            key_val_20ms = 0;
            keyplay_flag = TRUE;
            libc_print("<SUCCESS>",0,0);
        }
    }    
    else if (libc_strncmp(rx_buf,"connect",7) == 0)
    {
        connect_flag = TRUE;
        libc_print("<connected>",0,0);
        
    }
    else if (libc_strncmp(rx_buf,"disconnect",10) == 0)
    {
        connect_flag = FALSE;
        keyplay_flag = FALSE;
        g_set_reg = 0;
        g_set_reg_flag = 0;
        g_set_reg_val = 0;
        libc_print("<disconnect>",0,0);
    }
    else if (libc_strncmp(rx_buf,"regwrite",8) == 0)
    {
        if ((g_set_reg_val != 0) && (g_set_reg != 0))
        {
            act_writel(g_set_reg_val,g_set_reg);
            g_set_reg = 0;
            g_set_reg_flag = 0;
            g_set_reg_val = 0;
            //libc_print("reg:",g_set_reg,2);
            //libc_print("set:",g_set_reg_val,2);
            libc_print("reg changed",0,0);
        }
        else
        {
            //libc_print("err",0,0);
            //libc_print("regval:",g_set_reg_val,2);
            //libc_print("reg:",g_set_reg,2);
        }
    }
    else if (libc_strncmp(rx_buf,"regval:",7) == 0)
    {
        g_set_reg_val = atoitok(rx_buf,7,8);
        //libc_print("regval:",g_set_reg_val,2);
    }
    else if (libc_strncmp(rx_buf,"setreg:",7) == 0)
    {
        g_set_reg = atoitok(rx_buf,7,8);
        //libc_print("reg:",g_set_reg,2);
    }
    else if (libc_strncmp(rx_buf,"getreg:",7) == 0)
    {
        g_get_reg = atoitok(rx_buf,7,8);
        libc_print("regval:",act_readl(g_get_reg),2);
    }
    
    libc_memset(rx_buf,0,sizeof(rx_buf));
}
uint8 uart1_receive_deal(void)
{ 
    uint16 rw_index = 0; //读写数据序号
    uint8 uart_rx_buf[17]; //uart接收物理通信buffer
    
    libc_memset(uart_rx_buf,0,sizeof(uart_rx_buf));
    while ((act_readl(UART1_STA) & 0xf800) != 0)
    {
        //get data from  uart fifo
        uart_rx_buf[rw_index++] = (uint8) act_readl(UART1_RXDAT);
        if(rw_index == 16)
        {         
             rw_index = 0;
             break;
        }
    }    
    //识别数据
    key_data_check(uart_rx_buf);
    //clear pending bit    
    act_writel((1<<UART1_STA_RIP1),UART1_STA);    
}
void uart1_process(void)
{
    if ((act_readl(UART1_STA) & (1 << 5)) == 0)
    {
        time_cnt = 0;
        sys_mdelay(2);
        //receive data deal
        uart1_receive_deal();
    }
    else
    {
        time_cnt++;
        if (time_cnt >= 450)
        {
            //libc_print("no receive data",0,0);
            time_cnt = 0;
        }
    }
}

#endif
