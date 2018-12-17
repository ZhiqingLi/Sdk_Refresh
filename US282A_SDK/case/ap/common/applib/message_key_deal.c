/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：按键逻辑处理模块，将按键驱动发送上来的物理按键消息解析为逻辑按键消息，包括
 *       双击按键的分析处理。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

key_infor_t __section__(".gl_key_data") g_key_infor;

uint8 __section__(".gl_key_data") g_key_hold;
const uint8 IrcKeyValueTab[IR_CODE_MAX][IR_KEY_MAX] =
{
    {
        0x45, 0x46, 0x47, 0x44, 0x40, 0x43, 0x07, 0x15, 0x09,
        0x16, 0x19, 0x0d, 0x0c, 0x18, 0x5e, 0x08, 0x1c, 0x5a,
        0x42, 0x52, 0x4a
    },
    {
        0x00, 0x01, 0x02, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a,
        0x0c, 0x0d, 0x0e, 0x10, 0x11, 0x12, 0x14, 0x15, 0x16,
        0x18, 0x19, 0x1a
    },
    {
        0x12, 0x1a, 0x1e, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0a, 0x1b, 0x1f, 0x0c, 0x0d, 0x0e,
        0x00, 0x0f, 0x19
    },
    {
        0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2f,
        0x2b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00
    },
};

const uint8 IrcKeyMsgTab[IR_CODE_MAX][IR_KEY_MAX] =
{
    {
        KEY_MODE, KEY_CLK, KEY_MUTE, KEY_PREV,
        KEY_NEXT, KEY_PLAY, KEY_VSUB, KEY_VADD, KEY_EQ, KEY_NUM0,
        KEY_FOLDER_SUB, KEY_FOLDER_ADD, KEY_NUM1, KEY_NUM2, KEY_NUM3,
        KEY_NUM4, KEY_NUM5, KEY_NUM6, KEY_NUM7, KEY_NUM8, KEY_NUM9
    },
    {
        KEY_POWER, KEY_MODE, KEY_PLAY, KEY_REPEAT,
        KEY_CLK, KEY_MUTE, KEY_EQ, KEY_VSUB, KEY_VADD, KEY_NUM0,
        KEY_PREV, KEY_NEXT, KEY_NUM1, KEY_NUM2, KEY_NUM3,
        KEY_NUM4, KEY_NUM5, KEY_NUM6, KEY_NUM7, KEY_NUM8, KEY_NUM9
    },
#if(CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    {
        KEY_POWER, KEY_MODE, KEY_PLAY, KEY_PREV,
        KEY_NEXT, KEY_PHONEPLAY, KEY_VSUB, KEY_VADD, KEY_EQ,KEY_NUM0,
        KEY_PREV, KEY_NEXT, KEY_NUM1, KEY_NUM2, KEY_NUM3,
        KEY_NUM4, KEY_NUM5, KEY_NUM6, KEY_NUM7, KEY_NUM8, KEY_NUM9
    },
#else
    {
        KEY_POWER, KEY_MODE, KEY_PLAY, KEY_REPEAT,
        KEY_CLK, KEY_MUTE, KEY_EQ, KEY_VSUB, KEY_VADD, KEY_NUM0,
        KEY_PREV, KEY_NEXT, KEY_NUM1, KEY_NUM2, KEY_NUM3,
        KEY_NUM4, KEY_NUM5, KEY_NUM6, KEY_NUM7, KEY_NUM8, KEY_NUM9
    },
#endif
    {
        KEY_POWER, KEY_MUTE, KEY_NEXT, KEY_PREV,
        KEY_PLAY, KEY_NEXT, KEY_PREV, KEY_VSUB, KEY_MODE, KEY_VADD,
        KEY_NUM0, KEY_NUM0, KEY_NUM0, KEY_NUM0, KEY_NUM0,
        KEY_NUM0, KEY_NUM0, KEY_NUM0, KEY_NUM0, KEY_NUM0, KEY_NUM0
    },
};

/*! \cond COMMON_API */

uint8 com_phy_key_phy2log(key_phy_msg_t *p_key_msg)
{
    if (p_key_msg->key_type == KEY_TYPE_KEY)
    {
        if (p_key_msg->key_id == KEY_PHY_PLAY)
        {
            return KEY_PLAY;
        }

        return g_key_infor.key_val_map[p_key_msg->key_id];
    }
    else if (p_key_msg->key_type == KEY_TYPE_IR)
    {
        uint8 key_code, key_id;

        key_code = p_key_msg->key_id;
        for (key_id = 0; key_id < IR_KEY_MAX; key_id++)
        {
            if (key_code == IrcKeyValueTab[p_key_msg->key_ir_code][key_id])
            {
                break;
            }
        }

        if (key_id >= IR_KEY_MAX)
        {
            return KEY_NULL;
        }

        return IrcKeyMsgTab[p_key_msg->key_ir_code][key_id];
    }
    else if (p_key_msg->key_type == KEY_TYPE_TK)
    {
        return g_key_infor.tk_key_val_map[p_key_msg->key_id];
    }
    else
    {
        return KEY_NULL;
    }
}

bool com_need_cont_dck_deal(uint8 key_count)
{
    uint16 tmp_timestamp = (uint16) sys_get_ab_timer();
    uint32 tmp_time = tmp_timestamp;

    if (tmp_timestamp < g_key_infor.dck_timestamp)
    {
        tmp_time += 0x10000; 
    }

    if ((uint32)( tmp_time - (uint32)g_key_infor.dck_timestamp) > (uint32)300)
    {
        g_key_infor.status = KEY_STATUS_NULL;
        g_key_infor.dck_fail = TRUE;
        g_key_infor.count_fail = g_key_infor.count;
        g_key_infor.dck_flag = FALSE;
        g_key_infor.count = 0;
        return TRUE;
    }

    if (key_count <= g_key_infor.count)
    {
        return FALSE;
    }

    return TRUE;
}

#define KEYTONE_NOBLOCK        0 //非阻塞模式，默认为非阻塞模式
#define KEYTONE_BLOCK          1 //阻塞模式
extern void com_start_key_tone(uint8 mode) __FAR__;

bool com_phy_key_fsm(input_gui_msg_t *input_msg)
{
    key_phy_msg_t key_msg;
    uint8 key_log_id = KEY_NULL;
    key_type_e key_type = KEY_TYPE_NULL;
    bool need_ret_gui = FALSE;
    bool need_recv_phy = FALSE;
    bool no_keytone = FALSE;

    if (g_key_infor.status >= KEY_STATUS_DOWN_1ST)
    {
        if (sys_mq_traverse(MQ_ID_GUI, (void *) &key_msg, g_key_infor.count) == -1)
        {
            return FALSE;
        }
    }
    else
    {
        if (sys_mq_traverse(MQ_ID_GUI, (void *) &key_msg, 0) == -1)
        {
            return FALSE;
        }
    }

    PRINT_DBG_INT("phykey:", (key_msg.key_status << 16) + key_msg.key_id);

    key_log_id = com_phy_key_phy2log(&key_msg);
    if (key_log_id == KEY_NULL)
    {
        return FALSE;
    }

    cont_phy_key_fsm:
    switch (g_key_infor.status)
    {
        case KEY_STATUS_NULL:
        /*刚开始按键*/
        if ((g_key_infor.dck_fail == TRUE) || ((g_key_infor.dck_map & (uint32) (1 << key_log_id)) == 0))
        { 
            //检测到一个按键，如果其初始的物理状态为抬起，过滤掉该按键消息 
            if (key_msg.key_status == KEY_PHY_UP)
            {  
                need_ret_gui = FALSE;
                need_recv_phy = TRUE;             
                break;
            }
            
            //不需要分析双击按键，读取按键消息并返回
            g_key_infor.status = KEY_STATUS_DOWN;
            g_key_infor.count = 1;
            key_type = KEY_TYPE_DOWN;
            need_ret_gui = TRUE;
            need_recv_phy = TRUE;
            if (g_key_infor.dck_fail == TRUE)
            {
                no_keytone = TRUE;
            }
        }
        else
        { //需要分析双击按键，直接返回
            g_key_infor.status = KEY_STATUS_DOWN_1ST;
            com_start_key_tone(KEYTONE_NOBLOCK); //双击按键音
            g_key_infor.dck_val = key_log_id;
            g_key_infor.count = 1;
            g_key_infor.dck_timestamp = (uint16) sys_get_ab_timer();
            g_key_infor.dck_flag = TRUE;
        }
        break;

        case KEY_STATUS_DOWN:
        /*已经检测到按键按下*/
        if (key_msg.key_status == KEY_PHY_UP)
        {
            g_key_infor.status = KEY_STATUS_NULL;
            key_type = KEY_TYPE_SHORT_UP;
            need_ret_gui = TRUE;
        }
        else
        {
            if (g_key_infor.count < 5)
            {
                g_key_infor.count++;
            }
            else
            {
                g_key_hold=0;        
                g_key_infor.status = KEY_STATUS_HOLD;
                key_type = KEY_TYPE_LONG;
                need_ret_gui = TRUE;
            }
        }
        need_recv_phy = TRUE;
        break;

        case KEY_STATUS_HOLD:
        /*已经检测到按键保持*/
        if (key_msg.key_status == KEY_PHY_UP)
        {
            g_key_infor.status = KEY_STATUS_NULL;
            key_type = KEY_TYPE_LONG_UP;
            need_ret_gui = TRUE;
        }
        else
        {
            key_type = KEY_TYPE_HOLD;
            need_ret_gui = TRUE;
            g_key_hold++;
            libc_print("long hold",g_key_hold,2);
            if (g_key_hold>46)
            {
                g_key_hold=0;
                key_type = KEY_TYPE_LONG_10S;
                need_ret_gui = TRUE;
            }
        }
        need_recv_phy = TRUE;
        break;

        case KEY_STATUS_DOWN_1ST:
        g_key_infor.count++;
        if (key_log_id == g_key_infor.dck_val)
        {
            if (key_msg.key_status == KEY_PHY_UP)
            {
                g_key_infor.status = KEY_STATUS_UP_1ST;
                g_key_infor.dck_timestamp = (uint16) sys_get_ab_timer();
            }
        }
        else
        {
            g_key_infor.status = KEY_STATUS_NULL;
            g_key_infor.dck_fail = TRUE;
            g_key_infor.dck_flag = FALSE;
            g_key_infor.count_fail = g_key_infor.count;
            g_key_infor.count = 0;
            goto cont_phy_key_fsm;
        }
        break;

        case KEY_STATUS_UP_1ST:
        g_key_infor.count++;
        if (key_log_id == g_key_infor.dck_val)
        {
            g_key_infor.status = KEY_STATUS_DOWN_2ND;
            com_start_key_tone(KEYTONE_NOBLOCK); //双击按键音
            g_key_infor.dck_timestamp = (uint16) sys_get_ab_timer();
        }
        else
        {
            g_key_infor.status = KEY_STATUS_NULL;
            g_key_infor.dck_fail = TRUE;
            g_key_infor.dck_flag = FALSE;
            g_key_infor.count_fail = g_key_infor.count;
            g_key_infor.count = 0;
            goto cont_phy_key_fsm;
        }
        break;

        case KEY_STATUS_DOWN_2ND:
        g_key_infor.count++;
        if (key_log_id == g_key_infor.dck_val)
        {
            if (key_msg.key_status == KEY_PHY_UP)
            {
                uint8 i;

                for (i = 0; i < g_key_infor.count; i++)
                {
                    sys_mq_receive(MQ_ID_GUI, (void * ) &key_msg);
                }

                g_key_infor.count = 0;
                g_key_infor.status = KEY_STATUS_NULL;
                g_key_infor.dck_flag = FALSE;
                key_type = KEY_TYPE_DBL_CLICK;
                need_ret_gui = TRUE;
            }
        }
        else
        {
            g_key_infor.status = KEY_STATUS_NULL;
            g_key_infor.dck_fail = TRUE;
            g_key_infor.dck_flag = FALSE;
            g_key_infor.count_fail = g_key_infor.count;
            g_key_infor.count = 0;
            goto cont_phy_key_fsm;
        }
        break;

        default:
        break;
    }

    if (g_key_infor.dck_fail == TRUE)
    {
        if (g_key_infor.count_fail > 0)
        {
            g_key_infor.count_fail--;
        }

        if ((g_key_infor.count_fail == 0) && (key_msg.key_status == KEY_PHY_UP))
        {
            g_key_infor.dck_fail = FALSE;
        }
    }

    if (need_recv_phy == TRUE)
    {
        sys_mq_receive(MQ_ID_GUI, (void * ) &key_msg);
    }

    if (need_ret_gui == TRUE)
    {
        input_msg->data.kmsg.val = key_log_id;
        input_msg->data.kmsg.type = key_type;
        input_msg->data.kmsg.option = 0;
        if (no_keytone == TRUE)
        {
            input_msg->data.kmsg.option |= 1;
        }

        PRINT_DBG_INT("logkey:", (key_type << 16) + key_log_id);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//过滤当前队列中相同“动作”的按键消息
//k_type:KEY_TYPE_SHORT_UP 在DOWN或SHORT_UP响应时，会过滤掉短按动作的DOWN和SHORT_UP消息
//k_type:KEY_TYPE_HOLD 在LONG或HOLD响应时，会过滤掉HOLD消息
//其他暂不支持
void com_filter_keymsg_in_queue(key_value_e k_val, key_type_e k_type)
{
    key_phy_msg_t key_msg;
    uint8 key_log_id = KEY_NULL;
    uint8 key_count, i;
    bool filter_flag;

    switch (k_type)
    {
    case KEY_TYPE_SHORT_UP:
        //当前必须是等待新按键周期
        if ((g_key_infor.status == KEY_STATUS_NULL) || (g_key_infor.status == KEY_STATUS_DOWN))
        {
            filter_again:

            filter_flag = FALSE;
            key_count = 0;
            if (g_key_infor.status == KEY_STATUS_DOWN)
            {
                key_count = g_key_infor.count;
            }
            while (1)
            {
                if (sys_mq_traverse(MQ_ID_GUI, (void *) &key_msg, key_count) == -1)
                {
                    break;
                }

                key_log_id = com_phy_key_phy2log(&key_msg);
                if (key_log_id != k_val)
                {
                    break;
                }

                if ((key_count == 0) && (key_msg.key_status != KEY_PHY_DOWN))
                {
                    //队列中第一个按键，必须是按键周期的第一个按键，否则马上返回
                    break;
                }

                if (key_msg.key_status == KEY_PHY_UP)
                {
                    filter_flag = TRUE;
                    break;
                }

                key_count++;
                if (key_count > 5) //已经是LONG或HOLD了，马上返回
                {
                    break;
                }
            }

            if (filter_flag == TRUE)
            {
                PRINT_INFO("filter key");

                //过滤所有DOWN和UP物理消息
                for (i = 0; i < (key_count + 1); i++)
                {
                    sys_mq_receive(MQ_ID_GUI, (void * ) &key_msg);
                }

                //按键状态改为 KEY_STATUS_NULL
                if (g_key_infor.status == KEY_STATUS_DOWN)
                {
                    g_key_infor.status = KEY_STATUS_NULL;
                }

                goto filter_again;
            }
        }
        break;

    case KEY_TYPE_HOLD:
        //当前必须是按键保持状态
        if (g_key_infor.status == KEY_STATUS_HOLD)
        {
            while (1)
            {
                if (sys_mq_traverse(MQ_ID_GUI, (void *) &key_msg, 0) == -1)
                {
                    break;
                }

                key_log_id = com_phy_key_phy2log(&key_msg);
                if (key_log_id != k_val)
                {
                    break;
                }

                if (key_msg.key_status != KEY_PHY_UP)
                {
                    PRINT_INFO("filter key");

                    sys_mq_receive(MQ_ID_GUI, (void * ) &key_msg);
                }
                else
                {
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

/*! \endcond */
