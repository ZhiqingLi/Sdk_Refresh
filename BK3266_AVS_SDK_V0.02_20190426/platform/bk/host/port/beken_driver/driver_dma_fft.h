#ifndef __DRIVER_DMA_FFT_H_
#define __DRIVER_DMA_FFT_H_
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
/*
   DMA Trigger Condition 
   0x00 -- software
   0x01 -- uart_rx
   0x02 -- uart_tx
   0x03 -- pcm_rx 
   0x04 -- pcm_tx 
   0x05 -- aud_adc
   0x06 -- aud_dac  

   DMA Control Words 
   bit[    0] -- dma interrupt flag
   bit[    1] -- dma soft request
   bit[    2] -- dma enable bit
   bit[    3] -- dma interrupt enable
   bit[    4] -- dma abort
   bit[    5] -- dma trig type : 0(Posedge)  1(High Level) 
   bit[ 7: 6] -- scr data type : 0(8 bits) 1(16 bits) 2(32 bits)
   bit[ 9: 8] -- dst data type : 0(8 bits) 1(16 bits) 2(32 bits)
   bit[11:10] -- scr addr incr : 0/1(no change) 2(decrease) 3(increase)
   bit[13:12] -- dst addr incr : 0/1(no change) 2(decrease) 3(increase)
   bit[15:14] -- dma xfr  mode : 0(single) 1(block) 2(single repeat) 3(block repeat)
*/	

struct dma_struct {
    uint8   dma_enable;
	uint8   request_select   ;  // 0(Software) 1(Uart_rx) 2(Uart_tx) 3(Pcm_rx) 4(Pcm_tx) 5(Aud_rx) 6(Aud_tx)  
	uint8   transfer_mode    ;  // 0(Single) 1(Block) 2(SingleRepeat) 3(BlockRepeat)
	uint8   dst_addr_incr    ;  // 0/1(No change) 2(Decrease) 3(Increase)
	uint8   src_addr_incr    ;  // 0/1(No change) 2(Decrease) 3(Increase)
	uint8   dst_data_type    ;  // 0(8 bits) 1(16 bits) 2(32 bits)
	uint8   src_data_type    ;  // 0(8 bits) 1(16 bits) 2(32 bits)
	uint8   trigger_type     ;  // 0(Posedge)  1(High Level) 
	uint8   interrupt_en     ;  // 0(Disable)  1(Enable) 
	uint8   soft_request     ;  // 0(Hardware) 1(Software)
	uint16  transfer_size    ;  // DMA Transfer Length
	uint32  src_address      ;  // DMA Source Address
	uint32  dst_address      ;  // DMA Destination Address
	uint32  src_addr_top     ;  // DMA Source Top Boundary
	uint32  src_addr_bottom  ;  // DMA Source Bottom Boundary
	uint32  dst_addr_top     ;  // DMA Destination Top Boundary
	uint32  dst_addr_bottom  ;  // DMA Destination Bottom Boundary
};

void start_dma_transfer(uint8 dma_channel, struct dma_struct *dma_config);

void DMA_InterruptHandler(void);

void dma_initial(void);
#endif
enum
{
    FFT_MODE_FFT = 0,
    FFT_MODE_IFFT = 1
};

#ifdef BT_DUALMODE
int RAM_CODE fft_busy( void );
void RAM_CODE fft_enable( int mode, int16 *inbuf, int16* outbuf, uint16 size );
void RAM_CODE fft_isr( void );
#else
int DRAM_CODE fft_busy( void );
void DRAM_CODE fft_enable( int mode, int16 *inbuf, int16* outbuf, uint16 size );
void DRAM_CODE fft_isr( void );
#endif

typedef struct _driver_fir_conf_s
{
    uint8 fir_len;
    uint8 fir_cwidth;
    uint8 fir_dwidth;
}driver_fir_conf_t;

//void fir_single_enable( driver_fir_conf_t *fir_conf, int16 *coef, int16* input, int32 *mac );
//void fft_test( void );

#endif

