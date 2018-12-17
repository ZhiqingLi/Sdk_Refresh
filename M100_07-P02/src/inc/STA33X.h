#ifndef DDX_33X_H
#define DDX_33X_H

#define DDX_I2C_ADDR							    0x38			

#define DDX_CONFIGURE_A								0x00	
#define DDX_CONFIGURE_B								0x01  
#define DDX_CONFIGURE_C								0x02	
#define DDX_CONFIGURE_D								0x03	
#define DDX_CONFIGURE_E								0x04
#define DDX_CONFIGURE_F								0x05

#define DDX_MUTE									0x06	
#define DDX_MAIN_VOLUME								0x07
#define DDX_CHANNEL1_VOL							0x08	
#define DDX_CHANNEL2_VOL							0x09	
#define DDX_CHANNEL3_VOL 							0x0a

#define DDX_AUTO1			 						0x0b
#define DDX_AUTO2			 						0x0c
//#define DDX_AUTO3			 						0x0d

#define DDX_CHANNEL1_CFG 							0x0e
#define DDX_CHANNEL2_CFG 							0x0f
#define DDX_CHANNEL3_CFG 							0x10

#define DDX_TONEBASS								0x11
#define DDX_L1AR_RATE								0x12	
#define DDX_L1AR_THRESHOLD							0x13
#define DDX_L2AR_RATE								0x14	
#define DDX_L2AR_THRESHOLD					        0x15

#define STA33X_Cross_userdefine 0x00
#define STA33X_Cross_80Hz 		0x01
#define STA33X_Cross_100Hz 		0x02
#define STA33X_Cross_120Hz 		0x03
#define STA33X_Cross_140Hz 		0x04
#define STA33X_Cross_160Hz 		0x05
#define STA33X_Cross_180Hz 		0x06
#define STA33X_Cross_200Hz 		0x07
#define STA33X_Cross_220Hz 		0x08
#define STA33X_Cross_240Hz 		0x09
#define STA33X_Cross_260Hz 		0x0A
#define STA33X_Cross_280Hz 		0x0B
#define STA33X_Cross_300Hz 		0x0C
#define STA33X_Cross_320Hz 		0x0D
#define STA33X_Cross_340Hz 		0x0E
#define STA33X_Cross_360Hz 		0x0F


void		STA33X_init(void);
void    ExMuteOn(void);
void    ExMuteOff(void);
void 		STA33X_OutputConfiguration(unsigned char DDX_Configuration);
void 		STA33X_SetMasterVolume(unsigned char MasterVolume);
void 		STA33X_SetMasterMute(unsigned char Mute);
void 		STA33X_SetLeftVolume(unsigned char LeftVolume);
void 		STA33X_SetRightVolume(unsigned char RightVolume);
void 		STA33X_SetSubWooferVolume(unsigned char SubWooferVolume);
void 		STA33X_CrossOver(unsigned char DDX_CrossOverValue);
void 		STA33X_Poweronoff(unsigned char DDX_Powerflag);
void 		STA33X_Powerdownonoff(unsigned char DDX_Powerflag);
void 		STA33X_DSPBypass(unsigned char DSPBypassFlag);
void 		STA33X_DeEmphasis(unsigned char DeEmphasisFlag);
void 		STA33X_FilterLink(unsigned char FilterlinkFlag);
void 		STA33X_PostscaleLink(unsigned char PostscalelinkFlag);
void 		STA33X_Bass(unsigned char basssetting);
void 		STA33X_Treble(unsigned char treblesetting);
void        STA33X_SetVolume(unsigned char volume);
bool    STA33X_MuteControl(bool Mute);
//void 		STA33X_CoefficientWrite(unsigned char FilterIndex);
//void 		STA33X_CoefficientRead(unsigned char FilterIndex);
#endif
