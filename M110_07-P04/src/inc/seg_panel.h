#ifndef __SEG_PANEL_H__
#define __SEG_PANEL_H__

#if 1 //(defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))

//#define LED_COMMON_ANODE		0//共阳极
//#define LED_COMMON_CATHODE	1//共阴极
//#define LED_COMMON_TYPE		LED_COMMON_CATHODE

#ifdef AU6210K_BOOMBOX_DEMO
#define MAX_SEG_NUM		13	//8
#define MAX_COM_NUM		4

#else
#define MAX_SEG_NUM		7	//8
#define MAX_COM_NUM		5
#endif

#ifdef AU6210K_BOOMBOX_DEMO
#define MASK_LCD_COM	0x000F
#define MASK_LCD_SEG	0x7FF0
#else
#define MASK_LCD_COM	0xF300
#define MASK_LCD_SEG	0x00FF
#endif

#if (MAX_SEG_NUM > 8) //SEG数目大于8时，一个COM需要的存储空间为2个字节，应该定义为WORD类型。
#define DISP_TYPE	uint16_t
#else
#define DISP_TYPE	uint8_t
#endif


#define POS_COM_THOUSAND	0	// COM0	// D6
#define POS_COM_HUNDREDS	1	// COM1	// D5
#define POS_COM_TENS		2	// COM2	// D4
#define POS_COM_UNITS		3	// COM3	// D3

#ifndef FUNC_TM1628_LED_EN
//COM定义
#define LED_COM_PORT_IE		GPIO_A_IE
#define LED_COM_PORT_OE		GPIO_A_OE
#define LED_COM_PORT_OUT	GPIO_A_OUT
#define LED_COM_PORT_PU		GPIO_A_PU
#define LED_COM_PORT_PD		GPIO_A_PD
#if (defined(AU6210K_DEMO))
#define MASK_LED_COM		0x67
#define LED_COM_STARTBIT	0x01	// D0
#else
#define MASK_LED_COM		0xF8
#define LED_COM_STARTBIT	0x08	// D3
#endif

#define LED_SEG_PORT_IE		GPIO_B_IE
#define LED_SEG_PORT_OE		GPIO_B_OE
#define LED_SEG_PORT_OUT	GPIO_B_OUT
#define LED_SEG_PORT_PU		GPIO_B_PU
#define LED_SEG_PORT_PD		GPIO_B_PD
#define LED_SEG_PORT_1P7MA	GPIO_B_PULLDOWN1
#define LED_SEG_PORT_2P4MA	GPIO_B_PULLDOWN2

#if (defined(AU6210K_DEMO))
#define MASK_LED_SEG		0xBF
#else
#define MASK_LED_SEG		0xFF
#endif
#endif

//SEG定义
#if defined(FUNC_SEG_LCD_EN)
#define SEGA_BITNO	0
#define SEGB_BITNO	1
#define SEGC_BITNO	2
#define SEGD_BITNO	3
#define SEGE_BITNO	4
#define SEGF_BITNO	5
#define SEGG_BITNO	6
#define SEGH_BITNO	7

#elif defined(AU6210K_DEMO)
#define SEGA_BITNO	7
#define SEGB_BITNO	5
#define SEGC_BITNO	4
#define SEGD_BITNO	3
#define SEGE_BITNO	2
#define SEGF_BITNO	1
#define SEGG_BITNO	0
#define SEGH_BITNO	6

#else
#define SEGA_BITNO	0
#define SEGB_BITNO	1
#define SEGC_BITNO	2
#define SEGD_BITNO	3
#define SEGE_BITNO	4
#define SEGF_BITNO	5
#define SEGG_BITNO	6
#define SEGH_BITNO	7
#endif

#define	SEG_A	(1 << SEGA_BITNO)
#define	SEG_B	(1 << SEGB_BITNO)
#define	SEG_C	(1 << SEGC_BITNO)
#define	SEG_D	(1 << SEGD_BITNO)
#define	SEG_E	(1 << SEGE_BITNO)
#define	SEG_F	(1 << SEGF_BITNO)
#define	SEG_G	(1 << SEGG_BITNO)
#define	SEG_H	(1 << SEGH_BITNO)

#define	NUM_0	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define	NUM_1	(SEG_B | SEG_C)
#define	NUM_2	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define	NUM_3	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G)
#define	NUM_4	(SEG_B | SEG_C | SEG_F | SEG_G)
#define	NUM_5	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define	NUM_6	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define	NUM_7	(SEG_A | SEG_B | SEG_C)
#define	NUM_8	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define	NUM_9	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)

#define	CHR_A	(SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define	CHR_B	(SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define	CHR_C	(SEG_A | SEG_D | SEG_E | SEG_F)
#define	CHR_D	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)

#define	CHR_E	(SEG_A | SEG_D | SEG_E | SEG_F | SEG_G)
#define	CHR_F	(SEG_A | SEG_E | SEG_F | SEG_G)
#define	CHR_G	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F)
#define	CHR_H	(SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
//#define	CHR_H	(SEG_C | SEG_E | SEG_F | SEG_G)			//为了与X(显示位H)区别开，H显示位'h'
#define	CHR_I	(SEG_E | SEG_F)
#define	CHR_J	(SEG_B | SEG_C | SEG_D)
#define	CHR_K	(SEG_D | SEG_E | SEG_F | SEG_G)
#define	CHR_L	(SEG_D | SEG_E | SEG_F)
#define	CHR_M	(SEG_A | SEG_B | SEG_E | SEG_F)
#define	CHR_N	(SEG_A | SEG_B | SEG_C | SEG_E | SEG_F)
#define	CHR_O	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define	CHR_P	(SEG_A | SEG_B | SEG_E | SEG_F | SEG_G)
#define	CHR_Q	(SEG_A | SEG_B | SEG_C | SEG_F | SEG_G)
#define	CHR_R	(SEG_E | SEG_F | SEG_G)
#define	CHR_S	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define	CHR_T	(SEG_A | SEG_E | SEG_F)
#define	CHR_U	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define	CHR_V	(SEG_C | SEG_D | SEG_E)
#define	CHR_W	(SEG_C | SEG_D | SEG_E | SEG_F)
#define	CHR_X	(SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define	CHR_Y	(SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
#define	CHR_Z	(SEG_A | SEG_D)
#define	CHR_	(SEG_G)
#define	CHR_NUL 0


#ifdef FUNC_SEG_LCD_EN
//定义每个字符段SEG 所在的COM 数: (COM1--COMn:  0 -- (n-1);  0xFF:表示不存在)
#define A1_ADDR         0
#define B1_ADDR         1
#define C1_ADDR         0
#define D1_ADDR         0
#define E1_ADDR         0
#define F1_ADDR         0xFF
#define G1_ADDR         0

#define A2_ADDR         3
#define B2_ADDR         2
#define C2_ADDR         1
#define D2_ADDR         0
#define E2_ADDR         1
#define F2_ADDR         3
#define G2_ADDR         2

#define A3_ADDR         3
#define B3_ADDR         2
#define C3_ADDR         1
#define D3_ADDR         0
#define E3_ADDR         1
#define F3_ADDR         3
#define G3_ADDR         2

#define A4_ADDR         3
#define B4_ADDR         2
#define C4_ADDR         1
#define D4_ADDR         0
#define E4_ADDR         1
#define F4_ADDR         3
#define G4_ADDR         2


//定义每个字符段SEG 对应的位: (SEG1--SEGn:  1 -- n;  0x00:表示不存在)
#define A1_VALUE       (1 << 0)
#define B1_VALUE       (1 << 1)
#define C1_VALUE       (1 << 1)
#define D1_VALUE       (1 << 0)
#define E1_VALUE       (1 << 0)
#define F1_VALUE       (1 << 0)
#define G1_VALUE       (1 << 0)

#define A2_VALUE       (1 << 3)
#define B2_VALUE       (1 << 3)
#define C2_VALUE       (1 << 3)
#define D2_VALUE       (1 << 2)
#define E2_VALUE       (1 << 2)
#define F2_VALUE       (1 << 2)
#define G2_VALUE       (1 << 2)

#define A3_VALUE       (1 << 5)
#define B3_VALUE       (1 << 5)
#define C3_VALUE       (1 << 5)
#define D3_VALUE       (1 << 4)
#define E3_VALUE       (1 << 4)
#define F3_VALUE       (1 << 4)
#define G3_VALUE       (1 << 4)

#define A4_VALUE       (1 << 7)
#define B4_VALUE       (1 << 7)
#define C4_VALUE       (1 << 7)
#define D4_VALUE       (1 << 6)
#define E4_VALUE       (1 << 6)
#define F4_VALUE       (1 << 6)
#define G4_VALUE       (1 << 6)
#endif

// LED8888/LED1888 ICON TABLE
#define	ICON48_COLON	(4*8+SEGH_BITNO)	//COM3 SEGH //sizeof(DISP_TYPE)*COMn+SEGA_BITNO
#define	ICON48_FM_MHZ	(4*8+SEGE_BITNO)	//COM2 SEGH

// LED5*7 ICON TABLE
#if 1//(defined(AU6210HD_7PIN_LED_DEMO))
#define	ICON57_PLAY		(4*8+SEGA_BITNO)	//COM5 SEGA	//sizeof(DISP_TYPE)*COMn+SEGA_BITNO
#define	ICON57_PAUSE	(4*8+SEGB_BITNO)	//COM5 SEGB
#define ICON57_USB		(4*8+SEGC_BITNO) 	//COM5 SEGC
#define ICON57_SD		(4*8+SEGD_BITNO)	//COM5 SEGD
#define ICON57_COLON	(4*8+SEGE_BITNO)	//COM5 SEGE
#define ICON57_FM_MHZ   	(4*8+SEGF_BITNO)	//COM5 SEGF
#define ICON57_MP3	(4*8+SEGG_BITNO)	//COM5 SEGG

// 请根据POWER指示实际的图标笔段设置
#define ICON57_SLEEP	(3*8+SEGD_BITNO)	//COM5 SEGD
#define ICON57_BATFUL	(3*8+SEGF_BITNO)	//COM5 SEGF
#define ICON57_BATHAF	(3*8+SEGG_BITNO)	//COM5 SEGG

#else
#define	ICON57_PLAY		(4 * 8 + SEGA_BITNO)	//COM5 SEGA	//sizeof(DISP_TYPE)*COMn+SEGA_BITNO
#define	ICON57_PAUSE	(4 * 8 + SEGB_BITNO)	//COM5 SEGB
#define ICON57_USB		(4 * 8 + SEGC_BITNO) 	//COM5 SEGC
#define ICON57_SD		(4 * 8 + SEGD_BITNO)	//COM5 SEGD
#define ICON57_COLON	(4 * 8 + SEGE_BITNO)	//COM5 SEGE
#define ICON57_FM_MHZ	(4 * 8 + SEGF_BITNO)	//COM5 SEGF
#define ICON57_MP3		(4 * 8 + SEGG_BITNO)	//COM5 SEGG

// 请根据POWER指示实际的图标笔段设置
#define ICON57_SLEEP	0xFF	//COM5 SEGD
#define ICON57_BATFUL	0xFF	//COM5 SEGF
#define ICON57_BATHAF	0xFF	//COM5 SEGG
#endif


// LCD5*8 ICON TABLE
// 注意这里定义的COMx编号并不是LCD引脚定义的COMx，而是指实际对应的显示位，个位为COM1，十位为COM2...以此类推。
// 以AU6212CA对应的5*8段码LCD屏为例，此处定义的COM1为LCD屏资料上的COM4。
//对于 SEG数大于8的LCD屏:高3(bit15--bit13)定义为COMx编号，低13位(bit12--bit0) 定义为SEGx编号。
#ifdef AU6210K_BOOMBOX_DEMO
#define ICON58_L1		0xFF
#define ICON58_L2		0xFF
#define ICON58_L3		0xFF
#define ICON58_L4		0xFF
#define ICON58_RAN		0xFF
#define ICON58_INTO		0xFF
#define ICON58_REP		0xFF
#define ICON58_MUTE	   	0xFF
#define ICON58_FM_MHZ	((0 << 13) + (1 << 5))	//COM1 SEG6
#define ICON58_USB		((1 << 13) + (1 << 10))	//COM2 SEG11
#define ICON58_SD		((1 << 13) + (1 << 9))	//COM2 SEG10
#define ICON58_COLON	((0 << 13) + (1 << 3))	//COM1 SEG4
#define ICON58_STERO	((3 << 13) + (1 << 8))	//COM4 SEG8
#define ICON58_MEMORY	((3 << 13) + (1 << 10))	//COM4 SEG10
#define ICON58_ALARM1	((0 << 13) + (1 << 10))	//COM1 SEG10
#define ICON58_ALARM2	((0 << 13) + (1 << 9))	//COM1 SEG9

#else
#define ICON58_L1		(4 * 8 + SEGA_BITNO)	//COM5 SEGA
#define ICON58_L2		(4 * 8 + SEGB_BITNO)	//COM5 SEGB
#define ICON58_L3		(4 * 8 + SEGC_BITNO) 	//COM5 SEGC
#define ICON58_L4		(4 * 8 + SEGD_BITNO)	//COM5 SEGD
#define ICON58_RAN		(4 * 8 + SEGE_BITNO)	//COM5 SEGE
#define ICON58_INTO		(4 * 8 + SEGF_BITNO)    //COM5 SEGF
#define ICON58_REP		(4 * 8 + SEGG_BITNO)	//COM5 SEGG
#define ICON58_MUTE	   	(4 * 8 + SEGH_BITNO)	//COM5 SEGH
#define ICON58_FM_MHZ	(0 * 8 + SEGH_BITNO)	//COM1(个) SEGH
#define ICON58_USB		(1 * 8 + SEGH_BITNO)	//COM2(十) SEGH
#define ICON58_SD		(2 * 8 + SEGH_BITNO)	//COM3(百) SEGH
#define ICON58_COLON	(3 * 8 + SEGH_BITNO)	//COM4(千) SEGH
#endif

typedef enum _ICON_IDX
{
    ICON_PLAY = 0,  	// 0
    ICON_PAUSE,	  		// 1
    ICON_COLON,	 	 	// 2
    ICON_USB,		  	// 3
    ICON_SD,			// 4
    ICON_DP_FM_MHZ,		//5
    ICON_MP3,		  	//6
    ICON_REP,		  	//7
    ICON_RAN,		  	//8
    ICON_INTO,	  		//9
    ICON_MUTE,	  		//10
    ICON_L1,		  	//11
    ICON_L2,		  	//12
    ICON_L3,		  	//13
    ICON_L4,		  	//14
    ICON_SLEEP,			//15
    ICON_BATFUL,		//16
    ICON_BATHAF,		//17
#ifdef AU6210K_BOOMBOX_DEMO
    ICON_STERO,
    ICON_MEMORY,
    ICON_ALARM1,
    ICON_ALARM2,
#endif
    
    ICON_ALL,
    ICON_1,
    ICON_FOLDER,
    ICON_PROG,
    ICON_NUM

} ICON_IDX;

typedef enum _ICON_STATU
{
    LIGHT_OFF,
    LIGHT_ON,
    LIGHT_BLINK

} ICON_STATU;

//extern DISP_TYPE gDispBuff[];
extern const uint8_t  DispIconSet[];
extern const DISP_TYPE  DispCharSet[];

bool DispChar(DISP_TYPE Char, uint8_t DisCharPos);
void DispString(uint8_t* Str);
void DispIcon(ICON_IDX Icon, ICON_STATU Light);
void Num2Char(uint16_t Num, bool ZeroFill);
#endif

#endif
