/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * RAM-based file storage
 * $Revision: 1.3 $
 */

#include "mu_list.h"
#include "mu_mem.h"
#include "mu_strng.h"

#include "mu_ramfl.h"

static MUSB_LinkedList MGC_RamFileList;
static uint8_t MGC_bRamFileInit = FALSE;

static void MGC_RamFileCheck()
{
    if(!MGC_bRamFileInit)
    {
	MUSB_ListInit(&MGC_RamFileList);
	MGC_bRamFileInit = TRUE;
    }
}

/*
* implementation
*/
MUSB_RamFileInfo* MUSB_RamFileCreate(uint32_t dwSize, const char* pszName)
{
    uint8_t* pBuffer;
    char* pName;
    MUSB_RamFileInfo* pItem = NULL;

    MGC_RamFileCheck();
    pBuffer = (uint8_t*)MUSB_MemAlloc(dwSize);
    if(pBuffer)
    {
	pItem = (MUSB_RamFileInfo*)MUSB_MemAlloc(sizeof(MUSB_RamFileInfo));
	if(pItem)
	{
	    pItem->dwSize = dwSize;
	    pName = (char*)&(pItem->szName[0]);
	    *pName = (char)0;
	    MUSB_StringConcat(pName, MUSB_RAMFILE_MAX_NAME, pszName);
	    pItem->pData = pBuffer;
	    MUSB_ListAppendItem(&MGC_RamFileList, pItem, 0);
	}
	else
	{
	    MUSB_MemFree(pBuffer);
	    pBuffer = NULL;
	}
    }

    return pItem;
}

/*
* implementation
*/
uint8_t MUSB_RamFileDelete(const char* pszName)
{
    MUSB_LinkedList* pEntry;
    MUSB_RamFileInfo* pItem;
    uint8_t bOk = FALSE;

    MGC_RamFileCheck();
    pEntry = &MGC_RamFileList;
    while(pEntry)
    {
	pItem = (MUSB_RamFileInfo*)pEntry->pItem;
	if(0 == MUSB_StringCompare(pszName, pItem->szName, FALSE))
	{
	    MUSB_ListRemoveItem(&MGC_RamFileList, pItem);
	    MUSB_MemFree(pItem->pData);
	    MUSB_MemFree(pItem);
	    break;
	}
	pEntry = pEntry->pNext;
    }

    return bOk;
}

/*
* implementation
*/
uint16_t MUSB_RamFileCount()
{
    MGC_RamFileCheck();
    return MUSB_ListLength(&MGC_RamFileList);
}

/*
* implementation
*/
const MUSB_RamFileInfo* MUSB_RamFileGetInfo(uint16_t wIndex)
{
    MGC_RamFileCheck();
    return (MUSB_RamFileInfo*)MUSB_ListFindItem(&MGC_RamFileList, wIndex);
}
