#ifndef   __LP_MDD_H__
#define   __LP_MDD_H__


extern LPHANDLE  lp_mdd_init(_pf_lp_callback cbk) __FAR__;
extern void  lp_mdd_event_process(LPHANDLE handle) __FAR__;
extern LPHANDLE  lp_get_mdd_handle(void) __FAR__;


#endif
