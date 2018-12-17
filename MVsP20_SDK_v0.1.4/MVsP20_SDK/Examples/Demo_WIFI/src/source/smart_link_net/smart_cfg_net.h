#ifndef		__SMART_CFG_NET_H__
#define		__SMART_CFG_NET_H__






#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

struct  ap_ssid_pwd {
	uint8_t ap_ssid[40];
    uint8_t ap_ssid_len;
    uint8_t ap_password[40];
    uint8_t ap_password_len;
};

struct ap_record {
	uint8_t total_items;
    uint8_t current_item;
    struct  ap_ssid_pwd ssid_pwd[10];
}

#ifdef __cplusplus
}
#endif//__cplusplus

#endif	//__SMART_CFG_NET_H__
