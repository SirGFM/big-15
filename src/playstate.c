/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>

#include "playstate.h"
#include "ui.h"

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
 * Playstate implementation. Must initialize it, run the loop and clean it up
 */
void playstate() {
    GFraMe_ret rv;
    
    rv = ps_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init playstate", __ret);
    
    GFraMe_event_init(60, 60);
    
    while (1/* is running */) {
        ps_event();
        ps_update();
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
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up the playstate
 */
static void ps_clean() {
    ui_clean();
}

/**
 * Draw the current frame
 */
static void ps_draw() {
    GFraMe_event_draw_begin();
        ui_draw();
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ps_update() {
    GFraMe_event_update_begin();
        ui_update(GFraMe_event_elapsed);
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
//            if (GFraMe_keys.esc)
//                gl_running = 0;
        GFraMe_event_on_key_up();
        GFraMe_event_on_controller();
//            if (GFraMe_controller_max > 0 && GFraMe_controllers[0].home)
//                gl_running = 0;
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

