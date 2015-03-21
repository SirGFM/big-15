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

/**
 * Set a text to be shown
 * 
 * @param text Text (the pointer will be copied)
 * @param textLen Text's length
 * @param x Window's horizontal position (in tiles)
 * @param y Window's vertical position (in tiles)
 * @param w Window's width (in tiles)
 * @param h Window's height (in tiles)
 */
void ps_showText(char *text, int textLen, int x, int y, int w, int h);

#endif

