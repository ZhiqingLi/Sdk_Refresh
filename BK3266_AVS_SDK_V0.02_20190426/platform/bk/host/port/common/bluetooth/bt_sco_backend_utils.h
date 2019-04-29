/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef __SCO_BACKEND_UTILS__
#define __SCO_BACKEND_UTILS__
#include <bluetooth.h>

typedef void* sco_utils_h;

result_t util_sco_open(const btaddr_t *laddr, const btaddr_t *raddr, 
    sco_utils_h *priv);
void util_sco_close(sco_utils_h priv);
result_t start_sco_test_call(sco_utils_h priv);
void stop_sco_test_call(sco_utils_h priv);
result_t util_sco_connect(sco_utils_h priv);
result_t util_sco_disconnect(sco_utils_h priv);
uint32_t a2dp_has_music(void);

#if PTS_TESTING
result_t util_sco_connect_pts(sco_utils_h priv);
#endif

#endif
