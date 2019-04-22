/**
 * @file src/demo.c
 * 
 * Play a little demo (only text on a black screen, actually)
 */
#ifndef __DEMO_H_
#define __DEMO_H_

#include "types.h"

/**
 * Play the demo
 * 
 * @return The next state
 */
state demo();

/** Retrieve a new 'stateHandler' for the demo 'state'. */
void *demo_getHnd();

#endif

