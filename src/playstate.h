/**
 * @file src/playstate.h
 * 
 * Game's playstate loop
 */
#ifndef __PLAYSTATE_H_
#define __PLAYSTATE_H_

#include "types.h"

/**
 * Playstate implementation. Must initialize it, run the loop and clean it up
 */
state playstate(int doLoad);

#endif

