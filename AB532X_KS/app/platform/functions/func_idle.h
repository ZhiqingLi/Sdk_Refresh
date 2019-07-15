#ifndef _FUNC_IDLE_H
#define _FUNC_IDLE_H

#define  SPI_BAUD			(375000)
#define  SPI_RECV_BUF_LEN	32


#define SPI_RECV_TIMER					1000

//ä¼ ç»™ä¸»æœºçš„å‘½ä»¤
#define SLAVE_BOT_DON					0x01
#define SLAVE_RECV_OK					0x02			
#define GUI_ERRO_TIMER					500
#define RGB_SWAP_TIMER					3000			//È«²ÊÑÕÉ«ÇÐ»»Ê±¼ä
#define RGB_STOP_TIMER					5000			//È«²ÊÑÕÉ«Í£ÁôÊ±¼ä

#define IS_SPI1_CS_ENABLE()				(!(GPIOA & BIT(5)))

enum {
	COLOUR_IDLE = 0,
	COLOUR_RED,
	COLOUR_ORANGE,
	COLOUR_YELLOW,
	COLOUR_GREEN,
	COLOUR_CYAN,
	COLOUR_BLUE,
	COLOUR_PURPLE,
	COLOUR_WHITE,
	COLOUR_MAX,
};

typedef struct {
	uint8_t 	recv_head;
	uint8_t		recv_tail;
	uint8_t 	recv_buffer[SPI_RECV_BUF_LEN];
	int8_t 		recv_index;
	uint8_t		spi1_cmd[SPI_RECV_BUF_LEN];
	uint32_t	spi1_cmd_timer;
}func_idle_t;

extern func_idle_t		f_idle;

void func_idle_message(u16 msg);
void func_water_pump_process(bool work);

#if (GUI_SELECT != GUI_NO)
void func_idle_display(void);
#else
#define func_idle_display()
#endif

#endif // _FUNC_IDLE_H
