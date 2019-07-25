#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN
func_idle_t		f_idle;


AT(.text.func.idle)
static void spi1_cs_init(void)
{
	GPIOADE |= BIT(5);  
	GPIOADIR |= BIT(5); 
	GPIOAFEN &= ~BIT(5);
    GPIOAPU200K |= BIT(5);
}

///SPI接口获取1Byte数据
AT(.text.func.idle)
static u8 spi1_getbyte(void)
{
    SPI1CON |= BIT(4);                              //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)) && IS_SPI1_CS_ENABLE()) {
	    WDT_CLR();                   //Wait pending
    }
    return SPI1BUF;
}

//SPI接口发�?Byte数据
AT(.text.func.idle)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)) && IS_SPI1_CS_ENABLE()) {
    	WDT_CLR();                   //Wait pending
    }
}


AT(.text.func.idle)
void spi1_init(u32 baud)
{
//    //SPI1_G2:  DIO_PA7,  CLK_PA6, //CS_PA5
    GPIOAFEN |= BIT(6)|BIT(7);
    GPIOADE |= BIT(6)|BIT(7);
    GPIOADIR |= (BIT(6)|BIT(7));	//CLK input,DO input
    GPIOAPU |= BIT(6)|BIT(7);		//pu 10k
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (0x02<<12);			//G2
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SPI1BAUD = 26000000/baud - 1;   //sys 24M.
    SPI1CON |= BIT(4); 
    SPI1CON |= BIT(1);				//SLAVE MODE    
    SPI1CON |= (BIT(0)|BIT(2));
    printf("spi init 0x%X;\n", SPI1CON);

    SPI1BUF = 0xff;
    f_idle.recv_head = 0;
    f_idle.recv_tail = 0;
    f_idle.recv_index = -3;
    memset(&f_idle.recv_buffer, 0, sizeof(f_idle.recv_buffer));
    memset(&f_idle.spi1_cmd, 0, sizeof(f_idle.spi1_cmd));
}

AT(.text.func.idle)
uint8_t get_spi1_recv_buffer_data(uint8_t *data)
{
	uint8_t ret = 0;

	ret = (f_idle.recv_head+SPI_RECV_BUF_LEN-f_idle.recv_tail)%SPI_RECV_BUF_LEN;
	if (ret) {
		*data = f_idle.recv_buffer[f_idle.recv_tail++];
		f_idle.recv_tail %= SPI_RECV_BUF_LEN;
	}
	return ret;
}

AT(.text.func.idle)
void spi1_send_cmd_to_master(uint8_t cmd)
{
	uint8_t cmd_buf[SPI_RECV_BUF_LEN];
	uint8_t len, send_index;

	memset(&cmd_buf, 0, sizeof(cmd_buf));
	
	switch(cmd) {
		case SLAVE_BOT_DON:
			break;

		case SLAVE_RECV_OK:
			memcpy(&cmd_buf, "SLA+RECV+OK&", 12);
			len = 12;
			break;

		default:
			break;
	}

	for(send_index = 0; send_index != len; send_index++) {
		spi1_sendbyte(cmd_buf[send_index]);
	}

	printf ("send master cmd : %s\n", cmd_buf);
}


AT(.text.func.idle)
static bool spi1_recv_cmd_process(void)
{
	uint8_t spi1_data;
	bool ret = 0;
	
	f_idle.spi1_cmd_timer = tick_get();
	
	while (get_spi1_recv_buffer_data(&spi1_data)) {
		if(-3 == f_idle.recv_index) {
			if ('M' == spi1_data) {
				f_idle.recv_index = -2;
			}
		}
		else if (-2 == f_idle.recv_index) {
			if ('A' == spi1_data) {
				f_idle.recv_index = -1;
			}
			else {
				f_idle.recv_index = -3;
			}
		}
		else if (-1 == f_idle.recv_index) {
			if ('S' == spi1_data) {
				f_idle.recv_index = 0;
			}
			else {
				f_idle.recv_index = -3;
			}
		}
		else {
			if ((0 == f_idle.recv_index) && ('+' != spi1_data)) {
				f_idle.recv_index = -3;
			}
			else {
				f_idle.spi1_cmd[f_idle.recv_index] = spi1_data;
				if((f_idle.recv_index >= 8) || ('&' == f_idle.spi1_cmd[f_idle.recv_index])) {
					f_idle.spi1_cmd[f_idle.recv_index+1] = '\0';
					f_idle.recv_index = -3;
					ret = 1;
					return ret;
				}
				f_idle.recv_index++;
			}
		}
	}	

	return ret;
}

AT(.text.func.idle)
void func_idle_process(void)
{
	static bool is_power_off = 0;
    func_process();
    if (!is_power_off && get_extern_on_state()) {
    	printf ("external gpio power off;\n");
    	LOUDSPEAKER_MUTE_DIS();
    	is_power_off = 1;
#if USER_EXT_POWERON_EN
    	EXT_GPIO_POWEROFF();
#endif
		led_power_down();
    }

	if (IS_SPI1_CS_ENABLE()) {
		while (IS_SPI1_CS_ENABLE()) {
			f_idle.recv_buffer[f_idle.recv_head] = spi1_getbyte();
			f_idle.recv_head++;
			f_idle.recv_head %= SPI_RECV_BUF_LEN;
		} 
	}

	if (spi1_recv_cmd_process()) {
		
		spi1_send_cmd_to_master(SLAVE_RECV_OK);
		if (memcmp(&f_idle.spi1_cmd, "+SET+RBT&", 9) == 0) {
			param_exit_state_write(1);
		}
		else if (memcmp(&f_idle.spi1_cmd, "+CLR+RBT&", 9) == 0) {
			param_exit_state_write(0);
		}

		printf ("recv master cmd : %s;\n", f_idle.spi1_cmd);
		memset(&f_idle.spi1_cmd, 0x00, SPI_RECV_BUF_LEN);
	}
}

static void func_idle_enter(void)
{
#if PLUGIN_FUNC_IDLE_ENTER_CHECK
    if (!plugin_func_idle_enter_check()) {  //可以处理开�? 判断是否要停在idle模式, 等待设备插入的方�?
        return;
    }
#endif
    //led_idle();
    spi1_init(SPI_BAUD);
    spi1_cs_init();
#if USER_EXT_POWERON_EN
	if (!get_extern_on_state()) {
		EXT_GPIO_POWERON(); 					//20190224，进入开机时打开外部电源�?
	}
#endif //USER_EXT_POWERON_EN
}

static void func_idle_exit(void)
{
    func_cb.last = FUNC_IDLE;
}

AT(.text.func.idle)
void func_idle(void)
{
    printf("%s\n", __func__);

    func_idle_enter();

    while (func_cb.sta == FUNC_IDLE) {
        func_idle_process();
        func_idle_message(msg_dequeue());
        func_idle_display();
    }

    func_idle_exit();
}

#endif
