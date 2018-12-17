#include "type.h"
#include "app_config.h"
#include "display.h" 
#include "ht1622.h" 

#ifdef  FUNC_HT1622_DRIVE_EN//A92950_LCD
#if 1//def USE_TEMP_DEBUG_LCD
#define USE_HT1621_EN
#endif

void LCDDelay(uint16_t Delay)
{
	uint16_t j;
	
	while(Delay)
		{
			j = 3;
			while(j)
			{
				j--;
			}
			Delay--;
		}
}

//=====ht1621 operation start======
void HT1622_start(void)
{
	HT1622_CLK_SET;
	HT1622_CS_SET;
	HT1622_DATA_SET;
	
	LCDDelay(1);
	HT1622_CS_HIGH;
	HT1622_CLK_HIGH;
	HT1622_DATA_HIGH;
	LCDDelay(1);
	HT1622_CS_LOW;
}

//=====ht1621 send command and data interface======
void HT1622_send_data(uint8_t send_data, uint8_t send_len)
{   	
	uint8_t ht_count;
	
    for(ht_count = 0;ht_count < send_len;ht_count++)
	{
		HT1622_CLK_LOW;
		//LCDDelay(1);
		if ((send_data & 0x80) == 0x80)
		{
			HT1622_DATA_HIGH;
		}
		else
		{
    		HT1622_DATA_LOW;
		}
		LCDDelay(1);
		HT1622_CLK_HIGH;	
    	send_data = (send_data << 1);	
		LCDDelay(1);
	}	
	LCDDelay(1);
}

//=====ht1621 operation end======
void HT1622_end(void)
{	
	HT1622_DATA_LOW;
	HT1622_CS_LOW;
	HT1622_CLK_LOW;
}	

//======HT1621 Display init.=====
void HT1622_init(void)
{ 
   HT1622_start();
   #ifndef USE_HT1621_EN
   HT1622_send_data(0x80,3);                        //set command mode	 		
   HT1622_send_data( HT1622_COMM_SYS_EN,9 );        //sys-en  0x01
  // ht1621_send_data( HT1622_COMM_BIAS3_COM4,9 );  //bias 1/3  0x29 
   HT1622_send_data( HT1622_COMM_TNOR,9 );          //normal mode 0xe3
   HT1622_send_data( HT1622_COMM_RC256K,9 );        //256k       0x18//sys crystal is internal RC 0x18 
   HT1622_send_data( HT1622_COMM_LCD_ON,9);         //lcd on  0x03
   #else
   HT1622_send_data(0x80,3);                        //set command mode
   HT1622_send_data( HT1621_COMM_SYS_EN,9 );        //sys-en  0x01
   HT1622_send_data( HT1621_COMM_BIAS3_COM4,9 );    //bias 1/3  0x29 
   HT1622_send_data( HT1621_COMM_TNOR,9 );          //normal mode 0xe3
   HT1622_send_data( HT1621_COMM_RC256K,9 );        //256k       0x18
   HT1622_send_data( HT1621_COMM_LCD_ON,9);         //lcd on  0x03
   #endif
   HT1622_end();   
}

//=====ht1621 lcd off operation======
void HT1622_LcdOff(void)
{
	HT1622_start();
	HT1622_send_data(HT1622_COMM_LCD_OFF,0);
	HT1622_end();
}  

//=====ht1621 clear display ram buffer======
void HT1622_ClearDispRam(void)
{
	unsigned char DispI;
	
	HT1622_start();
	HT1622_send_data(0xa0,3);                       //set write data mode
	HT1622_send_data(0,6);  
	for (DispI = 0;DispI < DISP_ALINE_CHAR;DispI++)
	{
		HT1622_send_data(0,8);
	}
	HT1622_end();
} 

//=====ht1621 set display ram buffer======
void HT1622_SetDispRam(void)
{
	unsigned char DispI;
	
	HT1622_start();
	HT1622_send_data(0xa0,3);                       //set write data mode
	HT1622_send_data(0,6);  
	for (DispI = 0;DispI < DISP_ALINE_CHAR;DispI++)
	{
		HT1622_send_data(0xff,8);
	}
	HT1622_end();
} 
#endif


