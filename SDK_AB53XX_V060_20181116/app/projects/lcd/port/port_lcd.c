#include "include.h"

#if (GUI_SELECT == GUI_LCD)
AT(.text.lcd.port)
void lcd_spi_init(void)
{
    LCD_CMD_INIT();
    LCD_SPI_CS_INIT();
    LCD_SPI_CLK_INIT();
    LCD_SPI_DAT_INIT();
    LCD_RST_INIT();
    delay_us(50);
    LCD_RST_CLR();                              //LCD reset
    delay_us(300);
    LCD_RST_SET();
    delay_us(20);
    LCD_BACKLIGHT_INIT();

    FUNCMCON1 = SPI1MAP_G;
    SPI1CON = BIT(5) | BIT(2) | BIT(0);          //2 WIRE, IDLE 1, EN, NORMAL , dis irq
    SPI1BAUD = 0x01;
}
#endif
