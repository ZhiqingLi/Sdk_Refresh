/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：生成蓝牙设备随机地址。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_btmanager.h"

#define ADC_SOURCE_MIC    0
#define MAX_SAMPLE_CNT    50

uint8 g_adc_buffer[0x400] _BANK_DATA_ATTR_;
uint16 g_sound_data[MAX_SAMPLE_CNT] _BANK_DATA_ATTR_;
volatile uint8 g_sound_index _BANK_DATA_ATTR_;
int32 timer_id _BANK_DATA_ATTR_;

/******************************************************************************/
/*!
 * \par  Description:
 * \配置ADC采样的参数以及DMA的配置
 * \param[in]    dst_addr 采集数据存放的缓冲区地址
 * \param[in]    block_length 采集的数据长度
 * \param[out]   none
 * \return               app_result_e
 * \retval
 * \ingroup      bt_common_gen_addr.c
 * \note
 */
/*******************************************************************************/
static void start_sample_sound(uint32 dst_addr, uint32 block_length)
{
    uint32 status;

    uint8 loop_cnt = 0;

    //左右声道均会采集数据
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADLEN), ADC_ANACTL);

    //左右声道均会采集数据
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADREN), ADC_ANACTL);

    //abort DMA stransfer and reset config
    *((REG32) (DMA1CTL)) = 0x00;

    //data width 16bit
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DATAWIDTH_SHIFT), DMA1CTL);

    //source addr type:adc fifo
    act_writel(act_readl(DMA1CTL) | (0x0b << DMA1CTL_SRCTYPE_SHIFT), DMA1CTL);

    //dest addr type:memory
    act_writel(act_readl(DMA1CTL) | (0 << DMA1CTL_DSTTYPE_SHIFT), DMA1CTL);

#ifdef PRINT_CHANNEL_DATA
    //reload enable
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_reload), DMA1CTL);
#endif

    //dest address
    act_writel(dst_addr, DMA1DADDR0);

    //dma length
    act_writel(block_length / 2, DMA1FrameLen);

    //enable transfer
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);

#ifdef PRINT_CHANNEL_DATA
    while(1)
    {
        int i;

        DEBUG_ATT_PRINT("\n**************\n", 0, 0);

        for(i = 0; i < 128; i += 4)
        {
            DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i), 1);
            DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i + 2), 1);
        }

        sys_mdelay(500);
    }
#endif

    while (1)
    {
        status = act_readl(DMA1CTL);

        if ((status & 0x01) == 0)
        {
            loop_cnt++;

            if(loop_cnt < 2)
            {
                act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);
            }
            else
            {
                //abort DMA stransfer and reset config
                *((REG32) (DMA1CTL)) = 0x00;
                break;
            }

        }
    }

    //abort DMA stransfer and reset config
    *((REG32) (DMA1CTL)) = 0x00;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \计算能量值函数
 * \param[in]    dac_buffer 采集数据buffer
 * \param[in]    data_length 采集数据长度 uint16为单位
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 */
/*******************************************************************************/
static uint16 calculate_power_value(int16 *dac_buffer, uint32 data_length)
{
    uint32 i;
    uint32 power_value = 0;
    uint32 power_sample_value = 0;

    if (data_length == 0)
    {
        return 0;//for QAC
    }

    //计算单个声道的样本点
    for (i = 0; i < (data_length * 2); i += 2)
    {
        if (dac_buffer[i] >= 0)
        {
            power_sample_value = (uint32)(dac_buffer[i]);
        }
        else
        {
            power_sample_value = (uint32)(-dac_buffer[i]);
        }

        power_value += power_sample_value;
    }

    power_value = power_value / data_length;

    return power_value;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \初始化ADC函数及AIN通道
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 * \note
 */
/*******************************************************************************/
void sample_sound_init(void)
{
    enable_adc(1, 0);

    set_adc_rate(16);

    enable_ain(AI_SOURCE_ASEMIC, 6);
}

/******************************************************************************/
/*!
 * \par  Description:
 * \退出采集函数
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 * \note
 */
/*******************************************************************************/
void sample_sound_exit(void)
{
    disable_ain(AI_SOURCE_ASEMIC);

    disable_adc();
}

/******************************************************************************/
/*!
 * \par  Description:
 * \数据采集函数
 * \param[in]    arg 测试配置输入参数
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 */
/*******************************************************************************/
uint32 sample_sound_data(void)
{
    uint32 power_value;

    //mic只有一个左声道，使用左声道采集数据
    start_sample_sound(g_adc_buffer, sizeof(g_adc_buffer));

    //字节数据转换成uint16
    power_value = calculate_power_value(g_adc_buffer, sizeof(g_adc_buffer) / sizeof(short));

    return power_value;
}

/******************************************************************************/
/*!
 * \par  Description:
 * \挂载的timer1中断函数
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 */
/*******************************************************************************/
void tick_ISR(void)
{
    if(g_sound_index < MAX_SAMPLE_CNT)
    {
        g_sound_data[g_sound_index] = (uint16)sample_sound_data();
        g_sound_index++;
    }
}

void tick_ISR_install(void)
{
    timer_id = sys_set_irq_timer1(tick_ISR, 1);
    g_sound_index = 0;
}

void tick_ISR_uninstall(void)
{
    sys_del_irq_timer1((uint32)timer_id);
}

/******************************************************************************/
/*!
 * \par  Description:
 * \生成蓝牙LAP域随机地址
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      bt_common_gen_addr.c
 */
/*******************************************************************************/
uint32 gen_random_lap(void)
{
    uint8 i;

    uint32 temp_rand;

    uint32 high_val;

    sys_os_sched_lock();

    sample_sound_init();

    tick_ISR_install();

    while(1)
    {
        if(g_sound_index == MAX_SAMPLE_CNT)
        {
            tick_ISR_uninstall();
            sample_sound_exit();
            break;
        }

        sys_os_time_dly(2);
    }

    temp_rand = 0;

    for(i = 0; i < MAX_SAMPLE_CNT; i++)
    {
        temp_rand += g_sound_data[i];
    }

    temp_rand += sys_random();

    temp_rand &= 0xffff;

    high_val = ((temp_rand >> 8) + (temp_rand & 0xff)) & 0xff;

    temp_rand += (high_val << 16);

    sys_os_sched_unlock();

    return temp_rand;
}

//生成蓝牙LAP域随机地址
uint32 com_btmanager_gen_random_lap(void)
{
    uint32 bt_random_lap;
    uint16 btstack_magic;
    int32 ret_val;
    uint32 i;
    uint8 bt_addr[6];

    nvram_param_rw_t param_rw;

    sys_vm_read(&btstack_magic, VM_BTSTACK, sizeof(uint16));

    if ((btstack_magic != VRAM_MAGIC(VM_BTSTACK)) && (btstack_magic != ATT_MAGIC))
    {
gen_random_addr:    
        //如果还在播放按键音，要等待按键音播放完毕
        keytone_play_deal_wait();

        //如果还在播报TTS，要等待TTS播报完毕
        com_tts_state_play_wait();

        //生成随机数
        bt_random_lap = gen_random_lap();
        PRINT_INFO_INT("gen random lap:", bt_random_lap);    
    }
    else
    {
        if(btstack_magic == ATT_MAGIC)
        {
             param_rw.logical_index = PARAM_BT_ADDR;
             param_rw.rw_len = sizeof(bt_addr);
             param_rw.rw_buffer = bt_addr;
            
             //如果未读取到蓝牙地址，需要随机生成蓝牙地址
             if(base_param_read(&param_rw) != 0) 
             {
                goto gen_random_addr;
             }

        }
        else
        {
            bt_random_lap = sys_random();         
        }
    }

    return bt_random_lap;
}
