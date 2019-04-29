#ifndef  __LP_MESSAGE_H__
#define __LP_MESSAGE_H__

#include "lp_type.h"

extern LPStatus  LP_PostMessage(LPHANDLE handle, uint8_t event, uint8_t type, uint16_t Inparam, uint8_t *extra, uint32_t extra_len) __FAR__;
extern LPStatus  LP_SendMessage(LPHANDLE handle, uint8_t event, uint8_t type, uint16_t Inparam, uint8_t *extra, uint32_t extra_len) __FAR__;
extern LPStatus  LP_PeekMessge(LPHANDLE handle, LPMSG **pMsg, uint8 remove);
extern LPStatus  LP_GetMessage(LPHANDLE handle , LPMSG **pMsg);
extern LPStatus  LP_ReleaseMessage(LPMSG *pMsg) __FAR__;
extern LPStatus  LP_DispatchMessage(LPHANDLE handle, LPMSG *pMsg) __FAR__;


#endif
