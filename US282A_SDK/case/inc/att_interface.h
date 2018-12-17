#ifndef __ATT_INTERFACE_H
#define __ATT_INTERFACE_H

typedef struct
{
    uint8 filename[11];
    uint8 reserved1[5];
    uint32 offset;
    uint32 length;
    uint8 reserved2[8];
} atf_dir_t;

typedef struct
{
    uint8 magic[8];
    uint8 sdk_version[4];
    uint8 reserved[20];
    atf_dir_t atf_dir[15];
} atf_head_t;

typedef struct
{
    /* 信息结构大小:
     * 测试程序可检查该大小以确定当前固件和测试程序是否兼容,
     * 如果不兼容则需要重启从卡启动重烧固件;
     */
    uint8  info_size;
    
    /* 测试阶段:
     * 0: config 应用挂起等待直到测试阶段 1;
     * 1: config 应用继续运行并启动其它程序;
     */
    uint8  test_stage;
    
    uint8  base_type;
    uint8  stub_phy_type;
    uint32 ap_file_offset;
    uint32 cfg_file_offset;
    uint32 cfg_file_len;
}test_ap_info_t;

typedef struct
{
    stub_ext_cmd_t ext_cmd;
    uint16 offsetl;
    uint16 offseth;
    uint16 lengthl;
    uint16 lengthh;
}att_fread_arg_t;

#endif
