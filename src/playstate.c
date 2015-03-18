/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>
#ifdef DEBUG
#  include <GFraMe/GFraMe_pointer.h>
#endif
#include <GFraMe/GFraMe_util.h>

#ifdef DEBUG
#  include <SDL2/SDL_timer.h>
#endif

#include "bullet.h"
#include "camera.h"
#include "collision.h"
#include "controller.h"
#include "global.h"
#include "map.h"
#include "player.h"
#include "playstate.h"
#include "registry.h"
#include "signal.h"
#include "transition.h"
#include "types.h"
#include "ui.h"

#include "quadtree/quadtree.h"

#define PL_TWEEN_DELAY 1000

// Initialize variables used by the event module
GFraMe_event_setup();

int switchState;
static int _ps_pause;

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init(int isLoading);
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
 * Handle pause menu
 */
static void ps_doPause();
/**
 * Draw pause menu
 */
static void ps_drawPause();
/**
 * Switch the current map
 */
static GFraMe_ret ps_switchMap();

#ifdef DEBUG
static int _updCalls;
static int _drwCalls;
static unsigned int _time;
static unsigned int _ltime;
#endif

/**
 * Playstate implementation. Must initialize it, run the loop and clean it up
 */
void playstate(int doLoad) {
    GFraMe_ret rv;
    
    gl_running = 0;
    rv = ps_init(doLoad);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init playstate", __ret);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    _ps_pause = 0;
    gl_running = 1;
    while (gl_running) {
#ifdef DEBUG
        unsigned int t;
#endif
        
        ps_event();
        if (_ps_pause) {
            ps_doPause();
        }
        else {
            if (gv_isZero(SWITCH_MAP))
                ps_update();
            else {
                rv = ps_switchMap();
                GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to switch maps",
                    __ret);
            }
        }
        ps_draw();
        
#ifdef DEBUG
        t = SDL_GetTicks();
        if (t >= _time) {
            GFraMe_log("t=%04i, U=%03i/%03i D=%03i/%03i", _time - _ltime,
                _updCalls, GAME_UFPS, _drwCalls, GAME_DFPS);
            _updCalls = 0;
            _drwCalls = 0;
            _ltime = _time;
            _time = SDL_GetTicks() + 1000;
        }
#endif
    }
    
__ret:
    ps_clean();
}

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init(int isLoading) {
    GFraMe_ret rv;
    int map, plX, plY;
    
    if (!isLoading) {
        gv_init();
        
        plX = 16;
        plY = 184;
        map = 0;
    }
    else {
        rv = gv_load(SAVEFILE);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load state", __ret);
        
        plX = gv_getValue(DOOR_X) * 8;
        plY = gv_getValue(DOOR_Y) * 8;
        map = gv_getValue(MAP);
    }

    rv = ui_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init ui", __ret);
    
    rv = rg_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to registry ui", __ret);
    
    rv = map_init(&m);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    
    rv = player_init(&p1, ID_PL1, 224, plX, plY);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init player", __ret);
    
    rv = player_init(&p2, ID_PL2, 240, plX, plY);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init player", __ret);
    
    rv = map_loadi(m, map);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);

    signal_init();
    
    switchState = 0;
    transition_initFadeOut();
    
#ifdef DEBUG
    _updCalls = 0;
    _drwCalls = 0;
    _time = 0;
    _ltime = 0;
#endif
    
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
    rg_clean();
    qt_clean();
}

/**
 * Draw the current frame
 */
static void ps_draw() {
    GFraMe_event_draw_begin();
#ifdef DEBUG
        _drwCalls++;
#endif
        map_draw(m);
        rg_drawBullets();
        rg_drawObjects();
        rg_drawMobs();
        if (gv_nIsZero(SWITCH_MAP))
            transition_draw();
        ui_draw();
        player_draw(p2);
        player_draw(p1);
        if (gv_isZero(SWITCH_MAP))
            signal_draw();
        #ifdef QT_DEBUG_DRAW
            if (GFraMe_keys.f1 ||
                (GFraMe_controller_max > 0 && GFraMe_controllers[0].l2))
                qt_drawRootDebug();
        #endif 
        if (_ps_pause) {
            ps_drawPause();
        }
    GFraMe_event_draw_end();
}

/**
 * Switch the current map
 */
static GFraMe_ret ps_switchMap() {
    GFraMe_ret rv;
#if !defined(DEBUG) || !defined(FAST_TRANSITION)
    GFraMe_event_update_begin();
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
                cam_setPositionSt(p1, p2);
                
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
            default: {
                gv_setValue(SWITCH_MAP, 0);
                switchState = 0;
                signal_release();
                
                // Set the update time (for using on events)
                gv_setValue(GAME_UPS, GFraMe_event_elapsed);
                // Save the current state
                rv = gv_save(SAVEFILE);
                GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving file!", __ret);
#  if defined(DEBUG) && defined(RESET_GV)
                gv_init();
#  endif /* RESET_GV */
            }
        }
        gl_running = tmp;
    GFraMe_event_update_end();
#else /* FAST_TRANSITION */
    int tmp, x, y;
    int map;
    
    // Store whether the game was running
    tmp = gl_running;
    // Make it stop on any error
    gl_running = 0;
    
    map = gv_getValue(MAP);
    
    rv = map_loadi(m, map);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Get their destiny position
    x = gv_getValue(DOOR_X) * 8;
    y = gv_getValue(DOOR_Y) * 8;
    // Tween the players
    rv = player_tweenTo(p1, x, y, GFraMe_event_elapsed, 0);
    rv = player_tweenTo(p2, x, y, GFraMe_event_elapsed, 0);
    // Update camera
    cam_setPositionSt(p1, p2);
    
    gv_setValue(SWITCH_MAP, 0);
#  ifdef RESET_GV
    gv_init();
#  endif /* RESET_GV */
        
    gl_running = tmp;
    // Set the update time (for using on events)
    gv_setValue(GAME_UPS, GFraMe_event_elapsed);
    // Save the current state
    rv = gv_save(SAVEFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving file!", __ret);
#endif /* FAST_TRANSITION */
    
    // Set return variable
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ps_update() {
    GFraMe_event_update_begin();
        GFraMe_object *pObj;
        GFraMe_ret rv;
        int  h, w;
        
#ifdef DEBUG
        _updCalls++;
#endif
        pObj = 0;
        
        // Check if any player should teleport
        player_checkTeleport(p1);
        player_checkTeleport(p2);
        
        // Update everything
        map_update(m, GFraMe_event_elapsed);
        rg_updateMobs(GFraMe_event_elapsed);
        rg_updateObjects(GFraMe_event_elapsed);
        rg_updateBullets(GFraMe_event_elapsed);
        player_update(p1, GFraMe_event_elapsed);
        player_update(p2, GFraMe_event_elapsed);
        ui_update(GFraMe_event_elapsed);
        signal_update(GFraMe_event_elapsed);
        
        // Collide everythin against everything else
        map_getDimensions(m, &w, &h);
        
        rv = qt_initCol(-8, -8, w + 16, h + 16);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error initializing collision",
            __err_ret);
        
        rv = rg_qtAddWalls();
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding map to collision",
            __err_ret);
        
        rv = rg_qtAddObjects();
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding object to quadtree",
            __err_ret);
        
        rv = rg_qtAddMob();
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding mob to quadtree",
            __err_ret);
        
        rv = rg_qtAddEvents();
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding events to quadtree",
            __err_ret);
        
        rv = rg_qtAddBullets();
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding bullets to quadtree",
            __err_ret);
        
        rv = qt_addPl(p1);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding player to quadtree",
            __err_ret);
        
        rv = qt_addPl(p2);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error adding player to quadtree",
            __err_ret);
        
        // Collide both players, manually
        col_onPlayer(p1, p2);
        col_onPlayer(p2, p1);
        
        // Collide the carried player (if any) against the map
        if (player_isBeingCarried(p1))
            player_getObject(&pObj, p1);
        else if (player_isBeingCarried(p2))
            player_getObject(&pObj, p2);
        // Fix a bug that would let players clip into ceilings
        if (pObj)
            rg_collideObjWall(pObj);
        
        // Update camera
        cam_setPosition();
        
        // If the player is trying to switch maps, do it
        if (player_cmpDestMap(p1, p2) == GFraMe_ret_ok) {
            gv_setValue(SWITCH_MAP, 1);
            return;
        }
        
        if (!player_isAlive(p1) && !player_isInsideMap(p1)) {
            GFraMe_ret rv;
            
            // Recover previous state
            rv = gv_load(SAVEFILE);
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Error loading map", __err_ret);
            // Increase death counter
            gv_inc(PL1_DEATH);
            // Save death counter
            rv = gv_save(SAVEFILE);
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving map", __err_ret);
            // Force reload
            gv_setValue(SWITCH_MAP, 1);
        }
        else if (!player_isAlive(p2) && !player_isInsideMap(p2)) {
            GFraMe_ret rv;
            // Recover previous state
            rv = gv_load(SAVEFILE);
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Error loading map", __err_ret);
            // Increase death counter
            gv_inc(PL1_DEATH);
            // Save death counter
            rv = gv_save(SAVEFILE);
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving map", __err_ret);
            // Force reload
            gv_setValue(SWITCH_MAP, 1);
        }
    GFraMe_event_update_end();
    
    return;
__err_ret:
    gl_running = 0;
    return;
}

/**
 * Handle every event
 */
static void ps_event() {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
#ifdef DEBUG
        GFraMe_event_on_mouse_up();
        GFraMe_event_on_mouse_down();
        GFraMe_event_on_mouse_moved();
#endif
//        GFraMe_event_on_finger_down();
//        GFraMe_event_on_finger_up();
//        GFraMe_event_on_bg();
//        GFraMe_event_on_fg();
        GFraMe_event_on_key_down();
            if (GFraMe_keys.esc)
                gl_running = 0;
            if (ctr_pause())
                _ps_pause = !_ps_pause;
        GFraMe_event_on_key_up();
        GFraMe_event_on_controller();
            if (GFraMe_controller_max > 0 && GFraMe_controllers[0].home)
                gl_running = 0;
            if (event.type == SDL_CONTROLLERBUTTONDOWN && ctr_pause())
                _ps_pause = !_ps_pause;
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

/**
 * Handle pause menu
 */
static void ps_doPause() {
    GFraMe_event_update_begin();
    GFraMe_event_update_end();
}

/**
 * Draw pause menu
 */
static void ps_drawPause() {
    char text[] = "--PAUSED--";
    int i, x, y;
    
    // Draw the overlay
    transition_drawPause();
    // Get the text tiles
	GFraMe_str2tiles(text, text, 0);
    // Render the text
    i = 0;
    x = (SCR_W - sizeof(text)*8) / 2;
    y = 64;
    while (i < sizeof(text) - 1) {
        GFraMe_spriteset_draw(gl_sset8x8, text[i], x, y, 0/*flip*/);
        i++;
        x += 8;
    }
}

