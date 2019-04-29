/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2006              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW diagnostic messaging macros.
 * $Revision: 1.4 $
 */

#ifndef __MUSB_DIAG_H__
#define __MUSB_DIAG_H__

#include "mu_stdio.h"
#include "mu_strng.h"

/*
 * Diagnostics
 */
#if MUSB_DIAG > 0

#ifndef MGC_DIAG_MSG_SIZE
#define MGC_DIAG_MSG_SIZE   256
#endif
extern char MGC_DiagMsg[MGC_DIAG_MSG_SIZE];

#ifndef MGC_DIAG_NUM_SIZE
#define MGC_DIAG_NUM_SIZE   33
#endif
extern char MGC_DiagNumber[MGC_DIAG_NUM_SIZE];

extern uint8_t MGC_bDiagLevel;

/**
 * Print diagnostic string.
 * Use this to eliminate diagnostic code when not compiled with diagnostics.
 * @param _bLevel diagnostic level
 * @param _pMsg message to print
 */
#define MUSB_DIAG_STRING(_bLevel, _pMsg) if(MGC_bDiagLevel >= _bLevel) \
    MUSB_PrintLine(_pMsg)

/**
 * Print a diagnostic with a string and number.
 * Use this to eliminate diagnostic code when not compiled with diagnostics.
 * @param _bLevel diagnostic level
 * @param _pText the text portion of the message; printed first
 * @param _dwNum the numeric portion of the message; appended to the text portion
 * @param _bBase numeric base (2, 8, 10 or 16)
 * @param _bJust the justification (0 for none); this is for leading-zero padding
 */
#define MUSB_DIAG1(_bLevel, _pText, _dwNum, _bBase, _bJust) { \
    MGC_DiagMsg[0] = 0; \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, _pText); \
    MUSB_Stringize(MGC_DiagNumber, MGC_DIAG_NUM_SIZE, _dwNum, _bBase, _bJust); \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, MGC_DiagNumber); \
    MUSB_DIAG_STRING(_bLevel, MGC_DiagMsg); \
};

/**
 * Print a diagnostic with a two strings and two numbers.
 * Use this to eliminate diagnostic code when not compiled with diagnostics.
 * Print a diagnostic with a string followed by a number, followed by another set.
 * @param _bLevel diagnostic level
 * @param _pText1 the first text portion of the message; printed first
 * @param _dwNum1 the first numeric portion of the message; appended to the above
 * @param _pText2 the second text portion of the message; appended to the above
 * @param _dwNum2 the second numeric portion of the message; appended to the above
 * @param _bBase numeric base for both numbers (2, 8, 10 or 16)
 * @param _bJust the justification (0 for none); this is for leading-zero padding
 */
#define MUSB_DIAG2(_bLevel, _pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust) { \
    MGC_DiagMsg[0] = 0; \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, _pText1); \
    MUSB_Stringize(MGC_DiagNumber, MGC_DIAG_NUM_SIZE, _dwNum1, _bBase, _bJust); \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, MGC_DiagNumber); \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, _pText2); \
    MUSB_Stringize(MGC_DiagNumber, MGC_DIAG_NUM_SIZE, _dwNum2, _bBase, _bJust); \
    MUSB_StringConcat(MGC_DiagMsg, MGC_DIAG_MSG_SIZE, MGC_DiagNumber); \
    MUSB_DIAG_STRING(_bLevel, MGC_DiagMsg); \
};

#else

/* Print diagnostic */
#define MUSB_DIAG_STRING(_bLevel, _pMsg)

#define MUSB_DIAG1(_bLevel, _pText1, _dwNum1, _bBase, _bJust)
#define MUSB_DIAG2(_bLevel, _pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust)

#endif

#endif	/* multiple inclusion protection */
