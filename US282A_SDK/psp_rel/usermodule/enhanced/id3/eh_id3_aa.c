#include "eh_id3.h"

#define AA_ITEM_COUNT   4

typedef struct
{
    uint32 address;
    uint32 len;
} tag_item_t;

static const char item_key_tab[AA_ITEM_COUNT][7] =
{ "title", "author", "album", "genre" }; //
static const uint8 item_key_len[AA_ITEM_COUNT] =
{ 5, 6, 5, 5 };

//static tag_item_t tag_item_p _BANK_DATA_ATTR_; //不能定义 BANK_DATA,会切BANK

/******************************************************************************/
/*
 * \par  Description:从数据buffer中查找指定的字符
 *
 * \param[in]    buffer--数据buffer
 buflength--查找的长度
 tag_t--返回信息
 tag_secid--指定字符

 * \param[out]

 * \return      TRUE OR FALSE

 * \note
 *******************************************************************************/
bool aa_find_tag(uint8* buffer, uint32 buflength, tag_item_t* tag_t, uint32 tag_secid)
{
    uint32 i;
    uint32 j;
    uint32 sec_id;

    tag_t->address = 0;
    tag_t->len = 0;

    for (i = 16; i < buflength; i += 12)
    {
        sec_id = (uint32) buffer[i] * 0x1000000 + (uint32) buffer[i + 1] * 0x10000 + (uint32) buffer[i + 2] * 0x100
                + (uint32) buffer[i + 3];
        if (sec_id == tag_secid)
        {
            j = i + 4;
            tag_t->address = (uint32) buffer[j] * 0x1000000 + ((uint32) buffer[j + 1]) * 0x10000 + ((uint32) buffer[j
                    + 2]) * 0x100 + ((uint32) buffer[j + 3]);
            j += 4;
            tag_t->len = (uint32) buffer[j] * 0x1000000 + ((uint32) buffer[j + 1]) * 0x10000 + ((uint32) buffer[j + 2])
                    * 0x100 + ((uint32) buffer[j + 3]);
            return TRUE;
        }
    }

    return FALSE;
}

/******************************************************************************/
/*
 * \par  Description:从buf中匹配字符串
 *
 * \param[in]    buf
 len--数据长度
 * \param[out]

 * \return        0无效,>0有效

 * \note
 *******************************************************************************/
int8 aa_check_tag(char *buf, uint32 len)
{
    uint8 i;

    for (i = 0; i < AA_ITEM_COUNT; i++)
    {
        if ((check_flag[i] == 1) && (len == item_key_len[i]))
        {
            if (libc_memcmp(buf, &item_key_tab[i], (uint32) len) == 0)
            {
                check_flag[i] = 0;
                //设置将要保存的地址和长度
                id3_save_p->saveaddr = (void*) *((uint32*) id3_info_p + i);
                id3_save_p->savelen = *((uint8*) id3_info_p + 24 + i);
                return i;
            }
        }
    }

    return -1;
}

/******************************************************************************/
/*
 * \par  Description:查找pic标记
 *
 * \return        成功>=0,失败<0

 * \note
 *******************************************************************************/
static int8 aa_check_pic_flag(uint8 *buf)
{
    uint8 a;
    for (a = 0; a < 63; a++)
    {
        if ((buf[a] == 0xff) && (buf[a + 1] == 0xd8))
        {
            return a;
        }
    }
    return -1;
}

/******************************************************************************/
/*
 * \par  Description: 获取picture
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
// 解析 PICTURE 结构
static bool aa_get_picture(void)
{
    tag_item_t tag_item_p;
    int8 result;

    //回到文件头
    vfs_file_seek(eh_vfs_mount, 0, SEEK_SET, id3_handle);
    get_init();
    //----读TAG头标志
    result = (int8)aa_find_tag(id3_temp_buf, 0xa0, &tag_item_p, 0x0B);
    if ((FALSE == result) || (tag_item_p.len == 0))
    {
        return FALSE;
    }

    //跳到结构位置
    reset_file_pos(tag_item_p.address, 0);
    read_buf_data(key_buf, 64);
    result = aa_check_pic_flag(key_buf);

    if (result != -1)
    {
        id3_info_p->apic_offset = id3_pos_file + (uint32)result - 64;
        id3_info_p->apic_type = 1;
        return TRUE;
    }
    return FALSE;

}

/******************************************************************************/
/*
 * \par  Description:解析aa类型的id3信息
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool get_id3_aa(void)
{
    uint8 result;
    uint8 find_count;
    uint32 t_itemcount;
    uint32 type_name_len;
    uint32 item_date_len;
    uint32 tmp_len;
    tag_item_t tag_item_p;

    get_init();
    find_count = 0;

    //----读TAG头标志

    result = aa_find_tag(id3_temp_buf, 0xa0, &tag_item_p, 0x02);

    if ((FALSE == result) || (tag_item_p.len == 0))
    {
        return FALSE;
    }

    //跳到结构位置
    reset_file_pos(tag_item_p.address, 0);
    read_buf_data(key_buf, 4);

    t_itemcount = (uint32) key_buf[0] * 0x1000000 + (uint32) key_buf[1] * 0x10000 + (uint32) key_buf[2] * 0x100
            + (uint32) key_buf[3];

    if (t_itemcount == 0)
    {
        return FALSE;
    }

    while (t_itemcount > 0)
    {
        t_itemcount--;
        if (id3_pos_file > (tag_item_p.address + tag_item_p.len)) //超出范围，退出
        {
            break;
        }

        read_buf_data(key_buf, 9);
        if (key_buf[0] != 0)
        {
            break; //end
        }

        type_name_len = (uint32) key_buf[1] * 0x1000000 + (uint32) key_buf[2] * 0x10000 + (uint32) key_buf[3] * 0x100
                + (uint32) key_buf[4];
        item_date_len = (uint32) key_buf[5] * 0x1000000 + (uint32) key_buf[6] * 0x10000 + (uint32) key_buf[7] * 0x100
                + (uint32) key_buf[8];

        if ((type_name_len > 4) && (type_name_len < 7))
        {
            read_buf_data(key_buf, type_name_len);
            if (aa_check_tag(key_buf, type_name_len) != (int8) -1)
            {
                find_count++;
                //                read_buf_data(key_buf,item_date_len);
                if ((id3_save_p->savelen == 0) || (item_date_len == 0))
                {
                    continue;
                }
#if 1
                //默认内码

                if (((uint32) id3_save_p->savelen - 1) < item_date_len)
                {
                    tmp_len = id3_save_p->savelen - 1;
                }
                else
                {
                    tmp_len = item_date_len;
                }

                read_buf_data(id3_save_p->saveaddr, tmp_len);
                id3_save_p->saveaddr[tmp_len] = '\0';

#else
                //UTF8

                tmp_len = id3_read_utf8_deal(id3_save_p,tmp_len,NULL);

                //if (((uint32) id3_save_p->savelen - 4) < item_date_len)
                //{
                //  tmp_len = id3_save_p->savelen - 4;
                //}
                //else
                //{
                //  tmp_len = item_date_len;
                //}
                //
                //read_buf_data(id3_save_p->saveaddr + 3, tmp_len);
                //id3_save_p->saveaddr[0] = UTF8_FLAG_0;
                //id3_save_p->saveaddr[1] = UTF8_FLAG_1;
                //id3_save_p->saveaddr[2] = UTF8_FLAG_2;
                //id3_save_p->saveaddr[tmp_len + 3] = '\0';
#endif

                if (tmp_len < item_date_len)
                {
                    reset_file_pos(item_date_len - tmp_len, 0);
                }

                if (find_count >= AA_ITEM_COUNT)
                {
                    break;
                }

            }
            else
            {
                reset_file_pos(item_date_len, 0);
            }

        }
        else
        {
            reset_file_pos(type_name_len + item_date_len, 0);
        }

    }

    //check picture

    if (1 == id3_info_p->apic_flag)
    {
        result = aa_get_picture();
    }

    if (find_count > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

