/**
 * @file src/menustate.c
 * 
 * Menu state
 */
#ifndef __CREDITS_H_
#define __CREDITS_H_

#include "types.h"

/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 */
state credits();

/** Retrieve a new 'stateHandler' for the credits 'state'. */
void *credits_getHnd();

#endif

