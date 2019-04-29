#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_DRIVER_I2C

//NOTE: only support 7bit addr now
#define I2C_FIFO_SIZE       16
#define I2C_BUFF_SIZE       32
#define I2C_ADDR_LENGTH    2
#define I2C_SYS_CLOCK     26000000

enum {
    I2C_STATE_START = 0,
    I2C_STATE_ADDR_RX = 1,
    I2C_STATE_DATA = 2,
    I2C_STATE_FINISH =3,
    I2C_STATE_ADDR_RX2 = 4
};

typedef uint8 I2C_STATE;

#define  I2C_MASTER_TX_INTER	(bit_I2C2_STAT_SI|bit_I2C2_STAT_MASTER|bit_I2C2_STAT_TXMODE)
#define I2C_MASTER_RX_INTER	(bit_I2C2_STAT_SI|bit_I2C2_STAT_MASTER) 
#define I2C_SLAVE_TX_INTR          (bit_I2C2_STAT_SI|bit_I2C2_STAT_TXMODE )
#define I2C_SLAVE_RX_INTR           (bit_I2C2_STAT_SI)

typedef struct _driver_i2c_s
{
    uint8           data_buff[I2C_BUFF_SIZE];
    uint8           device_addr[I2C_ADDR_LENGTH];
    uint8           data_len;
    uint8 *        data_rx_ptr;
    uint8           data_index;
    
    uint8           i2c_mode;

    i2c_reg_read_callback   read_cb;
    i2c_reg_write_callback  write_cb;
    i2c_get_reg_len           len_cb;
    uint8           i2c_slv_reg_addr;
    
    volatile uint8           i2c_busy;
    volatile I2C_STATE    i2c_sm;
}driver_i2c_t;

//#define DEBUG_I2C
static driver_i2c_t  driver_i2c;

#define     I2C_WRITE_BYTE(v)       (BK3000_I2C2_DATA= ((v)&0xff))
#define     I2C_READ_BYTE()          ((BK3000_I2C2_DATA) & 0xFF)
#define     I2C_START()                  (BK3000_I2C2_STATUS|= bit_I2C2_STAT_STA )
#define     I2C_CLEAR_START()       (BK3000_I2C2_STATUS &= ~bit_I2C2_STAT_STA )
#define     I2C_STOP()                   (BK3000_I2C2_STATUS |= bit_I2C2_STAT_STO )
#define     I2C_ACK()                     (BK3000_I2C2_STATUS |= bit_I2C2_STAT_ACK )
#define     I2C_NACK()                   (BK3000_I2C2_STATUS &= ~bit_I2C2_STAT_ACK )

void i2c_init(int freq, int m_s)
{
    uint16  freq_div = 0;

    BK3000_PMU_PERI_PWDS &= ~bit_PMU_I2C2_PWD;

    freq_div = I2C_SYS_CLOCK/freq;
    
    BK3000_I2C2_CN =( (0x3<<sft_I2C2_CN_IDLE_CR)
                      |(0x7<<sft_I2C2_CN_SCL_CR)
                      |(freq_div<<sft_I2C2_CN_FREQ_DIV)
                      |(0x16<<sft_I2C2_CN_SLV_ADDR)
                      |(0x3<<sft_I2C2_CN_SMBCS)
                      |(0x1<<sft_I2C2_CN_SMBTOE)
                      |(0x1<<sft_I2C2_CN_SMBFTE)
                      |(0x0<<sft_I2C2_CN_INH)
                      |(0x1<<sft_I2C2_CN_ENSMB) );

    if( m_s ) // 1 is master
       BK3000_I2C2_CN |= ( 0x1 << sft_I2C2_CN_INH ); 
 
    BK3000_I2C2_STATUS = 0x00000000;    

    memset( &driver_i2c, 0, sizeof(driver_i2c_t) );
    driver_i2c.i2c_mode = m_s;
    
}

void i2c_attach_callback( i2c_reg_read_callback  reg_read, 
    i2c_reg_write_callback reg_write, i2c_get_reg_len get_len )
{
    driver_i2c.read_cb = reg_read;
    driver_i2c.write_cb = reg_write;
    driver_i2c.len_cb = get_len;

    return;
}

int  i2c_busy( void )
{
    return driver_i2c.i2c_busy;
}

int  i2c_write( uint8 addr, uint8 *buff, uint8 len )
{
    if( buff == NULL || len > I2C_FIFO_SIZE )
        return  ENV_ERR_PARA;

    if( driver_i2c.i2c_busy )
        return ENV_ERR_BUSY;

    if( driver_i2c.i2c_mode ) //master
    {
        driver_i2c.device_addr[0] = addr  & 0xFF;

        memcpy( driver_i2c.data_buff, buff, len );
        driver_i2c.data_len = len;

        driver_i2c.i2c_busy = 1;
        driver_i2c.i2c_sm = I2C_STATE_DATA;
        driver_i2c.data_index = 0;

        BK3000_I2C2_STATUS &= ~( 0x3 << sft_I2C_STAT_INT_MODE );
        
        I2C_WRITE_BYTE( driver_i2c.device_addr[0] );
        I2C_START();

        return len;
    }
    else
        return ENV_ERR_PARA;
}


static void i2c_flush_fifo( void )
{
    uint32 sts = BK3000_I2C2_STATUS;
    uint8 dummy_byte;

    while( !(sts & bit_I2C2_STAT_RXFIFO_EMPTY) )
    {
        dummy_byte = I2C_READ_BYTE();
        sts = BK3000_I2C2_STATUS;
    }
}


int i2c_read( uint8 addr, uint8 *buff, uint8 len )
{
    int   ret = -3;
    
    if( buff == NULL || len == 0 )
        return  ENV_ERR_PARA;

    if( driver_i2c.i2c_busy )
        return ENV_ERR_BUSY;

    if( driver_i2c.i2c_mode ) //master
    {
        driver_i2c.device_addr[0] = (addr & 0xFF) | 0x01;

        driver_i2c.data_len = len;
        driver_i2c.data_rx_ptr = buff;
        driver_i2c.data_index = 0;

        driver_i2c.i2c_busy = 1;
        driver_i2c.i2c_sm = I2C_STATE_DATA;

        BK3000_I2C2_STATUS |= ( 0x3 << sft_I2C_STAT_INT_MODE );

        i2c_flush_fifo();
        I2C_WRITE_BYTE( driver_i2c.device_addr[0] );
        I2C_START();

        while( driver_i2c.i2c_sm != I2C_STATE_FINISH );

        driver_i2c.i2c_sm = I2C_STATE_START;  //recv finish

        return ret;
    }
    else
        return ENV_ERR_PARA;
              
}

void i2c_isr(void)
{   
    uint32 oldmask = get_spr(SPR_VICMR(0));
    uint32 sts = BK3000_I2C2_STATUS;

    set_spr(SPR_VICMR(0), oldmask & 0); 

    /* Master tx interrupt*/
    if ((sts & I2C_MASTER_TX_INTER) == I2C_MASTER_TX_INTER )   
    {
        sts &= ~bit_I2C2_STAT_STA;

        if( sts & bit_I2C2_STAT_ACK )
        {
            if( driver_i2c.i2c_sm == I2C_STATE_ADDR_RX )
            {
                driver_i2c.i2c_sm = I2C_STATE_ADDR_RX2;
                I2C_WRITE_BYTE( driver_i2c.device_addr[1] );
            }
            else if( driver_i2c.i2c_sm == I2C_STATE_ADDR_RX2 )
            {
                driver_i2c.i2c_sm = I2C_STATE_DATA;
                I2C_WRITE_BYTE( driver_i2c.device_addr[0] );
                sts |= bit_I2C2_STAT_STA;
            }
            else if( driver_i2c.i2c_sm == I2C_STATE_DATA )
            {
                int end;

                if( driver_i2c.data_index + I2C_FIFO_SIZE < driver_i2c.data_len )
                {
                    end = driver_i2c.data_index + I2C_FIFO_SIZE;
                    driver_i2c.i2c_sm = I2C_STATE_DATA;
                }
                else
                {
                    end = driver_i2c.data_len;
                    driver_i2c.i2c_sm = I2C_STATE_FINISH;
                }

                for(; driver_i2c.data_index < end; driver_i2c.data_index++)
                {   
                    I2C_WRITE_BYTE( driver_i2c.data_buff[driver_i2c.data_index] );
                }          
            }
            else if( driver_i2c.i2c_sm == I2C_STATE_FINISH )
            {
                driver_i2c.i2c_sm = I2C_STATE_START;
                driver_i2c.i2c_busy = 0;
                sts &= ~bit_I2C2_STAT_ACK;
                sts |= bit_I2C2_STAT_STO;
            }
        }
        else
        {
            driver_i2c.i2c_sm = I2C_STATE_START;
            driver_i2c.i2c_busy = 0;
            sts &= ~bit_I2C2_STAT_ACK;
            sts |= bit_I2C2_STAT_STO;
        }
        sts &= ~(bit_I2C2_STAT_SI);
    }
    else if((sts &I2C_MASTER_RX_INTER) == I2C_MASTER_RX_INTER )
    {
        if( (sts & bit_I2C2_STAT_ACK) && (sts & bit_I2C2_STAT_STA) )
        {
            sts &= ~bit_I2C2_STAT_STA;
            sts |= bit_I2C2_STAT_ACK;
        }
        else if( sts & bit_I2C2_STAT_ACKRQ )
        {
            uint8 byte = I2C_READ_BYTE();
            driver_i2c.data_rx_ptr[driver_i2c.data_index++] = byte;

        #ifdef DEBUG_I2C
            os_printf("i2c rx: %02x\r\n", byte);
        #endif
            if( driver_i2c.data_index >= driver_i2c.data_len )
            {
                sts &= ~bit_I2C2_STAT_ACK;
                sts |= bit_I2C2_STAT_STO;
                driver_i2c.i2c_busy = 0;
                driver_i2c.i2c_sm = I2C_STATE_FINISH;
            }
            else
            {
                sts |= bit_I2C2_STAT_ACK;
            }
        }
        else
        {
            driver_i2c.i2c_sm = I2C_STATE_START;
            driver_i2c.i2c_busy = 0;
            sts &= ~bit_I2C2_STAT_ACK;
            sts |= bit_I2C2_STAT_STO;
        }
        sts &= ~(bit_I2C2_STAT_SI);
    }
    else if( (sts & I2C_SLAVE_TX_INTR ) == I2C_SLAVE_TX_INTR )
    {
        if( sts & bit_I2C2_STAT_ACKRQ )
            sts |= bit_I2C2_STAT_ACK;

        if( sts & bit_I2C2_STAT_ADDR_MATCH )
        {
            driver_i2c.i2c_sm = I2C_STATE_ADDR_RX;
            BK3000_I2C2_STATUS &= ~( 0x3 << sft_I2C_STAT_INT_MODE );
        }

        if( sts & (bit_I2C2_STAT_ACKRQ|bit_I2C2_STAT_ACK))
        {
            if( driver_i2c.read_cb )
            {
                int i, len;

                len = driver_i2c.read_cb( driver_i2c.i2c_slv_reg_addr, driver_i2c.data_buff[0],
                    I2C_FIFO_SIZE );

                for( i = 0; i < len; i++ )
                    BK3000_I2C2_DATA = driver_i2c.data_buff[i];
            }
            else
                BK3000_I2C2_DATA = 0xFF;
        }

        if( sts & bit_I2C2_STAT_STO )
        {   
            driver_i2c.i2c_sm = I2C_STATE_START;
        }
            
        sts &= ~(bit_I2C2_STAT_SI);
    }
    else if( (sts & I2C_SLAVE_RX_INTR ) == I2C_SLAVE_RX_INTR )
    {
        if( sts & bit_I2C2_STAT_ACKRQ )
            sts |= bit_I2C2_STAT_ACK;

        if( sts & bit_I2C2_STAT_ADDR_MATCH )
        {
            driver_i2c.i2c_sm = I2C_STATE_ADDR_RX;
            BK3000_I2C2_STATUS |= ( 0x3 << sft_I2C_STAT_INT_MODE );
        }

        if( driver_i2c.i2c_sm == I2C_STATE_ADDR_RX )
        {
            driver_i2c.i2c_sm = I2C_STATE_DATA;
            driver_i2c.data_len = 0;
            driver_i2c.i2c_slv_reg_addr = BK3000_I2C2_DATA;
        }
        else if( driver_i2c.i2c_sm == I2C_STATE_DATA )
        {
            while( !(sts & bit_I2C2_STAT_RXFIFO_EMPTY ) )
            {
                driver_i2c.data_buff[driver_i2c.data_len++] = BK3000_I2C2_DATA;
                if( driver_i2c.data_len >= driver_i2c.len_cb()  
                    || driver_i2c.data_len >= I2C_BUFF_SIZE )
                    driver_i2c.write_cb( driver_i2c.i2c_slv_reg_addr,
                        driver_i2c.data_buff, driver_i2c.data_len );
            }
        }

        if( sts & bit_I2C2_STAT_STO )
            driver_i2c.i2c_sm = I2C_STATE_START;
        
    }
    if(sts & bit_I2C2_STAT_SCL_TMOT)
    {
        BK3000_I2C2_STATUS = sts&(~bit_I2C2_STAT_SCL_TMOT);//clear
        BK3000_I2C2_CN = BK3000_I2C2_CN&0x7fffffff;
        BK3000_I2C2_CN = BK3000_I2C2_CN|0x80000000;
    }

    if(sts & bit_I2C2_STAT_ARBLOST)
    {
        BK3000_I2C2_STATUS = sts&(~bit_I2C2_STAT_ARBLOST);//clear
        BK3000_I2C2_CN = BK3000_I2C2_CN&0x7fffffff;
        BK3000_I2C2_CN = BK3000_I2C2_CN|0x80000000;
    }

    BK3000_I2C2_STATUS = sts;
    set_spr(SPR_VICMR(0), oldmask); 
}

#endif
