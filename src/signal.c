/**
 * @file src/signal.c
 * 
 * Signal item
 */
#include <GFraMe/GFraMe_spriteset.h>

#include "global.h"
#include "globalVar.h"
#include "signal.h"

/** List of possible frames */
enum { SGNL_FRAME0 = 421, SGNL_FRAME1, SGNL_FRAME2, SGNL_FRAME3, SGNL_FRAME4,
    SGNL_FRAME5, SGNL_FRAME_MAX };
/** List of states */
typedef enum { SGNL_NONE, SGNL_BEGIN, SGNL_ACTIVE, SGNL_END } sgnlState;

/** Signal's structure  */
typedef struct {
    sgnlState state;
    int frame;
    int x;
    int y;
    int time;
} stSignal;

stSignal cur;
stSignal tmp;

/**
 * Initialize this submodule
 */
void signal_init() {
    cur.state = SGNL_NONE;
    tmp.state = SGNL_NONE;
}

/**
 * Setup a new signal (and release the previous)
 * 
 * @param cx Horizontal position
 * @param cy Vertical position
 */
void signal_setPos(int cx, int cy) {
    // Set the teleport position
    gv_setValue(SIGL_X, cx);
    gv_setValue(SIGL_Y, cy);
    // Copy the current state, to properly finish it
    if (cur.state != SGNL_NONE) {
        tmp.state = SGNL_END;
        tmp.x = cur.x;
        tmp.y = cur.y;
        tmp.frame = cur.frame;
        tmp.time = cur.time;
    }
    // Init the new signal
    cur.x = cx - 4;
    cur.y = cy - 8;
    cur.frame = SGNL_FRAME0;
    cur.time = 0;
    cur.state = SGNL_BEGIN;
}

/**
 * Stop the signal animation
 */
void signal_release() {
    if (cur.state == SGNL_NONE) {
        cur.time = 0;
        cur.frame = SGNL_FRAME0;
        cur.x = gv_getValue(TELP_X) - 4;
        cur.y = gv_getValue(TELP_Y);
    }
    cur.state = SGNL_END;
    gv_setValue(SIGL_X, -1);
    gv_setValue(SIGL_Y, -1);
}

/**
 * Updates a signal structure
 * 
 * @param sg The signal
 * @param ms Time elapse since the previous frame, in milliseconds
 */
static void signal_intUpdate(stSignal *sg, int ms) {
    ASSERT_NR(sg->state != SGNL_NONE);
    
    // Update the signal's timer
    sg->time += ms;
    // Check if a new frame was issued
    if (sg->time > 80) {
        sg->time -= 80;
        // Handle each state
        switch (sg->state) {
            case SGNL_NONE: break;
            case SGNL_BEGIN: {
                sg->state++;
                sg->frame++;
            } break;
            case SGNL_ACTIVE: {
                if (sg->frame == SGNL_FRAME1)
                    sg->frame++;
                else if (sg->frame == SGNL_FRAME2)
                    sg->frame--;
            } break;
            case SGNL_END: {
                sg->frame++;
                if (sg->frame == SGNL_FRAME_MAX)
                    sg->state = SGNL_NONE;
            } break;
        }
    }
    
__ret:
    return;
}

/**
 * Updates the submodule
 * 
 * @param ms Time elapse since the previous frame, in milliseconds
 */
void signal_update(int ms) {
    signal_intUpdate(&cur, ms);
    signal_intUpdate(&tmp, ms);
}

/**
 * Draw a signal structure
 * 
 * @param sg The signal
 */
static void signal_intDraw(stSignal *sg) {
    ASSERT_NR(sg->state != SGNL_NONE);
    // Draw the signal's frame (mirroring it to the right"
    GFraMe_spriteset_draw(gl_sset8x16, sg->frame, sg->x    , sg->y, 0/*flip*/);
    GFraMe_spriteset_draw(gl_sset8x16, sg->frame, sg->x + 8, sg->y, 1/*flip*/);
__ret:
    return;
}

/**
 * Draw the submodule
 */
void signal_draw() {
    signal_intDraw(&cur);
    signal_intDraw(&tmp);
}

