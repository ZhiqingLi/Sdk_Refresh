#include "include.h"
#include "lcd.h"

#if (GUI_SELECT == GUI_LCD)

nmv_cb_t nmv_cb AT(.buf.lcd);
u8 lcd_buf[LINE_MAX * COLUMN_MAX] AT(.buf.lcd);
u8 freq_spectrum_buf[16] AT(.buf.lcd);

AT(.text.lcd.table)
const u8 tbl_freq_spectrum0[15] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
AT(.text.lcd.table)
const u8 tbl_freq_spectrum1[15] = {0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


///SPI接口发送1Byte数据
AT(.text.lcd)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                                 //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                       //Wait pending
}

/****************************************************
描    述: 更新屏显示
入口参数: x(列位置), y(行位置)
出口参数: 无
****************************************************/
AT(.text.lcd)
void lcd_screen_update(u8 x, u8 y, u8 *buf, u16 len)
{
    if (y >= LINE_MAX) {
        return;
    }
    LCD_CMD_EN();
    LCD_SPI_CS_EN();
    spi1_sendbyte(LCD_COLUMN_HIGH | (x >> 4));
    spi1_sendbyte(LCD_COLUMN_LOW  | (x & 0x0f));
    spi1_sendbyte(LCD_PAGE_ADDR   |  y);
    LCD_SPI_CS_DIS();
    LCD_CMD_DIS();
    asm("nop"); asm("nop");

    LCD_SPI_CS_EN();
    SPI1DMAADR = DMA_ADR(buf);
    SPI1DMACNT = len;
	while (!(SPI1CON & BIT(16)));                       //waiting finish pending
/*    for (u16 i = 0; i < len; i++) {
        spi1_sendbyte(buf[i]);
    }*/
    LCD_SPI_CS_DIS();
}

///设置LCD屏的对比度
AT(.text.lcd)
void lcd_set_contrast(u8 val)
{
    if (val > 15) {
        val = 15;
    }
    lcd_spi_send_cmd(LCD_ELE_VOLUME);                   //electronic Volume
    lcd_spi_send_cmd(val);
}

AT(.text.lcd)
void lcd_spi_send_cmd(u8 cmd)
{
    LCD_CMD_EN();
    LCD_SPI_CS_EN();
    spi1_sendbyte(cmd);
    LCD_SPI_CS_DIS();
    LCD_CMD_DIS();
}

AT(.text.lcd)
void lcd_screen_clear(void)
{
    memset(lcd_buf, 0, sizeof(lcd_buf));
    for (u8 i = 0; i != 8; i++) {
        lcd_screen_update(0, i, &lcd_buf[i*COLUMN_MAX], COLUMN_MAX);
    }
}

AT(.text.lcd)
void lcd_buf_clear(void)
{
    memset(lcd_buf, 0, sizeof(lcd_buf));
}

AT(.text.lcd)
void lcd_line_clear(u16 y)
{
    memset(&lcd_buf[y*COLUMN_MAX], 0, 256);
}


AT(.text.lcd.init)
void lcd_init(void)
{
    printf("%s\n", __func__);
    memset(&nmv_cb, 0, sizeof(nmv_cb));
    lcd_spi_init();
    lcd_spi_send_cmd(0x40);                             //start line 0
    lcd_spi_send_cmd(LCD_BIAS_SET | 0x00);              //lcd bias setting
    lcd_spi_send_cmd(LCD_ADC_SEL);                      //invert display 0 or 1
    lcd_spi_send_cmd(LCD_COMMON_MODE & 0xc8);           //common output mode
    lcd_spi_send_cmd(LCD_V0_RATIO | 0x06);              //Resistor Ratio
    lcd_set_contrast(LCD_CONTRAST_DEFAULT);             //设置对比度
    lcd_spi_send_cmd(LCD_POWER_MODE);                   //lcd power control
    lcd_spi_send_cmd(LCD_ON);

    lcd_screen_clear();
}

/****************************************************
描    述: LCD行更新, 每次更新两行
入口参数: y(行号)
出口参数: 无
****************************************************/
AT(.text.lcd)
void lcd_line_update(u16 y, u8 num)
{
    int i;
    if (y >= (LINE_MAX - 1)) {
        return;
    }
    for (i = 0; i < num; i++) {
        lcd_screen_update(0, y, &lcd_buf[y * COLUMN_MAX], COLUMN_MAX);
        y++;
    }
}

//获取一个字的点阵数据，返回字宽。
AT(.text.lcd)
u8 get_char_dots_data(u8 *buf, u16 code, s16 x, u8 mode)
{
    u8 half_mode = mode & 0x01;
    u8 wide = FONT_WIDTH, w;
    u32 row, num;
    u32 addr;

    //printf("code: %02x\n", code);
    if (code < 256) {
        wide = 7;
        if ((code < 0x20) || (code > 0x7e)) {
            code = 0x20;
        }
        code -= 0x20;
        addr = (u32)RES_BUF_FONT_ASCII_DAT + code * wide * 2;
    } else {
        addr = (u32)RES_BUF_FONT_GBK_DAT;
        if ((code < 0xA1A1) || (code > 0xF7FE) || ((code > 0xA6FE) && (code < 0xB0A1))) {
            code = 0xA1F5; //找不到编码，使用'□'
        }
        row = (code >> 8) - 0xa0 - 1;
        if (row >= 15) {
            row -= 9;
        }
        num = (code & 0xff) - 0xa0 - 1;
        num = (row * 94 + num) * wide * 2;
        addr += num;
    }

    w = wide;
    if ((x + wide) > COLUMN_MAX) {
        w = half_mode ? (COLUMN_MAX - x) : 0;        //确定是否拷贝行末的半个字
    }
    if (w == 0) {
        return 0;
    }

    if (x < 0) {
        if (half_mode && ((x + wide) > 0)) {
            w = half_mode ? (x + wide) : 0;
            addr -= x;
            x = 0;
        }
    }
    if (x >= 0) {
        buf += x;
        memcpy(buf, (u8 *)addr, w);
        memcpy(buf+COLUMN_MAX, (u8 *)(addr + wide), w);
    }

    return wide;
}

AT(.text.lcd)
char *lcd_buf_put_gbk(u8 *buf, char *str, s16 x, u8 mode)
{
    u8 disp_center_flag = 0;
    u8 ch_len, font_w = 0, shift;
    u16 code;
    u8 *src, *dst;

    if (x == LINE_CENTER) {
        x = 0;
        disp_center_flag = 1;
    }

    while (*str && x < COLUMN_MAX) {
        if (*str & 0x80) {
            code = str[0];
            code = (code << 8) | str[1];
            ch_len = 2;
        } else  {
            code = *str;
            ch_len = 1;
        }
        font_w = get_char_dots_data(buf, code, x, mode);
        if (font_w == 0) {
            break;
        }
        x += font_w;
        str += ch_len;
    }

    //居中显示处理
    if (disp_center_flag) {
        src = buf + x - 1;
        shift = (COLUMN_MAX - x) / 2;
        dst = src + shift;
        while (x--) {
            *(dst + COLUMN_MAX) = *(src + COLUMN_MAX);
            *dst = *src;
            dst--;
            src--;
        }
        while (shift--) {
            *(dst + COLUMN_MAX) = 0;
            *dst = 0;
            dst--;
        }
    }
    return str;
}

///字符显示（中英文）：mode=0，不显示半字；mode=1，显示半字
AT(.text.lcd)
char *lcd_draw_text(char *str, s16 x, u16 y, u8 mode)
{
    return lcd_buf_put_gbk(&lcd_buf[y * COLUMN_MAX], str, x, mode);
}

///字符显示（中英文）：mode=0，不显示半字；mode=1，显示半字
AT(.text.lcd)
char *lcd_disp_str(char *str, s16 x, u16 y, u8 mode)
{
    char *rstr;
    rstr = lcd_buf_put_gbk(&lcd_buf[y * COLUMN_MAX], str, x, mode);
    lcd_line_update(y, 2);
    return rstr;
}

AT(.text.lcd)
void utf16_convert_to_gbk(char *str, int code_len)
{
    int i;
    char *out = str;
    u16 code;

    for (i = 0; i < code_len*2; i += 2) {
        if ((str[i] == 0) && (str[i + 1] == 0)) {
            break;
        }
        if (str[i+1] > 0) {
            code = GET_LE16(&str[i]);
            code = convert_uni2gbk(code, RES_BUF_FONT_UTF2GBK_DAT, RES_LEN_FONT_UTF2GBK_DAT);
            *out++ = (u8)(code >> 8);
            *out++ = (u8)code;
        } else {
            *out++ = str[i];        //英文
        }
    }
    *out++ = '\0';
}

AT(.text.lcd)
u16 get_name_str_width(char *str)
{
    u16 width = 0;
    while (*str != 0) {
        if (*(str+1) == 0) {
            width += FONT_WIDTH/2;
            str++;
        } else {
            width += FONT_WIDTH;
            str += 2;
        }
    }
    return width;
}

///图片显示
AT(.text.lcd)
void lcd_draw_image(u32 addr, u32 index, u16 x, u16 y)
{
    u8  width = (index >> 20) & 0xff;        //图片宽度 (像素点)
    u8  height  = (index >> 28) & 0x0f;      //图片高度 (像素点/8)
    u32 image_addr = addr + (index & 0xfffff);
    u8 *buf, i, len;

    if ((y + height) > 8) {
        return;
    }

    buf = &lcd_buf[x + COLUMN_MAX * y];
    len = ((x + width) > COLUMN_MAX) ? (COLUMN_MAX - x) : width;
    height = ((height + y) > LINE_MAX) ? (LINE_MAX - y) : height;
    for (i = 0; i < height; i++) {
        memcpy(buf, (u8 *)image_addr, len);
        image_addr += width;
        buf += COLUMN_MAX;
    }
}

///图片显示
AT(.text.lcd)
void lcd_disp_image(u32 addr, u32 index, u16 x, u16 y)
{
    u8  height  = (index >> 28) & 0x0f;      //图片高度 (像素点/8)
    lcd_draw_image(addr, index, x, y);
    lcd_line_update(y, height);
}

///显示数字的函数。默认为左对齐
AT(.text.lcd)
void lcd_disp_num(u16 num, u16 x, u16 y, u8 mode)
{
    u16 i;
    u8 n;
    u16 max_num = 1;
    u8 fill = 0, wid = 7;
    u32 index;
    u8 num_bits = mode & DN_BITS_MASK;          //数字最大位数
    const u32 *bmp_index = lcd_mnum_table;

    if (num_bits == 0 || num_bits > 4) {
        num_bits = 4;
    }
    n = num_bits;
    while (n--) {
        max_num *= 10;                          //计算数值区域
    }
    if (num >= max_num) {                       //限定最大值
        num = max_num - 1;
    }
    max_num /= 10;

    if (mode & DN_RIGHT) {
        if (mode & DN_ZERO) {
            fill = 1;                           //右对齐，并填充0
        }
    }
    if (mode & DN_SMALL) {
        bmp_index = lcd_snum_table;
        wid = 7;
    } else if (mode & DN_BIG) {
        bmp_index = lcd_bnum_table;
        wid = 16;
    } else if (mode & DN_SSMALL) {
        bmp_index = lcd_ssnum_table;
        wid = 6;
    }

    for (i = 0; i < num_bits; i++) {
        n = num / max_num;
        if (n || fill) {                        //当有值、填0、左对齐、个位时，一定显示
            index = bmp_index[n];
            lcd_draw_image(RES_BUF_BMP_SYS_DAT, index, x, y);
            fill = 1;                           //出现过数据，则一定需要填充数据，包括0
            x += wid;
        } else {
            x += wid;
        }
        num %= max_num;
        max_num /= 10;
    }
}

//显示格式时间, 分秒
AT(.text.lcd)
void lcd_disp_medium_time(u8 min, u8 sec, u8 x, u8 y)
{
    lcd_disp_num(min, x, y, DN_RIGHT | DN_ZERO | 2);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_COLON, x + 18, y);
    lcd_disp_num(sec, x + 23, y, DN_RIGHT | DN_ZERO | 2);
}

//显示格式时间, 时分秒
AT(.text.lcd)
void lcd_disp_medium_atime(u8 hour, u8 min, u8 sec, u8 x, u8 line)
{
    lcd_disp_num(hour, x, line, DN_RIGHT | DN_ZERO | 2);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_COLON, x + 18, line);
    lcd_disp_num(min, x + 23, line, DN_RIGHT | DN_ZERO | 2);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_COLON, x + 41, line);
    lcd_disp_num(sec, x + 46, line, DN_RIGHT | DN_ZERO | 2);
}

AT(.text.lcd)
static u8 get_freq_spectrum_value(u8 num)
{
    u8 i = 0;

    if(num < 5) {
        i = get_random(8);
    } else if((num >= 5) && (num < 10)) {
        i = get_random(5);
    } else if(num >= 10) {
        i = get_random(2);
    }
    return i;
}

//显示音乐模式频谱
AT(.text.lcd)
void lcd_disp_freq_spectrum_line(u8 y)
{
    u8 i, j, val, x = 8;
    u8 *buf = &lcd_buf[COLUMN_MAX * y];

    for (i = 0; i < 16; i++) {
        if (f_msc.pause) {
            val = 0;
        } else {
            val = get_freq_spectrum_value(i);
        }

        if (val + 1 < freq_spectrum_buf[i]) {
            val = freq_spectrum_buf[i] - 2;
        } else if (val && val == freq_spectrum_buf[i]) {
            val = freq_spectrum_buf[i] - 1;
        }
        freq_spectrum_buf[i] = val;
        for (j = 0; j != 6; j++) {
            buf[x] = tbl_freq_spectrum0[val];
            buf[COLUMN_MAX + x] = tbl_freq_spectrum1[val];
            x++;
        }
        buf[x] = 0;
        buf[COLUMN_MAX + x] = 0;
        x++;
    }
}

AT(.text.lcd)
void lcd_disp_msgbox(char *title, u32 index)
{
    u8 x;
    u8  width = (index >> 20) & 0xff;        //图片宽度 (像素点)

    x = (128 - width) / 2;
    lcd_buf_clear();
    if (title) {
        lcd_draw_text(title, LINE_CENTER, 0 , 0);
    }
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_BOX, 0, 2);
    lcd_draw_image(RES_BUF_BMP_MENU_DAT, index, x, 3);
    lcd_line_update(0, 8);
}

AT(.text.lcd)
void lcd_disp_progressbox(char *title, u32 value, u32 min, u32 max)
{
    u8 x;
    u8 percent = ((value - min) << 7) / (max - min);

    lcd_buf_clear();
    if (title) {
        lcd_draw_text(title, LINE_CENTER, 0 , 0);
    }
    lcd_disp_num(value, 56, 3, DN_RIGHT | 2);

    x = 29 + (percent * 65 >> 7);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_BAR, 29, 5);
    lcd_draw_image(RES_BUF_BMP_SYS_DAT, BMP_SYS_INDEX, x, 5);
    lcd_line_update(0, 8);
}

AT(.text.lcd)
void lcd_draw_menu(u32 index, u16 x, u8 y)
{
    u8  width = (index >> 20) & 0xff;        //图片宽度 (像素点)
    if (x == LINE_CENTER) {
        x = (128 - width) / 2;
    }
    lcd_draw_image(RES_BUF_BMP_MENU_DAT, index, x, y);
}

///区域反显
void lcd_disp_invert(u8 x, u8 y, u8 width, u8 n)
{
    u8 i, j;
    u8 *buf;

    if (x+width > COLUMN_MAX || y+n > LINE_MAX) {
        return;
    }

    for (i = 0; i < n; i++) {
        buf = &lcd_buf[(COLUMN_MAX * (y + i)) + x];
        for (j = 0; j < width; j++) {
            *buf = ~*buf;
            buf++;
        }
    }
}
#endif
