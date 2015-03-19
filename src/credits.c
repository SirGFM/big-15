/**
 * @file src/menustate.c
 * 
 * Menu state
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "global.h"
#include "globalVar.h"
#include "transition.h"

// Initialize variables used by the event module
GFraMe_event_setup();

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ms_init();
/**
 * Clean up the menustate
 */
static void ms_clean();
/**
 * Draw the current frame
 */
static void ms_draw();
/**
 * Update the current frame, as many times as it's accumulated
 */
static void ms_update();
/**
 * Handle every event
 */
static void ms_event();

/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 */
void menustate() {
    GFraMe_ret rv;
    
    gl_running = 0;
    rv = ps_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init menustate", __ret);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    gl_running = 1;
    while (gl_running) {
        ms_event();
        ps_update();
        ms_draw();
    }
    
__ret:
    ms_clean();
}

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static GFraMe_ret ms_init() {
    GFraMe_ret rv;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up the menustate
 */
static void ps_clean() {
}

/**
 * Draw the current frame
 */
static void ms_draw() {
    GFraMe_event_draw_begin();
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ms_update() {
    GFraMe_event_update_begin();
    GFraMe_event_update_end();
}

/**
 * Handle every event
 */
static void ms_event() {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_key_down();
        GFraMe_event_on_key_up();
        GFraMe_event_on_controller();
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

