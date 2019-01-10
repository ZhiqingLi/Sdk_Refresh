#ifndef __KNOWLES_MIC_DEMO_H__
#define __KNOWLES_MIC_DEMO_H__


#define LIB_ID2_EVENT_NUM 9


enum sink_state
{
	deviceLimbo,
	deviceConnectable,
	deviceConnDiscoverable,	
	deviceConnected,
	deviceA2DPStreaming,
	deviceIncomingCallEstablish,
	deviceCallEnd,
	deviceActiveCallSCO,
	deviceActiveCallNoSCO,
};


#ifdef __cplusplus
extern "C" {
#endif










#ifdef __cplusplus
}
#endif

#endif

