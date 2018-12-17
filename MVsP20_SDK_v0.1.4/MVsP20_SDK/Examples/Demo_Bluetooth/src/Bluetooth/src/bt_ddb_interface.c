

#include "bt_stack_api.h"
#include "bt_platform_interface.h"
#include "bt_trust_list.h"


static bool OpenBtRecord(const uint8_t * localBdAddr);
static bool CloseBtRecord(void);
static bool AddBtRecord(const BT_DB_RECORD * btDbRecord);
static bool DeleteBtRecord(const uint8_t * bdAddr);
static bool FindBtRecord(const uint8_t * bdAddr, BT_DB_RECORD * btDbRecord);
static bool FindBtRecordByIndex(uint8_t index, BT_DB_RECORD * btDbRecord);




PLATFORM_INTERFACE_BT_DDB_T	pfiBtDdb = {
		OpenBtRecord,
		CloseBtRecord,
		AddBtRecord,
		DeleteBtRecord,
		FindBtRecord,
		FindBtRecordByIndex
};


static bool OpenBtRecord(const uint8_t * localBdAddr)
{

	if(localBdAddr == NULL)
		return FALSE;

	if(!InitTrustList(localBdAddr, 0))
		return FALSE;

	return TRUE;
}

static bool CloseBtRecord(void)
{
	if(!DeinitTrustList())
		return FALSE;

	return TRUE;
}

static bool AddBtRecord(const BT_DB_RECORD * btDbRecord)
{
	if(!InsertRecordToTrustList(btDbRecord, btDbRecord->trusted))
		return FALSE;

	return TRUE;
}

static bool DeleteBtRecord(const uint8_t * remoteBdAddr)
{
	if(!RemoveRecordFromTrustList(remoteBdAddr))
		return FALSE;

	return TRUE;
}

static bool FindBtRecord(const uint8_t * remoteBdAddr, BT_DB_RECORD * btDbRecord)
{
	if(!FindRecordInTrustList(remoteBdAddr, btDbRecord))
		return FALSE;

	return TRUE;
}

static bool FindBtRecordByIndex(uint8_t index, BT_DB_RECORD * btDbRecord)
{

	if(!FindRecordInTrustListByIndex(index, btDbRecord))
		return FALSE;

	return TRUE;
}



