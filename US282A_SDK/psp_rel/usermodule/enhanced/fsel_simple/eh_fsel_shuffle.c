//shuffle相关
#include "eh_fsel_entry.h"

const uint8 byte_set_val[8] = { 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
const uint8 byte_get_val[8] = { 0x01, 0x02, 0x4, 0x08, 0x10, 0x20, 0x40, 0x80 };

//根据组号初始化组内成员的map状态
void init_member_map(void)
{
    uint8 i, j;
    for (i = 0; i < (member_avg / 8); i++)
    {
        member_map[i] = 0xff;
    }

    j = member_avg % 8;

    member_max = member_avg;

    if (cur_group < member_avg_group)
    {
        //组号在平均值组的前面时
        j++;
        member_max++;
    }

    if (j != 0)
    {
        member_map[i] = byte_set_val[j - 1];
    }

    return;

}

//设置占用组的状态
//bitmap 位图表地址
//num 第几个bit(1~8)
bool set_bitmap_bit(uint8*bitmap, uint16 num)
{
    uint8 i, j;

    num--;//变为从0开始
    i = (uint8) (num / 8);
    j = (uint8) (num % 8);

    if ((bitmap[i] & byte_get_val[j]) > 0)
    {
        //清除该bit,表示占用
        bitmap[i] = (uint8) (bitmap[i] & (255 - byte_get_val[j]));
        return TRUE;
    }

    return FALSE;
}

//查找未播放过的成员
//返回值范围:1~n
uint16 get_bitmap_bit(uint8 *bitmap, uint8 count_max, uint16 rand_num)
{
    uint16 i;
    //随机数是奇数
    if ((rand_num & 0x01) > 0)
    {
        //从头往前找
        for (i = 1; i < (count_max + 1); i++)
        {
            if (FALSE != set_bitmap_bit(bitmap, i))
            {
                return i;
            }
        }
    }
    else
    {
        //从末往回找
        for (i = count_max; i > 0; i--)
        {
            if (FALSE != set_bitmap_bit(bitmap, i))
            {
                return i;
            }
        }

    }

    return 0;
}

//获取shuffle的组号
//返回值: group_num(1~n)
uint16 get_group_num(void)
{
    uint16 rand_num;
    uint8 i = 3;
    uint8 tmp_group;
    //如果只有一组？？？
    if (group_max == 0)
    {
        return 1;
    }
    tmp_group = group_max;
    while (i != 0)
    {
        i--;
        //try 3次,取得随机数
        rand_num = (uint16) ((sys_random() % tmp_group) + 1);
        //设置组号bitmap
        if (FALSE != set_bitmap_bit(group_map, rand_num))
        {
            return rand_num;
        }
    }
    //获取bitmap
    return get_bitmap_bit(group_map, group_max, rand_num);
}

//获取组内成员的shuffle号
//返回值: member_num(1~n)
static uint16 get_member_num(void)
{
    uint16 rand_num;
    uint16 temp8;
    uint8 i = 3;

    if (0 == member_max)
    {
        return 1;
    }

    temp8 = member_max;

    if (cur_group < member_avg_group)
    {
        //组号在平均值组的前面时
        temp8++;
    }

    while (i != 0)
    {
        i--;
        //try 3次
        rand_num = (uint8) ((sys_random() % temp8) + 1);
        if (FALSE != set_bitmap_bit(member_map, rand_num))
        {
            return rand_num;
        }
    }

    return get_bitmap_bit(member_map, member_max, rand_num);

}

//获取回溯队列中的下一个
//返回值: file_no (1~file_total)
uint16 get_nextlist_num(void)
{
    if (cur_save_pt >= (SET_PREV_MAX - 1))
    {
        //循环buffer的方式存放
        cur_save_pt = 0;
    }
    else
    {
        cur_save_pt++;
    }

    if (cur_save_prev_count < SET_PREV_MAX)
    {
        cur_save_prev_count++;
    }

    cur_save_next_count--;

    return play_num_list[cur_save_pt];

}

//获取回溯队列中的上一个
//返回值: file_no (1~file_total)
static uint16 get_prevlist_num(void)
{
    if (cur_save_pt == 0)
    {
        //循环buffer的方式存放
        cur_save_pt = SET_PREV_MAX - 1;
    }
    else
    {
        cur_save_pt--;
    }

    if (cur_save_next_count < SET_PREV_MAX)
    {
        cur_save_next_count++;
    }

    cur_save_prev_count--;

    return play_num_list[cur_save_pt];
}

//获取播放的shuffle的file_no
//返回值：file_no(1~file_total)
uint16 get_shuffle_num(void)
{
    uint16 temp_num;

    if (FALSE == use_member_flag)
    {
        //只使用组号shuffle
        cur_group = (uint8) get_group_num();
        return (uint16) cur_group;
    }

    //使用组内shuffle
    if (cur_group == 0)
    {//首次进入
        cur_group = (uint8) get_group_num();
        if (0 == cur_group)
        {
            //所有组已经播放完
            return 0;
        }
        init_member_map();
    }

    cur_member = get_member_num();
    if (0 == cur_member)
    {
        //当前组成员已全部播放完
        cur_group = (uint8) get_group_num();
        if (0 == cur_group)
        {
            //所有组已经播放完
            return 0;
        }
        init_member_map();
        cur_member = get_member_num();
        if (0 == cur_member)
        {
            fsel_error = FSEL_ERR_SHUFFLE;
            return 0;
        }
    }

    temp_num = SET_GROUP_MAX * (cur_member - 1) + cur_group;
    return temp_num;
}

//初始shuffle机制
//num_total 需要shuffle的文件总数
bool fsel_init_shuffle(uint16 num_total)
{
    uint8 i, j;
    //在此处赋值为0，初始化不过滤文件
    filtrate_shuffle_no = 0;//

    if (num_total == 0)
    {
        return FALSE;
    }
    //组号映射表
    libc_memset(group_map, 0x00, sizeof(group_map));
    //组内成员映射表
    libc_memset(member_map, 0x00, sizeof(member_map));
    //使用组内shuffle功能标志
    use_member_flag = FALSE;
    //当前组号
    cur_group = 0;
    //当前成员号
    cur_member = 0;
    //当前存的指针(0~)
    cur_save_pt = SET_PREV_MAX;
    //前一个号
    cur_save_prev_count = 1;
    //后一个号
    cur_save_next_count = 1;
    //表翻转标记,1-yes,0-no
    cur_overturn_flag = 0;
    //shuffle 文件总数
    shuffle_total = num_total;

    if (num_total > SET_GROUP_MAX)
    {
        //个数大于组数,启用组内成员shuffle
        use_member_flag = TRUE;
        libc_memset(group_map, 0xff, sizeof(group_map));
        group_max = SET_GROUP_MAX;
    }
    else
    {
        //个数少于组数,只用组shuffle就可以了
        group_max = (uint8) num_total;
        for (i = 0; i < (num_total / 8); i++)
        {
            group_map[i] = 0xff;
        }

        if ((num_total % 8) != 0)
        {
            j = (uint8) ((num_total % 8) - 1);
            group_map[i % (SET_GROUP_MAX / 8)] = byte_set_val[j];
        }
    }

    if (FALSE != use_member_flag)
    {
        //个数大于组数,组内成员shuffle
        member_avg = (uint8) (num_total / SET_GROUP_MAX);
        member_avg_group = (uint8) ((num_total % SET_GROUP_MAX) + 1);
    }

    return TRUE;

}

//获取shuffle机制的下一个文件
//返回值: file_no (1~file_total)
uint16 fsel_shuffle_next_file(void)
{
    uint16 temp_num;

    if (shuffle_total < eh_file_total_all)
    {
        //文件个数变多了(增加文件)，重新初始化
        fsel_init_shuffle(eh_file_total_all);
    }
    else
    {
        //重新赋值防止 file_total<shuffle_total的情况(文件删除)
        shuffle_total = eh_file_total_all;
    }
    //无效
    if (shuffle_total == 0)
    {
        return 0;
    }
    //如果回溯,则直接从保存的16个文件中查找,否则直接shuffle
    while (cur_save_next_count > 1)
    {
        temp_num = get_nextlist_num();
        if (temp_num <= shuffle_total)
        {
            if (temp_num != 0)
            {
                return temp_num;
            }
            else
            {
                break;
            }
        }

        cur_save_next_count--;
    }

    //list can't find,use shuffle al
    //保证随机出来的值在shuffle_total范围内,并与file_no不相同
    do
    {
        temp_num = get_shuffle_num();
        if (temp_num == 0)
        {
            break;
        }

    } while ((filtrate_shuffle_no == temp_num) || (temp_num > shuffle_total));

    if (0 != temp_num)
    {
        //SAVE TO PLAY_NUM_LIST
        if (cur_save_pt >= (SET_PREV_MAX - 1))
        {
            //循环buffer的方式存放
            cur_save_pt = 0;
        }
        else
        {
            cur_save_pt++;
        }
        //向前切换歌曲时的回溯
        if (cur_save_prev_count < SET_PREV_MAX)
        {
            cur_save_prev_count++;
        }
        //保存播放过的16个序号,以便回溯
        play_num_list[cur_save_pt] = temp_num;
    }

    return temp_num;
}

//获取shuffle机制的上一个文件
//返回值: file_no (1~file_total)
uint16 fsel_shuffle_prev_file(void)
{
    uint16 temp_num;

    if (shuffle_total < eh_file_total_all)
    {
        //文件个数变多了(增加文件)，重新初始化
        fsel_init_shuffle(eh_file_total_all);
    }
    else
    {
        //重新赋值防止 file_total<shuffle_total的情况(文件删除)
        shuffle_total = eh_file_total_all;
    }

    if (shuffle_total == 0)
    {
        return 0;
    }
    //如果已经存放了可回溯文件,则从保存列表中查找
    while (cur_save_prev_count > 1)
    {
        temp_num = get_prevlist_num();

        if (temp_num <= shuffle_total)
        {
            if (temp_num != 0)
            {
                return temp_num;
            }
            else
            {
                break;
            }
        }

        cur_save_prev_count--;
    }

    //list can't find, use shuffle al
    //保证随机出来的值在shuffle_total范围内,并与file_no不相同
    do
    {
        temp_num = get_shuffle_num();
        if (temp_num == 0)
        {
            break;
        }

    } while ((filtrate_shuffle_no == temp_num) || (temp_num > shuffle_total));

    if (0 != temp_num)
    {
        //SAVE TO PLAY_NUM_LIST
        if (cur_save_pt == 0)
        {
            //循环buffer的方式存放
            cur_save_pt = SET_PREV_MAX - 1;
        }
        else
        {
            cur_save_pt--;
        }

        if (cur_save_next_count < SET_PREV_MAX)
        {
            cur_save_next_count++;
        }
        //保存播放过的16个文件
        play_num_list[cur_save_pt] = temp_num;

    }

    return temp_num;
}

