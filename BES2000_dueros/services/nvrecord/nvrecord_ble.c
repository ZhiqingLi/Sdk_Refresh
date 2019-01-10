#include <assert.h>
#include "nvrecord.h"
#include "nvrecord_ble.h"
#include "nvrec_mempool.h"
#include "hal_trace.h"

#define ble_nv_debug
#ifdef ble_nv_debug
#define ble_trace TRACE
#else
#define ble_trace
#endif

extern nv_record_struct nv_record_config;
static struct Ble_nvrecord *nvrecord_ble_p = NULL;

static void blerec_specific_value_delete(const BleDeviceinfo* param_rec)
{
	uint8_t delete_index=0;
	struct Ble_nvrecord *dest_ptr;
	dest_ptr=nvrecord_ble_p;
	TRACE("%s start search addr %08x  list_num=%d",__func__,dest_ptr,dest_ptr->saved_list_num);
	if(dest_ptr->saved_list_num>=1){
		for(uint8_t i=0;i<dest_ptr->saved_list_num;i++){
			if(memcmp(dest_ptr->ble_nv[i].LTK,param_rec->LTK,BLE_LTK_SIZE)==0){
				ble_trace("%s XXXXXXX deleta one bonded info %d",__func__,i);
				memset(&(dest_ptr->ble_nv[i]),0,sizeof(BleDeviceinfo));
				delete_index=i;
				dest_ptr->saved_list_num=dest_ptr->saved_list_num-1;
				break;
			}else{
				delete_index=dest_ptr->saved_list_num;
			}
		}
	}
	//for example,delete the third one,move the second to the third and than move the first to the second
	for(uint8_t list_updata=0;list_updata<delete_index;list_updata++){
		memcpy(&(dest_ptr->ble_nv[delete_index-list_updata]),&(dest_ptr->ble_nv[delete_index-list_updata-1]),sizeof(BleDeviceinfo));
	}


	if(BLE_RECORD_NUM == dest_ptr->saved_list_num)
	{
		TRACE("<=====>blerec list is full,delete the oldest and add param_rec to list");
		memset(&(dest_ptr->ble_nv[BLE_RECORD_NUM-1]),0,sizeof(BleDeviceinfo));//delete the oldest
		for(uint8_t k=0;k<BLE_RECORD_NUM-1;k++){
			memcpy(&(dest_ptr->ble_nv[BLE_RECORD_NUM-1-k]),&(dest_ptr->ble_nv[BLE_RECORD_NUM-1-k-1]),sizeof(BleDeviceinfo));
		}
		dest_ptr->saved_list_num=dest_ptr->saved_list_num-1;
	}

    nv_record_config.is_update = true;
}

//the follow two case may use the function (1. after pair,LTK exchange,save the newest key 2.bond successful,updata the data list)
int nv_record_blerec_add(const BleDeviceinfo *param_rec)
{
	int nRet = 0; 

	nvrecord_ble_p = (struct Ble_nvrecord *)nvrec_config_get_int(nv_record_config.config, (const char *)section_name_ble_nvds,(const char *)NV_RECORD_BLE_KEY, NVRAM_BLE_INVALID);

	if (nvrecord_ble_p == (struct Ble_nvrecord *)NVRAM_BLE_INVALID){
		nvrecord_ble_p = (struct Ble_nvrecord *)pool_malloc(sizeof(struct Ble_nvrecord));
		if (!nvrecord_ble_p){
			ASSERT(0,"%s ble pool_malloc failed",__func__);
			return -1;
		}
		TRACE("ble nv malloc addr %08x",nvrecord_ble_p);
		memset(nvrecord_ble_p,0,sizeof(struct Ble_nvrecord));
		nvrecord_ble_p->saved_list_num=0;//init saved num
		nvrec_config_set_int(nv_record_config.config,section_name_ble_nvds,(const char *)NV_RECORD_BLE_KEY,(int)nvrecord_ble_p);
		
	}else{//data valid 
		blerec_specific_value_delete(param_rec);
	}

	//add device info into nv struct
	memcpy(nvrecord_ble_p->ble_nv[0].peer_bleAddr,param_rec->peer_bleAddr,BLE_ADDR_SIZE);//addr
	nvrecord_ble_p->ble_nv[0].EDIV=param_rec->EDIV;//EDIV
	memcpy(nvrecord_ble_p->ble_nv[0].RANDOM,param_rec->RANDOM,BLE_ENC_RANDOM_SIZE);//RANDOM
	memcpy(nvrecord_ble_p->ble_nv[0].LTK,param_rec->LTK,BLE_LTK_SIZE);//LTK
	nvrecord_ble_p->ble_nv[0].bonded=param_rec->bonded;//bond status
	nvrecord_ble_p->saved_list_num=nvrecord_ble_p->saved_list_num+1;//updata saved num
	
	nv_record_config.is_update = true;
	TRACE("%s  CURRENT BLE LIST NUM=%d",__func__,nvrecord_ble_p->saved_list_num);

#ifdef ble_nv_debug
	for(uint8_t k=0;k<nvrecord_ble_p->saved_list_num;k++){
		TRACE("NV EDIV %d  and random is:",nvrecord_ble_p->ble_nv[k].EDIV);
		DUMP8("%02x ",(uint8_t *)nvrecord_ble_p->ble_nv[k].RANDOM,8);
		TRACE("NV LTK:");
		DUMP8("%02x ",(uint8_t *)nvrecord_ble_p->ble_nv[k].LTK,16);
	}
#endif	
	return nRet;
}

//when master send encription req,if bonded,use ltk to bonding again(skip the pair step)
int nv_record_ble_record_find_ltk_through_ediv_and_random(uint16_t ediv,uint8_t *random,uint8_t *ltk)
{
	struct Ble_nvrecord *find_ptr;
	find_ptr=(struct Ble_nvrecord *)nvrec_config_get_int(nv_record_config.config, (const char *)section_name_ble_nvds,(const char *)NV_RECORD_BLE_KEY, NVRAM_BLE_INVALID);
	if (find_ptr == (struct Ble_nvrecord *)NVRAM_BLE_INVALID){
		TRACE("%s find data failed",__func__);
		return 0;
	}

	
	for(uint8_t find_index=0;find_index<find_ptr->saved_list_num;find_index++){
		if(find_ptr->ble_nv[find_index].EDIV==ediv){
			if(memcmp(find_ptr->ble_nv[find_index].RANDOM,random,BLE_ENC_RANDOM_SIZE)==0){
				ble_trace("%s FIND LTK IN NV SUCCESS %08x",__func__,find_ptr->ble_nv[find_index].LTK);
				memcpy(ltk,find_ptr->ble_nv[find_index].LTK,BLE_LTK_SIZE);
				return 0;
			}
		}
	}
	return 1;
}

bool nv_record_ble_record_Once_a_device_has_been_bonded()
{
	struct Ble_nvrecord *find_ptr;

	find_ptr=(struct Ble_nvrecord *)nvrec_config_get_int(nv_record_config.config, (const char *)section_name_ble_nvds,(const char *)NV_RECORD_BLE_KEY, NVRAM_BLE_INVALID);
	if (find_ptr == (struct Ble_nvrecord *)NVRAM_BLE_INVALID){
		return false;
	}
	for(uint8_t find_index=0;find_index<find_ptr->saved_list_num;find_index++){
		if(find_ptr->ble_nv[find_index].bonded==true){
			return true;
		}
	}
	return false;

}

