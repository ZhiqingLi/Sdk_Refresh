#include "include.h"
#include "bsp_i2c.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if I2S_DEVICE == I2S_DEV_WM8978 && I2S_EN

u16 wm8978_reg[58];

static const u16 tbl_wm8978_reg[58] = {
	0x0000,0x0000,0x0000,0x0000,0x0050,0x0000,0x0140,0x0000,
	0x0000,0x0000,0x0000,0x00FF,0x00FF,0x0000,0x0100,0x00FF,
	0x00FF,0x0000,0x012C,0x002C,0x002C,0x002C,0x002C,0x0000,
	0x0032,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0038,0x000B,0x0032,0x0000,0x0008,0x000C,0x0093,0x00E9,
	0x0000,0x0000,0x0000,0x0000,0x0003,0x0010,0x0010,0x0100,
	0x0100,0x0002,0x0001,0x0001,0x0039,0x0039,0x0039,0x0039,
	0x0001,0x0001
};

AT(.text.bsp.i2s)
static void bsp_wm8978_write_one_byte(u8 addr, u8 dat)
{
	bsp_i2c_start();
	bsp_i2c_tx_byte(0x34);
	bsp_i2c_rx_ack();
	bsp_i2c_tx_byte(addr);
    bsp_i2c_rx_ack();
	bsp_i2c_tx_byte(dat);
    bsp_i2c_rx_ack();
	bsp_i2c_stop();
}

AT(.text.bsp.i2s)
static void bsp_wm8978_sfr_write(u8 addr, u16 dat)
{
    bsp_wm8978_write_one_byte((addr << 1) | ((dat >> 8) & 0x01), (u8)dat);
    wm8978_reg[addr] = dat;
}

AT(.text.bsp.i2s)
static u16 bsp_wm8978_sfr_read(u8 addr)
{
    return wm8978_reg[addr];
}

AT(.text.bsp.i2s)
static void bsp_wm8978_reset(void)
{
    bsp_wm8978_sfr_write(0, 0);
    memcpy(wm8978_reg, tbl_wm8978_reg, sizeof(tbl_wm8978_reg));
}

//设置喇叭音量, voll:左声道音量(0~63)
void bsp_wm8978_speaker_vol(u8 vol)
{
	vol &= 0x3f;
	if (vol ==0) {
            vol |= 1<<6;                    //音量为0时,直接mute
	}
 	bsp_wm8978_sfr_write(54, vol);			//R54,喇叭左声道音量设置
	bsp_wm8978_sfr_write(55, vol | (1<<8));	//R55,喇叭右声道音量设置,同步更新(SPKVU=1)
}

//设置耳机左右声道音量
//voll:左声道音量(0~63)
//volr:右声道音量(0~63)
void bsp_wm8978_hp_vol(u8 voll, u8 volr)
{
	voll &= 0x3f;
	volr &= 0x3f;						    //限定范围
	if (voll == 0) {
        voll |= 1<<6;				        //音量为0时,直接mute
	}
	if (volr == 0) {
        volr |= 1<<6;					    //音量为0时,直接mute
	}
	bsp_wm8978_sfr_write(52, voll);		    //R52,耳机左声道音量设置
	bsp_wm8978_sfr_write(53, volr|(1<<8));  //R53,耳机右声道音量设置,同步更新(HPVU=1)
}

//WM8978 MIC增益设置(不包括BOOST的20dB,MIC-->ADC输入部分的增益)
//gain:0~63,对应-12dB~35.25dB,0.75dB/Step
void bsp_wm8978_mic_gain(u8 gain)
{
	gain &= 0x3f;
	bsp_wm8978_sfr_write(45, gain);		    //R45,左通道PGA设置
	bsp_wm8978_sfr_write(46, gain|1<<8);    //R46,右通道PGA设置
}


//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位; 1,20位;2,24位; 3,32位;
AT(.text.bsp.i2s)
static void bsp_wm8978_i2s_cfg(u8 fmt, u8 len)
{
    bsp_wm8978_sfr_write(4, ((fmt & 0x03)<<3)|((len & 0x03)<<5));    //R4, WM8978工作模式设置
}

//WM8978 DAC/ADC配置
//adcen:adc使能(1)/关闭(0)
//dacen:dac使能(1)/关闭(0)
AT(.text.bsp.i2s)
void bsp_wm8978_audio_cfg(u8 dacen, u8 adcen)
{
	uint16_t regval;
	regval = bsp_wm8978_sfr_read(3);		//读取R3
	if (dacen) {
		regval |= 3 << 0;					//R3最低2个位设置为1,开启DACR&DACL
	} else {
		regval &= ~(3 << 0);				//R3最低2个位清零,关闭DACR&DACL.
	}

	bsp_wm8978_sfr_write(3, regval);		//设置R3

	regval = bsp_wm8978_sfr_read(2);		//读取R2
	if (adcen) {
		regval |= 3 << 0;			        //R2最低2个位设置为1,开启ADCR&ADCL
	} else {
		regval &= ~(3 << 0);			    //R2最低2个位清零,关闭ADCR&ADCL.
	}
	bsp_wm8978_sfr_write(2, regval);	    //设置R2
}

//WM8978 输出配置
//dacen:DAC输出(放音)开启(1)/关闭(0)
//bpsen:Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0)
void bsp_wm8978_output_cfg(u8 dacen, u8 bpsen)
{
	u16 regval = 0;
	if (dacen) {
		regval |= 1 << 0;				    //DAC输出使能
	}
	if (bpsen) {
		regval |= 1 << 1;				    //BYPASS使能
		regval |= 5 << 2;				    //0dB增益
	}
	bsp_wm8978_sfr_write(50, regval);	    //R50设置
	bsp_wm8978_sfr_write(51, regval);       //R51设置
}

//WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void bsp_wm8978_linein_gain(u8 gain)
{
	u16 regval;
	gain &= 0x07;
	regval = bsp_wm8978_sfr_read(47);		    //读取R47
	regval &= ~(7<<4);						    //清除原来的设置
 	bsp_wm8978_sfr_write(47, regval|gain<<4);   //设置R47
	regval = bsp_wm8978_sfr_read(48);		    //读取R48
	regval &= ~(7<<4);						    //清除原来的设置
 	bsp_wm8978_sfr_write(48, regval|gain<<4);   //设置R48
}

//WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void bsp_wm8978_aux_gain(u8 gain)
{
	uint16_t regval;
	gain &= 0x07;
	regval = bsp_wm8978_sfr_read(47);		    //读取R47
	regval &= ~(7<<0);						    //清除原来的设置
 	bsp_wm8978_sfr_write(47, regval|gain<<0);   //设置R47
	regval = bsp_wm8978_sfr_read(48);		    //读取R48
	regval &= ~(7<<0);						    //清除原来的设置
 	bsp_wm8978_sfr_write(48, regval|gain<<0);   //设置R48
}


//WM8978 输入通道配置
//micen:MIC开启(1)/关闭(0)
//lineinen:Line In开启(1)/关闭(0)
//auxen:aux开启(1)/关闭(0)
void bsp_wm8978_input_cfg(u8 micen, u8 lineinen, u8 auxen)
{
	uint16_t regval;
	regval = bsp_wm8978_sfr_read(2);	    //读取R2
	if (micen) {
		regval |= 3 << 2;					//开启INPPGAENR,INPPGAENL(MIC的PGA放大)
	} else {
		regval &= ~(3 << 2);			    //关闭INPPGAENR,INPPGAENL.
	}
 	bsp_wm8978_sfr_write(2, regval);	    //设置R2
	regval = bsp_wm8978_sfr_read(44);		//读取R44
	if (micen) {
		regval |= 3<<4|3<<0;			    //开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	} else {
		regval &= ~(3<<4|3<<0);			    //关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	}
	bsp_wm8978_sfr_write(44,regval);	    //设置R44
	if (lineinen) {
		bsp_wm8978_linein_gain(5);		    //LINE IN 0dB增益
	} else {
		bsp_wm8978_linein_gain(0);		    //关闭LINE IN
	}
	if (auxen) {
		bsp_wm8978_aux_gain(7);			    //AUX 6dB增益
	} else {
		bsp_wm8978_aux_gain(0);			    //关闭AUX输入
	}
}

AT(.text.bsp.i2s)
void bsp_wm8978_init(void)
{
    TRACE("wm8978 start...\n");
    I2C_SDA_SCL_OUT();
    I2C_SDA_H();
    delay_5ms(1);

    bsp_wm8978_reset();
#if  I2S_MODE_SEL == 0
	bsp_wm8978_sfr_write(1, 0x1B);		    //R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
	bsp_wm8978_sfr_write(2, 0x1B0);		    //R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	bsp_wm8978_sfr_write(3, 0x6C);          //R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能
	bsp_wm8978_sfr_write(6, 0);		        //R6,MCLK由外部提供
	bsp_wm8978_sfr_write(43, BIT(4));       //R43,INVROUT2反向,驱动喇叭
	bsp_wm8978_sfr_write(47, BIT(8));       //R47设置,PGABOOSTL,左通道MIC获得20倍增益
	bsp_wm8978_sfr_write(48, BIT(8));       //R48设置,PGABOOSTR,右通道MIC获得20倍增益
	bsp_wm8978_sfr_write(49, BIT(1));       //R49,TSDEN,开启过热保护
	bsp_wm8978_sfr_write(10, BIT(3));       //R10,SOFTMUTE关闭,128x采样,最佳SNR
	bsp_wm8978_sfr_write(14, BIT(3));       //R14,ADC 128x采样率

	bsp_wm8978_i2s_cfg(2, 3);		        //设置I2S接口模式，播放从设备不使用

	//播放设置
	bsp_wm8978_audio_cfg(1, 0);			    //开启DAC
	bsp_wm8978_input_cfg(0, 0, 0);	        //关闭输入通道
	bsp_wm8978_output_cfg(1, 0);		    //开启DAC输出

	bsp_wm8978_hp_vol(25, 25);
#else
	bsp_wm8978_sfr_write(1, 0x1B);		    //R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
	bsp_wm8978_sfr_write(2, 0x1B0);		    //R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	bsp_wm8978_sfr_write(3, 0x6C);          //R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能
	bsp_wm8978_sfr_write(6, 2 << 2 | 1);             //R6
	bsp_wm8978_sfr_write(43, BIT(4));       //R43,INVROUT2反向,驱动喇叭
	bsp_wm8978_sfr_write(47, BIT(8));       //R47设置,PGABOOSTL,左通道MIC获得20倍增益
	bsp_wm8978_sfr_write(48, BIT(8));       //R48设置,PGABOOSTR,右通道MIC获得20倍增益
	bsp_wm8978_sfr_write(49, BIT(1));       //R49,TSDEN,开启过热保护
	bsp_wm8978_sfr_write(10, BIT(3));       //R10,SOFTMUTE关闭,128x采样,最佳SNR
	bsp_wm8978_sfr_write(14, BIT(3));       //R14,ADC 128x采样率

	bsp_wm8978_i2s_cfg(2, 0);		        //设置I2S接口模式，播放从设备不使用

	//播放设置
	bsp_wm8978_audio_cfg(0, 1);			    //开启ADC
	bsp_wm8978_input_cfg(0, 1, 0);	        //开启输入通道
	bsp_wm8978_output_cfg(0, 1);		    //开启ADC
    //bsp_wm8978_speaker_vol(30);
//    bsp_wm8978_mic_gain(6);
#endif
}

#endif
