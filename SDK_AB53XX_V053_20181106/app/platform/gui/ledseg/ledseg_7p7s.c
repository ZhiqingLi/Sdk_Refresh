#include "include.h"
#include "ledseg_common.h"
#include "ledseg_7p7s.h"

#if (GUI_SELECT == GUI_LEDSEG_7P7S)

//7COM7SEG Control Block
typedef struct {
    u8 buf[7];
    u8 seg_cnt;
    u8 com_cnt;
    volatile u8 disp_en;            //中断是否刷屏
} ledseg_cb_t;
ledseg_cb_t ledseg_cb AT(.buf.ledseg.cb);

void ledseg_ajust(uint disp_seg);
void ledseg_7p7s_set(uint8_t seg_bits, uint8_t com_pin);
int s_bcnt(int rs1);    //return number of bits set in rs1 (number of '1')

AT(.text.ledseg.init)
void ledseg_7p7s_init(void)
{
    ledseg_7p7s_clr();
    memset(&ledseg_cb, 0, sizeof(ledseg_cb));
    memset(ledseg_buf, 0, sizeof(ledseg_buf));
    ledseg_disp_num = 0xff;
}

AT(.text.ledseg)
void ledseg_7p7s_update_dispbuf_do(u8 *disp_buf)
{
    ledseg_cb.disp_en = 0;
    memcpy(ledseg_cb.buf, disp_buf, sizeof(ledseg_cb.buf));
    ledseg_cb.disp_en = 1;
}

//按COM扫描, 1ms扫描间隔
AT(.com_text.ledseg)
void ledseg_7p7s_scan(void)
{
    u8 disp_seg;
    u8 com_cnt;

    if (ledseg_7p7s_reuse_hook()) {     //LED复用
        return;
    }

    if (ledseg_cb.disp_en == 0) {
        return;
    }

    ledseg_7p7s_scan_hook(ledseg_cb.buf);
    disp_seg = ledseg_cb.buf[ledseg_cb.com_cnt];
    com_cnt = ledseg_cb.com_cnt;
    ledseg_cb.com_cnt++;
    if (ledseg_cb.com_cnt > 6) {
        ledseg_cb.com_cnt = 0;
    }

    ledseg_ajust(disp_seg);
    ledseg_7p7s_set(disp_seg, com_cnt);
}

#endif  // #if (GUI_SELECT == GUI_LEDSEG_7P7S)

