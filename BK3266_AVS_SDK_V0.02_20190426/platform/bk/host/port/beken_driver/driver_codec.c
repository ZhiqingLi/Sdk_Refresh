#include "driver_beken_includes.h"

#ifdef CONFIG_DRIVER_I2S

#define CODEC_DATAWIDTH_16      3
#define CODEC_DATAWIDTH_18      2
#define CODEC_DATAWIDTH_20      1
#define CODEC_DATAWIDTH_24      0
#define CODEC_DATAWIDTH_32      4

#define CODEC_SAMPLERATE_8K     0xa
#define CODEC_SAMPLERATE_48K    0x2
#define CODEC_SAMPLERATE_16K    0x6
#define CODEC_SAMPLERATE_441K    0x2

static void codec_command_set(unsigned char reg, unsigned char val)
{
    i2c_fm_sync_wr_reg(reg,  &val, 1);
    while( iic_fm_exception() );

    return;
}

void codec_init(void)
{
    codec_command_set(0x08, 0x40);  //slave

    codec_command_set(0x02, 0xF3);     //reset state machine/pll/ filter etc
    codec_command_set(0x2b, 0x80);
    codec_command_set(0x00, 0x06);   //PLAYBACK & RECORD Mode, EnRefr=1

    codec_command_set(0x01, 0x72);
    codec_command_set(0x2d, 0x00);   //vroi=1, 40k

    Delay(1000);

    codec_command_set(0x03, 0x00);   //pdn_ana=0, ibiasgen_pdn=0  adc power up
    codec_command_set(0x04, 0x0C);   //pdn_ana=0, ibiasgen_pdn=0 dac power up

//--------------------------------------------------------------
//DAC CONTROL 寄存器组
//--------------------------------------------------------------
    codec_command_set(0x19, 0x72);  //SOFT RAMP RATE=32LRCKS/STEP, Enable ZERO-CROSS CHECK, DAC MUTE	
    codec_command_set(0x1a, 0x00);  //DAC Left Volume=-1db
    codec_command_set(0x1b, 0x00);  //DAC Right Volume=-1db
    
    codec_command_set(0x27, 0xB8);  //Left DAC TO Left MIXER
    codec_command_set(0x28, 0x38);  
    codec_command_set(0x29, 0x38);  
    codec_command_set(0x2A, 0xB8);  //RIGHT DAC TO RIGHT MIXER

//---------------------------------------------------------------
//ADC Control 寄存器组
//---------------------------------------------------------------
/*---------------------------------------------------
下面寄存器0x09, 0x0a的设置可根据用户的实际需要去修改
----------------------------------------------------*/
    codec_command_set(0x09, 0x66);  //ADC L/R PGA = 9dB
    codec_command_set(0x0A, 0xf0);  //ADC左/右通道输入选择为LIN1/RIN1
    codec_command_set(0x0C, 0x4C); // I2s 16bit mode
    codec_command_set(0x0d, 0x02); //ADC MCLK/LRCK=1536, 8kHz	 
    codec_command_set(0x0f, 0x70); //Softramp enable and Ramp rate =0.5db/32LRCKs    
    codec_command_set(0x10, 0x02);  //ADC Left Volume=-1db
    codec_command_set(0x11, 0x02);  //ADC Right Volume=-1db

#if 1
    codec_command_set(0x12, 0xd2);  
    codec_command_set(0x13, 0x80);  
    codec_command_set(0x14, 0x12);  
    codec_command_set(0x15, 0x06);   
    codec_command_set(0x16, 0xdb);   
#endif

    codec_volume_control(0x1E);

    codec_command_set(0x02,0x00);  //START DLL and state-machine,START DSM
}



int codec_cfg(unsigned int fs, unsigned char datawidth)
{
    uint8 reg_mode = 0;
    int i;

    switch(datawidth)
    {
        case 18:
            reg_mode = CODEC_DATAWIDTH_18;
            break;
        case 20:
            reg_mode = CODEC_DATAWIDTH_20;
            break;
        case 24:
            reg_mode = CODEC_DATAWIDTH_24;
            break;
        case 32:
            reg_mode = CODEC_DATAWIDTH_32;
            break;
        case 16:
        default:
            reg_mode = CODEC_DATAWIDTH_16;
            break;
    }

    codec_command_set(0x17, reg_mode << 3);

    switch(fs)
    {
        case 8000:
            reg_mode = CODEC_SAMPLERATE_8K;
            break;
        case 16000:
            reg_mode = CODEC_SAMPLERATE_16K;
            break;
        case 48000:
            reg_mode = CODEC_SAMPLERATE_48K;
            break;
        case 44100:
        default:
            reg_mode = CODEC_SAMPLERATE_441K;
            break;
    }
    
    codec_command_set(0x18, reg_mode);
  
    return 0;
}


void codec_open(void)
{
/*--------------------------------------------------------
主芯片先输出I2S时钟，等到I2S时钟信号频率稳定之后，再配置DAC
---------------------------------------------------------*/ 
    codec_command_set(0x02, 0xF3);  //START DLL and state-machine,START DSM
    codec_command_set(0x04, 0x0C);   //pdn_ana=0, ibiasgen_pdn=0 dac power up
    codec_command_set(0x02,0x00);  //START DLL and state-machine,START DSM
}


void codec_close(void)
{
    codec_command_set(0x02,0xF3); //R2=0XF3,这一步是必须的 先停止ES8350/ES8331的ADC,DAC的CLOCK和状态机,然后才可以设置ES8350的各种参数
    codec_command_set(0x00,0x05); //置ES8350于PLAYBACK & RECORD 模式，且EnRefr=1
    codec_command_set(0x01,0x40); //pdn_ana=0,ibiasgen_pdn=0
    codec_command_set(0x03,0xff); //ADC Power-dn///////////?????????????
    codec_command_set(0x04,0xc0); //DAC Power-DN 
}


void codec_volume_control(unsigned char volume)
{
    codec_command_set(0x30, volume);
    codec_command_set(0x31, volume);
    codec_command_set(0x2E, volume);
    codec_command_set(0x2F, volume);

    return;
}


void codec_adc_enable(uint8 enable)
{
    if(enable)
    {
        codec_command_set(0x02, 0xA8); //R2=0XF3,这一步是必须的 先停止ES8350/ES8331的ADC,DAC的CLOCK和状态机,然后才可以设置ES8350的各种参数
        codec_command_set(0x03, 0x00);
        codec_command_set(0x02, 0x00);
    }
    else
    {
        codec_command_set(0x02, 0xA8);
        codec_command_set(0x03, 0xFF);
    }
}

void codec_volume_mute(uint8 enable)
{
    if(enable)
        codec_command_set(0x19, 0x76);
    else
        codec_command_set(0x19, 0x72);
}


void codec_adc_volume_adjust(uint8 volume)
{
    codec_command_set(0x10, volume);
    codec_command_set(0x11, volume);
}


#endif

