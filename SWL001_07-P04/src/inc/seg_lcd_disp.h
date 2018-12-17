#ifndef __SEG_LCD_DISP_H__
#define __SEG_LCD_DISP_H__
#include "app_config.h"
#include "seg_panel.h"


#ifdef FUNC_SEG_LCD_EN

typedef enum _AMPL_CURVE
{
	Ampl_Zero,
	Ampl_One,
	Ampl_Two,
	Ampl_Three,
	Ampl_Four
		
} AMPL_CURVE;


void DispLcdIcon(ICON_IDX Icon, ICON_STATU Light);
void LcdDispDevSymbol(void);
void LcdDispRepeat(void);
void LcdDispPlayState(void);
void LcdFlushDisp(void);
void LcdFlushDispData(void);
#endif

#endif
