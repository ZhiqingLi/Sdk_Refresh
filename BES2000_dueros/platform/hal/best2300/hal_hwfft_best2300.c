//#include "cmsis_os.h"
#include "plat_addr_map.h"
#include "reg_hwfir_best2300.h"
#include "hal_hwfir.h"
#include "hal_codec.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "hal_dma.h"
#include "analog.h"
#include "tgt_hardware.h"
#include "hal_hwfft.h"
#include "string.h"
#include "hal_timer.h"

#define FFT_BASE 0x40400000

#define FFT_CONFIG_OFFSET     0x0000
#define FFT_SCALE0_OFFSET     0x0004
#define FFT_SCALE1_OFFSET     0x0008
#define FFT_SCALE2_OFFSET     0x000c
#define FFT_SCALE3_OFFSET     0x0010

#define FFT_CMD_MEM_OFFSET    0x4000
#define FFT_DATA_MEM0_OFFSET  0x8000
#define FFT_DATA_MEM1_OFFSET  0x8800

#define READ_REG(b,a) *(volatile uint32_t *)(b+a)
#define WRITE_REG(v,b,a) *(volatile uint32_t *)(b+a) = v

#if defined(HAVE_EQUIVALENT_MODE)
#define EQVL_MODE_BUFF_SIZE (514)
short hwfft_eqvl_mode_buff[EQVL_MODE_BUFF_SIZE] = {0};
#endif

struct HAL_DMA_CH_CFG_T fft_dma_cfg;

static struct HAL_DMA_DESC_T fft_in_dma_desc[64];
static struct HAL_DMA_DESC_T fft_out_dma_desc[64];

#if defined(HWFFT_USE_BUSY_WAIT)
volatile int hwfft_dma_lock = 0;
#else
//osSemaphoreId current_sem_lock_id;
#endif

volatile int hwfft_lock = 0;

static uint8_t in_ch, ou_ch;


int hwfft_sleep_ticks = 0;

bool hwfft_is_enable(void)
{
    return true;
    // return false;
}
static inline int hwfft_get_order(enum hwfft_order_t hwfft_order)
{
    int order = 0;
    if(hwfft_order == HWFFT_ORDER_64SAMP)
    {
        order = 0;
    }
    else if(hwfft_order == HWFFT_ORDER_128SAMP)
    {
        order = 1;
    }
    else if(hwfft_order == HWFFT_ORDER_256SAMP)
    {
        order = 2;
    }
    else if(hwfft_order == HWFFT_ORDER_512SAMP)
    {
        order = 3;
    }
    else
    {
        ASSERT(0, "[%s] order = %d", __func__, hwfft_order);
    }

    return order;
}

int hwfft_create(struct hwfft_t *fft_inst, int mode, int order,int sym_fft)
{
    if (fft_inst == NULL)
        return -1;

    hal_cmu_clock_enable(HAL_CMU_MOD_H_FFT);
    hal_cmu_reset_clear(HAL_CMU_MOD_H_FFT);

    memset(fft_inst, 0, sizeof(struct hwfft_t));
    //fft_inst->_osSemaphoreDef.semaphore = fft_inst->_semaphore_data;
    //fft_inst->_osSemaphoreId = osSemaphoreCreate(&(fft_inst->_osSemaphoreDef), 0);
    //TRACE("%s:%d osSemaphoreId %x\n", __func__, __LINE__, fft_inst->_osSemaphoreId);

    fft_inst->mode  = mode;
    fft_inst->order = order;
    fft_inst->sym_fft = sym_fft;
    return 0;
}

int hwfft_destroy(struct hwfft_t *fft_inst)
{
    if (fft_inst == NULL)
        return -1;

#if !defined(HWFFT_USE_BUSY_WAIT)
    //osSemaphoreDelete(fft_inst->_osSemaphoreId);
#endif
    return 0;
}

static void hwfft_dout_handler(uint8_t ch,uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
#if defined(HWFFT_USE_BUSY_WAIT)
    hwfft_dma_lock = 0;
    //TRACE("%s: hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
    //TRACE("leiyuan:  0x%x, 0x%x , 0x%x,  0x%x, 0x%x,  0x%x,  0x%x,  0x%x", READ_REG(FFT_BASE,FFT_CONFIG_OFFSET), READ_REG(FFT_BASE,FFT_SCALE0_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_SCALE1_OFFSET), READ_REG(FFT_BASE,FFT_SCALE2_OFFSET), READ_REG(FFT_BASE,FFT_SCALE3_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM1_OFFSET));	
    //TRACE("%s: hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*1));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*2), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*3));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*62), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*63));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*64), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*65));	
#else
    //osSemaphoreRelease(current_sem_lock_id);
#endif
}


static void hwfft_multi_dout_handler(uint8_t ch,uint32_t remains, uint32_t error, struct HAL_DMA_DESC_T *lli)
{
#if defined(HWFFT_USE_BUSY_WAIT)
    uint32_t val;

    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffe; //0 fft_enable
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);

    hal_dma_stop(in_ch);
    hal_dma_stop(ou_ch);

    //TRACE("%s:%d", __func__, __LINE__);
    hal_dma_free_chan(in_ch);
    hal_dma_free_chan(ou_ch);

    hwfft_lock = 0;
    hwfft_dma_lock = 0;
    //TRACE("%s: hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
    //TRACE("leiyuan:  0x%x, 0x%x , 0x%x,  0x%x, 0x%x,  0x%x,  0x%x,  0x%x", READ_REG(FFT_BASE,FFT_CONFIG_OFFSET), READ_REG(FFT_BASE,FFT_SCALE0_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_SCALE1_OFFSET), READ_REG(FFT_BASE,FFT_SCALE2_OFFSET), READ_REG(FFT_BASE,FFT_SCALE3_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM1_OFFSET));	
    //TRACE("%s: hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*1));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*2), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*3));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*62), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*63));
    //TRACE("leiyuan:  0x%x, 0x%x", READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET+ 4*64), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*65));	
#else
    //osSemaphoreRelease(current_sem_lock_id);
#endif
}


int hwfft_ifft_input_add_samples(struct hwfft_t *fft_inst, short* in, short*out)
{
    int len = 0, i = 0, counter;

#if 0
    switch(fft_inst->order) {
        case HWFFT_ORDER_64SAMP:len = 66;break;
        case HWFFT_ORDER_128SAMP:len = 130;break;
        case HWFFT_ORDER_256SAMP:len = 258;break;
        case HWFFT_ORDER_512SAMP:len = 514;break;
    }
#else
    len = fft_inst->order + 2;
#endif

    counter = 0;
    for (i = 0; i < len; ++i) {
        // second or last sample is 0, remove
        if ((i == 1) || (i == (len-1))) {
            out[i] = 0;
        }
        else {
            out[i] = in[counter];
            ++counter;
        }
    }

    return 0;
}

int hwfft_fft_output_sub_samples(struct hwfft_t *fft_inst, short * in, short *out)
{
    int len = 0, i = 0, counter;
    
#if 0
    switch(fft_inst->order) {
        case HWFFT_ORDER_64SAMP:len = 66;break;
        case HWFFT_ORDER_128SAMP:len = 130;break;
        case HWFFT_ORDER_256SAMP:len = 258;break;
        case HWFFT_ORDER_512SAMP:len = 514;break;
    }
#else
    len = fft_inst->order + 2;
#endif

    counter = 0;
    for (i = 0; i < len; ++i) {
        // second or last sample is 0, remove
        if ((i == 1) || (i == (len-1)))
            continue;

        out[counter] = in[i];
        ++counter;
    }

    return 0;
}

int hwfft_fft_process(struct hwfft_t *fft_inst, short *in, short *out)
{
    int t = 0;
    uint32_t val;
    int fft_point_num;
    int fft_mode;
    int fft_order = hwfft_get_order(fft_inst->order);
    //uint8_t in_ch, ou_ch;

    //TRACE("%s",__FUNCTION__);

    struct HAL_DMA_2D_CFG_T in_src, in_dst;
    struct HAL_DMA_2D_CFG_T ou_src, ou_dst;

    if((((fft_inst->mode & HWFFT_MODE_EQVL_FFT) && (fft_inst->sym_fft == 0)))||
        (((fft_inst->mode & HWFFT_MODE_EQVL_IFFT) && (fft_inst->sym_fft == 0)))){
        TRACE("mode error!");
        while(1);
    }

    if (fft_inst->mode & (HWFFT_MODE_FFT|HWFFT_MODE_EQVL_FFT))
        fft_mode = 0; // fft
    else
        fft_mode = 1; // ifft
    //TRACE("%s: mode %d, order %d, sym_fft %d", __func__, fft_inst->mode, fft_inst->order, fft_inst->sym_fft);

    fft_point_num = 1 << (fft_order + 6);

    memset(&in_src, 0, sizeof(in_src));  
    memset(&in_dst, 0, sizeof(in_dst));  

    //setup dst 2D dma for fft tx
    if (fft_mode == 0) {
        in_dst.xmodify = 1;
        in_dst.xcount = fft_point_num;    
        in_dst.ymodify = 0;
        in_dst.ycount = 1;
    }
    //setup src 2D dma for ifft rx
    else if(fft_mode==1)
    {
        in_src.xmodify = 1;
        in_src.xcount = fft_point_num;    
        in_src.ymodify = 0;
        in_src.ycount = 1;
    }

    //TRACE("%s:%d", __func__, __LINE__);
    //fft din
    if(fft_inst->sym_fft == 1){
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_TX, HAL_DMA_HIGH_PRIO);
        in_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = HAL_AUDMA_FFT_TX; // useless
        if(fft_mode==0) //fft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_1;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_HALFWORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
        }
        else //ifft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        }
        fft_dma_cfg.handler = 0;
        fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_16;
        fft_dma_cfg.src_periph = 0; // useless
        if(fft_mode==0) //fft
            fft_dma_cfg.src_tsize = fft_point_num;
        else{ //ifft
            if((fft_inst->sym_fft == 1) )
                fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
            else
                fft_dma_cfg.src_tsize = fft_point_num;
    
        }
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_M2P_DI_DMA;
  
    }else{
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_TX, HAL_DMA_HIGH_PRIO);
        in_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = HAL_AUDMA_FFT_TX; // useless
        if(fft_mode==0) //fft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        }
        else //ifft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        }
        fft_dma_cfg.handler = 0;
        fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
        fft_dma_cfg.src_periph = 0; // useless
        if(fft_mode==0) //fft
            fft_dma_cfg.src_tsize = fft_point_num;
        else{ //ifft
            if((fft_inst->sym_fft == 1) )
                fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
            else
                fft_dma_cfg.src_tsize = fft_point_num;
    
        }
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_M2P_DI_DMA;
    }

    
        //TRACE("%s:%d", __func__, __LINE__);
        //512p
        if(fft_mode==0) //fft
            fft_dma_cfg.src = (int)in;
        else {//IFFT
    #if defined(HAVE_EQUIVALENT_MODE)
            if ((fft_inst->mode & HWFFT_MODE_EQVL_IFFT) && (fft_inst->sym_fft == 1)) {
                hwfft_ifft_input_add_samples(fft_inst, in, hwfft_eqvl_mode_buff);
                fft_dma_cfg.src = (int)hwfft_eqvl_mode_buff;
            }
            else
    #endif
            {
                fft_dma_cfg.src = (int)in;
            }
        }


    hal_audma_init_desc(&fft_in_dma_desc[0], &fft_dma_cfg, 0, 0);
    fft_in_dma_desc[0].dst = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
    //fft_in_dma_desc[0].ctrl = fft_in_dma_desc[0].ctrl | (1<<27); //addr incr

    if (fft_mode == 0) {
        hal_dma_sg_2d_start(&fft_in_dma_desc[0], &fft_dma_cfg, NULL, &in_dst);
    }
    else if (fft_mode == 1) {
        hal_dma_sg_2d_start(&fft_in_dma_desc[0], &fft_dma_cfg, NULL, NULL);
    }

    //TRACE("%d", fft_dma_cfg.src_tsize);
    //fft dout
    memset(&ou_src, 0, sizeof(ou_src));
    memset(&ou_dst, 0, sizeof(ou_dst));

    //setup src 2D dma for ifft rx
    if(fft_mode==1)
    {
        ou_src.xmodify = 1;
        ou_src.xcount = fft_point_num;
        ou_src.ymodify = 0;
        ou_src.ycount = 1;
    }

    if(fft_inst->sym_fft == 1){    	
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_RX, HAL_DMA_HIGH_PRIO);
        ou_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = 0; // useless
        fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
        fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
        fft_dma_cfg.handler = hwfft_dout_handler;
        fft_dma_cfg.src_periph = HAL_AUDMA_FFT_RX; // useless
        if(fft_mode==1) //ifft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_1;
            fft_dma_cfg.src_tsize = fft_point_num;
        }
        else //fft
        {
            if (fft_inst->sym_fft == 1) {
                fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_16;
                fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
            }else{
                fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_16;
                fft_dma_cfg.src_tsize = fft_point_num;
            }
        }
    
        //TRACE("%d", fft_dma_cfg.src_tsize);
        if(fft_mode==1) //ifft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
        else //fft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
     
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_P2M_SI_DMA;
    }else{
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_RX, HAL_DMA_HIGH_PRIO);
        ou_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = 0; // useless
        fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
        fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
        fft_dma_cfg.handler = hwfft_dout_handler;
        fft_dma_cfg.src_periph = HAL_AUDMA_FFT_RX; // useless
        if(fft_mode==1) //ifft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.src_tsize = fft_point_num;
        }
        else //fft
        {
            if (fft_inst->sym_fft == 1) {
                fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_16;
                fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
            }else{
                fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
                fft_dma_cfg.src_tsize = fft_point_num;
            }
        }
    
        //TRACE("%d", fft_dma_cfg.src_tsize);
        if(fft_mode==1) //ifft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        else //fft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
     
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_P2M_SI_DMA;
    }

	
    #if defined(HAVE_EQUIVALENT_MODE)
        if ((fft_inst->mode & HWFFT_MODE_EQVL_FFT) && (fft_inst->sym_fft == 1)) {
            fft_dma_cfg.dst = (int)hwfft_eqvl_mode_buff;
        }
        else
    #endif
        {
            fft_dma_cfg.dst = (int)out;
        }

    hal_audma_init_desc(&fft_out_dma_desc[0], &fft_dma_cfg, 0, 1);

    if((fft_order==3)||(fft_order==1)) //512=2^9, 128=2^7, => mem1
        fft_out_dma_desc[0].src = (FFT_BASE + FFT_DATA_MEM1_OFFSET); //addr
    else //256=2^8, 64=2^6, => mem0
        fft_out_dma_desc[0].src = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
#if defined(HWFFT_USE_BUSY_WAIT)
    hwfft_dma_lock = 1;
#endif
    //  TRACE("%s hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
  
    if (fft_mode == 0) {
        hal_dma_sg_2d_start(&fft_out_dma_desc[0], &fft_dma_cfg, NULL, NULL);
    }
    else if (fft_mode == 1) {
        hal_dma_sg_2d_start(&fft_out_dma_desc[0], &fft_dma_cfg, &ou_src, NULL);
    }


    //TRACE("%s:%d", __func__, __LINE__);
    val = READ_REG(FFT_BASE,FFT_SCALE0_OFFSET);
    val &= 0xfffc0000; //17:0 fft scale0
    // fft
    if (fft_mode == 0) {
        val |= 0x15555;
    }
    // ifft
    else {
        if(fft_order == 3)
            val |= 0x00000;
        else
        val |= 0x10000;
    }
    val &= 0xffcfffff; //21:20 arith scale0
    val |= (0x1<<20);
    WRITE_REG(val,FFT_BASE,FFT_SCALE0_OFFSET);

    val = READ_REG(FFT_BASE,FFT_SCALE1_OFFSET);
    val &= 0xfffc0000; //17:0 fft scale1
    val |= 0x15555;
    val &= 0xffcfffff; //21:20 arith scale1
    val |= (0x1<<20);
    WRITE_REG(val,FFT_BASE,FFT_SCALE1_OFFSET);

    val = READ_REG(FFT_BASE,FFT_SCALE2_OFFSET);
    val &= 0xfffc0000; //17:0 fft scale2
    val |= 0x15555;
    val &= 0xffcfffff; //21:20 arith scale2
    val |= (0x1<<20);
    WRITE_REG(val,FFT_BASE,FFT_SCALE2_OFFSET);

    val = READ_REG(FFT_BASE,FFT_SCALE3_OFFSET);
    val &= 0xfffc0000; //17:0 fft scale3
    val |= 0x15555;
    val &= 0xffcfffff; //21:20 arith scale3
    val |= (0x1<<20);
    WRITE_REG(val,FFT_BASE,FFT_SCALE3_OFFSET);

    //val = READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET);
    val = 0;
    val &= 0xfffffffe; //0 bypass_din
    val |= 0x0;
    val &= 0xfffffffd; //1 bypass_dout
    val |= (0x0<<1);
    val &= 0xfffffff3; //3:2 op_mode, 0 for fft/ifft 
    val |= (0x0<<2);
    val &= 0xffffffcf; //5:4 fft_order 
    val |= (fft_order<<4);
    val &= 0xffffff3f; //7:6 scale_sel 
    val |= (0x0);
    val &= 0xfffffeff; //8 result_16_20n 
    val |= (0x1<<8);
    val &= 0xfffffdff; //9 mem_sel 
    val |= (0x0<<9);
    val &= 0xfffffbff; //10 sym_fft 
    //val |= (1<<10);
    val |= ((fft_inst->sym_fft)<<10);
    val &= 0xfffff7ff; //11 ifft/fft 
    val |= (fft_mode<<11);
    WRITE_REG(val,FFT_BASE,FFT_CMD_MEM_OFFSET);
    //TRACE("%s  val %x, cmd %x", __func__, val, READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET));

    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffe; //0 fft_enable
    val |= 0x1;
    val &= 0xfffffff3; //3:2 dmactrl rx tx 
    val |= (0x3<<2);
    val &= 0xffffc00f; //13:4 burst len 
    val |= (0x1<<4);
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);


    //TRACE("leiyuan1:  0x%x, 0x%x , 0x%x,  0x%x, 0x%x,  0x%x,  0x%x,  0x%x", READ_REG(FFT_BASE,FFT_CONFIG_OFFSET), READ_REG(FFT_BASE,FFT_SCALE0_OFFSET),
    //		 READ_REG(FFT_BASE,FFT_SCALE1_OFFSET), READ_REG(FFT_BASE,FFT_SCALE2_OFFSET), READ_REG(FFT_BASE,FFT_SCALE3_OFFSET),
    //		 READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM1_OFFSET));

    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffd; //1 fft_start
    val |= (0x1<<1);
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);

    t = hal_sys_timer_get();
    //TRACE("11111111111111111"); 
#if defined(HWFFT_USE_BUSY_WAIT)
    while (hwfft_dma_lock);
#else
    //current_sem_lock_id = fft_inst->_osSemaphoreId;
    //osSemaphoreWait(current_sem_lock_id, osWaitForever);
#endif
    hwfft_sleep_ticks = hal_sys_timer_get()-t;

#if defined(HAVE_EQUIVALENT_MODE)
    if (fft_inst->mode & (HWFFT_MODE_EQVL_FFT))
    {
        hwfft_fft_output_sub_samples(fft_inst, hwfft_eqvl_mode_buff, out);
    }
#endif



    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffe; //0 fft_enable
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);

    hal_dma_stop(in_ch);
    hal_dma_stop(ou_ch);

    //TRACE("%s:%d", __func__, __LINE__);
    hal_dma_free_chan(in_ch);
    hal_dma_free_chan(ou_ch);

    return 0;
}

// short hwfft_eqvl_mode_buff_tmp[(64+2)*64] = {0};
 //struct HAL_DMA_DESC_T fft_in_dma_desc[4];

//mode:0 一直等到fft完成函数才返回
//mode:1 dma配置完成，fft开始函数即可返回，cpu可以在硬件fft 时做其他的运算
int hwfft_multifft_process(struct hwfft_t *fft_inst, short *in, short *out, unsigned short num, unsigned char mode)
{
    int t = 0;
    uint32_t val;
    int fft_mode;
    int fft_order = hwfft_get_order(fft_inst->order);
    int fft_point_num = 1 << (fft_order + 6);
    unsigned char i = 0;

    //if(mode == 1)
    //{
        while(hwfft_lock)
        {
            // TRACE("%s, wait!",__FUNCTION__);
        }
    //}

    //TRACE("%s, %d",__FUNCTION__, fft_point_num);

    struct HAL_DMA_2D_CFG_T in_src, in_dst;
    struct HAL_DMA_2D_CFG_T ou_src, ou_dst;

    //多次连续fft为了节省时间不支持HWFFT_MODE_EQVL_FFT|HWFFT_MODE_EQVL_IFFT模式，此模式有一次memory copy
    if((fft_inst->mode & (HWFFT_MODE_EQVL_FFT|HWFFT_MODE_EQVL_IFFT)))	{
        TRACE("mode err!%d",fft_inst->mode);
        while(1);
    }

    if (fft_inst->mode & (HWFFT_MODE_FFT|HWFFT_MODE_EQVL_FFT))
        fft_mode = 0; // fft
    else
        fft_mode = 1; // ifft
    //TRACE("%s: mode %d, order %d, sym_fft %d", __func__, fft_inst->mode, fft_inst->order, fft_inst->sym_fft);

    fft_point_num = 1 << (fft_order + 6);

    memset(&in_src, 0, sizeof(in_src));  
    memset(&in_dst, 0, sizeof(in_dst));  

    //setup dst 2D dma for fft tx
    if (fft_mode == 0) {
        in_dst.xmodify = 1;
        in_dst.xcount = fft_point_num;
        in_dst.ymodify = 0;
        in_dst.ycount = 1;
    }
    //setup src 2D dma for ifft rx
    else if(fft_mode==1)
    {
        in_src.xmodify = 1;
        in_src.xcount = fft_point_num;    
        in_src.ymodify = 0;
        in_src.ycount = 1;
    }

    //TRACE("%s:%d", __func__, __LINE__);
    //fft din
    if(fft_inst->sym_fft == 1){
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_TX, HAL_DMA_HIGH_PRIO);
        in_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = HAL_AUDMA_FFT_TX; // useless
        if(fft_mode==0) //fft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_1;//
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_HALFWORD;//
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;//
        }
        else //ifft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_16;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        }
        fft_dma_cfg.handler = 0;
        fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;//
        fft_dma_cfg.src_periph = 0; // useless
        if(fft_mode==0) //fft
            fft_dma_cfg.src_tsize = fft_point_num;
        else //ifft
            fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_M2P_DI_DMA;
    }else{
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_TX, HAL_DMA_HIGH_PRIO);
        in_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = HAL_AUDMA_FFT_TX; // useless
        if(fft_mode==0) //fft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;//
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;//
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;//
        }
        else //ifft
        {
            fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        }
        fft_dma_cfg.handler = 0;
        fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;//
        fft_dma_cfg.src_periph = 0; // useless
        if(fft_mode==0) //fft
            fft_dma_cfg.src_tsize = fft_point_num;
        else //ifft
            fft_dma_cfg.src_tsize = fft_point_num;
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_M2P_DI_DMA;
    }
    
    //TRACE("leiyuan1:  0x%x", fft_dma_cfg.src_width);

#if 0
    //TRACE("%s:%d", __func__, __LINE__);
    //512p
    if(fft_mode==0) //fft
        fft_dma_cfg.src = (int)in;
    else {
#if defined(HAVE_EQUIVALENT_MODE)
        if (fft_inst->mode & HWFFT_MODE_EQVL_IFFT) {
            hwfft_ifft_input_add_samples(fft_inst, in, hwfft_eqvl_mode_buff);
            fft_dma_cfg.src = (int)hwfft_eqvl_mode_buff;
        }
        else
#endif
        {
            fft_dma_cfg.src = (int)in;
        }
    }
    
    hal_audma_init_desc(&fft_in_dma_desc[0], &fft_dma_cfg, 0, 0);
    fft_in_dma_desc[0].dst = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
    //fft_in_dma_desc[0].ctrl = fft_in_dma_desc[0].ctrl | (1<<27); //addr incr
#else


    //TRACE("%s:%d", __func__, __LINE__);
    //512p
    for(i = 0;i < num; i++){
        if(fft_mode == 0) //fft
        {
            if(fft_inst->sym_fft == 1)
                fft_dma_cfg.src = (int)(in+fft_point_num*i);
            else
                fft_dma_cfg.src = (int)(in+fft_point_num*2*i);
        }
        else {//IFFT
            if (fft_inst->sym_fft == 1) {
                fft_dma_cfg.src = (int)(in + (fft_point_num+2)*i);
            }
            else
            {
                fft_dma_cfg.src = (int)(in+fft_point_num*2*i);
            }
        }
        if(i != (num-1)){        
            hal_audma_init_desc(&fft_in_dma_desc[i], &fft_dma_cfg, &fft_in_dma_desc[i+1], 0);
            fft_in_dma_desc[i].dst = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
        }else{
            hal_audma_init_desc(&fft_in_dma_desc[i], &fft_dma_cfg, 0, 1);
            fft_in_dma_desc[i].dst = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
        }		
        //hal_audma_init_desc(&fft_in_dma_desc[0], &fft_dma_cfg, 0, 0);
        //fft_in_dma_desc[0].dst = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
        //fft_in_dma_desc[0].ctrl = fft_in_dma_desc[0].ctrl | (1<<27); //addr incr
        //TRACE("%s: fft_dma_cfg.src 0x%x", __func__, fft_dma_cfg.src);
    }
#endif

    if(fft_inst->sym_fft == 1){
        if (fft_mode == 0) {
            hal_dma_sg_2d_start(&fft_in_dma_desc[0], &fft_dma_cfg, NULL, &in_dst);
        }
        else if (fft_mode == 1) {
            hal_dma_sg_2d_start(&fft_in_dma_desc[0], &fft_dma_cfg, NULL, NULL);
    }
    }else{
            hal_dma_sg_2d_start(&fft_in_dma_desc[0], &fft_dma_cfg, NULL, NULL);
	}
    //TRACE("%s:%d", __func__, __LINE__);
    //fft dout
    memset(&ou_src, 0, sizeof(ou_src));
    memset(&ou_dst, 0, sizeof(ou_dst));

    //setup src 2D dma for ifft rx
    if(fft_mode==1)
    {
        ou_src.xmodify = 1;
        ou_src.xcount = fft_point_num;
        ou_src.ymodify = 0;
        ou_src.ycount = 1;
    }

    if(fft_inst->sym_fft == 1){
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_RX, HAL_DMA_HIGH_PRIO);
        ou_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = 0; // useless
        fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
        fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
        fft_dma_cfg.handler = hwfft_multi_dout_handler;
        fft_dma_cfg.src_periph = HAL_AUDMA_FFT_RX; // useless
        if(fft_mode==1) //ifft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_1;
            fft_dma_cfg.src_tsize = fft_point_num;
        }
        else //fft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.src_tsize = (fft_point_num>>1) + 1;
        }
    
        //TRACE("%s:%d", __func__, __LINE__);
        if(fft_mode==1) //ifft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_HALFWORD;
        else //fft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
     
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_P2M_SI_DMA;
    }else{
        memset(&fft_dma_cfg, 0, sizeof(fft_dma_cfg));
        fft_dma_cfg.ch = hal_audma_get_chan(HAL_AUDMA_FFT_RX, HAL_DMA_HIGH_PRIO);
        ou_ch = fft_dma_cfg.ch;
        fft_dma_cfg.dst_periph = 0; // useless
        fft_dma_cfg.dst_bsize = HAL_DMA_BSIZE_4;
        fft_dma_cfg.dst_width = HAL_DMA_WIDTH_WORD;
        fft_dma_cfg.handler = hwfft_multi_dout_handler;
        fft_dma_cfg.src_periph = HAL_AUDMA_FFT_RX; // useless
        if(fft_mode==1) //ifft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.src_tsize = fft_point_num;
        }
        else //fft
        {
            fft_dma_cfg.src_bsize = HAL_DMA_BSIZE_4;
            fft_dma_cfg.src_tsize = fft_point_num;
        }
    
        //TRACE("%s:%d", __func__, __LINE__);
        if(fft_mode==1) //ifft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
        else //fft
            fft_dma_cfg.src_width = HAL_DMA_WIDTH_WORD;
     
        fft_dma_cfg.try_burst = 1;
        fft_dma_cfg.type = HAL_DMA_FLOW_P2M_SI_DMA;
    }
	
#if 0
#if defined(HAVE_EQUIVALENT_MODE)
    if (fft_inst->mode & HWFFT_MODE_EQVL_FFT) {
        fft_dma_cfg.dst = (int)hwfft_eqvl_mode_buff;
    }
    else
#endif
    {
        fft_dma_cfg.dst = (int)out;
    }

    hal_audma_init_desc(&fft_out_dma_desc[0], &fft_dma_cfg, 0, 1);


    if((fft_order==3)||(fft_order==1)) //512=2^9, 128=2^7, => mem1
        fft_out_dma_desc[0].src = (FFT_BASE + FFT_DATA_MEM1_OFFSET); //addr
    else //256=2^8, 64=2^6, => mem0
        fft_out_dma_desc[0].src = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
#if defined(HWFFT_USE_BUSY_WAIT)
    hwfft_dma_lock = 1;
#endif
    //  TRACE("%s hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
#else

    for(i = 0; i < num; i++){
        if (fft_mode == 0) {//FFT
            if(fft_inst->sym_fft == 1)
                fft_dma_cfg.dst = (int)(out + (fft_point_num+2)*i);
            else
                fft_dma_cfg.dst = (int)(out + fft_point_num*2*i);
        }
        else//IFFT
        {
            if(fft_inst->sym_fft == 1)
                fft_dma_cfg.dst = (int)(out + fft_point_num*i);
            else
                fft_dma_cfg.dst = (int)(out + fft_point_num*2*i);
        }

        if(i != (num-1))            
			hal_audma_init_desc(&fft_out_dma_desc[i], &fft_dma_cfg, &fft_out_dma_desc[i+1], 0);        
		else            
			hal_audma_init_desc(&fft_out_dma_desc[i], &fft_dma_cfg, 0, 1);

        if((fft_order==3)||(fft_order==1)) //512=2^9, 128=2^7, => mem1
            fft_out_dma_desc[i].src = (FFT_BASE + FFT_DATA_MEM1_OFFSET); //addr
        else //256=2^8, 64=2^6, => mem0
            fft_out_dma_desc[i].src = (FFT_BASE + FFT_DATA_MEM0_OFFSET); //addr
    	}
#if defined(HWFFT_USE_BUSY_WAIT)
    hwfft_dma_lock = 1;
#endif
    //TRACE("%s hwfft_dma_lock:%d", __func__,hwfft_dma_lock);
#endif

    if(fft_inst->sym_fft == 1){
        if (fft_mode == 0) {
            hal_dma_sg_2d_start(&fft_out_dma_desc[0], &fft_dma_cfg, NULL, NULL);
        }
        else if (fft_mode == 1) {
            hal_dma_sg_2d_start(&fft_out_dma_desc[0], &fft_dma_cfg, &ou_src, NULL);
        }
    }else{
            hal_dma_sg_2d_start(&fft_out_dma_desc[0], &fft_dma_cfg, NULL, NULL);
    }

    if(fft_inst->sym_fft == 1){//实数输入
        //TRACE("%s:%d", __func__, __LINE__);
        val = READ_REG(FFT_BASE,FFT_SCALE0_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale0
        // fft
        if (fft_mode == 0) {
            val |= 0x150a5;
        }
        // ifft
        else {
            val |= 0x10000;
        }
        val &= 0xffcfffff; //21:20 arith scale0
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE0_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE1_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale1
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale1
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE1_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE2_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale2
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale2
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE2_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE3_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale3
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale3
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE3_OFFSET);
    }else{//复数输入
    //TRACE("%s:%d", __func__, __LINE__);
        val = READ_REG(FFT_BASE,FFT_SCALE0_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale0
        // fft
        if (fft_mode == 0) {
            val |= 0x1500f;
        }
        // ifft
        else {
            val |= 0x15255;
        }
        val &= 0xffcfffff; //21:20 arith scale0
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE0_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE1_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale1
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale1
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE1_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE2_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale2
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale2
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE2_OFFSET);
    
        val = READ_REG(FFT_BASE,FFT_SCALE3_OFFSET);
        val &= 0xfffc0000; //17:0 fft scale3
        val |= 0x15555;
        val &= 0xffcfffff; //21:20 arith scale3
        val |= (0x1<<20);
        WRITE_REG(val,FFT_BASE,FFT_SCALE3_OFFSET);
    }


#if 0
    //val = READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET);
    val = 0;
    val &= 0xfffffffe; //0 bypass_din
    val |= 0x0;
    val &= 0xfffffffd; //1 bypass_dout
    val |= (0x0<<1);
    val &= 0xfffffff3; //3:2 op_mode, 0 for fft/ifft 
    val |= (0x0<<2);
    val &= 0xffffffcf; //5:4 fft_order 
    val |= (fft_order<<4);
    val &= 0xffffff3f; //7:6 scale_sel 
    val |= (0x0);
    val &= 0xfffffeff; //8 result_16_20n 
    val |= (0x1<<8);
    val &= 0xfffffdff; //9 mem_sel 
    val |= (0x0<<9);
    val &= 0xfffffbff; //10 sym_fft 
    val |= (1<<10);
    val &= 0xfffff7ff; //11 ifft/fft 
    val |= (fft_mode<<11);
    WRITE_REG(val,FFT_BASE,FFT_CMD_MEM_OFFSET);
    //TRACE("%s  val %x, cmd %x", __func__, val, READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET));
#else
    for(i = 0; i < num; i++){
    //val = READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET);
        val = 0;
        val &= 0xfffffffe; //0 bypass_din
        val |= 0x0;
        val &= 0xfffffffd; //1 bypass_dout
        val |= (0x0<<1);
        val &= 0xfffffff3; //3:2 op_mode, 0 for fft/ifft 
        val |= (0x0<<2);
        val &= 0xffffffcf; //5:4 fft_order 
        val |= (fft_order<<4);
        val &= 0xffffff3f; //7:6 scale_sel 
        val |= (0x0);
        val &= 0xfffffeff; //8 result_16_20n 
        val |= (0x1<<8);
        val &= 0xfffffdff; //9 mem_sel 
        val |= (0x0<<9);
        val &= 0xfffffbff; //10 sym_fft 
        //val |= (1<<10);
        val |= ((fft_inst->sym_fft)<<10);       
        val &= 0xfffff7ff; //11 ifft/fft 
        val |= (fft_mode<<11);
        WRITE_REG(val,FFT_BASE,FFT_CMD_MEM_OFFSET + (i<<2));
    }
#endif



    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffe; //0 fft_enable
    val |= 0x1;
    val &= 0xfffffff3; //3:2 dmactrl rx tx 
    val |= (0x3<<2);
    val &= 0xffffc00f; //13:4 burst len 
    //val |= (0x1<<4);
    val |= (num<<4);
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);


    memset((void*)(FFT_BASE+FFT_DATA_MEM0_OFFSET),0x00,256);
    //memset((void*)(FFT_BASE+FFT_DATA_MEM1_OFFSET),0x0,512);

    //TRACE("leiyuan1:  0x%x, 0x%x , 0x%x,  0x%x, 0x%x,  0x%x,  0x%x,  0x%x", READ_REG(FFT_BASE,FFT_CONFIG_OFFSET), READ_REG(FFT_BASE,FFT_SCALE0_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_SCALE1_OFFSET), READ_REG(FFT_BASE,FFT_SCALE2_OFFSET), READ_REG(FFT_BASE,FFT_SCALE3_OFFSET),
    //	 READ_REG(FFT_BASE,FFT_CMD_MEM_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM1_OFFSET));

    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffd; //1 fft_start
    val |= (0x1<<1);
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);

    //TRACE("leiyuan1:  0x%x",  READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET));
    if(mode == 1)
        hwfft_lock = 1;
    else{
        t = hal_sys_timer_get();
    #if defined(HWFFT_USE_BUSY_WAIT)
        while (hwfft_dma_lock);
            //TRACE("leiyuan11:  0x%x, 0x%x",  READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET), READ_REG(FFT_BASE,FFT_DATA_MEM0_OFFSET + 4*31));
    #else
        //current_sem_lock_id = fft_inst->_osSemaphoreId;
        //osSemaphoreWait(current_sem_lock_id, osWaitForever);
    #endif
        hwfft_sleep_ticks = hal_sys_timer_get()-t;
        // TRACE("leiyuan:%d",hwfft_sleep_ticks); 
    }
#if 0
    val = READ_REG(FFT_BASE,FFT_CONFIG_OFFSET);
    val &= 0xfffffffe; //0 fft_enable
    WRITE_REG(val,FFT_BASE,FFT_CONFIG_OFFSET);

    hal_dma_stop(in_ch);
    hal_dma_stop(ou_ch);

    //TRACE("%s:%d", __func__, __LINE__);
    hal_dma_free_chan(in_ch);
    hal_dma_free_chan(ou_ch);
#endif
    return 0;
}



