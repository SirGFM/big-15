/**
 * @file src/bullet.c
 * 
 * A projectile like thing
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include <stdlib.h>

#include "bullet.h"
#include "camera.h"
#include "global.h"
#include "types.h"

#define BUL_ANIM_MAX 3
/** Projectile's states */
typedef enum { PROJ_INIT=0, PROJ_DEF, PROJ_EXPLODE, PROJ_NONE } projState;

struct stBullet {
    GFraMe_sprite spr;
    projState state;
    int animLen;
    GFraMe_animation anim[BUL_ANIM_MAX];
};

/**
 * Store the animation in the following manner:
 *   _bul_*AnimData[i][0] = FPS
 *   _bul_*AnimData[i][1] = data len
 *   _bul_*AnimData[i][2] = do loop
 *   _bul_*AnimData[i]+3  = actual data
 */
static int _bul_EnAnimData[] = {
/* fps,len,loop,data...                    */
   15 , 1 , 0  , 805,                    /* PROJ_INIT */
    8 , 2 , 1  , 806, 807,               /* PROJ_DEF  */
   10 , 5 , 0  , 808, 809, 810, 811, 812 /* PROJ_NONE */
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
    ASSERT(ppBul, GFraMe_ret_bad_param);
    ASSERT(!*ppBul, GFraMe_ret_bad_param);
    
    // Allocate the memory
    tmp = (bullet*)malloc(sizeof(bullet));
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
 * @return GFraMe error code
 */
GFraMe_ret bullet_init(bullet *pBul, flag type, int cx, int cy, int dstCX, int dstCY) {
    //GFraMe_object *pObj;
    GFraMe_ret rv;
    GFraMe_sprite *pSpr;
    int *pData, len;
    
    // Sanitize input
    ASSERT_NR(type & ID_PROJ);
    
    // Get the bullet's sprite and object
    pSpr = &pBul->spr;
    //pObj = spr->obj;
    
    // Initialize the bullet, according to its type
    pData = 0;
    len = 0;
    switch (type) {
        case ID_ENEPROJ: {
            GFraMe_sprite_init(pSpr, cx - 4, cy - 4, 6/*w*/, 6/*h*/, gl_sset8x8,
                -2/*ox*/, -2/*oy*/);
            pData = _bul_EnAnimData;
            len = sizeof(_bul_EnAnimData) / sizeof(int);
        } break;
        default: {}
    }
    pSpr->id = type;
    
    // Set all animations
    if (pData) {
        int i, j;
        
        j = 0;
        i = 0;
        while (i < len) {
            int *data, fps, len, loop;
            
            GFraMe_assertRV(j < BUL_ANIM_MAX, "Bullets animations overflowed",
                rv = GFraMe_ret_failed, __ret);
            
            fps  = pData[i];
            len  = pData[i+1];
            loop = pData[i+2];
            data = pData+i+3;
            
            GFraMe_animation_init(&pBul->anim[j], fps, data, len, loop);
            
            i += len + 3;
            j++;
        }
        pBul->animLen = j;
    }
    
    // Set its initial state
    pBul->state = -1;
    bullet_setAnim(pBul, PROJ_INIT);
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Explodes a bullet and deactivate it
 * 
 * @param pBul The bullet
 */
void bullet_explode(bullet *pBul) {
    switch (pBul->spr.id) {
        case ID_ENEPROJ: bullet_setAnim(pBul, PROJ_EXPLODE); break;
        default: pBul->state = PROJ_NONE;
    }
}

/**
 * Get a bullet's object
 * 
 * @param ppObj The object
 * @param pBul The bullet
 */
void bullet_getObject(GFraMe_object **ppObj, bullet *pBul) {
    *ppObj = &pBul->spr.obj;
}

/**
 * Get a bullet's type (i.e., ID)
 * 
 * @param pID The bullet's ID
 * @param pBul The bullet
 */
void bullet_getID(flag *pID, bullet *pBul) {
    *pID = pBul->spr.id;
}

/**
 * Check whether a bullet is alive
 * 
 * @param pBul The bullet
 * @return 1 on success, 0 otherwise
 */
int bullet_isAlive(bullet *pBul) {
    return pBul->state != PROJ_EXPLODE;
}

/**
 * Switch the bullet's animation
 * 
 * @param pBul The bullet
 * @param anim Animation to be played
 */
void bullet_setAnim(bullet *pBul, int anim) {
    ASSERT_NR(pBul->state != anim);
    ASSERT_NR(anim >= 0);
    ASSERT_NR(anim < PROJ_NONE);
    ASSERT_NR(anim < pBul->animLen);
    
    GFraMe_sprite_set_animation(&pBul->spr, &pBul->anim[anim], 0);
    
    pBul->state = anim;
__ret:
    return;
}

/**
 * Draw a bullet
 * 
 * @param pBul The bullet
 */
void bullet_draw(bullet *pBul) {
    ASSERT_NR(pBul->state != PROJ_NONE);
    
    GFraMe_sprite_draw_camera(&pBul->spr, cam_x, cam_y, SCR_W, SCR_H);
    
__ret:
    return;
}

/**
 * Updates a bullet
 * 
 * @param pBul The bullet
 * @param ms Time elapsed from the previous frame, in milliseconds
 */
void bullet_update(bullet *pBul, int ms) {
    ASSERT_NR(pBul->state != PROJ_NONE);
    
    // Updates the animation and the bullet's position
    GFraMe_sprite_update(&pBul->spr, ms);
    
    // Check if the bullet animation should be changed
    if (pBul->state == PROJ_INIT && !pBul->spr.anim)
        bullet_setAnim(pBul, PROJ_DEF);
    else if (pBul->state == PROJ_EXPLODE && !pBul->spr.anim)
        pBul->state = PROJ_NONE;
__ret:
    return;
}

