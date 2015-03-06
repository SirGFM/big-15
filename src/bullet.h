/**
 * @file src/bullet.c
 * 
 * A projectile like thing
 */
#ifndef __BULLET_H_
#define __BULLET_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "type.h"

/** Export the bullet's type */
typedef struct stBullet bullet;

/**
 * Alloc a new bullet
 * 
 * @param ppBul The bullet
 * @return GFraMe error code
 */
GFraMe_ret bullet_getNew(bullet **ppBul);

/**
 * Dealloc a bullet
 * 
 * @param ppBul The bullet
 */
void bullet_clean(bullet **ppBul);

/**
 * Initialize a bullet
 * 
 * @param pBul The bullet
 * @param type The bullet's type (handles animations, speed and damage)
 * @param cx Center's horizontal position (in world space/pixels)
 * @param cy Center's vertical position (in world space/pixels)
 * @param dstCX Destination's horizontal position (in world space/pixels)
 * @param dstCY Destination's vertical position (in world space/pixels)
 */
void bullet_init(bullet *pBul, flag type, int cx, int cy, int dstCX, int dstCY);

/**
 * Explodes a bullet and deactivate it
 * 
 * @param pBul The bullet
 */
void bullet_explode(bullet *pBul);

/**
 * Get a bullet's object
 * 
 * @param ppObj The object
 * @param pBul The bullet
 */
void bullet_getObject(GFraMe_object **ppObj, bullet *pBul);

/**
 * Get a bullet's type (i.e., ID)
 * 
 * @param pID The bullet's ID
 * @param pBul The bullet
 */
void bullet_getID(flag *pID, bullet *pBul);

/**
 * Check whether a bullet is alive
 * 
 * @param pBul The bullet
 * @return 1 on success, 0 otherwise
 */
int bullet_isAlive(bullet *pBul);

/**
 * Draw a bullet
 * 
 * @param pBul The bullet
 */
void bullet_draw(buleet *pBul);

/**
 * Updates a bullet
 * 
 * @param pBul The bullet
 * @param ms Time elapsed from the previous frame, in milliseconds
 */
void bullet_update(bullet *pBul, int ms);

#endif

