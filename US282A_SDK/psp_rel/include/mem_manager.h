#ifndef __MEM_MANAGER_H
#define __MEM_MANAGER_H

//是否监控缓存临时内存池数据被覆盖
#define MPU_MONITOR_MEM_POOL

//是否监控缓存bank是否数据被覆盖
#define BANK_DATA_MONITOR 

//0x29000-0x34000
#define DSP_CACHE_MEM_ADDR              (0x29000)
#define DSP_CACHE_MEM_SIZE              (44 * 1024)

//0x29000-0x30000
#define DSP_CACHE_MEM_ADDR_L            (0x29000)       //usound使用，不使用0x33000-0x34000的空间
#define DSP_CACHE_MEM_SIZE_L            (28 * 1024)

//0x28000-0x34000
#define DSP_CACHE_MEM_ADDR2             (0x28000)
#define DSP_CACHE_MEM_SIZE2             (48 * 1024)

//0x34800-0x3c000
#define DSP_CACHE_MEM_ADDR_H            (0x34800)
#define DSP_CACHE_MEM_SIZE_H            (30 * 1024)

//0x34800-0x38000
#define DSP_CACHE_MEM_ADDR_H2           (0x34800)
#define DSP_CACHE_MEM_SIZE_H2           (14 * 1024)

#define URAM1_CACHE_MEM_ADDR            (0x3f800)
#define URAM1_CACHE_MEM_SIZE            (0x800)

#define PCMRAM1_CACHE_MEM_ADDR          (0x3e000)
#define PCMRAM1_CACHE_MEM_SIZE          (0x800)

#define CARD_FIFO_MEM_ADDR              (0xfc00)
#define CARD_FIFO_MEM_SIZE              (0x400)

#define CARD_FS_CACHE_MEM_ADDR          (0x16c00)
#define CARD_FS_CACHE_MEM_SIZE          (0x2980)

#define SBC_INPUT_BUFFER_ADDR           (0x16400)
#define SBC_INPUT_BUFFER_SIZE           (0x2980 + 0x800)

#define KEYTONE_CACHE_URAM0_ADDR0       (0x3f000)
#define KEYTONE_CACHE_URAM0_SIZE        (0x800)

#define KEYTONE_CACHE_PCMRAM2_ADDR0     (0x3e000)
#define KEYTONE_CACHE_PCMRAM2_SIZE      (0x1000)

#define RADIO_FS_CACHE_MEM_ADDR         (0x17400)
#define RADIO_FS_CACHE_MEM_SIZE         (0x2180)

#define WAV_DECODE_CACHE_ADDR           (0x2c000)
#define WAV_DECODE_CACHE_SIZE           (32 * 1024)

#define WAV_DECODE_CACHE_ADDR2          (0x2c000)       //usound使用，不使用0x33000-0x34000的空间
#define WAV_DECODE_CACHE_SIZE2          (16 * 1024)

#define TTS_MEM_CACHE_ADDR              (0x38000)
#define TTS_MEM_CACHE_SIZE              (14 * 1024)

#define SEND_BUFFER_ADDR                (0x30800)
#define SEND_BUFFER_SIZE                (14 * 1024)

#define USOUND_DATA_BUFFER_ADDR         (0x30000)
#define USOUND_DATA_BUFFER_SIZE         (0x4000)

#define USPEAKER_DATA_BUFFER_ADDR       (0x30000)
#define USPEAKER_DATA_BUFFER_SIZE       (0x3000)

#define USPEAKER24_DATA_BUFFER_ADDR     (0x28000)
#define USPEAKER24_DATA_BUFFER_SIZE     (0x4000)

#define SBC_INPUT_BUFFER_LOW_ADDR       (0x16400)
#define SBC_INPUT_BUFFER_LOW_SIZE       (0x800)

/* 18位内存地址 */
#define __addr18(_a32)  \
    \
    ((uint32)(_a32) & 0x3ffff)

/* 32位转16位内存地址 */
#define __addr16(_a32)  \
    \
    ((uint16)((uint32)(_a32) >> 2))

/* 16位转32位内存地址 */
#define __addr32(_a16)  \
    \
    (((uint32)(_a16) << 2))

#define MEM_MANAGER_DEBUG

typedef struct 
{
	uint32 addr;
	uint32 size;
	uint8 *mem_map;
	uint8 *mem_map2;
	uint8 block_size_mask;
	uint8 data_feature;
	uint8 insert_mode;
	uint8 mem_global;
}mem_pool_param_t;

//保证该数据结构是16字节
typedef struct
{
	uint16 addr; 				//起始地址
	uint16 next;	            //指向下一个内存池描述符
	uint8 *mem_map;
	uint8 *mem_map2;	
	uint32 total_index:7; 		//分配的index总数,以256为一个块，最大映射256k，以16为一个块，最大映射16k
	uint32 blk_size:4;			//分配块大小
	uint32 data_feature:6; 		//是否可分配代码段, 数据段，蓝牙数据段,bank数据段, ap数据段等
	uint32 mem_last:11;			//空余index数目,2^7*8=1024 
	uint32 pool_lock : 1;       //内存池是否锁定，不可使用
	uint32 mem_global : 1;      //内存池是否是全局内存池
	uint32 mpu_enable : 1;
	uint32 reserved:1;
}mem_pool_info_t;

//保证该结构是16字节对齐
typedef struct bank_info_stu
{
    uint16 vaddr;           //bank虚拟地址
    uint16 size;            //bank大小
    uint16 pmaddr;          //bank缓存地址
    uint16 run_addr;        //bank实际运行地址
    uint16 switch_count;    //切换次数统计
    uint16 pool_addr;       //指向mem_pool_info_t结构体
    uint16 next;            //下一个bank_info    
    uint16 data_tag;        //bank数据标记，确认bank数据是否被破坏
}bank_info_t;


typedef struct
{
    uint16 use_bt_data;        //bt_data使用量
    uint16 free_bt_data;       //bt数据空闲量
    uint16 free_bt_data_peak;  //bt数据剩余峰值
    uint16 bt_data_total;        
    uint16 use_global_data;    //小数据使用量 MEM_DATA, MEM_BANK_INFO
    uint16 global_data_total;  //小数据总量，不包括BT DATA
#ifdef MEM_MANAGER_DEBUG    
    mem_pool_info_t **mem_pool;
    bank_info_t **bank_info;
#endif    
}mem_use_info_t;

typedef enum
{
    //系统使用，用于缓存代码 
    MEM_TEXT = (1 << 0), 
    //BANK DATA,临时使用,该数据一般用完就会释放
    MEM_BANK_DATA = (1 << 1),
    //普通要使用的全局数据
    MEM_DATA = (1 << 2),  
    //蓝牙申请的数据
    MEM_BT_DATA = (1 << 3), 
    //系统使用，当数据空间不足时，可释放这部分空间
    MEM_BANK_INFO = (1 << 4), 
    //普通要使用的大数据，超过512字节，最多不超过全局cache大小
    MEM_LDATA = (1 << 5),     
}mem_type_e;

typedef struct
{
    //是否使用局部内存池
    uint8 use_temp_pool:1;
    //是否使用send buffer
    uint8 use_send_buffer:1;
    //是否是tws linein场景
    uint8 mem_scene:6;
}mem_use_scene_info_t;

typedef enum
{
    MEM_USE_NORMAL = 0,
    MEM_USE_TWS_LINEIN,
    MEM_USE_USOUND,
    MEM_USE_MUSICPLAY,
}mem_use_info_e;


#define MEM_IN_BANK 0

#define INSERT_MEM_POOL_TAIL  0

#define INSERT_MEM_POOL_HEAD  1

#define CLR_MEM_LOCK          0

#define SET_MEM_LOCK          1

#define MAX_MEM_LOCK_ITEM     1


typedef struct
{
    uint16 mem_addr;
    uint16 mem_size;   
}mem_lock_info_t;

typedef enum
{
    //清除mem pool所有数据
    MEM_POOL_OP_CLEAR = 0,
    //销毁mem pool
    MEM_POOL_OP_DESTROY = 1,
    //强制销毁mem pool,即使里面仍然有数据
    MEM_POOL_OP_FORCE_DESTROY = 2,
    //只先释放内存池，其内部数据稍后释放
    MEM_POOL_OP_ARRANGE
}mem_pool_ops_e;


extern mem_pool_info_t *g_p_mem_pool_info;

extern uint32 mem_pool_create(mem_pool_param_t *mem_pool_param) __FAR__;

extern uint32 mem_pool_destroy(uint32 addr, uint32 size, uint32 mode);
#endif
