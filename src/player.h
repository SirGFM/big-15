/**
 * @file src/player.h
 * 
 * Player abstraction. Must be generic (i.e., receive a pointer for the struct
 * as the first param), as to have two player in the playstate.
 */
#ifndef __PLAYER_H_
#define __PLAYER_H_

#include <GFraMe/GFraMe_error.h>

typedef struct stPlayer player;

/**
 * Initialize the player.
 * 
 * @param pl The player
 * @param firstTile Sprite's standing frame (first from its set)
 * @return GFraMe error code
 */
GFraMe_ret player_init(player *pl, int firstTile);

/**
 * Clean up the player
 * 
 * @param pl The player
 */
void player_clean(player *pl);

/**
 * Update the player. Also handle input (i.e., jump)
 * 
 * @param pl The player
 * @param ms Time elapsed from last frame, in milliseconds
 */
void player_update(player *pl, int ms);

/**
 * Draw the player to the screen.
 * 
 * @param pl The player
 */
void player_draw(player *pl);

#endif

