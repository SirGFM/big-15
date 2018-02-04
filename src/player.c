/**
 * @file src/player.c
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#ifdef DEBUG
#  include <GFraMe/GFraMe_pointer.h>
#endif
#include <GFraMe/GFraMe_sprite.h>

#include <stdio.h>
#include <stdlib.h>

#include "audio.h"
#include "camera.h"
#include "controller.h"
#include "global.h"
#include "globalVar.h"
#include "mob.h"
#include "player.h"
#include "registry.h"
#include "signal.h"
#include "types.h"

#define PL1_ICON 286
#define PL2_ICON 287
#define IFRAMES_MS 500

enum {
    PL_STAND,
    PL_WALK,
    PL_JUMP,
    PL_FALL,
    PL_HURT
};

struct stPlayer {
    GFraMe_sprite spr;
    
    GFraMe_animation standAnim;
    int standData[8];
    
    GFraMe_animation walkAnim;
    int walkData[8];
    
    GFraMe_animation hurtAnim;
    int hurtData[8];
    
    GFraMe_animation jumpAnim;
    int jumpData[1];
    
    GFraMe_animation fallAnim;
    int fallData[1];
    
    int curAnim;
    int isBeingCarried;
    int isBeingCarriedBoss;
    int pressedTeleport;
    int isTeleporting;
    int lastItemSwitch;
    
    // Info about the map the player is trying to move to
    int map;
    int map_x;
    int map_y;
    
    // Tween rates
    int tx;
    int ty;
    int tt;
    
    int step;
    int iframes;
    int skip;
};

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
GFraMe_ret player_init(player **ppPl, int ID, int firstTile, int x, int y) {
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
    pPl->hurtData[0] = firstTile + 9;
    pPl->hurtData[1] = firstTile + 10;
    pPl->hurtData[2] = firstTile + 9;
    pPl->hurtData[3] = firstTile + 10;
    pPl->hurtData[4] = firstTile + 9;
    pPl->hurtData[5] = firstTile + 10;
    pPl->hurtData[6] = firstTile + 9;
    pPl->hurtData[7] = firstTile + 10;
    GFraMe_animation_init(&pPl->hurtAnim, 12, pPl->hurtData, 8, 0);
    
    pPl->standData[0] = firstTile;
    pPl->standData[1] = firstTile;
    pPl->standData[2] = firstTile + 11;
    pPl->standData[3] = firstTile;
    pPl->standData[4] = firstTile;
    pPl->standData[5] = firstTile + 13;
    pPl->standData[6] = firstTile;
    pPl->standData[7] = firstTile + 12;
    GFraMe_animation_init(&pPl->standAnim, 8, pPl->standData, 8, 1);
    
    pPl->walkData[0] = firstTile + 1;
    pPl->walkData[1] = firstTile + 2;
    pPl->walkData[2] = firstTile + 3;
    pPl->walkData[3] = firstTile + 4;
    pPl->walkData[4] = firstTile + 5;
    pPl->walkData[5] = firstTile + 6;
    pPl->walkData[6] = firstTile + 7;
    pPl->walkData[7] = firstTile + 8;
    GFraMe_animation_init(&pPl->walkAnim, 14, pPl->walkData, 8, 1);
    
    pPl->jumpData[0] = firstTile + 14;
    GFraMe_animation_init(&pPl->jumpAnim, 0, pPl->jumpData, 1, 0);
    
    pPl->fallData[0] = firstTile + 15;
    GFraMe_animation_init(&pPl->fallAnim, 0, pPl->fallData, 1, 0);
    
    // Initialize the sprite
    GFraMe_sprite_init(&pPl->spr, x, y, 8/*w*/, 14/*h*/, gl_sset16x16, -4, -2);
    
    // Play the stand animation
    GFraMe_sprite_set_animation(&pPl->spr, &pPl->standAnim, 1);
    pPl->curAnim = PL_STAND;
    
    pPl->spr.id = ID;
    pPl->isBeingCarried = 0;
    pPl->pressedTeleport = 0;
    pPl->isTeleporting = 0;
    pPl->lastItemSwitch = 0;
    
    pPl->map = -1;
    pPl->map_x = -1;
    pPl->map_y = -1;
    
    pPl->tx = -1;
    pPl->ty = -1;
    pPl->tt = -1;
    
    pPl->step = 0;
    pPl->iframes = 0;
    pPl->skip = 0;
    
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
    
#ifdef DEBUG
    if (GFraMe_pointer_pressed) {
        GFraMe_object_set_pos(&pPl->spr.obj, GFraMe_pointer_x + cam_x, GFraMe_pointer_y + cam_y);
    }
#endif
    
    // Decrease item switch cooldown
    if (pPl->lastItemSwitch > 0)
        pPl->lastItemSwitch -= ms;

    // Decrease the iframes
    if (pPl->curAnim != PL_HURT && pPl->iframes > ms) {
        pPl->iframes -= ms;
    }
    else if (pPl->curAnim != PL_HURT) {
        pPl->iframes = 0;
    }
    
    obj = GFraMe_sprite_get_object(&pPl->spr);
    
    // Do nothing while the player is hurting
    ASSERT_NR(pPl->curAnim != PL_HURT || !pPl->spr.anim);
    
    // Check if the player should teleport
    if (pPl->isTeleporting) {
        int x, y;
        
        x = gv_getValue(TELP_X);
        y = gv_getValue(TELP_Y);
        
        pPl->spr.obj.dx = x;
        pPl->spr.obj.dy = y;
        pPl->isTeleporting = 0;
        
        sfx_teleport();
        goto __ret;
        //return;
    }
    
    isDown = obj->hit & GFraMe_direction_down;
    
    // Check if the player just set the signaler
    if (!pPl->pressedTeleport && ctr_item(pPl->spr.id)) {
        int item;
        
        if (pPl->spr.id == ID_PL1)
            item = gv_getValue(PL1_ITEM);
        else if (pPl->spr.id == ID_PL2)
            item = gv_getValue(PL2_ITEM);
        else item = 0; // Screw warnings!
        
       
        if (item == ID_SIGNALER) {
            int cx, cy;
            
            // Set the signaler position
            cx = obj->dx + obj->hitbox.cx;
            cy = obj->dy + obj->hitbox.cy;
            if (obj->hit & GFraMe_direction_left)
                cx += 1;
            else if (obj->hit & GFraMe_direction_right)
                cx -= 1;
            signal_setPos(cx, cy);
            
            pPl->pressedTeleport = 1;
        }
    }
    
    
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
    if (obj->hit & GFraMe_direction_up) {
        obj->vy = 0;
        if (!isDown) {
            sfx_plFall();
        }
    }
    else if (!isDown)
        obj->ay = GRAVITY;
    else {
        obj->ay = 0;
        if (ctr_jump(pPl->spr.id)) {
            if ((pPl->spr.id == ID_PL1 && gv_getValue(PL1_ITEM) == ID_HIGHJUMP) ||
                (pPl->spr.id == ID_PL2 && gv_getValue(PL2_ITEM) == ID_HIGHJUMP)) {
                obj->vy = -PL_HIGHJUMPS;
                sfx_plHighJump();
            }
            else {
                obj->vy = -PL_JUMPS;
                sfx_plJump();
            }
        }
        else if (!pPl->isBeingCarried && !pPl->isBeingCarriedBoss)
            obj->vy = 32;
//        else
//            obj->ay = GRAVITY;
    }
    
    if (pPl->curAnim == PL_FALL && isDown && !(obj->hit & GFraMe_direction_last_down)) {
        sfx_plFall();
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
    if (pPl->isBeingCarriedBoss) {
        if (gv_isZero(BOSS_DIR)) {
            // Boss is facing right
            obj->vx += BOSS_SPEED;
        }
        else {
            // Boss is facing left
            obj->vx -= BOSS_SPEED;
        }
        pPl->isBeingCarriedBoss = 0;
    }
    
    if (pPl->curAnim == PL_WALK && (pPl->spr.anim->index == 3
        || pPl->spr.anim->index == 7)) {
        if (!pPl->step)
            sfx_plStep();
        pPl->step = 1;
    }
    else {
        pPl->step = 0;
    }
    
__ret:
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
    int x, y;

    if (pPl->iframes != 0 && pPl->curAnim != PL_HURT) {
        // Skip two frames every two frames, if with iframes.
        // (i.e., render 2, then skip 2)
        pPl->skip = (pPl->skip + 1) & 0x3;
        if (pPl->skip & 0x2) {
            return;
        }
    }
    
    // Check if the player is inside the camera
    x = pPl->spr.obj.x - cam_x;
    y = pPl->spr.obj.y - cam_y;
    if (x >= 0 && x <= SCR_W && y >= 0 && y <= SCR_H)
        GFraMe_sprite_draw_camera(&pPl->spr, cam_x, cam_y, SCR_W, SCR_H);
    else {
        if (x < 0)
            x = 0;
        else if (x + 8 > SCR_W)
            x = SCR_W - 8;
        if (y < 0)
            y = 0;
        else if (y + 8 > SCR_H)
            y = SCR_H - 8;
        if (pPl->spr.id == ID_PL1) {
            GFraMe_spriteset_draw(gl_sset8x8, PL1_ICON, x, y, pPl->spr.flipped);
        }
        else if (pPl->spr.id == ID_PL2) {
            GFraMe_spriteset_draw(gl_sset8x8, PL2_ICON, x, y, pPl->spr.flipped);
        }
    }
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
            GFraMe_sprite_set_animation(&pPl->spr, &pPl->jumpAnim, 1); break;
        case PL_FALL:
            GFraMe_sprite_set_animation(&pPl->spr, &pPl->fallAnim, 1); break;
        case PL_HURT:
            GFraMe_sprite_set_animation(&pPl->spr, &pPl->hurtAnim, 0); break;
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
 * @param pOther The other player
 */
void player_getCarried(player *pPl, player *pOther) {
    GFraMe_object *pThisObj, *pObj;
    
    // Get the required Framework's object
    pThisObj = GFraMe_sprite_get_object(&pPl->spr);
    pObj = GFraMe_sprite_get_object(&pOther->spr);
    
    // Set the player as being carried above the other object
    pPl->isBeingCarried = 1;
    pPl->isBeingCarriedBoss = pOther->isBeingCarriedBoss;

    // Modify the player's VY
    pThisObj->vy = pObj->vy + 32;
}

/**
 * Position the player (and set it to be moved) as being carried by the boss
 * 
 * @param pPl The player
 * @param pObj The object of the boss
 */
void player_getCarriedBoss(player *pPl, GFraMe_object *pObj) {
    // Set the player as being carried above the other object
    pPl->isBeingCarriedBoss = 1;
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
 * Set a destination to this player. Does nothing if the player is in hitstun.
 * 
 * @param pPl The player
 * @param map The map index
 * @param x The horizontal position inside the new map
 * @param y The vertical position inside the new map
 */
void player_setDestMap(player *pPl, int map, int x, int y) {
    if (pPl->curAnim == PL_HURT) {
        return;
    }

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

/**
 * Checks if the player should teleport and setup everything
 * 
 * @param pPl The player
 */
void player_checkTeleport(player *pPl) {
    GFraMe_object *pObj;
    int item, otherItem, x, y;
    int isSide;
    
    // Get both players' items
    if (pPl->spr.id == ID_PL1) {
        item = gv_getValue(PL1_ITEM);
        otherItem = gv_getValue(PL2_ITEM);
        isSide = p2->spr.obj.hit & (GFraMe_direction_left | GFraMe_direction_right);
    }
    else if (pPl->spr.id == ID_PL2) {
        item = gv_getValue(PL2_ITEM);
        otherItem = gv_getValue(PL1_ITEM);
        isSide = p1->spr.obj.hit & (GFraMe_direction_left | GFraMe_direction_right);
    }
    else {
        ASSERT_NR(0);
    }
    // Check (and set) whether the player is teleporting
    ASSERT_NR(item == ID_TELEPORT);
    ASSERT_NR(!pPl->pressedTeleport);
    pPl->pressedTeleport = 1;
    // Check that it was triggered
    ASSERT_NR(ctr_item(pPl->spr.id));
    
    pObj = GFraMe_sprite_get_object(&pPl->spr);
    // Find the destination position
    x = gv_getValue(SIGL_X);
    y = gv_getValue(SIGL_Y);
    if (x == -1 || y == -1) {
        // Check whether teleporting is possible
        ASSERT_NR(otherItem == ID_SIGNALER && !isSide);
        
        // If the signaler isn't set, teleport to the other player
        if (pPl->spr.id == ID_PL1) {
            x = gv_getValue(PL2_CX);
            y = gv_getValue(PL2_CY);
        }
        else {
            x = gv_getValue(PL1_CX);
            y = gv_getValue(PL1_CY);
        }
        x -= pObj->hitbox.hw;
        y -= pObj->hitbox.hh;
        
        signal_setPos(x+4, y+6);
        signal_release();
    }
    else {
        y -= 8;
    }
    gv_setValue(TELP_X, x);
    gv_setValue(TELP_Y, y);
    
    pPl->isTeleporting = 1;
    signal_release();
__ret:
    if (!ctr_item(pPl->spr.id)) {
        pPl->pressedTeleport = 0;
    }
    else if (pPl->pressedTeleport) {
        // TODO fix this check
        // TODO play failure sound
    }
    
    return;
}

/**
 * Checks and change a player's item
 * 
 * @param pPl The player */
void player_changeItem(player *pPl) {
    int curItem, i, items, otherItem, prev;
    
    // Check if it's actually changing it
    ASSERT_NR(ctr_switchItem(pPl->spr.id));
    ASSERT_NR(pPl->lastItemSwitch <= 0);
    
    // Get all enabled items (and the current ones)
    items = gv_getValue(ITEMS);
    if (pPl->spr.id == ID_PL1) {
        curItem = gv_getValue(PL1_ITEM);
        otherItem = gv_getValue(PL2_ITEM);
    }
    else if (pPl->spr.id == ID_PL2) {
        curItem = gv_getValue(PL2_ITEM);
        otherItem = gv_getValue(PL1_ITEM);
    }
    else {
        // Screw warnings
        curItem = 0;
        otherItem = 0;
    }
    prev = curItem;
    
    if (curItem)
        i = curItem << 1;
    else
        i = 1;
    while (1) {
        if ((items & i) && otherItem != i)
            break;
        
        if (i == ID_LASTITEM) {
            i = 0;
            break;
        }
        i <<= 1;
    }
    if (pPl->spr.id == ID_PL1)
        gv_setValue(PL1_ITEM, i);
    else if (pPl->spr.id == ID_PL2)
        gv_setValue(PL2_ITEM, i);
    
    if (i != prev) {
        sfx_switchItem();
    }
    // Set the cooldown
    pPl->lastItemSwitch += 300;
__ret:
    // Reset if the button was released
    if (!ctr_switchItem(pPl->spr.id))
        pPl->lastItemSwitch = 0;
    
    return;
}

/**
 * Deal some damage to a player and play an animation
 * 
 * @param pPl The player
 * @param dmg How much damage should be dealt
 * @param dir Direction the player was hit from
 */
void player_hurt(player *pPl, int dmg, GFraMe_direction dir) {
    int x, y;
    
    // Check that the player isn't "hurting"
    ASSERT_NR(pPl->curAnim != PL_HURT);
    ASSERT_NR(pPl->iframes == 0);
    
    // Decreate the health
    if (pPl->spr.id == ID_PL1) {
        gv_sub(PL1_HP, dmg);
        if (gv_getValue(PL1_HP) <= 0)
            sfx_plDeath();
    }
    else if (pPl->spr.id == ID_PL2) {
        gv_sub(PL2_HP, dmg);
        if (gv_getValue(PL2_HP) <= 0)
            sfx_plDeath();
    }

    x = pPl->spr.obj.x + pPl->spr.obj.hitbox.cx;
    y = pPl->spr.obj.y + pPl->spr.obj.hitbox.cy + pPl->spr.obj.hitbox.hh - 1;
    // Push back the player
    if (dir == GFraMe_direction_left) {
        if (map_isPixelSolid(m, x+8, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x+16, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x+24, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x+32, y) == GFraMe_ret_failed)
            pPl->spr.obj.vx = 32;
        else
            pPl->spr.obj.vx = -32;
    }
    else if (dir == GFraMe_direction_right) {
        if (map_isPixelSolid(m, x-8, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x-16, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x-24, y) == GFraMe_ret_failed
            && map_isPixelSolid(m, x-32, y) == GFraMe_ret_failed)
            pPl->spr.obj.vx = -32;
        else
            pPl->spr.obj.vx = 32;
    }
    else {
        pPl->spr.obj.vx = 16;
    }
    pPl->spr.obj.vy = 0;
    // Play the 'hurt' animation
    player_setAnimation(pPl, PL_HURT);
    sfx_plHurt();
    // Set some iframes
    pPl->iframes = IFRAMES_MS;
__ret:
    return;
}

/**
 * Return when the player is still in hitstun
 * 
 * @param pPl The player
 * @return Whether the player is in hitstun or not
 */
int player_isHurt(player *pPl) {
    return pPl->curAnim == PL_HURT;
}

/**
 * Return whether a player is alive
 * 
 * @param pPl The player
 * @return 1 on sucess, 0 otherwise
 */
int player_isAlive(player *pPl) {
    if (pPl->spr.id == ID_PL1)
        return gv_getValue(PL1_HP) > 0;
    else if (pPl->spr.id == ID_PL2)
        return gv_getValue(PL2_HP) > 0;
    return 0;
}

/**
 * Check if a player is within vertical bounds of the map
 * 
 * @param pPl The player
 * @return 1 on sucess, 0 otherwise
 */
int player_isInsideMap(player *pPl) {
    int w, h;
    
    map_getDimensions(m, &w, &h);
    
    return pPl->spr.obj.y <= h;
}

/**
 * Resets a player vertical speed.
 * Only used when 'retry' is selected from the pause menu.
 * 
 * @param pPl The player
 */
void player_resetVerticalSpeed(player *pPl) {
    pPl->spr.obj.vy = 0;
}
