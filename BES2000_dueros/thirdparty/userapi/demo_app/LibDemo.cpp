#include "cmsis_os.h"
#include "cmsis.h"
#include "cqueue.h"
#include "hal_trace.h"
#include "resources.h"
#include "app_utils.h"
#include <string.h>


// for audio
#include "audioflinger.h"
#include "app_audio.h"
#include "app_bt_stream.h"
#include "app_media_player.h"

#include "demo_lib.h"
#include "LibDemo.h"


int demo_example_init(bool on,void *param)
{
	TRACE("demo_example_init");
}

int demo_example_start(bool on,void *param)
{
	TRACE("demo_example_start");
}

int demo_example_stop(bool on,void *param)
{
	TRACE("demo_example_stop");
}


#include "app_thirdparty.h"


THIRDPARTY_EVENT_HANDLER_TAB_ADD(demo,LIB_DEMO_EVENT_NUM)
{
    {{THIRDPARTY_ID_DEMO,THIRDPARTY_START},(APP_THIRDPARTY_HANDLE_CB_T)demo_example_start,true,NULL},
    {{THIRDPARTY_ID_DEMO,THIRDPARTY_STOP},(APP_THIRDPARTY_HANDLE_CB_T)demo_example_stop,true,NULL},
    {{THIRDPARTY_ID_DEMO,THIRDPARTY_INIT},(APP_THIRDPARTY_HANDLE_CB_T)demo_example_init,true,NULL},
};




