#ifndef _12864_H_
#define _12864_H_


/*****************定义端口******************/
#define LCD_CS_DE           GPIOEDE
#define LCD_CS_PORT         GPIOE
#define LCD_CS_DIR          GPIOEDIR
#define LCD_CS_PIN          1

#define LCD_CLK_DE          GPIOEDE
#define LCD_CLK_PORT        GPIOE
#define LCD_CLK_DIR         GPIOEDIR
#define LCD_CLK_PIN         3

#define LCD_DAT_DE          GPIOEDE
#define LCD_DAT_PORT        GPIOE
#define LCD_DAT_DIR         GPIOEDIR
#define LCD_DAT_PIN         2

#define LCD_DIR_DE          GPIOEDE
#define LCD_DIR_PORT        GPIOE
#define LCD_DIR_DIR         GPIOEDIR
#define LCD_DIR_PIN         4

#define cli()
#define sei()

/***************操作宏定义******************/
#define LCD_CS_DIROUT()     {LCD_CS_DE |= BIT(LCD_CS_PIN); LCD_CS_DIR &= ~BIT(LCD_CS_PIN);}
#define LCD_CS_SET()        LCD_CS_PORT |= BIT(LCD_CS_PIN)
#define LCD_CS_CLR()        LCD_CS_PORT &= ~BIT(LCD_CS_PIN)
#define LCD_CLK_DIROUT()    {LCD_CLK_DE |= BIT(LCD_CLK_PIN); LCD_CLK_DIR &= ~BIT(LCD_CLK_PIN);}
#define LCD_CLK_SET()       LCD_CLK_PORT |= BIT(LCD_CLK_PIN)
#define LCD_CLK_CLR()       LCD_CLK_PORT &= ~BIT(LCD_CLK_PIN)
#define LCD_DAT_DIROUT()    {LCD_DAT_DE |= BIT(LCD_DAT_PIN); LCD_DAT_DIR &= ~BIT(LCD_DAT_PIN);}
#define LCD_DAT_SET()       LCD_DAT_PORT |= BIT(LCD_DAT_PIN)
#define LCD_DAT_CLR()       LCD_DAT_PORT &= ~BIT(LCD_DAT_PIN)
#define LCD_DIR_DIROUT()    {LCD_DIR_DE |= BIT(LCD_DIR_PIN); LCD_DIR_DIR &= ~BIT(LCD_DIR_PIN);}
#define LCD_DIR_SET()       LCD_DIR_PORT |= BIT(LCD_DIR_PIN)
#define LCD_DIR_CLR()       LCD_DIR_PORT &= ~BIT(LCD_DIR_PIN)

/**************自定义指令集*****************/
#define command     0x01
#define data        0x00

/**************基本指令集*******************/
#define lcd_clean_disp   			0x01       //清除显示
#define lcd_address_to_zero 		0x02       //位址归位
//lcd_dote_set			读/写状态时点移动设置
#define lcd_dote_left 	 			0x04       //光标左移、显示不移动
#define lcd_dote_shift_left 		0x05       //光标左移、显示左移
#define lcd_dote_right 	 			0x06       //光标右移、显示不移动
#define lcd_dote_shift_right 	 	0x07       //光标右移、显示移动
//lcd_disp_state		显示状态设置
#define lcd_disp_on					0x0c		//显示开
#define lcd_disp_off				0x08		//显示关
#define lcd_disp_cursor_on			0x0e		//光标开、显示开
#define lcd_disp_cursor_off			0x0c		//关光标(显示开)
#define lcd_disp_cursor_blink		0x0f		//开闪烁(光标开)
#define lcd_disp_cursor_noblink		0x0e		//关闪烁(光标开)
//disp_shift_ctrl		游标、显示移动设置
#define lcd_cursor_shift_left 	    0x10       //游标左移
#define lcd_cursor_shift_right 	    0x14       //游标右移
#define lcd_disp_shift_left 	    0x18       //显示、游标左移
#define lcd_disp_shift_right 	    0x1c       //显示、游标右移

#define lcd_funtion_base 			0x30       //设置基本指令集
#define lcd_funtion_additional 		0x34 	   //设定扩展指令集
#define lcd_set_cgram_address 		0x40       //设定CGRAM位址
#define lcd_set_ddram_address 		0x80       //设定DDRAM位址

/**************扩展指令集*******************/
#define lcd_standby_mode 			0x01       //待命模式
#define lcd_move_select 			0x02       //卷动位址或IRAM位址选择
#define lcd_reverse_video_select 	0x04       //反白选择
#define lcd_sleepmode_enable 		0x08       //睡眠模式开
#define lcd_sleepmode_disable 		0x0c       //睡眠模式关
#define lcd_set_move_address 		0x40       //设定IRAM位址或卷动位址
#define lcd_cgram_address 			0x80       //设定绘图RAM位址
#define lcd_picture_on	 			0x36       //绘图显示开
#define lcd_picture_off 			0x34       //绘图显示关


/************函数声明***********************/
void serial_base(u8 base_byte);
void lcd_serial_send(u8 byte, u8 type);
void lcd_init(void);
void lcd_clean_ddram(void);
void lcd_set_xy(u8 x, u8 y);
void lcd_disp_char(u8 x, u8 y, u8 word_high, u8 word_low);
void lcd_clean_cgram(void);
void lcd_disp_str(u8 x, u8 y, char *str);
//void lcd_disp_picture(u8 x, u8 y, u8 *picture);
//void lcd_disp_charture(u8 x, u8 y, u8 *picture);
#endif
