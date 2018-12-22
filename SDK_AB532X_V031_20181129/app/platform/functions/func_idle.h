#ifndef _FUNC_IDLE_H
#define _FUNC_IDLE_H

void func_idle_message(u16 msg);

#if (GUI_SELECT != GUI_NO)
void func_idle_display(void);
#else
#define func_idle_display()
#endif

#endif // _FUNC_IDLE_H
