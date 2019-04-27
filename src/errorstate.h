/**
 * @file src/errorstate.h
 * 
 * Menu state
 */
#ifndef __ERRORSTATE_H_
#define __ERRORSTATE_H_

#include "types.h"

/**
 * Errorstate implementation. Must initialize it, run the loop and clean it up
 * 
 * @return To which state it's switching
 */
state errorstate();

/**
 * Retrieve a new 'stateHandler' for the errorstate 'state'.
 *
 * @param [in]jerr 'jjatError' that caused this state change.
 * @param [in]gfmErr Possible 'GFraMe_ret' that described the error.
 */
void *errorstate_getHnd(int jerr, int gfmErr);

#endif

