/*--------------------------------------------------------------------------*/
/**@file     bk1080.h
   @brief    BK1080 ’“Ù
   @details
   @author
   @date   2011-3-30
   @note
*/
/*----------------------------------------------------------------------------*/
#ifdef	FM_BK1080H

#ifndef _BK_1080_H_
#define _BK_1080_H_

//#define XTAL_CLOCK			0
#define CHIP_DEV_ID 		0x80


void init_BK1080(void);
void BK1080_SetFreq(uint16 curFreq);
uint8 set_fre_BK1080(uint16 freq);
void BK1080_PowerDown(void);
void BK1080_mute(uint8 flag);
uint8 BK1080_Read_ID(void);
void BK1080_setch(uint8 db);
void BK1080_set_vol(uint8 vol) ;

//#define MIN_FRE 875

#endif		/*  _BK_1080_H_ */
#endif
