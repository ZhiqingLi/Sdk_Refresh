/***************************************************************************
 *
 * File:
 *     $Id: utils.h 1411 2009-04-15 22:10:58Z suneelk $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1411 $
 *
 * Description:
 *     Includes for utility functions.
 *
 * Created:
 *     October 1, 1996
 *
 * Copyright 1996-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __UTILS_H
#define __UTILS_H

#include "xatypes.h"
#include "xastatus.h"

/* For backward compatibility */
BOOL StrnCmp(const U8* str1, U16 len1, const U8* str2, U16 len2);
#define StrnCmp(s1, l1, s2, l2) OS_MemCmp(s1, l1, s2, l2)


/* Min and Max macros.
 */
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif /* max */

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif /* min */

/* Macro to convert lower case ASCII characters to upper case
 */
#ifndef ToUpper
#define ToUpper(ch)  (U8)((((ch) >= 'a' && (ch) <= 'z') ? ((ch) - 'a' + 'A') : (ch)))
#endif

#ifndef ALIGN
/* Data structure alignment macros. Some modules aggregate data allocations,
 * care must be taken to insure that dynamically allocated structures fall
 * on appropriate byte boundries. The configuration constant XA_INTEGER_SIZE
 * is used to force alignment on 16 or 32 bit boundries. All data structures
 * will be sub-allocated on byte boundries that match the XA_INTEGER_SIZE.
 *
 * The ALIGN() macro will grow lengths to a multiple of
 * XA_INTEGER_SIZE byte. When XA_INTEGER_SIZE == 4 the macro will look like:
 * ((length + 3) & 0xFFFFFFFC)
 */
#define ALIGN(_LEN)     (((_LEN) + XA_INTEGER_SIZE - 1) & (U32)(0-XA_INTEGER_SIZE))
#endif


/*---------------------------------------------------------------------------
 * CheckUnlockedParm()
 *
 *     Verifies the validity of calling parameters outside
 *     of a function's OS_LockStack/OS_UnlockStack pair. The behavior of
 *     this macro depends on both XA_ERROR_CHECK and XA_DEBUG.
 *
 *     If XA_ERROR_CHECK is enabled, the expression will be evaluated. If the
 *     expression is false, the macro will return from the current function
 *     with the specified return code.
 *
 *     If XA_DEBUG is enabled, but XA_ERROR_CHECK is disabled, the expression
 *     will be evaluated. If the expression is false, the macro will assert.
 *
 *     If XA_DEBUG and XA_ERROR_CHECK are both disabled, the expression will
 *     not be evaluated.
 *
 * Parameters:
 *     code - Status code to return when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled)
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckUnlockedParm(code, exp) if (!(exp)) return (code)
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckUnlockedParm(code, exp) Assert(exp)
#else /* XA_DEBUG is disabled as well */
#define CheckUnlockedParm(code, exp) (void)0
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */

/*---------------------------------------------------------------------------
 * CheckLockedParm()
 *
 *     Verifies the validity of calling parameters inside
 *     of a function's OS_LockStack/OS_UnlockStack pair. This macro
 *     acts just as CheckUnlockedParm does, except that it will
 *     call OS_UnlockStack before returning with the specified return code.
 *
 * Parameters:
 *     code - Status code to return when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled)
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckLockedParm(code, exp) do { if (!(exp)) { OS_UnlockStack(); \
                        return (code);} } while(0)
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckLockedParm(code, exp) Assert(exp)
#else /* XA_DEBUG is disabled as well */
#define CheckLockedParm(code, exp) (void)0
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */



/*---------------------------------------------------------------------------
 * CheckParmExit()
 *
 *     Verifies the validity of calling parameters. The behavior of
 *     this macro depends on both XA_ERROR_CHECK and XA_DEBUG.
 *
 *     If XA_ERROR_CHECK is enabled, the expression will be evaluated. If the
 *     expression is false, this macro will set the "status" variable to
 *     the specified error code, then goto the "exit" label.
 *
 *     If XA_DEBUG is enabled, but XA_ERROR_CHECK is disabled, the expression
 *     will be evaluated. If the expression is false, the macro will assert.
 *
 *     If XA_DEBUG and XA_ERROR_CHECK are both disabled, the expression will
 *     not be evaluated.
 *
 *     IMPORTANT--This macro requires a local variable (called "status")
 *     to be defined by the function in which this macro occurs. The
 *     "status" variable is referenced by this macro only when XA_ERROR_CHECK
 *     is enabled. This macro requires the exit label "exit", for convenience
 *     the label "Check_Parm_Exit" can be used.
 *
 * Parameters:
 *     code - Status code to assign to "status" when "exp" is false (only when
 *         XA_ERROR_CHECK is enabled).
 *
 *     label - The name of a goto label. Code will jump to this label
 *         when "exp" is false (only when XA_ERROR_CHECK is enabled).
 *
 *     exp - Expression to evaluate
 */
#if XA_ERROR_CHECK == XA_ENABLED
#define CheckParmExit(code, exp) do { if (!(exp)) { status = (code); \
                                      goto exit; } } while(0)
#define Check_Parm_Exit  exit:
#else /* XA_ERROR_CHECK is disabled */
#if XA_DEBUG == XA_ENABLED
#define CheckParmExit(code, exp) Assert(exp)
#define Check_Parm_Exit
#else /* XA_DEBUG is disabled as well */
#define CheckParmExit(code, exp) (void)0
#define Check_Parm_Exit
#endif /* XA_DEBUG */
#endif /* XA_ERROR_CHECK */


/* Little Endian to Host integer format conversion functions */
U16 LEtoHost16(const U8 *le_value);
U32 LEtoHost32(const U8 *le_value);

/* Big Endian to Host integer format conversion functions */
U16 BEtoHost16(const U8* be_ptr);
U32 BEtoHost32(const U8* be_ptr);

/* Store value into a buffer in Little Endian integer format */
void StoreLE16(U8 *buff, U16 le_value);
void StoreLE32(U8 *buff, U32 le_value);

/* Store value into a buffer in Big Endian integer format */
void StoreBE16(U8 *ptr, U16 be_value);
void StoreBE32(U8 *ptr, U32 be_value);

#if XA_USE_ENDIAN_MACROS == XA_ENABLED

/* Little Endian to Host integer format conversion macros */
#define LEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)+1) << 8) | \
                                ((U16) *((U8*)(ptr))) )

#define LEtoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr)+3) << 24) | \
                                ((U32) *((U8*)(ptr)+2) << 16) | \
                                ((U32) *((U8*)(ptr)+1) << 8)  | \
                                ((U32) *((U8*)(ptr))) )

/* Big Endian to Host integer format conversion macros */
#define BEtoHost16(ptr)  (U16)( ((U16) *((U8*)(ptr)) << 8) | \
                                ((U16) *((U8*)(ptr)+1)) )

#define BEtoHost32(ptr)  (U32)( ((U32) *((U8*)(ptr)) << 24)   | \
                                ((U32) *((U8*)(ptr)+1) << 16) | \
                                ((U32) *((U8*)(ptr)+2) << 8)  | \
                                ((U32) *((U8*)(ptr)+3)) )

/* Store value into a buffer in Little Endian format */
#define StoreLE16(buff,num) ( ((buff)[1] = (U8) ((num)>>8)),    \
                              ((buff)[0] = (U8) (num)) )

#define StoreLE32(buff,num) ( ((buff)[3] = (U8) ((num)>>24)),  \
                              ((buff)[2] = (U8) ((num)>>16)),  \
                              ((buff)[1] = (U8) ((num)>>8)),   \
                              ((buff)[0] = (U8) (num)) )

/* Store value into a buffer in Big Endian format */
#define StoreBE16(buff,num) ( ((buff)[0] = (U8) ((num)>>8)),    \
                              ((buff)[1] = (U8) (num)) )

#define StoreBE32(buff,num) ( ((buff)[0] = (U8) ((num)>>24)),  \
                              ((buff)[1] = (U8) ((num)>>16)),  \
                              ((buff)[2] = (U8) ((num)>>8)),   \
                              ((buff)[3] = (U8) (num)) )

#endif /* XA_USE_ENDIAN_MACROS == XA_ENABLED */


/****************************************************************************
 *
 * List Manipulation Utilities
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * ListEntry is for creating doubly linked circular lists. They can be used as
 * either a list head, or as part of another structure to allow that
 * structure to be a link node.
 */
#ifndef LISTS_DEFINED
#define LISTS_DEFINED

typedef struct  _ListEntry
{
    struct _ListEntry *Flink;
    struct _ListEntry *Blink;

} ListEntry;
#endif

/*---------------------------------------------------------------------------
 *
 * The XA Buffer Descriptor is used for managing data buffers that are shared
 * between protocol layers. The number of bytes of data available to read is
 * writeOffset - readOffset.
 */
typedef struct _XaBufferDesc {
    ListEntry    link;          /* For Queuing, must be first */
    U8           flags;         /* Flag storage for use by callers */
    U16          writeOffset;   /* Write offset into buff */
    U16          readOffset;    /* Read offset into buff */
    U16          buffSize;      /* Maximum size of associated buffer */
    U8          *buff;
} XaBufferDesc;

/* Returns the length of data available to read */
#define XaBufferReadLen(b) ((U16)((b)->writeOffset - (b)->readOffset))

/* Writes from specified src into XaBufferDesc */
#define XaBufferWrite(b, src, len) \
    do { \
        OS_MemCopy((b)->buff + (b)->writeOffset, (const U8 *)(src), (len)); \
        (b)->writeOffset = (U16)((b)->writeOffset + (len)); } \
    while (0)

/* Writes a single char into the buffer */
#define XaBufferWriteChar(b, c)  (b)->buff[(b)->writeOffset++] = (c)


#define          XA_WBXML_BUFFER      0
#define          XA_NOT_WBXML_BUFFER  1

/****************************************************************************
 *
 * Functions and Macros Reference.
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 *
 * Used to define unused function parameters. Some compilers warn if this
 * is not done.
 */
#define UNUSED_PARAMETER(_PARM)     ((_PARM) = (_PARM))


/*---------------------------------------------------------------------------
 *
 *  Doubly-linked list manipulation routines.  Some are implemented as
 *  macros but logically are procedures.
 */
#ifndef LIST_MACROS
void InitializeListHead(ListEntry *head);
#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead) )

#define InitializeListEntry(Entry) (\
    (Entry)->Flink = (Entry)->Blink = 0 )

#define IsEntryAvailable(Entry) (\
    ((Entry)->Flink == 0))

#ifndef IsListEmpty
BOOL IsListEmpty(ListEntry *head);
#define IsListEmpty(ListHead) (\
    ((ListHead)->Flink == (ListHead)))
#endif

#define GetHeadList(ListHead) (ListHead)->Flink

#define GetTailList(ListHead) (ListHead)->Blink

#define GetNextNode(Node)     (Node)->Flink

#define GetPriorNode(Node)    (Node)->Blink

#define IsNodeConnected(n) (((n)->Blink->Flink == (n)) && ((n)->Flink->Blink == (n)))

void InsertTailList(ListEntry *head, ListEntry *entry);
void _InsertTailList(ListEntry* head, ListEntry* entry);
#define InsertTailList(a, b) (Assert(IsListCircular(a)), \
                            _InsertTailList(a, b), \
                            Assert(IsListCircular(a)))

void InsertHeadList(ListEntry *head, ListEntry *entry);
void _InsertHeadList(ListEntry* head, ListEntry* entry);
#define InsertHeadList(a, b) (Assert(IsListCircular(a)), \
                            _InsertHeadList(a, b), \
                            Assert(IsListCircular(a)))

ListEntry *RemoveHeadList(ListEntry *head);
ListEntry *_RemoveHeadList(ListEntry* head);
#define RemoveHeadList(a) (Assert(IsListCircular(a)), \
                            _RemoveHeadList(a))

void RemoveEntryList(ListEntry* entry);
BOOL IsNodeOnList(ListEntry* head, ListEntry* node);
BOOL IsListCircular(ListEntry* list);
void MoveList(ListEntry* dest, ListEntry* src);
#endif

/*---------------------------------------------------------------------------
 * IterateListSafe()
 *
 *     Sets up safe traversal of a list, preserving the "next" member so
 *     that the current member can be removed safely (with RemoveEntryList).
 *     Must be followed by a block of code containing the body of the
 *     iteration.
 *
 *     For example:
 *
 *     BtSecurityRecord *current, *next;
 *     IterateListSafe(MEC(secList), current, next, BtSecurityRecord *) {
 *         [...do something with "current". May issue callbacks on it, or
 *             remove it from the list...]
 *     }
 *
 * Parameters:
 *     head - Head of list (address of ListEntry structure)
 *     cur - Variable to use for current list member
 *     next - Variable to use for temporary storage of next list member
 *     type - Structure type of cur and next.
 */
#define IterateListSafe(head, cur, next, type) \
    for ( (cur) = (type) GetHeadList(&(head)) ; \
          (next) = (type) GetNextNode(&(cur)->node), \
            (cur) != (type) &(head); \
          (cur) = (next))

/*---------------------------------------------------------------------------
 * IterateList()
 *
 *     Sets up ordinary traversal of a list. The current member must NOT
 *     be removed during iteration. Must be followed by a block of code
 *     containing the body of the iteration.
 *
 *     For example:
 *     BtSecurityRecord *record;
 *     IterateList(MEC(secList), record, BtSecurityRecord *) {
 *         [...do something with "record", but do not remove it!...]
 *     }
 *
 * Parameters:
 *     head - Head of list (address of ListEntry structure)
 *     cur - Variable to use for current list member
 *     type - Structure type of cur and next.
 */
#define IterateList(head, cur, type) \
    for ( (cur) = (type) GetHeadList(&(head)) ; \
          (cur) != (type) &(head); \
          (cur) = (type) GetNextNode(&(cur)->node) )


#if XA_DEBUG == XA_ENABLED
void VerifyMacros(void);
#endif /* XA_DEBUG == XA_ENABLED */

/*---------------------------------------------------------------------------
 * XaStager struct
 *
 *     Used to manage the "staging" of input data. "Staging" refers to the
 *     process of collecting input data, which may arrive segmented, into
 *     a single contiguous buffer for processing.
 */
typedef struct _XaStager {
    U8 *buff;
    U16 cur;
    U16 target;
    BOOL skip;
} XaStager;

/* Stager routines */

/*---------------------------------------------------------------------------
 * XA_StageInit()
 *
 *     Configures a staging buffer to read a certain number of bytes.
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     buffer - Memory buffer used for staging the incoming data
 *
 *     len - Length of "buffer"
 */
void XA_StageInit(XaStager *stager, U8 *buffer, U16 len);
#define XA_StageInit(s,b,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=FALSE; (s)->buff = (b); } while (0)


/*---------------------------------------------------------------------------
 * XA_StageSkip
 *
 *     Configures a staging buffer to skip a certain number of bytes.
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     len - Number of bytes to skip.
 */
void XA_StageSkip(XaStager *stager, U16 len);
#define XA_StageSkip(s,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=TRUE; (s)->buff = 0; } while (0)

/*---------------------------------------------------------------------------
 * XA_StageGetRead
 *
 *     Returns number of bytes currently staged
 *
 * Parameters:
 *     stager - Staging structure
 *
 *     len - Number of bytes to skip.
 */
U16 XA_StageGetRead(XaStager *stager);
#define XA_StageGetRead(s) ((s)->cur)


/*---------------------------------------------------------------------------
 * XA_StageInput()
 *
 *     Stages input into a staging buffer.
 *
 * Parameters:
 *     stager - Staging structure.
 *
 *     inputBuff - On input, address of pointer to memory to read. On output,
 *         filled with the new starting address of input data.
 *
 *     inputLen - On input, pointer to length of data in inputBuff. On
 *         output, filled with number of unprocessed bytes.
 *
 * Returns:
 *     XA_STATUS_PENDING - More input data is required.
 *
 *     XA_STATUS_SUCCESS - Current stager has all the data it needs.
 *         Subsequent calls for this stager will also return _SUCCESS until
 *         the stager is reconfigured with a new target.
 */
XaStatus XA_StageInput(XaStager *stager, U8 **inputBuff, U16 *inputLen);



/****************************************************************************
 *
 * XA_DECODER elements
 *
 ****************************************************************************/

#if XA_DECODER == XA_ENABLED

/*---------------------------------------------------------------------------
 * XAD_MAX_TYPES constant
 *
 *     Defines the maximum number of types that can be registered
 *     concurrently with XadRegister().
 */
#ifndef XAD_MAX_TYPES
#define XAD_MAX_TYPES 80
#endif

/*---------------------------------------------------------------------------
 * XAD_MAX_STRING_SIZE constant
 *
 *     Defines the maximum size of a string that can be generated at
 *     runtime by Xad(). This limit does not apply to strings that are
 *     "hard-coded" in an XadEntry.
 */
#ifndef XAD_MAX_STRING_SIZE
#define XAD_MAX_STRING_SIZE 30
#endif

/*---------------------------------------------------------------------------
 * XAD_STRING_COUNT constant
 *
 *     Defines the maximum number of statically-allocated strings to
 *     cycle through. Because the memory for strings generated at run-time
 *     is reused during calls to Xad(), increasing the string count
 *     will reduce the chance of a corrupted printout.
 */
#ifndef XAD_MAX_STRINGS
#define XAD_MAX_STRINGS 10
#endif

/*---------------------------------------------------------------------------
 * XadEntry structure
 *
 *    A single instruction that tells how to decode a value into a string.
 *    Typically formed into an array and passed to XAD_RegisterDecoder().
 *
 *    XadEntries can be written out manually, or the XAD_ENTRY* macros can
 *    be used as a shortcut.
 */
typedef struct _XadEntry {
    U32 value;      /* Value to match */
    void *str;      /* String or special code */
} XadEntry;


/*---------------------------------------------------------------------------
 * XadContext structure
 *
 *     Internal context memory used by the XA_DECODER system. This memory
 *     must be allocated, prefilled to 0 at init time, and supplied to
 *     the XA_DECODER code in debug.c via the XADC() macro. If the XADC
 *     macro is not defined, debug.c will allocate its own RAM.
 */
typedef struct _XadContext  {
    /* === Internal use only === */
    const XadEntry *entries[XAD_MAX_TYPES];
    U16             entryLens[XAD_MAX_TYPES];
    const char     *typeNames[XAD_MAX_TYPES];

    char decodeStrings[XAD_MAX_STRINGS][XAD_MAX_STRING_SIZE];
    U8 curDecodeString;
} XadContext;


/* Internal use - We assume that no legitimate pointers live at these memory
 * locations so that we can overload the void * as an opcode.
 */
#define XAD_OPCODE_HEXVALUE ((void *)0x01)
#define XAD_OPCODE_DROPBITS ((void *)0x02)

/*---------------------------------------------------------------------------
 * XAD_ENTRY()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     a #define'd value into a string. For example:
 *
 *     XAD_ENTRY(BT_STATUS_SUCCESS),
 *
 *     will generate an entry that converts the #define'd value
 *     BT_STATUS_SUCCESS into the string "BT_STATUS_SUCCESS".
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     value - The #define for the value in question.
 */
#define XAD_ENTRY(value)                { value, #value }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_PREFIX()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     a #define'd value into a string, WITHOUT its prefix. For example:
 *
 *     XAD_ENTRY_PREFIX(BT_STATUS_, SUCCESS),
 *
 *     will generate an entry that converts the #define'd value for
 *     BT_STATUS_SUCCESS into the string "SUCCESS".
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     prefix - Prefix of the define value, which will be ommitted
 *         from the output.
 *
 *     value - Remainder of the define value, which will be included in
 *         the output.
 */
#define XAD_ENTRY_PREFIX(prefix, value) { prefix##value, #value }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_HEX()
 *
 *     A macro that results in a XadEntry structure which will convert
 *     any unmatched value into a hexadecimal display of the defined width.
 *     For example, XAD_ENTRY_HEX(2) would cause the unmatched value 0x44
 *     to decode into "0x0044".
 *
 *     Note that XAD_ENTRY_HEX halts further processing of the XadEntries
 *     for the type. Therefore it should only be found at the end of the
 *     XadEntry array.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     width - Width of the hex output, in bits. May be 8, 16, or 32.
 */
#define XAD_ENTRY_HEX(width)            { width, XAD_OPCODE_HEXVALUE }

/*---------------------------------------------------------------------------
 * XAD_ENTRY_DROPBITS()
 *
 *     A macro that results in a XadEntry structure which forces the
 *     decoder to ignore a certain set of bits. During iteration through
 *     the entries, reaching this line will cause the bits specified
 *     to be zeroed out. Processing then continues normally.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     ignore - Bits to ignore.
 */
#define XAD_ENTRY_DROPBITS(bits)        { (U32)bits, XAD_OPCODE_DROPBITS }

/*---------------------------------------------------------------------------
 * XAD_RegisterDecoder()
 *
 *     Registers a decoder with the XA_DECODE mechanism so that XA_Decode()
 *     or Xad() can be used.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     entries - Array of XadEntry structures. When decoding, XA_Decode
 *         will iterate through this array to determine how to decode the
 *         value.
 *
 *     entriesLen - Number of XadEntries in the entries array.
 *
 *     typeName - The name of the type of interest, as a string.
 *
 */
void XAD_RegisterDecoder(const XadEntry *entries,
                         U16 entriesLen, const char *type);

/*---------------------------------------------------------------------------
 * XadRegister()
 *
 *     A shortcut for registering a type. Assumes the existance of an array
 *     called entries_TypeName containing the XadEntry array. For example,
 *     to register a decoder for an integer type called "MyType" with two
 *     possible values, define an array:
 *
 *     static const XadEntry entries_MyType[] = {
 *     XAD_ENTRY(MY_TYPE_YES), XAD_ENTRY(MY_TYPE_NO) };
 *
 *     Then in your routine's _Init() code, call "XadRegister(MyType);"
 *     Later, calling "Xad(var, MyType);" will return the string
 *     "MY_TYPE_YES" or "MY_TYPE_NO" depending on the value of var.
 *
 * Requires:
 *     XA_DECODER enabled.
 *
 * Parameters:
 *     type - The actual type name. This is not provided as a string but
 *         entered normally, for example: XadRegister(MyTypeName);
 *
 */
#define XadRegister(type) \
        XAD_RegisterDecoder(entries_##type, \
        sizeof(entries_##type)/sizeof(*entries_##type), #type)


/*---------------------------------------------------------------------------
 * XAD_Decode()
 *
 *     Converts a value into a string according to the rules for its type.
 *
 * Requires:
 *     XA_DECODE enabled.
 *
 * Parameters:
 *     value - Value to be decoded into a string. If the value is not
 *         recognized, the string returned will look similar to
 *         "Unknown: 0x0123"
 *
 *     typeName - String which specifies the type. If the type name is not
 *         recognized, the string returned will look similar to
 *         "Unknown type: 0x0123"
 *
 * Returns:
 *     A string which describes the value. The string should be used soon after
 *     this routine returns to avoid corrupted output. For example, passing
 *     this return value into a call to printf() will make a safe copy of the
 *     buffer. If output corruption occurs, consider increasing XAD_MAX_STRING.
 */
const char *XAD_Decode(U32 value, const char *typeName);

/*---------------------------------------------------------------------------
 * Xad()
 *
 *     A shortcut for invoking XAD_Decode.
 *
 * Requires:
 *     XA_DECODE enabled.
 *
 * Parameters:
 *     value - Value to be decoded into a string.
 *
 *     name - Type name. This is not provided as a string but simply
 *         typed out: Xad(17, TypeName);
 *
 * Returns:
 *     See XAD_Decode().
 */
const char *Xad(U32 value, void *name);
#define Xad(a,b) XAD_Decode(a, #b)

#else  /*  XA_DECODER == XA_ENABLED */

/* Some defines to quietly turn off decodes */
#define Xad(a,b)                   "?"
#define XadRegister(t)             (void)0
#define XAD_Decode(a,b)            Xad(a,b)
#define XAD_RegisterDecoder(a,b,c) XadRegister(a)

#endif /*  XA_DECODER == XA_ENABLED */


/* Aliases for old-style decode functions - new code should use Xad() */
#define pHciCommand(a)         Xad(a, HciCommandType)
#define pHciEvent(a)           Xad(a, HciEventType)
#define pHC_Status(a)          Xad(a, BtErrorCode)
#define pBT_Status(a)          Xad(a, BtStatus)
#define pME_Event(a)           Xad(a, BtEventType)
#define pLLC_Event(a)          Xad(a, L2capSignalOpcode)
#define pL2CAP_SignalOpcode(a) Xad(a, L2capSignalOpcode)
#define pL2CAP_ConnStatus(a)   Xad(a, L2capConnStatus)
#define pL2CAP_DiscReason(a)   Xad(a, L2capDiscReason)
#define pL2CAP_Event(a)        Xad(a, L2capEventType)
#define pSdpService(a)         Xad(a, SdpServiceClassUuid)
#define pCodMajorClass(a)      Xad(a, BtClassOfDeviceMajor)
#define pRFC_Command(a)        Xad(a, RfCommand)
#define pRFC_MuxCommand(a)     Xad(a, RfMuxCommand)
#define pRFC_BaudRate(a)       Xad(a, RfBaudrate)


#endif /* __UTILS_H */
