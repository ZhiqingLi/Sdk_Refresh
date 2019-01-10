#include "cmsis.h"
#include "cmsis_os.h"
#include "bes_hal_i2c.h"

#ifdef VOICE_INPUT_MODULE_CX20921
#include "conexant_cmd.h"
#include "conexant_cx20921.h"
// The interval for reply bit polling and its timeout
#define REPLY_POLL_INTERVAL_MSEC     1
#define REPLY_POLL_TIMEOUT_MSEC   5000
#define RESET_INTERVAL_MSEC        100    //100 ms

static int32_t SendCmd_i2c (Command *cmd);

int32_t SendCmd (Command *cmd, uint32_t  app_module_id, uint32_t  command_id, uint32_t num_32b_words, ...)
{
    va_list     args;
    uint32_t    n;
    int32_t ret = 0;
    va_start (args , num_32b_words) ;
    
    // at least two words of header
    if (num_32b_words > MAX_COMMAND_SIZE)
    {
        CONEXANT_CX20921_ERROR("%s: after va_start num_32b_words > MAX_COMMAND_SIZE");
        va_end (args) ;
        return (SENDCMD_RC_INVALID_ARG_NUM);
    }
    cmd->num_32b_words = (command_id&CMD_GET(0)) ? MAX_COMMAND_SIZE : num_32b_words;
    cmd->command_id    = command_id;
    cmd->reply         = 0;
    cmd->app_module_id = app_module_id;
    for (n = 0 ; n < num_32b_words ; n++)
    {
        cmd->data[n] = va_arg(args, int32_t);
        CONEXANT_CX20921_DEBUG("%s: after va_start cmd->data[%d] = 0x%08x",__func__,n,cmd->data[n]);
    }
    va_end (args) ;

    ret = SendCmd_i2c(cmd);
    CONEXANT_CX20921_DEBUG("%s:exit SendCmd_i2c return %d",__func__,ret);
    return ret;
}

static int32_t SendCmd_i2c (Command *cmd)
{

    uint32_t *i2c_data = (uint32_t *)cmd;
    int32_t size = cmd->num_32b_words + 2;

    int32_t elapsed_ms = 0;
    int32_t address_offset = 0;

    CONEXANT_CX20921_DEBUG("%s: cmd->app_module_id = 0x%08x, cmd->command_id = 0x%08x, cmd->num_32b_words = %d",__func__,\
            cmd->app_module_id,cmd->command_id,cmd->num_32b_words);

    // write words 1 to N-1 , to addresses 4 to 4+4*N-1
    bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR, 0x4,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&i2c_data[1],(size-1)*sizeof(uint32_t));
    CONEXANT_CX20921_DEBUG("%s: send i2c data[1] ~ [n], i2c data[1] = 0x%08x!",__func__,i2c_data[1]);
    // write word 0 to address 0
    bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR, 0x0,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&i2c_data[0],sizeof(uint32_t));
    CONEXANT_CX20921_DEBUG("%s: send i2c data[0] = 0x%08x!",__func__,i2c_data[0]);
    while (elapsed_ms < REPLY_POLL_TIMEOUT_MSEC)
    {
        // only read the first word and check the reply bit
        bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x0+address_offset,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&i2c_data[0],sizeof(uint32_t));
         if (cmd->reply==1)
            break;
        osDelay(REPLY_POLL_INTERVAL_MSEC);
        elapsed_ms += REPLY_POLL_INTERVAL_MSEC;
    }

    if (cmd->reply==1)
    {
        if (cmd->num_32b_words < 0)
        {
            return (SENDCMD_RC_CMD_ERROR);
        }
        else 
        {
            if (cmd->num_32b_words > 0)
            {
                bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR, 0x8+address_offset,CX20921_I2C_REG_ADDR_LEN,(uint8_t*)&i2c_data[2],cmd->num_32b_words*sizeof(uint32_t));
            }
            CONEXANT_CX20921_DEBUG("%s:exit successed",__func__);
            return (SENDCMD_RC_SUCCESS);
        }
    }
    CONEXANT_CX20921_ERROR("%s:exit timeout",__func__);
    return(SENDCMD_RC_REPLY_TIMEOUT);
}
#endif