

#include <string.h>

#include "type.h"
#include "spi_flash.h"
#include "bt_trust_list.h"


#ifndef DWNLDR_USRDAT_ADDR
#define DWNLDR_USRDAT_ADDR      (0xB4)
#endif

#define MVBT_DB_FLAG						"MVBT_O18"
#define VALID_OCCUPY_BYTES					1
#define BDADDR_OCCUPY_BYTES					6
#define COUNT_OCCUPY_BYTES					1
#define BTDB_ONE_RECORD_SIZE				(sizeof(BT_DB_RECORD))
#define TIME_OUT_MAX						0xFFFF
#define BTDB_TOTAL_SIZE						(4096 * 1)


static uint32_t btDdbValidRecordAddress = 0;
static uint32_t btDdbValidFreeSpaceAddress = 0;
static uint8_t	btTotalRecords = 0;
static uint8_t	btDdbDbAddr[6];

static uint32_t tempReadAddress = 0;
static uint32_t tempWriteAddress = 0;


static uint32_t Flash_GetBtDbOffset(void)
{
	uint32_t		start_offset;

	SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&start_offset, 4, 0);

	return start_offset;
}




uint8_t BtDdb_Open(const uint8_t * localBdAddr)
{
	const uint8_t		mvbt_db_flag[] = MVBT_DB_FLAG;
	uint8_t				tmp_db_flag[] = MVBT_DB_FLAG;
	uint8_t				tmp_valid_flag[VALID_OCCUPY_BYTES];
	uint8_t				tmp_bdAddr[BDADDR_OCCUPY_BYTES];
	uint8_t				tmp_count[COUNT_OCCUPY_BYTES];
	uint32_t			db_offset, last_offset, tmp_offset;



	if(localBdAddr == NULL)
		return 0;

	db_offset = Flash_GetBtDbOffset();

	memset(tmp_db_flag, 0, sizeof(tmp_db_flag));
	last_offset = db_offset;

	memcpy(btDdbDbAddr, localBdAddr, 6);	// save local bluetooth device address

	while(1)
	{
		if(last_offset > (db_offset + BTDB_TOTAL_SIZE))
			break;

		SpiFlashRead(last_offset, tmp_db_flag, sizeof(tmp_db_flag), 0);
		if(memcmp(tmp_db_flag, mvbt_db_flag, sizeof(mvbt_db_flag)) == 0)
		{
			tmp_offset = last_offset;
			tmp_offset += sizeof(mvbt_db_flag);

			SpiFlashRead(tmp_offset, tmp_valid_flag, VALID_OCCUPY_BYTES, 0);
			tmp_offset += VALID_OCCUPY_BYTES;

			SpiFlashRead(tmp_offset, tmp_bdAddr, BDADDR_OCCUPY_BYTES, 0);
			tmp_offset += BDADDR_OCCUPY_BYTES;

			SpiFlashRead(tmp_offset, tmp_count, COUNT_OCCUPY_BYTES, 0);
			tmp_offset += COUNT_OCCUPY_BYTES;
			btTotalRecords = tmp_count[0];
			
			if(tmp_valid_flag[0] == 1) /*valid db*/
			{
				if(memcmp(tmp_bdAddr, localBdAddr, 6) == 0)
				{
					/*
					* here we find the lastest db
					*/
					btDdbValidRecordAddress = tmp_offset;
					return btTotalRecords;
				}
				else
				{
					tmp_offset += BTDB_ONE_RECORD_SIZE * tmp_count[0];
					last_offset = tmp_offset;
				}
			}
			else
			{
				tmp_offset += BTDB_ONE_RECORD_SIZE * tmp_count[0];
				last_offset = tmp_offset;
			}
		}
		else
		{
			last_offset++;
		}
	}

	return 0;
}


bool BtDdb_Close(void)
{

	memset(btDdbDbAddr, 0, 6);	// clear local bluetooth device address

	btDdbValidRecordAddress = 0;

	btDdbValidFreeSpaceAddress = 0;

	btTotalRecords = 0;

	return TRUE;
}

bool BtDdb_GetFirstRecord(BT_DB_RECORD * record)
{
	tempReadAddress = btDdbValidRecordAddress;

	if(tempReadAddress == 0)
		return FALSE;

	SpiFlashRead(tempReadAddress, (uint8_t*)record, BTDB_ONE_RECORD_SIZE, 0);
	tempReadAddress += BTDB_ONE_RECORD_SIZE;

	return TRUE;
}

bool BtDdb_GetNextRecord(BT_DB_RECORD * record)
{
	if(tempReadAddress >= btDdbValidRecordAddress + btTotalRecords * BTDB_ONE_RECORD_SIZE)
		return FALSE;

	SpiFlashRead(tempReadAddress, (uint8_t*)record, BTDB_ONE_RECORD_SIZE, 0);
	tempReadAddress += BTDB_ONE_RECORD_SIZE;

	return TRUE;
}


bool BtDdb_PrepareVaildFreeSpace(void)
{
	const uint8_t		mvbt_db_flag[] = MVBT_DB_FLAG;
	uint8_t				vaild_flag[VALID_OCCUPY_BYTES];
	uint8_t				count_of_devices[COUNT_OCCUPY_BYTES];

	uint8_t				tmp_db_flag[] = MVBT_DB_FLAG;
	uint8_t				tmp_valid_flag[VALID_OCCUPY_BYTES];
	uint8_t				tmp_bdAddr[BDADDR_OCCUPY_BYTES];
	uint8_t				tmp_count[COUNT_OCCUPY_BYTES];

	uint32_t			db_offset, last_offset, tmp_offset;

	db_offset = Flash_GetBtDbOffset();

	memset(tmp_db_flag, 0, sizeof(tmp_db_flag));
	last_offset = db_offset;

	while(1)
	{
		if(last_offset > db_offset + BTDB_TOTAL_SIZE)
			break;

		SpiFlashRead(last_offset, tmp_db_flag, sizeof(tmp_db_flag), 0);
		if(memcmp(tmp_db_flag, mvbt_db_flag, sizeof(mvbt_db_flag)) == 0)
		{
			tmp_offset = last_offset;
			tmp_offset += sizeof(mvbt_db_flag);

			SpiFlashRead(tmp_offset, tmp_valid_flag, VALID_OCCUPY_BYTES, 0);
			tmp_offset += VALID_OCCUPY_BYTES;

			SpiFlashRead(tmp_offset, tmp_bdAddr, BDADDR_OCCUPY_BYTES, 0);
			tmp_offset += BDADDR_OCCUPY_BYTES;

			SpiFlashRead(tmp_offset, tmp_count, COUNT_OCCUPY_BYTES, 0);
			tmp_offset += COUNT_OCCUPY_BYTES;

			if(tmp_valid_flag[0] == 1) /*valid db*/
			{
				if(memcmp(tmp_bdAddr, btDdbDbAddr, 6) == 0)
				{ 
				/*
				* here we find the old db, we should set this db as invaild
				* and write a new db following the old one
				*/
					uint32_t		valid_flag_offset;

					/*
					* erase the vaild flag
					*/
					memset(vaild_flag, 0, VALID_OCCUPY_BYTES);	// set the vaild flag as FALSE
					valid_flag_offset = last_offset + sizeof(mvbt_db_flag);
					SpiFlashWrite(valid_flag_offset, vaild_flag, VALID_OCCUPY_BYTES, 0);

					/*
					* get the vaild free space
					*/

					tmp_offset += BTDB_ONE_RECORD_SIZE * tmp_count[0];

					if((tmp_offset + \
						sizeof(mvbt_db_flag) + \
						VALID_OCCUPY_BYTES + \
						BDADDR_OCCUPY_BYTES + \
						(GetCountOfDevicesFromTrustList() * BTDB_ONE_RECORD_SIZE)) > (db_offset + BTDB_TOTAL_SIZE))
					{
						/*
						* the remain of db flash can not save all of trusted devices
						* so we should erase all of db flash
						*/
//						SpiFlashErase(db_offset, BTDB_TOTAL_SIZE);
						uint16_t		i;
						uint32_t		startSector;

						startSector = db_offset / 1024;

						for(i = 0; i < BTDB_TOTAL_SIZE/4096; i++, startSector++)
						{
							SpiFlashErase(SECTOR_ERASE, startSector, 0);
						}

						tmp_offset = db_offset;
					}

					goto END_FIND;
				}
				else
				{
					tmp_offset += BTDB_ONE_RECORD_SIZE * tmp_count[0];
					last_offset = tmp_offset;
				}
			}
			else
			{
				tmp_offset += BTDB_ONE_RECORD_SIZE * tmp_count[0];
				last_offset = tmp_offset;
			}
		}
		else
		{
			last_offset++;
		}
	}

	/*
	* There is no any matching db, so we should erase all db blcok of flash
	* and write the new db
	*/
	{
//		SpiFlashErase(db_offset, BTDB_TOTAL_SIZE);
		uint16_t		i;
		uint32_t		startSector;

		startSector = db_offset / 1024;
		for(i = 0; i < BTDB_TOTAL_SIZE/4096; i++, startSector++)
		{
			SpiFlashErase(SECTOR_ERASE, startSector, 0);
		}
		tmp_offset = db_offset;
	}


END_FIND:

	/*
	* write record header struct
	*/

	// write the db flag
	SpiFlashWrite(tmp_offset, (uint8_t*)mvbt_db_flag, sizeof(mvbt_db_flag), 0);
	tmp_offset += sizeof(mvbt_db_flag);

	// write the vaild flag
	memset(vaild_flag, 0, VALID_OCCUPY_BYTES);
	vaild_flag[0] = 1;
	SpiFlashWrite(tmp_offset, vaild_flag, VALID_OCCUPY_BYTES, 0);
	tmp_offset += VALID_OCCUPY_BYTES;

	// write local bd address
	SpiFlashWrite(tmp_offset, (uint8_t*)btDdbDbAddr, BDADDR_OCCUPY_BYTES, 0);
	tmp_offset += BDADDR_OCCUPY_BYTES;

	// write the count of trusted devices
	memset(count_of_devices, 0, COUNT_OCCUPY_BYTES);
	count_of_devices[0] = GetCountOfDevicesFromTrustList();
	SpiFlashWrite(tmp_offset, count_of_devices, COUNT_OCCUPY_BYTES, 0);
	tmp_offset += COUNT_OCCUPY_BYTES;


	btDdbValidFreeSpaceAddress = tmp_offset;
	tempWriteAddress = btDdbValidFreeSpaceAddress;

	return TRUE;
}


bool BtDdb_AddOneRecord(const BT_DB_RECORD * record)
{

	SpiFlashWrite(tempWriteAddress, (uint8_t*)record, BTDB_ONE_RECORD_SIZE, 0);
	tempWriteAddress += BTDB_ONE_RECORD_SIZE;

	return TRUE;
}


