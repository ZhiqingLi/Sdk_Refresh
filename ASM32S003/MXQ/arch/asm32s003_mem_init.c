/*
  ******************************************************************************
  * @file    asm32s003_mem_init.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "string.h"

/**
  * @addtogroup ASM32S003_StdPeriph_Driver
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
extern char _end_rodata[];
extern char _start_data[];
extern char _end_data[];

extern char _bss_start[];
extern char _ebss[];

/**
  * @addtogroup mem init Functions
  * @{
  */

/**
  * @brief  
  * @param  
  * @retval 
  */
void __main( void ) 
{
	  char *dst = _start_data;
	  char *src = _end_rodata;

	  /* if the start of data (dst)
		 is not equal to end of text (src) then
		 copy it, else it's already in the right place
		 */
	  if( _start_data != _end_rodata )
	  {
	//    __memcpy_fast( dst, src, (_end_data - _start_data));
		memcpy( dst, src, (_end_data - _start_data));
	  }

	  /* zero the bss 
	   */
	  if( _ebss - _bss_start ) 
	  {
	//    __memset_fast( _bss_start, 0x00, ( _ebss - _bss_start ));
		memset( _bss_start, 0x00, ( _ebss - _bss_start ));
	  }
}

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/