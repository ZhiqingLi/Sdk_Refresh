#include "eh_id3.h"

#define UTF8_GET_SIZE  128
static uint8 temp_bank_bufA[UTF8_GET_SIZE] _BANK_DATA_ATTR_;
static uint8 temp_bank_bufB[UTF8_GET_SIZE] _BANK_DATA_ATTR_;

/******************************************************************************/
/*!
 * \par  Description:
 *	  utf8字符串转unicode字符串
 * \param[in]    src：源字符串指针
 * \param[out]   dest：目的字符串指针
 * \param[out]   size：转换成unicode的字节数
 * \return       none
 * \ingroup      string
 * \note
 *******************************************************************************/
static void utf8_to_utf16(uint8* src, uint16*dest, int16* size)
{
    uint8* pin = src;
    uint8 in_val;
    int16 resultsize = 0;
    uint8 t1, t2, t3;
    uint8 tmpval[2] =
        { 0, 0 };
    while (*pin != 0)
    {
        in_val = *pin;
        if (in_val <= 0x7f)
        {
            tmpval[0] = in_val;
            tmpval[1] = 0;
        }
        else if ((in_val & (0xff << 5)) == 0xc0) //高2bit 是否为1
        {
            t1 = in_val & ((uint8)0xff >> 3); //取低5bit
            pin++;
            in_val = *pin;
            if (in_val == 0)
            {
                break;
            }
            t2 = in_val & ((uint8)0xff >> 2); //取低6bit
            //(t1的低2bit 放在最高两bit,或上t2的低6bit)
            tmpval[0] = t2 | ((t1 & ((uint8)0xff >> 6)) << 6);
            tmpval[1] = t1 >> 2; //取低中间3bit
        }
        else if ((in_val & (0xff << 4)) == 0xe0)
        {
            t1 = in_val & ((uint8)0xff >> 3); //取低5bit
            pin++;
            in_val = *pin;
            if (in_val == 0)
            {
                break;
            }
            t2 = in_val & ((uint8)0xff >> 2); //取低6bit
            pin++;
            in_val = *pin;
            if (in_val == 0)
            {
                break;
            }
            t3 = in_val & ((uint8)0xff >> 2); //取低6bit

            //Little Endian
            //(t2低2bit 放在最高2bit,或上t3的低6bit);
            tmpval[0] = ((t2 & ((uint8)0xff >> 6)) << 6) | t3;
            //(t1 的低4bit放在字节的高4bit,或上t2的中间4bit)
            tmpval[1] = (t1 << 4) | (t2 >> 2);
        }
        else
        {

        }
        pin++;
        resultsize += 2;
        *dest = tmpval[1];
        *dest = *dest << 8;
        *dest |= tmpval[0];
        dest++;
    }
    resultsize += 2;
    *size = resultsize;
    *dest = 0;
}
/*! \endcond */

/******************************************************************************/
/*!
 * \par  Description:
 *	  utf8字符串转unicode字符串,填入saveaddr
 * \param[in]    dest_p：填入的指针结构体
 * \param[in]   read_size：将要读文件的长度
 * \param[in]   data_buf： 不为空，就重该buffer取数据，为空就从文件读数据
 * \return       真实读出的长度
 * \ingroup      string
 * \note
 *******************************************************************************/
uint32 id3_read_utf8_deal(id3_save_t* dest_p, uint32 read_size, uint8 *src_buf)
{
    uint32 ret_len;
    uint16 utf16_cn;

    if (read_size == 0)
    {
        return read_size;
    }

    if (read_size < UTF8_GET_SIZE)
    {
        ret_len = read_size;
    }
    else
    {
        ret_len = UTF8_GET_SIZE;
    }

    sys_os_sched_lock();

    if (src_buf != NULL)
    {
        libc_memcpy(temp_bank_bufB, src_buf, ret_len);
    }
    else
    {
        read_buf_data(temp_bank_bufB, ret_len);
    }

    utf8_to_utf16(temp_bank_bufB, (uint16*) temp_bank_bufA, &utf16_cn);

    //utf16_cn 包含结束符
    if ((utf16_cn == 2) || (dest_p->savelen == 0))
    {
        dest_p->saveaddr[0] = 0x00;
    }
    else
    {
        if (utf16_cn > (dest_p->savelen - 4))
        {
            utf16_cn = dest_p->savelen - 4;
        }

        libc_memcpy(dest_p->saveaddr + 2, temp_bank_bufA, (uint32 )utf16_cn);
        *(uint16*) (dest_p->saveaddr) = UNICODE_FLAG;
        *(uint16*) (dest_p->saveaddr + (utf16_cn & 0xFFFE) + 2) = 0x00;
    }

    sys_os_sched_unlock();

    return ret_len;

}

