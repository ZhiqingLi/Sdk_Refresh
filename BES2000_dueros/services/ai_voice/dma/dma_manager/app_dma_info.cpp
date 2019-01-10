#include "nvrecord.h"
#include "cmsis_os.h"
#include "dueros_dma.h"
#include "app_dma_info.h"


#define  DMA_KEY  "R2rx57tF47YsQBPXP5ZtIlZqfbAciobQ" // linkplay KEY
#define  DMA_PRODUCT_ID "MTtSzapshal9iihMGW4yjpk1hmkOVtB1" // linkplay id
#define  DMA_SERIAL_NUM  "11111111"
#define  DMA_DEVICE_NAME  "bumblebee"
#define  DMA_DEVICE_TYPE "DOCK"
#define  DMA_MANUFACTURER  "LinkPlay"
#define  DMA_FIRMWARE_VERSION  "1"  // version is 1~999
#define  DMA_SOFTWARE_VERSION  "1"  // version is 1~999

char* app_get_info_dma_key(void)
{
	return DMA_KEY;
}

char* app_get_info_dma_product_id(void)
{
	return DMA_PRODUCT_ID;
}

char* app_get_info_dma_serial_num(void)
{
	return DMA_SERIAL_NUM;
}

char* app_get_info_dma_device_name(void)
{
	return DMA_DEVICE_NAME;
}

char* app_get_info_dma_device_type(void)
{
	return DMA_DEVICE_TYPE;
}

char* app_get_info_dma_manufacturer(void)
{
	return DMA_MANUFACTURER;
}

char* app_get_info_dma_firmware_version(void)
{
	return DMA_FIRMWARE_VERSION;
}

char* app_get_info_dma_software_version(void)
{
	return DMA_SOFTWARE_VERSION;
}

void dma_notify_speech_state(SpeechState speech_state)
{
    TRACE("[%s] speech_state=%d",__func__,speech_state);

	switch(speech_state)
	{
		case SPEECH_STATE__IDLE:
		{
			if(dueros_dma_get_speech_state() == SPEECH_STATE__LISTENING)
			{
				/*here	ASR end*/
			}

			break;
		}
		case SPEECH_STATE__LISTENING:
		{
			/*here ASR start*/
			break;
		}
		case SPEECH_STATE__PROCESSING:
		{

			if(dueros_dma_get_speech_state() == SPEECH_STATE__LISTENING)
			{
				/*here	ASR end*/
			}
			break;
		}

		case SPEECH_STATE__SPEAKING:
		{
			/*not care*/
			break;
		}
		defalut:
		{
			break;
		}
	}
}

int app_check_bt_ble_addr(void)
{
	int ret = 0;
    BT_BD_ADDR dev_bt_addr, dev_ble_addr;

    memset(dev_bt_addr.addr, 0, sizeof(dev_bt_addr.addr));
    memset(dev_ble_addr.addr, 0, sizeof(dev_ble_addr.addr));

    ret = nvrec_dev_get_btaddr((char*)dev_bt_addr.addr);
    ret = nvrec_dev_get_bleaddr((char*)dev_ble_addr.addr);
    if(!ret){
        TRACE("get bt and ble addr fail!!!");
        return -1;
    }

    for(int i=0; i<BD_ADDR_SIZE; i++){
        if(dev_bt_addr.addr[i] != dev_ble_addr.addr[i]){
            TRACE("dev_bt_addr:");
            DUMP8("0x%02x ", dev_bt_addr.addr, BD_ADDR_SIZE);
            TRACE("dev_ble_addr:");
            DUMP8("0x%02x ", dev_ble_addr.addr, BD_ADDR_SIZE);
            
            TRACE("BT and BLE addr is not the same!!!");
            return -1;
        }
    }   

    return 0;
}


