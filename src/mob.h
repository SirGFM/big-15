/**
 * @file src/mob.c
 * 
 * Mob structure and its functions
 */
#ifndef __MOB_H_
#define __MOB_H_

#include <GFraMe/GFraMe_error.h>

typedef enum {
    MOB_JUMPER,
    MOB_EYE,
    MOB_MAX
} mobType;

typedef enum {
    MOB_STAND,
    MOB_WALK,
    MOB_ATTACK
    MOB_ANIM_MAX
} mobAnim;

typedef struct stMob mob;

/**
 * Instantiate a new mob of a given type
 * 
 * @param ppMob The instantiated mob
 * @param x The mob's horizontal position (in pixels)
 * @param y The mob's vertical position (in pixels)
 * @param mobType The type of the mob
 */
GFraMe_ret mob_init(mob **ppMob, int x, int y, mobType type);

/**
 * Clean up memory for this mob
 * 
 * @param ppMob The mob
 */
void mob_clean(mob **ppMob);

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

#endif

