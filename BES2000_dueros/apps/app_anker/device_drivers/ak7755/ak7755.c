#include "bes_hal_board.h"
#include "hal_spi.h"
#include "hal_trace.h"
#include "cmsis_os.h"
#include "string.h"

#ifdef EXTRAL_CODEC_AK7755_ENABLE

#ifdef __FACTORY_AUDIO_TEST__
#define  AK7755_BYPASS_TEST 
#endif

//#define  AK7755_EQ_DEBUG

#define CRC_MAX_RETRAY_COUNT	(5)

#define MAX_VOL   16
#define MIN_VOL   0


#include "ak7755.h"
#include "ak7755_reg.h"
#include "ak7755_eq.h"

#define SPI_SND_BUF_LEN			(1024)
static uint8_t ak7755SndBuf[SPI_SND_BUF_LEN] = {0};

static osMutexId ak7755_write_mutex_id2 = NULL;

osMutexDef(ak7755_write_mutex);
static int8_t ak7755_local_volume = 5;
static uint8_t gAk7755_init_flag = 0;


struct HAL_SPI_CFG_T ak7755_spiCfg = {
    .clk_delay_half = true,
    .clk_polarity = true,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = false,
    .cs = 0,
    .rate = 1300000,
    .tx_bits = 8,
    .rx_bits = 8,
    .rx_frame_bits = 0,
};

//AK7755 复位
void ak7755_reset(void)
{
	ak7755_reset_ctrl(0);
	osDelay(1);
	ak7755_reset_ctrl(1);
}

int32_t ak7755_spi_init(void)
{
	int32_t ret = 0;
	
	ret = hal_spilcd_open(&ak7755_spiCfg);
	TRACE("%s: spi init return %d",__func__,ret);
	return ret;
}

int32_t ak7755_write(const uint8_t cmd, const uint8_t* pData, uint32_t dataLen)
{
	int32_t ret = 0;
	//TRACE("%s: %d",__func__,ak7755_write_mutex_id2);
	if(ak7755_write_mutex_id2)osMutexWait(ak7755_write_mutex_id2, osWaitForever);
	memset(ak7755SndBuf,0,SPI_SND_BUF_LEN);
	ak7755SndBuf[0] = cmd;
	memcpy(&ak7755SndBuf[1],pData,dataLen);
	ret = hal_spilcd_send(ak7755SndBuf,dataLen+1);
	if(ret<0){
		TRACE("------%s  spi write fail");
	}
	if(ak7755_write_mutex_id2)osMutexRelease(ak7755_write_mutex_id2);
	return ret;
}

int32_t ak7755_read(const uint8_t cmd, uint8_t* pData, uint32_t dataLen)
{
	int32_t ret = 0;
	uint8_t ak7755RcvBuf[10] = {0};
//	memset(ak7755RcvBuf,0,SPI_RCV_BUF_LEN);

	ret = hal_spilcd_recv(&cmd,ak7755RcvBuf,dataLen+1);
	//TRACE("%s: DUMP",__func__);
	//DUMP8("%02x ",ak7755RcvBuf,dataLen+1);
	if(ret >= 0)
	{
		memcpy(pData,&ak7755RcvBuf[1],dataLen);
	}
	return ret;
}

//AK7755初始化，
//执行复位
void ak7755_init(void)
{
	TRACE("%s: enter",__func__);
#ifdef AK7755_EQ_DEBUG
	hal_iomux_set_spilcd();
#endif
	ak7755_spi_init();

	ak7755_power_ctrl(1);

	ak7755_reset_ctrl(0);
	osDelay(10);
	ak7755_reset_ctrl(1);

	osDelay(100);

	ak7755_read_id();
	ak7755_control_register_write();
	TRACE("%s: exit",__func__);
}

int32_t ak7755_read_id(void)
{
	int32_t err = 0;
	uint8_t readID = 0;
	err = ak7755_read(AK7755_READ_IC,&readID,1);
	//TRACE("%s: readID = 0x%02x, err = %d",__func__,readID,err);
	if(err ==0 && readID == 0x55)
	{
		return 0;
	}
	return err;
}


int32_t ak7755_control_register_write(void)
{
	int32_t i=0,err=0;
	uint8_t* pRegPos;

	#ifdef AK7755_BYPASS_TEST
	pRegPos = (uint8_t*)ak7755_ctrl_bypass_t;
	#else
	pRegPos = (uint8_t*)ak7755_ctrl_t;
	#endif

	for(i=0;i<26;i++)
	{
		err=ak7755_write(*(pRegPos+ i*2),pRegPos + (i*2+1),1);
		if(err < 0)
			return err;
		osDelay(1);
	}
#if 0
	for(i=0;i<26;i++)
	{
		uint8_t tempChar = 0;
		ak7755_read(*(pRegPos + i*2)-0x80,&tempChar,1);
		
		TRACE("AK7755 Reg[%2x] = 0x%02x",*(pRegPos+ i*2),tempChar);
		osDelay(10);
	}
#endif
	return 0;
}

//AK7755时钟控制
//DISABLE = 0, ENABLE
//CKRESET  operation
#define CKRESET 0x01
int32_t ak7755_clock_control(uint8_t state)
{
	uint8_t cont01 = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT01,&cont01,1);
	if(err < 0)
		return err;
	if(state == 0)
	{
		cont01 = cont01 & (~CKRESET);
	}else
	{
		cont01 = cont01 | CKRESET;
	}

	err = ak7755_write(AK7755_W_CONT01,&cont01,1);
	return err;
}


//AK7755 Codec
//CONT0F CRESET operation
//codec(ADC & DAC) ON/OFF
#define CRESET_BIT 0x08
int32_t ak7755_codec_control(uint8_t state)
{
	uint8_t cont0f = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT0F,&cont0f,1);
	if(err < 0)
		return err;
	if(state == 0)
	{
		cont0f = cont0f & (~CRESET_BIT);
	}else
	{
		cont0f = cont0f | CRESET_BIT;
	}

	err = ak7755_write(AK7755_W_CONT0F,&cont0f,1);
	return err;
}

//CONTOF DSPRESET operation
//DSP ON/OFF
#define DSPRESET_BIT 0x04
int32_t ak7755_dsp_cmd(uint8_t state)
{
	uint8_t cont0f = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT0F,&cont0f,1);
	if(err < 0)
		return err;
	if(state == 0)
	{
		cont0f = cont0f & (~DSPRESET_BIT);
	}else
	{
		cont0f = cont0f | DSPRESET_BIT;
	}
	err = ak7755_write(AK7755_W_CONT0F,&cont0f,1);
	return 0;
}


//CONT0F DLRDY set  DSP Download Readly
//DISABLE  Normal Operation
//ENABLE   Program Downloading
//DSP programs and coefficient data can be downloaded by setting this bit to “1” during clock reset
//This bit must be set to “0” after finishing the downloading
int32_t ak7755_dsp_download_cmd(uint8_t state)
{
	uint8_t cont0f = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT0F,&cont0f,1);
	if(err < 0)
		return err;
	if(state == 0)
	{
		cont0f = cont0f & 0xfe;
	}else
	{
		cont0f = cont0f | 0x01;
	}

	err = ak7755_write(AK7755_W_CONT0F,&cont0f,1);
	return err;
}


/******************************************************************/
//ak7755 PRAM CRAM 写during reset
//写成功前提：CKRESETNbit H、DSPRESETNbit L、CRESETNbit L,DLRDYbit H,

//写ak7755 pram
int32_t ak7755_write_pram(uint8_t *ram_buffer,uint16_t length)
{
	int32_t err = 0;
	err = ak7755_write(AK7755_W_PRAM,&ram_buffer[1],length-1);
	return err;
}

#define CRC16_CCITT (0x1021)
uint16_t ak7755_calc_crc(const uint8_t *str, unsigned short length)
{
	uint16_t crc = 0x0000;
	uint16_t i, j;

	for(i = 0; i < length; i++){
		crc ^= *str++ << 8;
		for(j = 0; j < 8; j++){
			if(crc & 0x8000){
				crc <<= 1;
				crc ^= CRC16_CCITT;
			}
			else{
				crc <<= 1;
			}
		}
	}
	return crc;
}

uint16_t ak7755_read_crc(void)
{
	uint8_t readbuf[2] = {0};
	uint16_t crc = 0;
	ak7755_read(AK7755_R_CRC,(uint8_t*)readbuf,2);
	crc = readbuf[0];
	crc = (crc<<8) | readbuf[1];
	//TRACE("%s: CRC = 0x%04x",__func__,crc);
	return crc;
}

//写ak7755 pram
int32_t ak7755_write_pram_section(const uint8_t ram_buffer[][_AK77XX_DSP_PRAM_PIECE_SIZE],uint16_t num)
{
	int32_t err = 0;
	uint16_t i = 0, j = 0, lineLen = 0;
	uint16_t calCrc = 0, readCrc = 0;
	uint8_t retry = 0;
	lineLen = sizeof(ram_buffer[0]);
	//TRACE("%s: line_len = %d",__func__,lineLen);
	for(i = 0; i < num; i++)
	{
		retry = 0;
		if(i < num -1)
		{
			calCrc = ak7755_calc_crc(&ram_buffer[i][0],lineLen);
			do{
				err = ak7755_write(AK7755_W_PRAM,&ram_buffer[i][1],lineLen-1);
				osDelay(10);
				//TRACE("%s: %d line, len = %d",__func__,i,lineLen);
				readCrc = ak7755_read_crc();
				retry++;
				//TRACE("Read Crc = 0x%04x, calCrc = 0x%04x, %s",readCrc,calCrc, readCrc == calCrc ? "=======": "!!!!!!!!!!");
			}while((readCrc != calCrc) && (retry < CRC_MAX_RETRAY_COUNT));
			
			if(readCrc != calCrc)
			{
				err = -1;
				goto exit;
			}
		}
		else
		{
			for(j = 3; j < lineLen;)
			{
				if(ram_buffer[i][j] + ram_buffer[i][j+1] + ram_buffer[i][j+2] + ram_buffer[i][j+3] + ram_buffer[i][j+4] == 0)
					break;
				j+=5;
			}
			//TRACE("%s: last line, %d line, len = %d",__func__,i,j);
			calCrc = ak7755_calc_crc(&ram_buffer[i][0],j);
			do{
				err = ak7755_write(AK7755_W_PRAM,&ram_buffer[i][1],j-1);
				osDelay(10);
				readCrc = ak7755_read_crc();
				retry++;
				//TRACE("Read Crc = 0x%04x, calCrc = 0x%04x, %s",readCrc,calCrc, readCrc == calCrc ? "=======": "!!!!!!!!!!");
			}while((readCrc != calCrc) && (retry < CRC_MAX_RETRAY_COUNT));

			if(readCrc != calCrc)
			{
				err = -1;
				goto exit;
			}
		}
	}
exit:
	return err;
}

//写ak7755 CRAM
//
int32_t ak7755_write_cram_during_reset(const uint8_t *ram_buffer,uint16_t length)
{
	int32_t err = 0;
	uint16_t calCrc = 0, readCrc = 0;
	uint8_t retry = 0;
	//TRACE("%s: ----",__func__);
	calCrc = ak7755_calc_crc(&ram_buffer[0],length);
	do{
		err = ak7755_write(AK7755_W_CRAM,&ram_buffer[1],length-1);
		osDelay(10);
		readCrc = ak7755_read_crc();
		retry++;
		//TRACE("Read Crc = 0x%04x, calCrc = 0x%04x, %s",readCrc,calCrc, readCrc == calCrc ? "=======": "!!!!!!!!!!");
	}while((readCrc != calCrc) && (retry < CRC_MAX_RETRAY_COUNT));

	if(readCrc != calCrc)
		err = -1;
			
	return err;
}


//ak7755 ADC  ADM DAC mute
#define AD_MUTE  0x80
#define ADM_MUTE 0x40
#define DA_MUTE  0x20
int32_t ak7755_mute(uint8_t channel,uint8_t state)
{
	uint8_t cont1a = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT1A,&cont1a,1);
	if(err < 0)
		return err;
	if(state == 0)
	{
		cont1a = cont1a & ~channel;
	}else
	{
		cont1a = cont1a | channel;
	}

	err = ak7755_write(AK7755_W_CONT1A,&cont1a,1);
	return err;
}

//设置ak7755 DAC 信号源
int32_t ak7755_dac_input(uint8_t channel)
{
	uint8_t cont08 = 0;
	int32_t err = 0;
	//先读 CONT01
	err = ak7755_read(AK7755_R_CONT08,&cont08,1);
	if(err < 0)
		return err;

	cont08 = cont08 & 0x3f;
	cont08 |= channel;
	err = ak7755_write(AK7755_W_CONT08,&cont08,1);
	return err;
}

int32_t ak7755_set_slave(void)
{
	uint8_t cont00=0x3D;
	int32_t err = 0;
	err = ak7755_write(AK7755_W_CONT00,&cont00,1);
	//return err;
	cont00 = 0x10;//LRCK I/F Format I2S Compatible
	err = ak7755_write(0xC2,&cont00,1);
	return err;
}
#if 0
const uint8_t ak7755_volume_table[MAX_VOL][3]=
{
{0x00,0x00,0x00},//mute
{0x00,0x02,0x90},//-70
{0x00,0x04,0xA0},//-65
{0x00,0x08,0x30},//-60
{0x00,0x0B,0x90},//-57
{0x00,0x10,0x60},//-54
{0x00,0x17,0x10},//-51
{0x00,0x1D,0x10},//-49
{0x00,0x29,0x10},//-46
{0x00,0x3A,0x00},//-43
{0x00,0x51,0xF0},//-40
{0x00,0x73,0xB0},//-37
{0x00,0xA3,0x70},//-34
{0x00,0xE6,0xE0},//-31
{0x01,0x22,0xB0},//-29
{0x01,0x9A,0x90},//-26
{0x02,0x43,0xF0},//-23
{0x03,0x33,0x30},//-20
{0x04,0x07,0x50},//-18
{0x05,0x12,0x60},//-16
{0x06,0x62,0x80},//-14
{0x08,0x09,0xC0},//-12
{0x0A,0x1E,0x90},//-10
{0x0B,0x5A,0xA0},//-9
{0x0C,0xBD,0x50},//-8
{0x0E,0x4B,0x40},//-7
{0x10,0x09,0xC0},//-6
{0x11,0xFE,0xB0},//-5
{0x14,0x30,0xD0},//-4
{0x16,0xA7,0x80},//-3
{0x19,0x6B,0x20},//-2
{0x1C,0x85,0x20},//-1
{0x20,0x00,0x00}//0
};
#else
const uint8_t ak7755_volume_table[MAX_VOL+1][3]=
{
	{0x00,0x00,0x00},//mute
	{0x00,0x51,0xF0},//-40
	{0x00,0xA3,0x70},//-34
	{0x01,0x9A,0x90},//-26
	{0x03,0x33,0x30},//-20
	{0x05,0x12,0x60},//-16
	{0x0A,0x1E,0x90},//-10
	{0x0B,0x5A,0xA0},//-9
	{0x0C,0xBD,0x50},//-8
	{0x0E,0x4B,0x40},//-7
	{0x10,0x09,0xC0},//-6
	{0x11,0xFE,0xB0},//-5
	{0x14,0x30,0xD0},//-4
	{0x16,0xA7,0x80},//-3
	{0x19,0x6B,0x20},//-2
	{0x1C,0x85,0x20},//-1	
	{0x20,0x00,0x00}//0
};
#endif

void dsp_set_eq_during_run(uint8_t w_eq, uint16_t eqLen)
{
	uint8_t w_confirm[2] = {0};

	if(eqLen > (5*3 + 2))
		ak7755_write(AK7755_W_EQ_DOUBLE,w_eq,eqLen);
	else
		ak7755_write(AK7755_W_EQ_SIGNAL,w_eq,eqLen);

	// make the eq valid
	ak7755_write(CRAM_W_RUN,w_confirm,2);
}

void dsp_volume_set_during_run(uint8_t volum)
{
	uint8_t w_c[5]={0};
	if(volum>MAX_VOL)
		return;

	//TRACE("%s vol:%d",__func__,volum);
	w_c[0]= CRAM_VOLUME_ADD_H;
	w_c[1]= CRAM_VOLUME_ADD_L;
	w_c[2]=ak7755_volume_table[volum][0];
	w_c[3]=ak7755_volume_table[volum][1];
	w_c[4]=ak7755_volume_table[volum][2];
	//TRACE("%d,%d,%d",w_c[2],w_c[3],w_c[4]);
	ak7755_write(CRAM_W_CMD_BASE,w_c,5);
	memset(w_c,0,5);
	ak7755_write(CRAM_W_RUN,w_c,2);
	//TRACE("%s: set Volume %d",__func__,volum);
}

void dsp_volume_set_during_max(void)
{
	uint8_t w_c[5]={0};

	w_c[0]= CRAM_VOLUME_ADD_H;
	w_c[1]= CRAM_VOLUME_ADD_L;
	w_c[2]=0x20;
	w_c[3]=0;
	w_c[4]=0;
	//TRACE("%d,%d,%d",w_c[2],w_c[3],w_c[4]);
	ak7755_write(CRAM_W_CMD_BASE,w_c,5);
	memset(w_c,0,5);
	ak7755_write(CRAM_W_RUN,w_c,2);
	//TRACE("%s: set Volume max",__func__);
}

void dsp_volume_set_during_reset(uint8_t volum)
{
	uint8_t w_c[5]={0};
	if(volum>32)
		volum = 32;

	w_c[0]= CRAM_VOLUME_ADD_L;
	w_c[1]= CRAM_VOLUME_ADD_H;
	w_c[2]=ak7755_volume_table[volum][0];
	w_c[3]=ak7755_volume_table[volum][1];
	w_c[4]=ak7755_volume_table[volum][2];
	ak7755_write(0xb4,w_c,5);
	//TRACE("%s: set Volume %d",__func__,volum);
}

void ak7755_adc_mute(void)
{
	uint8_t cont09=0xff;

	ak7755_write(AK7755_ADL_VOL,&cont09,1); //set lch mute
	ak7755_write(AK7755_ADR_VOL,&cont09,1); //set rch mute
	//TRACE("%s--------",__func__);
}


void ak7755_adc_normal(void)
{
	uint8_t cont09=0x30;

	ak7755_write(AK7755_ADL_VOL,&cont09,1); //set lch normal
	ak7755_write(AK7755_ADR_VOL,&cont09,1); //set rch normal
	//TRACE("%s--------",__func__);
}

void ak7755_dac_mute(void)
{
	uint8_t cont09=0xff;

	ak7755_write(AK7755_DAL_VOL,&cont09,1); //set lch mute
	ak7755_write(AK7755_DAR_VOL,&cont09,1); //set rch mute
	//TRACE("%s--------",__func__);
}


void ak7755_dac_normal(void)
{
#ifdef __FACTORY_AUDIO_TEST__
	uint8_t cont09=0x17; // this value depend on tt
#else
	uint8_t cont09=0x0d; // this value depend on tt
#endif

	ak7755_write(AK7755_DAL_VOL,&cont09,1); //set lch normal
	ak7755_write(AK7755_DAR_VOL,&cont09,1); //set rch normal

#ifdef AK7755_EQ_DEBUG
	hal_spilcd_close(1);
	hal_iomux_set_3163_spi_normal();
#endif
	//TRACE("%s--------",__func__);
}

void ak7755_bick_set(uint8_t flag)
{
	uint8_t cont0f=0;
	//先读 CONT01

	ak7755_read(0x4a,&cont0f,1);
	if(flag == 0)
	{
		cont0f = cont0f & (~0x20);
	}else
	{
		cont0f = cont0f | 0x20;
	}
	ak7755_write(0xca,&cont0f,1);
}


void ak7755_write_cram_monitor(uint16_t address)
{
	uint8_t w_c[5]={0};

	w_c[0]= CRAM_MON_ADD >> 8;     //0x01;//CRAM_MON_ADD & 0x00ff;
	w_c[1]= CRAM_MON_ADD & 0x00ff; //0x05;//CRAM_MON_ADD >> 8;
	w_c[2]= address >> 8;
	w_c[3]= address & 0x00ff;
	w_c[4]=0x00;
	//写CRAM
	ak7755_write(CRAM_W_CMD_BASE,w_c,5);
	//执行操作
	w_c[0]=0x00;
	w_c[1]=0x00;
	ak7755_write(CRAM_W_RUN,w_c,2);
}

void ak7755_read_mir(uint8_t mir,uint8_t *mir_data)
{
	ak7755_read(mir,mir_data,4);
}


/*
	SYS_BT_CALL,
	SYS_BT_UN_CONNECT,
	SYS_BT_CONNECT,
	SYS_BT_PL_MUSIC
*/

uint8_t mir_buf[4];

uint8_t L_M=0,R_M=0;

//critical 
#define CRITIVAL_VOLUME  150

uint8_t  aux_in_monitor(void)
{
	ak7755_write_cram_monitor(L_LEVEL_OUT_ADD);
	osDelay(20);
	ak7755_read_mir(READ_MIR1,mir_buf);
	L_M = mir_buf[0];
	ak7755_write_cram_monitor(R_LEVEL_OUT_ADD);
	osDelay(20);
	ak7755_read_mir(READ_MIR1,mir_buf);
	R_M = mir_buf[0];
	

	if(L_M>135 && R_M >135)
	{

	}else 
	{

	}
	return 0;
}



//AK7755初始化
//master mode,
//ADC in:in4,in3()
void ak7755_application_init(void)
{
	int32_t err = 0;
	//TRACE("%s: init flag = %d",__func__,gAk7755_init_flag);
	if(gAk7755_init_flag)
		return;
	
	if(ak7755_write_mutex_id2==NULL){
		ak7755_write_mutex_id2 = osMutexCreate((osMutex(ak7755_write_mutex)));
	}	
	ak7755_init();
	ak7755_clock_control(1);	
	osDelay(30);
#ifndef AK7755_BYPASS_TEST
//	ak7755_write_pram(ak77dspPRAM,_AK77XX_DSP_PRAM_BUF_SIZE);
	err = ak7755_write_pram_section(ak77dspPRAM,_AK77XX_DSP_PRAM_PIECE);
	if(err < 0)
		goto exit;
	osDelay(10);
	err = ak7755_write_cram_during_reset(ak77dspCRAM,_AK77XX_DSP_CRAM_BUF_SIZE);
	if(err < 0)
		goto exit;
	osDelay(10);
	ak7755_codec_control(1);
	osDelay(10);
	ak7755_dsp_cmd(1);
	//开机播放蓝牙，设为0DB
#endif
	//dsp_volume_set_during_run(MAX_VOL-1);
	dsp_volume_set_during_run(ak7755_local_volume);
#if 0//#ifdef AK7755_EQ_DEBUG
	hal_spilcd_close(1);
	hal_iomux_set_3163_spi_normal();
#endif
	gAk7755_init_flag = 1;
	//TRACE("%s: init ak7755 succeed",__func__);
	return;
exit:
	//TRACE("%s: init ak7755 failed, turnoff ak7755",__func__);
	ak7755_power_off();
}

void ak7755_power_off(void)
{
#ifdef AK7755_EQ_DEBUG
	return;  // not power off ak7755 for ak7755 pc tools 
#endif
	//TRACE("%s: -------",__func__);
	ak7755_power_ctrl(0);
	hal_spilcd_close(1);
#ifdef AK7755_EQ_DEBUG
	hal_iomux_set_3163_spi_normal();
#endif
	gAk7755_init_flag = 0;
}

void app_ak7755_volumeset(u8 vol){
	//TRACE("%s vol=%d",__func__, vol);

    if (vol > MAX_VOL)
        vol = MAX_VOL;
    if (vol < MIN_VOL)
        vol = MIN_VOL;
	ak7755_local_volume = vol;
	dsp_volume_set_during_run(vol);
}

int8_t ak7755_get_volume(void){
	return ak7755_local_volume;
}
void ak7755_bt_stream_volumeup(void){
	if(ak7755_local_volume>=MAX_VOL)return;

	ak7755_local_volume++;
	app_ak7755_volumeset(ak7755_local_volume);
}

void ak7755_bt_stream_volumedown(void){
	if(ak7755_local_volume <= MIN_VOL)return;
	ak7755_local_volume--;
	app_ak7755_volumeset(ak7755_local_volume);
}

#endif