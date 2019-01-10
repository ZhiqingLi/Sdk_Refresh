#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"
#include "bes_hal_i2c.h"
#include "bes_hal_uart.h"
#include "assert.h"
//#include "app_ota.h"

#ifdef VOICE_INPUT_MODULE_CX20921
#include "conexant_mode.h"
#include "conexant_cx20921.h"
#include "conexant_firmware.h"
#include "conexant_cmd.h"


int g_bIsRenoDev = 1;
int g_is_dual_img = 1;
uint32_t g_sfs_version  = 0x00030000;

#define IMG_BLOCK_SIZE   (2048)
#define MAX_RETRIES          3

#define IMG_BLOCK_SIZE_RENO  4096
#define MAX_RETRIES          3
#define _SFS_VERSION(a,b,c,d)	(((a)<<24)|((b)<<16)|((c)<<8)|((d)<<0))

#define BADBEFF  0X8BADD00D
#define READY_POOLING_INTERVAL 10 /*10 ms*/
#define READY_POOLING_LOOP (2000/READY_POOLING_INTERVAL) /*2 s*/
#define READY_POOLING_LOOP_MANUALLY (10000/READY_POOLING_INTERVAL) /*10 s*/

/*
set S  [binary format i 83]; # device->host: standing by for next packet
set T  [binary format i 84]; # device->host: bad packet. retry.
set F  [binary format i 70]; # device->host: fatal error

set R  [binary format i 82]; # host->device: packet transfer complete
set D  [binary format i 68]; # host->device: file transfer complete
set A  [binary format i 65]; # host->device: aborting transfer
set C  [binary format i 67]; # host->device: packet transfer complete
*/
enum { 
    TRANSFER_STATE_COMPLETE       = 'C',  //host->device: packet transfer complete. For bootloader.  //67
    TRANSFER_STATE_ABORT_TRANSFER = 'A',  // # host->device: aborting transfer  //65
    TRANSFER_STATE_FILE_DONE      = 'D',  //# host->device: file transfer complete //68
    TRANSFER_STATE_PACKET_READY   = 'R',  //# host->device: packet transfer complete  //82
    TRANSFER_STATE_FATAL          = 'F',  //# device->host: fatal error  //70
    TRANSFER_STATE_BAD_PACKET     = 'T',  //# device->host: bad packet. retry  //84
    TRANSFER_STATE_STANDBY        = 'S',  //#  device->host: standing by for next packet.  //83
    TRANSFER_STATE_TRANSFER_COMPLETE = 'O',  //#  device->host: Trnasfer complete, report CRC  //79
    TRANSFER_STATE_RENO_LOADER_READY = 0x80u,
    TRANSFER_STATE_CLOSED_BY_DEVICE = 'X',  //#  device->host: Download window has been closed by device  //88
    TRANSFER_STATE_BAD_DEVICE = 'Z',  //#  device->host: Bad data reported by device  //90
}; 

struct i2c_xfer_s
{
    int32_t     xfer_flag;
    uint32_t    block_index;
    uint32_t    check_sum;
} __attribute__((packed));


struct i2c_xfer_reno_s
{
  struct i2c_xfer_s   xfer;
  unsigned int        num_dword; // unit is dword.
}  __attribute__((packed));

#if 0
static int32_t TransferData(uint8_t* pImg, uint32_t image_size)
{
    uint8_t done = 0;
    i2c_xfer_s i2c_xfer;
    int32_t err_no = 0;
    uint32_t readAddr = 0;
    uint32_t transferSize = 0, downloadLen = 0;
    uint32_t blockIndex = 0;
    uint32_t i2cRet = 0;
    uint32_t nRetry = 0;

    CONEXANT_CX20921_DEBUG(" %s : data addr = %p  size = %d ", __func__, pImg, image_size);

    for(;;)
    {
        uint32_t state = 0;
        i2cRet = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x00,2,(uint8_t*)&state,sizeof(uint32_t));
        if(i2cRet != CXERR_NOERR)
        {
            CONEXANT_CX20921_ERROR(" %s :i2c read reg00 failed ", __func__);
            err_no = CXERR_I2C_ERROR;
            break;
        }

        switch(state)
        {
            case TRANSFER_STATE_FATAL://# device->host: fatal error
                CONEXANT_CX20921_ERROR(" %s :FATAL error happened. aborting. ", __func__);
                err_no = CXERR_STATE_FATAL;
                break;
            case TRANSFER_STATE_BAD_PACKET://# device->host: bad packet. retry
            {
                if( nRetry >= MAX_RETRIES )
                {
                    uint32_t val = TRANSFER_STATE_ABORT_TRANSFER;
                    bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0x00000,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&val,sizeof(uint32_t));
                    CONEXANT_CX20921_ERROR(" %s :aborting. exceeded max number of retries ", __func__);
                    err_no = CXERR_LOAD_IMG_TIMEOUT;
                    break;
                }
                else
                {
                    // write the image data through i2c for the begin to end
                    cx20921_i2c_transmit((const uint8_t*)&i2c_xfer,sizeof(i2c_xfer),(const uint8_t*)readAddr+downloadLen,transferSize );
                    nRetry++;

                }
            }
            break;
            case TRANSFER_STATE_STANDBY://#  device->host: standing by for next packet.
            {
                uint32_t checkSum = 0, count = 0;
                
                nRetry =0;

                if(downloadLen == image_size)  
                {   
                    uint32_t val = TRANSFER_STATE_FILE_DONE;
                    //LOG((DBG_INFO "Image transfer completed successfully. "));
                    bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0x0000,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)&val,sizeof(uint32_t));
                    break;
                }

                if(downloadLen + IMG_BLOCK_SIZE <= image_size)
                {
                    transferSize = IMG_BLOCK_SIZE;
                }   
                else
                {
                    transferSize = image_size - downloadLen;
                }             

                // get the checksum of this block data.
                checkSum = 0;
                for( count = 0; count*sizeof(uint32_t) < transferSize; count++)
                {
                    checkSum+= *((uint32_t*)((uint8_t*)readAddr+downloadLen) + count);
                }

                i2c_xfer.xfer_flag   = TRANSFER_STATE_PACKET_READY;
                i2c_xfer.block_index = blockIndex;
                i2c_xfer.check_sum   = checkSum;

                cx20921_i2c_transmit((const uint8_t*)&i2c_xfer,sizeof(i2c_xfer),(const uint8_t*)readAddr+downloadLen,transferSize );
                downloadLen += transferSize;
                blockIndex++;
            }
            break;
            case TRANSFER_STATE_TRANSFER_COMPLETE://#  device->host: transfer complete
            {
                uint8_t tmpMagicRet[4] = {0};
                uint32_t tmpMagicNum = 0;
     
                err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 8,CX20921_I2C_REG_ADDR_LEN,tmpMagicRet,4);
                tmpMagicNum = tmpMagicRet[0];
                tmpMagicNum = (tmpMagicNum<<8) | tmpMagicRet[1];
                tmpMagicNum = (tmpMagicNum<<8) | tmpMagicRet[2];
                tmpMagicNum = (tmpMagicNum<<8) | tmpMagicRet[3];
                if( err_no == CXERR_NOERR)
                {
                    if( tmpMagicNum != 0x53435243 )
                    {
                        CONEXANT_CX20921_DEBUG("MAGIC NUM COMPAREerror!");
                        err_no = CXERR_CRC_CHECK_ERROR;
                    }
                    else
                    {
                        CONEXANT_CX20921_DEBUG("Image transfer completed successfully. ");
                        done =1;
                    }
                }
                else
                {
                    CONEXANT_CX20921_DEBUG("Read MAGIC NUM I2C read error!");
                }
            }
            break;
            case TRANSFER_STATE_PACKET_READY:/*echo*/
            case TRANSFER_STATE_FILE_DONE:/*echo*/
            break;  
            default:
                err_no = -CXERR_FAILED;
            break;
        }

        if( err_no !=CXERR_NOERR || done ) 
            break;
    }
    return err_no;
}

#else 
int TransferData( const unsigned char * image_data, unsigned int image_size)
{

  int err_no = CXERR_NOERR;
  const unsigned char *pBlock;
  const unsigned char *pCur; 
  const unsigned char  *pBlockEnd = NULL;
  uint32_t  ulBlockCheckSum  = 0;
  uint32_t  ulBlockIndex     = 0;
  uint32_t  ulRemainder       = 0;
  uint32_t  nRetry           = 0;
  uint32_t  done             = 0;
  uint32_t  CRCReturned      = 0;
  struct i2c_xfer_reno_s          i2c_xfer_reno;
  struct i2c_xfer_s               i2c_xfer;


  uint32_t  max_xfer_size =   IMG_BLOCK_SIZE;
  uint32_t  val; 


  int  ReadErr = CXERR_NOERR;
  //
  // Download FW image data.
  //
  if( g_bIsRenoDev )
  {
    max_xfer_size = IMG_BLOCK_SIZE_RENO;
  }
  ulBlockIndex=0;
  pBlock = image_data;
  ulRemainder = image_size;
  for(;;){

    //uint32_t state = i2c_sub_read(0x00,&ReadErr); 
    uint32_t state = 0;
    ReadErr = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x00,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&state,4);
    if ( ReadErr !=CXERR_NOERR )
    {
    
        CONEXANT_CX20921_ERROR("[%s   %d] ReadErr: %d", __func__,  __LINE__, ReadErr);
        err_no = ReadErr;
        break;
    }
    switch(state)
    {
    case TRANSFER_STATE_FATAL://# device->host: fatal error
      CONEXANT_CX20921_ERROR("FATAL error happened. aborting.");
      err_no = -CXERR_STATE_FATAL;
      break;
    case TRANSFER_STATE_BAD_PACKET://# device->host: bad packet. retry
      {
        if( nRetry >= MAX_RETRIES )
        {
          //i2c_sub_write(0x0000,TRANSFER_STATE_ABORT_TRANSFER);
          val = TRANSFER_STATE_ABORT_TRANSFER;
          bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0x0000,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&val,sizeof(uint32_t));

          CONEXANT_CX20921_ERROR("aborting. exceeded max number of retries ");
          //ShowProgress(image_size-ulRemainder,0,I2C_ERR,image_size);
          err_no = -CXERR_LOAD_IMG_TIMEOUT;
          break;
        }
        else
        {
          /// re-try
          if( g_bIsRenoDev )
          {
            cx20921_i2c_transmit((unsigned char*)&i2c_xfer_reno,sizeof(i2c_xfer_reno),
                          (unsigned char*)pBlock,pBlockEnd-pBlock );
            //ShowProgress(image_size-ulRemainder,0,I2C_RETRY,image_size);
          }
          else
          {

            cx20921_i2c_transmit((unsigned char*)&i2c_xfer,sizeof(i2c_xfer),
              (unsigned char*)pBlock,pBlockEnd-pBlock );
            //ShowProgress(image_size-ulRemainder,0,I2C_RETRY,image_size);

          }
          nRetry++;

        }
      }
      break;
    case TRANSFER_STATE_STANDBY://#  device->host: standing by for next packet.
    case TRANSFER_STATE_RENO_LOADER_READY:
      {
        int  *xfer_int;
        nRetry =0;
        if(pBlockEnd != 0)
        {
          ulRemainder = image_size - (pBlockEnd -image_data);
          pBlock = pBlockEnd;
          ulBlockIndex++;
        }

        if(ulRemainder == 0)  
        {   // No remainder data, write 'D' to address 0.
          //ShowProgress(image_size,0,I2C_OK,image_size);
          CONEXANT_CX20921_DEBUG("Image transfer completed successfully.");

          //i2c_sub_write(0x0000, TRANSFER_STATE_FILE_DONE);
          val = TRANSFER_STATE_FILE_DONE;
          bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0x0000,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&val,sizeof(uint32_t));
          break;
        }

        pBlockEnd = pBlock+ ( ulRemainder > max_xfer_size ?max_xfer_size:ulRemainder);

        if( g_bIsRenoDev )
        {
          // get the checksum of this block data.
          ulBlockCheckSum = 0;
          for( pCur =pBlock;pCur!=pBlockEnd; pCur +=4)
          {
            ulBlockCheckSum+= *((uint32_t*)pCur);
          }

          i2c_xfer_reno.xfer.xfer_flag    = TRANSFER_STATE_PACKET_READY;
          i2c_xfer_reno.xfer.block_index  = ulBlockIndex;
          i2c_xfer_reno.xfer.check_sum    = 0;
          i2c_xfer_reno.num_dword         = (pBlockEnd-pBlock) / 4;

          // get the checksum of xfer header.
          for( xfer_int =(int*)&i2c_xfer_reno;
                xfer_int !=((int*)&i2c_xfer_reno)+4; xfer_int++)
          {
            ulBlockCheckSum+= *xfer_int;
          }

         i2c_xfer_reno.xfer.check_sum  = ~ulBlockCheckSum;
	


         // conver the xfer to little endia
         for( xfer_int =(int*)&i2c_xfer_reno;
           xfer_int !=((int*)&i2c_xfer_reno)+4; xfer_int++)
         {
            *xfer_int = *xfer_int;
         }

         cx20921_i2c_transmit((unsigned char*)&i2c_xfer_reno,sizeof(i2c_xfer_reno),
           (unsigned char*)pBlock,pBlockEnd-pBlock );
        }
        else
        {
          // get the checksum of this block data.
          ulBlockCheckSum = 0;
          for( pCur =pBlock;pCur!=pBlockEnd; pCur +=4)
          {
            ulBlockCheckSum+= *((uint32_t*)pCur);
          }

          i2c_xfer.xfer_flag   = TRANSFER_STATE_PACKET_READY;
          i2c_xfer.block_index = ulBlockIndex;
          i2c_xfer.check_sum   = ulBlockCheckSum;

          cx20921_i2c_transmit((unsigned char*)&i2c_xfer,sizeof(i2c_xfer),
                       (unsigned char*)pBlock,pBlockEnd-pBlock );
        }
        if (!(g_bIsRenoDev || g_is_dual_img) && (image_size-ulRemainder ==0) )
        {
            /* using old download firmare approach. this will take about few secondes
              for easing whole chip*/ 
          //ShowProgress(image_size-ulRemainder,0,I2C_ERASE_CHIPS,image_size);
        } else{
          //ShowProgress(image_size-ulRemainder,0,I2C_OK,image_size);
        }
      }
      break;
    case TRANSFER_STATE_TRANSFER_COMPLETE://#  device->host: transfer complete
      {
        if (g_bIsRenoDev || g_sfs_version >= _SFS_VERSION(0,3,0,0))
        {
          done =1;
          break;
        }
      
        //CRCReturned = i2c_sub_read(8,&ReadErr);
         ReadErr = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x08,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&CRCReturned,4);
        if( ReadErr == CXERR_NOERR)
        {
          if( CRCReturned != 0x53435243 )
          {
            //ShowProgress(image_size,0,I2C_ERR,image_size);
            CONEXANT_CX20921_DEBUG("CRC error!");
            err_no = -CXERR_CRC_CHECK_ERROR;
          }
          else
          {
            //ShowProgress(image_size,0,I2C_OK,image_size);
            CONEXANT_CX20921_DEBUG("Image transfer completed successfully.");
            done =1;
          }
        }
        else
        {
          //ShowProgress(image_size,0,I2C_ERR,image_size);
          err_no = ReadErr;
        }
        
      }
      break;
	case TRANSFER_STATE_PACKET_READY:/*echo*/
	case TRANSFER_STATE_FILE_DONE:/*echo*/
		break;  
    default:
		err_no = -CXERR_FAILED;
      break;
    }

    if( err_no !=CXERR_NOERR || done ) break;
  }
  return err_no;
}


#endif 






int32_t wait_for_bootloader(void)
{
    uint32_t ret = 0;
    int32_t err_no = CXERR_NOERR;
    int32_t loop_times = 5000;
    uint32_t ready  = TRANSFER_STATE_STANDBY;
    

    if( g_bIsRenoDev )
    {
        ready  = TRANSFER_STATE_RENO_LOADER_READY;
        //ready = 0x8021fbde;
    }

    /*
    # Look for reply at address 0. Flags that a transfer can be initiated
    */
    while(true) 
    {
        ret = 0;
        err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x00,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&ret,4);
    //    CONEXANT_CX20921_ERROR("%s  err_no: %d, ret: 0x%8x", __func__, err_no, ret);
        if((ret == ready) && (err_no == CXERR_NOERR))
            break;
        osDelay(POLLING_INTERVAL_MS*10);
        //loop_times--; 
    }

    if( loop_times == 0)
    {
        CONEXANT_CX20921_ERROR("%s: aborting. timeout wating for bootloader",__func__);
        err_no = -CXERR_WAITING_LOADER_TIMEOUT;
    }
    return err_no;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Conexant ????  ------ ???
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Boot loader error code*/
#define BLERR_BLOCK_NR		-1
#define BLERR_CHECKSUM		-2
#define BLERR_LENGTH		-3
#define BLERR_TIMEOUT		-4
#define BLERR_NSUPP		-5
#define BLERR_NPROT		-6
#define BLERR_ERASE		-7
#define BLERR_WRITE		-8
#define BLERR_VERIFY		-9
#define BLERR_INVALID_CMD	-10
#define BLERR_NOTHING_TO_DO	-11
#define BLERR_INVALID_SFS_HDR	-12
#define BLERR_I2C_INTERNAL	-20


#define CXERR_I2C_BLOCK_NR_ERROR    125
#define CXERR_SEND_I2C_ERROR        126
#define CXERR_UNSUPPORTED_FLASH_MEMORY    128



#define CONEXANT_RAW_MAGIC  (('W' << 24) | ('A' << 16) | ('R' << 8) | 'C')
#define CONEXANT_BOOT_MAGIC (('T' << 24) | ('O' << 16) | ('O' << 8) | 'B')
typedef struct raw_firmware_header {
    uint32_t   magic_number;
    uint32_t   version[4]; 
    uint32_t  update_mode; 
    uint32_t  erase_flag; 
} raw_firmware_header_t;


typedef struct bootloader_descriptor {
    uint32_t magic_number; 
    uint32_t length; 
}bootloader_descriptor_t;

typedef struct hdrs_descriptor {
    uint32_t length; 
    uint32_t part_size; 
}hdrs_descriptor_t;

typedef struct block_descriptor {
    uint32_t sector_nr; 
    uint32_t length;
} block_descriptor_t; 





//from i2c_flash source code
#define PACKED                          __attribute__((packed))
struct i2c_message_s
{
    int32_t   cmd:6;			// the command
    uint32_t  err:1;			// error in message
    uint32_t  repl:1;			// done - command completed
    uint32_t  blk:8;			// block nr
    uint32_t  len:16;			// total length of mesage (in bytes)
    uint32_t crc;			// crc over message
} PACKED;				//  

struct i2c_start_auto_cmd_h_s   { struct i2c_message_s hdr; uint32_t part_size; uint32_t hdr_len; } PACKED;
struct i2c_start_auto_rpl_s     { struct i2c_message_s hdr; uint32_t status;  } PACKED;

struct i2c_write_cmd_h_s	{ struct i2c_message_s hdr; uint16_t  sector; uint16_t length; } PACKED;
struct i2c_write_rpl_s		{ struct i2c_message_s hdr; uint32_t status; } PACKED;

// definitions for new, command based update
enum i2c_cmd_e
{
    I2C_PING,			//  0 ping
    I2C_RSTAT,			//  1 get status
    I2C_WSTAT,			//  2 write status
    I2C_ERASES,			//  3 erase 4K sector
    I2C_ERASEC,			//  4 erase chip
    I2C_READ,			//  5 read from a 4K block, from offset 0
    I2C_WRITE_NORMAL,		//  6 write to a 4K block, from offset 0
    I2C_WRITE_VERIFY,		//  7 write to a 4K block, from offset 0, verify after write
    I2C_ERASE_WRITE_VERIFY,	//  8 erase, write 4K block from offset 0 and verify
    I2C_PROTECT,			//  9 change sw protection (on or off)
    I2C_RTUNNEL,			// 10 send a SPI read command
    I2C_WTUNNEL,			// 11 send a SPI write command
    I2C_START_AUTO,		// 12 Start auto (ping-pong) mode
    I2C_VERIFY_IMG,		// 13 verify image CRC
};
uint32_t  g_i2c_blknr = 0;

static int app_ota_conexant_init(void)
{
    int ret = 0;
    ret = cx20921_hal_init();
    TRACE("%s: cx20921_hal_init return %d ",__func__,ret);
    return ret;
}


static uint32_t hash(const void *dat,uint32_t n)
{
    const uint16_t *d=(const uint16_t *)dat;
    uint32_t s1=0xffff;
    uint32_t s2=0xffff;
    n=n/sizeof(short);
    while(n)
    {
        uint32_t l = n > 359 ? 359 : n;
        n-=l;
        do {
            s2+=s1+=*d++;
        } while (--l);
        s1=(s1&0xffff)+(s1>>16);
        s2=(s2&0xffff)+(s2>>16);
    }
    /* Second reduction step to reduce sums to 16 bits */
    s1=(s1&0xffff)+(s1>>16);
    s2=(s2&0xffff)+(s2>>16);
    return (s2<<16)|s1;
}

int i2c_send(const void *tbuf,uint32_t tlen,void *rbuf,uint32_t rlen, const void *tpayld, uint32_t tpaylen, void *rpayld, uint32_t rpayldlen)
{
    int err_no = CXERR_NOERR;
    struct i2c_message_s *msg;
    int loader_err;

    msg = (struct i2c_message_s *)tbuf;
    msg->blk = g_i2c_blknr;
    msg->len = (uint16_t)tlen;
    msg->crc = 0;
    msg->crc = hash(msg,tlen);

    err_no = cx20921_i2c_transmit((uint8_t*)msg,tlen,(byte *)tpayld,tpaylen);
    CONEXANT_CX20921_ERROR("tpayld = 0x%8x  tpaylen = %d ", tpayld, tpaylen);

    if(CXERR_NOERR != err_no) 
        return err_no;
    
    msg=(struct i2c_message_s *)rbuf;
    for(;;)
    {
        if(bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)msg,4) != CXERR_NOERR)
        {
            return -CXERR_I2C_ERROR;
        }
        else if (msg->repl==1)
            break;
    }
    g_i2c_blknr++;

    if (msg->err)
    {
        CONEXANT_CX20921_ERROR("msg cmd:0x%2x err:0x%2x repl:0x%2x blk:0x%2x len:0x%4x crc:0x%8x ", 
                msg->cmd, 
                msg->err,
                msg->repl,
                msg->blk,
                msg->len,
                msg->crc);
        loader_err = msg->cmd;
        if (loader_err == BLERR_BLOCK_NR)
        {
            CONEXANT_CX20921_ERROR("%s: Got I2C message error: cmd = %d , block nr = %d",__func__, loader_err, msg->repl);
            g_i2c_blknr = msg->repl;
            return -CXERR_I2C_BLOCK_NR_ERROR;
        } 
        else if (loader_err == BLERR_NSUPP) 
        {
            CONEXANT_CX20921_ERROR("%s: Unsupported flash memory!",__func__);
            return -CXERR_UNSUPPORTED_FLASH_MEMORY;
        }
        CONEXANT_CX20921_ERROR("%s: Got an error message from bootload : num = %d, cmd = %d",__func__, loader_err,msg->cmd);

        return -CXERR_SEND_I2C_ERROR;
    }

    if(bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)msg,rlen) != CXERR_NOERR)
    {
        return -CXERR_I2C_ERROR;
    }

    if( rpayldlen) 
    {
        err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, rlen,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)rpayld,rpayldlen) ;
    }

    return err_no;
}
/**
 *  
 */
static int i2c_start_automode(const uint8_t *data, uint32_t length, uint32_t part_size) {

    struct i2c_start_auto_cmd_h_s cmd ;
    struct i2c_start_auto_rpl_s   rpl ;
    int32_t ret = CXERR_NOERR;
    cmd.hdr.cmd=I2C_START_AUTO;			//  Start auto-update
    cmd.hdr.err=0;
    cmd.hdr.repl=0;
    cmd.hdr_len=length;
    cmd.part_size = part_size;

    ret = i2c_send(&cmd,sizeof(cmd),&rpl,sizeof(rpl),data,length,NULL,0);

    return ret; 
}


static int i2c_write_verify(uint32_t sector,uint32_t length,uint32_t erase, const uint8_t *data) 
{
    int err_no = CXERR_NOERR;
    struct i2c_write_cmd_h_s cmd ;
    struct i2c_write_rpl_s  rpl ;
    int32_t  pRet; 

    // if( (sector >= 2)  && (g_update_mode!=SFS_UPDATE_AUTO) ) sector +=g_partial_offset;

    if (erase) {
        CONEXANT_CX20921_DEBUG("%s  erase = %d.", __func__, erase );
        cmd.hdr.cmd=I2C_ERASE_WRITE_VERIFY;
    }
    else
    cmd.hdr.cmd=I2C_WRITE_VERIFY;			//  write to a 4K block, from offset 0
    cmd.hdr.err=0;
    cmd.hdr.repl=0;
    cmd.sector=sector;
    cmd.length=length;

    err_no =  i2c_send(&cmd,sizeof(cmd),&rpl,sizeof(rpl),data,length,NULL,0);

    pRet = rpl.status;
    CONEXANT_CX20921_DEBUG("rpl->status :%d", pRet);
    return err_no;

}

int download_boot_loader(const unsigned char *  loader_data, unsigned int num_byte)
{
    
    uint32_t  val;

    unsigned int   time_out_loop =  MANUAL_TIME_OUT_MS/POLLING_INTERVAL_MS;

    int err_no = CXERR_NOERR;

    cx20921_reset();

    CONEXANT_CX20921_DEBUG("cx20921 reset completed!");


     /*
    # Look for 'C' at address 0. Flags that a transfer can be initiated
    */
#ifdef CONFIG_I2C_DISABLE_RESET 
    for(;;) 
#else
    for(;time_out_loop;time_out_loop--) 
#endif
    {
        /*if( (uint32_t)(i2c_sub_read(0x00,&ReadErr)  ==(uint32_t) 0X43) && ((int)ReadErr==(int)CXERR_NOERR))*/

        if((err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)&val,4)) != CXERR_NOERR|| val != 0X000043U) 
        {
        //    CONEXANT_CX20921_DEBUG("%s %d  0x%8x", __func__, err_no, val);

        }

        //val = i2c_sub_read(0x00,&ReadErr);
        if( val==0X000043U) 
        {
            CONEXANT_CX20921_DEBUG("reset ok, magic number: 0x%08x ", val);
            break;
        }
        /*sys_mdelay(POLLING_INTERVAL_MS);*/
    }

    if( time_out_loop == 0)
    {
        CONEXANT_CX20921_DEBUG("%s aborting. timeout wating for device");
        return -CXERR_WAITING_RESET_TIMEOUT;
    }

    /*
    # Delete the magic number to signal that the download just started
    # but hasn't finished yet
    */
    val = 0;
    if(bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&val,sizeof(uint32_t))== CXERR_NOERR) {
        CONEXANT_CX20921_DEBUG("clear the magic number  ok .");
    }
    else 
    {
        CONEXANT_CX20921_DEBUG("Warning: clear the magic number failed.");
    }
    //i2c_sub_write(0x0000,0x0000); // to stop download sequence

    //if(g_bIsRenoDev )

    CONEXANT_CX20921_DEBUG("start to download bootloader...  ");

    if(g_bIsRenoDev)
    {
      err_no = TransferData((uint8_t*)loader_data,num_byte);
    }
    else
    {
        //i2c_transmit(loader_data,num_byte,NULL,0);
      cx20921_i2c_transmit(loader_data,num_byte,NULL,0);
    }
    return err_no;
}


static int32_t i2c_download_partition(uint8_t* pImg, uint32_t imgSize){


    raw_firmware_header_t  file_header; 
    bootloader_descriptor_t bootloader_descriptor; 
    hdrs_descriptor_t   firmware_hdrs; 
    block_descriptor_t block_descriptor;

    uint8_t * ptr; 
    int32_t ret = CXERR_NOERR;

    uint32_t erase_flag;  

    memcpy(&file_header, pImg, sizeof(raw_firmware_header_t)); 
    if(file_header.magic_number != CONEXANT_RAW_MAGIC) {
        CONEXANT_CX20921_DEBUG("%s: not a conexant raw firmware!",__func__);
        return CXERR_INVALID_PARAMETER;
    }

    erase_flag = file_header.erase_flag; 

    //TODO - check version number ? 

    CONEXANT_CX20921_DEBUG("firmware version: %d.%d.%d.%d", file_header.version[0], 
                            file_header.version[1],
                            file_header.version[2],
                            file_header.version[3]);

    ptr = pImg + sizeof(raw_firmware_header_t);

    memcpy(&bootloader_descriptor, ptr, sizeof(bootloader_descriptor_t));
    if(bootloader_descriptor.magic_number != CONEXANT_BOOT_MAGIC) {
        CONEXANT_CX20921_DEBUG("%s: not contain valid bootloader!",__func__);
        return CXERR_INVALID_PARAMETER;
    }
    else 
    {
         CONEXANT_CX20921_DEBUG("%s: bootloader length: %d !",__func__, bootloader_descriptor.length);
    }

    ptr = ptr + sizeof(bootloader_descriptor_t);

    //download bootloader 

    download_boot_loader(ptr, bootloader_descriptor.length);

    if( (ret = wait_for_bootloader()) != CXERR_NOERR) 
    {
        CONEXANT_CX20921_DEBUG("%s: wait_for_bootloader failed!",__func__);
        return ret;
    }

    CONEXANT_CX20921_DEBUG("firmware update mode entered.");

    ptr = ptr + bootloader_descriptor.length; 
    memcpy(&firmware_hdrs, ptr, sizeof(hdrs_descriptor_t));
    ptr += sizeof(hdrs_descriptor_t);
    CONEXANT_CX20921_DEBUG("#1 firmware_hdrs.length: %d , firmware_hdrs.part_size: %d .", 
                firmware_hdrs.length,
                firmware_hdrs.part_size);
    //start auto mode
    if((ret = i2c_start_automode(ptr, firmware_hdrs.length, firmware_hdrs.part_size))!=CXERR_NOERR) {
            CONEXANT_CX20921_DEBUG("%s: start auto mode failed!",__func__);
            return ret; 
    }
    CONEXANT_CX20921_DEBUG("#2");

    // start write sector. 
    ptr = ptr + firmware_hdrs.length; 

    while(ptr < pImg + imgSize ) {
        memcpy(&block_descriptor, ptr, sizeof(block_descriptor_t));
        ptr = ptr + sizeof(block_descriptor_t);

        if(ptr + block_descriptor.length > pImg+imgSize) {
            CONEXANT_CX20921_DEBUG("%s: invalid block descriptor!",__func__);
            ret =  CXERR_INVALID_PARAMETER;
            //app_ota_process_client_cmd_notify(0,0);
            break;
        }

        if((ret = i2c_write_verify(block_descriptor.sector_nr, block_descriptor.length, !erase_flag, ptr)) !=CXERR_NOERR ){
             CONEXANT_CX20921_DEBUG("%s: write secotr %d error!",__func__, block_descriptor.sector_nr);
             //app_ota_process_client_cmd_notify(0,0);
             break;
        }
        osDelay(50);  // delay for spp snd cmd could receive by app
        CONEXANT_CX20921_DEBUG("write from (%p) length %d to sector %d OK! ", ptr, block_descriptor.length, block_descriptor.sector_nr);
        //app_ota_process_client_cmd_notify(1,0);
        ptr = ptr + block_descriptor.length;
    }
    CONEXANT_CX20921_DEBUG("#3");

    if(ptr == pImg +imgSize ) {
        CONEXANT_CX20921_DEBUG("%s: update firmware successfully !", __func__);
        
        ret =  CXERR_NOERR;
    }
    return ret; 

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Conexant ????  ------ ??
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int32_t Download_Firmware(uint8_t* pImg, uint32_t imgSize, uint32_t* pChkVer)
{
    int32_t ret = CXERR_NOERR;
    g_i2c_blknr=0;

    app_ota_conexant_init();
 #if 0    
    // Tell the control app to switch to FW upgrade mode. 
    Command cmd = {0,CONTROL_APP_FW_UPGD,0,_ID('C','T','R','L')};

    if((ret = bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,4,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)(&cmd) + 4,4))!=0) {
         CONEXANT_CX20921_DEBUG("%s: bes_hal_i2c_write failed: %d!",__func__, ret);
    }

    if((ret = bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0,CX20921_I2C_REG_ADDR_LEN,  (uint8_t*)(&cmd),4)) !=0) {
         CONEXANT_CX20921_DEBUG("%s: bes_hal_i2c_write failed: %d!",__func__, ret);
    }

    CONEXANT_CX20921_DEBUG("waiting for bootloader...");
#endif 

    ret = i2c_download_partition(pImg, imgSize);
    if(ret == CXERR_NOERR)
    {
    //    CONEXANT_CX20921_DEBUG("%s: I2c transfor firmware succeed!",__func__);
        //app_ota_process_client_cmd_notify(1,1);
    }
    else
    {
    //    CONEXANT_CX20921_DEBUG("%s: I2c transfor firmware failed!",__func__);
        //app_ota_process_client_cmd_notify(0,1);
        goto exit;
    }
    
    CONEXANT_CX20921_DEBUG("%s: ota file version: [%d.%d.%d.%d]", __func__, (uint32_t)*((uint32_t*)pChkVer), (uint32_t)*((uint32_t*)pChkVer + 1), (uint32_t)*((uint32_t*)pChkVer + 2), (uint32_t)*((uint32_t*)pChkVer + 3));
#if 0
//    ret = verify_firmware_version(pChkVer);
    CONEXANT_CX20921_DEBUG("%s: verify_firmware_version return %d!",__func__,ret);
    if(ret == 0)
    {
        CONEXANT_CX20921_DEBUG("%s: verify firmware version succeed!------",__func__);
        app_ota_process_client_cmd_notify(1,1);
    }
    else
    {
        CONEXANT_CX20921_ERROR("%s: verify firmware version failed, err = %d!------",__func__,ret);
        app_ota_process_client_cmd_notify(0,1);
    }
#endif
exit:

    return ret;
}


int32_t verify_firmware_version(uint32_t* pChkVer) 
{
    int32_t err_no = 0;
    uint32_t check_loop = READY_POOLING_LOOP_MANUALLY;
    uint32_t  fw_ver[4] = {0};
    uint32_t i = 0;
    uint32_t val = 0;

    CONEXANT_CX20921_DEBUG("%s: Device rebooting... ",__func__); 
    check_loop = READY_POOLING_LOOP;
    // System can reset the device prgorammatically. 
    cx20921_power_onoff(0); //sets RESET PIN to low.
    osDelay(RESET_INTERVAL_MS);
    cx20921_power_onoff(1); //sets RESET PIN to high.

    for (i=0;i<check_loop;i++ ) {

        osDelay(READY_POOLING_INTERVAL);
        err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 4,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&val,sizeof(uint32_t));
        if (BADBEFF == val ) 
        {
            break;
        }
    }

    if (BADBEFF != val ) 
    {
        CONEXANT_CX20921_ERROR("%s: Failed to reboot!",__func__);
        return CXERR_FW_CANNOT_BOOT;
    }

    CONEXANT_CX20921_DEBUG("%s: Device rebooted successfully!",__func__);

    /*Check firmware version*/

    CONEXANT_CX20921_DEBUG("%s: Verify firmware version....",__func__);
    err_no = get_fw_version((uint32_t*)fw_ver);
    if (err_no == CXERR_NOERR && memcmp((void*)fw_ver,(void*)pChkVer,sizeof(fw_ver)-sizeof(uint32_t)) ==0) 
    {
        CONEXANT_CX20921_DEBUG("%s: Good",__func__);
    } 
    else 
    {
        if (err_no == CXERR_NOERR) 
        {
            CONEXANT_CX20921_DEBUG("%s: Warning: Dev FW ver [%d.%d.%d.%d] != SFS FW ver [%d.%d.%d.%d]",__func__,
                fw_ver[0], fw_ver[1], fw_ver[2], fw_ver[3],
                pChkVer[0], pChkVer[1], pChkVer[2], pChkVer[3]);
            err_no = CXERR_FW_VER_INCORRECT;
        } 
        else 
        {
            CONEXANT_CX20921_ERROR("%s: Error: %d",__func__,err_no);
        }
    }
    
    return err_no;
}

#define CONEXANT_OTA_THREAD_STACK_SIZE			(2*1024)
#define CONEXANT_OTA_THREAD_PRIORITY			(osPriorityAboveNormal)
static void conexant_ota_thread(const void *arg);
osThreadDef(conexant_ota_thread, CONEXANT_OTA_THREAD_PRIORITY, CONEXANT_OTA_THREAD_STACK_SIZE);
osThreadId conexantOTAThreadId;

//#include "app_ota.h"
static void conexant_ota_thread(const void *arg)
{
    uint8_t* pFirmwareBuf = NULL;
    uint32_t firmwareSize = 0;
    uint32_t* pMd5Info = NULL;
    int32_t nRet = 0, retry = 10;

    //pFirmwareBuf = app_ota_get_firmware_addr();
    //firmwareSize = app_ota_get_firmware_size();
    //pMd5Info = app_ota_get_firmware_md5info();
    CONEXANT_CX20921_DEBUG("%s: firmwareBuf = 0x%08x, size = %d, pMd5InfoAddr = 0x%08x",(uint32_t)pFirmwareBuf,firmwareSize,(uint32_t)pMd5Info);
    while(1)
    {
        nRet = Download_Firmware(pFirmwareBuf,firmwareSize,pMd5Info);
        CONEXANT_CX20921_DEBUG("%s: Download_Firmware return %d",__func__,nRet);
        if(nRet == 0)
        {
            CONEXANT_CX20921_DEBUG("%s: Download_Firmware Succeed!",__func__);
            break;
        }
        else
        {
            CONEXANT_CX20921_ERROR("%s: %d times Download_Firmware failed: err_no = %d",__func__,11-retry, nRet);
            retry--;
        //    if(retry <= 0)
                break;
        }

    }
}

void conexant_ota_init(void)
{
    CONEXANT_CX20921_DEBUG("%s: enter!",__func__);
    conexantOTAThreadId = osThreadCreate(osThread(conexant_ota_thread), NULL);
}
#endif