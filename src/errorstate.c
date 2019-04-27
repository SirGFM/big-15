/**
 * @file src/errorstate.c
 * 
 * Menu state
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "errorstate.h"
#include "global.h"
#include "state.h"
#include "types.h"

// Initialize variables used by the event module
GFraMe_event_setup();

struct stErrorstate {
    struct stateHandler hnd;
    jjatError jerr;
    GFraMe_ret gfmErr;
};

int errorstate_setup(void *self) {
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    return 0;
}

int errorstate_isRunning(void *self) {
    /* Yup, this never exits. D: */
    return 1;
}

/** Handle errorstate events */
static void err_event();

/** Handle updating errorstate */
static void err_update();

/** Handle drawing errorstate */
static void err_draw(jjatError jerr, GFraMe_ret gfmErr);

void errorstate_update(void *self) {
    struct stErrorstate *err = (struct stErrorstate*)self;

    err_event();
    err_update();
    err_draw(err->jerr, err->gfmErr);
}

int errorstate_nextState(void *self) {
    /* This shouldn't ever change, but if it does, go back to itself. */
    return ERRORSTATE;
}

void errorstate_release(void *self) {
}

int errorstate_getExitError(void *self) {
    return 0;
}

static struct stErrorstate global_err;
void *errorstate_getHnd(int jerr, int gfmErr) {
    struct stateHandler *hnd = &(global_err.hnd);

    memset(&global_err, 0x0, sizeof(global_err));
    hnd->setup = &errorstate_setup;
    hnd->isRunning = &errorstate_isRunning;
    hnd->update = &errorstate_update;
    hnd->nextState = &errorstate_nextState;
    hnd->release = &errorstate_release;
    hnd->getExitError = &errorstate_getExitError;

    global_err.jerr = jerr;
    global_err.gfmErr = gfmErr;

    return &global_err;
}

static void err_event() {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

static void err_update() {
    GFraMe_event_update_begin();
    GFraMe_event_update_end();
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
static void _renderText(char *text, int X, int Y, int l) {
    int i, x, y;

    x = X;
    y = Y;
    // Draw the text
    for (i = 0; i < l; i++) {
        char c;

        c = text[i];

        if (c == '\n') {
            x = X - 8;
            y += 8;
        }
        else if (c != ' ')
            GFraMe_spriteset_draw(gl_sset8x8, c-'!', x, y, 0/*flipped*/);

        x += 8;
    }
}


static void err_draw(jjatError jerr, GFraMe_ret gfmErr) {
    int x, y;

    #define PRINTERR(_txt_) \
        do { \
            _renderText(_txt_, x, y, sizeof(_txt_) - 1); \
            y += 16; \
        } while (0)

    GFraMe_event_draw_begin();
        x = 8;
        y = 32;

        PRINTERR("SOMETHING WENT TERRIBLY WRONG...");

        switch (jerr) {
        case JERR_NONE:
            PRINTERR("NO ERROR DETECTED.\nTHIS SHOULDN'T HAVE HAPPENED!!");
            break;
        case JERR_LOAD_MAP:
            PRINTERR("FAILED TO LOAD A MAP!");

            #define PRINT_GFM_ERR(_rv_) \
                case _rv_: \
                    PRINTERR(#_rv_); \
                    break

            switch (gfmErr) {
                case GFraMe_ret_ok: PRINTERR("GFRAME_RET_OK"); break;
                case GFraMe_ret_sdl_init_failed: PRINTERR("GFRAME_RET_SDL_INIT_FAILED"); break;
                case GFraMe_ret_bad_param: PRINTERR("GFRAME_RET_BAD_PARAM"); break;
                case GFraMe_ret_window_creation_failed: PRINTERR("GFRAME_RET_WINDOW_CREATION_FAILED"); break;
                case GFraMe_ret_renderer_creation_failed: PRINTERR("GFRAME_RET_RENDERER_CREATION_FAILED"); break;
                case GFraMe_ret_backbuffer_creation_failed: PRINTERR("GFRAME_RET_BACKBUFFER_CREATION_FAILED"); break;
                case GFraMe_ret_texture_creation_failed: PRINTERR("GFRAME_RET_TEXTURE_CREATION_FAILED"); break;
                case GFraMe_ret_timer_creation_failed: PRINTERR("GFRAME_RET_TIMER_CREATION_FAILED"); break;
                case GFraMe_ret_fps_req_low: PRINTERR("GFRAME_RET_FPS_REQ_LOW"); break;
                case GFraMe_ret_num_display_failed: PRINTERR("GFRAME_RET_NUM_DISPLAY_FAILED"); break;
                case GFraMe_ret_display_modes_failed: PRINTERR("GFRAME_RET_DISPLAY_MODES_FAILED"); break;
                case GFraMe_ret_new_acc_frame: PRINTERR("GFRAME_RET_NEW_ACC_FRAME"); break;
                case GFraMe_ret_anim_new_frame: PRINTERR("GFRAME_RET_ANIM_NEW_FRAME"); break;
                case GFraMe_ret_anim_finished: PRINTERR("GFRAME_RET_ANIM_FINISHED"); break;
                case GFraMe_ret_file_not_found: PRINTERR("GFRAME_RET_FILE_NOT_FOUND"); break;
                case GFraMe_ret_memory_error: PRINTERR("GFRAME_RET_MEMORY_ERROR"); break;
                case GFraMe_ret_read_file_failed: PRINTERR("GFRAME_RET_READ_FILE_FAILED"); break;
                case GFraMe_ret_no_overlap: PRINTERR("GFRAME_RET_NO_OVERLAP"); break;
                case GFraMe_ret_render_failed: PRINTERR("GFRAME_RET_RENDER_FAILED"); break;
                case GFraMe_ret_invalid_texture: PRINTERR("GFRAME_RET_INVALID_TEXTURE"); break;
                case GFraMe_ret_stop_timer_failed: PRINTERR("GFRAME_RET_STOP_TIMER_FAILED"); break;
                case GFraMe_ret_failed: PRINTERR("GFRAME_RET_FAILED"); break;
                case GFraMe_platform_not_supported: PRINTERR("GFRAME_PLATFORM_NOT_SUPPORTED"); break;
                case GFraMe_buffer_too_small: PRINTERR("GFRAME_BUFFER_TOO_SMALL"); break;
                case GFraMe_no_version_found: PRINTERR("GFRAME_NO_VERSION_FOUND"); break;
                case GFraMe_older_save_version: PRINTERR("GFRAME_OLDER_SAVE_VERSION"); break;
                default:
                    PRINTERR("UNKNOWN LIB ERROR!");
            }
            break;
        default:
            PRINTERR("UNKNOWN ERROR!");
        }
    GFraMe_event_draw_end();
}
