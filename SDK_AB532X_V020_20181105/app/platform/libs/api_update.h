#ifndef _API_UPDATE_H
#define _API_UPDATE_H

int updatefile_init(const char *file);     //初始化UPDATE模块
int updateota_init(void);                  //初始化OTA模块
void updateproc(void);                      //升级流程

#endif
