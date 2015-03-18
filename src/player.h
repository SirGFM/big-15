/**
 * @file src/player.h
 * 
 * Player abstraction. Must be generic (i.e., receive a pointer for the struct
 * as the first param), as to have two player in the playstate.
 */
#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "types.h"

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

typedef struct stPlayer player;

/**
 * Initialize the player.
 * 
 * @param ppPl The player
 * @param ID Player's ID
 * @param firstTile Sprite's standing frame (first from its set)
 * @param x Horizontal position
 * @param y Vertical position
 * @return GFraMe error code
 */
GFraMe_ret player_init(player **ppPl, int ID, int firstTile, int x, int y);

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
 * Get the player's sprite, for collision
 * 
 * @param ppSpr Player's sprite
 * @param pPl The player
 */
void player_getSprite(GFraMe_sprite **ppSpr, player *pPl);

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

/**
 * Position the player (and set it to be moved) as being carried by the boss
 * 
 * @param pPl The player
 * @param pObj The object of the boss
 */
void player_getCarriedBoss(player *pPl, GFraMe_object *pObj);

/**
 * Get a player's center position
 * 
 * @param pX The central horizontal position
 * @param pY The central vertical position
 * @param pPl The player
 */
void player_getCenter(int *pX, int *pY, player *pPl);

/**
 * Set a destination to this player
 * 
 * @param pPl The player
 * @param map The map index
 * @param x The horizontal position inside the new map
 * @param y The vertical position inside the new map
 */
void player_setDestMap(player *pPl, int map, int x, int y);

/**
 * Check if two players are trying to switch map, and to the same one
 * Also, set the global variables with map info
 * 
 * @param pPl1 One player
 * @param pPl2 The other player
 * @return GFraMe error code
 */
GFraMe_ret player_cmpDestMap(player *pPl1, player *pPl2);

/**
 * Moves the player to a new position
 * 
 * @param pPl The player
 * @param x The horizontal position
 * @param y The vertical position
 * @param ms Time, in milliseconds, elapsed since the previous frame
 * @param time How long the tween should last (in milliseconds)
 * @return GFraMe_ret_ok, if the position was reached
 */
GFraMe_ret player_tweenTo(player *pPl, int x, int y, int ms, int time);

/**
 * Check if the player is being carried
 * 
 * @param pPl The player
 */
int player_isBeingCarried(player *pPl);

/**
 * Get the player's ID
 * 
 * @param pPl The player
 */
flag player_getID(player *pPl);

/**
 * Checks if the player should teleport and setup everything
 * 
 * @param pPl The player
 */
void player_checkTeleport(player *pPl);

/**
 * Checks and change a player's item
 * 
 * @param pPl The player
 */
void player_changeItem(player *pPl);

/**
 * Deal some damage to a player and play an animation
 * 
 * @param pPl The player
 * @param dmg How much damage should be dealt
 * @param dir Direction the player was hit from
 */
void player_hurt(player *pPl, int dmg, GFraMe_direction dir);

/**
 * Return when the player is still in hitstun
 * 
 * @param pPl The player
 * @return Whether the player is in hitstun or not
 */
int player_isHurt(player *pPl);

/**
 * Return whether a player is alive
 * 
 * @param pPl The player
 * @return 1 on sucess, 0 otherwise
 */
int player_isAlive(player *pPl);

/**
 * Check if a player is within vertical bounds of the map
 * 
 * @param pPl The player
 * @return 1 on sucess, 0 otherwise
 */
int player_isInsideMap(player *pPl);

#endif

