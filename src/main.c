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
#include "errorstate.h"
#include "global.h"
#include "menustate.h"
#include "options.h"
#include "playstate.h"
#include "save.h"
#include "state.h"
#include "types.h"

void setIcon();

#if defined(EMCC)
#  include <stdio.h>
#  include <stdlib.h>
#  include <emscripten.h>
#endif

void mainloop(void **ctx) {
    struct stateHandler **pHandle = (struct stateHandler**)ctx;
    struct stateHandler *curState = *pHandle;

#if defined(EMCC)
    do {
        /* Issue a new frame on web */
        SDL_Event event;
        SDL_UserEvent userevent;

        memset(&userevent, 0x0, sizeof(userevent));
        userevent.type = SDL_USEREVENT;
        event.type = SDL_USEREVENT;
        event.user = userevent;

        SDL_PushEvent(&event);
    } while (0);
#endif

    if (gl_running && curState->isRunning(curState))
        curState->update(curState);

    if (!curState->isRunning(curState)) {
        state next;
        int jerr, gfmErr = 0;

        /* If the state stopped running, get its error and free resources */
        next = curState->nextState(curState);
        jerr = curState->getExitError(curState);
        if (isPlaystate(curState))
            gfmErr = playstate_getGfmError(curState);
        if (!gl_running)
            return;
        else if (next != OPTIONS && next != POP)
            curState->release(curState);

        /* Switch to the new state and return */
        switch (next) {
            case MENUSTATE:
                curState = (struct stateHandler*)menustate_getHnd();
                break;
            case NEW_PLAYSTATE:
                curState = (struct stateHandler*)playstate_getHnd(NEWGAME);
                break;
            case CNT_PLAYSTATE:
                curState = (struct stateHandler*)playstate_getHnd(CONTINUE);
                break;
            case MT_PLAYSTATE:
                curState = (struct stateHandler*)playstate_getHnd(MT_VERSION);
                break;
            case OPTIONS:
                push(&curState, options_getHnd());
                break;
            case DEMO:
                curState = (struct stateHandler*)demo_getHnd();
                break;
            case CREDITS:
                curState = (struct stateHandler*)credits_getHnd();
                break;
            case POP:
                /* XXX: When exiting the options from the playstate,
                 * everything works correctly and the state's setup
                 * shouldn't be executed.
                 * However, there's a bug in the menustate the causes it to
                 * keep looping back to the options menu. Although not
                 * optimal, calling its setup break out from that loop.
                 */
                pop(&curState);
                if (isPlaystate(curState)) {
                    *pHandle = curState;
                    return;
                }
                break;
            case ERRORSTATE:
                curState = errorstate_getHnd(jerr, gfmErr);
                break;
        }
        /* Update the handle passed to the mainloop */
        *pHandle = curState;

        curState->setup(curState);
    }
}

int main(int argc, char *argv[]) {
    struct stateHandler *curState;
    GFraMe_ret rv;
    GFraMe_wndext ext;
    int zoom, lang;
    
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
             1, // Log to file
             0  // Log append
            );
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Init failed", __ret);

    /* Ready the local save files */
    setup_blocks();
    
    setIcon();
    
    // Set the bg color
    GFraMe_set_bg_color(0x22, 0x20, 0x34, 0xff);
    // Set the actual game dimensions
    zoom = read_slot(BLK_CONFIG, sv_zoom);
    lang = read_slot(BLK_CONFIG, sv_lang);
    // Switch the resolution
    if (zoom != -1 && zoom != 0 && zoom != 2) {
        GFraMe_ret rv;

        rv = GFraMe_screen_set_window_size(SCR_W*zoom, SCR_H*zoom);
        if (rv == GFraMe_ret_ok)
            GFraMe_screen_set_pixel_perfect(0, 1);
    }
    else if (zoom == 0) {
        GFraMe_ret rv;

        rv = GFraMe_screen_setFullscreen();
        if (rv == GFraMe_ret_ok)
            GFraMe_screen_set_pixel_perfect(0, 1);
    }
    if (lang == EN_US) {
        gl_lang = EN_US;
    }
    else if (lang == PT_BR) {
        gl_lang = PT_BR;
    }
    
    rv = GFraMe_audio_player_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Audio player init failed", __ret);
    
    rv = gl_init();
    GFraMe_assertRet(rv == GFraMe_ret_ok, "global init failed", __ret);
    
    GFraMe_controller_init(1);
    
    curState = (struct stateHandler*)menustate_getHnd();
    curState->setup(curState);
#if defined(EMCC)
    do {
        struct stateHandler **ctx;

        ctx = (struct stateHandler**)malloc(sizeof(ctx));
        if (!ctx) {
            printf("Failed to alloc memory for the contex!\n");
            goto __ret;
        }
        *ctx = curState;

        emscripten_set_main_loop_arg((em_arg_callback_func)mainloop, ctx, 0, 0);
        /* Done setting up emscripten, let everything do their work. */
        return 0;
    } while (0);
#else
    while (gl_running)
        mainloop((void**)&curState);
#endif
    
    rv = 0;
__ret:
    GFraMe_audio_player_pause();
    GFraMe_audio_player_clear();
    gl_clean();
    GFraMe_controller_close();
    GFraMe_quit();
    
    return rv;
}

void setIcon() {
            /* AA , RR , GG , BB */
    #define _ 0x00,0x00,0x00,0x00,
    #define T 0xff,0xff,0xff,0xff,
    #define F 0xff,0xcb,0xdb,0xfc,
    #define E 0xff,0x63,0x9b,0xff,
    #define U 0xff,0x5b,0x6e,0xe1,
    #define H 0xff,0x30,0x60,0x82,
    #define D 0xff,0x3f,0x3f,0x74,
    #define B 0xff,0x22,0x20,0x34,
	char arr[16*16*4] = 
{ 
_ D D D D D D D D D D D D D D _
D F F T T T T T T T T T T T T D
D U B B B B B B B B B B B B T D
D U B B B B B E F F T B B B T D
D U B B B B D E E U U T B B T D
D U B B B E E D E E U E F B T D
D U B T T U U E F U E U E B T D
D U B F E U U F D F D E E B T D
D U B U D U F T E T E F U B T D
D U B B F F U T T T T D B B T D
D U B B D T D U U D D B B B T D
D U B B H T H T T F D B B B T D
D U B B T T T T T D B B B B T D
D U B B D D D D D B B B B B F D
D U U U U U U U U U U U U U F D
_ D D D D D D D D D D D D D D _
};
    #undef _
    #undef T
    #undef F
    #undef E
    #undef U
    #undef H
    #undef D
    #undef B
    
	GFraMe_set_icon(arr);
}

