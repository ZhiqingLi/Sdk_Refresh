#include "cmsis_os.h"
#include "cmsis.h"
#include "list.h"
#include "hal_trace.h"
#include <string.h>
#include "app_thirdparty.h"






int app_thirdparty_specific_lib_event_handle(THIRDPARTY_LIB_ID libId,THIRDPARTY_EVENT_TYPE event_type)
{
	int ret;
	APP_THIRDPARTY_HANDLE *dest_handle;
	TRACE("%sevent_type=%d",__func__,event_type);
	for (uint32_t index = 0;
		index < ((uint32_t)__thirdparty_handler_table_end-(uint32_t)__thirdparty_handler_table_start)/sizeof(APP_THIRDPARTY_HANDLE);index++)
	{
		if ((THIRDPARTY_EVENT_TYPE_PTR_FROM_ENTRY_INDEX(index)->thirdparty_sign.event == event_type)&&
			(THIRDPARTY_EVENT_TYPE_PTR_FROM_ENTRY_INDEX(index)->thirdparty_sign.id == libId))
		{
			 TRACE("find index=%d",index);
			 dest_handle=THIRDPARTY_EVENT_TYPE_PTR_FROM_ENTRY_INDEX(index);
			 if(dest_handle){
				ret=((APP_THIRDPARTY_HANDLE_CB_T)dest_handle->function)(dest_handle->status,dest_handle->param);
				return ret;
			 }
		}			
	}
		
	return 0;
}




