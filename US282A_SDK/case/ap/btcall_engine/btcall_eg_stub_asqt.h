

/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief  asqt调试功能头谁的
 *      Cailizhen          2015-1          creat
 *       Wekan        2015-3-30       review
 *******************************************************************************/


#ifndef _BTCALL_EG_ASQT_H
#define _BTCALL_EG_ASQT_H

#include "btcall_engine.h"
#include "ud_interface.h"
#include "mmm_pp.h"

#include "psp_includes.h"
#include "case_include.h"
#include "stub_interface.h"

#define SET_ENABLE  1
#define SET_DISABLE (!SET_ENABLE)

#define DEBUG_RPINT_EN SET_ENABLE


#define MAX_AEC_PERIOD 6

#define ASQT_PERIOD_BLOCK  4

#define ASQT_CALL_MODE 1

#define ADDR_M4K2DSP(addr) (((((uint32)(addr))&0x1FFFF)>>1)|0x30000)//addr =0x9FC00000+[0~0x20000)
#define ADDR_DSP2M4K(addr) ((((((uint32)(addr))<<1)&(~0x40000))^0x20000)+0x9FC00000)//addr =[0x30000~0x40000)


#define AEC_PARM_OFS   0
#define ASQT_PARM_OFS   4

#define ASRC_IN_WCLK_CPU  (0u << 8)
#define ASRC_OUT0_RCLK_CPU  (0u << 8)

#define asrc_in_wclk()  (act_readl(ASRC_IN_CTL) & (3u << 8))
#define asrc_out_rclk()  (act_readl(ASRC_OUT0_CTL) & (3u << 8))

#define set_asrc_in_wclk_cpu() act_writel(act_readl(ASRC_IN_CTL) & (~(3u << 8)), ASRC_IN_CTL)
#define set_asrc_out0_rclk_cpu() act_writel(act_readl(ASRC_OUT0_CTL) & (~(3u << 8)), ASRC_OUT0_CTL)


#define DEC2HEX(d) ( ((d)%10) + ((((d)/10)%10)<<4) )

#define SIZE_ARR(a)  (sizeof(a)/sizeof(a[0]))


#define vint32 volatile uint32
#define CASE_TYPE_2825 (1)
#define CASE_TYPE_2823 (1)


typedef struct {

uint16 period_flag;
uint16 block_num;
uint32 frame_size;
uint32 frame_cnt;
uint32 *dsp_addr;

}asqt_debug_t;


//上传结果数据结构
typedef struct
{
   uint32 *pIn_data;   //p1,远端输入
   uint32 *pPLC;       //p2,plc后数据
   uint32 *pAGC;       //p3,前AGC输出
   uint32 *pMicData;   //p4,Mic录入数据
   uint32 *pOutAEC;    // p5,AEC输出
   uint32 *pOut_data;  //p6 远端输输出
   sv_aec_info_t *pAEC_info;
}sv_result_t;

typedef struct{
    
uint16 in_index;
uint16 out_index;
uint16 data_size;   //data里面当前数据大小
uint16 total_size;
uint8* data;

}Fifo_ctl_t;



typedef enum
{
    sNotReady = 0, //还没准备好或者用户停止后
    sReady,       //填写参数和选择了文件
    sRunning,     //调试正常运行中
    sUserStop,    //用户停止状态
    sUserUpdate,  //用户更新了参数
    sUserStart,   //用户正在测试状态
    sUserSetParam //下载参数到小机
} PC_curStatus_e;




//内存使用
//此buf前4k空间与calling铃声空间冲突
typedef struct //可用10.5k
{
    uint32 stub_buf[1024/4];// 1k
    uint16 ppv_data[(1024-256)/2];//缓存远端数据
    uint32 mic_data[(1024-256)/4];//接通后能用此空间，避免callin时冲突
    uint32 agc_data[(1024-256)/4];//接通后能用此空间，避免callin时冲突
    uint32 plc_data[(1024-256)/4];//
    uint32 share_buf[6*(256*ASQT_PERIOD_BLOCK)/4];//6k可以同时支持6个时期的数据
    uint32 rev_buf[512/4];
}sv_asqt_ram_t;


typedef struct{

//sv_result_t Result;//上传结果
Fifo_ctl_t ppv_fifo;
sv_aec_info_t AEC_info;
sv_configs_t AsqtCfg;
uint32 out_frame_cnt[MAX_AEC_PERIOD];//记录读出的帧数
uint32 m4k_addr[MAX_AEC_PERIOD];//dsp导出数据给m4k的地址
Fifo_ctl_t agc_fifo;
Fifo_ctl_t mic_fifo;
Fifo_ctl_t plc_fifo;
uint32 asqt_volume;  //取值范围为0 ~ 31，默认值为15


}asqt_parm_t;

//cpu与dsp共享内存部分--不能超128bytes
typedef struct
{
  uint32 change_flag;		//和dsp交互用，告诉DSP peq是否更新 方案端置位1 DSP获取数据后复位
  uint32 band_peq1;			//第一段PEQ地址
  uint32 band_peq2;			//第二段PEQ地址
}peq_info_t;

typedef struct{
    sv_prms_t *pAEC_parm;
    asqt_debug_t Asqt_ctl[6];
	peq_info_t  peq_info;
    uint32 debug[3];
}dsp_m4k_share_t;


/*************************ASQT增加PEQ调节功能******************************/
#define	STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE			(256)

#define	STUB_CMD_ASQT_EQ_NUMS_EVERY_PACKAGE				(10)
#define	STUB_CMD_ASQT_MAQ_EQ_NUMS						(20)
#define STUB_CMD_ASQT_WRITE_CONFIG_INFO      			(0x1000)

#define STUB_CMD_ASQT_READ_PEQ_STATUS      				(0x1100)
#define STUB_CMD_ASQT_READ_EQ1_DATAS      				(0x1200)
#define STUB_CMD_ASQT_READ_EQ2_DATAS      				(0x1300)

typedef struct
{
	uint8  stub_head[STUB_COMMAND_HEAD_SIZE];				//保留字节	
	uint32 eq_point_nums;   		//asqt支持的最大PEQ点数              
	uint32 eq_version;				//PEQ的版本号		
	uint8  fw_version[8];			//和ASQT		
	uint8  reserve[16];				//保留字节	
}asqt_interface_config_t;

typedef struct
{
	uint32 update_flag;				//1:peq有更新     0:表示没有更新
	uint32 package1_len;			
	uint32 package2_len;			
}asqt_peq_status_t;	

typedef struct
{
	uint8* band_peq1_addr;	
	uint8* band_peq2_addr;	
}asqt_peq_addr_t;	

typedef struct
{
	 uint32 eq_data_changed; //0:not change 1:change
}asqt_status_t;

//ASQT工具调试完后,工具导出.bin文件，在DSP加载之前需要将.bin文件内容送给dsp,.bin文件头部的数据结构如下
typedef struct
{
    int package_count;      	 //包的个数
    int package_len_array[32];   //每个包的长度数组
}asqt_para_head_t;


extern sv_asqt_ram_t *g_pAsqtRam;


extern uint32 g_agc_aec_shm_ptr;


extern dsp_m4k_share_t *g_p_share_mem;

extern sv_prms_t g_dsp_init_parm;//存放dsp初始化参数

extern int8 g_stub_task_timer_id;//定时器id
extern asqt_parm_t g_asqt_parm;



extern void init_m4k_dsp_ctl(uint8 en_ASQT_period);
extern void task_timer10ms(void);
extern void set_asrc_cpu_clk(void);
extern int  reset_aec_asqt(void);
extern int stub_asqt_get_parameter(uint32 *stub_buf, sv_configs_t *parm);
extern void deal_load_simu_mmm(void);
extern void show_parm(void);

extern int load_stub_drv(void);
extern uint32 get_stub_type(void);

extern void btcalleg_load_hfp_mmm(void);

extern void stub_asqt_send_end(uint8 cmd);
extern void deal_load_simu_mmm(void);

extern void btcall_eg_asqt_deal(uint8 stub_opened);
extern sv_configs_t* asqt_set_dsp_init_parm(void);

extern void actaec_init_sv_prms(sv_prms_t *);
extern void show_dat(char*s,uint32 *pd, uint32 len);
extern int stub_asqt_down_param(uint32 *stub_buf);

extern void reset_dsp(uint8 call_mode);

extern void btcall_eg_asqt_deal(uint8 stub_tool_type);
extern sv_configs_t* asqt_set_dsp_init_parm(void);
extern bool actaec_init_sv_prms_from_VM(sv_prms_t *dsp_cfg);

extern uint32 stub_asqt_write_data(uint16 cmd, void* buf, uint16 len);
void __section__(".rcode")get_asqt_peq_para_from_pc(asqt_peq_status_t* p_peq_status_t);

extern asqt_peq_addr_t asqt_peq_addr;

#endif

