#ifndef __APP_DMA_INFO_H__
#define __APP_DMA_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

// get Customized information
char* app_get_info_dma_key(void);
char* app_get_info_dma_product_id(void);
char* app_get_info_dma_serial_num(void);
char* app_get_info_dma_device_name(void);
char* app_get_info_dma_device_type(void);
char* app_get_info_dma_manufacturer(void);
char* app_get_info_dma_firmware_version(void);
char* app_get_info_dma_software_version(void);

// check bt and ble addr func
int app_check_bt_ble_addr(void);


#ifdef __cplusplus
}
#endif

#endif // __APP_DMA_INFO_H__
