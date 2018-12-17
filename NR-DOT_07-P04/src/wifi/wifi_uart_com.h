#ifndef _WIFI_UART_COM_H_
#define _WIFI_UART_COM_H_

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

extern bool WiFiDataRcvStartFlag; 
extern int8_t SlaveRxIndex;
extern bool RxDataEnableFlag;

void WiFi_UartInit(void);
void ClearRxQueue(void);
void Mcu_TimerPollingRcvUartData(void);
void Mcu_SendCmdToWiFi(uint8_t McuCmd, uint8_t* DataBuf);
void WiFi_CmdProcess(void);
void WiFiSetMicState(uint16_t State);
	
#ifdef __cplusplus
}
#endif//__cplusplus

#endif

