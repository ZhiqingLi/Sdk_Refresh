/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *
 *      <author>       <time>
 *       liminxian  2014-8-20
 *       Wekan   2015-3-27  review
 *******************************************************************************/

#include "adjust_freq_inc.h"

//#define _ADJUST_FREQ_DEBUG_

#define _FQ_PRINTD_INFO_INT(a,b)     //PRINTD_INFO_INT(a,b)
#define _FQ_PRINTD_DBG(a)            //PRINTD_INFO(a)
adjust_freq_t adjust_id_table[ADJUST_RUN_TASK_MAX];
int16 add_freq_a;
int16 add_freq_b;

//线程对应的id 表
const uint8 id_to_prio_table[ADJUST_ID_MAX] =
{
    //0
    AP_PROCESS_MANAGER_PRIO,
    //1
    AP_BACK_HIGH_PRIO,//跟DSP关连
    //2
    AP_BACK_LOW_PRIO,
    //3
    AP_BT_HIGH_PRIO,
    //4
    AP_BT_LOW_PRIO,
    //5
    AP_FRONT_HIGH_PRIO,
    //6
    AP_FRONT_LOW_PRIO,
    //7
    KEYTONE_PLAY_PRIO,
    //8
    0,
    //9
    0,
    //10
    0,
    //11
    0 
};

//对应的各档位的 misp 频点
const uint8 a_level_table_sn[1][FREQ_LEVEL_MAX] =
{
    { 00, 1, 2, 4, 6, 8, 10, 20, 30, 40, 60, 85 }, //0
};

//对应的各档位的 dsp 频点
const uint8 b_level_table_sn[2][FREQ_LEVEL_MAX] =
{
    { 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00 },//0
    { 00, 35, 40, 55, 60, 70, 80, 100, 120, 140, 160, 180 }, //1
};

/******************************************************************************
 * \par  Description:  模块初始化
 * \param[in]    none
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/
void adjust_freq_do_init(void)
{
    _FQ_PRINTD_DBG("ad_init");
    libc_memset(adjust_id_table, 0x00, sizeof(adjust_id_table));
    add_freq_a = 0;
    add_freq_b = 0;
}

/******************************************************************************
 * \par  Description:  模块退出
 * \param[in]    none
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/
//void adjust_freq_do_exit(void)
//{
//
//}

/******************************************************************************
 * \par  Description:  查找线程对应的ID
 * \param[in]    prio  调频的线程的优先级
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/

int adjust_freq_find_id(uint8 prio)
{
    uint8 i;
    for (i = 0; i < ADJUST_ID_MAX; i++)
    {
        if (id_to_prio_table[i] == prio)
        {
            return (int) i;
        }
    }
    return -1;
}

/******************************************************************************
 * \par  Description:  真正调整系统频率
 * \param[in]    none
 * \param[out]   null
 * \return           返回0
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/

int adjust_freq_update(void)
{
    adjust_freq_t *pt = adjust_id_table;
    int i, freq_a, freq_b;
    int sum_a = 0, sum_b = 0;

    for (i = 0; i < ADJUST_RUN_TASK_MAX; i++)
    {
        freq_a = (int) a_level_table_sn[pt->freq_a_level_sn][pt->freq_a_level];
        freq_b = (int) b_level_table_sn[pt->freq_b_level_sn][pt->freq_b_level];

        sum_a += freq_a;
        sum_b += freq_b;

        if ((freq_a + freq_b) != 0)
        {
            _FQ_PRINTD_INFO_INT("prio:",(freq_a | (freq_b << 8) | (id_to_prio_table[i] << 16)));
        }
        pt++;
    }

    freq_a = sum_a + (int) add_freq_a;
    freq_b = sum_b + (int) add_freq_b;

    if ((add_freq_a < 0) || (add_freq_b < 0))
    {
        PRINTD_ERR("ad_add_value err!!!");
        freq_a = sum_a;
        freq_b = sum_b;
    }
    
    if(freq_a > 250)
    {
        freq_a = 250;            
    }
    
    if(freq_b > 250)
    {
        freq_b = 250;            
    }
    
    _FQ_PRINTD_INFO_INT("val:",add_freq_a|(add_freq_b<<8));
    _FQ_PRINTD_INFO_INT("ad_update:",freq_a|(freq_b<<8));

    if((GLOBAL_KERNEL_BACKDOOR_FLAG & BACKDOOR_SYSCLK_LOCKED) == 0)
    {
        sys_adjust_clk(freq_a | (freq_b << 8), 0);
    }
    return 0;
}

/******************************************************************************
 * \par  Description:  调节线程的档位设置
 * \param[in]    prio-线程优先级
 * \param[in]   level-频率点档位
 * \param[out]   null
 * \return           返回0成功，-1失败
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/

int adjust_freq_do_task_level(int prio, int level)
{
    int adjust_id;
    uint32 err_no, level_a, level_b;
    level_a = (uint32) (level & 0xff);
    level_b = (uint32) level;
    level_b = level_b >> 8;

    sys_os_sched_lock();

    _FQ_PRINTD_INFO_INT("ad_level:", (prio << 16) | (level_b << 8)| level_a);

    if (prio > OS_TASK_PRIO_MAX)
    {
        err_no = 1;
        goto err;
    }

    adjust_id = adjust_freq_find_id((uint8) prio);
    if (adjust_id == -1)
    {
        err_no = 2;
        goto err;
    }

    if (adjust_id >= ADJUST_RUN_TASK_MAX)
    {
        err_no = 3;
        goto err;
    }

    if (level_a >= FREQ_LEVEL_MAX)
    {
        _FQ_PRINTD_DBG("freq_a is overflow");
        level_a = FREQ_LEVEL_MAX - 1;
    }

    if (level_b >= FREQ_LEVEL_MAX)
    {
        _FQ_PRINTD_DBG("freq_b is overflow");
        level_b = FREQ_LEVEL_MAX - 1;
    }

    adjust_id_table[adjust_id].freq_a_level = level_a;
    adjust_id_table[adjust_id].freq_b_level = level_b;

    if (AP_BACK_HIGH_PRIO == prio)
    {
        //关联DSP频率
        adjust_id_table[adjust_id].freq_b_level_sn = 1;
    }

    adjust_freq_update();

    sys_os_sched_unlock();
    return 0;

    err:

    PRINTD_ERR("*adjust_fq_err");
    PRINTD_INFO_INT("err_id: ", err_no);
    sys_os_sched_unlock();
    return -1;
}

/******************************************************************************
 * \par  Description:  微调dsp，misp频率设置
 * \param[in]
 * \param[in]
 * \param[out]   null
 * \return       返回0成功，-1失败
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/
int adjust_freq_do_add_value(int16 freq_a, int16 freq_b)
{
    sys_os_sched_lock();
    add_freq_a = (int16)freq_a;
    add_freq_b = (int16)freq_b;
    //libc_print("m4k_add_freq",freq_a,2);
    //libc_print("dsp_add_freq",freq_b,2);
    
//    if ((add_freq_a < 0) || (add_freq_b < 0))
//    {
//        add_freq_a = 0;
//        add_freq_b = 0;
//        PRINTD_ERR("ad_add_err");
//        return -1;
//    }

    adjust_freq_update();
    sys_os_sched_unlock();
    
    return 0;
}

//调频接口表

const uint32 adjust_freq_op_entry_table[ADJUST_FQ_CMD_MAX] =
{ 
    (uint32) adjust_freq_do_task_level, 
    (uint32) adjust_freq_do_add_value, 
};

/******************************************************************************
 * \par  Description:  调频总入口,分发
 * \param[in]    p1
 * \param[in]    p2
 * \param[in]    cmd
 * \param[out]   null
 * \return          -1失败,返回0成功
 * \note
 *   <author>    <time>
 *    liminxian  2014-8-20
 *******************************************************************************/

int adjust_freq_op_entry(void *p1, void *p2, int cmd)
{
    int ret = -1;
    //    uint32 sys_ra_call = (uint32) __builtin_return_address(0);

    if (cmd < ADJUST_FQ_CMD_MAX)
    {
        int(*adjust_call)(void*, void*) = (void*)adjust_freq_op_entry_table[cmd];
        ret = (int) adjust_call(p1, p2);
    }
    else
    {
        PRINTD_ERR("ad_cmd err");
    }
    //    if (ret == -1)
    //    {
    //        PRINTD_INFO_INT("ra:", sys_ra_call);
    //    }

    return ret;
}

