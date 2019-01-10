#include "include.h"
#include "bsp_lcd.h"

#if DISP_LCD_EN
static void lcd_delay_ms(u16 ms)
{
    while(ms--) {
        delay_us(800);
    }
}

/****************************************************
描    述: 串行发送构成单元
函数属性: 私有
入口参数: u8 base_byte (串行发送单元字节)
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void serial_base(u8 base_byte)
{
    s8 i;
    for(i=7; i>=0; i--) {
        if(base_byte&(0x01<<i)) {
            LCD_DAT_SET();
            asm("nop");
        } else {
            LCD_DAT_CLR();
            asm("nop");
        }
        LCD_CLK_SET();
        asm("nop");asm("nop");
        asm("nop");asm("nop");      //下降沿
        LCD_CLK_CLR();
        asm("nop");
    }
}

/****************************************************
描    述: 串行发送数据或指令
函数属性: 私有
入口参数: byte(发送数据),type(数据类型：数据or指令)
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_serial_send(u8 byte, u8 type)
{
    u8 temp = 0;

    cli();
    LCD_CLK_CLR();
    LCD_CS_SET();                                   //关中断，sclk=0，cs=1
    if(type==0x01) {
        temp = 0xf8;    //rw=0,rs=0
    } else {
        temp = 0xfa;    //rw=0,rs=1
    }
    asm("nop");
    serial_base(temp);
    serial_base(byte&0xf0);                         //发送高四位
    serial_base((byte<<4)&0xf0);                    //发送低四位
    LCD_CS_CLR();
    sei();                                          //cs=0,开中断
    delay_us(72);
}

/****************************************************
描    述: 12864初始化
函数属性: 公有
入口参数: 无
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_init(void)
{
    LCD_DIR_DIROUT();
    LCD_CS_DIROUT();
    LCD_CLK_DIROUT();
    LCD_DAT_DIROUT();                               //cs,sclk,std口设置输出

    LCD_DIR_SET();
    LCD_CS_CLR();
    LCD_CLK_CLR();
    LCD_DAT_CLR();                                  //cs=sclk=std=0

    lcd_delay_ms(30);
    lcd_serial_send(lcd_funtion_base, command);     //基本指令集lcd_funtion_base=0x30
    lcd_serial_send(lcd_disp_on, command);          //开显示
    lcd_serial_send(lcd_clean_disp, command);       //清显示
    lcd_delay_ms(5);
    lcd_serial_send(lcd_dote_left, command);
    lcd_delay_ms(5);
    lcd_clean_cgram();
}

/****************************************************
描    述: 清中文RAM
函数属性: 私有
入口参数: 无
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_clean_ddram(void)
{
    lcd_serial_send(lcd_funtion_base, command);
    lcd_serial_send(lcd_clean_disp, command);
    lcd_delay_ms(5);
}

/****************************************************
描    述: 清图形RAM
函数属性: 私有
入口参数: 无
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_clean_cgram(void)
{
    u8 x;
    u8 y;

    lcd_serial_send(lcd_funtion_additional, command);       //扩展指令集
    lcd_serial_send(lcd_move_select, command);              //允许设定cgram地址
    lcd_serial_send(lcd_funtion_additional, command);       //关cgram显示
    for(x=0; x<16; x++) {
        for(y=0; y<32; y++) {
            lcd_serial_send(lcd_cgram_address|y, command);  //设置绘图区的Y地址坐标
            lcd_serial_send(lcd_cgram_address|x, command);  //设置绘图区的X地址坐标
            lcd_serial_send(0x00, data);
            lcd_serial_send(0x00, data);
        }
    }
    lcd_serial_send(lcd_funtion_additional|0x02, command);  //开cgram显示
}

/****************************************************
描    述: 设定中文坐标
函数属性: 私有
入口参数: x(列序号1-8),y(行序号1-4)
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_set_xy(u8 x, u8 y)
{
    u8 address_temp = 0;

    if(--x>7) x=0;
    if(--y>3) y=0;
    switch(y) {
        case 0: address_temp = 0x80+x; break;
        case 1: address_temp = 0x90+x; break;
        case 2: address_temp = 0x88+x; break;
        case 3: address_temp = 0x98+x; break;
    }
    lcd_serial_send(lcd_set_ddram_address|address_temp, command);
}

/****************************************************
描    述: 显示一个中文
函数属性: 公有
入口参数: x(列序号1-18),y(行序号1-4)word(中文ascii码)
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_disp_char(u8 x, u8 y, u8 word_high, u8 word_low)
{
    lcd_serial_send(lcd_funtion_base, command);
    lcd_set_xy(x, y);
    lcd_serial_send(word_high,data);
    lcd_serial_send(word_low,data);
}

/****************************************************
描    述: 显示一串字符（中文和符号）
函数属性: 公有
入口参数: x(列序号1-8), y(行序号1-4), *str(字符首指针)
出口参数: 无
设    计: zoro
日    期: 2009.10.8
最后修改: 日期(2009.10.8)  修改人(zoro)
****************************************************/
void lcd_disp_str(u8 x, u8 y, char *str)
{
    u8 i = 0;

    x--;
    while(str[i]) {
        if(++x >= 9) {
            x = 1;
            y++;
        }

        if(str[i+1] == '\0') {
            lcd_disp_char(x, y, str[i], '\0');
            break;
        } else {
            lcd_disp_char(x, y, str[i], str[i+1]);
        }
        i = i+2;
    }
}

/****************************************************
描    述: 绘图形式写入一个汉字(16*16)
函数属性: 公有
入口参数: *charture(字库首指针)
出口参数: 无
设    计: ljf
日    期: 2009.1.6
最后修改: 日期(2009.1.6)  修改人(ljf)
***************************************************
void lcd_disp_charture(u8 x, u8 y, u8 *charture)
{
    u16 index = 0;
    u8 x_start_temp = x-1;
    u8 y_start_temp = (y-1)*16;
    u8 x_end_temp = x;//x+1-1
    u8 y_end_temp = y*16;//16+(y-1)*16

    lcd_serial_send(lcd_funtion_additional, command);       //扩展指令集
    lcd_serial_send(lcd_move_select, command);              //允许输入IRAM地址
    if(y_start_temp < 32) {                                 //字在第1、2行时
        while(y_start_temp < y_end_temp) {
            while(x_start_temp < x_end_temp) {
                lcd_serial_send(lcd_cgram_address|y_start_temp, command);            //y坐标
                lcd_serial_send(lcd_cgram_address|x_start_temp, command);            //x坐标
                lcd_serial_send(charture[index], data);                //高字节
                index++;
                lcd_serial_send(charture[index], data);                //低字节
                index++;
                x_start_temp++;
            }
            x_start_temp = x-1;
            y_start_temp++;
        }
    } else {                                                    //字在第3、4行时
        y_start_temp -= 32;
        y_end_temp -= 32;
        x_start_temp += 8;
        x_end_temp += 8;
        while(y_start_temp < y_end_temp) {
            while(x_start_temp < x_end_temp) {
                lcd_serial_send(lcd_cgram_address|y_start_temp, command);   //y坐标
                lcd_serial_send(lcd_cgram_address|x_start_temp, command);   //x坐标
                lcd_serial_send(charture[index], data);         //高字节
                index++;
                lcd_serial_send(charture[index], data);         //低字节
                index++;
                x_start_temp++;
            }
            x_start_temp = x+7;//x+8-1
            y_start_temp++;
        }
    }
    lcd_serial_send(lcd_picture_on, command);                   //开cgram显示
}
*/
/****************************************************
描    述: 绘图形式写入一串汉字(16*16)
函数属性: 公有
入口参数: *str_ture(字库首指针)
出口参数: 无
设    计: ljf
日    期: 2009.1.6
最后修改: 日期(2009.1.6)  修改人(ljf)
***************************************************
void lcd_disp_picstr_slow(u8 x,unsigned y,u8 *picstr)
{
    u8 i = 0;
    u8 count = 0;
    while(picstr[i]) {
        if(++x > 9) {
            x = 2;
            y++;
        }
        lcd_disp_charture(x-1, y, picstr[i]);
        i = i+2;
        count++;
        lcd_delay_ms(200);
        if(count > 31) {
            lcd_clean_ddram();
            lcd_delay_ms(200);
            count=0;
            x=1;
            y=1;
        }
    }
}
*/

/****************************************************
描    述: 显示一幅图片
函数属性: 公有
入口参数: *picture(图片首指针)
出口参数: 无
设    计: zoro
日    期: 2009.10.9
最后修改: 日期(2009.10.9)  修改人(zoro)
***************************************************
void lcd_disp_picture(u8 x, u8 y, u8 *picture)
{
    volatile u8 x_max = 0;
    volatile u8 y_max = 0;
    u16 index = 2;
    u8 x_start_temp;
    u8 y_start_temp;
    u8 x_end_temp;
    u8 y_end_temp;
    u8 temp;

    lcd_serial_send(lcd_funtion_additional, command);   //扩展指令集
    lcd_serial_send(lcd_move_select, command);          //允许输入IRAM地址

    x_max = picture[0]+x;
    y_max = picture[1]+y;
    if(y > 31) {
        y_start_temp = y-32;
        x_start_temp = (x/16)+8;
        x_end_temp = (x_max/16)+8;
        y_end_temp = y_max-32;
        while(y_start_temp < y_end_temp) {
            while(x_start_temp < x_end_temp) {
                lcd_serial_send(lcd_cgram_address|y_start_temp,command);
                lcd_serial_send(lcd_cgram_address|x_start_temp,command);
                lcd_serial_send(picture[index],data);
                lcd_serial_send(picture[index+1],data);
                index = index+2;
                x_start_temp++;
            }
            x_start_temp = (x/16)+8;
            y_start_temp++;
        }
    } else {
        if(y_max > 31) {
            y_start_temp = y;
            x_start_temp = x/16;
            x_end_temp = (x_max/16) + 8;
            y_end_temp = y_max-32;
            while(y_start_temp < 32) {
                while(x_start_temp<x_end_temp) {
                    lcd_serial_send(lcd_cgram_address|y_start_temp, command);
                    lcd_serial_send(lcd_cgram_address|x_start_temp, command);
                    lcd_serial_send(picture[index], data);
                    lcd_serial_send(picture[index+1], data);
                    index = index+2;
                    x_start_temp++;
                }
                x_start_temp = x/16;
                y_start_temp++;
            }
            y_start_temp = 0;
            x_start_temp = x/16+8;

            while(y_start_temp < y_end_temp) {
                while(x_start_temp < x_end_temp) {
                    lcd_serial_send(lcd_cgram_address|y_start_temp, command);
                    lcd_serial_send(lcd_cgram_address|x_start_temp, command);
                    lcd_serial_send(picture[index], data);
                    lcd_serial_send(picture[index+1], data);
                    index = index+2;
                    x_start_temp++;
                }
                x_start_temp = x/16+8;
                y_start_temp++;
            }
        } else {
            y_start_temp = y;
            x_start_temp = x/16;
            x_end_temp = x_max/16;
            y_end_temp = y_max;
            while(y_start_temp < y_start_temp) {
                while(x_start_temp < x_end_temp) {
                    lcd_serial_send(lcd_cgram_address|y_start_temp, command);
                    lcd_serial_send(lcd_cgram_address|x_start_temp, command);
                    lcd_serial_send(picture[index], data);
                    lcd_serial_send(picture[index+1], data);
                    index = index+2;
                    x_start_temp++;
                }
                x_start_temp = x/16;
                y_start_temp++;
            }
        }
    }

    lcd_serial_send(lcd_picture_on, command);           //开cgram显示
}
*/

#endif //DISP_LCD_EN
