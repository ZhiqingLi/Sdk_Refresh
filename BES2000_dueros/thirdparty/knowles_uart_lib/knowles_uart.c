#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "cmsis_nvic.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "knowles_uart.h"

#define KMOWLES_SMART_MIC_DEBUG_OPEN

#ifdef KMOWLES_SMART_MIC_DEBUG_OPEN
#define KNOWLES_SMART_MIC_DEBUG TRACE
#else
#define KNOWLES_SMART_MIC_DEBUG
#endif


#ifdef KNOWLES_EVB_PIN
//
#else
const struct HAL_IOMUX_PIN_FUNCTION_MAP app_smart_mic_irq_pin_cfg = {
    HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENALBE
};

const struct HAL_IOMUX_PIN_FUNCTION_MAP app_smart_mic_ldo_en_pin_cfg = {
    HAL_IOMUX_PIN_P0_4, HAL_IOMUX_FUNC_AS_GPIO, HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENALBE
};//use for ia610 power(14 for tonly)

#endif

#define msleep(n) osDelay(n)


struct iacore_priv *iacore = NULL;

char *available_keywords[AVAILABLE_KEYWORDS];


char *ref_filename[FILE_NUM]={"knowles_firmware/sysConfig.bin","knowles_firmware/boskoApp.bin","knowles_firmware/alexa.bin" };



#define KNOWLES_PSKEY 0x02b8
struct pskey_config {
    uint16 multimic;
    uint16 vad;
    uint16 sensitivity;
}knowles_pskey;

static int iacore_uart_exit(struct iacore_priv *iacore);
static int iacore_wakeup(struct iacore_priv *iacore);


#define GET_VS_PRIV(iacore) \
	((!iacore || !iacore->voice_sense) ? NULL : iacore->voice_sense)




/*********************BES_ADD***************************************************/
/******************* MEMORY FUNCTIONS ***************************************************/

#define APP_SMART_MIC_BUFFER_SIZE 1024*4
static unsigned char app_smart_mic_memory[APP_SMART_MIC_BUFFER_SIZE];
static uint32_t buff_size_used;
static bool app_smart_mic_memory_init = false;

#define free(n)  memset(n,0,sizeof(n))


static int app_smart_mic_mempool_init()
{
    buff_size_used = 0;

    //if do not memset, memset in app level
    memset(app_smart_mic_memory, 0, APP_SMART_MIC_BUFFER_SIZE);

    return 0;
}

static uint32_t app_smart_mic_mempool_free_memory_size()
{
    return APP_SMART_MIC_BUFFER_SIZE - buff_size_used;
}

static void * app_smart_mic_mempool_malloc(uint32_t size)
{
	void *buff;

    uint32_t buff_size_free;

    buff_size_free = app_smart_mic_mempool_free_memory_size();

    if (size % 4){
        size = size + (4 - size % 4);
    }

    ASSERT(size <= buff_size_free, "[%s] size = %d > free size = %d", __func__, size, buff_size_free);

    buff = app_smart_mic_memory + buff_size_used;

    buff_size_used += size;

    return buff;
}



/*******************  FUNCTIONS REGISTER ***************************************************/

static void (*iacore_configure_uart_dev)(unsigned int, unsigned char);
static int (*iacore_uart_data_send)(unsigned char *, unsigned int);
static int (*iacore_uart_data_receive)(unsigned char *, unsigned int ,unsigned int *);
static void (*iacore_uart_wait_ack_timeout)(unsigned int);
static void (*iacore_irq_pin_enter_detect_mode)(void);



int smart_mic_uart_config_register(void (* cb)(unsigned int, unsigned char))
{
    iacore_configure_uart_dev = cb;
    return 0;
}

int smart_mic_uart_write_register(int (* cb)(unsigned char *, unsigned int))
{
    iacore_uart_data_send = cb;
    return 0;
}

int smart_mic_uart_read_register(int (* cb)(unsigned char *, unsigned int ,unsigned int *))
{
    iacore_uart_data_receive = cb;
    return 0;
}

int smart_mic_uart_read_timeout_register(void (* cb)(unsigned int))
{
    iacore_uart_wait_ack_timeout = cb;
    return 0;
}

int smart_mic_irq_pin_active_register(void (* cb)(void))
{
    iacore_irq_pin_enter_detect_mode = cb;
    return 0;
}

static int get_firmware_data(char *filename,unsigned char *data,unsigned int offset_addr, unsigned int want_len,unsigned int *total_len)
{

	bool need_read_file=false;
	unsigned char current_id;
    extern unsigned char __hw_start[];
    extern unsigned char __hw_end[];
	
    extern unsigned char __vs_start[];
    extern unsigned char __vs_end[];
	/*key word*/
	
	extern unsigned char __alexa_start[];
    extern unsigned char __alexa_end[];

	for(unsigned char i=0;i<FILE_NUM;i++){
		if (strcmp(filename,ref_filename[i]) == 0){
			//TRACE("current_file_id=%d",i);
			current_id=i;
			need_read_file=true;
			break;
		}
	}
	if(!need_read_file){
		need_read_file=false;
		return 1;
	}
		
	switch((enum LOAD_FILE_ID)current_id){
		case HW_FILE:
			*total_len = __hw_end - __hw_start;
			memcpy(data,&__hw_start[offset_addr],want_len);
			break;
		case VS_FILE:
			*total_len = __vs_end - __vs_start;
			memcpy(data,&__vs_start[offset_addr],want_len);
			break;	
		case ALEXA_FILE:
			*total_len = __alexa_end - __alexa_start;
			memcpy(data,&__alexa_start[offset_addr],want_len);
			break;
		default:
			break;
	}
	return 0;
}


/***********BES ADD END**************/


static void uint32_to_uint8_array(uint32 b32,uint8* b8)
{
    b8[0] = b32 >> 24 & 0xFF;
    b8[1] = b32 >> 16 & 0xFF;
    b8[2]=  b32 >> 8 & 0xFF;
    b8[3] = b32 &0xFF;
}

static void uint8_array_to_uint32(uint8* b8,uint32* b32)
{
    (*b32) = b8[0];
    (*b32) = ((*b32) << 8) | b8[1];
    (*b32) = ((*b32) << 8) | b8[2];
    (*b32) = ((*b32) << 8) | b8[3];
}

static int iacore_uart_write(struct iacore_priv *iacore, void *buf, int len)
{
	if(iacore_uart_data_send((uint8*)buf,(uint32_t)len))
		return -EIO;

	return 0;
}

static int iacore_uart_read(struct iacore_priv *iacore, void *buf, int len)
{
	unsigned int rlen;
	int ret;
	uint8  uart_map_address[len];
	ret=iacore_uart_data_receive(uart_map_address,len,&rlen);
	//ret=iacore_uart_data_receive((unsigned char *)buf,len,&rlen);
	if(ret){
		KNOWLES_SMART_MIC_DEBUG("Uart Read Failed!!!!");
		return -EIO;
	}else if(rlen!=len){
		KNOWLES_SMART_MIC_DEBUG("Uart Read Failed for len: %d, fact_len = %d\n", len, rlen);
		return -EIO;
	}else{
		KNOWLES_SMART_MIC_DEBUG("Uart Read success  len: %d", len);
		memcpy(buf, uart_map_address, len);
	}
	return 0;
}

int iacore_uart_cmd(struct iacore_priv *iacore, uint32 cmd, uint32 *resp)
{
	uint8 command[4],response[4];
	int retry = IA_MAX_RETRIES + 1,err = 0;
    bool sr;

    sr = ((cmd & IA_NONRESPONSE_CMD) != 0)? TRUE : FALSE;

	*resp = 0;
	uint32_to_uint8_array(cmd,command);
	err = iacore_uart_write(iacore, command, sizeof(command));
	if (err || sr)
		goto cmd_exit;

	do {

		msleep(5);
		err = iacore_uart_read(iacore, response, sizeof(response));
		uint8_array_to_uint32(response,resp);
		if (err) {
			KNOWLES_SMART_MIC_DEBUG("%s: uart_read() fail %d\n", __func__, err);
		} else if ((*resp & IA_ILLEGAL_CMD) == IA_ILLEGAL_CMD) {
			KNOWLES_SMART_MIC_DEBUG("%s: illegal command 0x%08lx\n",
				__func__, cmd);
			err = -EINVAL;
			/* Calibrate the UART if host receives some unexpected
			 * response from the chip */
			goto cmd_exit;
		} else if (*resp == IA_NOT_READY) {
			KNOWLES_SMART_MIC_DEBUG("%s: iacore_uart_read() not ready\n", __func__);
			err = -EBUSY;
		} else {
			goto cmd_exit;
		}
		--retry;
	} while (retry != 0);

cmd_exit:
    KNOWLES_SMART_MIC_DEBUG("%s: cmd=0x%08lx resp=0x%08lx sr=%d\n", __func__, cmd, *resp, sr);
	return err;
}

/* read all extra bytes on the UART line */
static void iacore_uart_discard_rxbytes(struct iacore_priv *iacore)
{

	uint8 need_clean[1];
	uint32 len=1;
	uint32 receive_len;
	int ret;
	do{
		ret=iacore_uart_data_receive(need_clean,len,&receive_len);
	}while(ret==0);
}



static int set_sbl_baud_rate(struct iacore_priv *iacore, uint32 sbl_rate)
{
    struct iacore_uart_device *iacore_uart = iacore->dev_data;
    uint8 retry = IACORE_UART_SBL_SYNC_WRITE_RETRY, bytes_to_read, msg[4] = {0};
	uint8 *buf = NULL;
	int rc;
	uint32 sbl_rate_req_cmd = IACORE_SBL_SET_RATE_REQ_CMD,baudrate_change_resp;

    sbl_rate_req_cmd = sbl_rate_req_cmd << 16;
	sbl_rate_req_cmd |= iacore_uart->baudrate_vs_cmd;

    /* Get Baud rate value for the request SBL rate */
	KNOWLES_SMART_MIC_DEBUG("Setting uart baud rate %08lx\n", sbl_rate_req_cmd);

	uint32_to_uint8_array(sbl_rate_req_cmd,msg);
	rc = iacore_uart_write(iacore, msg, sizeof(msg));
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Baud rate setting for UART fail %d\n", rc);
		rc = -EIO;
		return rc;
	}

    msleep(30);
	/* Configure the host side UART speed */
	iacore_configure_uart_dev(sbl_rate, (unsigned char)UART_VM_STOP_BITS);
    iacore_uart_discard_rxbytes(iacore);

	baudrate_change_resp = 0;
	/* Once UART rate request command is sent, SBL will switch to rate
	 * calibration mode and will be waiting for 00 over UART with the new
	 * baud rate. */

	memset(msg, 0, sizeof(msg));
	do {
		rc = iacore_uart_write(iacore, msg, sizeof(msg));
	} while (rc && retry--);

	if (rc) {
		rc = -EIO;
		return rc;
	}

	/* SHP-1964: firmware take ~28ms to send the response for set baud rate
	 * command */
	msleep(10);
	/* Sometimes an extra byte (0x00) is received over UART
	 * which should be discarded.
	 */
	bytes_to_read = sizeof(msg);

	rc = iacore_uart_read(iacore, msg, sizeof(char));
	if (rc < 0) {
		KNOWLES_SMART_MIC_DEBUG("Set Rate Request read rc = %d\n", rc);
		return rc;
	}

	if (msg[0] == 0) {
		/* Discard this byte */
		KNOWLES_SMART_MIC_DEBUG("Received extra zero\n");
		buf = &msg[0];
	} else {
		/* 1st byte was valid, now read only remaining bytes */
		bytes_to_read -= sizeof(char);
		buf = &msg[1];
	}

	rc = iacore_uart_read(iacore, buf, bytes_to_read);
	if (rc < 0) {
		KNOWLES_SMART_MIC_DEBUG("Set Rate Request read rc = %d\n", rc);
		return rc;
	}
    uint8_array_to_uint32((uint8 *)msg,&baudrate_change_resp);
	if (baudrate_change_resp != sbl_rate_req_cmd) {
		KNOWLES_SMART_MIC_DEBUG("Invalid response to Rate Request :0x%08lx, 0x%08lx\n",
				baudrate_change_resp, sbl_rate_req_cmd);
		return -EINVAL;
	}

	return rc;
}

static int iacore_uart_wakeup(struct iacore_priv *iacore)
{
	char wakeup_byte = 0x00;
	int ret = 0;

	ret = iacore_uart_write(iacore, &wakeup_byte, 1);
	if (ret < 0) {
		KNOWLES_SMART_MIC_DEBUG("%s() UART wakeup failed:%d\n",
				__func__, ret);
		goto iacore_uart_wakeup_exit;
	}

	iacore_uart_discard_rxbytes(iacore);

	/* TODO fix the sleep */
	msleep(50);

iacore_uart_wakeup_exit:
	return ret;
}




static int iacore_configure_uart(struct iacore_priv *iacore)
{
	struct iacore_uart_device *iacore_uart = iacore->dev_data;
	uint8 retry = IACORE_UART_SBL_SYNC_WRITE_RETRY;
	uint8 sbl_sync_cmd[2] = {IACORE_SBL_SYNC_CMD};
	uint8 sbl_auto_det_cmd = IACORE_SBL_AUTO_DET_CMD;
	char msg[4] = {0};
	int rc;

	/* set Host UART speed to bootloader baud */
	iacore_configure_uart_dev(iacore_uart->baudrate_sbl, (unsigned char)UART_VM_STOP_BITS);
	
	iacore_uart_wait_ack_timeout(UART_RECEIVE_TIMEOUT);//wait receive
	
	iacore_uart_discard_rxbytes(iacore);
	/* write SBL SYNC BYTES 0x0000 */
	KNOWLES_SMART_MIC_DEBUG("write IACORE_SBL_SYNC_CMD = 0x%x 0x%x\n", sbl_sync_cmd[0],sbl_sync_cmd[1]);
    iacore_uart_write(iacore, sbl_sync_cmd, sizeof(sbl_sync_cmd));

	/* TODO fix the sleep */
	msleep(20);
	iacore_uart_discard_rxbytes(iacore);

	/* write SBL AUTODET BYTE 0xB7 */
	KNOWLES_SMART_MIC_DEBUG("write IACORE_SBL_AUTO_DET_CMD = 0x%02x\n", sbl_auto_det_cmd);
	rc = iacore_uart_write(iacore, &sbl_auto_det_cmd, sizeof(sbl_auto_det_cmd));
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed sbl auto detect write %d\n", rc);
		rc = -EIO;
		goto iacore_configure_failed;
	}

	/* SBL AUTODET BYTE ACK 0xB7 */
	/* TODO fix the sleep */
	msleep(60);
	memset(msg, 0, sizeof(msg));
	retry = IACORE_UART_SBL_BOOT_ACK_RETRY;
#if 0
	do {
		rc = iacore_uart_read(iacore, msg, sizeof(char));
	} while (rc && retry--);
#else
	do {
		rc = iacore_uart_read(iacore, msg, sizeof(char));
	} while (rc && retry--);
#endif
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed AUTO DET ack %d\n", rc);
		goto iacore_configure_failed;
	}

	KNOWLES_SMART_MIC_DEBUG("sbl auto det ack = 0x%02x", (unsigned char)msg[0]);//0xb7

	if ((unsigned char )msg[0] != IACORE_SBL_AUTO_DET_ACK) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed boot ack pattern\n");
		rc = -EIO;
		goto iacore_configure_failed;
	}
iacore_configure_failed:
	return rc;
}




static int iacore_uart_boot_setup(struct iacore_priv *iacore, bool bootsetup)
{
    struct iacore_uart_device *iacore_uart = iacore->dev_data;
	uint8 retry = IACORE_UART_SBL_SYNC_WRITE_RETRY;
	uint8 sbl_boot_cmd = IACORE_SBL_BOOT_CMD;
	char msg[4] = {0};
	int rc = 0;

	if (bootsetup) {
		rc = iacore_configure_uart(iacore);
		if (rc < 0) {
			KNOWLES_SMART_MIC_DEBUG("failed to configure uart for bootup %d\n", rc);
			goto iacore_bootup_failed;
		}

        KNOWLES_SMART_MIC_DEBUG("iacore_configure_uart done\n");
		
#ifdef KNOWLES_I2S_INTERFACE
	uint32 cmd, rsp = 0;
	cmd=0x80040001;// 1
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("########error %d\n", rc);
	}
#endif
		/* TODO fix the sleep */
		msleep(10);
		rc = set_sbl_baud_rate(iacore, iacore_uart->baudrate_vs);
		if (rc < 0) {
			KNOWLES_SMART_MIC_DEBUG("set_sbl_baud_rate fail %d\n", rc);
			goto iacore_bootup_failed;
		}

		/* TODO fix the sleep */
		msleep(20);		
	}

	/* SBL BOOT BYTE 0x01 */
	memset(msg, 0, sizeof(msg));
	KNOWLES_SMART_MIC_DEBUG("write IACORE_SBL_BOOT_CMD = 0x%02x\n",  sbl_boot_cmd);
	memcpy(msg, (char *)&sbl_boot_cmd, 1);
	rc = iacore_uart_write(iacore, msg, 1);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed sbl boot write %d\n", rc);
		rc = -EIO;
		goto iacore_bootup_failed;
	}

	/* SBL BOOT BYTE ACK 0x01 */
	msleep(20);
	memset(msg, 0, sizeof(msg));
	retry = IACORE_UART_SBL_BOOT_ACK_RETRY;
	do {
		rc = iacore_uart_read(iacore, msg, 1);
	} while (rc && retry--);

	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed boot ack %d\n", rc);
		goto iacore_bootup_failed;
	}
	KNOWLES_SMART_MIC_DEBUG("sbl boot ack = 0x%02x\n", msg[0]);

	if (msg[0] != IACORE_SBL_BOOT_ACK) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed boot ack pattern\n");
		rc = -EIO;
		goto iacore_bootup_failed;
	}
	rc = 0;	
iacore_bootup_failed:
	return rc;
}

static int iacore_uart_boot_finish(struct iacore_priv *iacore)
{
	int rc;
	uint32 sync_cmd = IA_SYNC_CMD;
	uint32 sync_ack;
	int sync_retry = IA_SYNC_MAX_RETRY;

	/* Use Polling method if gpio-a is not defined */
	/*
	 * Give the chip some time to become ready after firmware
	 * download. (FW is still transferring)
	 */
	msleep(35);//35

	/* now switch to firmware baud to talk to chip */
	/*iacore_configure_uart_dev(iacore_uart->baudrate_vs, UART_VM_STOP_BITS);*/

	/* sometimes ia6xx chip sends success in second sync command */
	do {
		/* Discard extra bytes from iacore after firmware load.
		 * Host gets extra bytes after VS firmware download.
		 */
		iacore_uart_discard_rxbytes(iacore);

		KNOWLES_SMART_MIC_DEBUG("write IA_SYNC_CMD = 0x%08lx\n", sync_cmd);
		rc = iacore_uart_cmd(iacore, sync_cmd, &sync_ack);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("fw load failed in write sync cmd - %d\n", rc);
			continue;
		}
		KNOWLES_SMART_MIC_DEBUG("sync_ack = 0x%08lx\n", sync_ack);
		if (sync_ack != IA_SYNC_ACK) {
			KNOWLES_SMART_MIC_DEBUG("fw load failed sync ack pattern=0x%08lx\n",
				sync_ack);
			rc = -EIO;
		} else {
			KNOWLES_SMART_MIC_DEBUG("firmware load success\n");
			break;
		}
	} while (sync_retry--);

	return rc;
}




/* SBL Bypass mode is supported only in PDM bus combination */
static int iacore_uart_sbl_bypass_setup(struct iacore_priv *iacore)
{
	struct iacore_uart_device *iacore_uart = iacore->dev_data;
	uint8 retry = IACORE_UART_SBL_SYNC_WRITE_RETRY;
	uint16 sbl_sync_cmd = IACORE_SBL_SYNC_CMD;
	uint8 sbl_auto_det_cmd = IACORE_SBL_AUTO_DET_CMD;
	char msg[4] = {0};
	int rc;

	KNOWLES_SMART_MIC_DEBUG("prepare for SBL bypass\n");

	/* set Host UART speed to bootloader baud */
	iacore_configure_uart_dev(iacore_uart->baudrate_sbl,(unsigned char)UART_VM_STOP_BITS);

	iacore_uart_discard_rxbytes(iacore);

	/* SBL SYNC BYTES 0x0000 */
	KNOWLES_SMART_MIC_DEBUG("write IACORE_SBL_SYNC_CMD = 0x%02x\n", sbl_sync_cmd);
	memcpy(msg, (char *)&sbl_sync_cmd, sizeof(sbl_sync_cmd));
	do {
		rc = iacore_uart_write(iacore, msg, 2);
	} while (rc && retry--);

	if (rc) {
		rc = -EIO;
		goto iacore_sbl_bypass_failed;
	}

	msleep(20);

	/* SBL AUTODET BYTE 0xB7 */
	KNOWLES_SMART_MIC_DEBUG("write IACORE_SBL_AUTO_DET_CMD = 0x%02x\n", sbl_auto_det_cmd);
	rc = iacore_uart_write(iacore, &sbl_auto_det_cmd, sizeof(sbl_auto_det_cmd));
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed sbl auto detect write %d\n", rc);
		rc = -EIO;
		goto iacore_sbl_bypass_failed;
	}

	/* SBL AUTODET BYTE ACK 0xB7 */
	msleep(60);
	memset(msg, 0, sizeof(msg));
	retry = IACORE_UART_SBL_BOOT_ACK_RETRY;
	do {
		rc = iacore_uart_read(iacore, msg, 1);
	} while (rc && retry--);

	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed AUTO DET ack %d\n",  rc);
		goto iacore_sbl_bypass_failed;
	}
	KNOWLES_SMART_MIC_DEBUG("sbl auto det ack = 0x%02x\n", msg[0]);

	if (msg[0] != IACORE_SBL_AUTO_DET_ACK) {
		KNOWLES_SMART_MIC_DEBUG("firmware load failed boot ack pattern\n");
		rc = -EIO;
	}

iacore_sbl_bypass_failed:
	return rc;
}



static int iacore_set_chip_sleep(struct iacore_priv *iacore)
{
	int rc = 0;
	uint32 cmd, rsp;

    cmd = IA_SET_POWER_STATE;
	cmd = (cmd << 16) | IA_SET_CHIP_SLEEP;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc)
		KNOWLES_SMART_MIC_DEBUG("%s(): Set chip sleep cmd fail %d\n",
				__func__, rc);

	return rc;
}

static uint8 iacore_get_power_state(struct iacore_priv *iacore)
{
	return iacore->fw_state;
}

static void iacore_power_ctrl(struct iacore_priv *iacore, bool value)
{
	//using VMIC pin for power
}

static void iacore_poweroff_chip(struct iacore_priv *iacore)
{
	//iacore_power_ctrl(iacore, IA_POWER_OFF);
	iacore->fw_state = POWER_OFF;
}




static int iacore_set_bypass(struct iacore_priv *iacore)
{
	uint32 cmd, rsp = 0;
	int rc = 0;

	if (iacore->fw_state == SBL) {

		/* send some bus related activity */
		rc = iacore_uart_sbl_bypass_setup(iacore);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("SBL Sleep setup fails %d\n", rc);
			goto sbl_bypass_err;
		}
	} else if (iacore->fw_state == FW_LOADED) {
		/* Do nothing */
	} else {
		KNOWLES_SMART_MIC_DEBUG("%s: firmware is not in required power state: %d\n",
			__func__, iacore->fw_state);
		rc = -EINVAL;
		goto sbl_bypass_err;
	}

	msleep(60);
	
#ifdef KNOWLES_I2S_INTERFACE
	cmd=0x80330000;// 0
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail2 %d\n", rc);
		goto sbl_bypass_err;
	}
	msleep(10);
	
	cmd=0x80300001;// sample rate 16k
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail %d\n", rc);
		goto sbl_bypass_err;
	}

	cmd=0x800c1002;// 4
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail5 %d\n", rc);
		goto sbl_bypass_err;
	}

	/* Put the chip in to sleep */
	msleep(10);

	cmd=0x800d0000;// 5
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail6 %d\n", rc);
		goto sbl_bypass_err;
	}

	cmd=0x80420000;// 6
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail7 %d\n", rc);
		goto sbl_bypass_err;
	}

	cmd=0x90320004;// 8
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail %d\n", rc);
		goto sbl_bypass_err;
	}
#else
    cmd = IA_SET_BYPASS_MODE;
	cmd = cmd << 16 | IA_BYPASS_ON;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd ON fail %d\n", rc);
		goto sbl_bypass_err;
	}

	/* Put the chip in to sleep */
	msleep(5);

    cmd = IA_SET_POWER_STATE;
	cmd = cmd << 16 | IA_SET_CHIP_SLEEP;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("Set SBL Sleep Power State cmd fail %d\n", rc);
		goto sbl_bypass_err;
	}
#endif
sbl_bypass_err:
	return rc;
}

static int iacore_set_active_route(struct iacore_priv *iacore, bool ll_route, uint32 route)
{
    uint32 cmd, rsp;
    int rc;
    /* set active route */
    route = route & 0xffff;
    if (ll_route == TRUE)
    {
        cmd = IA_SET_LL_ROUTE;
        cmd = cmd << 16 | route;
    }
    else
    {
        cmd = IA_SET_ROUTE;
        cmd = cmd << 16 | route;
    }
    rc = iacore_uart_cmd(iacore, cmd, &rsp);
    if (rc)
    {
        KNOWLES_SMART_MIC_DEBUG("set route fail %d", rc);
    }
    else
    {
        if(ll_route == TRUE)
            iacore->low_latency_route_set = TRUE;
        iacore->active_route_set = TRUE;
    }
    return rc;
}


static int iacore_vs_route_preset(struct iacore_priv *iacore)
{
	struct iacore_voice_sense *voice_sense = GET_VS_PRIV(iacore);
	uint32 cmd, rsp, route;
	int rc = 0;

	if (!voice_sense) {
		KNOWLES_SMART_MIC_DEBUG("%s(): invalid private pointer\n",
			__func__);
		rc = -EINVAL;
		goto route_error;
	}

    cmd = IA_SET_SAMPLE_RATE;
	cmd = cmd << 16 | voice_sense->params.rate;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): Set rate cmd failed %d\n",
			__func__, rc);
		goto route_error;
	}
	if (voice_sense->params.mode == IA_CVQ_MODE){
	    cmd = IA_SET_FRAME_SIZE;
		cmd = cmd << 16 | voice_sense->params.frame_size;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set frame size cmd failed %d\n",
				__func__, rc);
			goto route_error;
		}
	}
    cmd = IA_SET_BUFFERED_DATA_FORMAT;
	cmd = cmd << 16 | voice_sense->params.format;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): Set buffer format cmd failed %d\n",
			__func__, rc);
		goto route_error;
	}

	if (voice_sense->params.vad == IA_MIC_VAD){
        route = IA_2CH_VQ_BUFFERED_PDM_IN;
    }
	else{
		/* No vad */
        route = IA_1CH_VQ_PDM_IN0;
    }

	rc = iacore_set_active_route(iacore,FALSE,route);
	if (rc)
    {
        KNOWLES_SMART_MIC_DEBUG( "%s(): Set route  %08lx failed %i\n",
            __func__, route, rc);
    }

route_error:
	return rc;
}



static int iacore_vs_set_algo_params(struct iacore_priv *iacore, bool set)
{
	uint32 cmd, rsp;
	int rc = 0;

	if (set) {
        cmd = IA_SET_ALGO_PARAM_ID;
		cmd = cmd << 16 | IA_VS_PROCESSING_MODE;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param VS processing cmd fail %d\n",
			__func__, rc);
			goto vs_algo_param_err;
		}
        cmd = IA_SET_ALGO_PARAM;
		cmd = cmd << 16 | IA_VS_DETECT_KEYWORD;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param keyword detect cmd fail %d\n",
			__func__, rc);
			goto vs_algo_param_err;
		}
	} else {
        cmd = IA_SET_ALGO_PARAM_ID;
		cmd = cmd << 16 | IA_VS_ALGO_MEM_RESET;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param VS processing cmd fail %d\n",
			__func__, rc);
			goto vs_algo_param_err;
		}
        cmd = IA_SET_ALGO_PARAM;
		cmd = cmd << 16 | IA_VS_MEM_PERFORM_RESET;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param keyword detect cmd fail %d\n",
			__func__, rc);
			goto vs_algo_param_err;
		}
	}

vs_algo_param_err:
	return rc;
}


static int iacore_vs_write_keywords(struct iacore_priv *iacore)
{
    struct iacore_uart_device *iacore_uart = iacore->dev_data;
	unsigned char *file_read=iacore->file_ptr;

    uint32 wdb_cmd;
    uint32 fileSize = 0,currChunkSize;
    uint8 chunk_header[4],val[4];
    int8 rc = 0,i;
    const uint8* file_map_address;
	
	unsigned int offset_addr=0;


    for(i=0;i<iacore->voice_sense->kw_count;i++)
    {
    	//data clear
    	fileSize=0;
		currChunkSize=0;
		offset_addr=0;
		memset(chunk_header,0,sizeof(chunk_header));
		memset(val,0,sizeof(val));
		
		if(get_firmware_data((char *)iacore->voice_sense->kw[i],file_read,0,0, &fileSize))
		{
			rc = -1;
			return rc;
		}else{
			TRACE("file_size=%d, %08x",fileSize,fileSize);
		}

        wdb_cmd = IA_WRITE_DATA_BLOCK;
        wdb_cmd = ( (wdb_cmd << 16) | ((uint16)fileSize) );
        uint32_to_uint8_array(wdb_cmd,val);
        iacore_uart_write(iacore,(char *)val,4);

        msleep(10);

        /*copy the header*/
        get_firmware_data((char *)iacore->voice_sense->kw[i],chunk_header,0,sizeof(chunk_header), NULL);

		offset_addr+=sizeof(chunk_header);
		fileSize-=sizeof(chunk_header);

        /*update the keyword id*/
        chunk_header[2] = (chunk_header[2] & 0x0F) | (((iacore->voice_sense->kw_id[i])<<4));

        while(fileSize > 0)
        {
           /*add a command delay between each write (firmware requirement)*/

           if(fileSize >= (IA6XX_WDB_BLOCK_SIZE - sizeof(chunk_header)))
           {
              currChunkSize = ( IA6XX_WDB_BLOCK_SIZE - sizeof(chunk_header) );
           }
           else
           {
              currChunkSize = fileSize;
           }
           /*write 4 byte the header*/
           iacore_uart_write(iacore,(char *)chunk_header,sizeof(chunk_header));
           /*write remaining packet length*/
           /* Using StreamMove which is faster than memcpy to reduce IA61x mic setup time */
		   get_firmware_data((char *)iacore->voice_sense->kw[i],file_read,offset_addr,currChunkSize, NULL);
		   offset_addr+=currChunkSize;
		   fileSize-=currChunkSize;
		   if(iacore_uart_data_send((uint8*)file_read,currChunkSize))
				return -EIO;


           /* write extra zeros to fill last block size */
           while(currChunkSize <  ( IA6XX_WDB_BLOCK_SIZE - sizeof(chunk_header) )){
               memset(chunk_header,0,sizeof(chunk_header));
               iacore_uart_write(iacore,(char *)chunk_header,sizeof(chunk_header));
               currChunkSize = currChunkSize + sizeof(chunk_header);
           }

           if(fileSize > ( IA6XX_WDB_BLOCK_SIZE - sizeof(chunk_header) ))
               chunk_header[3]++;
           else
               chunk_header[3] = 0xFF;
        }
        iacore_uart_read(iacore,(char *)&val,sizeof(val));
        if( (val[2] != 0) || (val[3] != 0) ){
			rc = -1;
			KNOWLES_SMART_MIC_DEBUG("val[2]=%02x val[3]=%02x",val[2],val[3]);
        }
            
        KNOWLES_SMART_MIC_DEBUG("iacore_vs_write_keywords [%s] Done !!!!!\n",iacore->voice_sense->kw[i]);
     }

	return rc;
}


static int iacore_write_firmware(struct iacore_priv *iacore, char *filename)
{
	int8 rc = 0;

    struct iacore_uart_device *iacore_uart = iacore->dev_data;
	unsigned char *file_read=iacore->file_ptr;
	unsigned int fileSize,currChunkSize;
	unsigned int offset_addr=0;
	TRACE("READ FILE!!!,file name=%s",filename);
	if(get_firmware_data(filename,file_read,0,0, &fileSize))
	{
		rc = -1;
		return rc;
	}else{
		TRACE("file_size=%d",fileSize);
	}

	while(fileSize > 0)
    {
    	
       if(fileSize >= IA6XX_WDB_BLOCK_SIZE)
       {
          currChunkSize = IA6XX_WDB_BLOCK_SIZE;
       }
       else
       {
          currChunkSize = fileSize;
       }
	   get_firmware_data(filename,file_read,offset_addr,currChunkSize, NULL);

	   if(iacore_uart_data_send((uint8*)file_read,currChunkSize))
            return -EIO;

       offset_addr+=currChunkSize;
       fileSize-=currChunkSize;
    }
	KNOWLES_SMART_MIC_DEBUG("ia61x_load_binary  [%s]Done !!!!!\n",filename);

	return rc;
}



static int iacore_vs_load(struct iacore_priv *iacore)
{

	struct iacore_voice_sense *voice_sense = GET_VS_PRIV(iacore);
	int rc = 0;

	rc = iacore_uart_boot_setup(iacore, IA_BOOT_SETUP_REQ);

	if (rc) {
		KNOWLES_SMART_MIC_DEBUG( "%s(): fw download start error %d\n",
			__func__, rc);
		goto iacore_vs_fw_download_failed;
	}

	rc = iacore_write_firmware(iacore, voice_sense->hw);
	
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): vs firmware image write error %d\n",
			__func__, rc);
		rc = -EIO;
		goto iacore_vs_fw_download_failed;
	}

	rc = iacore_uart_boot_setup(iacore, IA_BOOT_SETUP_NREQ);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): fw download start error %d\n",
			__func__, rc);
		goto iacore_vs_fw_download_failed;
	}

	rc = iacore_write_firmware(iacore, voice_sense->vs);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG( "%s(): vs firmware image write error %d\n",
			__func__, rc);
		rc = -EIO;
		goto iacore_vs_fw_download_failed;
	}

	/* boot_ops.finish is required only in the case of POLL mode
	 * command completion*/
	rc = iacore_uart_boot_finish(iacore);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s() vs fw download finish error %d\n",
				__func__, rc);
		goto iacore_vs_fw_download_failed;
	}

	KNOWLES_SMART_MIC_DEBUG("%s(): fw download done\n", __func__);

iacore_vs_fw_download_failed:

	return rc;

}

static int iacore_set_sw_bypass(struct iacore_priv *iacore)
{
    uint32 cmd, rsp;
    int rc;

    if(iacore->use_preset)
    {
        cmd = IA_SET_LL_PRESET;
        if(iacore->multimic == COMBINED_TWO_MIC)
        {
            cmd = cmd << 16 | PRESET_SPECIAL_PDM_16K_ROUTE_43;
        }
        else
        {
            cmd = cmd << 16 | PRESET_SPECIAL_PDM_16K_ROUTE_41;
        }
        /* set IA6xx sampling rate req */
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("I2S BYPASS mode Set sample rate cmd fail %d",rc);
            goto sw_bypass_err;
        }
        iacore->active_route_set = TRUE;
        iacore->low_latency_route_set = TRUE;
    }
    else
    {
        cmd = IA_SET_SAMPLE_RATE;
        cmd = cmd << 16 | IA_SAMPLE_RATE_16KHZ;
        /* set IA6xx sampling rate req */
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("I2S BYPASS mode Set sample rate cmd fail %d",rc);
            goto sw_bypass_err;
        }

        /* set IA6xx Internal Mic Clock to 1.024 MHz */
        cmd = IA_SET_DEVICE_PARAMID;
        cmd = cmd << 16 | 0x1002;
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("set param command failed - %d", rc);
            goto sw_bypass_err;
        }

        cmd = IA_SET_DEVICE_PARAM;
        cmd = cmd << 16 | 0x0002;
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("set param value failed - %d", rc);
            goto sw_bypass_err;
        }

        /* set IA6xx PDMOUT0 Clock to 1.024 MHz */
        cmd = IA_SET_DEVICE_PARAMID;
        cmd = cmd << 16 | 0x1402;
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("set param command failed - %d", rc);
            goto sw_bypass_err;
        }

        cmd =IA_SET_DEVICE_PARAM;
        cmd = cmd << 16 | 0x0002;
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("set param value failed - %d", rc);
            goto sw_bypass_err;
        }
        msleep(10);

        /* set IA6xx port clock frequency */
        cmd = IA_SET_AUD_PORT_CLK_FREQ;
        cmd = cmd << 16 | 0x0002;
        rc = iacore_uart_cmd(iacore, cmd, &rsp);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("I2S BYPASS mode aud port clk freq cmd fail %d", rc);
            goto sw_bypass_err;
        }
        if(iacore->multimic == COMBINED_TWO_MIC)
        {
            iacore_set_active_route(iacore, TRUE, IA_2CH_PDM_IN_PDM_OUT_LL);
        }
        else
        {
            iacore_set_active_route(iacore, TRUE, IA_1CH_PDM_IN0_PDM_OUT0_LL);
        }
    }
sw_bypass_err:
    return rc;
}

static int iacore_stop_active_route(struct iacore_priv *iacore)
{
    uint32 cmd, rsp;
    int rc;

    if (iacore->active_route_set == FALSE)
        return 0;

    /* For low latency routes, just send wakeup byte to stop the route */
    if (iacore->low_latency_route_set == TRUE)
    {
        rc = iacore_wakeup(iacore);
        if (rc)
        {
            KNOWLES_SMART_MIC_DEBUG("wakeup failed rc = %d\n",rc);
        }
        iacore->low_latency_route_set = FALSE;
        iacore->active_route_set = FALSE;
        return rc;
    }
    /* stop any active route */
    cmd = IA_SET_STOP_ROUTE;
    cmd = cmd << 16 | IA_STOP_ROUTE_VALUE;
    rc = iacore_uart_cmd(iacore, cmd, &rsp);
    if (rc)
    {
        KNOWLES_SMART_MIC_DEBUG("stop route fail %d\n", rc);
    }
    else
    {
        iacore->active_route_set = FALSE;
    }
    return rc;

}


static int iacore_vs_configure(struct iacore_priv *iacore)
{
	int rc;
	uint32 cmd, rsp;
	struct iacore_voice_sense *voice_sense = GET_VS_PRIV(iacore);

	if (!voice_sense->kw_model_loaded) {
		if (!voice_sense->kw) {
			KNOWLES_SMART_MIC_DEBUG("%s(): no keyword stored\n",
				__func__);
			rc = -EINVAL;
			goto vs_config_err;
		}

		/* reset algo memory */
		rc = iacore_vs_set_algo_params(iacore, IA_ALGO_MEM_RESET);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): set algo params fail %d\n",
				__func__, rc);
			goto vs_config_err;
		}

		/* TODO fix the sleep */
		msleep(10);
		/* write background model and keywords files */
		rc = iacore_vs_write_keywords(iacore);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): datablock write fail rc = %d\n",
				__func__, rc);
			goto vs_config_err;
		}
		voice_sense->kw_model_loaded = TRUE;

	    cmd = IA_EVENT_RESPONSE_CMD;
		cmd = cmd << 16 | IA_LEVEL_TRIGGER_LOW;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param ID cmd fail %d\n",
				__func__, rc);
			goto vs_config_err;
		}
	}

	/* TODO fix the Sleep */
	msleep(15);

	if(iacore->use_preset)
	{
		rc = iacore_stop_active_route(iacore);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s: failed to put the chip to sleep = %d\n",
				__func__, rc);
			goto vs_config_err;
		}
		msleep(30);
//#ifndef KNOWLES_UART_DATA //this command is supported for both PDM and UART in next release binary
		cmd = 0x803C0120;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("Set preroll command failed %d",rc);
			goto vs_config_err;
		}
//#endif
		cmd = IA_SET_PRESET;
		if (voice_sense->params.mode == IA_VQ_MODE)
		{
			if(voice_sense->params.vad == IA_MIC_VAD)
				cmd = cmd << 16 | PRESET_VAD_ON_VQ_NO_BUFFERING;
			else
				cmd = cmd << 16 | PRESET_VAD_OFF_VQ_NO_BUFFERING;
		}
		else/*CVQ MODE*/
		{
			if(voice_sense->params.kw_preserve == IA_PRESERVE_KW)
			{
				if(voice_sense->params.vad == IA_MIC_VAD)
					cmd = cmd << 16 | PRESET_VAD_ON_CVQ_KW_PRSRV;
				else
					cmd = cmd << 16 | PRESET_VAD_OFF_CVQ_KW_PRSRV;
			}
			else
			{
				if(voice_sense->params.vad == IA_MIC_VAD)
					cmd = cmd << 16 | PRESET_VAD_ON_CVQ_NO_KW_PRSRV;
				else
					cmd = cmd << 16 | PRESET_VAD_OFF_CVQ_NO_KW_PRSRV;
			}
		}
		/* set preset */
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("Set Preset failed %d",rc);
			goto vs_config_err;
		}
		else
		{
			iacore->active_route_set = TRUE;
		}
	}
	else
	{
		/* Disabled buffering command as the CVQ buffer is removed from the sysconfig
		 * Enable this command if we are using CVQ buffer enabled sysconfig */
#if 1
		if (voice_sense->params.mode == IA_VQ_MODE){
			cmd = IA_KW_PRESERVATION;
			cmd = cmd << 16 | IA_DISABLE_BUFFERING;
		}
		else if (voice_sense->params.kw_preserve == IA_PRESERVE_KW){
			cmd = IA_KW_PRESERVATION;
			cmd = cmd << 16 | IA_ENABLE_PRESERVATION;
		}
		else{
			cmd = IA_KW_PRESERVATION;
			cmd = cmd << 16 | IA_DISABLE_PRESERVATION;
		}
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): Set keyword preserve cmd failed %d\n",__func__, rc);
			goto vs_config_err;
		}
#endif
		/* TODO fix the sleep */
		msleep(10);
		rc = iacore_vs_route_preset(iacore);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): vs route preset fail %d\n",
				__func__, rc);
			goto vs_config_err;
		}

		rc = iacore_vs_set_algo_params(iacore, IA_ALGO_MEM_SET);
		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("%s(): set algo params fail %d\n",
				__func__, rc);
			goto vs_config_err;
		}
	 }

	cmd = IA_SET_ALGO_PARAM_ID;
	cmd = cmd << 16 | IA_OEM_SENSITIVITY;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param VS processing cmd fail %d\n",
			__func__, rc);
		goto vs_config_err;
	}
	cmd = IA_SET_ALGO_PARAM;
	cmd = cmd << 16 | voice_sense->sensitivity;;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): Set Algo Param keyword detect cmd fail %d\n",
			__func__, rc);
		goto vs_config_err;
	}

vs_config_err:
	return rc;
}

static int iacore_set_vs_sleep(struct iacore_priv *iacore)
{
	int rc = 0;

	struct iacore_voice_sense *voice_sense = GET_VS_PRIV(iacore);
	uint32 cmd, rsp;

	if (!voice_sense) {
		KNOWLES_SMART_MIC_DEBUG("%s(): invalid private pointer\n",
			__func__);
		return -EINVAL;
	}

	if (voice_sense->params.vad == IA_MIC_VAD) {
        cmd = IA_SET_POWER_STATE;
        cmd = (cmd << 16) | IA_SET_CHIP_SLEEP;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc)
			KNOWLES_SMART_MIC_DEBUG("%s(): Set chip sleep cmd fail %d\n",
					__func__, rc);
	} else {
		/* set the chip to no vad */
        cmd = IA_SET_POWER_STATE;
		cmd = (cmd << 16) | IA_SET_LOWPOWER;
		rc = iacore_uart_cmd(iacore, cmd, &rsp);
		if (rc)
			KNOWLES_SMART_MIC_DEBUG("%s(): Setting chip to low power failed %d\n",
					__func__, rc);
	}
	if(rc==0)
		KNOWLES_SMART_MIC_DEBUG("iacore_set_vs_sleep SUCCESS!!!");
	return rc;
}


static int ia6xx_set_streaming_burst(struct iacore_priv *iacore, int value)
{
	uint32 resp = 0, cmd;
    char res8[4],cmd8[4];
	int rc, resp_req;

	/* retry reason is explained below */
	int retry;

	if (value == IACORE_STREAM_ENABLE) {
		TRACE("Start Bursting.......\n");
		rc = iacore_uart_cmd(iacore,IACORE_SET_START_STREAM_BURST_CMD | value, &resp);
	} else {
		/* On sending stop streaming command during the streaming,
		 * The response of the command will be consumed by the
		 * streaming_producer thread, so using iacore_cmd_nopm will
		 * trigger the firmware recovery if the response is not
		 * received */

		//reinit UART CMD transport
		iacore_configure_uart_dev(iacore->dev_data->baudrate_vs, (unsigned char)UART_VM_STOP_BITS);

		TRACE("Stop Bursting.......\n");
#if 1
		
	 	cmd = IACORE_SET_STOP_STREAM_BURST_CMD | value;
		resp_req = !(cmd & (0x10000000));

        uint32_to_uint8_array(cmd,(uint8 *)cmd8);
		rc = iacore_uart_write(iacore,cmd8,(int)sizeof(cmd8));

		/* TODO Fix this sleep */
		msleep(30);

		iacore_configure_uart_dev(iacore->dev_data->baudrate_vs, (unsigned char)UART_VM_STOP_BITS);

		/* Need to read all stream data from IA6xx to clean up the
		 * buffers. Firmware Tx buffer depends on various factors and
		 * the max number of 32 bit words in Tx buffer could be FIFO
		 * depth(16) + buffer size(32) = 48 words or 192 bytes.
		 */
		retry = IA_STOP_STRM_CMD_RETRY;

		if (resp_req) {
			do {
                uint32_to_uint8_array(resp,(uint8 *)res8);
				rc = iacore_uart_read(iacore, res8,(int)sizeof(res8));
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: resp read failed, error=%d\n",
						__func__, rc);
				}
				if (resp == cmd)
					break;
				msleep(5);
			} while (retry--);


			if (retry == 0) {
				KNOWLES_SMART_MIC_DEBUG("%s: stop streaming response retry failed\n",
					__func__);
				return -EIO;
			}

			/* Sending SYNC command */
            cmd = IA_SYNC_CMD;
			rc = iacore_uart_cmd(iacore,(cmd << 16) | IA_SYNC_POLLING, &resp);
			if (rc) {
				KNOWLES_SMART_MIC_DEBUG("%s: Failed to send SYNC cmd, error %d\n",
					__func__, rc);
				return rc;
			}
		}
#endif
	}


	return rc;
}


static int iacore_wakeup(struct iacore_priv *iacore)
{
	uint32 cmd;
	uint32 rsp;
	int rc = 0;
	int retry = 10;
	uint8 current_state;

	/* SHP-4573/SHP-4598: chip wakeup immediately after sleep fails */
	msleep(30);

	do {
		current_state = iacore_get_power_state(iacore);
		if (iacore->low_latency_route_set == TRUE)
            goto skip_state_check;
		if (current_state != FW_HW_BYPASS &&
		    current_state != VS_SLEEP &&
		    current_state != HW_BYPASS &&
		    current_state != FW_SLEEP) {
			KNOWLES_SMART_MIC_DEBUG("%s firmware is not in sleep\n",
				 __func__);
			break;
		}

		/* Bus specific callback is called, if any callback is
		 * registered. */
skip_state_check:
        rc = iacore_uart_wakeup(iacore);
        if (rc) {
            KNOWLES_SMART_MIC_DEBUG("%s() Wakeup failed rc = %d\n",
                    __func__, rc);
            goto iacore_wakeup_exit;
        }
		msleep(30);
		/* Send sync command to verify device is active */
		/* stop active route */
        cmd = IA_SET_STOP_ROUTE;
        cmd = cmd << 16 | IA_STOP_ROUTE_VALUE;
		rc = iacore_uart_cmd(iacore,cmd, &rsp);
		if (rc < 0) {
			KNOWLES_SMART_MIC_DEBUG("%s(): failed sync cmd resume %d\n",
					__func__, rc);
		}

		/* Response will not be same as command if the chip is in SBL */
		if (cmd != (rsp & (0xFFFF0000))) {
			KNOWLES_SMART_MIC_DEBUG("%s(): failed sync rsp resume %d\n",
					__func__, rc);
			rc = -EIO;
		}
	} while (rc && --retry);

iacore_wakeup_exit:
	return rc;
}


int8 iacore_event_status()
{
    uint32 cmd, event_type;
	int rc;

    cmd = IA_GET_EVENT_CMD;
	rc = iacore_uart_cmd(iacore, cmd, &event_type);
	if (rc){
		KNOWLES_SMART_MIC_DEBUG("Error reading IRQ event %d\n", rc);
        return rc;
    }
    KNOWLES_SMART_MIC_DEBUG("event 0x%08lx\n", event_type);

    event_type &= IA_MASK_INTR_EVENT;
    return event_type;
}

void iacore_init_model(struct iacore_priv *iacore)
{
    int i;
    if(iacore->voice_sense->kw_count > MAX_KW_MODELS)
    {
      KNOWLES_SMART_MIC_DEBUG("[%s]kwd count is greter than supported by binary loading only [%d] keywords\n", __func__,MAX_KW_MODELS); 
      iacore->voice_sense->kw_count = MAX_KW_MODELS;
    }

    for(i=0;i < iacore->voice_sense->kw_count; i++)
    {
        iacore->voice_sense->kw[i] = available_keywords[iacore->voice_sense->kwds[i] - 1];
        iacore->voice_sense->kw_id[i] = iacore->voice_sense->kwds[i];
    }
}


bool iacore_is_keywords_changed()
{
    int8 cur_kw_count,previous_kw_count;

    /* reset unused keywords */
    for(cur_kw_count = iacore->voice_sense->kw_count; cur_kw_count < MAX_KW_MODELS; cur_kw_count++ )
    {
        iacore->voice_sense->kw_id[cur_kw_count] = 0;
        iacore->voice_sense->kw[cur_kw_count] = NULL;
    }
    for(cur_kw_count = 0; cur_kw_count < iacore->voice_sense->kw_count; cur_kw_count++)
    {
        for(previous_kw_count = 0; previous_kw_count < iacore->voice_sense->kw_count; previous_kw_count++)
        {
            if( iacore->voice_sense->kwds[cur_kw_count] == iacore->voice_sense->kw_id[previous_kw_count] )
                break;
        }
        /* keyword is not available reload required */
        if(previous_kw_count >= iacore->voice_sense->kw_count)
        {
            return TRUE;
        }
    }
    return FALSE;

}

static int iacore_stop_sbl_bypass(struct iacore_priv *iacore)
{
	uint32 cmd, rsp;
	int rc;

	/* wake up the chip */
	rc = iacore_wakeup(iacore);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s() wakeup failed rc = %d\n",
				__func__, rc);
		return rc;
	}

	/* Disable PDM Bypass mode */
    cmd = IA_SET_BYPASS_MODE;
	cmd = cmd << 16 | IA_BYPASS_MODE_OFF;
	rc = iacore_uart_cmd(iacore, cmd, &rsp);
	if (rc)
		KNOWLES_SMART_MIC_DEBUG("Set BYPASS MODE cmd OFF fail %d\n", rc);

	return rc;
}


bool start_receive_interupt;


int firmware_state_machine(struct iacore_priv *iacore, uint8 new_state)
{
    int rc = 0;

    if (new_state == POWER_OFF)
		iacore_poweroff_chip(iacore);

    if (new_state == SBL) {
		iacore_poweroff_chip(iacore);
		iacore_power_ctrl(iacore, IA_POWER_ON);
		iacore->fw_state = SBL;
	}

    while (iacore->fw_state != new_state) {
		switch (iacore->fw_state) {
        case POWER_OFF:// first
			switch (new_state) {
			case FW_LOADED:
			case HW_BYPASS:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
			case IO_STREAMING:
                iacore_power_ctrl(iacore, IA_POWER_OFF);
                msleep(30);
				iacore_power_ctrl(iacore, IA_POWER_ON);
                msleep(30);
				iacore->fw_state = SBL;
				break;
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
		case SBL://second
			switch (new_state) {
			case FW_SLEEP:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_LOADED:
			case IO_STREAMING:
                rc = iacore_vs_load(iacore);
                if (rc) {
                    KNOWLES_SMART_MIC_DEBUG("%s() VS FW load failed rc = %d\n",
                                           __func__, rc);
                    iacore_power_ctrl(iacore, IA_POWER_OFF);
                    iacore->fw_state = POWER_OFF;
                    goto error;
                }
                iacore->fw_state = FW_LOADED;
                break;
#if 1				
			case HW_BYPASS:
				rc = iacore_set_bypass(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s() failed to set chip to hw bypass mode rc = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = HW_BYPASS;
				break;
#endif				
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
			
		case FW_LOADED:// third  set bypass second
			switch (new_state) {
			case FW_HW_BYPASS:
				rc = iacore_set_bypass(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s() failed to set chip to bypass mode rc = %d\n",
						__func__, rc);
					goto error;
				}else{
					KNOWLES_SMART_MIC_DEBUG("iacore_set_bypass SUCCESS!!!!!,START TALKING");
				}
				iacore->fw_state = FW_HW_BYPASS;
				break;
			case SW_BYPASS:
				rc = iacore_set_sw_bypass(iacore);
                if (rc) {
                    KNOWLES_SMART_MIC_DEBUG("%s() failed to set chip to sw bypass mode rc = %d\n",
                        __func__, rc);
                    goto error;
                }
				KNOWLES_SMART_MIC_DEBUG("nothing to do, caller has to setup the pt route %d\n",rc);
				iacore->fw_state = SW_BYPASS;
				break;
			case VS_MODE:
			case VS_SLEEP://write keyword
				rc = iacore_vs_configure(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to enable voice sense mode = %d\n",
							__func__, rc);
					goto error;
				}
				iacore->fw_state = VS_MODE;
				break;
			case FW_SLEEP:
				rc = iacore_set_chip_sleep(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to put the chip to sleep = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_SLEEP;
				break;
			case IO_STREAMING:
				/*rc = iacore_set_iostreaming(iacore,
						IACORE_IOSTREAM_ENABLE);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG(("%s: failed to start iostream = %d\n",
						__func__, rc));
					goto error;
				}*/
				iacore->fw_state = IO_STREAMING;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
#if 1			
		case HW_BYPASS:
			switch (new_state) {
			case SBL:
			case FW_LOADED:
			case FW_HW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
				/* wakeup the chip */
				rc = iacore_stop_sbl_bypass(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to stop hw bypass = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = SBL;
				break;
			case SW_BYPASS:
			case IO_STREAMING:
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
			
		case FW_HW_BYPASS:
			switch (new_state) {
			case FW_LOADED:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
				rc = iacore_stop_sbl_bypass(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to stop hw bypass = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_LOADED;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case SW_BYPASS:
			case IO_STREAMING:
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
#endif			
#if 1			
		case SW_BYPASS:
			switch (new_state) {
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
				rc = iacore_stop_active_route(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to put the chip to sleep = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_LOADED;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case IO_STREAMING:
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;			
		case IO_STREAMING:
			switch (new_state) {
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
				/*rc = iacore_set_iostreaming(iacore,
						IACORE_IOSTREAM_DISABLE);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG(("%s: failed to stop iostreaming = %d\n",
						__func__, rc));
					goto error;
				}*/
				iacore->fw_state = FW_LOADED;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
#endif			
		case VS_MODE://fourth
			switch (new_state) {
			case VS_SLEEP:
				rc = iacore_set_vs_sleep(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to put the chip to sleep = %d\n",
						__func__, rc);
					goto error;
				}else{
					iacore_irq_pin_enter_detect_mode();
					start_receive_interupt=true;
				}
				iacore->fw_state = VS_SLEEP;
				break;
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case FW_SLEEP:
				rc = iacore_stop_active_route(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s: failed to put the chip to sleep = %d\n",
						__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_LOADED;
				break;
			case VS_BURSTING:
				/* Busting is possible only from VS mode */
				rc = ia6xx_set_streaming_burst(iacore,IACORE_STREAM_ENABLE);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("failed to start streaming %d\n",rc);
					rc = -EBUSY;
					goto error;
				}
				iacore->fw_state = VS_BURSTING;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case IO_STREAMING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
			
		case VS_SLEEP://set bypass first
			switch (new_state) {
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case FW_SLEEP:
				/* wake up the chip from voice sense sleep */
				rc = iacore_wakeup(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s() wakeup failed rc = %d\n",
							__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_LOADED;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case VS_MODE:
			case IO_STREAMING:
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
#if 1			
		case VS_BURSTING:
			switch (new_state) {
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case FW_SLEEP:
				rc = ia6xx_set_streaming_burst(iacore,IACORE_STREAM_DISABLE);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s(): failed to turn off streaming: %d\n",
						__func__, rc);
					rc = -EIO;
					goto error;
				}
				iacore->fw_state = VS_MODE;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case IO_STREAMING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
		case FW_SLEEP:
			switch (new_state) {
			case FW_LOADED:
			case FW_HW_BYPASS:
			case SW_BYPASS:
			case VS_MODE:
			case VS_SLEEP:
			case IO_STREAMING:
				/* wake up the chip */
				rc = iacore_wakeup(iacore);
				if (rc) {
					KNOWLES_SMART_MIC_DEBUG("%s() wakeup failed rc = %d\n",
							__func__, rc);
					goto error;
				}
				iacore->fw_state = FW_LOADED;
				break;
			case HW_BYPASS:
				KNOWLES_SMART_MIC_DEBUG("%s: May not be valid state, bypass is possible from firmware load state\n",
					 __func__);
			case VS_BURSTING:
			default:
				KNOWLES_SMART_MIC_DEBUG("%s() Invalid transition requested = %d\n",
							__func__, new_state);
				rc = -EINVAL;
				break;
			}
			break;
#endif			
		default:
			KNOWLES_SMART_MIC_DEBUG("%s current mode is invalid = %d\n",
				__func__, iacore->fw_state);

			rc = -EINVAL;
            goto error;
			break;
		}

		if (rc) {
			KNOWLES_SMART_MIC_DEBUG("Error changing the power state:%d from %d to %d\n",
				rc, iacore->fw_state, new_state);
			break;
		}
	}
    error:
    return rc;
}


/******************* INIT FUNCTIONS ***************************************************/

static int iacore_vs_init(struct iacore_priv *iacore)
{
	int rc = 0;
    uint8 i;
	struct iacore_voice_sense *voice_sense;

    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);

	voice_sense = (struct iacore_voice_sense *)app_smart_mic_mempool_malloc(sizeof(struct iacore_voice_sense));
	TRACE("voice_sense_addr=%x  malloc_len=%d",voice_sense,sizeof(struct iacore_voice_sense));

	if (!voice_sense) {
		return -ENOMEM;
	}

	iacore->voice_sense = (struct iacore_voice_sense *)voice_sense;

	/* Initialize variables */
	voice_sense->hw= "knowles_firmware/sysConfig.bin";
	voice_sense->vs= "knowles_firmware/boskoApp.bin";

    /* Initialize the available keywords*/
    available_keywords[ALEXA-1]= "knowles_firmware/alexa.bin";

    /* Reset loaded keyword id's*/
    for(i=0;i<MAX_KW_MODELS;i++)
    {
        voice_sense->kw_id[i] = 0;
    }

    voice_sense->kw_model_loaded = FALSE;
	voice_sense->params.rate = IA_16KHZ;
	voice_sense->params.mode = IA_CVQ_MODE;
	voice_sense->params.format = IA_FORMAT_Q15;
	voice_sense->params.frame_size = IA_16MS_FRAME;
	voice_sense->params.kw_preserve = IA_PRESERVE_KW; //punith
	voice_sense->params.vad = IA_NO_VAD;
    voice_sense->sensitivity = IA_OEM_SENSITIVITY_VALUE;

	voice_sense->iostream_params.rate = IA_16KHZ;
	voice_sense->iostream_params.format = IA_FORMAT_Q15;
	voice_sense->iostream_params.frame_size = IA_16MS_FRAME;

	iacore->file_ptr= (unsigned char *)app_smart_mic_mempool_malloc(IA6XX_WDB_BLOCK_SIZE);

	return rc;
}


static int iacore_uart_init(struct iacore_priv *iacore)
{
	int rc = 0;
	struct iacore_uart_device *iacore_uart;

    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);

	/* Create driver private-data struct */
	iacore_uart = (struct iacore_uart_device*) app_smart_mic_mempool_malloc(sizeof(struct iacore_uart_device));
	TRACE("iacore_uart_addr=%x  malloc_len=%d",iacore_uart,sizeof(struct iacore_uart_device));
	if (!iacore_uart)
		return -ENOMEM;

    /* supported uart baud rates in sbl
    19.2 k
    28.8 k
    38.4 k
    57.6 k
    115.2 k*/
    //iacore_uart->baudrate_sbl = VM_UART_RATE_115K2;
	iacore_uart->baudrate_sbl = 115200;

    /*
    IA61x vs supported      IA61x
       baudrates           baudrate_vs_cmd

	    115200          -       0x480
        460800          -       0x1200
        921600          -       0x2400
        1000000         -       0x2710
        1024000         -       0x2800
        1152000         -       0x2D00
	    2000000         -       0x4E20
        2048000         -       0x5000
        3000000         -       0x7530
        3072000         -       0x7800
    */  
    iacore_uart->baudrate_vs = 921600;
    iacore_uart->baudrate_vs_cmd = 0x2400;

	iacore->dev_data = iacore_uart;

	return rc;
}

int iacore_init(void)
{
	int rc;

    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);
	app_smart_mic_mempool_init();

	/* Create iacore private-data struct */
	iacore = (struct iacore_priv*)app_smart_mic_mempool_malloc(sizeof(struct iacore_priv));
	TRACE("iacore_addr=%x  malloc_len=%d",iacore,sizeof(struct iacore_priv));
	if (!iacore)
		return -ENOMEM;

	rc = iacore_uart_init(iacore);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): voice sense init failed %d\n",
			__func__, rc);
		goto iacore_init_alloc_err;
	}

    /* Initialize vs variables */
	rc = iacore_vs_init(iacore);
	if (rc) {
		KNOWLES_SMART_MIC_DEBUG("%s(): voice sense init failed %d\n",
			__func__, rc);
		goto vs_init_error;
	}

    iacore->fw_state = POWER_OFF;
	iacore->active_route_set = FALSE;
    iacore->low_latency_route_set = FALSE;
    iacore->use_preset = TRUE;
    iacore->multimic = ONE_MIC;
    iacore->alexa_playback_active = FALSE;

    return rc;

vs_init_error:
	iacore_uart_exit(iacore);
iacore_init_alloc_err:
    return rc;
}


/******************* EXIT FUNCTIONS ***************************************************/
static int iacore_uart_exit(struct iacore_priv *iacore)
{
    int rc = 0;
	struct iacore_uart_device *iacore_uart = iacore->dev_data;

    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);

	//SourceClose(iacore_uart->uartSource);
	//SinkClose(iacore_uart->uartSink);

	//iacore_uart->uartSource = NULL;
	//iacore_uart->uartSink = NULL;

	free(iacore_uart);

	return rc;
}

static void iacore_vs_exit(struct iacore_priv *iacore)
{
    int i;
	struct iacore_voice_sense *voice_sense = GET_VS_PRIV(iacore);
	unsigned char *p_file=iacore->file_ptr;
    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);

	if(voice_sense->kw){
        for(i=0; i<voice_sense->kw_count; i++)
		    voice_sense->kw[i] = NULL;
		voice_sense->kw_model_loaded = FALSE;
	}
	if (voice_sense->vs)
		voice_sense->vs = NULL;
	if (voice_sense->hw)
		voice_sense->hw = NULL;

	free(voice_sense);
	free(p_file);
}

int iacore_exit(void)
{
	int rc = 0;

    KNOWLES_SMART_MIC_DEBUG("[%s]\n", __func__);

    iacore_poweroff_chip(iacore);
	rc = iacore_uart_exit(iacore);
    iacore_vs_exit(iacore);
    if(iacore)
        free(iacore);
    iacore = NULL;
	
	app_smart_mic_mempool_init();//clean memory

	return 0;
}



