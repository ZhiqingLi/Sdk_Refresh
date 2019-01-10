#ifndef __RBPLAYSD_H_
#define __RBPLAYSD_H_

#include <stdint.h>

//playlist
#define FILE_PATH_LEN (128+4)
#define FILE_SHORT_NAME_LEN (128)
#define MP3_TITLE_LEN (128)
#define MP3_ARTIST_LEN (128)
#define MP3_ALBUM_LEN (128)
#define MP3_GENRE_LEN (128)
#define MP3_COMPOSER_LEN (128)
#define MP3_YEAR_STRING_LEN (8)
#define MP3_TRACK_LEN  (128)

typedef struct {
    char file_path[FILE_PATH_LEN];
    char file_name[FILE_SHORT_NAME_LEN];
#ifdef PARSER_DETAIL
    uint16_t  title[MP3_TITLE_LEN];
    uint16_t  artist[MP3_ARTIST_LEN];
    uint16_t  album[MP3_ALBUM_LEN];
    uint16_t  genre[MP3_GENRE_LEN];
    uint16_t  composer[MP3_COMPOSER_LEN];
    uint16_t  year[MP3_YEAR_STRING_LEN];
    uint16_t  track[MP3_TRACK_LEN];
#endif
    uint16_t song_idx;

    unsigned int codectype;
    unsigned int bitrate;
    unsigned long frequency;

    unsigned long filesize; /* without headers; in bytes */
    unsigned long length;   /* song length in ms */
} playlist_item;

typedef struct {
    playlist_item *current_item;
    uint16_t      total_songs;
} playlist_struct;

void app_rbplay_load_playlist(playlist_struct *list);
playlist_item *app_rbplay_get_playitem(const int idx);
int app_ctl_remove_file(const int idx);

#endif
