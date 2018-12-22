#include "include.h"

#if MUSIC_ID3_TAG_EN
id3_tag_t id3_tag AT(.mp3_id3_buf);

extern u8 wma_title[128];
extern u8 wma_artist[128];

extern u8 mp3_bitpool[1030];
extern unsigned char avio_buf[556];
bool mp3_id3v2_match(u8 *buf);
u32 mp3_id3v2_tag_len(u8 *buf);

static u16 get_tag_data_ansi(u8 *out, u8 *in, u16 len)
{
    int i, cnt = 0;
    for (i = 0; i < len; i++) {
        if (in[i] == 0) {
            break;
        } else if (in[i] & 0x80) {
#if GUI_LCD_EN
            out[cnt++] = in[i++];
            out[cnt++] = in[i];
#else
            //中文字符不支持, 每个中文字符占两字节
            out[cnt++] = '-';
            i++;
#endif
        } else {
            out[cnt++] = in[i];
        }
        if (cnt == (TAG_DAT_SIZE - 2)) {
            break;
        }
    }
    out[cnt++] = 0;
    out[cnt++] = 0;
    return cnt;
}

static u16 get_tag_data_utf16(u8 *out, u8 *in, u16 len)
{
    int i, cnt = 0;
    for (i = 0; i < len; i += 2) {
        if ((in[i] == 0) && (in[i + 1] == 0)) {
            break;
        } else if ((in[i] & 0x80) || (in[i + 1] > 0)) {
#if GUI_LCD_EN
            u16 code;
            //unicode convert to gbk
            code = GET_LE16(&in[i]);
            code = convert_uni2gbk(code, RES_BUF_FONT_UTF2GBK_DAT, RES_LEN_FONT_UTF2GBK_DAT);
            out[cnt++] = (u8)(code >> 8);
            out[cnt++] = (u8)code;
#else
            //中文字符不支持, utf-16中文字符占两字节
            out[cnt++] = '-';
#endif
        } else {
            out[cnt++] = in[i];
        }
        if (cnt == (TAG_DAT_SIZE - 2)) {
            break;
        }
    }
    out[cnt++] = 0;
    out[cnt++] = 0;
    return cnt;
}

static u16 get_tag_data_utf16_be(u8 *out, u8 *in, u16 len)
{
    int i, cnt = 0;
    for (i = 0; i < len; i += 2) {
        if ((in[i] == 0) && (in[i + 1] == 0)) {
            break;
        } else if ((in[i+1] & 0x80) || (in[i] > 0)) {
#if GUI_LCD_EN
            u16 code;
            //unicode convert to gbk
            code = GET_BE16(&in[i]);
            code = convert_uni2gbk(code, RES_BUF_FONT_UTF2GBK_DAT, RES_LEN_FONT_UTF2GBK_DAT);
            out[cnt++] = (u8)(code >> 8);
            out[cnt++] = (u8)code;
#else
            //中文字符不支持, utf-16中文字符占两字节
            out[cnt++] = '-';
#endif
        } else {
            out[cnt++] = in[i + 1];
        }
        if (cnt == (TAG_DAT_SIZE - 2)) {
            break;
        }
    }
    out[cnt++] = 0;
    out[cnt++] = 0;
    return cnt;
}

static u16 get_tag_data_utf8(u8 *out, u8 *in, u16 len)
{
    int i, cnt = 0;
    u8 char_size;


    for (i = 0; i < len; ) {
        char_size = utf8_char_size(in[i]);
        if (char_size == 1) {
            //英文字母占1字节
            out[cnt++] = in[i++];
        } else {
#if GUI_LCD_EN
            u16 code;
            code = utf8_convert_to_unicode(&in[i], char_size);
            code = convert_uni2gbk(code, RES_BUF_FONT_UTF2GBK_DAT, RES_LEN_FONT_UTF2GBK_DAT);
            out[cnt++] = (u8)(code >> 8);
            out[cnt++] = (u8)code;
#else
            //非英文字母占2~6字节
            out[cnt++] = '-';
#endif
            i += char_size;
        }
        if (cnt == (TAG_DAT_SIZE - 2)) {
            break;
        }
    }
    out[cnt++] = 0;
    out[cnt++] = 0;
    return cnt;
}

static u8 get_id3v2_tag(u8 *buf)
{
    u8 *ptr;
    u32 tag_len = 0, got_flag = 0;
    u32 size = mp3_id3v2_tag_len(buf);

    printf("got id3v2: %d\n", size);

    stream_read(&buf[512], 512);
    if (size > 1024) {
        size = 1024;
    }
    size -= ID3v2_HEADER_SIZE;
    ptr   = buf + ID3v2_HEADER_SIZE;

    while (size) {
        if (*ptr == 0) {
            return 0;
        }

        tag_len = GET_BE32(&ptr[4]) + 10;           //每帧长度
        if (tag_len < 11) {
            return 0;                               //帧内容的大小，不包括帧头，不得小于1
        }
        if (memcmp(ptr, "TIT2", 4) == 0) {
            if (ptr[10] == 0x01) {
                if ((ptr[11] == 0xff) && (ptr[12] == 0xfe)) {
                    //小端unicode： 0x01, 0xff, 0xfe开头
                    id3_tag.title_len = get_tag_data_utf16(id3_tag.title, &ptr[13], tag_len - 13);
                } else {
                    //大端unicode： 0x01, 0xfe, 0xff开头
                    id3_tag.title_len = get_tag_data_utf16_be(id3_tag.title, &ptr[13], tag_len - 13);
                }
            } else if (ptr[10] == 0x03) {
                //utf-8
                id3_tag.title_len = get_tag_data_utf8(id3_tag.title, &ptr[11], tag_len - 11);
            } else {
                id3_tag.title_len = get_tag_data_ansi(id3_tag.title, &ptr[11], tag_len - 11);
            }
            got_flag |= BIT(0);
            printf("Title(%d) : %s\n", id3_tag.title_len, id3_tag.title);
        } else if (memcmp(ptr, "TPE1", 4) == 0) {
            if (ptr[10] == 0x01) {
                if ((ptr[11] == 0xff) && (ptr[12] == 0xfe)) {
                    //小端unicode： 0x01, 0xff, 0xfe开头
                    id3_tag.artist_len = get_tag_data_utf16(id3_tag.artist, &ptr[13], tag_len - 13);
                } else {
                    //大端unicode： 0x01, 0xfe, 0xff开头
                    id3_tag.artist_len = get_tag_data_utf16_be(id3_tag.artist, &ptr[13], tag_len - 13);
                }
            } else if (ptr[10] == 0x03) {
                //utf-8
                id3_tag.artist_len = get_tag_data_utf8(id3_tag.artist, &ptr[11], tag_len - 11);
            } else {
                id3_tag.artist_len = get_tag_data_ansi(id3_tag.artist, &ptr[11], tag_len - 11);
            }
            got_flag |= BIT(1);
            printf("Artist(%d): %s\n", id3_tag.artist_len, id3_tag.artist);
        } else if (memcmp(ptr, "TALB", 4) == 0) {
            if (ptr[10] == 0x01) {
                if ((ptr[11] == 0xff) && (ptr[12] == 0xfe)) {
                    //小端unicode： 0x01, 0xff, 0xfe开头
                    id3_tag.album_len = get_tag_data_utf16(id3_tag.album, &ptr[13], tag_len - 13);
                } else {
                    //大端unicode： 0x01, 0xfe, 0xff开头
                    id3_tag.album_len = get_tag_data_utf16_be(id3_tag.album, &ptr[13], tag_len - 13);
                }
            } else if (ptr[10] == 0x03) {
                //utf-8
                id3_tag.album_len = get_tag_data_utf8(id3_tag.album, &ptr[11], tag_len - 11);
            } else {
                id3_tag.album_len = get_tag_data_ansi(id3_tag.album, &ptr[11], tag_len - 11);
            }
            got_flag |= BIT(2);
            printf("Album(%d) : %s\n", id3_tag.album_len, id3_tag.album);
        }

        ptr += tag_len;
        if (size > tag_len) {
            size -= tag_len;
        } else {
            size = 0;
        }
        if ((got_flag & 0x07) == 0x07) {
            break;
        }
    }
    return 1;
}

static void get_id3v1_tag(void)
{
    u8 *ptr = avio_buf, *buf = mp3_bitpool;
    u32 fsize = fs_get_file_size();
    u32 sects = fsize >> 9;
    u32 rest_len = fsize - (sects << 9);
    u32 clen;

    if (rest_len >= 128) {
        stream_seek(sects, SEEK_SET);
        stream_read(buf, 512);
        memcpy(ptr, buf + rest_len - 128, 128);
    } else {
        stream_seek(sects - 1, SEEK_SET);
        stream_read(buf, 512);
        clen = 128 - rest_len;
        memcpy(ptr, buf + 512 - clen, clen);
        stream_read(buf, 512);
        memcpy(ptr + clen, buf, rest_len);
    }

    if ((memcmp(ptr, "TAG", 3) != 0) || (ptr[3] == 0)) {
        return;
    }

    id3_tag.title_len  = get_tag_data_ansi(id3_tag.title,  &ptr[3],  30);
    id3_tag.artist_len = get_tag_data_ansi(id3_tag.artist, &ptr[33], 30);
    id3_tag.album_len  = get_tag_data_ansi(id3_tag.album,  &ptr[63], 30);

    printf("got id3v1\n");
    printf("Title(%d) : %s\n", id3_tag.title_len, id3_tag.title);
    printf("Artist(%d): %s\n", id3_tag.artist_len, id3_tag.artist);
    printf("Album(%d) : %s\n", id3_tag.album_len, id3_tag.album);
}

//获取MP3的ID3V2或ID3V1
void get_mp3_id3_tag(void)
{
    u8 *buf = mp3_bitpool;
    u8 id3v2_ok = 0;

    memset(&id3_tag, 0, sizeof(id3_tag));
    stream_seek(0, SEEK_SET);
    stream_read(buf, 512);
    if (mp3_id3v2_match(buf)) {
        id3v2_ok = get_id3v2_tag(buf);
    }
    if (!id3v2_ok) {
        get_id3v1_tag();
    }
    stream_seek(0, SEEK_SET);
}

//获取wma的ID3
void get_wma_id3_tag(void)
{
    memset(&id3_tag, 0, sizeof(id3_tag));
    id3_tag.title_len = get_tag_data_utf16(id3_tag.title, wma_title, 128);
    id3_tag.artist_len = get_tag_data_utf16(id3_tag.artist, wma_artist, 128);

    if (id3_tag.title_len || id3_tag.artist_len) {
        printf("\ngot wma id3\n");
    }
    if (id3_tag.title_len) {
        printf("Title(%d) : %s\n", id3_tag.title_len, id3_tag.title);
    }
    if (id3_tag.artist_len) {
        printf("Artist(%d): %s\n", id3_tag.artist_len, id3_tag.artist);
    }
}
#endif
