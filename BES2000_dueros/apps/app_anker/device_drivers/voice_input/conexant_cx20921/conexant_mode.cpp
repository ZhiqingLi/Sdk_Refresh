
#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"

#ifdef VOICE_INPUT_MODULE_CX20921
#include "conexant_cx20921.h"
#include "conexant_cmd.h"
#include "conexant_mode.h"
#include "bes_hal_i2c.h"

#define ID(a,b,c,d)  ((((a)-0x20)<<8)|(((b)-0x20)<<14)|(((c)-0x20)<<20)|(((d)-0x20)<<26))
#define APP_ID_SOS  ID('S','O','S',' ')
#define APP_ID_CTRL ID('C','T','R','L')
#define APP_ID_CAPT ID('C','A','P','T')

#define SPATIAL_SEP2_CMD_INTERNAL_STATE 0x0042
#define hex2id0(id)  ((id>>8 )&0x3f)+0x20
#define hex2id1(id)  ((id>>14)&0x3f)+0x20
#define hex2id2(id)  ((id>>20)&0x3f)+0x20
#define hex2id3(id)  ((id>>26)&0x3f)+0x20

uint32_t current_mode_index = 0;

#define MAX_NUM_OF_MODES 64     // This is not a fixed number, if host has lost less modes, it can limit this number
struct mode {
    uint32_t id;
    uint8_t mode_name[4];
};

typedef struct _avail_modes_t{
    int32_t num_of_modes;
    struct mode modes[MAX_NUM_OF_MODES];
} avail_modes_t;
avail_modes_t avail_modes;

static uint8_t conexant_firmware_version_flag = 0;
static uint8_t conexant_firmware_version_str[40] = {0};


#if 1
int32_t get_fw_version (uint32_t *pVersion)
{
    Command cmd;
    int32_t ret_val = SENDCMD(&cmd, APP_ID_CTRL, CMD_GET(CONTROL_APP_VERSION));
    if (ret_val<0)
    {
        CONEXANT_CX20921_ERROR ("%s: failed to get FW version",__func__);
    }
    else
    {
        CONEXANT_CX20921_DEBUG ("%s: FW version: %d.%d.%d.%d",__func__,cmd.data[0],
            cmd.data[1], cmd.data[2], cmd.data[3]);
        memcpy((void*)pVersion,(void*)cmd.data,4*sizeof(uint32_t));
    }

    return ret_val;
}
#else
int32_t get_fw_version(uint32_t *pVersion)
{
	int32_t err_no ;
	
	uint32_t data,num,reply,i, writeVal = 0;
	
	if (!pVersion)
    {
		err_no = -CXERR_INVALID_PARAMETER;
		goto LEAVE;
	}
	
    writeVal = 0xB32D2300;
    err_no = bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,4,2,(uint8_t*)&writeVal,sizeof(uint32_t));
    writeVal = 0x0103000d;
	err_no = err_no<0?err_no:bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0,2,(uint8_t*)&writeVal,sizeof(uint32_t));
	if (err_no < 0) 
        goto LEAVE;
	for (i=0;i < 50;i++)
	{
		osDelay(10);
        err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR,0,2,(uint8_t*)&data,sizeof(uint32_t));
		if (err_no < 0) 
            goto LEAVE;
		num = data & 0xffff;
		reply = data>>31;
		if (num ==4 && reply)
		{
			goto GOT_VER;
		}
	}
	err_no = -CXERR_FW_NO_RESPONSE;
	goto LEAVE;

GOT_VER:
	for (i=0; i < num;i++) {
        err_no = bes_hal_i2c_read(CX20921_I2C_SLAVE_ADDR,8+i*4,2,(uint8_t*)((uint32_t*)pVersion+i),sizeof(uint32_t));
		if (err_no < 0) 
            goto LEAVE;
	}
    CONEXANT_CX20921_DEBUG("%s: get firmware version succeed: %d.%d.%d.%d",__func__,
        (uint32_t)*pVersion,(uint32_t)*((uint32_t*)pVersion+1),(uint32_t)*((uint32_t*)pVersion+2),(uint32_t)*((uint32_t*)pVersion+3));
LEAVE:
    CONEXANT_CX20921_ERROR("%s: get firmware version, errno = %d",err_no);
	return err_no;
}
#endif

static int32_t conexant_frimware_version_init(void)
{
    uint32_t firmwareVer[4] = {0};
    int32_t err_no = 0;
    if(conexant_firmware_version_flag == 0)
    {
        err_no = cx20921_hal_init();
        if(err_no < 0)
        {
            CONEXANT_CX20921_ERROR("%s: cx20921_hal_init failed",__func__);
            return err_no;
        }
        err_no = get_fw_version((uint32_t*)firmwareVer);
        if(err_no < 0)
        {
            CONEXANT_CX20921_ERROR("%s: get_fw_version failed",__func__);
            return 0;
        }
       
        CONEXANT_CX20921_DEBUG("%s: conexant ver --> [%d.%d.%d.%d]",__func__,(int)firmwareVer[0],(int)firmwareVer[1],(int)firmwareVer[2],(int)firmwareVer[3]);
        
        sprintf((char*)conexant_firmware_version_str,"%d.%d.%d.%d",(int)firmwareVer[0],(int)firmwareVer[1],(int)firmwareVer[2],(int)firmwareVer[3]);
        conexant_firmware_version_flag = 1;
    }
    return 0;
}

int32_t conexant_firmware_version_get(uint8_t* pVerStr)
{
    int32_t err_no = 0;
    if(conexant_firmware_version_flag == 0)
    {
        err_no = conexant_frimware_version_init();
        if(err_no < 0)
            return err_no;
    }
    memcpy(pVerStr,conexant_firmware_version_str,strlen((char*)conexant_firmware_version_str));
    return err_no;
}

int32_t add_mode(uint32_t id) 
{
    if (avail_modes.num_of_modes < (MAX_NUM_OF_MODES-1))
    {
        char tempStr[5] = {0};
        avail_modes.modes[avail_modes.num_of_modes].id  = id;
        avail_modes.modes[avail_modes.num_of_modes].mode_name[0] = hex2id0(id);
        avail_modes.modes[avail_modes.num_of_modes].mode_name[1] = hex2id1(id);
        avail_modes.modes[avail_modes.num_of_modes].mode_name[2] = hex2id2(id);
        avail_modes.modes[avail_modes.num_of_modes].mode_name[3] = hex2id3(id);
        
        memcpy(tempStr,avail_modes.modes[avail_modes.num_of_modes].mode_name,4);
        CONEXANT_CX20921_DEBUG("%s: Succeed: add %d mode, mode id = 0x%08x , name = %s ", __func__,avail_modes.num_of_modes,id, tempStr);

        avail_modes.num_of_modes++;
        return 0;
    } 
    
    CONEXANT_CX20921_ERROR("%s: Error: Cannot add more modes as it exceeds the max number of modes, num_of_modes = %d", __func__,avail_modes.num_of_modes);
    return -1;
}

int32_t get_mode_id(int32_t index) 
{
    if (index < avail_modes.num_of_modes) 
    {
        return avail_modes.modes[index].id;
    } 
    else 
    {
        CONEXANT_CX20921_ERROR("%s: Error: Index >= number of modes", __func__);
        return -1;
    }
}

uint8_t* get_mode_name(int32_t index) 
{
    if (index < avail_modes.num_of_modes) 
    {
        return avail_modes.modes[index].mode_name;
    } 
    else 
    {
        CONEXANT_CX20921_ERROR("%s: Error: Index >= number of modes",__func__);
        return NULL;
    }
}

int32_t get_num_of_avail_modes (void) 
{
    return avail_modes.num_of_modes;
}

void init_num_of_avail_modes(void) 
{
    avail_modes.num_of_modes = 0;
}

///////////////////////////////////////////////////////
//
// get a list of available tuned modes from the device
//
///////////////////////////////////////////////////////
// golem::sendcmd CTRL CONTROL_MGR_GET_TUNED_MODES {0}
int32_t get_tuned_modes(void)
{
    Command cmd;
    int32_t index = 0;
    int32_t count = 0;
    int32_t ret_val = 0;
    int32_t i;

    do 
    {
        ret_val = SENDCMD(&cmd, APP_ID_CTRL, CMD_GET(CONTROL_MGR_TUNED_MODES), index);
        if (ret_val<0)
        {
            CONEXANT_CX20921_ERROR("%s: failed to get a list of modes",__func__);
            return -1;
        }
        count = cmd.num_32b_words;
        if ( (count > 0) && (count <= MAX_COMMAND_SIZE) ) 
        {
            index += count;
            for (i = 0; i < count; i++)
            {
                ret_val = add_mode(cmd.data[i]);
                if(ret_val < 0)
                {
                    CONEXANT_CX20921_ERROR("%s: failed to add_mode %d",__func__,cmd.data[i]);
                    return -2;
                }
            }
        }
    } while (count == MAX_COMMAND_SIZE);
    return 0;
}

///////////////////////////////////////
//
// get the current mode
//
///////////////////////////////////////
// golem::sendcmd CTRL CONTROL_APP_GET_VERSION {}
int32_t get_current_mode (uint32_t* pCurMode)
{
    Command cmd;
    int32_t ret_val = 0;
    int32_t i;
    uint32_t modeID = 0, tempVal = 0;
    int32_t num_of_modes = get_num_of_avail_modes();

    tempVal = (uint32_t)APP_ID_CTRL;
    CONEXANT_CX20921_DEBUG("%s: enter,num_of_modes = %d,tempVal = 0x%08x",__func__,num_of_modes,tempVal);
    ret_val = SENDCMD(&cmd, APP_ID_SOS, CMD_GET(SOS_RESOURCE), APP_ID_CTRL);
    CONEXANT_CX20921_DEBUG("%s: SENDCMD ret_val = %d",__func__,ret_val);
    if (ret_val < 0)
    {
        CONEXANT_CX20921_ERROR("%s: Error: failed to get the current mode",__func__);
        return -1;
    }

    i = 0;
    modeID = (uint32_t)get_mode_id(i);
    CONEXANT_CX20921_DEBUG("%s: modeID = %d",__func__,modeID);
    while ( (cmd.data[0] != modeID) && (i < num_of_modes) )
    {
        CONEXANT_CX20921_DEBUG("%s: i = %d, cmd.data[0] = 0x%08x, modeID = 0x%08x, num_of_modes = %d",__func__,i,cmd.data[0],modeID,num_of_modes);
        i++;
        modeID = (uint32_t)get_mode_id(i);
    }

    if (i < num_of_modes) 
    {
        CONEXANT_CX20921_DEBUG("%s: Current mode is: %s",__func__, get_mode_name(i));
        current_mode_index = i;
        *pCurMode = i;
        return 0;
    } 
    else 
    {
        CONEXANT_CX20921_ERROR("%s: Error: Cannot find the current mode: %08x",__func__, cmd.data[0]);
        return -1;
    }
}

///////////////////////////////////////
//
// get the angle of arrival
//
///////////////////////////////////////
// golem::sendcmd CAPT 30 SPATIAL_SEP_CMD_GET_DETECTED_DIRECTION {}
int32_t get_angle_of_arrival(float* angle_of_arrival)
{
    Command cmd;

	int32_t ret_val = SENDCMD(&cmd, (APP_ID_CAPT | 30), CMD_GET(SPATIAL_SEP2_CMD_INTERNAL_STATE));

	if (ret_val<0)
	{
		CONEXANT_CX20921_ERROR("%s: ERROR:CMD_GET(SPATIAL_SEP2_CMD_INTERNAL_STATE) command failed with error: %08x",__func__, ret_val);
		return ret_val;
	}

	if (cmd.num_32b_words != 7)
	{
		CONEXANT_CX20921_DEBUG("%s: ERROR:CMD_GET(SPATIAL_SEP2_CMD_INTERNAL_STATE) command has wrong number of return values: %d",__func__, cmd.num_32b_words);
		return ret_val;
	}
	*angle_of_arrival = ((float)((long)cmd.data[2]))/(1<<23);
//	CONEXANT_CX20921_DEBUG("%s: The angle of arrival (degrees): %f", __func__,*angle_of_arrival);
	
	return ret_val;
}

///////////////////////////////////////
//
// Switch mode
//
///////////////////////////////////////
// golem::sendcmd CTRL CONTROL_APP_EXEC_FILE {(id)<configuration name>}
int32_t switch_to_mode (uint32_t new_mode)
{
    Command cmd;
    int32_t          id        =  get_mode_id(  new_mode);
    uint8_t * mode_str  =  get_mode_name(new_mode);
    int32_t ret_val =  0;

    if(current_mode_index == new_mode)
    {
        CONEXANT_CX20921_DEBUG("%s: curmode = newmode = %s mode",__func__,mode_str);
        return 0;
    }

    ret_val = SENDCMD(&cmd, APP_ID_CTRL, CMD_SET(CONTROL_APP_EXEC_FILE), id);
    if (ret_val==0)
    {
        osDelay(50);
        ret_val = SENDCMD(&cmd, APP_ID_SOS, CMD_GET(SOS_RESOURCE), APP_ID_CTRL);
        if ((ret_val==0)&&((int32_t)(cmd.data[0])==id))
        {
            current_mode_index = new_mode;
            CONEXANT_CX20921_DEBUG("%s: Switched to mode:  %s",__func__,mode_str);
        }
        else
        {
            CONEXANT_CX20921_ERROR("%s: failed to confirm switching to mode: %d %x %x",__func__,ret_val, id, cmd.data[0]);
        }
    }
    else 
    {
        CONEXANT_CX20921_ERROR("%s: command to switch to mode: %d, %s failed",__func__,new_mode,mode_str);
    }
    return ret_val;
}

int32_t conexant_mode_change(CONEXANT_WORK_MODE_T mode)
{
    int32_t ret = 0;
    switch(mode)
    {
        case CONEXANT_MODE_CAPTURE_16K_16B_STERO:
    //    ret = switch_to_mode(3);
        break;
        case CONEXANT_MODE_CAPTURE_16K_16B_MONO:
        break;
        case CONEXANT_MODE_STANDBY:
        break;
        case CONEXANT_MODE_SLEEP:
        break;
        default:
        break;
    }
    return ret;
}

int32_t conexant_mode_init(void)
{
    int32_t err_no = 0;

    uint32_t curMode = MAX_NUM_OF_MODES;
    
    CONEXANT_CX20921_DEBUG("%s: enter!",__func__);
    err_no = cx20921_hal_init();
    if(err_no < 0)
    {
        CONEXANT_CX20921_ERROR("%s: cx20921_hal_init failed",__func__);
    }

    init_num_of_avail_modes();

    conexant_frimware_version_init();

    err_no = get_tuned_modes();
    if(err_no < 0)
    {
        CONEXANT_CX20921_ERROR("%s: get_tuned_modes failed ",__func__);
    }
    else
    {
        CONEXANT_CX20921_DEBUG("%s: get_tuned_modes succeed",__func__);
    }

    err_no = get_current_mode((uint32_t*)&curMode);
    if(err_no < 0)
    {
        CONEXANT_CX20921_ERROR("%s: get_current_mode failed",__func__);
    }
    else
    {
        CONEXANT_CX20921_DEBUG("%s: get_current_mode succeed, curMode = %d",__func__,curMode);
    }

#if 0
    float angle = 0.0;
    err_no = get_angle_of_arrival((float*)&angle);
    if(err_no < 0)
    {
        CONEXANT_CX20921_ERROR("%s: get_current_mode failed, err_no = %d",__func__,err_no);
    }
    else
    {
        CONEXANT_CX20921_DEBUG("%s: get_current_mode succeed",__func__);
    }
    osDelay(100);
#endif

#if 0
    uint32_t modeIndex = 0;
    modeIndex = 3;
    err_no = switch_to_mode(modeIndex);
    if(err_no < 0)
    {
        CONEXANT_CX20921_ERROR("%s: switch_to_mode %d : %s MODE failed, err_no = %d",__func__,modeIndex,get_mode_name(modeIndex),err_no);
    }
    else
    {
        CONEXANT_CX20921_DEBUG("%s: switch_to_mode %d : %s MODE succeed",__func__,modeIndex,get_mode_name(modeIndex));
    }
#endif
    CONEXANT_CX20921_DEBUG("%s: exit!",__func__);
    return err_no;
}

int32_t conexant_mode_deinit(void)
{
    cx20921_hal_deinit();
    return 0;
}

#endif