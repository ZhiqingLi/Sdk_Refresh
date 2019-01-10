#include "cmsis_os.h"
#include "hal_trace.h"
#include "app_utils.h"
#include "app_thread.h"


static APP_MOD_HANDLER_T mod_handler[APP_MODUAL_NUM];

static osThreadId app_tid;
static void app_thread(void const *argument);
osThreadDef(app_thread, osPriorityHigh, 4096);

osMailQDef (app_mailbox, APP_MAILBOX_MAX, APP_MESSAGE_BLOCK);
static osMailQId app_mailbox = NULL;
static uint8_t app_mailbox_cnt = 0;

static int app_mailbox_init(void)
{
	app_mailbox = osMailCreate(osMailQ(app_mailbox), NULL);
	if (app_mailbox == NULL)  {
        TRACE("Failed to Create app_mailbox\n");
		return -1;
	}
	app_mailbox_cnt = 0;
	return 0;
}

int app_mailbox_put(APP_MESSAGE_BLOCK* msg_src)
{
	osStatus status;

	APP_MESSAGE_BLOCK *msg_p = NULL;

	msg_p = (APP_MESSAGE_BLOCK*)osMailAlloc(app_mailbox, 0);
	ASSERT(msg_p, "osMailAlloc error");
	msg_p->src_thread = (uint32_t)osThreadGetId();
	msg_p->dest_thread = (uint32_t)NULL;
	msg_p->mod_id = msg_src->mod_id;
	msg_p->msg_body.message_id = msg_src->msg_body.message_id;
	msg_p->msg_body.message_ptr = msg_src->msg_body.message_ptr;
	msg_p->msg_body.message_Param0 = msg_src->msg_body.message_Param0;
	msg_p->msg_body.message_Param1 = msg_src->msg_body.message_Param1;

	status = osMailPut(app_mailbox, msg_p);
	if (osOK == status)
		app_mailbox_cnt++;
	return (int)status;
}

int app_mailbox_free(APP_MESSAGE_BLOCK* msg_p)
{
	osStatus status;

	status = osMailFree(app_mailbox, msg_p);
	if (osOK == status)
		app_mailbox_cnt--;

	return (int)status;
}

int app_mailbox_get(APP_MESSAGE_BLOCK** msg_p)
{
	osEvent evt;
	evt = osMailGet(app_mailbox, osWaitForever);
	if (evt.status == osEventMail) {
		*msg_p = (APP_MESSAGE_BLOCK *)evt.value.p;
		return 0;
	}
	return -1;
}

static void app_thread(void const *argument)
{
	while(1){
		APP_MESSAGE_BLOCK *msg_p = NULL;
#ifdef __WATCHER_DOG_RESET__
        app_wdt_ping();
#endif
		if (!app_mailbox_get(&msg_p)){
			if (msg_p->mod_id < APP_MODUAL_NUM)
			mod_handler[msg_p->mod_id](&(msg_p->msg_body));
			app_mailbox_free(msg_p);
		}
	}
}

int app_os_init(void)
{
	if (app_mailbox_init())
		return -1;

	app_tid = osThreadCreate(osThread(app_thread), NULL);
	if (app_tid == NULL)  {
        TRACE("Failed to Create app_thread\n");
		return 0;
	}
	return 0;
}

int app_set_threadhandle(enum APP_MODUAL_ID_T mod_id, APP_MOD_HANDLER_T handler)
{
	if (mod_id>=APP_MODUAL_NUM)
		return -1;

	mod_handler[mod_id] = handler;
	return 0;
}


