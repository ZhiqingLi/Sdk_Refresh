#ifndef _FUNC_FMAM_H
#define _FUNC_FMAM_H


void func_fmam_message(u16 msg);

#if (GUI_SELECT != GUI_NO)
void func_fmam_display(void);
#else
#define func_fmam_display()
#endif
#endif // _FUNC_FMAM_H
