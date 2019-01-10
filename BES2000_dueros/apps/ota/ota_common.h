#ifndef __OTA_COMMON_H
#define __OTA_COMMON_H

/* Debug switch */
#define OTA_SPP_DEBUGx
#define OTA_CONTROL_DEBUGx
#define OTA_MAIN_DEBUGx
#define OTA_ME_DEBUGx
#define OTA_BLE_DEBUGx
/*-----------------------*/


#define SECURITY_PIN_CODE  0x00
#define SECURITY_SSP       0x01

#define OTA_SECURITY_MODE SECURITY_SSP


/*-----------------------*/
#define BD_ADDR_LEN 6 

#define STREAM_TO_UINT8(u8, p)   {u8 = (uint8_t)(*(p)); (p) += 1;}
#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#define STREAM_TO_UINT32(u32, p) {u32 = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24)); (p) += 4;}

#define STREAM_TO_BDADDR(a, p)   {register int ijk; register uint8_t *pbda = (uint8_t *)a + BD_ADDR_LEN - 1; for (ijk = 0; ijk < BD_ADDR_LEN; ijk++) *pbda-- = *p++;}

#define LEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)+1) << 8) | \
                                ((U16) *((U8*)(ptr))) )

#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}

#define BDADDR_TO_STREAM(p, a)   {register int ijk; for (ijk = 0; ijk < BD_ADDR_LEN;  ijk++) *(p)++ = (uint8_t) a[BD_ADDR_LEN - 1 - ijk];}

#define OTA_WATCH_DOG_PING_TIMER_ID 0

#define OTA_POWER_OFF_TIMEOUT_IN_SECONDS	(5*60)
#define OTA_WD_PING_PERIOD_IN_SECONDS		(5)

#define UNUSED(x) (void)(x)

#define ROLE_SLAVE 1


#endif

