#include "eh_id3.h"

#define AAX_ITEM_COUNT  3

static const char ftypString[] = "ftyp";
static const char moovString[] = "moov";
static const char mvhdString[] = "mvhd";
static const char trakString[] = "trak";
static const char udtaString[] = "udta";
static const char metaString[] = "meta";
static const char hdlrString[] = "hdlr";
static const char ilstString[] = "ilst";

static const char dataString[] = "data";
static const char tag_string[] = "namARTalb";
static const char pic_string[] = "covr";

//static uint32 frame_end_pos _BANK_DATA_ATTR_;//不能定义 BANK_DATA,会切BANK

static uint32 aax_get_frame_size(void);

/******************************************************************************/
/*
 * \par  Description: 计算frame的长度
 *
 * \param[in]

 * \param[out]

 * \return        长度，byte

 * \note
 *******************************************************************************/
static uint32 aax_get_frame_size(void)
{
    return ((uint32) key_buf[0]) * 0x1000000 + ((uint32) key_buf[1]) * 0x10000 + ((uint32) key_buf[2]) * 0x100
            + (uint32) key_buf[3];
}

/******************************************************************************/
/*
 * \par  Description:品牌字符串表
 *
 * \param[in]    check_str
 tab_str

 * \param[out]

 * \return       成功>0,失败<0

 * \note
 *******************************************************************************/

static int8 aax_check_tag(uint8 *check_str, uint8 *tab_str)
{
    uint8 i;

    for (i = 0; i < AAX_ITEM_COUNT; i++)
    {
        if (check_flag[i] == 1)
        {
            if (libc_memcmp(check_str, tab_str, 3) == 0)
            {
                check_flag[i] = 0;
                break;
            }
        }
        tab_str += 3;
    }

    if (i == AAX_ITEM_COUNT)
    {
        return -1;
    }

    //设置将要保存的地址和长度
    id3_save_p->saveaddr = (void*) *((uint32*) id3_info_p + i);
    id3_save_p->savelen = *((uint8*) id3_info_p + 24 + i);

    return i;
}

/******************************************************************************/
/*
 * \par  Description:查找pic标记
 *
 * \param[in]

 * \param[out]

 * \return        成功>0,失败<0

 * \note
 *******************************************************************************/
static int8 aax_check_pic_flag(uint8 *buf)
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

static void _get_sub2(uint32 *p_frame_size, uint32 *p_str_length, uint8 *p_num)
{
    read_buf_data(key_buf, 8);
    *p_frame_size = *p_frame_size - 8;
    if (libc_memcmp(&key_buf[4], dataString, 4) == 0)
    {
        *p_str_length = aax_get_frame_size() - 8;
        (*p_num)++;
        if (id3_save_p->savelen != 0)
        {
            //reset_file_pos(8, 0);
            read_buf_data(key_buf, 8);
            *p_str_length = *p_str_length - 8;

            if (key_buf[3] == 1)
            {
                //utf8

                *p_str_length = id3_read_utf8_deal(id3_save_p, *p_str_length, NULL);

                //if (*p_str_length > ((uint32) id3_save_p->savelen - 4))
                //{
                //  *p_str_length = id3_save_p->savelen - 4;
                //}
                //
                //read_buf_data(id3_save_p->saveaddr + 3, *p_str_length);
                //*(id3_save_p->saveaddr + *p_str_length + 3) = 0x00;
                //id3_save_p->saveaddr[0] = UTF8_FLAG_0;
                //id3_save_p->saveaddr[1] = UTF8_FLAG_1;
                //id3_save_p->saveaddr[2] = UTF8_FLAG_2;
            }
            else if (key_buf[3] == 2)
            {
                //unicode
                if (*p_str_length > ((uint32) id3_save_p->savelen - 4))
                {
                    *p_str_length = id3_save_p->savelen - 4;
                }
                *(uint16 *)(id3_save_p->saveaddr) = UNICODE_FLAG;
                read_buf_data(id3_save_p->saveaddr + 2, *p_str_length);
                *(uint16 *)(id3_save_p->saveaddr + (*p_str_length & 0xFFFE) + 2) = 0x00;
            }
            else
            {
                //asc
                if (*p_str_length > ((uint32) id3_save_p->savelen - 1))
                {
                    *p_str_length = id3_save_p->savelen - 1;
                }
                read_buf_data(id3_save_p->saveaddr, *p_str_length);
                *(id3_save_p->saveaddr + *p_str_length) = 0x00;
            }

            *p_frame_size = *p_frame_size - 8 - *p_str_length;
        }

        if (*p_num > AAX_ITEM_COUNT)
        {
            return;
        }
    }

    if (*p_frame_size > 0)
    {
        reset_file_pos(*p_frame_size, 0);
    }
}

/******************************************************************************/
/*
 * \par  Description:获取aax子程序
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool get_id3_aax_sub(uint32 end_pos_param)
{
    uint32 str_length;
    uint32 frame_size;

    //uint8 *copy_addr;
    int8 result;
    //uint8 temp_num = 0;
    uint8 num = 0;

    while (id3_pos_file < end_pos_param)
    {
        read_buf_data(key_buf, 8);
        frame_size = aax_get_frame_size() - 8;
        result = aax_check_tag(&key_buf[5], (uint8 *) &tag_string);
        if (result != (int8) -1)
        {
            _get_sub2(&frame_size, &str_length, &num);

            if (num > AAX_ITEM_COUNT)
            {
                break;
            }
        }
        else if ((1 == id3_info_p->apic_flag) && (0 == libc_memcmp(&key_buf[4], pic_string, 4)))
        {
            num++;
            read_buf_data(key_buf, 64);
            result = aax_check_pic_flag(key_buf);

            if (result != -1)
            {
                id3_info_p->apic_offset = id3_pos_file + (uint32)result - 64;
                id3_info_p->apic_type = 1;
            }

            if (num > AAX_ITEM_COUNT)
            {
                break;
            }
        }
        else
        {
            reset_file_pos(frame_size, 0);
        }
    }

    if (num > 0)
    {
        return TRUE;
    }

    return FALSE;
}

/******************************************************************************/
/*
 * \par  Description:解析aax类型的id3信息
 *
 * \param[in]

 * \param[out]

 * \return        TRUE OR FALSE

 * \note
 *******************************************************************************/
bool get_id3_aax(void)
{
    uint32 tag_size, frame_end_pos;
    uint32 frame_len;
    uint8 num = 0;

    get_init();
    read_buf_data(key_buf, 8);
    if (libc_memcmp(&key_buf[4], ftypString, 4) == 0)
    {
        frame_len = aax_get_frame_size();
        reset_file_pos(frame_len - 8, 0);
    }
    else
    {
        return FALSE;
    }

    read_buf_data(key_buf, 8);
    if (libc_memcmp(&key_buf[4], moovString, 4) == 0)
    {
        tag_size = aax_get_frame_size();
        frame_end_pos = tag_size + id3_pos_file - 8;

        while ((id3_pos_file < frame_end_pos) && (num == 0))
        {
            read_buf_data(key_buf, 8);
            if (libc_memcmp(&key_buf[4], udtaString, 4) == 0)
            {
                tag_size = aax_get_frame_size();
                frame_end_pos = tag_size + id3_pos_file - 8;
                num++;
            }
            else
            {
                frame_len = aax_get_frame_size();
                reset_file_pos(frame_len - 8, 0);
            }
        } //while1

        while ((id3_pos_file < frame_end_pos) && (num == 1))
        {
            read_buf_data(key_buf, 12);
            if (libc_memcmp(&key_buf[4], metaString, 4) == 0)
            {
                tag_size = aax_get_frame_size();
                frame_end_pos = tag_size + id3_pos_file - 12;
                num++;
            }
            else
            {
                frame_len = aax_get_frame_size();
                reset_file_pos(frame_len - 12, 0);
            }
        } //while2

        while ((id3_pos_file < frame_end_pos) && (num == 2))
        {
            read_buf_data(key_buf, 8);
            if (libc_memcmp(&key_buf[4], ilstString, 4) == 0)
            {
                tag_size = aax_get_frame_size();
                frame_end_pos = tag_size + id3_pos_file - 8;
                num++;
            }
            else
            {
                frame_len = aax_get_frame_size();
                reset_file_pos(frame_len - 8, 0);
            }
        } //while3

        if (num == 3)
        {
            return get_id3_aax_sub(frame_end_pos);
        }

    }

    return FALSE;
}

