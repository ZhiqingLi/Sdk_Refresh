#ifndef __BT_SCO_CHAIN_ECHO_SUPPRESS_H__
#define __BT_SCO_CHAIN_ECHO_SUPPRESS_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_sco_chain_echo_suppress_init(int sampleRate);

int bt_sco_chain_echo_suppress_deinit(void);

uint32_t bt_sco_chain_echo_suppress_sendchain(int16_t *buf, int16_t *echo_ref_buf, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif//__BT_SCO_CHAIN_WEBRTC_H__
