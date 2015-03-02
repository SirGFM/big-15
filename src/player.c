/**
 * @file src/player.c
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include "camera.h"
#include "controller.h"
#include "global.h"
#include "globalVar.h"
#include "player.h"
#include "types.h"

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
    int isBeingCarried;
    
    // Info about the map the player is trying to move to
    int map;
    int map_x;
    int map_y;
    
    // Tween rates
    int tx;
    int ty;
    int tt;
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
    GFraMe_animation_init(&pPl->walkAnim, 14, pPl->walkData, 8, 1);
    
    // Initialize the sprite
    GFraMe_sprite_init(&pPl->spr, 16, 184, 8, 14, gl_sset16x16, -4, -2);
    
    // Play the stand animation
    GFraMe_sprite_set_animation(&pPl->spr, &pPl->standAnim, 1);
    pPl->curAnim = PL_STAND;
    
    pPl->spr.id = ID;
    pPl->isBeingCarried = 0;
    
    pPl->map = -1;
    pPl->map_x = -1;
    pPl->map_y = -1;
    
    pPl->tx = -1;
    pPl->ty = -1;
    pPl->tt = -1;
    
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
    
    // Reset destiny map
    pPl->map = -1;
    pPl->map_x = -1;
    pPl->map_y = -1;
    
    obj = GFraMe_sprite_get_object(&pPl->spr);
    isDown = obj->hit & GFraMe_direction_down;
    
    // Set player's horizontal speed
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
    
    // Set gravity, when in air
    if (obj->hit & GFraMe_direction_up)
        obj->vy = 0;
    else if (!isDown)
        obj->ay = GRAVITY;
    else {
        obj->ay = 0;
        if (ctr_jump(pPl->spr.id)) {
            if ((pPl->spr.id == ID_PL1 && gv_getValue(PL1_ITEM) == ID_HIGHJUMP) ||
                (pPl->spr.id == ID_PL2 && gv_getValue(PL2_ITEM) == ID_HIGHJUMP))
                obj->vy = -PL_HIGHJUMPS;
            else
                obj->vy = -PL_JUMPS;
        }
        else if (!pPl->isBeingCarried)
            obj->vy = 32;
        else
            obj->ay = GRAVITY;
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
    
    if (pPl->isBeingCarried) {
        int otherID;
        
        otherID = ID_PL2 - pPl->spr.id % ID_PL1;
        if (ctr_left(otherID))
            obj->vx -= PL_VX;
        else if (ctr_right(otherID))
            obj->vx += PL_VX;
        
        pPl->isBeingCarried = 0;
    }
    
    GFraMe_sprite_update(&pPl->spr, ms);
    
    // Store the player's central position so it's easilly acessible
    if ((pPl->spr.id & ID_PL1) == ID_PL1) {
        gv_setValue(PL1_CX, pPl->spr.obj.x + pPl->spr.obj.hitbox.cx);
        gv_setValue(PL1_CY, pPl->spr.obj.y + pPl->spr.obj.hitbox.cy);
    }
    else {
        gv_setValue(PL2_CX, pPl->spr.obj.x + pPl->spr.obj.hitbox.cx);
        gv_setValue(PL2_CY, pPl->spr.obj.y + pPl->spr.obj.hitbox.cy);
    }
}

/**
 * Draw the player to the screen.
 * 
 * @param pPl The player
 */
void player_draw(player *pPl) {
    //GFraMe_sprite_draw(&pPl->spr);
    GFraMe_sprite_draw_camera(&pPl->spr, cam_x, cam_y, SCR_W, SCR_H);
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
 * Get the player's sprite, for collision
 * 
 * @param ppSpr Player's sprite
 * @param pPl The player
 */
void player_getSprite(GFraMe_sprite **ppSpr, player *pPl) {
    *ppSpr = &pPl->spr;
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

//#include "registry.h"

/**
 * Position the player (and set it to be moved) as being carried by another
 * player
 * 
 * @param pPl The player
 * @param pObj The object of the carring player
 */
void player_getCarried(player *pPl, GFraMe_object *pObj) {
    GFraMe_hitbox *pHb;
    GFraMe_object *pThisObj;
    double vy, maxvy;
    
    // Get the required Framework's object
    pThisObj = GFraMe_sprite_get_object(&pPl->spr);
    pHb = GFraMe_object_get_hitbox(pThisObj);
    
    // Set the player as being carried above the other object
    //GFraMe_object_set_y(pThisObj, pObj->y - pHb->hh - pHb->cy);
    pThisObj->dy = pObj->dy - pHb->hh - pHb->cy;
    pThisObj->y = (int)pThisObj->dy;
    pPl->isBeingCarried = 1;
    
    // Get the carrying player vertical speed
    vy = pObj->vy;
    // Modify the speeds if the high jump boots are on
    if (vy < 0)
        maxvy = 0.99 * vy;
    else if (vy > 0)
        maxvy = 1.01 * vy;
    // Modify the player's VY
    if (!ctr_jump(pPl->spr.id))
        pThisObj->vy = maxvy;
}

/**
 * Get a player's center position
 * 
 * @param pX The central horizontal position
 * @param pY The central vertical position
 * @param pPl The player
 */
void player_getCenter(int *pX, int *pY, player *pPl) {
    *pX = pPl->spr.obj.x + pPl->spr.obj.hitbox.cx;
    *pY = pPl->spr.obj.y + pPl->spr.obj.hitbox.cy;
}

/**
 * Set a destination to this player
 * 
 * @param pPl The player
 * @param map The map index
 * @param x The horizontal position inside the new map
 * @param y The vertical position inside the new map
 */
void player_setDestMap(player *pPl, int map, int x, int y) {
    pPl->map = map;
    pPl->map_x = x;
    pPl->map_y = y;
}

/**
 * Check if two players are trying to switch map, and to the same one
 * Also, set the global variables with map info
 * 
 * @param pPl1 One player
 * @param pPl2 The other player
 * @return GFraMe error code
 */
GFraMe_ret player_cmpDestMap(player *pPl1, player *pPl2) {
    GFraMe_ret rv;
    
    // Check that at least one player is trying to switch maps
    ASSERT(pPl1->map != -1, GFraMe_ret_failed);
    
    // Check that their destination is the same
    ASSERT(pPl1->map == pPl2->map, GFraMe_ret_failed);
    ASSERT(pPl1->map_x == pPl2->map_x, GFraMe_ret_failed);
    ASSERT(pPl1->map_y == pPl2->map_y, GFraMe_ret_failed);
    
    gv_setValue(MAP, pPl1->map);
    gv_setValue(DOOR_X, pPl1->map_x);
    gv_setValue(DOOR_Y, pPl1->map_y);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

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
GFraMe_ret player_tweenTo(player *pPl, int x, int y, int ms, int time) {
    GFraMe_object *pO;
    
    // Get the player's object (to easily modify its position)
    pO = GFraMe_sprite_get_object(&pPl->spr);
    
    // Set the original position, if not yet set
    if (pPl->tx == -1 && pPl->ty == -1) {
        pPl->tx = pO->x;
        pPl->ty = pO->y;
        pPl->tt = 0;
    }
    
    // Update the time and tween to the new position
    pPl->tt += ms;
    pO->dx = (pPl->tx * (time - pPl->tt) + x * pPl->tt) / (float)time;
    pO->dy = (pPl->ty * (time - pPl->tt) + y * pPl->tt) / (float)time;
    
    // Set the drawing posititon
    pO->x = (int)pO->dx;
    pO->y = (int)pO->dy;
    
    // If we reached the destination, corret the position and clear the tween
    if (pPl->tt >= time) {
        GFraMe_object_set_x(pO, x);
        GFraMe_object_set_y(pO, y);
        pPl->tx = -1;
        pPl->ty = -1;
        return GFraMe_ret_ok;
    }
    return GFraMe_ret_failed;
}

/**
 * Check if the player is being carried
 * 
 * @param pPl The player
 */
int player_isBeingCarried(player *pPl) {
    return pPl->isBeingCarried;
}

/**
 * Get the player's ID
 * 
 * @param pPl The player
 */
flag player_getID(player *pPl) {
    return pPl->spr.id;
}

