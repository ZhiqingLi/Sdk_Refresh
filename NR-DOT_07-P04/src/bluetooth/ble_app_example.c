/////////////////////////////////////////////
//
//

#include "type.h"
#include "app_config.h"

#include "timer_api.h"
#include "sys_app.h"
#include "spp_app_proc.h"
#include "folder.h"
#include "fat_file.h"
#include "player_control.h"
#include "eq_params.h"
#include "ble_api.h"
#include "ble_app_example.h"
#include "bt_app_func.h"
#include "eq.h"
#include "dev_detect_driver.h"
#include "task_decoder.h"
#include "sys_vol.h"
#include "dac.h"

extern SwEqContext * p_gSwEq;

extern uint32_t GetNextModeId(uint32_t CurModeId);

void PlayCtrlProcess(uint8_t* CmdData, uint8_t datalen);
int16_t playcontrol_att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int16_t playcontrol_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
int16_t att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int16_t att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
int16_t gap_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

BLE_APP_CONTEXT			g_playcontrol_app_context;
GATT_SERVER_PROFILE		g_playcontrol_profile;
GAP_MODE				g_gap_mode;



uint8_t * g_profile_data = NULL;
static uint8_t			gap_device_name[20] = "PlayControlDemo";
static uint16_t 		gap_appearance = 0;
static uint8_t			gap_reconnection_address[6] = {0x11, 0x22,0x33,0x44,0x55, 0x66};
static int 				gap_privacy = 0;

uint8_t primary_service_uuid[] = {0x00, 0x18};//0x9234;
uint8_t characteristic_declare_2a00[] = {0x0a, 0x05, 0x00, 0x00,0x2a};//gap name
uint8_t characteristic_declare_2a01[] = {0x0a, 0x07, 0x00, 0x01, 0x2a};
uint8_t characteristic_declare_2a02[] = {0x0a, 0x09, 0x00, 0x02, 0x2a};
uint8_t characteristic_declare_2a03[] = {0x0a, 0x0b, 0x00, 0x03, 0x2a};
attribute_item_t gap_attribute_list[] = {
        {0xf002, 0x01, 0x2800,0, primary_service_uuid, 2},
        {0xf002, 0x04, 0x2803,0, characteristic_declare_2a00,5 },
        {0xf10a, 0x05, 0x2a00, 0, gap_device_name, 20},//gap name
        {0xf002, 0x06, 0x2803,0, characteristic_declare_2a01,5 },
        {0xf10a, 0x07, 0x2a01, 0, (uint8_t*)&gap_appearance,2},//apperance
        {0xf002, 0x08, 0x2803,0, characteristic_declare_2a02,5 },
        {0xf10a, 0x09, 0x2a02, 0, (uint8_t*)&gap_privacy,1},//privacy
        {0xf002, 0x0a, 0x2803,0, characteristic_declare_2a03,5 },
        {0xf10a, 0x0b, 0x2a03, 0, gap_reconnection_address,6}, //reconnection address
};


static uint8_t playcontrol_cmd_data[255];		// 根据spp文档，此处buffer不超过255字节
static uint8_t playcontrol_rsp_data[255];
static uint8_t* playcontrol_rsp_buffer = &playcontrol_rsp_data[5];

static uint8_t playcontrol_cmd_data_len = 0;
static uint8_t playcontrol_rsp_data_len = 0;

#define PLAYCONTROL_HANDLE_START 0x0080
#define PLAYCONTROL_HANDLE_END 0x0105

uint8_t playcontrol_service_uuid[] = {0x32, 0xa0};//0xa032;
uint8_t playcontrol_characteristic_cmd_declare[] = {0x08, 0x03, 0x01, 0x40,0xa0};//conmand,write only
uint8_t playcontrol_characteristic_result_declare[] = {0x02, 0x05, 0x01, 0x41,0xa0};//conmand excute result, read only
uint8_t playcontrol_characteristic_modechange_result_declare[] = {0x20, 0x91, 0x00, 0x42,0xa0};//conmand excute result, read only
uint8_t playcontrol_rsp_client_configuration = 0;

attribute_item_t  playcontrol_attribute[] = {
    {0xf002, 0x0080, 0x2800, NULL, playcontrol_service_uuid, 2},
    {0xf002, 0x0090, 0x2803, NULL, playcontrol_characteristic_modechange_result_declare, 5},//spp conmand excute result, read only
    {0xf020, 0x0091, 0xa042, NULL, playcontrol_rsp_data,7}, 
    {0xf10a, 0x0092, 0x2902, NULL, &playcontrol_rsp_client_configuration,1}, //new add for phone trigering the data send
    {0xf002, 0x0102, 0x2803, NULL, playcontrol_characteristic_cmd_declare, 5},//spp playcontrol conmand, write only
    {0xf108, 0x0103, 0xa040, NULL, playcontrol_cmd_data,255},
    {0xf002, 0x0104, 0x2803, NULL, playcontrol_characteristic_result_declare, 5},//spp conmand excute result, read only
    {0xf102, 0x0105, 0xa041, NULL, playcontrol_rsp_data,255},     
};

uint16_t playcontrol_cmd_handle 		= 0x0103;
uint16_t playcontrol_response_handle 	= 0x0091;

///////////// advertisement data /////////////////
static uint8_t advertisement_data[32];

uint32_t ConfigAdvertisementData(uint8_t * advertisement_data, uint32_t data_len)
{
	uint32_t		len = 0;

	uint8_t			discoverable_mode;
	uint8_t			flag;
	
//	uint8_t			device_name[20];

	if(advertisement_data == NULL || data_len == 0)
		return 0;

	// init buffer
	memset(advertisement_data, 0 , data_len);

	// flag
	advertisement_data[len++] = 2;
	advertisement_data[len++] = 0x01;

	discoverable_mode = GetGapDiscoverableMode();
	switch(discoverable_mode)
	{
		case NON_DISCOVERABLE_MODE:
			flag = 0;
			break;

		case LIMITED_DISCOVERABLE_MODE:
			flag =1;
			break;

		case GENERAL_DISCOVERABLE_MODE:
		default:
			flag = 2;
			break;
	}

	advertisement_data[len++] = flag;

	// name
	advertisement_data[len++] = strlen((const char*)gap_device_name) + 1;
	advertisement_data[len++] = 0x09;
	memcpy(&advertisement_data[len], gap_device_name, strlen((const char*)gap_device_name));
	len += strlen((const char*)gap_device_name);

	// service uuid
	advertisement_data[len++] = 3;
	advertisement_data[len++] = 0x03;
	memcpy(&advertisement_data[len], playcontrol_service_uuid, 2);
	len += 2;

	return len;
}

///////////// scan response data /////////////////
static uint8_t scan_response_data[32];

uint32_t ConfigScanResponseData(void)
{
	uint32_t		len = 0;

	memset(scan_response_data, 0 , sizeof(scan_response_data));
	
	scan_response_data[len++] = 3;
	scan_response_data[len++] = 0x03;
	memcpy(&scan_response_data[len], playcontrol_service_uuid, 2);
	len += 2;

	return len;
}

int8_t InitBlePlaycontrolProfile(void)
{
	uint32_t		profile_size = 0;
	uint32_t		profile_offset = 0;

	uint32_t		adver_data_len;
	uint32_t		scan_resp_data_len;
	
	g_playcontrol_app_context.ble_device_role = PERIPHERAL_DEVICE;

	
//	SetBleSecurityMode(1);
//	SetBlePeripheralPassword(1234);


	// create profile data
	profile_size = sizeof(gap_attribute_list) + sizeof(playcontrol_attribute) + 1;
	g_profile_data = mmm_malloc(profile_size, MMM_ALLOC_FAST);
	memset(g_profile_data, 0, profile_size);

	profile_offset = CreateProfileData( gap_attribute_list, 
										sizeof(gap_attribute_list)/sizeof(attribute_item_t), 
										g_profile_data, 
										profile_offset);

	
	profile_offset = CreateProfileData( playcontrol_attribute, 
										sizeof(playcontrol_attribute)/sizeof(attribute_item_t), 
										g_profile_data, 
										profile_offset);

	g_profile_data[profile_offset] = 0;

	g_playcontrol_profile.profile_data 	= g_profile_data;
	g_playcontrol_profile.attr_read		= att_read;
	g_playcontrol_profile.attr_write	= att_write;
	
	// set gap mode
	g_gap_mode.broadcase_mode		= NON_BROADCAST_MODE;
	g_gap_mode.discoverable_mode	= GENERAL_DISCOVERABLE_MODE;
	g_gap_mode.connectable_mode		= UNDIRECTED_CONNECTABLE_MODE;
	g_gap_mode.bondable_mode		= NON_BONDABLE_MODE;
	SetGapMode(g_gap_mode);

	// config advertisment data
	adver_data_len = ConfigAdvertisementData(advertisement_data, sizeof(advertisement_data));
	SetAdvertisingData(advertisement_data, adver_data_len);

	// config scan response data
	scan_resp_data_len = ConfigScanResponseData();
	SetScanResponseData(scan_response_data, scan_resp_data_len);
	
	return 0;
}

int8_t UninitBlePlaycontrolProfile(void)
{
	mmm_free(g_profile_data);
	g_profile_data = NULL;
	return 0;
}

int16_t att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
//    int16_t 	ret = 0;
    uint16_t 	uuid16 = GetUuidForHandle(attribute_handle);
    uint16_t  att_value_len;

	if(attribute_handle >= PLAYCONTROL_HANDLE_START && attribute_handle <= PLAYCONTROL_HANDLE_END)
	{
		return playcontrol_att_read(con_handle, attribute_handle, offset, buffer, buffer_size);
	}
	else
	{
		switch(uuid16)
		{
	        case 0x2a00:
	            att_value_len = strlen((const char*)gap_device_name);
	            if (buffer) 
	            {
	                memcpy(buffer, gap_device_name, att_value_len);
	            }
	            return att_value_len;
	        case 0x2a01:
	            if (buffer)
	            {
					buffer[0] = gap_appearance & 0xFF;
					buffer[1] = (gap_appearance >> 8)& 0xFF;
	            }
	            return 2;
	        case 0x2a02:
	            if (buffer)
	            {
	                buffer[0] = gap_privacy;
	            }
	            return 1;
	        case 0x2A03:
	            if (buffer) 
	            {
				    buffer[0] = gap_reconnection_address[5];
				    buffer[1] = gap_reconnection_address[4];
				    buffer[2] = gap_reconnection_address[3];
				    buffer[3] = gap_reconnection_address[2];
				    buffer[4] = gap_reconnection_address[1];
				    buffer[5] = gap_reconnection_address[0];
	            }
	            return 6;
	        default:
	            return 0;
		}
	}
}

int16_t att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    int ret = 0;
    //preccess prepare write
    ret = gap_att_write(con_handle, attribute_handle, transaction_mode, offset, buffer, buffer_size);
    if(ret>=0)
    {
        return ret;
    }
    
	ret = playcontrol_att_write(con_handle, attribute_handle, transaction_mode, offset, buffer, buffer_size);
	if(ret >= 0)
	{
		return ret;
	}

    return 0;
}

int16_t gap_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    attribute_item_t* item  = GetAttrForHandle(gap_attribute_list, sizeof(gap_attribute_list)/sizeof(attribute_item_t), attribute_handle);

    if(!item)
        return -1;

	switch (transaction_mode)
	{
		case TRANSACTION_MODE_NONE:
			if(item->value_len < buffer_size)
				return 0x0d ;//ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH;
			break;
	}

	switch (item->uuid)
	{
		case 0x2a00:
			memcpy(gap_device_name, buffer, buffer_size);
			gap_device_name[buffer_size]=0;
			//APP_DBG("Setting device name to '%s'\n", gap_device_name);
			return 0;
		case 0x2a01:
			gap_appearance = ( ((uint16_t) buffer[0]) | (((uint16_t)buffer[1]) << 8));
			
			//APP_DBG("Setting appearance to 0x%04x'\n", gap_appearance);
			return 0;
		case 0x2a02:
			gap_privacy = buffer[0];
			//APP_DBG("Setting privacy to 0x%04x'\n", gap_privacy);
			//update_advertisements();
			return 0;

		case 0x2A03:
			    gap_reconnection_address[0] = buffer[5];
			    gap_reconnection_address[1] = buffer[4];
			    gap_reconnection_address[2] = buffer[3];
			    gap_reconnection_address[3] = buffer[2];
			    gap_reconnection_address[4] = buffer[1];
			    gap_reconnection_address[5] = buffer[0];
				//APP_DBG("Setting Reconnection Address to %s\n", bd_addr_to_str(gap_reconnection_address));
			return 0;
	}
    return 0;
}


int16_t playcontrol_att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
	uint16_t  att_value_len;
	uint16_t uuid16 = GetUuidForHandle(attribute_handle);
	uint16_t bytes_to_copy;

	if(uuid16 == 0x2902)
	{
		bytes_to_copy = 2;
		if (!buffer)
			return bytes_to_copy;

		if (bytes_to_copy > buffer_size)
		{
			bytes_to_copy = buffer_size;
		}

		memcpy(buffer, &playcontrol_rsp_client_configuration, bytes_to_copy);
		return bytes_to_copy;
	}
	else if(uuid16 == 0xa041)
	{

		att_value_len = playcontrol_rsp_data_len;

		if(att_value_len == 0)		// data not ready
		{
			uint8_t ErrorAck[1] = {0xFF};
			ResponseCommand(0xffff, ErrorAck, 1);
			att_value_len = 7;
		}

		if (offset > att_value_len)
		{
			return 0;
		}

		bytes_to_copy = att_value_len - offset;

		if (!buffer) 
			return bytes_to_copy;

		if (bytes_to_copy > buffer_size)
		{
			bytes_to_copy = buffer_size;
		}

		memcpy(buffer, &playcontrol_rsp_data[offset], bytes_to_copy);
		return bytes_to_copy;
	}
	else
		return -1;
}

int16_t playcontrol_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    uint16_t uuid16 ;
    
	uuid16 = GetUuidForHandle(attribute_handle);
	if(uuid16 == 0x2902)
	{
		playcontrol_rsp_client_configuration = buffer[0]+(buffer[1]<<8);
		if(playcontrol_rsp_client_configuration)
		{
			/*
			at here, you can start a timer to indication data to phone app
			in timer process function you can like following:
			if(playcontrol_rsp_client_configuration == 0x02)
			GattServerIndicate(...);
			else if(playcontrol_rsp_client_configuration == 0x01)
			GattServerNotify(...);
			*/
		}
	}
	else if(uuid16 == 0xa040)
	{
		switch(transaction_mode)
		{
			case TRANSACTION_MODE_NONE:
				memcpy(playcontrol_cmd_data + offset, buffer, buffer_size);
				playcontrol_cmd_data_len = buffer_size;
				ParseCommandData(playcontrol_cmd_data, playcontrol_cmd_data_len);
				playcontrol_cmd_data_len = 0;
			break;

			case TRANSACTION_MODE_ACTIVE:
				memcpy(playcontrol_cmd_data + offset, buffer, buffer_size);
				playcontrol_cmd_data_len += buffer_size;
			break;

			case TRANSACTION_MODE_EXECUTE:
				ParseCommandData(playcontrol_cmd_data, playcontrol_cmd_data_len);
				playcontrol_cmd_data_len = 0;
			break;

			case TRANSACTION_MODE_CANCEL:
				memset(playcontrol_cmd_data, 0, 256);
				playcontrol_cmd_data_len = 0;
			break;
		}
	}

	return 0;
}












#define BLE_CMD_CHECK_PERIOD_MS 50
extern AUDIO_PLAYER_CONTROL*  gPlayContrl;

static uint8_t isConmandError = 0;
uint16_t prevCmd= 0xffff;

static SW_TIMER BleCommandTimerHandle;
static bool bBleCommandTimer = FALSE;


uint8_t calc_checksum(uint8_t* Buf, uint8_t Len)
{
    uint8_t CheckSum=0;
    uint8_t i=0;

    while(i < Len)
    {
        CheckSum += Buf[i];
        i++;
    }
    CheckSum = 0 - CheckSum;
    return CheckSum;
}

static void BleCommandTimerCB(void* Param)
{
	if(GetModeSwitchState() == MODE_SWITCH_STATE_DONE)
	{
		uint8_t rsp_buf[1];
		
		bBleCommandTimer = FALSE;
		StopTimer(&BleCommandTimerHandle);
		DeinitTimer(&BleCommandTimerHandle);

		if(prevCmd == SLAVE_SELECT_MODE)
			rsp_buf[0] = (uint8_t)gSys.CurModuleID;
		else if(prevCmd == SLAVE_NEXT_MODE)
			rsp_buf[0] = (uint8_t)gSys.NextModuleID;
		ResponseCommand(prevCmd, rsp_buf, 1);

		// send the response data to client
		GattServerIndicate(playcontrol_response_handle, playcontrol_rsp_data, playcontrol_rsp_data_len);
	}
}

void BleCommandTimerDetect(void)
{
	if(bBleCommandTimer)
	{
		CheckTimer(&BleCommandTimerHandle);
	}
}

void WaitCommandComplete(void)
{
	if(InitTimer(&BleCommandTimerHandle, BLE_CMD_CHECK_PERIOD_MS, BleCommandTimerCB))
	{
		StartTimer(&BleCommandTimerHandle);
		bBleCommandTimer = TRUE;

		playcontrol_rsp_data_len = 0;

	}
}

void ParseCommandData(uint8_t* data, uint8_t data_len)
{
	uint8_t ErrorAck[1] = {0xFF};
	uint8_t ErrorExcute[1] = {0x7F};
	uint16_t cmd = (data[3]<<8)+data[4];
//	uint8_t checksum = data[5+data[2]] ;
	
	if(data[5 + data[2]] != calc_checksum(&data[2], data[2] + 3))
	{
		ResponseCommand(cmd, ErrorAck, 1);            
		return;
	}

	PlayCtrlProcess(data+3, data[2]);

	if(isConmandError)
		ResponseCommand(cmd, ErrorExcute, 1);    
}

void ResponseCommand(uint16_t Cmd, uint8_t* Buf, uint8_t Len)
{
    uint8_t CheckSum;
    uint8_t LenTemp;
//    uint8_t Size_onetimecansend;
    
//    int ret = 0;

    CheckSum = Len +( ((Cmd>>8)&0xff) | 0x80) + (Cmd&0xff);
    LenTemp = Len;
    while(LenTemp)
    {
        CheckSum += Buf[LenTemp - 1];
        LenTemp--;
    }
    
    CheckSum = 0 - CheckSum;
    playcontrol_rsp_data[0] = 0x55;
    playcontrol_rsp_data[1] = 0xAA;
    playcontrol_rsp_data[2] = Len;
    playcontrol_rsp_data[3] = (((Cmd>>8)&0xff) | 0x80);
    playcontrol_rsp_data[4] = (Cmd&0xff);

    if(Len)
    {
        memcpy(playcontrol_rsp_data + 5, Buf, Len);
    }

    playcontrol_rsp_data[Len + 5] = CheckSum;
    playcontrol_rsp_data_len = Len + 6;
}


void PlayCtrlProcess(uint8_t* CmdData, uint8_t datalen)
{
    FOLDER  folder;
    FOLDER  ParentFolder;
    FAT_FILE    file;
    uint8_t temp = 0;
    int16_t 	sTempWord;
    int16_t 	sTempWord2;
    uint16_t*  P;
    uint32_t	sTempDWord;
    uint16_t Cmd = (CmdData[0]<<8)+CmdData[1];
//    uint8_t playcontrol_rsp_buffer[90] = {0};
    
    isConmandError = 1;
    switch(Cmd)
    {
    case SLAVE_NEXT_MODE:
        gSys.NextModuleID = GetNextModeId(gSys.CurModuleID);
        MsgSend(MSG_MODE);
        SetModeSwitchState(MODE_SWITCH_STATE_START);
        isConmandError = 0;
        prevCmd = Cmd;
        WaitCommandComplete();
        break;

    case SLAVE_SELECT_MODE:
        if(IsModeEnvOk((uint32_t)CmdData[2]) == FALSE || (CmdData[2] == gSys.CurModuleID) )
        {
            isConmandError = 0;
            playcontrol_rsp_buffer[0] = (uint8_t)gSys.CurModuleID;
            ResponseCommand(SLAVE_SELECT_MODE, playcontrol_rsp_buffer, 1);
            break;
        }
        else
        {
            gSys.NextModuleID = (uint32_t)CmdData[2];
            MsgSend(MSG_MODE);
            SetModeSwitchState(MODE_SWITCH_STATE_START);
            isConmandError = 0;
            prevCmd = Cmd;
            WaitCommandComplete();
        }
        break;

    case SLAVE_GET_MODE:
        playcontrol_rsp_buffer[0] = (uint8_t)gSys.CurModuleID;
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_MODE, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_DEVICE_LINK:
        temp = IsUDiskLink() ? 1 : 0;
        temp |= IsCardLink() ? 2 : 0;
        temp |= IsLineInLink()? 4 : 0;
        temp |= IsPcLink() ? 8 : 0;
        temp |= IsAvrcpConnected()? 0x10 : 0;
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = temp;
        ResponseCommand(SLAVE_GET_DEVICE_LINK, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_VALID_FOLDER_SUM:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
        playcontrol_rsp_buffer[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
        playcontrol_rsp_buffer[2] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
        playcontrol_rsp_buffer[3] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
        ResponseCommand(SLAVE_GET_VALID_FOLDER_SUM, playcontrol_rsp_buffer, 4);
        break;
    case SLAVE_GET_FILE_SUM:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
        playcontrol_rsp_buffer[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
        ResponseCommand(SLAVE_GET_FILE_SUM, playcontrol_rsp_buffer, 2);
        break;
    case SLAVE_GET_FOLDER_INFO:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        BYTE1(sTempWord) = CmdData[2];
        BYTE0(sTempWord) = CmdData[3];
        BYTE1(sTempWord2) = CmdData[4];
        BYTE0(sTempWord2) = CmdData[5];
        if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
        {
            break;
        }
        //打开相应文件夹
        if(sTempWord == 0)
        {
            if(!FolderOpenByNum(&folder, NULL, sTempWord2))
            {
                break;
            }
        }
        else
        {
            if(!FolderOpenByNum(&ParentFolder, NULL, sTempWord))
            {
                break;
            }
            if(!FolderOpenByNum(&folder, &ParentFolder, sTempWord2))
            {
                break;
            }
        }
        playcontrol_rsp_buffer[0] = (uint8_t)(folder.FolderNum >> 8);
        playcontrol_rsp_buffer[1] = (uint8_t)folder.FolderNum;
        playcontrol_rsp_buffer[2] = (uint8_t)(folder.ValidFolderNum >> 8);
        playcontrol_rsp_buffer[3] = (uint8_t)folder.ValidFolderNum;
        playcontrol_rsp_buffer[4] = (uint8_t)(folder.StartFileNum >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)folder.StartFileNum;
        playcontrol_rsp_buffer[6] = (uint8_t)(folder.IncFileCnt >> 8);
        playcontrol_rsp_buffer[7] = (uint8_t)folder.IncFileCnt;
        playcontrol_rsp_buffer[8] = (uint8_t)(folder.IncFolderCnt >> 8);
        playcontrol_rsp_buffer[9] = (uint8_t)folder.IncFolderCnt;
        playcontrol_rsp_buffer[10] = (uint8_t)(folder.RecFileCnt >> 8);
        playcontrol_rsp_buffer[11] = (uint8_t)folder.RecFileCnt;
        playcontrol_rsp_buffer[12] = (uint8_t)(folder.RecFolderCnt >> 8);
        playcontrol_rsp_buffer[13] = (uint8_t)folder.RecFolderCnt;
        memcpy(&playcontrol_rsp_buffer[14], &folder.ShortName[0], 8);
        if(FolderGetLongName(&folder, &playcontrol_rsp_buffer[23], 66))
        {
            for(P = (uint16_t*)(&playcontrol_rsp_buffer[23]); *P != 0; P++)//计算长文件名长度
            {
                temp++;
            }
            playcontrol_rsp_buffer[22] = temp * 2;
        }
        else
        {
            playcontrol_rsp_buffer[22] = 0;
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_FOLDER_INFO, playcontrol_rsp_buffer, 23 + playcontrol_rsp_buffer[22]);
        break;
    case SLAVE_GET_FILE_INFO:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        BYTE1(sTempWord) = CmdData[2];
        BYTE0(sTempWord) = CmdData[3];
        BYTE1(sTempWord2) = CmdData[4];
        BYTE0(sTempWord2) = CmdData[5];
        if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
        {
            break;
        }
        //打开相应文件夹
        if(sTempWord != 0)
        {
            if(!FolderOpenByNum(&folder, NULL, sTempWord))
            {
                break;
            }

            if(!FileOpenByNum(&file, &folder, sTempWord2))
            {
                break;
            }
        }
        else
        {
            if(!FileOpenByNum(&file, NULL, sTempWord2))
            {
                break;
            }
        }
        playcontrol_rsp_buffer[0] = (uint8_t)(file.FileNumInDisk >> 8);
        playcontrol_rsp_buffer[1] = (uint8_t)file.FileNumInDisk;
        playcontrol_rsp_buffer[2] = (uint8_t)(file.FileNumInFolder >> 8);
        playcontrol_rsp_buffer[3] = (uint8_t)file.FileNumInFolder;
        playcontrol_rsp_buffer[4] = (uint8_t)(file.FolderNum >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)file.FolderNum;
        memcpy(&playcontrol_rsp_buffer[6], &file.ShortName[0], 11);
        if(FileGetLongName(&file, &playcontrol_rsp_buffer[18], 66))
        {
            for(P = (uint16_t*)(&playcontrol_rsp_buffer[18]); *P != 0; P++)//计算长文件名长度
            {
                temp++;
            }
            playcontrol_rsp_buffer[17] = temp * 2;
        }
        else
        {
            playcontrol_rsp_buffer[17] = 0;
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_FILE_INFO, playcontrol_rsp_buffer, 18 + playcontrol_rsp_buffer[17]);
        FileClose(&file);
        break;
        
    case SLAVE_PLAY:
    case SLAVE_PAUSE:
        isConmandError = 0;
        MsgSend(MSG_PLAY_PAUSE);
        ResponseCommand(Cmd, NULL, 0);
        break;
    case SLAVE_STOP:
        isConmandError = 0;
        MsgSend(MSG_STOP);
        ResponseCommand(SLAVE_STOP, NULL, 0);
        break;
    case SLAVE_NEXT:
        isConmandError = 0;
        MsgSend(MSG_NEXT);
        ResponseCommand(SLAVE_NEXT, NULL, 0);
        break;

    case SLAVE_PRE:
        isConmandError = 0;
        MsgSend(MSG_PRE);
        ResponseCommand(SLAVE_PRE, NULL, 0);
        break;

    case SLAVE_FF:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        //MsgSend(SYS_KEY_ID_FAST_FORWARD);
        MsgSend(MSG_FF_START);
        ResponseCommand(SLAVE_FF, NULL, 0);
        break;

    case SLAVE_FB:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        //MsgSend(SYS_KEY_ID_FAST_BACKWARD);
        MsgSend(MSG_FB_START);
        ResponseCommand(SLAVE_FB, NULL, 0);
        break;
    case SLAVE_STOP_FF_FB:
        /*
        if(!isUSBorSD())
        break;
        SuspendTick = 0;
        MessageSend(MSG_FIFO_KEY, MSG_FF_FB_END);
        SlaveSendResp(SLAVE_STOP_FF_FB, SlaveRespBuf, 0);
        */
        break;
    case SLAVE_SEEK_PLAY_TIME:
        if(datalen != 4)
            break;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        sTempDWord = (CmdData[2]<<24) +(CmdData[3]<<16) + (CmdData[4]<<8) +CmdData[5];
        if(sTempDWord <= gPlayContrl->CurSongInfo->duration)
        {
            gPlayContrl->CurPlayTime = sTempDWord;
            if(GetPlayState() == PLAYER_STATE_PLAYING)
            {
                DecoderTaskSeek(gPlayContrl->CurPlayTime);
            }
        }
        ResponseCommand(SLAVE_SEEK_PLAY_TIME, NULL, 0);
        break;
    case SLAVE_PLAY_PAUSE:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        MsgSend(MSG_PLAY_PAUSE);
        ResponseCommand(SLAVE_PLAY_PAUSE, NULL, 0);
        break;
    case SLAVE_SELECT_SONG:
        if(datalen != 4)
            return;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        sTempWord = (CmdData[2]<<8) + CmdData[3];
        sTempWord2 = (CmdData[4]<<8) + CmdData[5];
        if((sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum) || (sTempWord == 0))
        {
            break;
        }
        if(gPlayContrl->CurFolderIndex != sTempWord)
        {
            if(!FolderOpenByNum(&gPlayContrl->FolderHandle, NULL, sTempWord))
            {
                break;
            }
        }
        if((sTempWord2 <= 0) || (sTempWord2 > gPlayContrl->FolderHandle.RecFileCnt))
        {
            break;
        }   /**/
        // 停止播放
        AudioPlayerStop();
        gPlayContrl->CurFileIndex = sTempWord2;
        gPlayContrl->CurFolderIndex = sTempWord;

        AudioPlayerPlayPause();

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 1;
        ResponseCommand(SLAVE_SELECT_SONG, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_PLAY_STATUS:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.CurModuleID;
        playcontrol_rsp_buffer[1] = gPlayContrl->State;
        playcontrol_rsp_buffer[2] = ((gPlayContrl->CurFolderIndex)>>8)&0xff;
        playcontrol_rsp_buffer[3] = (gPlayContrl->CurFolderIndex)&0xff;
        playcontrol_rsp_buffer[4] = ((gPlayContrl->FolderHandle.ValidFolderNum)>>8)&0xff;
        playcontrol_rsp_buffer[5] = (gPlayContrl->FolderHandle.ValidFolderNum)&0xff;
        playcontrol_rsp_buffer[6] = ((gPlayContrl->CurFileIndex)>>8)&0xff;
        playcontrol_rsp_buffer[7] = ((gPlayContrl->CurFileIndex)&0xff);
        playcontrol_rsp_buffer[8] = ((gPlayContrl->PlayerFile.FileNumInFolder)>>8)&0xff;
        playcontrol_rsp_buffer[9] = (gPlayContrl->PlayerFile.FileNumInFolder)&0xff;
        playcontrol_rsp_buffer[10] = ((gPlayContrl->CurPlayTime)>>24)&0xff;
        playcontrol_rsp_buffer[11] = ((gPlayContrl->CurPlayTime)>>16)&0xff;
        playcontrol_rsp_buffer[12] = ((gPlayContrl->CurPlayTime)>>8)&0xff;
        playcontrol_rsp_buffer[13] = ((gPlayContrl->CurPlayTime)&0xff);
        ResponseCommand(SLAVE_GET_PLAY_STATUS, playcontrol_rsp_buffer, 14);
        break;
    case SLAVE_GET_SONG_INFO:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gPlayContrl->CurSongInfo->stream_type;
        playcontrol_rsp_buffer[1] = (uint8_t)gPlayContrl->CurSongInfo->num_channels;
        playcontrol_rsp_buffer[2] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 24);
        playcontrol_rsp_buffer[3] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 16);
        playcontrol_rsp_buffer[4]  = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)gPlayContrl->CurSongInfo->sampling_rate;

        playcontrol_rsp_buffer[6] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 24);
        playcontrol_rsp_buffer[7] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 16);
        playcontrol_rsp_buffer[8] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 8);
        playcontrol_rsp_buffer[9] = (uint8_t)gPlayContrl->CurSongInfo->bitrate;
        playcontrol_rsp_buffer[10] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 24);
        playcontrol_rsp_buffer[11] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 16);
        playcontrol_rsp_buffer[12] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 8);
        playcontrol_rsp_buffer[13] = (uint8_t)gPlayContrl->CurSongInfo->duration;
        playcontrol_rsp_buffer[14] = (uint8_t)gPlayContrl->CurSongInfo->vbr_flag;
        ResponseCommand(SLAVE_GET_SONG_INFO, playcontrol_rsp_buffer, 15); //17
        break;
    case SLAVE_SET_REPEAT_MODE:
        if(datalen != 1)
            break;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        if(CmdData[2] < PLAY_MODE_SUM)
        {
            gPlayContrl->CurPlayMode = CmdData[2];
        }

        ResponseCommand(SLAVE_SET_REPEAT_MODE, NULL, 0);
        break;
    case SLAVE_GET_REPEAT_MODE:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] =  gPlayContrl->CurPlayMode;
        ResponseCommand(SLAVE_GET_REPEAT_MODE, playcontrol_rsp_buffer, 1);
        break;
#if 1//def FUNC_TAG_EN	
    case SLAVE_GET_TITLE:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->title[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->title[0], temp);
        ResponseCommand(SLAVE_GET_TITLE, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_ARTIST:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->artist[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->artist[0], temp);
        ResponseCommand(SLAVE_GET_ARTIST, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_ALBUM:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->album[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->album[0], temp);
        ResponseCommand(SLAVE_GET_ALBUM, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_COMMENT:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->comment[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->comment[0], temp);
        ResponseCommand(SLAVE_GET_COMMENT, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_YEAR:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->year[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->year[0], temp);
        ResponseCommand(SLAVE_GET_YEAR, playcontrol_rsp_buffer, temp + 3);
        break;
#endif
    case SLAVE_VOLADD:
    	isConmandError = 0;
    	MsgSend(MSG_VOL_UP);
    	ResponseCommand(SLAVE_VOLADD, playcontrol_rsp_buffer, 0);
    	break;

    case SLAVE_VOLSUB:
    	isConmandError = 0;
    	MsgSend(MSG_VOL_DW);
    	ResponseCommand(SLAVE_VOLSUB, playcontrol_rsp_buffer, 0);
    	break;
    case SLAVE_SET_VOL:
        if(CmdData[2] <= MAX_VOLUME)
        {
            gSys.Volume = CmdData[2];
        }
#ifdef FUNC_LINEIN_EN
        if(gSys.CurModuleID == MODULE_ID_LINEIN)    //line in
        {
            SetSysVol();
        }
#endif
#ifdef FUNC_RADIO_EN
        else if(gSys.CurModuleID == MODULE_ID_RADIO)
        {
            //RadioSetVolume(gSys.Volume);
        }
        else
#endif
        {
            SetSysVol();
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_SET_VOL, playcontrol_rsp_buffer, 0);
        break;
    case SLAVE_GET_VOL:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.Volume;
        ResponseCommand(SLAVE_GET_VOL, playcontrol_rsp_buffer, 1);
        break;
    case SLAVE_SET_EQ:
        if(CmdData[2] < EQ_STYLE_SUM)
        {
            if(gSys.Eq == EQ_STYLE_OFF && CmdData[2] != EQ_STYLE_OFF)
            {
                EqEnable();
            }
            else if(gSys.Eq != EQ_STYLE_OFF && CmdData[2] == EQ_STYLE_OFF)
            {
                EqDisable();
            }
            gSys.Eq = CmdData[2];
            if(gPlayContrl->CurSongInfo != NULL)
            {
				uint32_t SampleRateTmp = DacAdcSampleRateGet();
				if(SampleRateTmp == 0)
				{
					SampleRateTmp = 44100;
				}

				EqStyleSelect(p_gSwEq, SampleRateTmp, gSys.Eq);
			}
            isConmandError = 0;
            ResponseCommand(SLAVE_SET_EQ, playcontrol_rsp_buffer, 0);
        }
        break;
    case SLAVE_GET_EQ:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.Eq;
        ResponseCommand(SLAVE_GET_EQ, playcontrol_rsp_buffer, 1);
        break;
    case SLAVE_MUTE:
        isConmandError = 0;
        if(!gSys.MuteFlag)
        {
            gSys.MuteFlag = TRUE;
            DacSoftMuteSet(TRUE, TRUE);
        }
        ResponseCommand(SLAVE_MUTE, playcontrol_rsp_buffer, 0);
        break;
    case SLAVE_UNMUTE:
        isConmandError = 0;
        if(gSys.MuteFlag)
        {
            gSys.MuteFlag = FALSE;
#ifdef FUNC_LINEIN_EN
            if(gSys.CurModuleID == MODULE_ID_LINEIN)    //line in
            {
                SetSysVol();
            }
            else
#endif
#ifdef FUNC_RADIO_EN
                if(gSys.CurModuleID == MODULE_ID_RADIO)
                {
                    //RadioSetVolume(gSys.Volume);
                }
                else
#endif
                {
                    SetSysVol();
                }
            DacSoftMuteSet(FALSE, FALSE);
        }
        ResponseCommand(SLAVE_UNMUTE, playcontrol_rsp_buffer, 0);
        break;
    }
}
