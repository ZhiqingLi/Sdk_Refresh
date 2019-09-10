#ifndef __DUMP_H__
#define __DUMP_H__

#include "type.h"

#define  DUMP_FLAG      1

bool IsSdInitedAndFsMountedOK(void);
bool SdInitAndFsMount(void);
bool OpenDumpDataFile(void);
void OpenDumpDataFileInAppendMode(uint8_t* name);
bool IsDumpFileOpenedOk(void);
void DumpData(void* data, uint32_t len);
void SaveDumpDataFile(void);
void CloseDumpDataFile(void);

void DumpBufferInit(uint8_t* buffer, uint32_t capacity);
uint32_t DumpBufferRemain(void);
uint32_t DumpBufferPush(uint8_t* buffer, uint32_t length);
uint32_t DumpBufferPop(void);
uint32_t DumpBufferPopByBlock(void);

#endif//__DUMP_H__
