//////////////////////////////////////////////////////////////////////
//
// pseudo-codes for BLE application
// 
//

#include "ble_api.h"
#include "os.h"
#include "debug.h"

/***********************************************************************
* Declare the callback function
*/
// the function will be called in data transfer
uint16_t att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

uint32_t ConfigAdvertisementData(uint8_t * adver_data, uint32_t data_len);
uint32_t ConfigScanResponseData(uint8_t * scan_resp_data, uint32_t data_len);


uint8_t * g_profile_data = NULL;
BLE_APP_CONTEXT			g_app_context;
GATT_SERVER_PROFILE		g_profile;
GAP_MODE				g_gap_mode;
static uint8_t 			scan_response_data[32];
static uint8_t 			advertisement_data[32];



/***********************************************************************
* create profile data
*/

//Create profile data by using the function "CreateProfileData()"
// Add code here.....

//uuid = 0x1800; Generic Access Service
static uint8_t 		primary_service_uuid[] = {0x00, 0x18};

static uint8_t characteristic_declare_2a00[] = {0x0a, 0x05, 0x00, 0x00,0x2a};//gap name
static uint8_t characteristic_declare_2a01[] = {0x0a, 0x07, 0x00, 0x01, 0x2a};
static uint8_t characteristic_declare_2a02[] = {0x0a, 0x09, 0x00, 0x02, 0x2a};
static uint8_t characteristic_declare_2a03[] = {0x0a, 0x0b, 0x00, 0x03, 0x2a};

static uint8_t			gap_device_name[20] = "PlayControlDemo";
static uint16_t 		gap_appearance = 0;
static uint8_t			gap_reconnection_address[6] = {0x11, 0x22,0x33,0x44,0x55, 0x66};
static int 				gap_privacy = 0;

attribute_item_t gap_attribute_list[] = 
{
        {0xf002, 0x01, 0x2800, 0, primary_service_uuid, 2},
        {0xf002, 0x04, 0x2803, 0, characteristic_declare_2a00,5 },
        {0xf10a, 0x05, 0x2a00, 0, gap_device_name, 20},//gap name
        {0xf002, 0x06, 0x2803, 0, characteristic_declare_2a01,5 },
        {0xf10a, 0x07, 0x2a01, 0, (uint8_t*)&gap_appearance,2},//apperance
        {0xf002, 0x08, 0x2803, 0, characteristic_declare_2a02,5 },
        {0xf10a, 0x09, 0x2a02, 0, (uint8_t*)&gap_privacy,1},//privacy
        {0xf002, 0x0a, 0x2803, 0, characteristic_declare_2a03,5 },
        {0xf10a, 0x0b, 0x2a03, 0, gap_reconnection_address,6}, //reconnection address
};

// uuid = 0xa032; 
uint8_t playcontrol_service_uuid[] = {0x32, 0xa0};//0xa032;
uint8_t playcontrol_characteristic_cmd_declare[] = {0x08, 0x03, 0x01, 0x40,0xa0};//conmand,write only
uint8_t playcontrol_characteristic_result_declare[] = {0x02, 0x05, 0x01, 0x41,0xa0};//conmand excute result, read only
uint8_t playcontrol_characteristic_modechange_result_declare[] = {0x20, 0x07, 0x01, 0x42,0xa0};//conmand excute result, read only
uint8_t playcontrol_rsp_client_configuration = 0;

static uint8_t playcontrol_cmd_data[256];		// 根据spp文档，此处buffer不超过255字节
static uint8_t playcontrol_rsp_data[256];

attribute_item_t playcontrol_attribute[] =
{
    {0xf002, 0x0080, 0x2800, NULL, playcontrol_service_uuid, 2},
    {0xf002, 0x0090, 0x2803, NULL, playcontrol_characteristic_modechange_result_declare, 5},//spp conmand excute result, read only
    {0xf020, 0x0091, 0xa042, NULL, playcontrol_rsp_data,7}, 
    {0xf10a, 0x0092, 0x2902, NULL, &playcontrol_rsp_client_configuration,2}, //new add for phone trigering the data send
    {0xf002, 0x0102, 0x2803, NULL, playcontrol_characteristic_cmd_declare, 5},//spp playcontrol conmand, write only
    {0xf108, 0x0103, 0xa040, NULL, playcontrol_cmd_data,256},
    {0xf002, 0x0104, 0x2803, NULL, playcontrol_characteristic_result_declare, 5},//spp conmand excute result, read only
    {0xf102, 0x0105, 0xa041, NULL, playcontrol_rsp_data,256},     
};

int8_t InitBlePlaycontrolProfile(void)
{
	uint32_t		profile_size = 0;
	uint32_t		profile_offset = 0;

	uint32_t		adv_len;
	uint32_t		scan_resp_len;


	// role
	g_app_context.ble_device_role = PERIPHERAL_DEVICE;

	// create profile
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

	g_profile.profile_data	= g_profile_data;
	g_profile.attr_read		= att_read;
	g_profile.attr_write	= att_write;


	// set gap mode
	g_gap_mode.broadcase_mode		= NON_BROADCAST_MODE;
	g_gap_mode.discoverable_mode	= GENERAL_DISCOVERABLE_MODE;
	g_gap_mode.connectable_mode		= UNDIRECTED_CONNECTABLE_MODE;
	g_gap_mode.bondable_mode		= NON_BONDABLE_MODE;
	SetGapMode(g_gap_mode);

	// set adv data
	adv_len = ConfigAdvertisementData(advertisement_data, sizeof(advertisement_data));
	SetAdvertisingData(advertisement_data, adv_len);

	// set scan response data
	scan_resp_len = ConfigScanResponseData(scan_response_data, sizeof(scan_response_data));
	SetScanResponseData(scan_response_data, scan_resp_len);

}

/***********************************************************************
* set advertisement data
*/

// make an advertisement data, and set the advertisement data with function SetAdvertisingData();
// the length of advertisement data is limit in 32 bytes
// Add code here...

uint32_t ConfigAdvertisementData(uint8_t * adver_data, uint32_t data_len)
{
	uint32_t		len = 0;
	uint8_t			flag;

	if(adver_data == NULL || data_len == 0)
		return ;

	// init buffer
	memset(adver_data, 0 , data_len);

	// flag
	adver_data[len++] = 2;
	adver_data[len++] = 0x01;
	adver_data[len++] = 2;	//GENERAL_DISCOVERABLE_MODE

	// name
	adver_data[len++] = strlen(gap_device_name) + 1;
	adver_data[len++] = 0x09;
	memcpy(&adver_data[len], gap_device_name, strlen(gap_device_name));
	len += strlen(gap_device_name);
	
	// service uuid
	advertisement_data[len++] = 3;
	advertisement_data[len++] = 0x03;
	memcpy(&advertisement_data[len], playcontrol_service_uuid, 2);
	
	len += 2;

	return len;
}

/***********************************************************************
* set scan response data
*/

// make an advertisement data, and set the advertisement data with function SetAdvertisingData();
// the length of advertisement data is limit in 32 bytes
// Add code here...

uint32_t ConfigScanResponseData(uint8_t * scan_resp_data, uint32_t data_len)
{
	uint32_t		len = 0;
	uint8_t			flag; 

	if(scan_resp_data == NULL || data_len == 0)
		return ;

	// init buffer
	memset(scan_resp_data, 0 , data_len);

	// service uuid
	scan_resp_data[len++] = 5;
	scan_resp_data[len++] = 0x03;
	memcpy(&scan_resp_data[len], primary_service_uuid, 2);
	len += 2;
	
	return len;
}



/*
* Implement of the callback function
*/
uint16_t playcontrol_att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
	return 0;
}

int playcontrol_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
	return 0;
}
uint16_t att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
	uint16_t	uuid16 = GetUuidForHandle(attribute_handle);
	
	APP_DBG("ble att read handle = %04x, uuid16 = %04x\n", attribute_handle, uuid16);

	if(attribute_handle >= 0x0080 && attribute_handle <= 0x0105)
	{
		return playcontrol_att_read(con_handle, attribute_handle, offset, buffer, buffer_size);
	}
	else
	{
		switch(uuid16)
		{
			case 0x2a00:
				//...
				break;
			//
			// ...
			//

			
		}
	}
	return 0;
}


int att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
	uint16_t 	uuid16 = GetUuidForHandle(attribute_handle);

	APP_DBG("ble att write con_handle = %04x, attri_handle = %04x, mode = %d, offset = %d, buffer_size = %d\n", 
							con_handle, attribute_handle, transaction_mode, offset, buffer_size);

	if(attribute_handle >= 0x0080 && attribute_handle <= 0x0105)
	{
		return playcontrol_att_write(con_handle, attribute_handle, transaction_mode, offset, buffer, buffer_size);
	}
	else
	{
		switch(uuid16)
		{
			case 0x2a00:
				//...
				break;
			//
			// ...
			//

			
		}
	}
    return 0;
}



