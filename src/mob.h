/**
 * @file src/mob.c
 * 
 * Mob structure and its functions
 */
#ifndef __MOB_H_
#define __MOB_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "types.h"

typedef struct stMob mob;

#define BOSS_SPEED 90

/**
 * Instantiate a new mob
 * 
 * @param ppMob The instantiated mob
 */
GFraMe_ret mob_getNew(mob **ppMob);

/**
 * Clean up memory for this mob
 * 
 * @param ppMob The mob
 */
void mob_clean(mob **ppMob);

/**
 * Initialize a mob of a given type
 * 
 * @param pMob The mob
 * @param x The mob's horizontal position (in pixels)
 * @param y The mob's vertical position (in pixels)
 * @param type The type of the mob
 */
GFraMe_ret mob_init(mob *pMob, int x, int y, flag type);

/**
 * Updates the mob
 * 
 * @param pMob The mob
 * @param ms Time elapsed, in milliseconds, from the previous frame
 */
void mob_update(mob *pMob, int ms);

/**
 * Draw the mob
 * 
 * @param pMob The mob
 */
void mob_draw(mob *pMob);

/**
 * Change the currently playing animation
 * 
 * @param pMob The mob
 * @param anim The new animation
 * @param dontReset Whether the animation shouldn't be reset
 */
void mob_setAnim(mob *pMob, int n, int dontReset);

/**
 * Get both horizontal and vertical distance from the closest player
 * 
 * @param pDx The horizontal distance
 * @param pDy The vertical distance
 * @param pMob The mob
 */
void mob_getClosestPlDist(int *pDx, int *pDy, mob *pMob);

/**
 * Get the horizontal distance from the closest player
 * 
 * @param pMob The mob
 * @return The horizontal distance
 */
int mob_getClosetsPlHorDist(mob *pMob);

/**
 * Get the vertical distance from the closest player
 * 
 * @param pMob The mob
 * @return The vertical distance
 */
int mob_getClosestPlVerDist(mob *pMob);

/**
 * Get both horizontal and vertical distance from the closest player
 * 
 * @param pDx The horizontal distance
 * @param pDy The vertical distance
 * @param pMob The mob
 * @param plID The player's ID
 */
void mob_getPlDist(int *pDx, int *pDy, mob *pMob, flag plID);

/**
 * Get the horizontal distance from the closest player
 * 
 * @param pMob The mob
 * @param plID The player's ID
 * @return The horizontal distance
 */
int mob_getPlHorDist(mob *pMob, flag plID);

/**
 * Get the vertical distance from the closest player
 * 
 * @param pMob The mob
 * @param plID The player's ID
 * @return The vertical distance
 */
int mob_getPlVerDist(mob *pMob, flag plID);

/**
 * Try to hit a mob for some damage
 * 
 * @param pMob The mob
 * @param dmg Amount of damage that should be done
 * @param type Type of "hurting method"
 * @return Whether the mob was damaged (GFraMe_ret_ok) or not
 */
GFraMe_ret mob_hit(mob *pMob, int dmg, flag type);

/**
 * Get how much damage the mob deal on touch
 * 
 * @param pMob The mob
 * @return The damage
 */
int mob_getDamage(mob *pMob);

/**
 * Check whether the mob is alive or not
 * 
 * @param pMob The mob
 * @return 1 it's alive, 0 otherwise
 */
int mob_isAlive(mob *pMob);

/**
 * Get the mob's object, for collision
 * 
 * @param ppObj Mob's object
 * @param pMob The mob
 */
void mob_getObject(GFraMe_object **ppObj, mob *pMob);

/**
 * Check if a mob animation finished playing
 * 
 * @param pMob The mob
 * @return 1 if true, 0 if false
 */
int mob_didAnimFinish(mob *pMob);

/**
 * Read the mob's ID
 * 
 * @param pMob The mob
 * @return The mob ID
 */
int mob_getID(mob *pMob);

#endif

