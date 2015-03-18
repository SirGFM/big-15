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

#include "global.h"
#include "globalVar.h"
#include "transition.h"
#include "types.h"

#define GFM_ICON 39

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

enum {OPT_CONTINUE, OPT_NEWGAME, OPT_OPTIONS, OPT_QUIT, OPT_MAX};
struct stMenustate {
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
    /** Whether the menu is running */
    int runMenu;
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

/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 * 
 * @return To which state it's switching
 */
state menustate() {
    GFraMe_ret rv;
    state ret;
    struct stMenustate ms;
    
    rv = -1;
    gl_running = 0;
    rv = ms_init(&ms);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init menustate", __ret);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    gl_running = 1;
    while (gl_running && ms.runMenu) {
        ms_event(&ms);
        ms_update(&ms);
        ms_draw(&ms);
    }
    
    if (ms.curOpt == OPT_CONTINUE)
        ret = CNT_PLAYSTATE;
    else if (ms.curOpt == OPT_NEWGAME)
        ret = NEW_PLAYSTATE;
    else if (ms.curOpt == OPT_OPTIONS)
        ret = OPTIONS;
__ret:
    ms_clean(&ms);
    return ret;
}

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ms_init(struct stMenustate *ms) {
    GFraMe_ret rv;
    GFraMe_save sv;
    
    // Check if there's already a saved game
    rv = GFraMe_save_bind(&sv, SAVEFILE);
    if (rv == GFraMe_ret_ok) {
        // Check if something other than the version is written
        if (sv.size > 50)
            ms->hasSave = 1;
        else
            ms->hasSave = 0;
        GFraMe_save_close(&sv);
    }
    else {
        ms->hasSave = 0;
    }
    
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
    
    rv = GFraMe_ret_ok;
//__ret:
    return rv;
}

/**
 * Clean up the menustate
 */
static void ms_clean(struct stMenustate *ms) {
}

/**
 * Draw the current frame
 */
static void ms_draw(struct stMenustate *ms) {
    char options[] = "  CONTINUE  \n"
                     "  NEW GAME  \n"
                     "  OPTIONS   \n"
                     "    QUIT    ";
    char devText[] = "A GAME BY";
    char twitterText[] = "@SIRGFM";
    GFraMe_event_draw_begin();
        int i, l, x, y;
        
        x = 0;
        y = 0;
        while (1) {
            GFraMe_spriteset_draw(gl_sset8x8, 64, x, y, 0/*flipped*/);
            x += 8;
            if (x >= 320) {
                x = 0;
                y += 8;
            }
            if (y >= 240) {
                break;
            }
        }
        // TODO draw TILE
        
        // Put the 'selected' mark
        i = 13 * ms->curOpt;
        options[i] = '-'; options[i+1] = '-';
        options[i+10] = '-'; options[i+11] = '-';
        // Render texts
        l = sizeof(options);
        if (!ms->hasSave) {
            _ms_renderText(options, ms->textX, ms->textY + 8, 13/*i*/, l);
        }
        else {
            _ms_renderText(options, ms->textX, ms->textY, 0/*i*/, l);
        }
        l = sizeof(devText);
        _ms_renderText(devText, ms->iconX - l*8, ms->iconY + 16, 0, l);
        l = sizeof(twitterText);
        _ms_renderText(twitterText, ms->iconX - l*8, ms->iconY + 24, 0, l);
        //_ms_renderText(twitterText, (40 - l)*8, ms->iconY -8, 0, l);
        
        GFraMe_spriteset_draw(gl_sset32x32, GFM_ICON, ms->iconX, ms->iconY, 0);
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ms_update(struct stMenustate *ms) {
    GFraMe_event_update_begin();
        int isEnter;
        
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
            isUp = GFraMe_keys.up;
            isUp = isUp || GFraMe_keys.w;
            if (GFraMe_controller_max >= 1) {
                isUp = isUp || GFraMe_controllers[0].ly < -0.5;
                isUp = isUp || GFraMe_controllers[0].up;
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
            }
        }
        
        isEnter = GFraMe_keys.enter;
        isEnter = isEnter || GFraMe_keys.z;
        isEnter = isEnter || GFraMe_keys.space;
        if (GFraMe_controller_max > 0) {
            isEnter = isEnter || GFraMe_controllers[0].a;
            isEnter = isEnter || GFraMe_controllers[0].start;
        }
        if (isEnter) {
            ms->runMenu = 0;
            if (ms->curOpt == OPT_QUIT)
                gl_running = 0;
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
        GFraMe_event_on_key_up();
            ms->firstPress = 0;
            ms->lastPressedTime = 0;
        GFraMe_event_on_controller();
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

