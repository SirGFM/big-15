/**
 * @file src/playstate.c
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_util.h>

#include "global.h"
#include "map.h"
#include "playstate.h"
#include "ui.h"

// Initialize variables used by the event module
GFraMe_event_setup();

/**
 * Game map
 */
map *m;

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
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    while (gl_running) {
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
	char name[128];
    int len;
    
    GFraMe_ret rv;
    
    rv = ui_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init ui", __ret);
    
    rv = map_init(&m);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    
    len = 128;
	rv = GFraMe_assets_clean_filename(name, "maps/test_tm.txt", &len);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    rv = map_loadf(m, name);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to init map", __ret);
    
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
}

/**
 * Draw the current frame
 */
static void ps_draw() {
    GFraMe_event_draw_begin();
        map_draw(m);
        ui_draw();
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void ps_update() {
    GFraMe_event_update_begin();
        map_update(m, GFraMe_event_elapsed);
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

