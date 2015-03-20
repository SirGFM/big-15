/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#include <GFraMe/GFraMe.h>
#include <GFraMe/GFraMe_audio_player.h>
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_screen.h>
//#include <GFraMe/GFraMe_sprite.h>

#include "controller.h"
#include "demo.h"
#include "global.h"
#include "menustate.h"
#include "options.h"
#include "playstate.h"
#include "types.h"

int main(int argc, char *argv[]) {
    GFraMe_ret rv;
    GFraMe_wndext ext;
    state st;
    
    ext.atlas = "atlas";
    ext.atlasWidth = 256;
    ext.atlasHeight = 256;
    ext.flags = GFraMe_wndext_none;
	//ext.flags = GFraMe_wndext_scanline;
    
    rv = GFraMe_init
            (
             SCR_W,
             SCR_H,
             WND_W,
             WND_H,
             ORG,
             NAME,
             GFraMe_window_none, // GFraMe_window_fullscreen
             &ext,
             FPS,
             0, // Log to file
             0  // Log append
            );
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Init failed", __ret);
    
    rv = GFraMe_audio_player_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Audio player init failed", __ret);
    
    rv = gl_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "global init failed", __ret);
    
    GFraMe_controller_init(1);
    
    st = MENUSTATE;
    while (gl_running) {
        switch (st) {
            case     MENUSTATE: st = menustate(); break;
            case NEW_PLAYSTATE: st = playstate(0); break;
            case CNT_PLAYSTATE: st = playstate(1); break;
            case       OPTIONS: st = options(); break;
            case          DEMO: st = demo(); break;
            // case       CREDITS:
            default: GFraMe_assertRet(0, "Invalid state!", __ret);
        }
    }
    
__ret:
    GFraMe_audio_player_pause();
    GFraMe_audio_player_clear();
    gl_clean();
    GFraMe_controller_close();
    GFraMe_quit();
    
    return 0;
}


