/**
 * @file src/menustate.c
 * 
 * Menu state
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_save.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <string.h>

#include "audio.h"
#include "camera.h"
#include "commonEvent.h"
#include "controller.h"
#include "global.h"
#include "globalVar.h"
#include "map.h"
#include "object.h"
#include "state.h"
#include "transition.h"
#include "types.h"

#define GFM_ICON 34
#define TIME_TO_DEMO 5000
#define J1_VY 64
#define J2_VY 64
#define  A_VY 64
#define  T_VY 64
#define J_TILE 13
#define A_TILE 14
#define T_TILE 15

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param i Initial index
 * @param l Text length
 */
static void _ms_renderText(char *text, int X, int Y, int i, int l);

// Initialize variables used by the event module
GFraMe_event_setup();

enum {OPT_CONTINUE, OPT_NEWGAME, OPT_MTVERSION, OPT_OPTIONS, OPT_QUIT, OPT_MAX};
struct stMenustate {
    struct stateHandler hnd;
    /** Whether there's already a saved game */
    int hasSave;
    /** For how long the last key has been pressed */
    int lastPressedTime;
    /** Whether a key was being pressed */
    int firstPress;
    /** Current option */
    int curOpt;
    /** Initial text x position */
    int textX;
    /** Initial text y position */
    int textY;
    /** Dev icon's x position */
    int iconX;
    /** Dev icon's y position */
    int iconY;
    /** Whether the title finished setting up */
    int isTitleSet;
    /** Letter j x position */
    int j1X;
    float j1Xf;
    /** Letter j y position */
    int j1Y;
    float j1Yf;
    /** Letter j x position */
    int j2X;
    float j2Xf;
    /** Letter j y position */
    int j2Y;
    float j2Yf;
    /** Letter a x position */
    int aX;
    float aXf;
    /** Letter a y position */
    int aY;
    float aYf;
    /** Letter t x position */
    int tX;
    float tXf;
    /** Letter t y position */
    int tY;
    float tYf;
    /** Whether the menu is running */
    int runMenu;
    /** How long the menu has been idle */
    int idleTime;
    /** BG map */
    map *pM;
    /** The small 'heartup' terminal */
    object *pO;
};

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ms_init(struct stMenustate *ms);
/**
 * Clean up the menustate
 */
static void ms_clean(struct stMenustate *ms);
/**
 * Draw the current frame
 */
static void ms_draw(struct stMenustate *ms);
/**
 * Update the current frame, as many times as it's accumulated
 */
static void ms_update(struct stMenustate *ms);
/**
 * Handle every event
 */
static void ms_event(struct stMenustate *ms);

int menustate_setup(void *self) {
    GFraMe_ret rv;

    struct stMenustate *ms = (struct stMenustate*)self;

    rv = ms_init(ms);
    if (rv == GFraMe_ret_ok)
        GFraMe_event_init(GAME_UFPS, GAME_DFPS);

    return rv;
}

int menustate_isRunning(void *self) {
    struct stMenustate *ms = (struct stMenustate*)self;

    return ms->runMenu;
}

void menustate_update(void *self) {
    struct stMenustate *ms = (struct stMenustate*)self;

    ms_event(ms);
    ms_update(ms);
    ms_draw(ms);
}

int menustate_nextState(void *self) {
    struct stMenustate *ms = (struct stMenustate*)self;

    if (ms->curOpt == OPT_CONTINUE)
        return CNT_PLAYSTATE;
    else if (ms->curOpt == OPT_NEWGAME)
        return NEW_PLAYSTATE;
    else if (ms->curOpt == OPT_MTVERSION)
        return MT_PLAYSTATE;
    else if (ms->curOpt == OPT_OPTIONS)
        return OPTIONS;
    else if (ms->curOpt == OPT_MAX)
        return DEMO;
    return (state)-1;
}

void menustate_release(void *self) {
    struct stMenustate *ms = (struct stMenustate*)self;

    ms_clean(ms);
}

int menustate_getExitError(void *self) {
    return 0;
}

static struct stMenustate global_ms;
void *menustate_getHnd() {
    struct stateHandler *hnd = &(global_ms.hnd);

    memset(&global_ms, 0x0, sizeof(global_ms));
    hnd->setup = &menustate_setup;
    hnd->isRunning = &menustate_isRunning;
    hnd->update = &menustate_update;
    hnd->nextState = &menustate_nextState;
    hnd->release = &menustate_release;
    hnd->getExitError = &menustate_getExitError;

    return &global_ms;
}

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ms_init(struct stMenustate *ms) {
    GFraMe_ret rv;
    GFraMe_save sv;
    int tmp, ctrPl1, ctrPl2;
    
    ms->pO = 0;
    ms->pM = 0;
    // Check if there's already a saved game
    rv = GFraMe_save_bind(&sv, SAVEFILE);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Check if something other than the version is written
    if (sv.size > 50)
        ms->hasSave = 1;
    else
        ms->hasSave = 0;
    GFraMe_save_close(&sv);
    
    // Check the configurations
    rv = GFraMe_save_bind(&sv, CONFFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error reading config file", __ret);
    // Try to read both players control mode
    ctrPl1 = -1;
    ctrPl2 = -1;
    rv = GFraMe_save_read_int(&sv, "ctr_pl1", &tmp);
    if (rv == GFraMe_ret_ok)
        ctrPl1 = tmp;
    rv = GFraMe_save_read_int(&sv, "ctr_pl2", &tmp);
    if (rv == GFraMe_ret_ok)
        ctrPl2 = tmp;
    // set the control scheme
    if (ctrPl1 != -1 && ctrPl2 != -1) {
        tmp = ctr_setModeForce(ID_PL1, ctrPl1);
        if (tmp)
            tmp = ctr_setModeForce(ID_PL2, ctrPl2);
        // In case any error happened, set the default
        if (!tmp)
            ctr_setDef();
    }
    else
        ctr_setDef();
    // Set the song volume
    rv = GFraMe_save_read_int(&sv, "music", &tmp);
    if (rv == GFraMe_ret_ok)
        audio_setVolume(tmp);
    else
        audio_setVolume(60);
    // Set the sfx volume
    rv = GFraMe_save_read_int(&sv, "sfx", &tmp);
    if (rv == GFraMe_ret_ok)
        sfx_setVolume(tmp);
    else
        sfx_setVolume(50);
    GFraMe_save_close(&sv);
    
    // Zero some variables
    ms->lastPressedTime = 0;
    ms->firstPress = 0;
    // Set the selected option
    if (ms->hasSave)
        ms->curOpt = 0;
    else
        ms->curOpt = 1;
    // Set text position
    ms->textX = 112;
    ms->textY = 176;
    // Set the dev icon position
    ms->iconX = 35 * 8;
    ms->iconY = 25 * 8;
    // Start the menu
    ms->runMenu = 1;
    // Set the title position
    ms->isTitleSet = 0;
    ms->j1X = 82;
    ms->j1Yf = -32.0f;
    ms->j1Y = -32;
    ms->j2X = 122;
    ms->j2Yf = -40.0f;
    ms->j2Y = -40;
    ms->aX = 154;
    ms->aYf = -48.0f;
    ms->aY = -48;
    ms->tX = 194;
    ms->tYf = -54.0f;
    ms->tY = -54;
    ms->idleTime = 0;
    
    rv = map_init(&ms->pM);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load background", __ret);
    rv = map_loadf(ms->pM, "maps/mainmenu.gfm");
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load background", __ret);
    
    rv = obj_getNew(&ms->pO);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load background", __ret);
    obj_setZero(ms->pO);
    obj_setBounds(ms->pO, 8*8, 17*8, 2*8, 2*8);
    obj_setID(ms->pO, ID_HEARTUP);
    obj_setCommonEvent(ms->pO, CE_NONE);
    
    cam_x = 0;
    cam_y = 0;
    cam_setMapDimension(40, 30);
    
    audio_playMenu();
    
    rv = GFraMe_ret_ok;
__ret:
    
    return rv;
}

/**
 * Clean up the menustate
 */
static void ms_clean(struct stMenustate *ms) {
    if (ms->pM)
        map_clean(&ms->pM);
    if (ms->pO)
        obj_clean(&ms->pO);
}

/**
 * Draw the current frame
 */
static void ms_draw(struct stMenustate *ms) {
    char optionsEN[] = "  CONTINUE  \n"
                       "  NEW GAME  \n"
                       "  MT MODE   \n"
                       "  OPTIONS   \n"
                       "  QUIT";
    char optionsPT[] = "  CONTINUAR \n"
                       "  NOVO JOGO \n"
                       "  VERSAO MT \n"
                       "  OPCOES    \n"
                       "  SAIR";
    char devTextEN[]   = "A GAME BY";
    char devTextPT[]   = "UM JOGO DE";
    char twitterText[] = "@SIRGFM";
    char *options, *devText;
    
    if (gl_lang == PT_BR) {
        options = optionsPT;
        devText = devTextPT;
    }
    else /* if (gl_lang == EN_US) */ {
        options = optionsEN;
        devText = devTextEN;
    }
    
    GFraMe_event_draw_begin();
        if (ms->isTitleSet) {
            int i, l;
            
            map_draw(ms->pM);
            // Put the 'selected' mark
            if (ms->curOpt < OPT_MAX) {
                i = 13 * ms->curOpt;
                options[i] = '-'; options[i+1] = '-';
            }
            // Render texts
            l = strlen(options) + 1;
            if (!ms->hasSave) {
                _ms_renderText(options, ms->textX, ms->textY + 8, 13/*i*/, l);
            }
            else {
                _ms_renderText(options, ms->textX, ms->textY, 0/*i*/, l);
            }
            l = strlen(devText) + 1;
            _ms_renderText(devText, ms->iconX - l*8, ms->iconY + 16, 0, l);
            l = strlen(twitterText) + 1;
            _ms_renderText(twitterText, ms->iconX - l*8, ms->iconY + 24, 0, l);
            // Render the dev icon
            GFraMe_spriteset_draw(gl_sset32x32, GFM_ICON, ms->iconX, ms->iconY, 0);
            // Draw both characters
            GFraMe_spriteset_draw(gl_sset32x32, 20, 18*8+4, 11*8-13, 0);
            // Draw the 'heartup' terminal
            obj_draw(ms->pO);
        }
        
        // Draw the title
        GFraMe_spriteset_draw(gl_sset64x32, J_TILE, ms->j1X, ms->j1Y, 0);
        GFraMe_spriteset_draw(gl_sset64x32, J_TILE, ms->j2X, ms->j2Y, 0);
        GFraMe_spriteset_draw(gl_sset64x32, A_TILE, ms->aX, ms->aY, 0);
        GFraMe_spriteset_draw(gl_sset64x32, T_TILE, ms->tX, ms->tY, 0);
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ms_update(struct stMenustate *ms) {
    GFraMe_event_update_begin();
        int isEnter;
        
        if (!ms->isTitleSet) {
            float t;
            
            t = GFraMe_event_elapsed / 1000.0f;
            #define UPD_LETTER(L, LF, V) \
                if (L < 32) { \
                    LF += t * V; \
                    L = (int)LF; \
                } \
                else if (L >= 32) { \
                    LF = 32.0f; \
                    L = 32; \
                }
            UPD_LETTER(ms->j1Y, ms->j1Yf, J1_VY)
            UPD_LETTER(ms->j2Y, ms->j2Yf, J2_VY)
            UPD_LETTER(ms->aY, ms->aYf, A_VY)
            UPD_LETTER(ms->tY, ms->tYf, T_VY)
            #undef UPD_LETTER
            if (ms->j1Y == 32 && ms->j2Y == 32 && ms->aY == 32 && ms->tY == 32) {
                ms->isTitleSet =1;
            }
        }
        else {
            map_update(ms->pM, GFraMe_event_elapsed);
            obj_update(ms->pO, GFraMe_event_elapsed);
            if (ms->lastPressedTime > 0)
                ms->lastPressedTime -= GFraMe_event_elapsed;
            else {
                int isDown, isUp;
                
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
                    ms->curOpt++;
                    if (ms->curOpt >= OPT_MAX) {
                        if (ms->hasSave)
                            ms->curOpt = 0;
                        else
                            ms->curOpt = 1;
                    }
                    if (!ms->firstPress)
                        ms->lastPressedTime += 300;
                    else
                        ms->lastPressedTime += 100;
                    ms->firstPress = 1;
                    sfx_menuMove();
                }
                else if (isUp) {
                    ms->curOpt--;
                    if ((!ms->hasSave && ms->curOpt < 1) || ms->curOpt < 0)
                        ms->curOpt = OPT_MAX - 1;
                    if (!ms->firstPress)
                        ms->lastPressedTime += 300;
                    else
                        ms->lastPressedTime += 100;
                    ms->firstPress = 1;
                    sfx_menuMove();
                }
            }
            
            isEnter = GFraMe_keys.enter;
            isEnter = isEnter || GFraMe_keys.z;
            isEnter = isEnter || GFraMe_keys.space;
            if (GFraMe_controller_max > 0) {
                isEnter = isEnter || GFraMe_controllers[0].a;
                isEnter = isEnter || GFraMe_controllers[0].start;
            }
            if (GFraMe_controller_max > 1) {
                isEnter = isEnter || GFraMe_controllers[1].a;
                isEnter = isEnter || GFraMe_controllers[1].start;
            }
            if (isEnter) {
                sfx_menuSelect();
                ms->runMenu = 0;
                if (ms->curOpt == OPT_QUIT)
                    gl_running = 0;
            }
            
            ms->idleTime += GFraMe_event_elapsed;
            if (ms->idleTime >= TIME_TO_DEMO) {
                ms->runMenu = 0;
                ms->curOpt = OPT_MAX;
            }
        }
    GFraMe_event_update_end();
}

/**
 * Handle every event
 */
static void ms_event(struct stMenustate *ms) {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_key_down();
            ms->idleTime = 0;
        GFraMe_event_on_key_up();
            ms->firstPress = 0;
            ms->lastPressedTime = 0;
            ms->idleTime = 0;
            if (!ms->isTitleSet) {
                ms->isTitleSet = 1;
                ms->j1Y = 32;
                ms->j2Y = 32;
                ms->aY = 32;
                ms->tY = 32;
            }
        GFraMe_event_on_controller();
            if (event.type == SDL_CONTROLLERBUTTONUP && !ms->isTitleSet) {
                ms->isTitleSet = 1;
                ms->j1Y = 32;
                ms->j2Y = 32;
                ms->aY = 32;
                ms->tY = 32;
            }
            if (event.type == SDL_CONTROLLERBUTTONUP ||
                (event.type == SDL_CONTROLLERAXISMOTION
                && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY
                && GFraMe_controllers[event.caxis.which].ly < 0.3
                && GFraMe_controllers[event.caxis.which].ly > -0.3)
               ) {
                ms->firstPress = 0;
                ms->lastPressedTime = 0;
            }
            ms->idleTime = 0;
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
 * @param i Initial index
 * @param l Text length
 */
static void _ms_renderText(char *text, int X, int Y, int i, int l) {
    int x, y;
    
    x = X;
    y = Y;
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

