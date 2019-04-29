//#include "ff.h"
#if (CONFIG_APP_MP3PLAYER == 1)
uint16 get_musicfile_count(void);
uint16 get_curDirMusicfile_count(uint16 curDir);
FILE_INFO *get_file_info(void);

FRESULT fat_malloc_files_buffer(void);
FRESULT fat_free_files_buffer(void);
#endif
