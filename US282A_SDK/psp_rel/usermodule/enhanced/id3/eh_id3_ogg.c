#include "eh_id3.h"

#define OGG_ITEM_COUNT   4

static const char item_key_tab[OGG_ITEM_COUNT][8] =
{ "TITLE=", "ARTIST=", "ALBUM=", "GENRE=" };
static const uint8 item_key_len[OGG_ITEM_COUNT] =
{ 6, 7, 6, 6 };
static const char ogg_file_tag[] =
{ "OggS" };

////#pragma memory=constseg(MOC_EH_OGGTAG)
static const uint8 ogg_taginfo_keyword[8] =
    { 0x07, 0x03, 0x56, 0x4f, 0x52, 0x42, 0x49, 0x53 }; // 0x03+"VORBIS"

////#pragma memory=dataseg(MOD_EH_OGGTAG)

//不能定义 BANK_DATA,会切BANK
//static uint32 tag_info_len _BANK_DATA_ATTR_;//TAG信息的长度
//static uint32 tag_info_count _BANK_DATA_ATTR_;//TAG信息的项数

static int8 ogg_check_tag(char *buf);

/******************************************************************************/
/*
 * \par  Description: 获取item的id3信息
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
//解析 VORBIS_COMMENT 结构的ID3
bool ogg_get_items(uint32 tag_count)
{
    uint32 tag_info_len;
    int8 sn;
    uint8 find_count;

    find_count = 0; //个数从0开始

    while (tag_count > 0)
    {
        tag_count--;
        if (FALSE == read_buf_data((uint8*) &tag_info_len, 4))
        {
            return FALSE;
        }

        if (tag_info_len == 0)
        {
            continue;
        }

        if (!read_buf_data(key_buf, tag_info_len))
        {
            return FALSE;
        }

        sn = ogg_check_tag(key_buf);

        if (sn != (int8) (-1))
        {
            find_count++;
            if (id3_save_p->savelen == 0)
            {
                continue;
            }
            else
            {
                tag_info_len -= item_key_len[sn]; //去掉标识符

                tag_info_len = id3_read_utf8_deal(id3_save_p, tag_info_len, key_buf + item_key_len[sn]);

                //if (((uint32) id3_save_p->savelen - 4) < tag_info_len)
                //{
                //  tag_info_len = id3_save_p->savelen - 4;
                //}
                //
                //libc_memcpy(id3_save_p->saveaddr + 3, key_buf + item_key_len[sn], tag_info_len);
                //
                //id3_save_p->saveaddr[0] = UTF8_FLAG_0;
                //id3_save_p->saveaddr[1] = UTF8_FLAG_1;
                //id3_save_p->saveaddr[2] = UTF8_FLAG_2;
                //id3_save_p->saveaddr[tag_info_len + 3] = '\0';
            }

            if (find_count >= OGG_ITEM_COUNT)
            {
                break;
            }
        }

    }

    if (find_count > 0)
    {
        return TRUE;
    }

    return FALSE;

}

/******************************************************************************/
/*
 * \par  Description: 查找指定的字符串
 *
 * \param[in]    mem_buf--查找字符串
 sectors--搜索的扇区个数
 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool ogg_search_key(const char *mem_buf, uint32 sectors)
{
    char key;
    uint16 j = 0;

    id3_pos_file = 0;
    while (sectors > 0)
    {
        sectors--;
        while (id3_pos_buffer < SECTOR_SIZE)
        {
            key = (char) id3_temp_buf[id3_pos_buffer];
            id3_pos_buffer++;

            if ((key >= 'a') && (key <= 'z'))
            //if (check_char_type(key) == CHAR_TYPE_DNER)
            {
                key = key - 0x20; //变为大写来处理
            }

            if (key != mem_buf[j + 1]) //因为第一个BUFF是数组的长度
            {
                j = 0;
            }
            else
            {
                j++;
            }

            if (j >= mem_buf[0])
            {
                //是否全部查找完
                id3_pos_file = id3_pos_file + id3_pos_buffer;
                return TRUE; //--->> TRUE
            }

        }

        id3_pos_file += SECTOR_SIZE;
        id3_pos_buffer = 0;
        vfs_file_read(eh_vfs_mount, id3_temp_buf, SECTOR_SIZE, id3_handle);
    }
    return FALSE;

}

/******************************************************************************/
/*
 * \par  Description:匹配字符串
 *
 * \param[in]    buf

 * \param[out]

 * \return      成功>0,失败<0

 * \note
 *******************************************************************************/
static int8 ogg_check_tag(char *buf)
{
    uint8 i;
    if (str_to_capital(buf, '=') != 0)
    {
        for (i = 0; i < OGG_ITEM_COUNT; i++)
        {
            if (check_flag[i] == 1)
            {
                if (libc_memcmp(buf, &item_key_tab[i], (uint32) item_key_len[i]) == 0)
                {
                    check_flag[i] = 0;
                    //设置将要保存的地址和长度
                    id3_save_p->saveaddr = (void*) *((uint32*) id3_info_p + i);
                    id3_save_p->savelen = *((uint8*) id3_info_p + 24 + i);
                    return i;
                }
            }
        }
    }

    return -1;
}

/******************************************************************************/
/*
 * \par  Description:解析ogg类型的id3信息
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool get_id3_ogg(void)
{
    uint32 tag_info_len, tag_info_count;
    bool result;

    get_init();
    read_buf_data(key_buf, 4);

    if (libc_memcmp(key_buf, ogg_file_tag, 4) != 0)
    {
        return FALSE;
    }

    //查找过程 是否可优化？
    result = ogg_search_key(ogg_taginfo_keyword, 0x08);

    if (FALSE == result)
    {
        return FALSE;
    }

    //读第一项的长度
    read_buf_data((uint8*) &tag_info_len, 4);
    //这一项数据不要,跳过
    reset_file_pos(tag_info_len, 0);

    //读信息的总项数
    read_buf_data((uint8*) &tag_info_count, 4);

    if (tag_info_count == 0)
    {
        return FALSE;
    }

    //读取ID3信息
    return ogg_get_items(tag_info_count);

}

