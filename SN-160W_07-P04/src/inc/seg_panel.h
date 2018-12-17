#ifndef __SEG_PANEL_H__
#define __SEG_PANEL_H__

#if 1 //(defined(FUNC_SEG_LED_EN) || defined(FUNC_SEG_LCD_EN))

#define MAX_SEG_NUM		8	//8
#define MAX_COM_NUM		8

#if (MAX_SEG_NUM > 8) //SEG数目大于8时，一个COM需要的存储空间为2个字节，应该定义为WORD类型。
#define DISP_TYPE	uint16_t
#else
#define DISP_TYPE	uint8_t
#endif


#define POS_COM_THOUSAND	0	// COM0	// D6
#define POS_COM_HUNDREDS	1	// COM1	// D5
#define POS_COM_TENS		2	// COM2	// D4
#define POS_COM_UNITS		3	// COM3	// D3


//SEG定义
#if defined(FUNC_AIP1629A_LED_EN)
#define SEGA_BITNO	5
#define SEGB_BITNO	4
#define SEGC_BITNO	3
#define SEGD_BITNO	2
#define SEGE_BITNO	1
#define SEGF_BITNO	0
#define SEGG_BITNO	6
#define SEGH_BITNO	7

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
#define	CHR_I	(SEG_B | SEG_C)
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

// LED8888/LED1888 ICON TABLE
#if defined(FUNC_AIP1629A_LED_EN)
#define	ICON48_COLON	(1*8+SEGC_BITNO)	//GRID1 SEG12 = 1*8+3(01H，BIT3)
#define	ICON48_DOTNO	(1*8+SEGA_BITNO)	//GRID1 SEG14 = 1*8+5(01H，BIT5)
#define	ICON48_A1MARK	(10*8+SEGH_BITNO)	//GRID6 SEG8 = 10*8+7(0AH，BIT7)
#define	ICON48_A2MARK	(11*8+SEGC_BITNO)	//GRID6 SEG12 = 11*8+3(0BH，BIT3)
#define	ICON48_ALARM1	(11*8+SEGE_BITNO)	//GRID6 SEG10 = 11*8+1(0BH，BIT1)
#define	ICON48_ALARM2	(11*8+SEGG_BITNO)	//GRID6 SEG12 = 11*8+6(0BH，BIT6)
#define	ICON48_A1LOOP	(11*8+SEGD_BITNO)	//GRID6 SEG11 = 11*8+2(0BH，BIT2)
#define	ICON48_A2LOOP	(11*8+SEGA_BITNO)	//GRID6 SEG14 = 11*8+5(0BH，BIT5)
#define	ICON48_24H_12H	(15*8+SEGE_BITNO)	//GRID8 SEG10 = 15*8+1(0FH，BIT1)
#define	ICON48_AUX  	(13*8+SEGA_BITNO)	//GRID7 SEG14 = 13*8+5(0DH，BIT5)
#define	ICON48_TF 		(14*8+SEGH_BITNO)	//GRID8 SEG8 = 14*8+7(0EH，BIT7)
#define	ICON48_BLUE 	(13*8+SEGE_BITNO)	//GRID7 SEG10 = 13*8+1(0DH，BIT1)
#define	ICON48_USB  	(12*8+SEGH_BITNO)	//GRID7 SEG8 = 12*8+7(0CH，BIT7)
#define	ICON48_FM   	(13*8+SEGC_BITNO)	//GRID7 SEG12 = 13*8+3(0DH，BIT3)
#define	ICON48_BAT1   	(9*8+SEGE_BITNO)	//GRID5 SEG10 = 9*8+1(09H，BIT1)
#define	ICON48_BAT2   	(9*8+SEGD_BITNO)	//GRID5 SEG11 = 9*8+2(09H，BIT2)
#define	ICON48_BAT3   	(9*8+SEGC_BITNO)	//GRID5 SEG12 = 9*8+3(09H，BIT3)
#define	ICON48_BATFUL   (9*8+SEGB_BITNO)	//GRID5 SEG13 = 9*8+4(09H，BIT4)

#else
// LED5*7 ICON TABLE
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
#endif

#ifdef FUNC_AIP1629A_LED_EN
typedef enum _ICON_IDX
{
    ICON_USB = 0,  	   // 0
    ICON_SD,	       // 1
    ICON_AUX,	       // 2
    ICON_FM,	       // 3
    ICON_BT,	       // 4
    ICON_MHZ,	       // 5
    ICON_BAT1,	       // 6
    ICON_BAT2,	       // 7
    ICON_BAT3,	       // 8
    ICON_BATFUL,       // 9
    ICON_DP,	       // 10
    ICON_COLON,	       // 11
    ICON_WIFI,          // 12
    ICON_PLAY,  	   // 13
    ICON_PAUSE,	  	   // 14
    ICON_DP_FM_MHZ,	   // 15
    ICON_24_12H,	   // 16 
    ICON_NULL,		   // 17
    ICON_RAN,		   // 18
    ICON_INTO,	  	   // 19
    ICON_MUTE,	  	   // 20
    ICON_SLEEP,		   // 21
    ICON_A1MARK,	   // 22
    ICON_A2MARK,	   // 23
    ICON_ALARM1,       // 24
    ICON_ALARM2,       // 25
    ICON_A1LOOP,       // 26
    ICON_A2LOOP,       // 27
    ICON_NUM

} ICON_IDX;

#else

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
    ICON_STERO,
    ICON_MEMORY,
    ICON_ALARM1,
    ICON_ALARM2,
    ICON_ALL,
    ICON_1,
    ICON_FOLDER,
    ICON_PROG,
    ICON_NUM

} ICON_IDX;

#endif
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
