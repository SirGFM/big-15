/**
 * @file src/playstate.h
 * 
 * Game's playstate loop
 */
#ifndef __PLAYSTATE_H_
#define __PLAYSTATE_H_

#include "types.h"

enum enPlaystateCmd {
    NEWGAME = 0
  , CONTINUE
  , MT_VERSION
};
typedef enum enPlaystateCmd playstateCmd;

/**
 * Playstate implementation. Must initialize it, run the loop and clean it up
 */
state playstate(playstateCmd cmd);

/**
 * Retrieve a new 'stateHandler' for the playstate 'state'.
 *
 * @param [in]cmd In which mode the playstate should start.
 */
void *playstate_getHnd(playstateCmd cmd);

/** Check whether the given state is a playstate. */
int isPlaystate(void *hnd);

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

