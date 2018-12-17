#ifndef	__PACFG339_H__
#define	__PACFG339_H__

#define  DEVICE_ADDR1	 0x38
#define  DEVICE_ADDR2	 0x3a

#define  CONFIG_A        0x00
#define  CONFIG_B        0x01
#define  CONFIG_C        0x02
#define  CONFIG_D        0x03
#define  CONFIG_E        0x04
#define  CONFIG_F        0x05
#define  MUTE_LOC        0x06
#define  MAIN_VOL        0x07
#define  CHN1_VOL        0x08
#define  CHN2_VOL        0x09
#define  CHN3_VOL        0x0a
#define  AUTO1           0x0b
#define  AUTO2           0x0c
#define  AUTO3           0x0d
#define  CHN1_CFG        0x0e
#define  CHN2_CFG        0x0f
#define  CHN3_CFG        0x10
#define  TONE_BASS       0x11
#define  L1AR_RATE       0x12
#define  L1AR_THLD       0x13
#define  L2AR_RATE       0x14
#define  L2AR_THLD       0x15

#define  CHN3_VOL        0x0a


//PA
//void IIS_Init(void);
void SetPAvol(uint8 vol);
void SetBassCro(uint8 vol);
void SetSub(uint8 vol);
void PAinit(void);
extern void Set_Device_sfr(uint8 reg,uint8 *data,uint8 set_len,uint8 addr);


#endif
