/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：设备驱动接口定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef _STORAGE_DEVICE_H
#define _STORAGE_DEVICE_H

#define PLUG_OUT  -1
#define PLUG_IN 0
#define CHARGING 1

#include <ucos/init.h>

#ifndef _ASSEMBLER_

typedef void* (*device_op_i)(void*, void*, void*);

typedef enum {
	DEVICE_READ = 0,
	DEVICE_WRITE,
	DEVICE_UPDATE,
	DEVICE_GET_CAP,
	DEVICE_GET_SEC_SIZE,
	DEVICE_IS_WRITE_PROTECT,
} device_cmd_e;

/*non-base storage drviver struct*/
struct block_device_operations {
	device_op_i block_read;
	device_op_i block_write;
	device_op_i update_cache;
	device_op_i get_cap;
	device_op_i get_sector_size;
	device_op_i is_write_protect;
};

typedef enum {
	BASE_READ = 0,
	BASE_WRITE,
	BASE_UPDATE,
	BASE_GET_CAP,
	BASE_GET_SEC_SIZE,
	BASE_IS_WRITE_PROTECT,
	BASE_GET_INFO,
	BASE_MERGE_LOGBLOCK,
	BASE_MBREC_WRITE,
	BASE_BREC_WRITE,
	BASE_FW_WRITE,
	BASE_FW_READ,
	BASE_SD_READ
} base_stg_cmd_e;

struct base_block_device_operations {
	device_op_i block_read;
	device_op_i block_write;
	device_op_i update_cache;
	device_op_i get_cap;
	device_op_i get_sector_size;
	device_op_i is_write_protect;
	device_op_i get_info;
	device_op_i merge_logblock;
	device_op_i mbrec_write;
	device_op_i brec_write;
	device_op_i fw_write;
	device_op_i fw_read;
	device_op_i sd_read;
};

#define     BASE_GET_PAGE_SIZE              (0)
#define     BASE_GET_BASE_TYPE              (1)
#define     BASE_GET_BLOCK_SIZE             (2)
#define     BASE_GET_FLASH_ID               (3)
//提供给NOR使用，查询开机按键
#define     BASE_GET_TEST_KEY               (4)
//6和7 预留给NORBASE使用
#define     BASE_START_CTI_RW               (6)
#define     BASE_STOP_CTI_RW                (7)

#define     BASE_CLEAR_VRAM                 (8)
#define     BASE_VRAM_EXT_WRITE             (9)
#define     BASE_VRAM_EXT_READ              (10)
#define     BASE_PARAM_WRITE                (11)
#define     BASE_PARAM_READ                 (12)
#define     BASE_FREQ_COMPENSATION_WRITE    (13)
#define     BASE_FREQ_COMPENSATION_READ     (14)
#define     BASE_UPG_INIT                   (15)
#define     BASE_OTA_WRITE_QUERY            (16)
#define     BASE_OTA_CLEAR_PART             (17)
#define     BASE_DSP_START                  (18)
#define     BASE_OTA_PREPARE                (19)
#define     BASE_OTA_PART_QUERY             (20)
#define     BASE_QUERY_FIRST_BOOT           (30)
#define     BASE_PARAM_MERGE_DEAL           (31)
#define     BASE_SET_WRITE_ONOFF            (32)
#define     BASE_MERGE_VRAM                 (33)
#define     BASE_RW_VRAM_INDEX              (34)
#define     BASE_SET_S3BT_SCENE             (35)

/*3.对上层提供的总接口函数*/
extern void * card_op_entry(void *param1, void *param2, void *param3, device_cmd_e cmd)__FAR__;
extern void * uhost_op_entry(void *param1, void *param2, void *param3, device_cmd_e cmd)__FAR__;
extern void * nand_op_entry(void *param1, void *param2, void *param3, device_cmd_e cmd)__FAR__;
extern void * base_op_entry(void *param1, void *param2, void *param3, device_cmd_e cmd)__FAR__;

#define base_read(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_READ)
#define base_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_WRITE)
#define base_update(a, b,c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_UPDATE)
#define base_get_cap() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_CAP)
#define base_get_sec_size() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_SEC_SIZE)
#define base_is_write_protect() base_op_entry((void*)0, (void*)0, (void*)0, BASE_IS_WRITE_PROTECT)
#define base_get_page_size() base_op_entry((void*)BASE_GET_PAGE_SIZE, (void*)0, (void*)0, BASE_GET_INFO)
#define base_get_base_type() base_op_entry((void*)BASE_GET_BASE_TYPE, (void*)0, (void*)0, BASE_GET_INFO)
#define base_get_block_size() base_op_entry((void*)BASE_GET_BLOCK_SIZE, (void*)0, (void*)0, BASE_GET_INFO)
#define base_get_flash_id() base_op_entry((void*)BASE_GET_FLASH_ID, (void*)0, (void*)0, BASE_GET_INFO)
#define base_clear_vram() base_op_entry((void*)BASE_CLEAR_VRAM, (void*)0, (void*)0, BASE_GET_INFO)
#define base_ext_vram_read(a) base_op_entry((void*)BASE_VRAM_EXT_READ, (void*)(a), (void*)0, BASE_GET_INFO)
#define base_ext_vram_write(a) base_op_entry((void*)BASE_VRAM_EXT_WRITE, (void*)(a), (void*)0, BASE_GET_INFO)
#define base_param_read(a) base_op_entry((void*)BASE_PARAM_READ, (void*)(a), (void*)0, BASE_GET_INFO)
#define base_param_write(a) base_op_entry((void*)BASE_PARAM_WRITE, (void*)(a), (void*)0, BASE_GET_INFO)
#define base_trim_cap_read(a,b) base_op_entry((void*)BASE_FREQ_COMPENSATION_READ, (void*)(a), (void*)(b), BASE_GET_INFO)
#define base_trim_cap_write(a,b) base_op_entry((void*)BASE_FREQ_COMPENSATION_WRITE, (void*)(a), (void*)(b), BASE_GET_INFO)
#define base_upg_init(a)  base_op_entry((void*)BASE_UPG_INIT, (void*)(a), (void*)(0), BASE_GET_INFO)

#define base_part_table_write(a)  base_op_entry((void*)(a), (void*)(0), (void*)(0), BASE_MERGE_LOGBLOCK)
#define base_part_data_write(a, b, c)  base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_SD_READ)

#define base_ota_part_write_query(a) base_op_entry((void*)BASE_OTA_WRITE_QUERY, (void*)(a), (void*)(0), BASE_GET_INFO)
#define base_ota_clear_part_table() base_op_entry((void*)BASE_OTA_CLEAR_PART, (void*)(0), (void*)(0), BASE_GET_INFO)
#define base_dsp_start(a)   base_op_entry((void*)BASE_DSP_START, (void*)(a), (void*)(0), BASE_GET_INFO) 
#define base_ota_prepare(a,b) base_op_entry((void*)BASE_OTA_PREPARE, (void*)(a), (void*)(b), BASE_GET_INFO)
#define base_ota_read_part_info() base_op_entry((void*)BASE_OTA_PART_QUERY, (void*)(0), (void*)(0), BASE_GET_INFO)

#define base_query_first_boot() base_op_entry((void*)BASE_QUERY_FIRST_BOOT, (void*)(0), (void*)(0), BASE_GET_INFO)
#define base_param_merge_deal() base_op_entry((void*)BASE_PARAM_MERGE_DEAL, (void*)(0), (void*)(0), BASE_GET_INFO)
#define base_set_enable_write_protect() base_op_entry((void*)BASE_SET_WRITE_ONOFF, (void*)(1), (void*)(0), BASE_GET_INFO)
#define base_set_disable_write_protect() base_op_entry((void*)BASE_SET_WRITE_ONOFF, (void*)(0), (void*)(0), BASE_GET_INFO)
#define base_query_support_write_protect(a) base_op_entry((void*)BASE_SET_WRITE_ONOFF, (void*)(2), (void*)(a), BASE_GET_INFO)
#define base_special_vram_merge(a, b) base_op_entry((void*)BASE_MERGE_VRAM, (void*)(a), (void*)(b), BASE_GET_INFO)
#define base_clear_vram_index(a) base_op_entry((void*)BASE_RW_VRAM_INDEX, (void*)(a), (void*)(0), BASE_GET_INFO)
#define base_read_vram_index(a) base_op_entry((void*)BASE_RW_VRAM_INDEX, (void*)(a), (void*)(1), BASE_GET_INFO)
#define base_enter_s3bt_scene() base_op_entry((void*)BASE_SET_S3BT_SCENE, (void*)(1), (void*)(0), BASE_GET_INFO)
#define base_exit_s3bt_scene() base_op_entry((void*)BASE_SET_S3BT_SCENE, (void*)(0), (void*)(0), BASE_GET_INFO)

//复用了获取sector大小接口
#define base_ota_part_write(a) base_op_entry((void*)(a), (void*)(0), (void*)(0), BASE_GET_SEC_SIZE)

/****************************************************************************************
card base 增加的接口
*****************************************************************************************/
#define base_start_cti_rw()   base_op_entry((void*)BASE_START_CTI_RW, (void*)0, (void*)0, BASE_GET_INFO)
#define base_stop_cti_rw()    base_op_entry((void*)BASE_STOP_CTI_RW, (void*)0, (void*)0, BASE_GET_INFO)
#define base_cti_read(a,b,c)  base_op_entry((void*)(a), (void*)b, (void*)c, BASE_MERGE_LOGBLOCK)
#define base_cti_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_MBREC_WRITE)
#define base_merge_logblock(a) base_op_entry((void*)(a), (void*)0, (void*)0, BASE_MERGE_LOGBLOCK)
#define base_enter_compact_mode() base_op_entry((void*)(0), (void*)1, (void*)0, BASE_MERGE_LOGBLOCK)
#define base_exit_compact_mode() base_op_entry((void*)(0), (void*)2, (void*)0, BASE_MERGE_LOGBLOCK)
#define base_mbrec_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_MBREC_WRITE)
#define base_brec_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_BREC_WRITE)
#define base_fw_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_FW_WRITE)
#define base_fw_read(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_FW_READ)
#define base_sd_read(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_SD_READ)

#ifndef NOR_BASE
#define nand_read(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_READ)
#define nand_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_WRITE)
#define nand_update() base_op_entry((void*)0, (void*)0, (void*)0, BASE_UPDATE)
#define nand_get_cap() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_CAP)
#define nand_get_sec_size() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_SEC_SIZE)
#define nand_is_write_protect() base_op_entry((void*)0, (void*)0, (void*)0, BASE_IS_WRITE_PROTECT)
#else
#define nor_read(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_READ)
#define nor_write(a, b, c) base_op_entry((void*)(a), (void*)(b), (void*)(c), BASE_WRITE)
#define nor_update() base_op_entry((void*)0, (void*)0, (void*)0, BASE_UPDATE)
#define nor_get_cap() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_CAP)
#define nor_get_sec_size() base_op_entry((void*)0, (void*)0, (void*)0, BASE_GET_SEC_SIZE)
#define nor_is_write_protect() base_op_entry((void*)0, (void*)0, (void*)0, BASE_IS_WRITE_PROTECT)
#endif

#define card_read(a, b, c) card_op_entry((void*)(a), (void*)(b), (void*)(c), DEVICE_READ)
#define card_write(a, b, c) card_op_entry((void*)(a), (void*)(b), (void*)(c), DEVICE_WRITE)
#define card_update() card_op_entry((void*)0, (void*)0, (void*)0, DEVICE_UPDATE)
#define card_get_cap() card_op_entry((void*)0, (void*)0, (void*)0, DEVICE_GET_CAP)
#define card_get_sec_size() card_op_entry((void*)0, (void*)0, (void*)0, DEVICE_GET_SEC_SIZE)
#define card_is_write_protect() card_op_entry((void*)0, (void*)0, (void*)0, DEVICE_IS_WRITE_PROTECT)

#define uhost_read(a, b, c) uhost_op_entry((void*)(a), (void*)(b), (void*)(c), DEVICE_READ)
#define uhost_write(a, b, c) uhost_op_entry((void*)(a), (void*)(b), (void*)(c), DEVICE_WRITE)
#define uhost_update() uhost_op_entry((void*)0, (void*)0, (void*)0, DEVICE_UPDATE)
#define uhost_get_cap() uhost_op_entry((void*)0, (void*)0, (void*)0, DEVICE_GET_CAP)
#define uhost_get_sec_size() uhost_op_entry((void*)0, (void*)0, (void*)0, DEVICE_GET_SEC_SIZE)
#define uhost_is_write_protect() uhost_op_entry((void*)0, (void*)0, (void*)0, DEVICE_IS_WRITE_PROTECT)

#endif

#endif

