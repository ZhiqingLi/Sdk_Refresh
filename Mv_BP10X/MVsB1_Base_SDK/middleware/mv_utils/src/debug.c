
#include <debug.h>
#include "type.h"

void LedPortInit(void)
{
#ifdef LED1_PORT
	LedOff(1);
	GPIO_RegOneBitSet(PORT_OE_REG(LED1_PORT), LED1_INDEX);
	GPIO_RegOneBitClear(PORT_IE_REG(LED1_PORT), LED1_INDEX);
#endif //LED1_PORT

#ifdef LED2_PORT
	LedOff(2);
	GPIO_RegOneBitSet(PORT_OE_REG(LED2_PORT), LED2_INDEX);
	GPIO_RegOneBitClear(PORT_IE_REG(LED2_PORT), LED2_INDEX);
#endif //LED1_PORT

#ifdef LED3_PORT
	LedOff(3);
	GPIO_RegOneBitSet(PORT_OE_REG(LED3_PORT), LED3_INDEX);
	GPIO_RegOneBitClear(PORT_IE_REG(LED3_PORT), LED3_INDEX);
#endif //LED1_PORT
#ifdef LED4_PORT
	LedOff(4);
	GPIO_RegOneBitSet(PORT_OE_REG(LED4_PORT), LED4_INDEX);
	GPIO_RegOneBitClear(PORT_IE_REG(LED4_PORT), LED4_INDEX);//led
#endif //LED1_PORT

}


void LedOn(uint8_t Index)
{
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
		#if !LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
		#endif	
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
		#if !LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
		#endif	
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
		#if !LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
		#endif
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
		#if !LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
		#endif
			break;
#endif //#ifdef LED4_PORT


	}
}

void LedOff(uint8_t Index)
{
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
		#if LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
		#endif	
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
		#if LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
		#endif	
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
		#if LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
		#endif
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
		#if LED_ON_LEVEL
			GPIO_RegOneBitClear(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
		#else
			GPIO_RegOneBitSet(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
		#endif
			break;
#endif //#ifdef LED4_PORT
	}
}

void LedToggle(uint8_t Index)
{
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED1_PORT), LED1_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
			}
			else
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
			}
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED2_PORT), LED2_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
			}
			else
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
			}
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED3_PORT), LED3_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
			}
			else
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
			}
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED4_PORT), LED4_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
			}
			else
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
			}
			break;
#endif //#ifdef LED4_PORT

	}
}

void LedPortRise(uint8_t Index)
{
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
			if(!GPIO_RegOneBitGet(PORT_OUT_REG(LED1_PORT), LED1_INDEX))
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
			}
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
			if(!GPIO_RegOneBitGet(PORT_OUT_REG(LED2_PORT), LED2_INDEX))
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
			}
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
			if(!GPIO_RegOneBitGet(PORT_OUT_REG(LED3_PORT), LED3_INDEX))
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
			}
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
			if(!GPIO_RegOneBitGet(PORT_OUT_REG(LED4_PORT), LED4_INDEX))
			{
				GPIO_RegOneBitSet(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
			}
			break;
#endif //#ifdef LED4_PORT

	}
}

void LedPortDown(uint8_t Index)
{
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED1_PORT), LED1_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
			}
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED2_PORT), LED2_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
			}
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED3_PORT), LED3_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
			}
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
			if(GPIO_RegOneBitGet(PORT_OUT_REG(LED4_PORT), LED4_INDEX))
			{
				GPIO_RegOneBitClear(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
			}
			break;
#endif //#ifdef LED4_PORT

	}
}

bool LedPortGet(uint8_t Index)
{
	bool ret = FALSE;
	switch(Index)
	{
#ifdef LED1_PORT
		case 1:
			ret = GPIO_RegOneBitGet(PORT_OUT_REG(LED1_PORT), LED1_INDEX);
			break;
#endif //#ifdef LED1_PORT
		
#ifdef LED2_PORT
		case 2:
			ret = GPIO_RegOneBitGet(PORT_OUT_REG(LED2_PORT), LED2_INDEX);
			break;
#endif //#ifdef LED2_PORT
		
#ifdef LED3_PORT
		case 3:
			ret = GPIO_RegOneBitGet(PORT_OUT_REG(LED3_PORT), LED3_INDEX);
			break;
#endif //#ifdef LED3_PORT

#ifdef LED4_PORT
		case 4:
			ret = GPIO_RegOneBitGet(PORT_OUT_REG(LED4_PORT), LED4_INDEX);
			break;
#endif //#ifdef LED4_PORT

	}
	return ret;
}


