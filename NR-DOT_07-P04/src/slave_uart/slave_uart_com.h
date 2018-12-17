#ifndef _SLAVE_UART_COM_H_
#define _SLAVE_UART_COM_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
	
extern uint16_t MasterModeState;
extern uint16_t MasterPowerState;

void Slave_UartInit(void);
void Slave_SendCmdToBt(uint8_t McuCmd);
void Slave_CmdProcess(void);
	
#ifdef __cplusplus
}
#endif//__cplusplus

#endif

