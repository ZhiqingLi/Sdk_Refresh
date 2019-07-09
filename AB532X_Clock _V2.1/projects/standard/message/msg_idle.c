#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN
AT(.text.func.idle.msg)
void func_idle_message(u16 msg)
{
	switch (msg)
	{

		default:
		if (msg == KLH_POWER)
		func_message(msg);
		break;
	}
}
#endif // FUNC_IDLE_EN
