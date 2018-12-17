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
void Mcu_SendCmdToWiFi(uint16_t McuCmd, uint8_t* DataBuf);
void WiFi_CmdProcess(void);
void  WiFi_SoundRemind(uint16_t SoundID);
	
#ifdef __cplusplus
}
#endif//__cplusplus

#endif

