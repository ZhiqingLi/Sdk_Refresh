#ifndef _SPIFLASH_H
#define _SPIFLASH_H

void spiflash1_init(void);
void spiflash1_read(void *buf, u32 addr, u32 len);
void spiflash1_write(void *buf, u32 addr, u32 len);
u32 spiflash1_id_read(void);
void spiflash1_erase(u32 addr);
void spiflash1_erase_block(u32 addr);
bool is_exspiflash_online(void);

bool exspiflash_init(void);
void write_music_bin_to_spiflash(void);
extern void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));
#endif // _SPIFLASH_H
