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
#include "save.h"
#include "signal.h"
#include "state.h"
#include "textwindow.h"
#include "timer.h"
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
static int _ps_justRetry;
static int _ps_firstPress;
static int _ps_lastPress;
static int _ps_opt;
static int _ps_onOptions;
static int _timerTilCredits;
static int _psRunning;
static int _ps_text;
static int _maxUfps;
static int _maxDfps;
static int _ps_isSpeedrun;

struct stGame {
    struct stateHandler hnd;
    playstateCmd cmd;
    jjatError err;
    GFraMe_ret lastRv;
};

static char _ps_map001_textPT[] =
 "PRESSIONE PARA CIMA EM TERMINAIS\n"
 "PARA ATIVAR PORTAS\n"
 "\n"
 "PRESSIONE QUALQUER TECLA/BOTAO...";
static char _ps_map001_textEN[] =
 "PRESS UP ON TERMINALS TO ACTIVATE\n"
 "DOORS.\n"
 "\n"
 "PRESS ANY KEY/BUTTON TO CONTINUE...";

static char _ps_map_afterItemPT[] =
 "NOTA: AMBOS OS JOGADORES DEVEM ESTAR\n"
 "PROXIMOS PARA TROCAR DE ITEM\n"
 "\n"
 "PRESSIONE QUALQUER TECLA/BOTAO...";
static char _ps_map_afterItemEN[] = 
 "REMEMBER: YOU CAN ONLY SWITCH ITEMS\n"
 "WHEN BOTH PLAYERS ARE CLOSE!\n"
 "\n"
 "PRESS ANY KEY/BUTTON TO CONTINUE...";

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init(playstateCmd cmd);
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

int playstate_setup(void *self) {
    struct stGame *ps = (struct stGame*)self;
    GFraMe_ret rv;

    rv = ps_init(ps->cmd);
    if (rv == GFraMe_ret_ok) {
        GFraMe_event_init(_maxUfps, _maxDfps);

        _ps_pause = 0;
        _ps_justRetry = 0;
        _psRunning = 1;
    }

    return rv;
}

int playstate_isRunning(void *self) {
    struct stGame *ps = (struct stGame*)self;

    return _psRunning && !_ps_onOptions && ps->err == JERR_NONE;
}

void playstate_update(void *self) {
    struct stGame *ps = (struct stGame*)self;
#ifdef DEBUG
    unsigned int t;
#endif

    ps_event();
    timer_update();
    if (_ps_pause)
        ps_doPause();
    else {
        if (gv_isZero(SWITCH_MAP))
            ps_update();
        else {
            ps->lastRv = ps_switchMap();
            if (ps->lastRv != GFraMe_ret_ok) {
                ps->err = JERR_LOAD_MAP;
                return;
            }
        }
    }
    ps_draw();

#ifdef DEBUG
    t = SDL_GetTicks();
    if (t >= _time) {
        GFraMe_log("t=%04i, U=%03i/%03i D=%03i/%03i", _time - _ltime,
            _updCalls, _maxUfps, _drwCalls, _maxDfps);
        _updCalls = 0;
        _drwCalls = 0;
        _ltime = _time;
        _time = SDL_GetTicks() + 1000;
    }
#endif
}

int playstate_nextState(void *self) {
    struct stGame *ps = (struct stGame*)self;

    if (ps->err != JERR_NONE)
        return ERRORSTATE;
    else if (_ps_onOptions) {
        _ps_onOptions = 0;
        _ps_lastPress = 300;
        return OPTIONS;
    }
    else if (_timerTilCredits >= 5000)
        return CREDITS;
    else
        return MENUSTATE;
}

void playstate_release(void *self) {
    ps_clean();
}

int playstate_getExitError(void *self) {
    struct stGame *ps = (struct stGame*)self;

    return (int)ps->err;
}

static struct stGame global_ps;
void *playstate_getHnd(playstateCmd cmd) {
    struct stateHandler *hnd = &(global_ps.hnd);

    memset(&global_ps, 0x0, sizeof(global_ps));
    hnd->setup = &playstate_setup;
    hnd->isRunning = &playstate_isRunning;
    hnd->update = &playstate_update;
    hnd->nextState = &playstate_nextState;
    hnd->release = &playstate_release;
    hnd->getExitError = &playstate_getExitError;
    global_ps.cmd = cmd;

    return &global_ps;
}

int isPlaystate(void *hnd) {
    return hnd == &global_ps;
}

/**
 * Initialize the playstate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ps_init(playstateCmd cmd) {
    GFraMe_ret rv;
    int map, plX, plY, time;
    
    // Read the desired fps (for update and drawing)
    _maxUfps = read_slot(BLK_CONFIG, sv_ufps);
    if (_maxUfps == -1)
        _maxUfps = GAME_UFPS;
    _maxDfps = read_slot(BLK_CONFIG, sv_dfps);
    if (_maxDfps == -1)
        _maxDfps = GAME_DFPS;
    _ps_isSpeedrun = read_slot(BLK_CONFIG, sv_speedrun);
    if (_ps_isSpeedrun == -1)
        _ps_isSpeedrun = 0;
    
    if (cmd != CONTINUE) {
        gv_init();
        
        plX = 16;
        plY = 184;
        if (cmd == MT_VERSION) {
            map = 21;
        }
        else /* if (cmd == NEWGAME) */ {
            map = 0;
        }
        gv_setValue(DOOR_X, 16 / 8);
        gv_setValue(DOOR_Y, 184 / 8);
        gv_setValue(MAP, map);
    }
    else {
        rv = gv_load(SAVEFILE);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load state", __ret);
        
        plX = gv_getValue(DOOR_X) * 8;
        plY = gv_getValue(DOOR_Y) * 8;
        map = gv_getValue(MAP);
    }
    time = gv_getValue(GAME_TIME);
    timer_init(time);
    
    if ((map % 21) >= 20) {
        audio_playBoss();
    }
    else if ((map % 21) >= 15) {
        audio_playTensionGoesUp();
    }
    else if ((map % 21) >= 4) {
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
        if (_ps_isSpeedrun)
            timer_draw();
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
                rg_updateObjects(0);
                
                if ((map % 21) >= 20) {
                    audio_playBoss();
                }
                else if ((map % 21) >= 15) {
                    audio_playTensionGoesUp();
                }
                else if ((map % 21) >= 4) {
                    audio_playMovingOn();
                }
                else {
                    audio_playIntro();
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
                    if (gl_lang == EN_US) {
                        ps_showText(_ps_map001_textEN, sizeof(_ps_map001_textEN), 0, 0, 40, 6);
                    }
                    else if (gl_lang == PT_BR) {
                        ps_showText(_ps_map001_textPT, sizeof(_ps_map001_textPT), 0, 0, 40, 6);
                    }
                }
                else if (map == 8 || map == 13) {
                    if (gl_lang == EN_US) {
                        ps_showText(_ps_map_afterItemEN, sizeof(_ps_map_afterItemEN), 0, 0, 40, 6);
                    }
                    else if (gl_lang == PT_BR) {
                        ps_showText(_ps_map_afterItemPT, sizeof(_ps_map_afterItemPT), 0, 0, 40, 6);
                    }
                }
                
                gv_setValue(SWITCH_MAP, 0);
                switchState = 0;
                signal_release();
                
                if (_ps_justRetry) {
                   player_resetVerticalSpeed(p1);
                   player_resetVerticalSpeed(p2);
                   _ps_justRetry = 0;
                }
                if (!_ps_isSpeedrun) {
                    // If speedrun mode is enabled, this is skipped. By doing that, the
                    // teleport target is loaded from the previous saved state (i.e.,
                    // it become the last position teleported to).
                    // This only works on the first frame after loading a level, though.
                    player_resetTeleport(p1);
                    player_resetTeleport(p2);
                }
                
                // Set the update time (for using on events)
                gv_setValue(GAME_UPS, GFraMe_event_elapsed);
                // Save the current state
                if (player_isAlive(p1) && player_isAlive(p2)) {
                    gv_setValue(GAME_TIME, timer_getTime());
                    rv = gv_save(SAVEFILE);
                    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving file!", __ret);
                }
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
    if (player_isAlive(p1) && player_isAlive(p2)) {
        gv_setValue(GAME_TIME, timer_getTime());
        rv = gv_save(SAVEFILE);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving file!", __ret);
    }
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
                timer_stop();
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
        
        do {
            int didDie = 0;

            // Check if any of the players died/is OOB
            if (!player_isAlive(p1) && !player_isInsideMap(p1)) {
                didDie = 1;
            }
            else if (!player_isInsideMap(p1)) {
               // P1 is OOB... Heck yeah, great strat!
               player_resetVerticalSpeed(p1);
            }
            if (!player_isAlive(p2) && !player_isInsideMap(p2)) {
                didDie |= 2;
            }
            else if (!player_isInsideMap(p2)) {
               // P2 is OOB... Heck yeah, great strat!
               player_resetVerticalSpeed(p2);
            }

            if (didDie != 0) {
                GFraMe_ret rv;

                // Recover previous state. Ignore errors if on the first map.
                rv = gv_load(SAVEFILE);

                // Increase death counter
                if (didDie & 1) {
                    gv_inc(PL1_DEATH);
                }
                if (didDie & 2) {
                    gv_inc(PL2_DEATH);
                }

                if (rv != GFraMe_ret_ok && gv_getValue(MAP) == 0) {
                    int death1, death2;

                    death1 = gv_getValue(PL1_DEATH);
                    death2 = gv_getValue(PL2_DEATH);

                    // Reset the variables (since it's most likely a buggy situation)
                    gv_init();
                    gv_setValue(DOOR_X, 16 / 8);
                    gv_setValue(DOOR_Y, 184 / 8);
                    gv_setValue(MAP, 0);
                    gv_setValue(PL1_DEATH, death1);
                    gv_setValue(PL2_DEATH, death2);
                }
                else {
                    // Save death counter and timer
                    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error loading map", __err_ret);
                    gv_setValue(GAME_TIME, timer_getTime());
                    rv = gv_save(SAVEFILE);
                    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error saving map", __err_ret);
                }

                // Force reload
                gv_setValue(SWITCH_MAP, 1);
            }
        } while (0);
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
                sfx_pause();
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
                    sfx_pause();
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
            if (GFraMe_controller_max >= 2) {
                isDown = isDown || GFraMe_controllers[1].ly > 0.5;
                isDown = isDown || GFraMe_controllers[1].down;
            }
            isUp = GFraMe_keys.up;
            isUp = isUp || GFraMe_keys.w;
            if (GFraMe_controller_max >= 1) {
                isUp = isUp || GFraMe_controllers[0].ly < -0.5;
                isUp = isUp || GFraMe_controllers[0].up;
            }
            if (GFraMe_controller_max >= 2) {
                isUp = isUp || GFraMe_controllers[1].ly < -0.5;
                isUp = isUp || GFraMe_controllers[1].up;
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
            if (GFraMe_controller_max > 1) {
                isEnter = isEnter || GFraMe_controllers[1].a;
            }
            
            if (isEnter) {
                switch (_ps_opt) {
                    case OPT_CONT: _ps_pause = 0; break;
                    case OPT_RETRY: {
                        gv_setValue(PL1_HP, 0);
                        gv_setValue(PL2_HP, 0);
                        _ps_pause = 0;
                        _ps_justRetry = 1;
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
    
#define selectLang(textEN, textPT) \
  do { \
    if (gl_lang == EN_US) { _op_renderText(textEN, x, y, sizeof(textEN)-1); } \
    else if (gl_lang == PT_BR) { _op_renderText(textPT, x, y, sizeof(textPT)-1); } \
  } while (0)
    
    // Draw the overlay
    transition_drawPause();
    // Render the text
    x = 14;
    y = 6;
    selectLang(" --PAUSED--", "--PAUSADO--");
    
    x = 14;
    y = 10;
    
    _op_renderText("--", x-2, y+_ps_opt, 2);
    
    selectLang("CONTINUE", "CONTINUAR");
    y++;
    selectLang("RETRY", "REINICIAR MAPA");
    y++;
    selectLang("OPTIONS", "OPCOES");
    y++;
    selectLang("QUIT TO MENU", "VOLTAR AO MENU");
    y++;
    selectLang("EXIT GAME", "SAIR DO JOGO");
    
#undef selectLang
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
    y = Y * 10;
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
    int hint;
    
    // Check if the user disabled it
    hint = read_slot(BLK_CONFIG, sv_hint);
    if (hint == -1 || hint) {
        textWnd_init(x, y, w, h, text, textLen);
        _ps_text = 1;
    }
}

int playstate_getGfmError(void *self) {
    struct stGame *ps = (struct stGame*)self;
    return ps->lastRv;
}
