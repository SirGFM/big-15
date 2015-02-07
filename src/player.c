/**
 * @file src/player.c
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include "controller.h"
#include "global.h"
#include "player.h"

#include <stdio.h>

enum {
    PL_STAND,
    PL_WALK,
    PL_JUMP,
    PL_FALL
};

struct stPlayer {
    GFraMe_sprite spr;
    
    GFraMe_animation standAnim;
    int standData[1];
    
    GFraMe_animation walkAnim;
    int walkData[8];
    
    int curAnim;
};

/**
 * Initialize the player.
 * 
 * @param ppPl The player
 * @param ID Player's ID
 * @param firstTile Sprite's standing frame (first from its set)
 * @return GFraMe error code
 */
GFraMe_ret player_init(player **ppPl, int ID, int firstTile) {
    //GFraMe_hitbox *hb;
    //GFraMe_object *obj;
    GFraMe_ret rv;
    player *pPl;
    
    // Initialize this so it can be cleaned on error
    pPl = NULL;
    
    // Sanitize parameters
    ASSERT(ppPl, GFraMe_ret_bad_param);
    ASSERT(!*ppPl, GFraMe_ret_bad_param);
    
    // Alloc the player
    pPl = (player*)malloc(sizeof(player));
    ASSERT(pPl, GFraMe_ret_memory_error);
    
    // Create every animation
    pPl->standData[0] = firstTile;
    GFraMe_animation_init(&pPl->standAnim, 0, pPl->standData, 1, 0);
    
    pPl->walkData[0] = firstTile + 1;
    pPl->walkData[1] = firstTile + 2;
    pPl->walkData[2] = firstTile + 3;
    pPl->walkData[3] = firstTile + 4;
    pPl->walkData[4] = firstTile + 5;
    pPl->walkData[5] = firstTile + 6;
    pPl->walkData[6] = firstTile + 7;
    pPl->walkData[7] = firstTile + 8;
    GFraMe_animation_init(&pPl->walkAnim, 12, pPl->walkData, 8, 1);
    
    // Initialize the sprite
    GFraMe_sprite_init(&pPl->spr, 16, 184, 16, 16, gl_sset16x16, 0, 0);
    // TODO properly set the hitbox
    //obj = GFraMe_sprite_get_object(&pPl->spr);
    //hb = GFraMe_object_get_hitbox(obj);
    //GFraMe_hitbox_set(hb, GFraMe_hitbox_upper_left, 0, 0, w, h);
    
    // Play the stand animation
    GFraMe_sprite_set_animation(&pPl->spr, &pPl->standAnim, 1);
    pPl->curAnim = PL_STAND;
    
    pPl->spr.id = ID;
    
    // Set the return variables
    *ppPl = pPl;
    rv = GFraMe_ret_ok;
__ret:
    if (rv != GFraMe_ret_ok && pPl)
        free(pPl);
    
    return rv;
}

/**
 * Clean up the player
 * 
 * @param ppPl The player
 */
void player_clean(player **ppPl) {
    // Sanitize parameters
    ASSERT_NR(ppPl);
    ASSERT_NR(*ppPl);
    
    free(*ppPl);
    *ppPl = NULL;
__ret:
    return;
}

/**
 * Update the player. Also handle input (i.e., jump)
 * 
 * @param pPl The player
 * @param ms Time elapsed from last frame, in milliseconds
 */
void player_update(player *pPl, int ms) {
    GFraMe_object *obj;
    int isDown;
    
    obj = GFraMe_sprite_get_object(&pPl->spr);
    isDown = obj->hit & GFraMe_direction_down;
    
    if (ctr_left(pPl->spr.id)) {
        obj->vx = -PL_VX;
        pPl->spr.flipped = 1;
    }
    else if (ctr_right(pPl->spr.id)) {
        obj->vx = PL_VX;
        pPl->spr.flipped = 0;
    }
    else
        obj->vx = 0;
    
    if (isDown && ctr_jump(pPl->spr.id))
        obj->vy = -PL_JUMPS;
    
    // Set gravity, when in air
    if (!isDown)
        obj->ay = GRAVITY;
    else {
        obj->ay = 0;
        obj->vy = 0;
    }
    
    // Check current state and play the apropriate animation
    if (isDown && obj->vx == 0)
        player_setAnimation(pPl, PL_STAND);
    else if (isDown && obj->vx != 0)
        player_setAnimation(pPl, PL_WALK);
    else if (!isDown && obj->vy < 0)
        player_setAnimation(pPl, PL_JUMP);
    else if (!isDown && obj->vy > 0)
        player_setAnimation(pPl, PL_FALL);
    
    GFraMe_sprite_update(&pPl->spr, ms);
}

/**
 * Draw the player to the screen.
 * 
 * @param pPl The player
 */
void player_draw(player *pPl) {
    GFraMe_sprite_draw(&pPl->spr);
}

/**
 * Get the player's object, for collision
 * 
 * @param ppObj Player's object
 * @param pPl The player
 */
void player_getObject(GFraMe_object **ppObj, player *pPl) {
    GFraMe_object *pObj;
    
    pObj = GFraMe_sprite_get_object(&pPl->spr);
    
    *ppObj = pObj;
}

/**
 * Set, if necessary, the animation
 * 
 * @param pPl The player
 * @param anim The animation
 */
void player_setAnimation(player *pPl, int anim) {
    // Make sure the animation isn't already playing
    ASSERT_NR(pPl->curAnim != anim);
    
    switch (anim) {
        case PL_STAND:
            GFraMe_sprite_set_animation(&pPl->spr, &pPl->standAnim, 1); break;
        case PL_WALK:
            GFraMe_sprite_set_animation(&pPl->spr, &pPl->walkAnim, 1); break;
        case PL_JUMP:
            /* TODO play jump anim */ break;
        case PL_FALL:
            /* TODO play fall anim */ break;
    }
    pPl->curAnim = anim;
    
__ret:
    return;
}

