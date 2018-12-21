#ifndef _LCD_H
#define _LCD_H

#define LCD_WIDTH               128
#define LCD_HEIGHT              64

#define COLUMN_MAX              LCD_WIDTH           //屏的列数
#define LINE_MAX                LCD_HEIGHT/8        //屏的行数

#define LCD_CONTRAST_DEFAULT    7

//LCD command definition
#define LCD_ON                  0xaf
#define LCD_OFF                 0xae
#define LCD_ALL_ON              0xa5
#define LCD_ALL_NORMAL          0xa4
#define LCD_START_LINE          0x40
#define LCD_PAGE_ADDR           0xb0
#define LCD_COLUMN_HIGH         0x10
#define LCD_COLUMN_LOW          0x00
#define LCD_ADC_SEL             0xa0                //bit0 0=normal,1=reverse
#define LCD_DISP_MODE           0xa6                //bit0 0=normal,1=reverse
#define LCD_BIAS_SET            0xa2
#define LCD_END                 0xee
#define LCD_SOFT_RST            0xe2
#define LCD_COMMON_MODE         0xc8                //bit3 0=normal,1=reverse
#define LCD_POWER_MODE          0x2f
#define LCD_V0_RATIO            0x20
#define LCD_ELE_VOLUME          0x81
#define LCD_STATIC_IND_ON       0xad
#define LCD_STATIC_IND_OFF      0xac
#define LCD_PAGE_BLINK          0xd5
#define LCD_DRIV_MODE           0xd2
#define LCD_NOP                 0xe3
#define LCD_OSC_SET             0xe4

#define LINE_CENTER             0x7FFF

#define DN_ZERO         0x80                        //左边填0
#define DN_RIGHT        0x40                        //左对齐
#define DN_BIG          0x20                        //大号字
#define DN_SMALL        0x10                        //小号字
#define DN_SSMALL       0x08                        //加小号字
#define DN_BITS_MASK    0x07                        //低3bit表示数值的最大位数限定

#define MID_NUM_SIZE    16
#define MID_NUM_WIDTH   8
#define FONT_WIDTH      14                          //字高12
//#define FONT_WIDTH      16                          //字高16

//字符移动显示控制
typedef struct {
    u8 en;
volatile u8 ms_cnt;      //100ms计数
    u16 wide;       //需要显示字符串点阵宽度
    s16 x;          //偏移位置
} nmv_cb_t;
extern nmv_cb_t nmv_cb;

extern u8 lcd_buf[LINE_MAX * COLUMN_MAX];

void lcd_spi_send_cmd(u8 cmd);
void lcd_buf_clear(void);
void lcd_screen_clear(void);
void lcd_init(void);

void lcd_line_clear(u16 y);
void lcd_line_update(u16 y, u8 num);
char *lcd_disp_str(char *str, s16 x, u16 y, u8 mode);
char *lcd_draw_text(char *str, s16 x, u16 y, u8 mode);
void lcd_draw_image(u32 addr, u32 index, u16 x, u16 y);
void lcd_disp_image(u32 addr, u32 index, u16 x, u16 y);
void lcd_disp_num(u16 num, u16 x, u16 y, u8 mode);
void lcd_disp_medium_time(u8 min, u8 sec, u8 x, u8 y);
void lcd_disp_medium_atime(u8 hour, u8 min, u8 sec, u8 x, u8 line);
void lcd_disp_freq_spectrum_line(u8 y);
void lcd_disp_msgbox(char *title, u32 index);
void lcd_disp_progressbox(char *title, u32 value, u32 min, u32 max);
void lcd_draw_menu(u32 index, u16 x, u8 y);
void lcd_disp_invert(u8 x, u8 y, u8 width, u8 n);

u16 get_name_str_width(char *str);
void utf16_convert_to_gbk(char *str, int code_len);

#endif // _LCD_H
