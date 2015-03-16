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

#include "bullet.h"
#include "camera.h"
#include "global.h"
#include "map.h"
#include "mob.h"
#include "registry.h"
#include "types.h"

#define MOB_ANIM_MAX 6

/** States for the boss' mob */
enum {BOSS_HEAD_DEF = 0, BOSS_HEAD_ATTACK, BOSS_HEAD_HURT};
enum {BOSS_WHEEL_STAND = 0, BOSS_WHEEL_RIGHT, BOSS_WHEEL_LEFT};
enum {BOSS_TANK_DEF = 0};
enum {BOSS_PLAT_DEF = 0};
#define BOSS_HEAD_COUNTDOWN 2250
#define BOSS_HEAD_MAXSPEED 160
/** States for the 'phantom' mob */
enum {PHANTOM_STAND = 0};
#define PHANTOM_MAXSPEED 100
/** States for the seifer('charger') mob */
enum {CHARGER_STAND = 0, CHARGER_FLOAT, CHARGER_CHARGE};
#define CHARGER_COUNTDOWN 1500
#define CHARGER_DIST 80
/** States for the 'jumper' mob */
enum {JUMPER_STAND = 0, JUMPER_PREJUMP, JUMPER_JUMP, JUMPER_LANDED };
#define JUMPER_COUNTDOWN 1000
/** States for the wallee('eye') mob */
enum {EYE_CLOSED = 0, EYE_OPENING, EYE_OPEN, EYE_FOCUSED, EYE_CLOSING, EYE_BLINK};
#define EYE_COUNTDOWN  500
#define EYE_MINDIST 64
#define EYE_MAXDIST 96

struct stMob {
    GFraMe_sprite spr;       /** Mob's sprite (for rendering and collision)   */
    int health;              /** How many hitpoints this mob has              */
    int damage;              /** How much damage this mob does on the player  */
    flag weakness;           /** IDs that can do damage to this mob           */
    int countdown;           /** Counter, in frames, for next action          */
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
/* fps,len,loop,data...                         */
    2 , 2 , 1  , 704, 705,        /* stand      */
    6 , 1 , 0  , 706,             /* pre-jump   */
    0 , 1 , 0  , 707,             /* jump       */
    6 , 1 , 0  , 706              /* after-jump */
};
static int _mob_eyeAnimData[] = {
/* fps,len,loop,data...                         */
    0 , 1 , 0  , 736,             /* closed     */
    8 , 2 , 0  , 737, 738,        /* opening    */
    0 , 1 , 0  , 738,             /* open       */
    4 , 1 , 0  , 737,             /* focused    */
    8 , 2 , 0  , 737, 736,        /* closing    */
   12 , 3 , 0  , 737, 736, 737    /* blink      */
};
static int _mob_chargerAnimData[] = {
/* fps,len,loop,data...                         */
    6 , 2 , 1  , 201, 217,        /* stand      */
    0 , 1 , 0  , 202,             /* float      */
    0 , 1 , 0  , 218              /* charge     */
};
static int _mob_phantomAnimData[] = {
/* fps,len,loop,data...                         */
   12 , 4 , 1  , 204, 205,206,207 /* stand      */
};
static int _mob_bossHeadAnimData[] = {
/* fps,len,loop,data...                         */
    0 , 1 , 0  ,180,              /* def        */
    2 , 1 , 0  ,181,              /* attack     */
    8 , 8 , 0  ,182,183,182,183,182,183,182,183/* hurt */
};
static int _mob_bossWheelAnimData[] = {
/* fps,len,loop,data...                         */
    0 , 1 , 0  , 83,              /* stand      */
    12, 3 , 1  , 91, 87, 83,      /* right      */
    12, 3 , 1  , 83, 87, 91       /* left       */
};
static int _mob_bossTankAnimData[] = {
/* fps,len,loop,data...                         */
    0 , 1 , 0  , 44               /* def        */
};
static int _mob_bossPlatAnimData[] = {
/* fps,len,loop,data...                         */
    0 , 1 , 0  , 41               /* def        */
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
    
    #define SET_ANIMDATA(__animData__) \
            animData = __animData__; \
            dataLen = sizeof(__animData__) / sizeof(int)
    // Initialize the mob
    animData = 0;
    switch (type) {
        case ID_JUMPER: {
            GFraMe_sprite_init(&pMob->spr, x, y, 6/*w*/, 6/*h*/, gl_sset8x8,
                -1/*ox*/, -1/*oy*/);
            
            pMob->spr.obj.ay = GRAVITY;
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = JUMPER_COUNTDOWN;
            
            SET_ANIMDATA(_mob_jumperAnimData);
        } break;
        case ID_EYE_LEFT:
        case ID_EYE: {
            GFraMe_sprite_init(&pMob->spr, x, y, 8/*w*/, 8/*h*/, gl_sset8x8,
                0/*ox*/, 0/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = EYE_COUNTDOWN;
            
            if (type == ID_EYE_LEFT) {
                pMob->spr.flipped= 1;
                type = ID_EYE;
            }
            
            SET_ANIMDATA(_mob_eyeAnimData);
        } break;
        case ID_CHARGER: {
            GFraMe_sprite_init(&pMob->spr, x, y, 10/*w*/, 12/*h*/, gl_sset16x16,
                -4/*ox*/, -4/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = CHARGER_COUNTDOWN;
            
            SET_ANIMDATA(_mob_chargerAnimData);
        } break;
        case ID_PHANTOM: {
            GFraMe_sprite_init(&pMob->spr, x, y, 6/*w*/, 8/*h*/, gl_sset16x16,
                -4/*ox*/, -4/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = 0;
            
            SET_ANIMDATA(_mob_phantomAnimData);
        } break;
        case ID_BOSS_HEAD: {
            GFraMe_sprite_init(&pMob->spr, x, y, 12/*w*/, 12/*h*/, gl_sset16x16,
                -2/*ox*/, -2/*oy*/);
            pMob->health = 3;
            pMob->damage = 1;
            pMob->countdown = BOSS_HEAD_COUNTDOWN;
            
            SET_ANIMDATA(_mob_bossHeadAnimData);
        } break;
        case ID_BOSS_WHEEL: {
            GFraMe_sprite_init(&pMob->spr, x, y, 46/*w*/, 8/*h*/, gl_sset64x8,
                3/*ox*/, 0/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = 0;
            
            SET_ANIMDATA(_mob_bossWheelAnimData);
        } break;
        case ID_BOSS_TANK: {
            GFraMe_sprite_init(&pMob->spr, x, y, 20/*w*/, 20/*h*/, gl_sset32x32,
                -2/*ox*/, -4/*oy*/);
            pMob->health = 1;
            pMob->damage = 1;
            pMob->countdown = 0;
            
            SET_ANIMDATA(_mob_bossTankAnimData);
        } break;
        case ID_BOSS_PLAT: {
            GFraMe_sprite_init(&pMob->spr, x, y, 44/*w*/, 12/*h*/, gl_sset64x16,
                0/*ox*/, 0/*oy*/);
            pMob->health = 1;
            pMob->damage = 0;
            pMob->countdown = 0;
            
            SET_ANIMDATA(_mob_bossPlatAnimData);
        } break;
        default: {
            GFraMe_assertRV(0, "Invalid mob type!", rv = GFraMe_ret_failed,
                __ret);
        }
    }
    #undef SET_ANIMDATA
    pMob->spr.id = type;
    
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
        pMob->animLen = j;
    }
    
    // Set the mob animation
    pMob->anim = -1;
    mob_setAnim(pMob, 0, 0);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Draw the mob
 * 
 * @param pMob The mob
 */
void mob_draw(mob *pMob) {
    // Check that the mob is alive
    ASSERT_NR(mob_isAlive(pMob));
    
    switch (pMob->spr.id) {
        case ID_EYE: {
            int x, y;
            
            x = pMob->spr.obj.x - cam_x;
            y = pMob->spr.obj.y - cam_y;
            
            if (pMob->anim != EYE_CLOSED) {
                int pX, pY;
                // Render the eye ball
                if (pMob->spr.flipped)
                    GFraMe_spriteset_draw(gl_sset4x4, 2951/*tile*/, x  , y+3, 1);
                else
                    GFraMe_spriteset_draw(gl_sset4x4, 2951/*tile*/, x+4, y+3, 0);
                // Get the closest player's position
                mob_getClosestPlDist(&pX, &pY, pMob);
                pX = pX / (float)(EYE_MAXDIST) * 2.0f + 1;
                pY = pY / (float)(EYE_MAXDIST) * 3.0f + 1;
                // Render the pupil
                if (pMob->spr.flipped)
                    GFraMe_spriteset_draw(gl_sset4x4, 2950/*tile*/, x - 2 + pX,
                        y + 3 + pY, 1);
                else
                    GFraMe_spriteset_draw(gl_sset4x4, 2950/*tile*/, x + 4 + pX,
                        y + 3 + pY, 0);
            }
            // Render the eyelid
            GFraMe_sprite_draw_camera(&pMob->spr, cam_x, cam_y, SCR_W, SCR_H);
        } break;
        case ID_CHARGER: {
            if (pMob->anim != CHARGER_STAND) {
                int x, y;
                
                // Draw an after effect based on the velocity
                x = pMob->spr.obj.x - cam_x + pMob->spr.offset_x;
                x -= pMob->spr.obj.vx / 20;
                y = pMob->spr.obj.y - cam_y + pMob->spr.offset_x;
                GFraMe_spriteset_draw(gl_sset16x16, pMob->spr.cur_tile + 1, x,
                    y, pMob->spr.flipped); 
            }
            GFraMe_sprite_draw_camera(&pMob->spr, cam_x, cam_y, SCR_W, SCR_H);
        } break;
        default: {
            GFraMe_sprite_draw_camera(&pMob->spr, cam_x, cam_y, SCR_W, SCR_H);
        }
    }
    
__ret:
    return;
}

/**
 * Change the currently playing animation
 * 
 * @param pMob The mob
 * @param n The new animation
 * @param dontReset Whether the animation shouldn't be reset
 */
void mob_setAnim(mob *pMob, int n, int dontReset) {
    ASSERT_NR(n != pMob->anim);
    ASSERT_NR(n >= 0);
    ASSERT_NR(n < pMob->animLen);
    
    GFraMe_sprite_set_animation(&pMob->spr, &pMob->mob_anim[n], dontReset);
    
    pMob->anim = n;
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
void mob_getClosestPlDist(int *pDx, int *pDy, mob *pMob) {
    GFraMe_object *pObj;
    int p1X, p1Y, p2X, p2Y;
    
    mob_getObject(&pObj, pMob);
    // Get both player positions (relative to the mob)
    p1X = gv_getValue(PL1_CX) - pObj->x - pObj->hitbox.cx;
    p1Y = gv_getValue(PL1_CY) - pObj->y - pObj->hitbox.cy;
    p2X = gv_getValue(PL2_CX) - pObj->x - pObj->hitbox.cx;
    p2Y = gv_getValue(PL2_CY) - pObj->y - pObj->hitbox.cy;
    
    // Return the closest player position
    if (p1X*p1X + p1Y*p1Y < p2X*p2X + p2Y*p2Y) {
        *pDx = p1X;
        *pDy = p1Y;
    }
    else {
        *pDx = p2X;
        *pDy = p2Y;
    }
}

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
int mob_getDamage(mob *pMob) {
    return pMob->damage;
}

/**
 * Check whether the mob is alive or not
 * 
 * @param pMob The mob
 * @return 1 it's alive, 0 otherwise
 */
int mob_isAlive(mob *pMob) {
    return pMob->health > 0;
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

/**
 * Check if a mob animation finished playing
 * 
 * @param pMob The mob
 * @return 1 if true, 0 if false
 */
int mob_didAnimFinish(mob *pMob) {
    return pMob->spr.anim == NULL;
}

/**
 * Read the mob's ID
 * 
 * @param pMob The mob
 * @return The mob ID
 */
int mob_getID(mob *pMob) {
    return pMob->spr.id;
}

/**
 * Updates the mob
 * 
 * @param pMob The mob
 * @param ms Time elapsed, in milliseconds, from the previous frame
 */
void mob_update(mob *pMob, int ms) {
    int isDown;
    
    // Check that the mob is alive
    ASSERT_NR(mob_isAlive(pMob));
    
    isDown = pMob->spr.obj.hit & GFraMe_direction_down;
    if (pMob->countdown > 0)
        pMob->countdown -= ms;
    
    switch (pMob->spr.id) {
        case ID_JUMPER: {
            // Check if next action should be selected
            if (pMob->anim == JUMPER_STAND && pMob->countdown <= 0) {
                mob_setAnim(pMob, JUMPER_PREJUMP, 0);
            }
            else if (pMob->anim == JUMPER_PREJUMP && mob_didAnimFinish(pMob)) {
                mob_setAnim(pMob, JUMPER_JUMP, 0);
                
                // Set the horizontal speed according to the mob direction
                if (pMob->spr.flipped)
                    pMob->spr.obj.vx = 16;
                else
                    pMob->spr.obj.vx = -16;
                
                pMob->spr.obj.vy = -GRAVITY / 4;
            }
            else if (pMob->anim == JUMPER_JUMP && isDown) {
                GFraMe_object *pObj;
                
                // Set the new animation and stop the mob
                mob_setAnim(pMob, JUMPER_LANDED, 0);
                pMob->spr.obj.vx = 0;
                
                mob_getObject(&pObj, pMob);
                
                // If the mob is touching either side, make it move to the other one
                if (pObj->hit & GFraMe_direction_left)
                    pMob->spr.flipped = 1;
                else if (pObj->hit & GFraMe_direction_right)
                    pMob->spr.flipped = 0;
                else {
                    GFraMe_hitbox *pHb;
                    GFraMe_ret rv;
                    int speed;
                    
                    // Other wise, check if the next tile is solid
                    if (pMob->spr.flipped)
                        speed = 16;
                    else
                        speed = -16;
                    
                    pHb = GFraMe_object_get_hitbox(pObj);
                
                    rv = map_isPixelSolid(m, pObj->x + pHb->cx + speed / 2, pObj->y + pHb->cy + pHb->hh);
                    if (rv != GFraMe_ret_ok) {
                        pMob->spr.flipped = !pMob->spr.flipped;
                    }
                }
            }
            else if (pMob->anim == JUMPER_LANDED && mob_didAnimFinish(pMob)) {
                // Set the new animation and update the countdown
                mob_setAnim(pMob, JUMPER_STAND, 0);
                pMob->countdown += JUMPER_COUNTDOWN;
            }
            // Make sure the mob is always grounded
            if (pMob->anim != JUMPER_JUMP && isDown)
                pMob->spr.obj.vy = 32;
        } break; /* ID_JUMPER */
        case ID_EYE: {
            if (pMob->anim == EYE_CLOSED && pMob->countdown <= 0) {
                int x, y;
                // Get the closest player's position
                mob_getClosestPlDist(&x, &y, pMob);
                // Check if any player is at least 8 tiles close
                if (x*x + y*y <= EYE_MINDIST*EYE_MINDIST) {
                    // Set the new animation
                    mob_setAnim(pMob, EYE_OPENING, 0);
                }
            }
            else if (pMob->anim == EYE_OPENING && mob_didAnimFinish(pMob)) {
                mob_setAnim(pMob, EYE_OPEN, 0);
                pMob->countdown += EYE_COUNTDOWN;
            }
            else if (pMob->anim == EYE_OPEN && pMob->countdown <= 0) {
                int x, y;
                // Get the closest player's position
                mob_getClosestPlDist(&x, &y, pMob);
                // If the player got 11 tiles away, stop following
                if (x*x + y*y > EYE_MAXDIST*EYE_MAXDIST) {
                    // Set the new animation
                    mob_setAnim(pMob, EYE_CLOSING, 0);
                }
                else {
                    mob_setAnim(pMob, EYE_BLINK, 0);
                }
            }
            else if (pMob->anim == EYE_CLOSING && mob_didAnimFinish(pMob)) {
                mob_setAnim(pMob, EYE_CLOSED, 0);
                pMob->countdown += EYE_COUNTDOWN;
            }
            else if (pMob->anim == EYE_BLINK && mob_didAnimFinish(pMob)) {
                bullet *pBul;
                GFraMe_ret rv;
                int cx, cy, dx, dy;
                
                // Get the closest player's position
                mob_getClosestPlDist(&dx, &dy, pMob);
                if (pMob->spr.flipped) {
                    cx = pMob->spr.obj.x - 2;
                    dx += 4;
                }
                else
                    cx = pMob->spr.obj.x + 12;
                cy = pMob->spr.obj.y + 8;
                
                mob_setAnim(pMob, EYE_FOCUSED, 0);
                pMob->countdown += EYE_COUNTDOWN;
                
                // Recycle a bullet
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // SHOOT!
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx+dx, cy+dy);
                ASSERT_NR(rv == GFraMe_ret_ok);
            }
            else if (pMob->anim == EYE_FOCUSED && pMob->countdown <= 0) {
                mob_setAnim(pMob, EYE_OPEN, 0);
                pMob->countdown += EYE_COUNTDOWN * 2;
            }
        } break; /* ID_EYE */
        case ID_CHARGER: {
            GFraMe_object *pObj;
            GFraMe_hitbox *pHb;
            
            mob_getObject(&pObj, pMob);
            pHb = GFraMe_object_get_hitbox(pObj);
            
            pObj->ay = GRAVITY;
            if (pObj->hit & GFraMe_direction_down) {
                pObj->vy = 32;
            }
            if ((pObj->hit & GFraMe_direction_left) && pObj->vx < 0) {
                pMob->spr.flipped = 0;
                pObj->vx *= -1;
            }
            else if ((pObj->hit & GFraMe_direction_right) && pObj->vx > 0) {
                pMob->spr.flipped = 1;
                pObj->vx *= -1;
            }
            else if ((!pMob->spr.flipped && map_isPixelSolid(m,
                pObj->x + pHb->cx + pHb->hw,
                pObj->y + pHb->cy + pHb->hh) != GFraMe_ret_ok)
                || (pMob->spr.flipped && map_isPixelSolid(m,
                pObj->x + pHb->cx - pHb->hw,
                pObj->y + pHb->cy + pHb->hh) != GFraMe_ret_ok))
                {
                pMob->spr.flipped = !pMob->spr.flipped;
                pObj->vx *= -1;
            }
            
            
            if (pMob->anim == CHARGER_STAND && pMob->countdown <= 0) {
                mob_setAnim(pMob, CHARGER_FLOAT, 0);
                pMob->countdown += CHARGER_COUNTDOWN;
                if (pMob->spr.flipped)
                    pObj->vx = -120;
                else
                    pObj->vx = 120;
            }
            else if (pMob->anim == CHARGER_FLOAT && pMob->countdown > 0) {
                int x, y;
                
                // Get the closest player's position
                mob_getClosestPlDist(&x, &y, pMob);
                
                if (x*x + y*y <= CHARGER_DIST*CHARGER_DIST) {
                    mob_setAnim(pMob, CHARGER_CHARGE, 0);
                    pMob->countdown += CHARGER_COUNTDOWN;
                    pObj->vx *= 2.5;
                }
            }
            else if (pMob->anim != CHARGER_STAND && pMob->countdown <= 0) {
                mob_setAnim(pMob, CHARGER_STAND, 0);
                pMob->countdown += CHARGER_COUNTDOWN;
                pObj->vx = 0;
            }
        } break; /* ID_CHARGER */
        case ID_PHANTOM: {
            GFraMe_object *pObj;
            int x, y;
            
            // Get the mob's object
            mob_getObject(&pObj, pMob);
            // Get the closest player's position
            mob_getClosestPlDist(&x, &y, pMob);
            
            pObj->ax = (x / 8) * 50;
            pObj->ay = (y / 8) * 50;
            
            if (pObj->vx > PHANTOM_MAXSPEED)
                pObj->vx = PHANTOM_MAXSPEED;
            else if (pObj->vx < -PHANTOM_MAXSPEED)
                pObj->vx = -PHANTOM_MAXSPEED;
            if (pObj->vy > PHANTOM_MAXSPEED)
                pObj->vy = PHANTOM_MAXSPEED;
            else if (pObj->vy < -PHANTOM_MAXSPEED)
                pObj->vy = -PHANTOM_MAXSPEED;
            
            if (pObj->vx < 0)
                pMob->spr.flipped = 0;
            else if (pObj->vx > 0)
                pMob->spr.flipped = 1;
        } break;
        case ID_BOSS_HEAD: {
            if (pMob->anim == BOSS_HEAD_HURT) {
                // Do nothing!
            }
            else if (pMob->anim == BOSS_HEAD_DEF && pMob->countdown <= 0) {
                bullet *pBul;
                GFraMe_ret rv;
                int cx, cy;
                
                // Get the mob's center
                cx = pMob->spr.obj.x + pMob->spr.obj.hitbox.cx;
                cy = pMob->spr.obj.y + pMob->spr.obj.hitbox.cy;
                
                // Shoot to the right
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx + 8, cy + 0);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot upward, toward the right
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx + 8, cy - 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot upward
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx + 0, cy - 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot upward, toward the left
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx - 8, cy - 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot to the left
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx - 8, cy + 0);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot downward, toward the left
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx - 8, cy + 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot downward
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx + 0, cy + 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // Shoot downward, toward the right
                pBul = 0;
                rv = rg_recycleBullet(&pBul);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = bullet_init(pBul, ID_ENEPROJ, cx, cy, cx + 8, cy + 8);
                ASSERT_NR(rv == GFraMe_ret_ok);
                
                mob_setAnim(pMob, BOSS_HEAD_ATTACK, 0);
            }
            else if (pMob->anim == BOSS_HEAD_ATTACK && mob_didAnimFinish(pMob)) {
                mob_setAnim(pMob, BOSS_HEAD_DEF, 0);
                pMob->countdown += BOSS_HEAD_COUNTDOWN;
            }
        } break;
        default: {
            if (pMob->spr.obj.hit & GFraMe_direction_down) {
                pMob->spr.obj.vy = 32;
            }
        }
    }
    
    GFraMe_sprite_update(&pMob->spr, ms);
    
__ret:
    return;
}

