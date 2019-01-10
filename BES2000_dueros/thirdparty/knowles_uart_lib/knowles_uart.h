#ifndef __KMOWLES_UART_H__
#define __KMOWLES_UART_H__

#include <stdlib.h>

//#define KNOWLES_EVB_PIN


typedef unsigned int   uint32;
typedef unsigned short   uint16;
typedef unsigned char   uint8;

typedef char   int8;

#define FALSE 0
#define TRUE 1


/* PIO used for IA61X reset */
#define   IA61X_RESET_PIO	2

/* UART Related */
#define UART_VM_STOP_BITS		2

#define UART_RECEIVE_TIMEOUT 100
#define IACORE_UART_SBL_SYNC_WRITE_RETRY 10
#define IACORE_UART_SBL_SYNC_READ_RETRY 10
#define IACORE_UART_SBL_BOOT_ACK_RETRY 10

#define IA_RESP_TOUT				20 /* 20ms */
#define IA_RESP_POLL_TOUT			4  /*  4ms */
#define IA_MAX_RETRIES		(IA_RESP_TOUT / IA_RESP_POLL_TOUT)


/* IACORE Related */
/* Standard commands used by all chips */

#define IA6XX_WDB_BLOCK_SIZE        512
#define IACORE_SBL_SYNC_CMD         0x00
#define IACORE_SBL_SYNC_ACK         IACORE_SBL_SYNC_CMD
#define IACORE_SBL_AUTO_DET_CMD		0xB7
#define IACORE_SBL_AUTO_DET_ACK		IACORE_SBL_AUTO_DET_CMD
#define IACORE_SBL_BOOT_CMD		     0x01
#define IACORE_SBL_BOOT_ACK		     IACORE_SBL_BOOT_CMD

#define IA_SYNC_CMD             0x80000000
#define IA_WAKEUP_SYNC_CMD          0X9000
#define IA_SYNC_POLLING			    0x0000
#define IA_SYNC_ACK			    0x80000000
#define IA_SET_ALGO_PARAM_ID	    0x8017
#define IA_SET_ALGO_PARAM		    0x8018
#define IACORE_SBL_SET_RATE_REQ_CMD	0x8019

#define IA_SET_POWER_STATE		    0x9010
#define IA_SET_CHIP_SLEEP		    0x0001
#define IA_SET_LOWPOWER			    0x0002

#define IA_SYNC_MAX_RETRY           5
#define MAX_READ_FAILURE_RETRY		10

#define IA_NOT_READY		    0x00000000
#define IA_ILLEGAL_CMD		    0xFFFF0000
#define IA_NONRESPONSE_CMD      0x10000000

#define IA_EVENT_RESPONSE_CMD		0x801a
#define IA_LEVEL_TRIGGER_LOW		0x0001
#define IA_LEVEL_TRIGGER_HIGH		0x0002
#define IA_EDGE_FALLING			    0x0003
#define IA_EDGE_RISING			    0x0004



/*
 * Codec Interrupt event type
 */
#define IA_NO_EVENT             0x0000
#define IA_VS_INTR_EVENT		0x0001
#define IA_MASK_INTR_EVENT		0x0000FFFF

#define IA_SYNC_POLLING             0x0000
#define IA_SET_DEVICE_PARAMID		0x800C
#define IA_SET_DEVICE_PARAM		    0x800D
#define IA_CONFIG_DATA_PORT		     0x802C
#define IA_CONFIG_ENABLE		0x01
#define IA_CONFIG_2CHAN			0x02
#define IA_CONFIG_16k			0x01
#define IA_CONFIG_48k			0x04

#define IA_GET_EVENT_CMD    	0x806D0000
#define IA_SET_SAMPLE_RATE		0x8030
#define IA_SET_PRESET			0x8031
#define IA_SET_LL_PRESET		0x9031
#define IA_SET_ROUTE			0x8032
#define IA_SET_LL_ROUTE			0x9032
#define IA_SET_STOP_ROUTE		0x8033
#define IA_SET_BUFFERED_DATA_FORMAT	0x8034
#define IA_SET_FRAME_SIZE		    0x8035
#define IA_SET_AUD_PORT_CLK_FREQ	0x8042
#define IA_SET_CVS_PRESET		    0x906F

#define IA_VS_PROCESSING_MODE		0x5003
#define IA_VS_DETECT_KEYWORD		0x0000
#define IA_VS_ALGO_MEM_RESET		0x500E
#define IA_VS_MEM_PERFORM_RESET		0x0001
#define IA_OEM_SENSITIVITY  		0x5008
#define IA_OEM_SENSITIVITY_VALUE	0x0009


#define IA_STOP_ROUTE_VALUE		0x0000
#define IA_FRAME_SIZE_16MS		0x0010
#define IA_FRAME_SIZE_2MS		0x0002
#define IA_SAMPLE_RATE_16KHZ		0x0001
#define IA_SAMPLE_RATE_48KHZ		0x0004
#define IA_BUFFER_DATA_FORMAT_Q15	0x0002

#define IA_SET_PRESET_VALUE		    0x0480
#define IA_SET_CVS_PRESET_VALUE		0x0480

#define IA_SET_BYPASS_MODE		0x8040
#define IA_BYPASS_MODE_ON		0x0001
#define IA_BYPASS_MODE_OFF		0x0000

#define IA_READ_DATA_BLOCK		0x802E
#define IA_WRITE_DATA_BLOCK		0x802F

#define IA_KW_PRESERVATION		0x802A
#define IA_DISABLE_BUFFERING		0
#define IA_DISABLE_PRESERVATION		1
#define IA_ENABLE_PRESERVATION		2

#define IA_SELECT_STREAMING		0x9028
#define IA_RX_ENDPOINT			0x000C

#define IA_SET_IOSTREAMING		0x9025
#define IACORE_IOSTREAM_DISABLE		0
#define IACORE_IOSTREAM_ENABLE		1

#ifdef CONFIG_SND_SOC_IA_I2S_PERF
#define IACORE_STREAM_DISABLE	0x10
#define IACORE_STREAM_ENABLE	0x11
#else
#define IACORE_STREAM_DISABLE	0
#define IACORE_STREAM_ENABLE	1
#endif
#define IACORE_SET_START_STREAM_BURST_CMD	0x90260000
#ifdef CONFIG_SND_SOC_IA_SPI
#define IACORE_SET_STOP_STREAM_BURST_CMD	0x90260000
#else
#define IACORE_SET_STOP_STREAM_BURST_CMD	0x90260000
#endif

#define IA_STOP_STRM_CMD_RETRY	48	/* 48 words or 192 bytes */


enum {
    EIO = 1,
    EINVAL,
    EBUSY,
    ENOMEM,
    ENODEV
};

/* VQ Related */
enum ia_cvq_rate {
	IA_8KHZ = 0,
	IA_16KHZ = 1,
	IA_24KHZ = 2,
	IA_48KHZ = 4
};

enum ia_vq_mode {
	IA_VQ_MODE = 0,
	IA_CVQ_MODE
};

enum ia_format {
	IA_FORMAT_Q11 = 1,
	IA_FORMAT_Q15 = 2
};

enum ia_frame_size {
	IA_1MS_FRAME = 1,
	IA_2MS_FRAME = 2,
	IA_8MS_FRAME = 8,
	IA_10MS_FRAME = 10,
	IA_15MS_FRAME = 15,
	IA_16MS_FRAME = 16
};

enum ia_kw_preserve {
	IA_IGNORE_KW = 0,
	IA_PRESERVE_KW
};

enum ia_vad_mode {
	IA_NO_VAD = 0,
	IA_MIC_VAD
};

struct ia_cvq_params {
	int rate;
	int mode;
	int format;
	int frame_size;
	int kw_preserve;
	int vad;
};

/* IACORE related */
enum {
	IA_BOOT_SETUP_NREQ = 0,
	IA_BOOT_SETUP_REQ = 1
};


/* Streaming modes */
enum {
	IA_CVQ_STREAMING,
	IA_NORMAL_STREAMING
};



/* Bypass Mode ON OFF */
enum iacore_bypass_mode {
	IA_BYPASS_OFF,
	IA_BYPASS_ON,
	IA_BYPASS_SOUNDWIRE_DHWPT = 4
};

/* IA VS ALGO MEMORY SET/RESET */
enum {
	IA_ALGO_MEM_RESET = 0,
	IA_ALGO_MEM_SET
};


/* chip Power On Off */
enum {
	IA_POWER_OFF,
	IA_POWER_ON
};

enum {
	IA_PROFILING_OFF,
	IA_PROFILING_ON
};

enum {
    ONE_MIC = 0,
    SEPERATE_TWO_MIC,
    SHARED_TWO_MIC,
    COMBINED_TWO_MIC
};


/* Routes */
enum iacore_audio_routes {
	IA_1CH_PCM_IN0_PCM_OUT0		= 0x00,
	IA_1CH_PDM_IN0_PDM_OUT0		= 0x01,
	IA_1CH_PDM_IN0_PDM_OUT1		= 0x02,
	IA_1CH_SDW_PCM_IN_SDW_PCM_OUT	= 0x03,
	IA_1CH_PDM_IN0_PCM_OUT0_LL	= 0x04,
	IA_1CH_PDM_IN0_SDW_PCM_OUT_LL	= 0x05,
	IA_1CH_VQ_PDM_IN0		= 0x06,
	IA_1CH_VQ_PDM_IN2		= 0x07,
	IA_2CH_VQ_BUFFERED_PDM_IN	= 0x08,
	IA_1CH_PDM_IN0_PCM_OUT0		= 0x09,
	IA_1CH_PDM_IN0_SDW_PCM_OUT	= 0x0A,
	IA_1CH_SDW_PDM_IN_SDW_PCM_OUT	= 0x0B,
	IA_1CH_PDM_IN0_SDW_PDM_OUT	= 0x0C,
	IA_1CH_SDW_PDM_IN_SDW_PDM_OUT	= 0x0D,
	IA_1CH_TX_AUDIO_BURST_PCM_OUT0	= 0x0E,
	IA_1CH_TX_AUDIO_BURST_SDW_PCM_OUT = 0x0F,
	IA_1CH_PDM_IN0_PDM_OUT0_LL = 0x29,  /*route 41*/
    IA_2CH_PDM_IN_PDM_OUT_LL = 0x2B,    /*route 43*/
	IA_ROUTE_SELECTED_NONE = 0xFF
};

/* Presets */
enum iacore_audio_presets {
    PRESET_NONE = 0x00,
    PRESET_VAD_ON_CVQ_KW_PRSRV      = 0x01,
    PRESET_VAD_ON_CVQ_NO_KW_PRSRV   = 0x02,
    PRESET_VAD_ON_VQ_NO_BUFFERING   = 0x03,
    PRESET_VAD_OFF_CVQ_KW_PRSRV = 0x04,
    PRESET_VAD_OFF_CVQ_NO_KW_PRSRV  = 0x05,
    PRESET_VAD_OFF_VQ_NO_BUFFERING  = 0x06,
    PRESET_SPECIAL_PDM_16K_ROUTE_41  = 0x07,
    PRESET_I2S_RECORDING_48K    = 0x08,
    PRESET_SPECIAL_PDM_16K_ROUTE_43  = 0x09,
    PRESET_MASK = 0xF
};


enum iacore_fw_state {
	POWER_OFF,
	SBL,
	FW_LOADED,
	HW_BYPASS,
	FW_HW_BYPASS,
	SW_BYPASS,
	IO_STREAMING,
	VS_MODE,
	VS_SLEEP,
	VS_BURSTING,
	FW_SLEEP,
	PROXY_MODE, /* moving to this state is only possible through ioctl */
	FW_MIN = POWER_OFF,
	FW_MAX = FW_SLEEP
};





#define AVAILABLE_KEYWORDS 1
enum {
    ALEXA = 1
};




/* VQ Related */

/* Maximum keywords supported by the binary */
#define MAX_KW_MODELS 1
/* voice sense private data structure */

struct iacore_uart_device {
	uint32 baudrate_sbl;
	uint32 baudrate_vs;
    uint32 baudrate_vs_cmd;
};

struct iacore_voice_sense {
	char *vs;
    char *hw;
    int8 kw_count;
    char *kw[MAX_KW_MODELS];
    int kw_id[MAX_KW_MODELS];
    int kwds[MAX_KW_MODELS];
	struct ia_cvq_params params;
	struct ia_cvq_params iostream_params;
	bool cvq_sleep;
	bool kw_model_loaded;
	uint16 sensitivity;
};


struct iacore_priv {
    //TaskData iacore_task;
	struct iacore_uart_device *dev_data;
    struct iacore_voice_sense *voice_sense;
    uint8 fw_state;
	uint8 bypass_mode;
    uint8 event_id;
    bool use_preset;
    bool active_route_set;
    bool low_latency_route_set;
    uint16 multimic;
    bool alexa_playback_active;

	uint8 *file_ptr; 
};

enum iacore_message
{
    IA61X_VW,
    IA61X_PDM_BYPASS,
    IA61X_BURSTING,
    BES_KEYWORD,
    IA61X_TOP
};


//BES ADD
enum LOAD_FILE_ID
{
    HW_FILE,
    VS_FILE,
    ALEXA_FILE,
    FILE_NUM
};

extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_smart_mic_irq_pin_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_smart_mic_ldo_en_pin_cfg;

extern struct iacore_priv *iacore;

#ifdef __cplusplus
extern "C" {
#endif

int smart_mic_uart_config_register(void (* cb)(unsigned int, unsigned char));
int smart_mic_uart_write_register(int (* cb)(unsigned char *, unsigned int));
int smart_mic_uart_read_register(int (* cb)(unsigned char *, unsigned int ,unsigned int *));
int smart_mic_uart_read_timeout_register(void (* cb)(unsigned int));
int smart_mic_irq_pin_active_register(void (* cb)(void));



int8 iacore_event_status(void);
void iacore_init_model(struct iacore_priv *iacore);
bool iacore_is_keywords_changed();
int firmware_state_machine(struct iacore_priv *iacore, uint8 new_state);
int iacore_init(void);
int iacore_exit(void);


int iacore_uart_cmd(struct iacore_priv *iacore, uint32 cmd, uint32 *resp);


#ifdef __cplusplus
}
#endif

#endif

