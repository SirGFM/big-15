/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_object.h>
#ifdef DEBUG
#  include <GFraMe/GFraMe_pointer.h>
#endif
#include <GFraMe/GFraMe_save.h>
#include <GFraMe/GFraMe_util.h>

#ifdef DEBUG
#  include <SDL2/SDL_timer.h>
#endif

#include "audio.h"
#include "bullet.h"
#include "camera.h"
#include "collision.h"
#include "controller.h"
#include "global.h"
#include "map.h"
#include "options.h"
#include "player.h"
#include "playstate.h"
#include "registry.h"
#include "signal.h"
#include "textwindow.h"
#include "transition.h"
#include "types.h"
#include "ui.h"

#include "quadtree/quadtree.h"

#define PL_TWEEN_DELAY 1000

enum {OPT_CONT, OPT_RETRY, OPT_OPTIONS, OPT_MENU, OPT_EXIT, OPT_MAX};

// Initialize variables used by the event module
GFraMe_event_setup();

int switchState;
static int _ps_pause;
static int _ps_firstPress;
static int _ps_lastPress;
static int _ps_opt;
static int _ps_onOptions;
static int _timerTilCredits;
static int _psRunning;
static int _ps_text;

static char _ps_map001_text[] = 
"PRESS UP ON TERMINALS TO ACTIVATE\n"
"DOORS.\n"
"\n"
"PRESS ANY KEY/BUTTON TO CONTINUE...";

static char _ps_map_afterItem[] = 
"REMEMBER: YOU CAN ONLY SWITCH ITEMS\n"
"WHEN BOTH PLAYERS ARE CLOSE!\n"
"\n"
"PRESS ANY KEY/BUTTON TO CONTINUE...";

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
state playstate(int doLoad) {
    GFraMe_ret rv;
    state ret;
    
    ret = -1;
    rv = ps_init(doLoad);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init playstate", __ret);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    _ps_pause = 0;
    _psRunning = 1;
    while (gl_running && _psRunning) {
#ifdef DEBUG
        unsigned int t;
#endif
        
        ps_event();
        if (_ps_pause) {
            ps_doPause();
            if (_ps_onOptions) {
                options();
                _ps_onOptions = 0;
                _ps_lastPress = 300;
            }
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
    
    if (_timerTilCredits >= 5000)
        ret = CREDITS;
    else
        ret = MENUSTATE;
__ret:
    ps_clean();
    
    return ret;
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
    
    if (map >= 20) {
        audio_playBoss();
    }
    else if (map >= 15) {
        audio_playMovingOn();
    }
    else {
        audio_playIntro();
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
    
    _timerTilCredits = 0;
    _ps_onOptions = 0;
    _ps_text = 0;
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
        if (_ps_text) {
            textWnd_draw();
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
                
                if (map >= 20) {
                    audio_playBoss();
                }
                else if (map >= 15) {
                    audio_playMovingOn();
                }
                
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
                int map;
                map = gv_getValue(MAP);
                
                if (map == 1) {
                    ps_showText(_ps_map001_text, sizeof(_ps_map001_text), 0, 0, 40, 6);
                }
                else if (map == 8 || map == 13) {
                    ps_showText(_ps_map_afterItem, sizeof(_ps_map_afterItem), 0, 0, 40, 6);
                }
                
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
        
        if (gv_getValue(BOSS_ISDEAD) >= 4) {
            if (_timerTilCredits == 0) {
                audio_playVictory();
            }
            else if (_timerTilCredits > 5000) {
                _psRunning = 0;
            }
            _timerTilCredits += GFraMe_event_elapsed;
            if (_timerTilCredits >= 2000)
                return;
        }
        else if (gv_nIsZero(SWITCH_MAP)) {
            return;
        }
        else if (_ps_text) {
            textWnd_update(GFraMe_event_elapsed);
            return;
        }
        
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
        if (!player_isAlive(p2) && !player_isInsideMap(p2)) {
            GFraMe_ret rv;
            // Recover previous state
            rv = gv_load(SAVEFILE);
            GFraMe_assertRet(rv == GFraMe_ret_ok, "Error loading map", __err_ret);
            // Increase death counter
            gv_inc(PL2_DEATH);
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
        GFraMe_event_on_key_down();
            if (ctr_pause() && (!_ps_pause || !GFraMe_keys.enter)) {
                _ps_pause = !_ps_pause;
                _ps_firstPress = 0;
                _ps_opt = 0;
                _ps_onOptions = 0;
                _ps_lastPress = 300;
                GFraMe_audio_play(gl_aud_pause, 0.4f);
            }
            else if (_ps_text && textWnd_didFinish()) {
                _ps_text = 0;
            }
        GFraMe_event_on_key_up();
            _ps_firstPress = 0;
            _ps_lastPress = 0;
        GFraMe_event_on_controller();
            if (_ps_pause) {
                if (event.type == SDL_CONTROLLERBUTTONUP ||
                    (event.type == SDL_CONTROLLERAXISMOTION
                    && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY
                    && GFraMe_controllers[event.caxis.which].ly < 0.3
                    && GFraMe_controllers[event.caxis.which].ly > -0.3)
                   ) {
                    _ps_firstPress = 0;
                    _ps_lastPress = 0;
                }
            }
            if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                if (ctr_pause()) {
                    _ps_pause = !_ps_pause;
                    _ps_firstPress = 0;
                    _ps_opt = 0;
                    _ps_onOptions = 0;
                    _ps_lastPress = 300;
                    GFraMe_audio_play(gl_aud_pause, 0.4f);
                }
                else if (_ps_text && textWnd_didFinish()) {
                    _ps_text = 0;
                }
            }
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _op_renderText(char *text, int X, int Y, int l);

/**
 * Handle pause menu
 */
static void ps_doPause() {
    GFraMe_event_update_begin();
        if (_ps_lastPress > 0)
            _ps_lastPress -= GFraMe_event_elapsed;
        else {
            int isDown, isUp, isEnter;
            
            // Get key state
            isDown = GFraMe_keys.down;
            isDown = isDown || GFraMe_keys.s;
            if (GFraMe_controller_max >= 1) {
                isDown = isDown || GFraMe_controllers[0].ly > 0.5;
                isDown = isDown || GFraMe_controllers[0].down;
            }
            isUp = GFraMe_keys.up;
            isUp = isUp || GFraMe_keys.w;
            if (GFraMe_controller_max >= 1) {
                isUp = isUp || GFraMe_controllers[0].ly < -0.5;
                isUp = isUp || GFraMe_controllers[0].up;
            }
            
            if (isDown) {
                _ps_opt++;
                if (_ps_opt >= OPT_MAX) {
                    _ps_opt = 0;
                }
                if (!_ps_firstPress)
                    _ps_lastPress += 300;
                else
                    _ps_lastPress += 100;
                _ps_firstPress = 1;
                sfx_menuMove();
            }
            else if (isUp) {
                _ps_opt--;
                if (_ps_opt < 0)
                    _ps_opt = OPT_MAX - 1;
                if (!_ps_firstPress)
                    _ps_lastPress += 300;
                else
                    _ps_lastPress += 100;
                _ps_firstPress = 1;
                sfx_menuMove();
            }
            
            isEnter = GFraMe_keys.enter;
            isEnter = isEnter || GFraMe_keys.z;
            isEnter = isEnter || GFraMe_keys.space;
            if (GFraMe_controller_max > 0) {
                isEnter = isEnter || GFraMe_controllers[0].a;
            }
            
            if (isEnter) {
                switch (_ps_opt) {
                    case OPT_CONT: _ps_pause = 0; break;
                    case OPT_RETRY: {
                        gv_setValue(PL1_HP, 0);
                        gv_setValue(PL2_HP, 0);
                        _ps_pause = 0;
                    } break;
                    case OPT_OPTIONS: {
                        _ps_onOptions = 1;
                        _ps_lastPress = 500;
                    } break;
                    case OPT_MENU: _psRunning = 0; break;
                    case OPT_EXIT: gl_running = 0; break;
                }
            }
        }
    GFraMe_event_update_end();
}

/**
 * Draw pause menu
 */
static void ps_drawPause() {
    int x, y;
    
    // Draw the overlay
    transition_drawPause();
    // Render the text
    _op_renderText("--PAUSED--", 15, 8, sizeof("--PAUSED--")-1);
    
    x = 14;
    y = 16;
    
    _op_renderText("--", x-2, y+_ps_opt, 2);
    
    _op_renderText("CONTINUE", x, y, sizeof("CONTINUE")-1);
    y++;
    _op_renderText("RETRY", x, y, sizeof("RETRY")-1);
    y++;
    _op_renderText("OPTIONS", x, y, sizeof("OPTIONS")-1);
    y++;
    _op_renderText("QUIT TO MENU", x, y, sizeof("QUIT TO MENU")-1);
    y++;
    _op_renderText("EXIT GAME", x, y, sizeof("EXIT GAME")-1);
}

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _op_renderText(char *text, int X, int Y, int l) {
    int i, x, y;
    
    i = 0;
    x = X*8;
    y = Y*8;
    // Draw the text
    while (i < l) {
        char c;
        
        c = text[i];
        
        if (c == '\n') {
            x = X - 8;
            y += 8;
        }
        else if (c != ' ')
            GFraMe_spriteset_draw(gl_sset8x8, c-'!', x, y, 0/*flipped*/);
        
        x += 8;
        i++;
    }
}

/**
 * Set a text to be shown
 * 
 * @param text Text (the pointer will be copied)
 * @param textLen Text's length
 * @param x Window's horizontal position (in tiles)
 * @param y Window's vertical position (in tiles)
 * @param w Window's width (in tiles)
 * @param h Window's height (in tiles)
 */
void ps_showText(char *text, int textLen, int x, int y, int w, int h) {
    GFraMe_ret rv;
    GFraMe_save sv, *pSv;
    int hint;
    
    hint = 1;
    pSv = 0;
    // Open the configurations
    rv = GFraMe_save_bind(&sv, CONFFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error reading config file", __ret);
    pSv = &sv;
    // Check if the user disabled it
    GFraMe_save_read_int(&sv, "hint", &hint);
    
    if (hint) {
        textWnd_init(x, y, w, h, text, textLen);
        _ps_text = 1;
    }
__ret:
    if (pSv)
        GFraMe_save_close(pSv);
}

