#ifndef API_PFF_H
#define API_PFF_H

#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET    0
#define SEEK_CUR    1

typedef struct {
    u32 file_number;
    u32 file_count;
    u32 file_total;
    u32 dir_deepth;
    char fname[13];
} pff_api_t;
extern pff_api_t pff_api;

bool pf_scan_music(void);
bool pf_get_nfile(u16 file_num);
bool fs_mount(void);
u16 get_music_total(void);
void fsdisk_callback_init(u8 dev_num);

#ifdef __cplusplus
}
#endif
#endif
