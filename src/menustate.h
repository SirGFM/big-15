/**
 * @file src/menustate.h
 * 
 * Menu state
 */
#ifndef __MENUSTATE_H_
#define __MENUSTATE_H_

#include "types.h"

/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 * 
 * @return To which state it's switching
 */
state menustate();

/** Retrieve a new 'stateHandler' for the menustate 'state'. */
void *menustate_getHnd();

#endif

