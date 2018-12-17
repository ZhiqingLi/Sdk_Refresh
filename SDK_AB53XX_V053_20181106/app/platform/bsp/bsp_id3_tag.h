#ifndef _BSP_ID3_TAG_H
#define _BSP_ID3_TAG_H

#define ID3v2_HEADER_SIZE   10
#define TAG_DAT_SIZE        32
typedef struct {
    u8 title[TAG_DAT_SIZE];           //歌曲名
    u8 artist[TAG_DAT_SIZE];          //歌手名
    u8 album[TAG_DAT_SIZE];           //专辑名
    u16 title_len;
    u16 artist_len;
    u16 album_len;
} id3_tag_t;
extern id3_tag_t id3_tag;

void get_mp3_id3_tag(void);
void get_wma_id3_tag(void);

#endif
