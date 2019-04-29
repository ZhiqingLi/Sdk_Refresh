/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "includes.h"
#include "bk3000_reg.h"
#include "tra_hcit.h"
#include "tc_const.h"
#include "app_beken_includes.h"
#if (CONFIG_DRC == 1)
#include "app_drc.h"
#endif
#ifdef CONFIG_PRODUCT_TEST_INF
#include "driver_icu.h"
#endif

#if PTS_TESTING
#include "pts/pts.h"
#endif

#if (DEBUG_BASEBAND_MONITORS == 1)
extern void TC_Read_Local_Baseband_Monitors_Via_App(void);
extern void LSLCstat_Reset_Monitors();
#endif
#if (DEBUG_SCATTERNET_MONITORS == 1)
extern void TC_Read_Local_LMPconfig_Monitors_Via_App(void);
#endif

extern void TRAhcit_Rx_Char(u_int8 ch);
extern void juart_receive(u_int8 *buf, u_int16 size);
#ifdef BEKEN_DEBUG
extern void app_env_dump(void);
#endif
extern void app_print_linkkey(void);
extern u_int32 XVR_analog_reg_save[16];

unsigned char uart_rx_buf[RX_FIFO_THRD]={0};
unsigned char uart_tx_buf[TX_FIFO_THRD]={0};
volatile boolean uart_rx_done = FALSE;
volatile unsigned int uart_rx_index = 0;
HCI_COMMAND_PACKET *pHCIrxBuf = (HCI_COMMAND_PACKET *)(&uart_rx_buf[0]);
HCI_EVENT_PACKET   *pHCItxBuf = (HCI_EVENT_PACKET *)(&uart_tx_buf[0]);
#ifdef	CONFIG_BLUETOOTH_AVDTP_SCMS_T
extern void security_control_cp_support_print(void);  //print the array's param for check
#endif

enum
{
    DBG_HCI_STATE_RX_TYPE = 0,
    DBG_HCI_STATE_RX_COMMAND_OPCODE1,
    DBG_HCI_STATE_RX_COMMAND_OPCODE2,
    DBG_HCI_STATE_RX_COMMAND_LENGTH,
    DBG_HCI_STATE_RX_DATA_START,
    DBG_HCI_STATE_RX_DATA_CONTINUE,
    DBG_HCI_STATE_RX_DATA_COMMIT
};
//u_int8 dbg_hci_rx_buf[255] = {0};
static u_int8 s_dbg_hci_rx_state;
static volatile u_int8* s_dbg_hci_rx_pdu_buf = (u_int8 *)&uart_rx_buf[4];
static volatile u_int16 s_dbg_hci_rx_pdu_length;
static u_int8 s_dbg_hci_rx_length;
u_int8* s_dbg_hci_rx_head_buf = (u_int8 *) &uart_rx_buf[0];

extern void show_bt_stack_status(void);
#ifdef BEKEN_UP_OTA_3266
extern int app_button_ota_enable_action( void );
#endif

/*
 * uart_initialise
 *
 * This function initialises the UART registers & UART driver paramaters.
 */
void uart_initialise(u_int32 baud_rate) {
    u_int32 baud_divisor;

    baud_divisor           = UART_CLOCK_FREQ_26M/baud_rate;
    baud_divisor           = baud_divisor-1;

    REG_APB3_UART_CFG      = (   (DEF_STOP_BIT    << sft_UART_CONF_STOP_LEN)
                                 | (DEF_PARITY_MODE << sft_UART_CONF_PAR_MODE)
                                 | (DEF_PARITY_EN   << sft_UART_CONF_PAR_EN)
                                 | (DEF_DATA_LEN    << sft_UART_CONF_UART_LEN)
                                 | (baud_divisor    << sft_UART_CONF_CLK_DIVID)
                                 | (DEF_IRDA_MODE   << sft_UART_CONF_IRDA)
                                 | (DEF_RX_EN       << sft_UART_CONF_RX_ENABLE)
                                 | (DEF_TX_EN       << sft_UART_CONF_TX_ENABLE));

    REG_APB3_UART_FIFO_THRESHOLD = ((RX_FIFO_THRD << sft_UART_FIFO_CONF_RX_FIFO)
                                    | (TX_FIFO_THRD << sft_UART_FIFO_CONF_TX_FIFO));
    REG_APB3_UART_INT_ENABLE=0;             /* Disable UART Interrupts */
    REG_APB3_UART_INT_ENABLE = bit_UART_INT_RX_NEED_READ | bit_UART_INT_RX_STOP_END; //enable Rx interrupt
    BK3000_GPIO_0_CONFIG = 0x70;
    BK3000_GPIO_1_CONFIG = 0x7C;
}

void uart_send (unsigned char *buff, unsigned int len) {
    
#ifdef CONFIG_APP_DATA_CAPTURE

	if( !app_aec_get_data_capture_mode() ) {
		while (len--) {
			while(!UART_TX_WRITE_READY);
                UART_WRITE_BYTE(*buff++);
		}
	}
    
#else

    while (len--) {
        while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(*buff++);
    }
    
#endif

}

#if (CONFIG_DEBUG_PCM_TO_UART == 1)
void uart_send_ppp(unsigned char *buff, unsigned char fid,unsigned short len)
{
    unsigned char xsum = 0;
    unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.

    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(0x7E);
    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(fid);
    xsum ^= fid;
    while (len--)
    {
        xsum ^= *buff;
        if(0x7E == *buff)
        {   while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x7D);
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x5e);
        }
        else if(0x7D == *buff)
        {
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x7D);
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(0x5D);
        }
        else
        {
            while(!UART_TX_WRITE_READY);
            UART_WRITE_BYTE(*buff);
        }
        buff++;
    }
    if(0x7E == xsum)
    {   while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x7D);
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x5e);
    }
    else if(0x7D == xsum)
    {
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x7D);
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(0x5D);
    }
    else
    {
        while(!UART_TX_WRITE_READY);
        UART_WRITE_BYTE(xsum);
    }
    while(!UART_TX_WRITE_READY);
    UART_WRITE_BYTE(0x7E);
    set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.

}
#endif
#define PRINT_BUF_PREPARE(rc, buf, fmt)             \
    int rc;                                         \
    va_list args;                                   \
    va_start(args, fmt);                            \
    rc = vsnprintf(buf, sizeof(buf), fmt, args);    \
    va_end(args);                                   \
    buf[sizeof(buf) - 1] = '\0';
#define PRINT_BUF_SIZE 0X100
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
static int32 os_printf_cnt = 0;
int32 os_printf(const char *fmt, ...) {
    char buf[PRINT_BUF_SIZE];

    unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.
#if 1
    if(os_printf_cnt < 1)
    {
        PRINT_BUF_PREPARE(rc, buf, fmt);
        uart_send((unsigned char *)&buf[0], rc);
    }
    else
    {
        delay_us(1);
    }
    os_printf_cnt++;
#endif
    set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return 0;
}
#else
int32 os_printf(const char *fmt, ...) {
#if (BT_HOST_MODE == JUNGO_HOST)
    char buf[PRINT_BUF_SIZE];

    //unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    //set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.

    PRINT_BUF_PREPARE(rc, buf, fmt);
    uart_send((unsigned char *)&buf[0], rc);

    //set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return rc;
 #else
    //unsigned int oldmask = get_spr(SPR_VICMR(0));    //read old spr_vicmr
    //set_spr(SPR_VICMR(0), 0x00);                     //mask all/low priority interrupt.
    delay_us(10);
    //set_spr(SPR_VICMR(0), oldmask);                  //recover the old spr_vicmr.
    return 0;
 #endif
}
#endif
int32 os_null_printf(const char *fmt, ...) {
    return 0;
}

void clear_uart_buffer(void) {
    uart_rx_index = 0;
    uart_rx_done = FALSE;
    memset(uart_rx_buf, 0, sizeof(uart_rx_buf)); /**< Clear the RX buffer */
    memset(uart_tx_buf, 0, sizeof(uart_tx_buf)); /**< Clear the TX buffer */
}

void Beken_Uart_Tx(void) {
    unsigned int tx_len       = HCI_EVENT_HEAD_LENGTH+pHCItxBuf->total;
    pHCItxBuf->code  = TRA_HCIT_EVENT;
    pHCItxBuf->event = HCI_COMMAND_COMPLETE_EVENT;
    uart_send(uart_tx_buf, tx_len);
}

static void app_debug_showstack(void) {
    extern uint32 _sbss_end;
    extern uint32 _stack;
    uint32 count;
    uint32 *ptr;
    uint32 i;

    count = (((uint32)&_stack - (uint32)&_sbss_end) >> 2) - 2;
    ptr = (uint32 *)((uint32)&_sbss_end  & (~3));

    os_printf("ShowStack:%p:%p\r\n",  &_sbss_end, &_stack);
    for(i = 0; i < count; i ++)
        os_printf("0x%x:%p\r\n", &ptr[i], ptr[i]);
}

#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
extern void _LSLCacc_Read_AGC_Param(void);
#endif
extern void hfp_app_ptr_debug_printf(void);

extern int hf_sco_handle_process(int oper);
#define HF_SCO_CONN 0
#define HF_SCO_DISCONN 1

//extern void app_bt_sdp_connect(void);
extern void app_led_dump(void);
#ifdef CONFIG_APP_EQUANLIZER
extern void app_set_eq_gain(int8 gain);
#endif
extern void set_sdp_browse_on(void);
extern result_t sdp_send_serviceSearchRequest(void);
extern void app_bt_sdp_send_serviceAttributeRequest(void);
extern uint32 XVR_reg_0x24_save;

RAM_CODE void Beken_Uart_Rx(void) {
    if ((uart_rx_done != TRUE) || (uart_rx_index == 0))
        return;
#ifdef BEKEN_DEBUG
    #if 0
	os_printf("Beken_Uart_Rx %x,%x,%x,%x,%x,%x,%x\r\n",
		pHCIrxBuf->code,
		pHCIrxBuf->opcode.ogf,
		pHCIrxBuf->opcode.ocf,
		pHCIrxBuf->total,
		pHCIrxBuf->cmd,
		pHCIrxBuf->param[0],
		uart_rx_index);
    #endif
#endif
 loop_mode:
    //os_delay_ms(10);

    //os_printf("code=0x%x, ogf=0x%x, ocf=0x%x, count=0x%x\r\n",pHCIrxBuf->code,pHCIrxBuf->opcode.ogf,pHCIrxBuf->opcode.ocf,uart_rx_index);
    //os_printf("total=%d\r\n", pHCIrxBuf->total);
    
#if PTS_TESTING
    if ( (pHCIrxBuf->code       == TRA_HCIT_COMMAND)
      && (pHCIrxBuf->opcode.ogf == PTS_TESTING_OGF)
      && (pHCIrxBuf->opcode.ocf == BEKEN_OCF)
      && (uart_rx_index         == (HCI_COMMAND_HEAD_LENGTH+pHCIrxBuf->total))) {
       /*******************************************
        * uart cmd: 0x01 e0 40 01 10 
        *                         10~4F for hfp
        *                         50~8F for a2dp
        *******************************************/
        pts_entry(pHCIrxBuf->cmd);      
        goto ret;
    }    
#endif

    if (   (pHCIrxBuf->code       != TRA_HCIT_COMMAND)
        || (pHCIrxBuf->opcode.ogf != VENDOR_SPECIFIC_DEBUG_OGF)
        || (pHCIrxBuf->opcode.ocf != BEKEN_OCF)
        || (uart_rx_index         != (HCI_COMMAND_HEAD_LENGTH+pHCIrxBuf->total)))
        goto ret;

// 01E0FC.len.cmd.para1.para2....// len from cmd, hex fomate send
    /* os_printf("cmd=%x\r\n",pHCIrxBuf->cmd); */
    switch (pHCIrxBuf->cmd) {
    case BEKEN_UART_LINK_CHECK:
        pHCItxBuf->total = uart_rx_index;
        memcpy(pHCItxBuf->param, uart_rx_buf, pHCItxBuf->total);
        break;

    case BEKEN_UART_REGISTER_WRITE_CMD: {
        int reg_index;
        REGISTER_PARAM *rx_param        = (REGISTER_PARAM *)pHCIrxBuf->param;
        REGISTER_PARAM *tx_param        = (REGISTER_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];

        pHCItxBuf->total                = uart_rx_index-1;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        tx_param->addr                  = rx_param->addr;
        tx_param->value                 = rx_param->value;
        *(volatile unsigned int *)rx_param->addr = rx_param->value;
        reg_index                       = (rx_param->addr-BK3000_XVR_BASE_ADDR)/4;
        if ((reg_index>=0) && (reg_index<=0x0f))
            XVR_analog_reg_save[reg_index] = rx_param->value;
        if(reg_index == 0x24)
            XVR_reg_0x24_save = rx_param->value;
		//#ifdef BEKEN_DEBUG
        // os_printf("addr=%x,val=%x\r\n",rx_param->addr, rx_param->value);
		//#endif
        break;
    }

    case BEKEN_UART_REGISTER_READ_CMD: {
        int reg_index;
        REGISTER_PARAM *rx_param = (REGISTER_PARAM *)pHCIrxBuf->param;
        REGISTER_PARAM *tx_param = (REGISTER_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];

        pHCItxBuf->total         = HCI_EVENT_HEAD_LENGTH+uart_rx_index;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]      = pHCIrxBuf->cmd;
        tx_param->addr           = rx_param->addr;
        reg_index                = (rx_param->addr-BK3000_XVR_BASE_ADDR)/4;
        if ((reg_index>=0) && (reg_index<=0x0f))
            tx_param->value        = XVR_analog_reg_save[reg_index];
        else
            tx_param->value        = *(volatile unsigned int *)rx_param->addr;
		//#ifdef BEKEN_DEBUG
        // os_printf("addr=%x,val=%x\r\n",tx_param->addr, tx_param->value);
		//#endif
        break;
    }

    case BEKEN_FLASH_READ_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total         = HCI_COMMAND_HEAD_LENGTH+sizeof(FLASH_PARAM)+rx_param->len;
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]      = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_read_data(tx_param->data, tx_param->addr, tx_param->len);
        break;
    }

    case BEKEN_FLASH_WRITE_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total                = HCI_COMMAND_HEAD_LENGTH+sizeof(FLASH_PARAM);
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_write_data(rx_param->data, rx_param->addr, rx_param->len);
        break;
    }

    case BEKEN_FLASH_ERASE_CMD: {
        FLASH_PARAM *rx_param = (FLASH_PARAM *)pHCIrxBuf->param;
        FLASH_PARAM *tx_param = (FLASH_PARAM *)&pHCItxBuf->param[HCI_COMMAND_HEAD_LENGTH];
        pHCItxBuf->total                = HCI_COMMAND_HEAD_LENGTH+sizeof(tx_param->addr);
        memcpy(pHCItxBuf->param, uart_rx_buf, HCI_EVENT_HEAD_LENGTH);
        pHCItxBuf->param[3]             = pHCIrxBuf->cmd;
        memcpy((unsigned char *)tx_param, (unsigned char *)rx_param, sizeof(FLASH_PARAM));
        flash_erase_sector(rx_param->addr);
        break;
    }

    case BEKEN_SHOW_STACK_CMD:
        app_debug_showstack();
        goto ret;

    case BEKEN_DUMP_ENV_CMD:
#ifdef BEKEN_DEBUG
		//01E0FC05AB00000000
        app_env_dump();
#endif
        goto ret;
    case BEKEN_CLEAR_LINKKEY_CMD:
 	flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
 	break;
    case LOOP_MODE_CMD: {
        clear_uart_buffer();
        while(uart_rx_done == FALSE);
        goto loop_mode;
    }

    case BEKEN_SHOW_BT_STATUS:
        app_bt_status_show();
	#if 0//def CONFIG_BLUETOOTH_AVDTP_SCMS_T
        security_control_cp_support_print();
	#endif
        goto ret;

#ifdef BEKEN_DEBUG
    case BEKEN_SHOW_BT_DEBUG:
        app_bt_debug_show();
        goto ret;
	case BEKEN_LED_EQ_BUTTON:
        if(pHCIrxBuf->param[0]==0xff)
        {
        //app_led_dump();
        }
#ifdef CONFIG_APP_EQUANLIZER
        else if(pHCIrxBuf->param[0]==0xfe)
        app_set_eq_gain((int8)(pHCIrxBuf->param[1]));
    #if (CONFIG_PRE_EQ == 1)
        else if(pHCIrxBuf->param[0]==0xfa)
            app_set_pre_eq(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xfb)
            app_show_pre_eq();
        else if(pHCIrxBuf->param[0]==0xfc)
            app_set_pre_eq_gain(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xfd)
            bt_flag2_operate(APP_FLAG2_SW_MUTE,pHCIrxBuf->param[1]);
    #if (CONFIG_DRC == 1)
        else if(pHCIrxBuf->param[0]==0xe1)
            app_set_drc_para(&pHCIrxBuf->param[1]);
        else if(pHCIrxBuf->param[0]==0xeb)
            app_show_drc_para();
    #endif

    #endif
#endif
        //else
        //    app_button_sw_action(pHCIrxBuf->param[0]);
		goto ret;

#ifdef CONFIG_APP_AEC
	case BEKEN_SET_AEC_PARA:
        app_set_aec_para(&pHCIrxBuf->param[0]);
		goto ret;
#endif
#endif

    case BEKEN_PRINT_LINK_KEY:
        app_print_linkkey();
        goto ret;
#if 1
    case BEKEN_ENTRY_DUT_MODE:
	 //01E0FC05AF00000000
        //app_bt_enable_dut_mode(pHCIrxBuf->param[0]);
        goto ret;
    case BEKEN_ENTRY_FCC_TESTMODE:
//        app_bt_enable_fcc_mode(pHCIrxBuf->param[0],pHCIrxBuf->param[1]);
    #ifdef BEKEN_UP_OTA_3266
        app_button_ota_enable_action();
    #endif
        goto ret;
#endif
#if (DEBUG_BASEBAND_MONITORS == 1)
    case BEKEN_READ_BASEBAND_MONITORS:{
        TC_Read_Local_Baseband_Monitors_Via_App();
        goto ret;
    }
    case BEKEN_RESET_BASEBAND_MONITORS:{
        LSLCstat_Reset_Monitors();
        goto ret;
    }
#endif

#if (DEBUG_SCATTERNET_MONITORS == 1)
    case BEKEN_READ_SCATTERNET_MONITORS:
    {
        TC_Read_Local_LMPconfig_Monitors_Via_App();
        goto ret;
    }
#endif


#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
    case BEKEN_AGC_MODE_PARAM:
        _LSLCacc_Read_AGC_Param();
        goto ret;
#endif
#endif

    case BEKEN_TEMP_CMD: {
        os_printf("BEKEN_TEMP_CMD\r\n");
        hfp_app_ptr_debug_printf();
        goto ret;
    }
#ifdef CONFIG_PRODUCT_TEST_INF
    case BEKEN_RSSI_CMD: {
	os_printf("rssi:0x%d,freqoffset:0x%x\r\n",aver_rssi,aver_offset);
	goto ret;
    }
#endif

#ifdef CONFIG_TWS
    case BEKEN_TWS: {
        app_tws_debug(&pHCIrxBuf->param[0]);
        break;
    }
#endif

    case BEKEN_SHOW_SYSTEM_INFO: {
        //os_printf("BEKEN_SHOW_SYSTEM_INFO.\r\n");
        show_bt_stack_status();
        goto ret;
    }

    case BEKEN_CMD_SDP_CONNECT: {
        set_sdp_browse_on();
        //app_bt_sdp_connect();
        goto ret;
    }
    
    case BEKEN_CMD_SERVICE_SEARCH_REQUEST: {
        sdp_send_serviceSearchRequest();
        goto ret;
    }
    
    case BEKEN_CMD_SERVICE_ATTRIBUTE_REQUEST: {
        app_bt_sdp_send_serviceAttributeRequest();
        goto ret;
    }
    
    case BEKEN_CMD_A2DP_SSA_REQUEST: {
        /* for a2dp sdp_query request. */
        extern result_t send_a2dp_ssa_request(void);
        send_a2dp_ssa_request();
        goto ret;
    }
    
    case BEKEN_CMD_HFP_SSA_REQUEST: {
        /* for hfp sdp_query request. */
       // extern result_t send_hfp_ssa_request(void);
       // send_hfp_ssa_request();
        goto ret;
    }
    case BEKEN_CMD_SYS_RESET:
    {
        /* watchdog reset for uart download */
        if(pHCIrxBuf->total >= 5)
        {
            uint32 param = pHCIrxBuf->param[0]<<24 | pHCIrxBuf->param[1]<<16 | pHCIrxBuf->param[2]<<8 | pHCIrxBuf->param[3];
            if(param == 0x95279527) 
            {
                /* watchdog reset */
                BK3000_WDT_CONFIG = 0x5A0001;
                BK3000_WDT_CONFIG = 0xA50001;
            }
        }
        goto ret;
    }
        
    default:
        goto ret;
    }

    Beken_Uart_Tx();

 ret:
    clear_uart_buffer();
}
void dbg_hci_rx_char(u_int8 ch)
{
    switch(s_dbg_hci_rx_state)
    {
        case DBG_HCI_STATE_RX_DATA_CONTINUE:
__DBG_HCI_STATE_RX_DATA_CONTINUE:
            *s_dbg_hci_rx_pdu_buf = ch;
            s_dbg_hci_rx_pdu_buf++;
            s_dbg_hci_rx_pdu_length--;
            if(!s_dbg_hci_rx_pdu_length)
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_COMMIT;
                goto __DBG_HCI_STATE_RX_DATA_COMMIT;
            }
            return;

        case  DBG_HCI_STATE_RX_TYPE:
            if(ch == 0x01)   // HCI_COMMAND
            {
                s_dbg_hci_rx_head_buf[0] = ch;
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_OPCODE1;
            }
            else
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_TYPE;
                uart_rx_index = 0;
                uart_rx_done = FALSE;
            }
            return;

        case DBG_HCI_STATE_RX_COMMAND_OPCODE1:
            s_dbg_hci_rx_head_buf[1] = ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_OPCODE2;
            return;

        case DBG_HCI_STATE_RX_COMMAND_OPCODE2:
            s_dbg_hci_rx_head_buf[2] = ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_COMMAND_LENGTH;
            return;

        case DBG_HCI_STATE_RX_COMMAND_LENGTH:
            s_dbg_hci_rx_head_buf[3] = ch;
            s_dbg_hci_rx_pdu_length = ch;
            s_dbg_hci_rx_length = 4 + ch;
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_START;
            
            if (s_dbg_hci_rx_pdu_length==0)
            {
                goto __DBG_HCI_STATE_RX_DATA_START;
            }
            return;

        case DBG_HCI_STATE_RX_DATA_START:
__DBG_HCI_STATE_RX_DATA_START:
            s_dbg_hci_rx_pdu_buf = (volatile u_int8*) &uart_rx_buf[4];
            if((s_dbg_hci_rx_pdu_length == 0) || (s_dbg_hci_rx_pdu_length >59))    
            {
                s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_COMMIT;
                goto __DBG_HCI_STATE_RX_DATA_COMMIT;
            }
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_DATA_CONTINUE;
            goto __DBG_HCI_STATE_RX_DATA_CONTINUE;

        case DBG_HCI_STATE_RX_DATA_COMMIT:
__DBG_HCI_STATE_RX_DATA_COMMIT:
            uart_rx_done = TRUE;
            uart_rx_index = s_dbg_hci_rx_length;
            #if 0
            {
                u_int8 i;
                os_printf("===hci_dbg:\r\n");
                for(i=0;i<s_dbg_hci_rx_length;i++)
                {
                    os_printf("%02x,",uart_rx_buf[i]);
                }
                os_printf("===\r\n");
            }
            #endif
            Beken_Uart_Rx();
            s_dbg_hci_rx_state = DBG_HCI_STATE_RX_TYPE;
            return;
        default:    
            return;
    }
    return;
}
void uart_handler(void) {
    u_int32 status;
    u_int8 value;

    status = REG_APB3_UART_INT_FLAG;
    if(status & (bit_UART_INT_RX_NEED_READ|bit_UART_INT_RX_STOP_END)) {
        while (UART_RX_READ_READY) {
            value=UART_READ_BYTE();
            if (BT_HOST_MODE == NONE_CONTROLLER)
                juart_receive(&value, 1);
            else if ((BT_HOST_MODE == JUNGO_HOST)) {
                #if 0
                uart_rx_buf[uart_rx_index++] = value;
                if (uart_rx_index == RX_FIFO_THRD)
                    uart_rx_index = 0;
                uart_rx_done = TRUE;;
                #endif
                dbg_hci_rx_char(value);
            } else
                TRAhcit_Rx_Char(value);
        }
    }
    REG_APB3_UART_INT_FLAG = status;
}
