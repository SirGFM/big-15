/**
 * @file src/mob.c
 * 
 * Mob structure and its functions
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdio.h>

#include "camera.h"
#include "global.h"
#include "mob.h"
#include "types.h"

#define MOB_ANIM_MAX 4

struct stMob {
    GFraMe_sprite spr;       /** Mob's sprite (for rendering and collision)   */
    int health;              /** How many hitpoints this mob has              */
    int damage;              /** How much damage this mob does on the player  */
    flag weakness;           /** IDs that can do damage to this mob           */
    int anim;                /** The mob's current animation                  */
    int animLen;             /** How many animations this mob has             */
    /** Every possible animation, so it won't overlap another mob's */
    GFraMe_animation mob_anim[MOB_ANIM_MAX];
};

/**
 * Store the animation in the following manner:
 *   _mob_*AnimData[i][0] = FPS
 *   _mob_*AnimData[i][1] = data len
 *   _mob_*AnimData[i][2] = do loop
 *   _mob_*AnimData[i]+3  = actual data
 */
static int _mob_jumperAnimData[] = {
    1, 2, 1, 704, 705, /* stand */
    2, 1, 0, 706,      /* pre-jump */
    0, 1, 0, 707,      /* jump */
    1, 1, 0, 705       /* after-jump */
};

/**
 * Instantiate a new mob
 * 
 * @param ppMob The instantiated mob
 */
GFraMe_ret mob_getNew(mob **ppMob) {
    GFraMe_ret rv;
    mob *tmp;
    
    // Sanitize parameters
    ASSERT(ppMob, GFraMe_ret_bad_param);
    ASSERT(!*ppMob, GFraMe_ret_bad_param);
    
    // Alloc the new mob
    tmp = (mob*)malloc(sizeof(mob));
    ASSERT(tmp, GFraMe_ret_memory_error);
    
    // Set the return variables
    *ppMob = tmp;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up memory for this mob
 * 
 * @param ppMob The mob
 */
void mob_clean(mob **ppMob) {
    // Sanitize parameters
    ASSERT_NR(ppMob);
    ASSERT_NR(*ppMob);
    
    free(*ppMob);
    *ppMob = 0;
__ret:
    return;
}

/**
 * Initialize a mob of a given type
 * 
 * @param pMob The mob
 * @param x The mob's horizontal position (in pixels)
 * @param y The mob's vertical position (in pixels)
 * @param type The type of the mob
 */
GFraMe_ret mob_init(mob *pMob, int x, int y, flag type) {
    GFraMe_ret rv;
    int *animData, dataLen;
    
    // Sanitize parameters
    ASSERT(pMob, GFraMe_ret_bad_param);
    //ASSERT(type == ID_JUMPER || type == ID_EYE, GFraMe_ret_bad_param);
    
    // Initialize the mob
    animData = 0;
    switch (type) {
        case ID_JUMPER: {
            GFraMe_sprite_init(&pMob->spr, x, y, 6/*w*/, 5/*h*/, gl_sset8x8,
                -1/*ox*/, -2/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            animData = _mob_jumperAnimData;
            dataLen = sizeof(_mob_jumperAnimData) / sizeof(int);
        } break;
        case ID_EYE: {
            GFraMe_sprite_init(&pMob->spr, x, y, 8/*w*/, 8/*h*/, gl_sset8x8,
                0/*ox*/, 0/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
        } break;
        default: {
            // Shouldn't happen!
            GFraMe_assertRV(0, "Invalid mob type!", rv = GFraMe_ret_failed,
                __ret);
        }
    }
    
    // Set all animations
    if (animData) {
        int i, j;
        
        j = 0;
        i = 0;
        while (i < dataLen) {
            int *data, fps, len, loop;
            
            GFraMe_assertRV(j < MOB_ANIM_MAX, "Mob animations overflowed",
                rv = GFraMe_ret_failed, __ret);
            
            fps  = animData[i];
            len  = animData[i+1];
            loop = animData[i+2];
            data = animData+i+3;
            
            GFraMe_animation_init(&pMob->mob_anim[j], fps, data, len, loop);
            
            i += len + 3;
            j++;
        }
    }
    
    // Set the mob animation
    pMob->anim = -1;
    mob_setAnim(pMob, 0);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Updates the mob
 * 
 * @param pMob The mob
 * @param ms Time elapsed, in milliseconds, from the previous frame
 */
void mob_update(mob *pMob, int ms) {
    // Check that the mob is alive
    ASSERT_NR(mob_isAlive(pMob) == GFraMe_ret_ok);
    
    // TODO add AI
    
    GFraMe_sprite_update(&pMob->spr, ms);
    
__ret:
    return;
}

/**
 * Draw the mob
 * 
 * @param pMob The mob
 */
void mob_draw(mob *pMob) {
    // Check that the mob is alive
    ASSERT_NR(mob_isAlive(pMob) == GFraMe_ret_ok);
    
    GFraMe_sprite_draw_camera(&pMob->spr, cam_x, cam_y, SCR_W, SCR_H);
    
__ret:
    return;
}

/**
 * Change the currently playing animation
 * 
 * @param pMob The mob
 * @param n The new animation
 */
void mob_setAnim(mob *pMob, int n) {
    ASSERT_NR(n != pMob->anim);
    ASSERT_NR(n < pMob->animLen);
    
    GFraMe_sprite_set_animation(&pMob->spr, &pMob->mob_anim[n], 0);
    
__ret:
    return;
}

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
 * Check whether the mob is alive or not
 * 
 * @param pMob The mob
 * @return GFraMe_ret_ok if it's alive
 */
GFraMe_ret mob_isAlive(mob *pMob) {
    if (pMob->health > 0)
        return GFraMe_ret_ok;
    return GFraMe_ret_failed;
}

/**
 * Get the mob's object, for collision
 * 
 * @param ppObj Mob's object
 * @param pMob The mob
 */
void mob_getObject(GFraMe_object **ppObj, mob *pMob) {
    *ppObj = &pMob->spr.obj;
}

