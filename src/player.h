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
 * @param ppPl The player
 * @param ID Player's ID
 * @param firstTile Sprite's standing frame (first from its set)
 * @return GFraMe error code
 */
GFraMe_ret player_init(player **ppPl, int ID, int firstTile);

/**
 * Clean up the player
 * 
 * @param ppPl The player
 */
void player_clean(player **ppPl);

/**
 * Update the player. Also handle input (i.e., jump)
 * 
 * @param pPl The player
 * @param ms Time elapsed from last frame, in milliseconds
 */
void player_update(player *pPl, int ms);

/**
 * Draw the player to the screen.
 * 
 * @param pPl The player
 */
void player_draw(player *pPl);

/**
 * Get the player's object, for collision
 * 
 * @param ppObj Player's object
 * @param pPl The player
 */
void player_getObject(GFraMe_object **ppObj, player *pPl);

/**
 * Set, if necessary, the animation
 * 
 * @param pPl The player
 * @param anim The animation
 */
void player_setAnimation(player *pPl, int anim);

/**
 * Position the player (and set it to be moved) as being carried by another
 * player
 * 
 * @param pPl The player
 * @param pObj The object of the carring player
 */
void player_getCarried(player *pPl, GFraMe_object *pObj);

#endif

