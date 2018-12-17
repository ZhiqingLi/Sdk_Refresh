



#ifndef __BT_DDB_FLASH_H__
#define __BT_DDB_FLASH_H__

#include "type.h"

uint8_t BtDdb_Open(const uint8_t * localBdAddr);

bool BtDdb_Close(void);

bool BtDdb_GetFirstRecord(BT_DB_RECORD * record);

bool BtDdb_GetNextRecord(BT_DB_RECORD * record);

bool BtDdb_PrepareVaildFreeSpace(void);

bool BtDdb_AddOneRecord(const BT_DB_RECORD * record);

#endif //__BT_DDB_FLASH_H__

