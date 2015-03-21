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

#include "credits.h"
#include "controller.h"
#include "demo.h"
#include "global.h"
#include "menustate.h"
#include "options.h"
#include "playstate.h"
#include "types.h"

void setIcon();

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
    
    setIcon();
    
    rv = GFraMe_audio_player_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Audio player init failed", __ret);
    
    rv = gl_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "global init failed", __ret);
    
    GFraMe_controller_init(1);
    
    st = MENUSTATE;
    // st = CREDITS;
    while (gl_running) {
        switch (st) {
            case     MENUSTATE: st = menustate(); break;
            case NEW_PLAYSTATE: st = playstate(0); break;
            case CNT_PLAYSTATE: st = playstate(1); break;
            case       OPTIONS: st = options(); break;
            case          DEMO: st = demo(); break;
            case       CREDITS: st = credits(); break;
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

void setIcon() {
            /* AA , RR , GG , BB */
    #define x 0x00,0x00,0x00,0x00,
    #define w 0xff,0x3f,0x3f,0x74,
    #define m 0xff,0x5b,0x6e,0xe1,
    #define n 0xff,0x63,0x9b,0xff,
    #define o 0xff,0x22,0x20,0x34,
    #define e 0xff,0x5f,0xcd,0xe5,
    #define s 0xff,0xcb,0xbf,0xfc,
    #define i 0xff,0xff,0xff,0xff,
    #define z 0xff,0x30,0x60,0x82,
	char arr[16*16*4] = 
{
x w w w w w w w w w w w w w w x
w n n n n n n n n n n n n n n w
w m o o o o o o o o o o o o n w
w m o o o o o e e e e o o o n w
w m o o o o e m n m m e o o n w
w m o e e e m e m n m n e o n w
w m e m n e n m s n n m e o n w
w m e w m e m s w s w n e o n w
w m o e s w s i z i z s m e n w
w m o e w i w i i i i w e o n w
w m o e z i z w w w w e o o n w
w m o e i i i i i w e o o o n w
w m o e w w w w w e o o o o n w
w m o o e e e e e o o o o o n w
w m m m m m m m m m m m m m m w
x w w w w w w w w w w w w w w x
};
    #undef x
    #undef w
    #undef m
    #undef n
    #undef o
    #undef e
    #undef s
    #undef i
    #undef z
    
	GFraMe_set_icon(arr);
}

