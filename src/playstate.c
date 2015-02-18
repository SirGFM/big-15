/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_util.h>

#include "camera.h"
#include "global.h"
#include "map.h"
#include "player.h"
#include "playstate.h"
#include "transition.h"
#include "types.h"
#include "ui.h"

#define PL_TWEEN_DELAY 1000

// Initialize variables used by the event module
GFraMe_event_setup();

map *m;     /** Game map */
player *p1; /** First player */
player *p2; /** Second player */

int switchState;

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init();
/**
 * Clean up the playstate
 */
static void ps_clean();
/**
 * Draw the current frame
 */
static void ps_draw();
/**
 * Update the current frame, as many times as it's accumulated
 */
static void ps_update();
/**
 * Handle every event
 */
static void ps_event();
/**
 * Switch the current map
 */
static GFraMe_ret ps_switchMap();

/**
 * Playstate implementation. Must initialize it, run the loop and clean it up
 */
void playstate() {
    GFraMe_ret rv;
    
    gl_running = 0;
    rv = ps_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init playstate", __ret);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    gl_running = 1;
    while (gl_running) {
        ps_event();
        if (gv_isZero(SWITCH_MAP))
            ps_update();
        else {
            rv = ps_switchMap();
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to switch maps",
                __ret);
        }
        ps_draw();
    }
    
__ret:
    ps_clean();
}

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init() {
    GFraMe_ret rv;
    
    rv = ui_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init ui", __ret);
    
    rv = map_init(&m);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    
    rv = player_init(&p1, ID_PL1, 224);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init player", __ret);
    
    rv = player_init(&p2, ID_PL2, 240);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init player", __ret);
    
    rv = map_loadi(m, 0);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    
    switchState = 0;
    transition_initFadeOut();
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up the playstate
 */
static void ps_clean() {
    ui_clean();
    map_clean(&m);
    player_clean(&p1);
    player_clean(&p2);
}

/**
 * Draw the current frame
 */
static void ps_draw() {
    GFraMe_event_draw_begin();
        map_draw(m);
        if (gv_isZero(SWITCH_MAP)) {
            player_draw(p2);
            player_draw(p1);
            map_drawObjs(m);
            ui_draw();
        }
        else {
            map_drawObjs(m);
            ui_draw();
            transition_draw();
            player_draw(p2);
            player_draw(p1);
        }
    GFraMe_event_draw_end();
}

/**
 * Switch the current map
 */
static GFraMe_ret ps_switchMap() {
    GFraMe_ret rv;
    int tmp;
    
    // Store whether the game was running
    tmp = gl_running;
    // Make it stop on any error
    gl_running = 0;
    
    switch (switchState) {
        /** Simply start the transition */
        case 0: transition_initFadeOut(); switchState++; break;
        /** Fade out */
        case 1: {
            if (transition_fadeOut(GFraMe_event_elapsed) == TR_COMPLETE)
                switchState++;
        } break;
        /** Load the map */
        case 2: {
            int map;
            map = gv_getValue(MAP);
            
            rv = map_loadi(m, map);
            ASSERT(rv == GFraMe_ret_ok, rv);
            
            switchState++;
        } break;
        /** Tween players to their new position */
        case 3: {
            int x, y;
            
            // Get their destiny position
            x = gv_getValue(DOOR_X) * 8;
            y = gv_getValue(DOOR_Y) * 8;
            // Tween the players
            rv = player_tweenTo(p1, x, y, GFraMe_event_elapsed, PL_TWEEN_DELAY);
            rv = player_tweenTo(p2, x, y, GFraMe_event_elapsed, PL_TWEEN_DELAY);
            // Update camera
            cam_setPosition(p1, p2);
            
            if (rv == GFraMe_ret_ok)
                switchState++;
        } break;
        /** Init fade in animation */
        case 4: transition_initFadeIn(); switchState++; break;
        /** Fade in */
        case 5: {
            if (transition_fadeIn(GFraMe_event_elapsed) == TR_COMPLETE)
                switchState++;
        } break;
        /** Finish the transition */
        default:
            gv_setValue(SWITCH_MAP, 0);
            switchState = 0;
    }
    
    // Set return variable
    gl_running = tmp;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ps_update() {
    GFraMe_event_update_begin();
        GFraMe_object *pWalls, *pPlObj1, *pPlObj2, *pObj;
        GFraMe_sprite *pSpr;
        int len, i;
        
        pObj = 0;
        
        // Update everything
        map_update(m, GFraMe_event_elapsed);
        player_update(p1, GFraMe_event_elapsed);
        player_update(p2, GFraMe_event_elapsed);
        ui_update(GFraMe_event_elapsed);
        
        // Collide every entity with the environment
        player_getObject(&pPlObj1, p1);
        player_getObject(&pPlObj2, p2);
        map_getWalls(&pWalls, &len, m);
        i = 0;
        while (i < len) {
            GFraMe_object_overlap(&pWalls[i], pPlObj1, GFraMe_first_fixed);
            GFraMe_object_overlap(&pWalls[i], pPlObj2, GFraMe_first_fixed);
            
            i++;
        }
        
        // Collide both players
        {
            GFraMe_ret rv;
            int wasPl1Down, wasPl2Down, pl1Flags, pl2Flags;
            
            pl1Flags = pPlObj1->hit;
            pl2Flags = pPlObj2->hit;
            
            pPlObj1->hit = 0;
            pPlObj2->hit = 0;
            
            wasPl1Down = (pl1Flags & GFraMe_direction_down);
            wasPl2Down = (pl2Flags & GFraMe_direction_down);
            
            rv = GFraMe_object_overlap(pPlObj1, pPlObj2, GFraMe_dont_collide);
            if (rv == GFraMe_ret_ok) {
                // If any player wasn't touching down but is now, then it's
                // above the other one
                if (pPlObj1->y == pPlObj2->y) {} // Do nothing if they are side-by-side
                else if ((pl1Flags & GFraMe_direction_up)
                    || (pl2Flags & GFraMe_direction_up)) {}
                else if (!wasPl1Down && (pPlObj1->hit&GFraMe_direction_down)){
                    player_getCarried(p1, pPlObj2);
                    pl1Flags |= GFraMe_direction_down;
                    pObj = pPlObj1;
                }
                else if (!wasPl2Down && (pPlObj2->hit&GFraMe_direction_down)){
                    player_getCarried(p2, pPlObj1);
                    pl2Flags |= GFraMe_direction_down;
                    pObj = pPlObj2;
                }
            }
            
            pPlObj1->hit = pl1Flags;
            pPlObj2->hit = pl2Flags;
        }
        
        // Fix for a stupid bug
        if (pObj) {
            i = 0;
            while (i < len) {
                GFraMe_object_overlap(&pWalls[i], pObj, GFraMe_first_fixed);
                i++;
            }
        }
        
        // Collide against every object on the map
        map_collideObjects(m, pPlObj1);
        map_collideObjects(m, pPlObj2);
        
        // Check if any event was triggered
        player_getSprite(&pSpr, p1);
        map_checkEvents(m, pSpr);
        player_getSprite(&pSpr, p2);
        map_checkEvents(m, pSpr);
        
        // Update camera
        cam_setPosition(p1, p2);
        
        // If the player is trying to switch maps, do it
        if (player_cmpDestMap(p1, p2) == GFraMe_ret_ok) {
            gv_setValue(SWITCH_MAP, 1);
            return;
        }
    GFraMe_event_update_end();
}

/**
 * Handle every event
 */
static void ps_event() {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
//        GFraMe_event_on_mouse_up();
//        GFraMe_event_on_mouse_down();
//        GFraMe_event_on_mouse_moved();
//        GFraMe_event_on_finger_down();
//        GFraMe_event_on_finger_up();
//        GFraMe_event_on_bg();
//        GFraMe_event_on_fg();
        GFraMe_event_on_key_down();
            if (GFraMe_keys.esc)
                gl_running = 0;
        GFraMe_event_on_key_up();
        GFraMe_event_on_controller();
            if (GFraMe_controller_max > 0 && GFraMe_controllers[0].home)
                gl_running = 0;
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

