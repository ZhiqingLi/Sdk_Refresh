#ifndef _PORT_LCD_H
#define _PORT_LCD_H

//SPI GROUP MAP
#define SPI1MAP_G               SPI1MAP_G1

//LCD CMD PIN(A0)
#define LCD_CMD_INIT()          {GPIOADE |= BIT(2); GPIOADIR &= ~BIT(2); GPIOASET = BIT(2);}
#define LCD_CMD_EN()            {GPIOACLR = BIT(2);}
#define LCD_CMD_DIS()           {GPIOASET = BIT(2);}

//LCD RST PIN
#define LCD_RST_INIT()          {GPIOADE |= BIT(1); GPIOADIR &= ~BIT(1); GPIOASET = BIT(1);}
#define LCD_RST_SET()           {GPIOASET = BIT(1);}
#define LCD_RST_CLR()           {GPIOACLR = BIT(1);}

//LCD CS PIN
#define LCD_SPI_CS_INIT()       {GPIOADE |= BIT(0); GPIOADIR &= ~BIT(0); GPIOASET = BIT(0);}
#define LCD_SPI_CS_EN()         {GPIOACLR = BIT(0);}
#define LCD_SPI_CS_DIS()        {GPIOASET = BIT(0);}

#define LCD_SPI_CLK_INIT()      {GPIOAFEN |= BIT(3); GPIOADE |= BIT(3); GPIOADIR &= ~BIT(3); GPIOASET = BIT(3);}
#define LCD_SPI_DAT_INIT()      {GPIOAFEN |= BIT(4); GPIOADE |= BIT(4); GPIOADIR |= BIT(4); GPIOAPU |= BIT(4);}

#define LCD_BACKLIGHT_INIT()    {GPIOADE |= BIT(5); GPIOADIR &= ~BIT(5); GPIOASET = BIT(5);}
#define LCD_BACKLIGHT_ON()      {GPIOASET = BIT(5);}
#define LCD_BACKLIGHT_OFF()     {GPIOACLR = BIT(5);}


void lcd_spi_init(void);

#endif // _PORT_LCD_H
