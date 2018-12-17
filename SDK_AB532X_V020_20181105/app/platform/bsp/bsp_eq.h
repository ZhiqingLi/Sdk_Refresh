#ifndef _BSP_EQ_H
#define _BSP_EQ_H

#define little_endian_read_16(buf, ofs)         *(uint16_t *)((uint8_t *)buf + (ofs))
#define little_endian_read_32(buf, ofs)         *(uint32_t *)((uint8_t *)buf + (ofs))
#define EQ_BUFFER_LEN                           (260+10)

#if EQ_MODE_EN
#define MUSIC_EQ_TBL_LEN                        6
#else
#define MUSIC_EQ_TBL_LEN                        1
#endif // EQ_MODE_EN

typedef struct {
    u32 *addr;
    u32 *len;
} eq_param;

typedef struct  {
    u8 remain   :   1;      //spp拼包标志
    u16 remian_ptr;         //拼包长度
    u16 rx_size;            //拼包长度
}eq_spp_cb_t;

typedef struct {
    u8 rx_type;
    eq_spp_cb_t eq_spp_cb;
}eq_dbg_cb_t;

extern eq_dbg_cb_t eq_dbg_cb;

void sys_set_eq(void);
void eq_parse_cmd(void);
void eq_dbg_init(void);
void bsp_eq_init(void);
#endif
