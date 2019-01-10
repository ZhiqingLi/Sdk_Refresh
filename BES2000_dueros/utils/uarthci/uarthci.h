/***
 * uart hci
 * YuLongWang @2015
*/

#ifndef UARTHCI_H
#define UARTHCI_H

#if defined(__cplusplus)
extern "C" {
#endif

void BESHCI_BufferAvai(void *packet);
void BESHCI_Open(void);
void BESHCI_Poll(void);
void BESHCI_LockBuffer(void);
void BESHCI_UNLockBuffer(void);
void BESHCI_SendData(void *packet);
void BESHCI_SCO_Data_Start(void);
void BESHCI_SCO_Data_Stop(void);
void uartrxtx(void const *argument);

#if defined(__cplusplus)
}
#endif

#endif /* UARTHCI_H */
