/**
 * @file src/options.c
 * 
 * Options state
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_save.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "audio.h"
#include "controller.h"
#include "global.h"
#include "options.h"
#include "types.h"

enum { OPT_P1DEV, OPT_P1MODE, OPT_P2DEV, OPT_P2MODE, OPT_BACK, OPT_MAX };

struct stOptions {
    /** Whether the menu is still running */
    int running;
    /** For how long the last key has been pressed */
    int lastPressedTime;
    /** Whether a key was being pressed */
    int firstPress;
    /** Current option */
    int curOpt;
};

// Initialize variables used by the event module
GFraMe_event_setup();

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _op_renderText(char *text, int X, int Y, int l);

/**
 * Render a input mode, on the desired position
 * 
 * mode The mode
 * x Horizontal position
 * y Vertical position
 */
static void _op_renderMode(ctr_mode mode, int x, int y);

/**
 * Initialize everything!!!
 */
static void op_init(struct stOptions *op);
/**
 * Draw everything
 */
static void op_draw(struct stOptions *op);
/**
 * Update everything
 */
static void op_update(struct stOptions *op);
/**
 * Handle every event
 */
static void op_event(struct stOptions *op);

/**
 * Menustate options. Must initialize it, run the loop and clean it up
 * 
 * @return To which state it's switching
 */
state options() {
    GFraMe_ret rv;
    GFraMe_save sv;
    int pl1, pl2;
    state ret;
    struct stOptions op;
    
    op_init(&op);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    while (gl_running && op.running) {
        op_event(&op);
        op_update(&op);
        op_draw(&op);
    }
    
    // Get the current input mode
    ctr_getModes(&pl1, &pl2);
    // Save it into a file
    rv = GFraMe_save_bind(&sv, CONFFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error opening file", __ret);
    rv = GFraMe_save_write_int(&sv, "ctr_pl1", pl1);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error writing variable", __ret);
    rv = GFraMe_save_write_int(&sv, "ctr_pl2", pl2);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Error writing variable", __ret);
    
    ret = MENUSTATE;
__ret:
    return ret;
}

/**
 * Initialize everything!!!
 */
static void op_init(struct stOptions *op) {
    op->running = 1;
    
    op->lastPressedTime = 0;
    op->firstPress = 0;
    op->curOpt = 0;
}

/**
 * Draw everything
 */
static void op_draw(struct stOptions *op) {
    GFraMe_event_draw_begin();
        int pl1, pl2, x, y, _y;
        
        ctr_getModes(&pl1, &pl2);
        
        // Draw the BG
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
        
        _op_renderText("OPTIONS", 16/*x*/, 3/*y*/, sizeof("OPTIONS")-1);
        
/*
        "PL1 DEVICE"
        " PL1 MODE"
        "PL2 DEVICE"
        " PL2 MODE"
        "   BACK"
*/
        
        x = 3;
        y = 8;
        
        _op_renderText("<", x-1, y+op->curOpt, 1);
        _op_renderText(">", x+10, y+op->curOpt, 1);
        _op_renderText("PL1 DEVICE", x, y, sizeof("PL1 DEVICE")-1);
        if (pl1 < CTR_PAD1_A)
            _op_renderText("KEYBOARD", x+12, y, sizeof("KEYBOARD")-1);
        else if (pl1 < CTR_PAD2_A)
            _op_renderText("GAMEPAD1", x+12, y, sizeof("GAMEPAD1")-1);
        else
            _op_renderText("GAMEPAD2", x+12, y, sizeof("GAMEPAD2")-1);
        y++;
        
        _op_renderText("PL1 MODE", x, y, sizeof("PL1 MODE")-1);
        if (pl1 % 4 == 0)
            _op_renderText("TYPE A ", x+12, y, sizeof("TYPE A")-1);
        else if (pl1 % 4 == 1)
            _op_renderText("TYPE B ", x+12, y, sizeof("TYPE B")-1);
        else if (pl1 % 4 == 2)
            _op_renderText("TYPE C ", x+12, y, sizeof("TYPE C")-1);
        else if (pl1 % 4 == 3)
            _op_renderText("TYPE D ", x+12, y, sizeof("TYPE D")-1);
        y++;
        
        _op_renderText("PL2 DEVICE", x, y, sizeof("PL2 DEVICE")-1);
        if (pl2 < CTR_PAD1_A)
            _op_renderText("KEYBOARD", x+12, y, sizeof("KEYBOARD")-1);
        else if (pl2 < CTR_PAD2_A)
            _op_renderText("GAMEPAD1", x+12, y, sizeof("GAMEPAD1")-1);
        else
            _op_renderText("GAMEPAD2", x+12, y, sizeof("GAMEPAD2")-1);
        y++;
        
        _op_renderText("PL2 MODE", x, y, sizeof("PL2 MODE")-1);
        if (pl2 % 4 == 0)
            _op_renderText("TYPE A ", x+12, y, sizeof("TYPE A")-1);
        else if (pl2 % 4 == 1)
            _op_renderText("TYPE B ", x+12, y, sizeof("TYPE B")-1);
        else if (pl2 % 4 == 2)
            _op_renderText("TYPE C ", x+12, y, sizeof("TYPE C")-1);
        else if (pl2 % 4 == 3)
            _op_renderText("TYPE D ", x+12, y, sizeof("TYPE D")-1);
        y++;
        
        _op_renderText("BACK", x, y, sizeof("BACK")-1);
        y += 4;
        
        _op_renderText("PLAYER1", x+13, y, sizeof("PLAYER1")-1);
        _op_renderText("PLAYER2", x+24, y, sizeof("PLAYER2")-1);
        y += 2;
        
        _y = y;
        _op_renderText("LEFT", x, _y++, sizeof("LEFT")-1);
        _op_renderText("RIGHT", x, _y++, sizeof("RIGHT")-1);
        _op_renderText("UP", x, _y++, sizeof("UP")-1);
        _op_renderText("JUMP", x, _y++, sizeof("JUMP")-1);
        _op_renderText("ITEM", x, _y++, sizeof("ITEM")-1);
        _op_renderText("SWITCH", x, _y++, sizeof("SWITCH")-1);
        
        _op_renderMode(pl1, x+12, y);
        _op_renderMode(pl2, x+23, y);
        
    GFraMe_event_draw_end();
}

/**
 * Update everything
 */
static void op_update(struct stOptions *op) {
    GFraMe_event_update_begin();
        int isEnter;
        
        if (op->lastPressedTime > 0)
            op->lastPressedTime -= GFraMe_event_elapsed;
        else {
            int isDown, isUp, isLeft, isRight;
            
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
            isLeft = GFraMe_keys.left;
            isLeft = isLeft || GFraMe_keys.a;
            if (GFraMe_controller_max >= 1) {
                isLeft = isLeft || GFraMe_controllers[0].lx < -0.5;
                isLeft = isLeft || GFraMe_controllers[0].left;
            }
            isRight = GFraMe_keys.right;
            isRight = isRight || GFraMe_keys.d;
            if (GFraMe_controller_max >= 1) {
                isRight = isRight || GFraMe_controllers[0].lx > 0.5;
                isRight = isRight || GFraMe_controllers[0].right;
            }
            
            if (isDown) {
                op->curOpt++;
                if (op->curOpt >= OPT_MAX) {
                    op->curOpt = 0;
                }
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
                sfx_menuMove();
            }
            else if (isUp) {
                op->curOpt--;
                if (op->curOpt < 0)
                    op->curOpt = OPT_MAX - 1;
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
                sfx_menuMove();
            }
            else if (isLeft && op->curOpt >= OPT_P1DEV && op->curOpt <= OPT_P2MODE) {
                flag ID;
                int p1, p2;
                ctr_mode val;
                
                ctr_getModes(&p1, &p2);
                if (op->curOpt < OPT_P2DEV) {
                    ID = ID_PL1;
                    val = p1;
                }
                else {
                    ID = ID_PL2;
                    val = p2;
                }
                if (op->curOpt == OPT_P1DEV || op->curOpt == OPT_P2DEV) {
                    while (1) {
                        if (val > CTR_KEYS_D)
                            val -= 4;
                        else
                            val += 8;
                        if (ctr_setMode(ID, val))
                            break;
                    }
                }
                else {
                    while (1) {
                        if (val % 4 == 0)
                            val += 3;
                        else
                            val--;
                        if (ctr_setMode(ID, val))
                            break;
                    }
                }
                
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
                sfx_menuMove();
            }
            else if (isRight && op->curOpt >= OPT_P1DEV && op->curOpt <= OPT_P2MODE) {
                flag ID;
                int p1, p2;
                ctr_mode val;
                
                ctr_getModes(&p1, &p2);
                if (op->curOpt < OPT_P2DEV) {
                    ID = ID_PL1;
                    val = p1;
                }
                else {
                    ID = ID_PL2;
                    val = p2;
                }
                if (op->curOpt == OPT_P1DEV || op->curOpt == OPT_P2DEV) {
                    while (1) {
                        if (val < CTR_PAD2_A)
                            val += 4;
                        else
                            val -= 8;
                        if (ctr_setMode(ID, val))
                            break;
                    }
                }
                else {
                    while (1) {
                        if (val % 4 == 3)
                            val -= 3;
                        else
                            val++;
                        if (ctr_setMode(ID, val))
                            break;
                    }
                }
                
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
                sfx_menuMove();
            }
        }
        isEnter = GFraMe_keys.enter;
        isEnter = isEnter || GFraMe_keys.z;
        isEnter = isEnter || GFraMe_keys.space;
        if (GFraMe_controller_max > 0) {
            isEnter = isEnter || GFraMe_controllers[0].a;
            isEnter = isEnter || GFraMe_controllers[0].start;
        }
        if (isEnter && op->curOpt == OPT_BACK) {
            op->running = 0;
        }
    GFraMe_event_update_end();
}

/**
 * Handle every event
 */
static void op_event(struct stOptions *op) {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_key_down();
        GFraMe_event_on_key_up();
            op->firstPress = 0;
            op->lastPressedTime = 0;
        GFraMe_event_on_controller();
            if (event.type == SDL_CONTROLLERBUTTONUP ||
                (GFraMe_controller_max > 0
                    && (GFraMe_controllers[0].ly < 0.5
                        && GFraMe_controllers[0].ly > -0.5)
                    && (GFraMe_controllers[0].lx < 0.5
                        && GFraMe_controllers[0].lx > -0.5)
                    && !GFraMe_controllers[0].up
                    && !GFraMe_controllers[0].down)) {
                op->firstPress = 0;
                op->lastPressedTime = 0;
            }
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
 * @param l Text length
 */
static void _op_renderText(char *text, int X, int Y, int l) {
    int i, x, y;
    
    i = 0;
    x = X*8;
    y = Y*8;
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

/**
 * Render a input mode, on the desired position
 * 
 * mode The mode
 * x Horizontal position
 * y Vertical position
 */
static void _op_renderMode(ctr_mode mode, int x, int y) {
    switch (mode) {
        case CTR_KEYS_A: {
            _op_renderText("  A      ", x, y++, sizeof("         ")-1);
            _op_renderText("  D      ", x, y++, sizeof("         ")-1);
            _op_renderText("  W      ", x, y++, sizeof("         ")-1);
            _op_renderText("  SPACE  ", x, y++, sizeof("         ")-1);
            _op_renderText("  SHIFT  ", x, y++, sizeof("         ")-1);
            _op_renderText("  TAB    ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_KEYS_B: {
            _op_renderText("  LEFT   ", x, y++, sizeof("         ")-1);
            _op_renderText("  RIGHT  ", x, y++, sizeof("         ")-1);
            _op_renderText("  UP     ", x, y++, sizeof("         ")-1);
            _op_renderText("  X      ", x, y++, sizeof("         ")-1);
            _op_renderText("  C      ", x, y++, sizeof("         ")-1);
            _op_renderText("  V      ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_KEYS_C: {
            _op_renderText("  NUM4   ", x, y++, sizeof("         ")-1);
            _op_renderText("  NUM6   ", x, y++, sizeof("         ")-1);
            _op_renderText("  NUM8   ", x, y++, sizeof("         ")-1);
            _op_renderText("  NUM0   ", x, y++, sizeof("         ")-1);
            _op_renderText(" NUMENTER", x, y++, sizeof("         ")-1);
            _op_renderText(" NUMCOMMA", x, y++, sizeof("         ")-1);
        } break;
        case CTR_KEYS_D: {
            _op_renderText("  H      ", x, y++, sizeof("         ")-1);
            _op_renderText("  K      ", x, y++, sizeof("         ")-1);
            _op_renderText("  U      ", x, y++, sizeof("         ")-1);
            _op_renderText("  L      ", x, y++, sizeof("         ")-1);
            _op_renderText("  G      ", x, y++, sizeof("         ")-1);
            _op_renderText("  I      ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_PAD2_A:
        case CTR_PAD1_A: {
            _op_renderText("  LEFT   ", x, y++, sizeof("         ")-1);
            _op_renderText("  RIGHT  ", x, y++, sizeof("         ")-1);
            _op_renderText("  UP     ", x, y++, sizeof("         ")-1);
            _op_renderText("  A      ", x, y++, sizeof("         ")-1);
            _op_renderText("  B      ", x, y++, sizeof("         ")-1);
            _op_renderText("  X      ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_PAD2_B:
        case CTR_PAD1_B: {
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText("  A      ", x, y++, sizeof("         ")-1);
            _op_renderText("  B      ", x, y++, sizeof("         ")-1);
            _op_renderText("  X      ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_PAD2_C:
        case CTR_PAD1_C: {
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" LSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText("LSHOULDER", x, y++, sizeof("         ")-1);
            _op_renderText(" LTRIGGER", x, y++, sizeof("         ")-1);
            _op_renderText(" LEFT    ", x, y++, sizeof("         ")-1);
        } break;
        case CTR_PAD2_D:
        case CTR_PAD1_D: {
            _op_renderText(" RSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" RSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText(" RSTICK  ", x, y++, sizeof("         ")-1);
            _op_renderText("RSHOULDER", x, y++, sizeof("         ")-1);
            _op_renderText(" RTRIGGER", x, y++, sizeof("         ")-1);
            _op_renderText("  X      ", x, y++, sizeof("         ")-1);
        } break;
        default: {}
    }
}
