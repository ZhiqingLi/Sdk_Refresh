#ifndef NVRECORD_BLE_H
#define NVRECORD_BLE_H




#define BLE_RECORD_NUM 5
#define section_name_ble_nvds     "blenvds"
#define NV_RECORD_BLE_KEY  "ble_key"
#define NVRAM_BLE_INVALID (0xfae66666)





#define BLE_ADDR_SIZE 6
#define BLE_ENC_RANDOM_SIZE 8
#define BLE_LTK_SIZE 16

typedef struct _BleDeviceRecord
{
    U8         		peer_bleAddr[BLE_ADDR_SIZE];
	U16             EDIV;
	U8				RANDOM[BLE_ENC_RANDOM_SIZE];
	U8				LTK[BLE_LTK_SIZE];
    BOOL            bonded;

} BleDeviceinfo;

struct Ble_nvrecord
{
	uint8_t saved_list_num;
	BleDeviceinfo ble_nv[BLE_RECORD_NUM];
	
};


#ifdef __cplusplus
extern "C" {
#endif


int nv_record_blerec_add(const BleDeviceinfo *param_rec);
int nv_record_ble_record_find_ltk_through_ediv_and_random(uint16_t ediv,uint8_t *random,uint8_t *ltk);
bool nv_record_ble_record_Once_a_device_has_been_bonded();





#ifdef __cplusplus
}
#endif
#endif
