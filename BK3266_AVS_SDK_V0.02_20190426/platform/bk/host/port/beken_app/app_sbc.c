#include <string.h>
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
#include "beken_external.h"
#endif
volatile uint8 flag_sbc_buffer_play = 0;
static sbc_mem_node_t  *sbc_mem_node=NULL;
static app_sbc_t  app_sbc;
int encode_buffer_full = 0;

#ifdef BEKEN_DEBUG
int encode_pkts = 0;
int decode_pkts = 0;
int encode_buffer_empty = 0;
#endif

#ifdef CONFIG_TWS
int stereo_mode_slave_latency = 0;
#endif

#if (SBC_ERR_FRM_PROC == 1)
static uint8 MidFilter_Flag[AUDIO_BUFF_LEN >> 9];
#define SBC_MUTE_THR	6
#endif

#ifdef SBC_FIRST_DISCARD_ENABLE 
uint8 sbc_first_discrad_flag = 0;
uint8 sbc_first_discard_count = 0;
uint8 sbc_first_discard_data = 0;
#endif
#if (CONFIG_PRE_EQ == 1)
extern aud_pre_equ_t aud_pre_eqe;
extern void func_sw_eq(int *input,int *output,int len,int eq_cnt,uint32 index);//len: 1=L+R=4bytes
#endif

void sbc_init_adjust_param(void)
{
    app_sbc.sbc_ecout = 0;
    app_sbc.sbc_clk_mode = 0;
}

static RAM_CODE void sbc_mem_pool_freeNode( sbc_mem_list_t  *list,
     sbc_mem_node_t *node )
{
    if( list->head == NULL )
    {
        list->head = list->tail = node;
        node->next = NULL;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
        node->next = NULL;
    }
}

static RAM_CODE sbc_mem_node_t * sbc_mem_pool_getNode( sbc_mem_list_t *list)
{
    sbc_mem_node_t *node;

    if( list->head == NULL )
        return NULL;

    node = list->head;
    list->head = list->head->next;
    if( list->head == NULL )
        list->tail = NULL;

    return node;
}

void sbc_mem_pool_init( int framelen )
{
    uint8* mem_base = app_sbc.sbc_encode_buffer;
    int   node_len = 120; //( (framelen >> 2) + 1 ) << 2; // alignment
    int   node_num = SBC_ENCODE_BUFFER_LEN / node_len;
    int  i;

    if( node_num > SBC_MEM_POOL_MAX_NODE )
        node_num = SBC_MEM_POOL_MAX_NODE;

    app_sbc.sbc_mem_pool.node_num = node_num;

    sbc_mem_node = (sbc_mem_node_t *)jmalloc(sizeof(sbc_mem_node_t)*SBC_MEM_POOL_MAX_NODE, M_ZERO);

    //os_printf("[sbc buffer pool init: node num: %d, node_len : %d\n", node_num, node_len );
    for( i = 0; i < node_num; i++)
    {
        sbc_mem_node[i].data_ptr = mem_base + i*node_len;
        sbc_mem_pool_freeNode( &app_sbc.sbc_mem_pool.freelist, &sbc_mem_node[i] );
        //os_printf("node[%d] memptr: %x\n", i, sbc_mem_node[i].data_ptr);
    }
	#ifdef CONFIG_TWS
	stereo_mode_slave_latency = 0;
	dac_init_clk();
//	show_dac_clk(1);
	#endif
}

void sbc_mem_pool_deinit( void )
{
    sbc_mem_node_t *node;
//    os_printf("sbc_mem_pool_deinit_1\r\n");
    do
    {
        node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
    #ifdef BEKEN_DEBUG
        decode_pkts++;
    #endif
    }while( node != NULL );

    do
    {
        node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.freelist );
    }while( node != NULL );

//    os_printf("sbc_mem_pool_deinit_2\r\n");
    app_sbc.sbc_mem_pool.node_num = 0;
    app_sbc.sbc_target_initial = 0;
    
#ifdef CONFIG_TWS
    app_sbc.decode_cnt = 0;
    app_sbc.src_pkt_num = 0;
#endif
    
    if (sbc_mem_node) {
        jfree(sbc_mem_node);
        sbc_mem_node = NULL;
    }
}

uint32 sbc_buf_get_use_count(void)
{
    #ifdef BT_ONE_TO_MULTIPLE
        return app_sbc.use_count;
    #else
        return 0;
    #endif
}

void sbc_buf_increase_use_count(void)
{
    #ifdef BT_ONE_TO_MULTIPLE
    app_sbc.use_count += 1;
    #endif
}

void sbc_buf_decrease_use_count(void)
{
    #ifdef BT_ONE_TO_MULTIPLE
    app_sbc.use_count -= 1;
    #endif
}

uint16 sbc_buf_get_node_count(void)
{
    return app_sbc.sbc_ecout;
}

void sbc_target_init_malloc_buff(void)
{
#if defined(BT_ONE_TO_MULTIPLE) && (!defined(CONFIG_APP_AEC))
        app_sbc.sbc_encode_buffer =(uint8 *) &sbc_encode_buff[0];
#else

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE

    if(app_sbc.sbc_encode_buffer)
        jfree(app_sbc.sbc_encode_buffer);

    if(bt_flag1_is_set(APP_FLAG_LINEIN))
    {
        app_sbc.sbc_encode_buffer = (uint8 *)jmalloc(get_linein_buffer_length() * sizeof(uint8), M_ZERO);
    }
    else
    {
        app_sbc.sbc_encode_buffer = (uint8 *)jmalloc(SBC_ENCODE_BUFFER_LEN * sizeof(uint8), M_ZERO);
    }

#else

    if(NULL == app_sbc.sbc_encode_buffer)
        app_sbc.sbc_encode_buffer = (uint8 *)jmalloc(SBC_ENCODE_BUFFER_LEN * sizeof(uint8), M_ZERO);

#endif

#endif

    if(NULL == app_sbc.sbc_output) 
    {
        app_sbc.sbc_output = (uint8 *)jmalloc(512 * sizeof(uint8), M_ZERO);
    }
}

void sbc_target_deinit_jfree_buff(void)
{
    if(app_sbc.sbc_output)
    {
        jfree(app_sbc.sbc_output);
        app_sbc.sbc_output = NULL;
    }
#if defined(BT_ONE_TO_MULTIPLE) && (!defined(CONFIG_APP_AEC))
    app_sbc.sbc_encode_buffer = NULL;
#else
    if(app_sbc.sbc_encode_buffer)
    {
        jfree(app_sbc.sbc_encode_buffer);
        app_sbc.sbc_encode_buffer = NULL;
    }
#endif
}

void *sbc_get_sbc_ptr(void)
{
    return (void *)app_sbc.sbc_ptr;
}

void sbc_target_init( sbc_t *sbc )
{
    app_sbc.sbc_ptr = sbc;
    app_sbc.sbc_target_initial = 0;
#ifdef CONFIG_TWS
    app_sbc.src_pkt_num = 0;
#endif

    /* sbc_target_init_malloc_buff(); */

    return;
}

void sbc_target_deinit( void )
{
    sbc_mem_pool_deinit();
    sbc_target_deinit_jfree_buff();//modify by zjw for malloc space
    app_sbc.sbc_ptr = NULL;
}
void sbc_mem_free(void)
{
	sbc_mem_pool_deinit();
	sbc_target_deinit_jfree_buff();
}
void sbc_stream_start_init( void )
{
    app_sbc.sbc_first_try = 0;
}

#ifdef CONFIG_TWS
static void sbc_dac_clk_adjust_tws( void )
{
	if( app_sbc.sbc_ecout >= 55 )
	{
		dac_clk_adjust_tws(3);
		app_sbc.sbc_clk_mode = 3;
	}
	else if( app_sbc.sbc_ecout <= 15 )
	{
		dac_clk_adjust_tws(4);
		app_sbc.sbc_clk_mode = 4;
	}
	else if(app_sbc.sbc_ecout >= 40)
	{
		dac_clk_adjust_tws(1);
		app_sbc.sbc_clk_mode = 1;
	}
	else if(app_sbc.sbc_ecout <= 30)
	{
		dac_clk_adjust_tws(2);
		app_sbc.sbc_clk_mode = 2;
	}
	else if(app_sbc.sbc_clk_mode != 0 )
	{
		dac_clk_adjust_tws(0);
		app_sbc.sbc_clk_mode = 0;
	}
}
#endif

static void sbc_dac_clk_adjust( void )
{
#if 0
    if( app_sbc.sbc_clk_mode != 1 && app_sbc.sbc_ecout >= 50 )
    {
        dac_clk_adjust(1);
        app_sbc.sbc_clk_mode = 1;
    }
    else if( app_sbc.sbc_clk_mode != 0 && app_sbc.sbc_ecout <= 40 && app_sbc.sbc_ecout >= 30 )
    {
        dac_clk_adjust(0);
        app_sbc.sbc_clk_mode = 0;
    }
    else if( app_sbc.sbc_clk_mode != 2 && app_sbc.sbc_ecout <= 15 )
    {
        dac_clk_adjust(2);
        app_sbc.sbc_clk_mode = 2;
    }
#else
    if( app_sbc.sbc_clk_mode != 1 && app_sbc.sbc_ecout >= (SBC_MEM_POOL_MAX_NODE *3)>>2 ) // 3/4
    {
        dac_clk_adjust(1);
        app_sbc.sbc_clk_mode = 1;
    }
    else if(app_sbc.sbc_clk_mode != 2 && app_sbc.sbc_ecout <= SBC_MEM_POOL_MAX_NODE >> 2 ) // 1/4
    {
        dac_clk_adjust(2);
        app_sbc.sbc_clk_mode = 2;
    }
    else
    {
        dac_clk_adjust(0);
        app_sbc.sbc_clk_mode = 0;
    }
#endif
    
}
#ifdef CONFIG_TWS
void app_stereo_master_start_play(void);
#endif


/*
 * status = sbc_encode_buffer_status(device_index),where:
 * <value> = 0: normal;
 * <value> = 1: host layer sbc node buffer is nearly full;
 * <value> = 2: host layer sbc node buffer is nearly empty;         
 */
uint8 RAM_CODE sbc_encode_buffer_status(void)
{
    uint8 status = 0;
    if(bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_SCO_CONNECTION))
        return 0;

    if (!bt_flag1_is_set(APP_FLAG_MUSIC_PLAY))
    	return 0;

    if((SBC_MEM_POOL_MAX_NODE - app_sbc.sbc_ecout) < 10)
        status = 1;  /* SBC node buffer nearly full !!! */
    else if(app_sbc.sbc_ecout < 10)
        status = 2; /* SBC node buffer nearly empty !!! */
    return status; 
        
}

uint8 RAM_CODE sbc_encode_and_audio_low(void)
{
    if (!bt_flag1_is_set(APP_FLAG_MUSIC_PLAY))
    	return 0;

    if ((app_sbc.sbc_ecout < 20) || (aud_get_buffer_size() > 512))
        return 1;

    return 0;
}

#ifdef LE_PROTECT_MUSIC_FLUENCY_ENABLE 
uint8 RAM_CODE BLE_check_sbc_encode_and_audio_low(void)
{
    if (!bt_flag1_is_set(APP_FLAG_MUSIC_PLAY))
    	return 0;

    if ((app_sbc.sbc_ecout < 10) || (aud_get_buffer_size() > 1024))    //sbc_ecout 10*2.9ms=29ms 
        return 1;

    return 0;
}
#endif

void RAM_CODE sbc_fill_encode_buffer( struct mbuf *m, int len, int frames )
{
    int i;
    int sbc_frame_len = len/frames;
    sbc_mem_node_t *node;

#ifndef BT_ONE_TO_MULTIPLE
    if( !bt_flag1_is_set( APP_FLAG_MUSIC_PLAY ) )
        return;
#endif
	#ifdef CONFIG_TWS
		if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
			return;
	#endif

// change begin by cai.zhong 20190301 for sync code
#if 1
    if(app_get_mic_state())
        return;
#endif
// change end by cai.zhong 20190301 for sync code

    if( app_sbc.sbc_target_initial == 0 )
    {
        sbc_target_init_malloc_buff();
        sbc_mem_pool_init(  sbc_frame_len );
	 	encode_buffer_full = 0;
        app_sbc.sbc_target_initial = 1;
        app_sbc.sbc_ecout = 0;
        app_sbc.sbc_clk_mode = 0;
    }
#ifdef CONFIG_SBC_PROMPT
if(!app_wave_playing())
#endif
{
    for( i = 0; i < frames; i++)
    {
        node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.freelist );
        if( node == NULL )
        {
#ifdef CONFIG_TWS
		if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
			sync_data.u.sync_send.aud_num+=(512/4)*(frames-i);
		os_printf("f");
#endif
            //os_printf("f:%d\r\n", encode_buffer_full);
            encode_buffer_full++;
            break;
        }
     #ifdef BEKEN_DEBUG
        encode_pkts++;
     #endif

        m_copydata( m, i*sbc_frame_len, sbc_frame_len, (void *)node->data_ptr);
        sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.uselist, node);
   }

   app_sbc.sbc_ecout += i;
  }
#ifdef CONFIG_TWS
	  if(!bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
#endif
		sbc_dac_clk_adjust();
}

//void aud_src_from_bt_audio(void * arg);
#if (SBC_ERR_FRM_PROC == 1)
uint8 RAM_CODE mid_filter(uint8 *output,uint8 *filt_flag)
{
//	int16 *smpl = (int16 *)output;
	uint16 *smplr = (uint16 *)output;
	int16 k;
	uint32 smpl_amp_sum = 0;
	uint16 flag_sum = 0;

	for(k=0; k<256; k++)
	{
//		smpl_amp_sum += ABS(*(smpl+2*k));
//		smpl_amp_sum += ABS(*(smpl+2*k+1));
		if((*smplr)!=0)
		{
			smpl_amp_sum = 1;
			break;
		}
		smplr ++;
	}
	for(k=1; k<(AUDIO_BUFF_LEN>>9); k++)
	{
		flag_sum += filt_flag[k];
		filt_flag[k-1] = filt_flag[k];

	}
 	filt_flag[(AUDIO_BUFF_LEN>>9)-1] = (smpl_amp_sum > 0);
 	flag_sum += filt_flag[(AUDIO_BUFF_LEN>>9)-1];
 	if(flag_sum > SBC_MUTE_THR)
 		return 1;
 	else
 		return 0;
}
#endif
#ifdef A2DP_SBC_DUMP_SHOW
void sbc_encode_frame_info(void)
{
    if(app_sbc.sbc_ptr != NULL)
    {
        os_printf("------SBC FRM INFO-----\r\n");
        os_printf("|     freq:%d\r\n",app_sbc.sbc_ptr->frequency);
        os_printf("|   blocks:%d\r\n",app_sbc.sbc_ptr->blocks);
        os_printf("| subbands:%d\r\n",app_sbc.sbc_ptr->subbands);
        os_printf("|     mode:%d\r\n",app_sbc.sbc_ptr->mode);
        os_printf("|  bitpool:%d\r\n",app_sbc.sbc_ptr->bitpool);
        os_printf("----------------------------\r\n");
    }
}
#endif
#if (CONFIG_DRC == 1)
static __inline int16 f_sat(int32 din)
{
    if (din>32767)
        return 32767;
    if (din<-32768)
        return -32768;
    else
        return(din);
}
#endif
void RAM_CODE sbc_do_decode( void )
{
#ifdef CONFIG_TWS
//app_handle_t app_h = app_get_sys_handler();
//static int delta_total=0;
//static int delta_last=0;
#endif
    int decode_frms, i, frame_len;
    sbc_mem_node_t *node;
    if((app_sbc.sbc_ptr == NULL)
		|| (app_sbc.sbc_target_initial == 0))
    {
        return;
    }
#if 0//def CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
        return ;
#endif

    // change begin by cai.zhong 20190301 for sync code
    if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION) || app_get_mic_state())  //for the bug:the sbc still has remained data to fill in buff while sco has connection
    // change end by cai.zhong 20190301 for sync code
    {
        do
        {
            node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
            if(node != NULL)
            {
                sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
                app_sbc.sbc_ecout --;
            }
        }while(node != NULL);
        return;
    }
#ifdef CONFIG_TWS
    if (bt_flag1_is_set(APP_FLAG_HFP_OUTGOING))
        return;

    if (bt_flag2_is_set(APP_FLAG2_HFP_INCOMING))
        return;

    //if( bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
    //  && bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
    //  && app_sbc.sync_flags == 0 )
    //{
    //  app_sbc.sync_flags = 1;
    //  app_sbc.decode_cnt = 0;
    //  return;
    //}
#endif

	//  sbc buffer cache, 	avoid "POP"
    if(!flag_sbc_buffer_play)
    {
        if(bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
        {
            do
            {
                node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
                if(node != NULL)
                {
                    sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
                    app_sbc.sbc_ecout --;
                }
            }while(node != NULL);
            return;
        }
        if(app_sbc.sbc_ecout < SBC_FIRST_TRY_TIME)
            return ;
		
    #ifdef SBC_FIRST_DISCARD_ENABLE
        /*discrad SBC begin data due to the data flow overflow*/
        if(sbc_first_discrad_flag == 1)
        {
        #ifndef CONFIG_TWS
            uint8 i;
        #endif
            sbc_first_discard_count++;
            if(sbc_first_discard_count <= sbc_first_discard_data) //discard sbc_first_discard_data*45 note SBC data
            {
        #ifdef CONFIG_TWS
            if(get_sbc_mem_pool_Nodes()<SBC_FIRST_TRY_TIME)
                return;
        #else
            for(i = 0; i < SBC_FIRST_TRY_TIME; i++)
            {
                node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
                if(node != NULL)
                {
                    sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
                    app_sbc.sbc_ecout --;
                }
            }
            return;
        #endif
            }
            sbc_first_discrad_flag = 0;
            sbc_first_discard_count = 0;
            sbc_first_discard_data = 0;
        }
    #endif
		
        flag_sbc_buffer_play = 1;
    #ifdef CONFIG_TWS
        if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
            && bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
            && bt_flag2_is_set (APP_FLAG2_SYNC)
            &&!bt_flag1_is_set( APP_FLAG_ROLE_SWITCH )
            &&bt_flag2_is_set (APP_FLAG2_STEREO_STREAMING))
        {
            app_stereo_master_start_play();
        }
    #endif
        os_printf("===sbc cache:%d,%d\r\n",app_sbc.sbc_ecout,app_sbc.sbc_target_initial);
        #if(CONFIG_AUD_FADE_IN_OUT == 1)
            set_aud_fade_in_out_state(AUD_FADE_IN);
        #endif
		/* if it selected 1to2 option, this sbc_re_init() is very important and necessary */
        sbc_re_init(app_sbc.sbc_ptr);
    #if (SBC_ERR_FRM_PROC == 1)
        memset(MidFilter_Flag,0,sizeof(MidFilter_Flag));
    #endif
    }
    decode_frms = (aud_get_buffer_size()+1)/512;
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    if(decode_frms > 0)
    {
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
        for( i = 0; i < decode_frms; i++ )
        {
            node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
        #if 0//def CONFIG_SBC_PROMPT
            if(node == NULL)
            {
                if(app_wave_playing()
                    &&(app_wave_check_type(INTER_SBC)||app_wave_check_type(EXT_SBC))
                    &&(app_wave_check_status(PROMPT_EMPTY)))
                {
                    app_wave_set_status(PROMPT_STOP);
                }
            }
        #endif
        #if (SBC_ERR_FRM_PROC == 1)
            if(node == NULL)
            {
                memset(app_sbc.sbc_output,0,512);
                encode_buffer_empty++;
            }
        #else
            if( node == NULL )
            {
            #ifdef BEKEN_DEBUG
                encode_buffer_empty++;
            #endif
                break;
            }
        #endif
        #ifndef BT_ONE_TO_MULTIPLE
            /*if A is playing music,the app_sleep_func() can't be executed,and B may not enter sniff mode !*/
            CLEAR_SLEEP_TICK;
        #endif
            CLEAR_PWDOWN_TICK;

        #if (SBC_ERR_FRM_PROC == 1)
            if(node != NULL)
            {
                frame_len = sbc_decode( app_sbc.sbc_ptr, node->data_ptr,
                                                        SBC_ENCODE_BUFFER_LEN,
                                                        app_sbc.sbc_output, 512, NULL,SBC_MODE);
					if(bt_flag2_is_set(APP_FLAG2_SW_MUTE))
					{
						memset(app_sbc.sbc_output,0,512);
					}
            #ifdef CONFIG_TWS
                if(app_sbc.decode_cnt<200)
                    app_sbc.decode_cnt ++;
            #endif

                sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
            #ifdef BEKEN_DEBUG
                decode_pkts++;
            #endif
                app_sbc.sbc_ecout --;
                if( frame_len < 0 )
                {
                    os_printf("ERR: frame_len(=%d) < 0\r\n",frame_len);
                	continue;
                }
            }
        #else
            frame_len = sbc_decode( app_sbc.sbc_ptr, node->data_ptr,
                                                    SBC_ENCODE_BUFFER_LEN,
	                app_sbc.sbc_output, 512, NULL,SBC_MODE);
					if(bt_flag2_is_set(APP_FLAG2_SW_MUTE))
					{
						memset(app_sbc.sbc_output,0,512);
					}
        /*************************************
	#ifdef CONN_WITH_MUSIC
		if(bt_flag2_is_set(APP_FLAG2_SW_MUTE))
		{
			memset(app_sbc.sbc_output,0,512);
		}
	#endif
       ************************************/
        #ifdef BEKEN_DEBUG
            decode_pkts++;
        #endif

            sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
            if( frame_len < 0 ) {
                os_printf("ERR: frame_len < 0\r\n");
                continue;
            }
        #endif

	        //if( app_sbc.sbc_first_try <= SBC_FIRST_DISCARD_DATA )
	        //     app_sbc.sbc_first_try++;
	        //else
			//if( !bt_flag1_is_set( APP_FLAG_WAVE_PLAYING|APP_FLAG_HFP_CALLSETUP) )
        #if 0//def CONFIG_SBC_PROMPT
            if(!bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
            	&&app_wave_playing()
            	&&(app_wave_check_type(INTER_SBC)||app_wave_check_type(EXT_SBC)))
            {

            	#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            		app_wave_file_fill_buffer(app_sbc.sbc_output, 256);
            	#else
            		aud_fill_buffer( app_sbc.sbc_output,256);
            	#endif
            }
#endif
            //if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING) && !bt_flag2_is_set(APP_FLAG2_HFP_INCOMING))
    		if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
            {
            #if (SBC_ERR_FRM_PROC == 1)
                if(!mid_filter(app_sbc.sbc_output,MidFilter_Flag))
                    memset(app_sbc.sbc_output,0,512);
            #endif
				
                if(get_bt_dev_priv_work_flag())
                {
                    set_aud_fade_in_out_state(AUD_FADE_OUT);
                }
                else if(bt_flag1_is_set(APP_FLAG_HFP_OUTGOING) || bt_flag2_is_set(APP_FLAG2_HFP_INCOMING))
                {
                    memset(app_sbc.sbc_output,0,512);   
                }
			
            #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
                int16 *sample = (int16 *)app_sbc.sbc_output;
                int a;
                for( a = 0; a < 256; a+= 16 )
                aud_mute_update( sample[a], sample[a+1] );
            #endif

            #ifdef BT_ONE_TO_MULTIPLE
                if(!app_wave_playing ())
            #endif
                {
                #ifdef CONFIG_TWS
                //if(app_sbc.decode_cnt>FRAME_REDUCE_NUM)
                    {
                        int j;//delta_clk
                        j = 512;
                        if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)&&
                            !bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
                        {
                        //if(abs(stereo_mode_slave_latency)>=AUDIO_DIV_INIT)
                            {
                                while(stereo_mode_slave_latency<-512/4)
                                {
                                    node = sbc_mem_pool_getNode( &app_sbc.sbc_mem_pool.uselist );
                                    if(node)
                                    {
                                        sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
                                        stereo_mode_slave_latency += 512/4;
                                        sync_data.u.sync_send.aud_num += 512/4;
                                        app_sbc.sbc_ecout --;
                                    }
                                    else
                                        break;
                                }
                                if(stereo_mode_slave_latency >48)
                                {
                                    int tmp_j;
                                    tmp_j= MIN(aud_get_buffer_size()-512,stereo_mode_slave_latency*4);
                                    stereo_mode_slave_latency -= tmp_j/4;

                                    sync_data.u.sync_send.aud_num -= tmp_j/4;
                                    aud_fill_buffer(NULL, tmp_j);
                                }
                                else if(stereo_mode_slave_latency<-48)
                                {
                                    j = 512 + stereo_mode_slave_latency*4;
                                    if(j<0) j=0;
                                        stereo_mode_slave_latency += (512-j)/4;
                                    sync_data.u.sync_send.aud_num += (512-j)/4;
                                }
                            }
                        }

                        debug_show_trace(5);
                        debug_set_trace_val(0);

                        aud_fill_buffer(app_sbc.sbc_output, j);

                        if( a2dp_has_music() && aud_get_buffer_size() < 1024 )
                        {
                            if(!bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
                            {
                                aud_open();
                            }
                        }
                    }
                #else
                #if (CONFIG_PRE_EQ == 1)
                    int32 data_tmp[256],p;
                    int16 *ptr;
                    ptr = (int16 *)app_sbc.sbc_output;
                    if((0x5a == aud_pre_eqe.online_flag)||(1 == aud_pre_eqe.online_flag))
                    {
                        for(p=0;p<256;p++)
                            data_tmp[p] = (ptr[p]*aud_pre_eqe.globle_gain);
                        func_sw_eq((int32 *)data_tmp,(int32 *)data_tmp,512/4,aud_pre_eqe.totle_EQ,0);
                        for(p=0;p<256;p++)
                            ptr[p] = (data_tmp[p]>>14);
                    }
                #endif
                #if (CONFIG_DRC == 1)
                    extern DRCContext app_drc;
                    if(!app_drc.init)
                    {
                        drc_init(&app_drc,44100,2);
                    }
                    else if(app_drc.flag)
                    {
                        int16 *ptr,p;
                        ptr = (int16 *)app_sbc.sbc_output;
                        if((app_drc.flag)&(1<<0))
                        {
                            for(p=0;p<256;p++)
                                ptr[p] = f_sat(((int)ptr[p]*app_drc.pre_vol)>>14);
                        }
                        ///////
                        if((app_drc.flag)&(1<<2))
                            drc_apply(&app_drc, (int16_t*)app_sbc.sbc_output, (int16_t*)app_sbc.sbc_output, 512 / 4);
                        ///////
                        if((app_drc.flag)&(1<<1))
                        {
                            for(p=0;p<256;p++)
                            ptr[p] = f_sat(((int)ptr[p]*app_drc.post_vol)>>14);
                        }
                    }
                #endif

                    aud_fill_buffer(app_sbc.sbc_output, 512);
                #endif
                }
                //if(aud_get_buffer_size()<5)
                //aud_src_from_bt_audio(0);
            }
        }
    #if (SBC_ERR_FRM_PROC != 1)
        app_sbc.sbc_ecout -= i;
    #endif
    #ifdef CONFIG_TWS
        if(!bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
    #endif
            sbc_dac_clk_adjust();
                /*When sbc node buffer < 5 sbc frms, wait for ....*/
        if(app_sbc.sbc_ecout < 2)
            flag_sbc_buffer_play = 0;

#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
        BK3000_set_clock(1, 0);
    }
    else
    {
        BK3000_set_clock(1, 0);
    }
#endif
}

void sbc_discard_uselist_node(void)
{
    //uint32 i;
    sbc_mem_node_t *node;
    uint32 interrupts_info, mask;

    VICMR_disable_interrupts(&interrupts_info, &mask);

    //i = 0;
    do
    {
        node = sbc_mem_pool_getNode(&app_sbc.sbc_mem_pool.uselist);
        if(node)
        {
            sbc_mem_pool_freeNode(&app_sbc.sbc_mem_pool.freelist, node);
        }

        /*i ++;
        if(i > 20)
        {
            break;
        }*/
    }while(node);

    VICMR_restore_interrupts(interrupts_info, mask);
	//os_printf("node:%d\r\n",i);
}

void sbc_first_play_init(uint8 enable,uint8 cnt)
{
    os_printf("sbc_first_play_init\r\n");
    flag_sbc_buffer_play = 0;
#ifdef SBC_FIRST_DISCARD_ENABLE
    sbc_first_discrad_flag = enable;
    sbc_first_discard_count = 0;
    sbc_first_discard_data = cnt;
#endif
}
#ifdef CONFIG_TWS
int get_sbc_mem_pool_Nodes( void)
{
    sbc_mem_node_t *node;
    int i=0;
    node = app_sbc.sbc_mem_pool.uselist.head;
    while(node)
    {
        i++;
        node = node->next;
    }
    return i;

}
void app_sbc_dac_clk_adjust(void)
{
    sbc_dac_clk_adjust_tws();
}
uint8 get_sbc_clk_mode(void)
{
    return app_sbc.sbc_clk_mode;
}
void sbc_show_all_status(void)
{
    os_printf("aud_num:%d,buf:%d,eCnt:%d,clk_m:%d,0x%x",sync_data.u.sync_send.aud_num,aud_get_buffer_size(),app_sbc.sbc_ecout,app_sbc.sbc_clk_mode,BK3000_AUD_DAC_FRACCOEF);
}
void sbc_stream_start_clear(void)
{
    app_sbc.src_pkt_num = 0;
}

void sbc_src_num_oper( uint8 oper, uint8 num )
{
    if( oper == 0 ) // plus
    {
        app_sbc.src_pkt_num += num;
    }
    else if( oper == 1 )
    {
        app_sbc.src_pkt_num -= num;
        if( app_sbc.src_pkt_num <= 0 )
            app_sbc.src_pkt_num = 0;
    }

    return;
}

void sbc_src_num_set(uint8 num )
{
    app_sbc.src_pkt_num = num;
}

void sbc_notify_num( hci_link_t *link, uint8 num )
{
//    btaddr_t * saddr = &link->hl_btaddr;
//    if( btaddr_same( saddr, (btaddr_t *)app_bt_get_handle(2)) )
//    {
//        if( bt_flag2_is_set( APP_FLAG2_STEREO_STREAMING ) )
//            sbc_src_num_oper( 0, num );
//    }

    return;
}

int sbc_src_num( void )
{
    return (int8)app_sbc.src_pkt_num;
}

int sbc_check_src_num(void)
{
    if((int8)app_sbc.src_pkt_num<16)
        return 1;
    else
    {
        os_printf("sn:%d\r\n",app_sbc.src_pkt_num);
        return 0;
    }
}

#endif

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
void linein_sbc_encode_init(void)
{
    media_packet_sbc_t * pkt;
    sbc_t *sbc;
    sbc = (sbc_t *)jmalloc(sizeof(sbc_t), 0);
    if (!sbc)
        return ;
    os_printf("sbc_en:%x\n",sbc);
    sbc_init(sbc, 0,SBC_MODE);
    sbc_target_init(sbc);
    sbc_target_init_malloc_buff();
    rb_init( get_line_ringbuf(), get_linein_buffer_base(),0, get_linein_buffer_length());
//	LTX_PRT("rb_init LINE IN\r\n");

    app_sbc.sequence = 0;
    app_sbc.samples = 0;
    app_sbc.src_pkt_num = 0;

    app_sbc.in_frame_len = sbc_get_codesize(app_sbc.sbc_ptr,SBC_MODE);
    app_sbc.out_frame_len = sbc_get_frame_length(app_sbc.sbc_ptr,SBC_MODE);
    app_sbc.payload_len = L2CAP_MTU_DEFAULT - sizeof(rtp_header_t);//default mtu
    app_sbc.frames_per_packet = 8; //app_sbc.payload_len/ app_sbc.out_frame_len;
    app_sbc.sbc_output_buffer = (uint8 *)jmalloc(L2CAP_MTU_DEFAULT, 0);//		outputbuffer;

    LTX_PRT("in_len:%d,out_len:%d\r\n",app_sbc.in_frame_len,app_sbc.out_frame_len);
    if(app_sbc.sbc_output_buffer)
    {
        LTX_PRT("encode_buf ok\r\n");
        pkt = (media_packet_sbc_t*)app_sbc.sbc_output_buffer;
        memset((uint8 *)(&pkt->hdr), 0, sizeof(rtp_header_t) );

        pkt->hdr.ssrc = uw_htobe32(1);
        pkt->hdr.pt = 0x60;
        pkt->hdr.v = 2;
    }
    else
    {
        LTX_PRT("encode_buf err\r\n");
    }
//    app_sbc.sbc_output_buffer[0]=0x80;
}

void linein_sbc_alloc_free(void)
{
    sbc_t *sbc;
    sbc = app_sbc.sbc_ptr;

//	sbc_finish(sbc);
    if (sbc)
    {
        jfree(sbc->priv);
        sbc->priv = NULL;
        memset(sbc, 0, sizeof(sbc_t));
        jfree(sbc);
        jfree(app_sbc.sbc_output_buffer);
    }
    sbc_target_deinit_jfree_buff();
    app_sbc.sbc_target_initial = 0;
    app_sbc.sbc_ptr = NULL;
    bt_flag2_operate(APP_FLAG2_STEREO_LINEIN_TX, 0);

}

/*
Header + n* SBC frame
input:
		buffer: input buffer
		len:PCM DATA length
		mp: output buffer
		mp_data_len:packet length
		written:encoded data length
output:
		written: a2dp packet length
		return value: consumed PCM data length

added by beken
*/
static int	sbc_encode_mediapacket(
					uint32_t len, void *mp,
					uint32_t mp_data_len, uint32 *written)
{
    media_packet_sbc_t *mp_sbc = (media_packet_sbc_t *) mp;
    uint32_t consumed = 0;
    uint32_t encoded = 0;
    uint8_t frame_count = 0;
    int len_t;
    uint8_t *tmpbuffer = &(mp_sbc->frame_count);
    mp_data_len -= 1;//sizeof(mp_sbc->payload);
    tmpbuffer += 1;//skip
//LTX_PRT("in_len:%d,out_len:%d,write:%d\r\n",len,mp_data_len,*written);
    while (len - consumed >= app_sbc.in_frame_len &&
                mp_data_len - encoded >= app_sbc.out_frame_len &&
                frame_count < app_sbc.frames_per_packet)
                //&& aud_get_buffer_size()>=app_sbc.in_frame_len
    {
        uint32_t read;
        uint32_t written_s;
        int16 aud_tmp[256];
//		LTX_PRT("1\r\n");
        written_s =0;
        len_t = rb_get_buffer_with_length(get_line_ringbuf(), (uint8 *)aud_tmp, app_sbc.in_frame_len);
        if(len_t == 0)
            break;
//		LTX_PRT("2:%d\r\n",len_t);
        {
            int i;
            int16 *ptr_obj=(int16 *)app_sbc.sbc_output;
            for(i=0;i<128;i++)
            {
                ptr_obj[i]=aud_tmp[2*i+0];
                ptr_obj[128+i]=aud_tmp[2*i+1];
            }
        }

    #ifndef CONFIG_LINE_IN_TX_LOOP
//	if(debug_show_trace(2))
//	{
//		uint16 *ptr;
//		int m;
//		debug_set_trace_val(0);
//		ptr = (uint16 *)app_sbc.sbc_output;
//		for(m=0;m<512/4;m++)
//		LTX_PRT("%x\r\n",ptr[2*m+1]);
//	}
        line_in_fill_aud_buf(app_sbc.sbc_output,app_sbc.in_frame_len);
//		aud_fill_buffer(app_sbc.sbc_output,app_sbc.in_frame_len);
        if(bt_flag2_is_set(APP_FLAG2_STEREO_PLAYING)
		&&aud_get_buffer_size()<1024)
        {
            aud_open();
        }
    #endif
		/*
        read = sbc_encode(app_sbc.sbc_ptr,
                                        (void *)app_sbc.sbc_output,
                                        app_sbc.in_frame_len,  // 512
                                        tmpbuffer+encoded,
                                        mp_data_len - encoded, // 80+20
                                        &written_s,SBC_MODE);
        */
        /* in while(): mp_data_len - encoded >= app_sbc.out_frame_len */
        read = sbc_encode(app_sbc.sbc_ptr,(uint16_t *)&written_s,tmpbuffer+encoded,
								        app_sbc.in_frame_len,  // 512
                                        (void *)app_sbc.sbc_output,SBC_MODE);
        
        if (read < 0)
        {
            break;
        }

        frame_count++;//encoded SBC frame count
        consumed += read;//consumed PCM data len
        encoded += written_s;//encoded SBC data len
    }

    *written = encoded + 1;//sizeof(mp_sbc->payload);
    mp_sbc->frame_count = frame_count;
    return consumed;

}


result_t RAM_CODE sbc_do_encode( void )
{
    uint8 *outbuf = app_sbc.sbc_output_buffer; //?????
    uint32 free_space = app_sbc.payload_len; //??????
    uint32 read,samples,consumed=0;
    result_t err = UWE_BUSY;
	//int i;
    //uint32 input_len=LINEIN_DATA_BUFFER_LEN- rb_get_buffer_size(get_line_ringbuf());
    int32 input_len = rb_get_sample_number(get_line_ringbuf());

    if(input_len < 2*app_sbc.in_frame_len)
        return err;
    if((int8)sbc_src_num()>=20)
    {
    //	LTX_PRT("num:%d\r\n",sbc_src_num());
        return err;
    }
//	if(!app_bt_is_flag2_set(APP_FLAG2_STEREO_STREAMING)) return err;
	if(!bt_flag2_is_set(APP_FLAG2_STEREO_LINEIN_TX)) return err;

    media_packet_sbc_t *pkt = (media_packet_sbc_t*)outbuf;
    pkt->hdr.sequence_number = uw_htobe16(app_sbc.sequence);
    pkt->hdr.timestamp = uw_htobe32(app_sbc.samples);
    app_sbc.sequence++;
//LTX_PRT("sbc_do_encode\r\n");
    //while(consumed < input_len)
    {
        uint32 written = 0;
	 struct mbuf* m;

        read = sbc_encode_mediapacket(input_len-consumed, (void *)outbuf, free_space, &written);
        if(read <=0)
            return err;
        if(written > 0)
        {
            written += (12);//sizeof(rtp_header_t);
            m = m_get_flags(MT_DATA, M_PKTHDR, written);
            if(!m)
                return UWE_NOMEM;
        //for(i=0;i<outbuf[12];i++)
        //{
        //if(outbuf[13+((written-13)/outbuf[12])*i]!=0x9c)
        //	LTX_PRT("%d,%d,%x\n",written,outbuf[12],outbuf[13+((written-13)/outbuf[12])*i]);
        //}
        //os_printf("%d\n",written);
            m_copyback(m,0,written,outbuf);
        //if(sbc_check_src_num())
        //{
                a2dp_src_cmd_stream_send(m);
            //m_free(m);
        //}
        //else
        //{
            //m_free(m);
        //}
        //err = a2dp_src_stream_send(outbuf,written);
        // if(err)
            //return err;

            if(app_sbc.sbc_ptr->mode == 0x03/*SBC_MODE_JOINT_STEREO*/)
                samples = read >> 2;// /4
            else
                samples = read >> 1;// /2
            app_sbc.samples += samples;
            consumed += read;
        }
    }
    return err;
}

uint8 *get_linein_buffer_base(void)
{
    return app_sbc.sbc_encode_buffer;
}

uint32 get_linein_buffer_length(void)
{
    return 512*4; //SBC_ENCODE_BUFFER_LEN;
}
#endif




#ifdef CONFIG_SBC_PROMPT
void app_wave_file_sbc_fill(void)
{
    //	sbc_mem_node_t *node;
    static sbc_t* wave_sbc_ptr = NULL;
    static uint8_t* in_ptr;
    static uint8_t* out_ptr;
    uint8_t input_len = app_wave_get_blocksize();
    int i,decode_frms;
    int16_t ret;

    if(!app_wave_playing()||app_wave_check_type(INTER_WAV)||app_wave_check_type(EXT_WAV))
        return;

    if(wave_sbc_ptr == NULL)
    {
        wave_sbc_ptr = (sbc_t *)jmalloc(sizeof(sbc_t), 0);
        if (!wave_sbc_ptr)
        {
            return ;
        }
        os_printf("sbc_prompt:%x\n",wave_sbc_ptr);
        wave_sbc_init(wave_sbc_ptr, 0,SBC_MODE);
        //in_ptr = jmalloc(input_len,0);
        in_ptr = jmalloc(64,0);
        out_ptr = jmalloc(256,0);
    }
    decode_frms = (aud_get_buffer_size()+1)/512;
    for(i=0;i<decode_frms;i++)
    {
        if(app_wave_check_status(PROMPT_EMPTY))
        {
            if(!wave_sbc_ptr)
                return;
            if(!wave_sbc_ptr->priv)
                return;
            wave_sbc_ptr->priv = NULL;;
            jfree(wave_sbc_ptr);
            wave_sbc_ptr = NULL;
            jfree(in_ptr);
            jfree(out_ptr);            
 //           app_wave_set_status(PROMPT_STOP);
            app_wave_set_status(PROMPT_CLOSED);
            app_wave_file_play_stop();
            break;
        }
        else if(app_wave_check_status(PROMPT_FILL_ZERO))
        {
            app_wave_fill_sbc_node(in_ptr);
            memset(out_ptr,0,256);
        #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            app_wave_file_fill_buffer(out_ptr, 256);
        #else
            aud_fill_buffer( out_ptr,256);
        #endif
        }
        else
        {
            app_wave_fill_sbc_node(in_ptr);
            ret = sbc_decode(wave_sbc_ptr,in_ptr,input_len,out_ptr,256,NULL,SBC_MODE);
            if(ret < 0)
            {
                memset(out_ptr,0,256);    
            }
        #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            app_wave_file_fill_buffer(out_ptr, 256);
        #else
            aud_fill_buffer( out_ptr,256);
        #endif
        }
    }

}
/*
void app_wave_file_sbc_stop(void)
{
#ifdef BT_ONE_TO_MULTIPLE
	if(!(a2dp_has_music()|(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))))
#else
	if(!bt_flag1_is_set(  APP_FLAG_MUSIC_PLAY
						 |APP_FLAG_SCO_CONNECTION
						 ))
#endif
	{
            sbc_t *sbc;
            sbc = app_sbc.sbc_ptr;            
            sbc_target_deinit();
            sbc_finish(sbc);
	}
	flag_sbc_buffer_play = 0;
	#ifdef SBC_FIRST_DISCARD_ENABLE
		sbc_first_discrad_flag = 1;
		sbc_first_discard_count = 0;
		sbc_first_discard_data = 1; //empirical value by measured when recover from prompt wav
	#endif

	app_wave_file_play_stop();
}
*/
#endif

// EOF
