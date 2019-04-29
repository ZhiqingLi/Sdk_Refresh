#include "hw_memcpy_impl.h"
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#if(CONFIG_AUD_FADE_IN_OUT == 1)
extern int16_t s_fade_scale;
#endif
#ifdef BT_DUALMODE
int16 RAM_CODE rb_get_buffer_size(driver_ringbuff_t *rb)
#else
int16 DRAM_CODE rb_get_buffer_size(driver_ringbuff_t *rb)
#endif
{
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    uint32 dma_cur_ptr = 0;
#endif
    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_1\r\n");
        return -1;
    }
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    if(rb->dma_mode == 1)
    {
        dma_cur_ptr = rb_get_dma_src_cur_ptr();
        if(dma_cur_ptr > rb->wptr)
            rb->buffer_fill = rb->buffer_len + rb->wptr - dma_cur_ptr ;
        else
            rb->buffer_fill = rb->wptr - dma_cur_ptr;
    }
    //os_printf("Fill:%d\r\n",rb->buffer_fill);
    //if((rb->buffer_len - rb->buffer_fill)==AUDIO_BUFF_LEN)
    //    os_printf("***\r\n");
#endif
    return (rb->buffer_len - rb->buffer_fill);
}

//add begin by cai.zhong 20190220 for audio data
#ifdef BT_DUALMODE
int16 RAM_CODE rb_get_buffer_item_count(driver_ringbuff_t *rb)
#else
int16 DRAM_CODE rb_get_buffer_item_count(driver_ringbuff_t *rb)
#endif
{
    if (rb->buffp == NULL)
    {
        os_printf("Buffer_null_6\r\n");
        return -1;
    }  

    return rb->buffer_fill;
}
//add end by cai.zhong 20190220 for audio data

static __INLINE__ int16_t smpl_saturate(int32_t s)
{
	if(s > 32767)
		return 32767;
	else if(s < -32768)
		return -32768;
	else
		return s;
}
#ifdef BT_DUALMODE
int16  RAM_CODE rb_fill_buffer( driver_ringbuff_t *rb, uint8 *buff, uint16 size, int debug )
#else
int16  DRAM_CODE rb_fill_buffer( driver_ringbuff_t *rb, uint8 *buff, uint16 size, int debug )
#endif
{
    uint16 buff_space, cpy_size,differ;
    uint32 interrupts_info, mask;
    //app_handle_t sys_hdl = app_get_sys_handler();
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);

    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_3\r\n");
        return -1;
    }

#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||(CONFIG_EXT_PA_DIFF_EN == 1)
    if((debug != AEC_RX_FILL)&&(debug != AEC_TX_FILL)&&(debug != ADC_FILL)&&(flag_L_is_LplusR))
    {
        int16 *smpl = (int16 *)buff;
        int16 k;
        int32 tmp_smpl_L = 0;
        //int32 tmp_smpl_R = 0;
        for(k=0;k<(size>>2);k++)
        {
            tmp_smpl_L = (*(smpl + 2*k) + *(smpl + 2*k + 1)) >> 1;
            *(smpl + 2*k) = smpl_saturate(tmp_smpl_L);
            *(smpl + 2*k + 1) = smpl_saturate(tmp_smpl_L * (-1));
        }
    }
#endif
#if(CONFIG_AUD_FADE_IN_OUT == 1)
    {
        int32 smpl32_L,smpl32_R;
        int16 j;
        int16 *smpl16 = (int16 *)buff;
        if(get_aud_fade_in_out_state() & (AUD_FADE_IN | AUD_FADE_OUT ))
        {
            for(j=0; j<(size >> 2);j++)
            {
                smpl32_L =(int32)(*(smpl16 + 2*j) * s_fade_scale);
                smpl32_R =(int32)(*(smpl16 + 2*j + 1) * s_fade_scale);
                *(smpl16 + 2*j) = smpl32_L >> 7;
                *(smpl16 + 2*j + 1) = smpl32_R >> 7;
            }
        }
    }
#endif
    buff_space = rb_get_buffer_size(rb);
    
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)    
    if((rb->dma_mode == 1) && (buff_space <= 4))
    	return -1;
#endif

    if( buff_space > size )
    {
    #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
        if(rb->dma_mode == 1)
        {
        	/* For let src_ptr no equal cur_ptr */
            if((buff_space - size) > 3)
                cpy_size = size;
            else
                cpy_size = size - 4;
        }
        else
        {
            cpy_size = size;
        }
    #else
        cpy_size = size;
    #endif    
        //cpy_size = size;
    }
    else
    {
    #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
        if(rb->dma_mode == 1)
        {
        	/* 
        		buff_space must be greater than 4 
        	   and be saturate that src_ptr no equal cur_ptr
        	*/
            if(buff_space > 4)
                cpy_size = buff_space - 4;
            else
                cpy_size = 0;
        }
        else
        {
            cpy_size = buff_space;
        }
    #else
        cpy_size = buff_space;
    #endif
    }

    if( rb->wptr + cpy_size > rb->buffer_len ) 
    {
        differ = rb->buffer_len - rb->wptr;

    #if defined(CONFIG_TWS) 
    		if(buff)
    		{
    	        hw_memcpy8( (uint8 *)&rb->buffp[rb->wptr],
    	            buff, differ);
    	        hw_memcpy8( (uint8 *)&rb->buffp[0], buff+differ, cpy_size - differ);
    		}
    		else
    		{
    	        hw_memset8( (uint8 *)&rb->buffp[rb->wptr],
    	            0, differ);
    	        hw_memset8( (uint8 *)&rb->buffp[0], 0, cpy_size - differ);
    		}
    #else
                hw_memcpy8( (uint8 *)&rb->buffp[rb->wptr],
                    buff, differ);
                hw_memcpy8( (uint8 *)&rb->buffp[0], buff+differ, cpy_size - differ);
    #endif
        
        rb->wptr = cpy_size-differ;
    }
    else
    {
    #if defined(CONFIG_TWS) 
    		    if(buff)
    			{
    	        hw_memcpy8( (uint8 *)&rb->buffp[rb->wptr],
    	            buff, cpy_size);
    			}
    			else
    			{
    			hw_memset8( (uint8 *)&rb->buffp[rb->wptr],
    				0, cpy_size);
    			}

    #else
    	        hw_memcpy8( (uint8 *)&rb->buffp[rb->wptr],
    	            buff, cpy_size);
    #endif

        rb->wptr += cpy_size;
    }

    if( rb->wptr >= rb->buffer_len)
        rb->wptr -= rb->buffer_len;
    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill += cpy_size;
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    if(rb->dma_mode == 1)
        rb_set_dma_src_ptr((uint32)((&rb->buffp[0]) + rb->wptr));
#endif
    VICMR_restore_interrupts(interrupts_info, mask);
    return cpy_size;
}


void rb_init( driver_ringbuff_t *rb, uint8 *pbuff, uint16 dma_mode, uint16 len )
{    
    memset( (uint8 *)rb, 0, sizeof(driver_ringbuff_t));
    /* To be saturate that the ringbuff data are all '0' */
    memset(pbuff,0,len);
    rb->buffp = pbuff;
    rb->buffer_len = len;
    rb->buffer_fill = 0;
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    rb->dma_mode = dma_mode;
#endif
    return;
}

#ifdef BT_DUALMODE
int RAM_CODE rb_get_buffer_pcm(driver_ringbuff_t *rb, uint8 *buff, uint16 len )
#else
int DRAM_CODE rb_get_buffer_pcm(driver_ringbuff_t *rb, uint8 *buff, uint16 len )
#endif
{
     int ret = 0;
     int size1, size2;
     uint32 interrupts_info, mask;

     if (rb->buffp == NULL)
     {
        //os_printf("Buffer does not exist!\r\n");
        return -1;
     }  
     if (!rb->buffer_fill)
        goto exit;

     if( rb->rptr >= rb->wptr )
     {
         size1 = rb->buffer_len - rb->rptr;
         size2 = rb->wptr;

         if( len < size1 )
         {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
         }
         else
         {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            if( len - size1 < size2 )
            {
                memcpy( buff + size1, &rb->buffp[0], (len - size1) );//lianxue.liu
                ret = len;
                rb->rptr = len - size1;
            }
            else
            {
                memcpy( buff + size1, &rb->buffp[0], size2 ); //lianxue.liu
                ret = size1 + size2;
                rb->rptr = size2;
            }
         }
     }
     else
     {
        size1 = rb->wptr - rb->rptr;

        if( len < size1 )
        {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
        }
        else
        {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            ret = size1;
            rb->rptr += size1;
         }
     }
    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill -= ret;
    VICMR_restore_interrupts(interrupts_info, mask);

exit:
    return ret;

}

#ifdef BT_DUALMODE
RAM_CODE int  rb_get_buffer_with_length( driver_ringbuff_t *rb, uint8 *buff, uint16 len )
#else
DRAM_CODE int  rb_get_buffer_with_length( driver_ringbuff_t *rb, uint8 *buff, uint16 len )
#endif
{
     int ret = 0;
     int size1, size2;
     uint32 interrupts_info, mask;

     if (rb->buffp == NULL)
     {
        //os_printf("Buffer_null_4\r\n");
        return -1;
     }
     
     if (!rb->buffer_fill)
        goto exit;
     
     if( rb->rptr >= rb->wptr )
     {
         size1 = rb->buffer_len - rb->rptr;
         size2 = rb->wptr;

         if( len < size1 )
         {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
         }
         else
         {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            if( len - size1 < size2 )
            {
                memcpy( buff + size1, &rb->buffp[0], (len - size1) );//lianxue.liu
                ret = len;
                rb->rptr = len - size1;
            }
            else
            {
                memcpy( buff + size1, &rb->buffp[0], size2 ); //lianxue.liu
                ret = size1 + size2;
                rb->rptr = size2;
            }
         }
     }
     else
     {
        size1 = rb->wptr - rb->rptr;

        if( len < size1 )
        {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
        }
        else
        {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            ret = size1;
            rb->rptr += size1;
         }
     }


    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill -= ret;
    VICMR_restore_interrupts(interrupts_info, mask);

exit:
    return ret;
}

#ifdef BT_DUALMODE
int RAM_CODE rb_get_one_sample( driver_ringbuff_t *rb, uint16 *out )
#else
int DRAM_CODE rb_get_one_sample( driver_ringbuff_t *rb, uint16 *out )
#endif
{
    uint32 interrupts_info, mask;

    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_5\r\n");
        return -1;
    }
    
    if (!rb->buffer_fill)
        return -1;

    *out = rb->buffp[rb->rptr] + (rb->buffp[rb->rptr + 1] << 8);

    rb->rptr += 2;
    
    if( rb->rptr >= rb->buffer_len )
        rb->rptr = 0;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill -= 2;
    VICMR_restore_interrupts(interrupts_info, mask);

    return 0;
}
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
__INLINE__ uint16 rb_get_dma_src_cur_ptr(void)
{
    return(reg_DMA_CHO_SRC_CUR_PTR - reg_DMA_CH0_SRC_TOP);
}
__INLINE__ void rb_set_dma_src_ptr(uint32 ptr)
{
    reg_DMA_CH0_SRC_ADDR = ptr;
    //os_printf("Wptr:%p,Rptr:%p\r\n",reg_DMA_CH0_SRC_ADDR,reg_DMA_CHO_SRC_CUR_PTR);
}
#endif

#if (CONFIG_APP_MP3PLAYER == 1)
int16 RAM_CODE rb_get_buffdata_size(driver_ringbuff_t *rb) //add by zjw for mp3decode
{
    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_2\r\n");
        return -1;
    }
    
    if( rb->wptr < rb->rptr )
        return (rb->buffer_len - (rb->rptr - rb->wptr));
    else
        return ((rb->wptr - rb->rptr));
}

int RAM_CODE rb_get_buffer_mp3(driver_ringbuff_t *rb, uint8 *buff, uint16 len )
{
     int ret = 0;
     int size1, size2;
     uint32 interrupts_info, mask;
     if (rb->buffp == NULL)
     {
        //os_printf("Buffer does not exist!\r\n");
        return -1;
     }  
     if (!rb->buffer_fill)
        goto exit;
     if( rb->rptr >= rb->wptr )
     {
         size1 = rb->buffer_len - rb->rptr;
         size2 = rb->wptr;

         if( len < size1 )
         {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
         }
         else
         {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            if( len - size1 < size2 )
            {
                memcpy( buff + size1, &rb->buffp[0], (len - size1) );//lianxue.liu
                ret = len;
                rb->rptr = len - size1;
            }
            else
            {
                memcpy( buff + size1, &rb->buffp[0], size2 ); //lianxue.liu
                ret = size1 + size2;
                rb->rptr = size2;
            }
         }
     }
     else
     {
        size1 = rb->wptr - rb->rptr;

        if( len < size1 )
        {
            memcpy( buff, &rb->buffp[rb->rptr], len );
            ret = len;
            rb->rptr += len;
        }
        else
        {
            memcpy( buff, &rb->buffp[rb->rptr], size1 );
            ret = size1;
            rb->rptr += size1;
         }
     }
    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill -= ret;
    VICMR_restore_interrupts(interrupts_info, mask);

exit:
    return ret;
}
#if 0
int16 RAM_CODE rb_fill_buffer_MP3( driver_ringbuff_t *rb, uint8 *buff, uint16 size, int debug )
{
    uint16 /* buff_space,  */cpy_size,differ;
    uint32 interrupts_info, mask;

    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_6\r\n");
        return -1;
    }
    
    cpy_size = size;

   //os_printf("size: %d, buff_space: %d, cpy_size: %d\n", size, buff_space, cpy_size);
    
    if( rb->wptr + cpy_size > rb->buffer_len ) 
    {
        differ = rb->buffer_len - rb->wptr;
        memcpy( (uint8 *)&rb->buffp[rb->wptr],
            buff, differ);
        memcpy( (uint8 *)&rb->buffp[0], buff+differ, cpy_size - differ);
        rb->wptr = cpy_size-differ;
    }
    else
    {
        memcpy( (uint8 *)&rb->buffp[rb->wptr],
            buff, cpy_size);
        rb->wptr += cpy_size;
    }

    if( rb->wptr >= rb->buffer_len)
        rb->wptr -= rb->buffer_len;

    VICMR_disable_interrupts(&interrupts_info, &mask);
    rb->buffer_fill += cpy_size;
    VICMR_restore_interrupts(interrupts_info, mask);

    return cpy_size;
}

int RAM_CODE rb_get_one_sample_mp3( driver_ringbuff_t *rb, uint16 *out )
{
    if (rb->buffp == NULL)
    {
        //os_printf("Buffer_null_7\r\n");
        return -1;
    }
    
    if( rb->rptr == rb->wptr )
        return -1;

    *out = *((uint16 *)&rb->buffp[rb->rptr]);

    rb->rptr += 2;

    return 0;
}
#endif
#endif

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
int32 rb_get_sample_number(driver_ringbuff_t *rb)
{
    if (rb->buffp == NULL)
    {
        return -1;
    }
    return (rb->buffer_fill);
}
#endif

#ifdef CONFIG_APP_AEC
//adjust length of data in aud_buff.Goal for HFP aec.
//len=adjust step.
RAM_CODE void rb_adj_dummy_data(driver_ringbuff_t *rb,int16 len)
{
    int16 tmp,adj_len;
    uint32 interrupts_info, mask;
//limit value of 'len' in valid range
	tmp = (int16)rb->buffer_fill + len;
    if(tmp<0)
    {
        adj_len = (-(int16)rb->buffer_fill) ;
    }
    else if((uint16)tmp >= (rb->buffer_len-4))
    {
        adj_len = rb->buffer_len-4-rb->buffer_fill;
        if(adj_len<0) 
            adj_len = 0;
    }
    else
    {
        adj_len = len;
    }
    rb->buffer_fill = (uint16)((int16)rb->buffer_fill + adj_len);
    if(adj_len<0) 
        adj_len += rb->buffer_len;
    rb->wptr += adj_len ;
    if( rb->wptr >= rb->buffer_len)
        rb->wptr -= rb->buffer_len;
    VICMR_disable_interrupts(&interrupts_info, &mask);
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    if(rb->dma_mode == 1)
        rb_set_dma_src_ptr((uint32)((&rb->buffp[0]) + rb->wptr));
#endif
	VICMR_restore_interrupts(interrupts_info, mask);
}
#endif
// EOF
