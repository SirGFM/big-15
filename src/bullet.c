/**
 * @file src/bullet.c
 * 
 * A projectile like thing
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_sprite.h>

#include <stdlib.h>
#include <string.h>

#include "bullet.h"
#include "global.h"

/** Projectile's states */
typedef enum { PROJ_INIT=0, PROJ_DEF, PROJ_EXPLODE } projState;

struct stBullet {
    GFraMe_sprite spr;
    projState state;
};

/**
 * Alloc a new bullet
 * 
 * @param ppBul The bullet
 * @return GFraMe error code
 */
GFraMe_ret bullet_getNew(bullet **ppBul) {
    bullet *tmp;
    GFraMe_ret rv;
    
    // Sanitize input
    ASSERT(ppBul, GFraMe_ret_param_bad);
    ASSERT(!*ppBul, GFraMe_ret_param_bad);
    
    // Allocate the memory
    tmp = (*bullet)malloc(sizeof(bullet));
    ASSERT(tmp, GFraMe_ret_memory_error);
    
    // Set return variables
    *ppBul = tmp;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Dealloc a bullet
 * 
 * @param ppBul The bullet
 */
void bullet_clean(bullet **ppBul) {
    // Sanitize input
    ASSERT_NR(ppBul);
    ASSERT_NR(*ppBul);
    
    // Dealloc the object
    free(*ppBul);
    *ppBul = 0;
__ret:
    return;
}

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
void bullet_init(bullet *pBul, flag type, int cx, int cy, int dstCX, int dstCY) {
    GFraMe_object *pObj;
    GFraMe_sprite *pSpr;
    
    // Sanitize input
    ASSERT_NR(type & ID_PROJ);
    
    // Get the bullet's sprite and object
    pSpr = &pBul->spr;
    pObj = spr->obj;
    
    // Initialize the bullet, according to its type
    switch (type) {
        case ID_ENEPROJ: {
            GFraMe_sprite_init(pSpr, cx - 4, cy - 4, 6/*w*/, 6/*h*/, gl_sset8x8,
                -2/*ox*/, -2/*oy*/);
        } break;
        default: {}
    }
    
    pBul->state = PROJ_INIT;
__ret:
    return;
}

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


