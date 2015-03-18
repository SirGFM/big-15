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
#include "global.h"
#include "playstate.h"

int main(int argc, char *argv[]) {
    GFraMe_ret rv;
    GFraMe_wndext ext;
    
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
    
//    rv = GFraMe_audio_player_init();
//    GFraMe_assertRet(rv == GFraMe_ret_ok, "Audio player init failed", __ret);
    
    rv = gl_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "global init failed", __ret);
    
    GFraMe_controller_init(1);
    if (GFraMe_controller_max >= 2)
        ctr_setMode(CTR_2CTR);
    else if (GFraMe_controller_max == 0)
        ctr_setMode(CTR_KEYS);
//    GFraMe_audio_player_play_bgm(gl_bgm, 0.60f);
    
    while (gl_running) {
        playstate(0);
    }
    
__ret:
//    GFraMe_audio_player_pause();
//    GFraMe_audio_player_clear();
    gl_clean();
    GFraMe_controller_close();
    GFraMe_quit();
    
    return 0;
}


