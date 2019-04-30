/**
 * @file src/options.c
 * 
 * Options state
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_save.h>
#include <GFraMe/GFraMe_screen.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "audio.h"
#include "controller.h"
#include "global.h"
#include "options.h"
#include "save.h"
#include "state.h"
#include "types.h"

enum { OPT_UFPS, OPT_DFPS, OPT_RES, OPT_MUSIC, OPT_SFX, OPT_HINT, OPT_SPEEDRUN,
       OPT_P1DEV, OPT_P1MODE, OPT_P2DEV, OPT_P2MODE, OPT_LANG, OPT_BACK, OPT_MAX };

enum {
    TXT_OPTS,
    TXT_UPS,
    TXT_DPS,
    TXT_ZOOM,
    TXT_MUSIC,
    TXT_SFX,
    TXT_HINT,
    TXT_SPEEDRUN,
    TXT_PL1DEV,
    TXT_PL1MODE,
    TXT_PL2DEV,
    TXT_PL2MODE,
    TXT_LANG,
    TXT_BACK,
    TXT_ENABLED,
    TXT_DISABLED,
    TXT_MUTED,
    TXT_KEYBOARD,
    TXT_GAMEPAD1,
    TXT_GAMEPAD2,
    TXT_TYPEA,
    TXT_TYPEB,
    TXT_TYPEC,
    TXT_TYPED,
    TXT_LEFT,
    TXT_RIGHT,
    TXT_UP,
    TXT_JUMP,
    TXT_ITEM,
    TXT_SWITCH,
    TXT_MAX
};

struct stOptions {
    struct stateHandler hnd;
    /** Whether the menu is still running */
    int running;
    /** For how long the last key has been pressed */
    int lastPressedTime;
    /** Whether a key was being pressed */
    int firstPress;
    /** Current option */
    int curOpt;
    /** Whether or not hint should be shown */
    int hint;
    /** Current window resolution; [0, 4], where 0 mean fullscreen */
    int res;
    /** Desired update rate */
    int ufps;
    /** Desired draw rate */
    int dfps;
    /** Whether we are running in speed run mode (i.e., should display the timer) */
    int speedrun;
    /** Selected language */
    int lang;
};

// Initialize variables used by the event module
GFraMe_event_setup();

/**
 * Render a localized text to the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 */
static void _op_renderLang(struct stOptions *op, int text, int X, int Y);

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _op_renderText(char *text, int X, int Y, int l);
#define _op_renderTextStatic(text, X, Y) \
        _op_renderText(text, X, Y, sizeof(text) - 1)
#define _op_renderValueStatic(text, X, Y) \
        _op_renderText(text, X + tab, Y, sizeof(text) - 1)

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

int options_setup(void *self) {
    struct stOptions *op = (struct stOptions*)self;

    op_init(op);

    GFraMe_event_init(GAME_UFPS, GAME_DFPS);

    return 0;
}

int options_isRunning(void *self) {
    struct stOptions *op = (struct stOptions*)self;

    return op->running;
}

void options_update(void *self) {
    struct stOptions *op = (struct stOptions*)self;

    op_event(op);
    op_update(op);
    op_draw(op);
}

int options_nextState(void *self) {
    struct stOptions *op = (struct stOptions*)self;
    int pl1, pl2;

    // Get the current input mode
    ctr_getModes(&pl1, &pl2);
    // Save it into a file
    write_slot(BLK_CONFIG, sv_ctr_pl1, pl1);
    write_slot(BLK_CONFIG, sv_ctr_pl2, pl2);
    write_slot(BLK_CONFIG, sv_hint, op->hint);
    write_slot(BLK_CONFIG, sv_zoom, op->res);
    write_slot(BLK_CONFIG, sv_music, audio_getVolume());
    write_slot(BLK_CONFIG, sv_sfx, sfx_getVolume());
    write_slot(BLK_CONFIG, sv_ufps, op->ufps);
    write_slot(BLK_CONFIG, sv_dfps, op->dfps);
    write_slot(BLK_CONFIG, sv_speedrun, op->speedrun);
    write_slot(BLK_CONFIG, sv_lang, op->lang);
    flush_block(BLK_CONFIG);

    // Update the current language
    gl_lang = op->lang;

    return POP;
}

void options_release(void *self) {
    return;
}

int options_getExitError(void *self) {
    return 0;
}

static struct stOptions global_op;
void *options_getHnd() {
    struct stateHandler *hnd = &(global_op.hnd);

    memset(&global_op, 0x0, sizeof(global_op));
    hnd->setup = &options_setup;
    hnd->isRunning = &options_isRunning;
    hnd->update = &options_update;
    hnd->nextState = &options_nextState;
    hnd->release = &options_release;
    hnd->getExitError = &options_getExitError;

    return &global_op;
}

/**
 * Initialize everything!!!
 */
static void op_init(struct stOptions *op) {
    op->running = 1;
    
    op->lastPressedTime = 0;
    op->firstPress = 0;
    op->curOpt = 0;
    
    /* Try to read every option from the saved file */
    op->hint = read_slot(BLK_CONFIG, sv_hint);
    op->res = read_slot(BLK_CONFIG, sv_zoom);
    op->ufps = read_slot(BLK_CONFIG, sv_ufps);
    op->dfps = read_slot(BLK_CONFIG, sv_dfps);
    op->speedrun = read_slot(BLK_CONFIG, sv_speedrun);
    op->lang = read_slot(BLK_CONFIG, sv_lang);

    /* Set uninitialized values to their defaults */
    if (op->hint == -1)
        op->hint = 1;
    if (op->res == -1)
        op->res = 2;
    if (op->ufps == -1)
        op->ufps = GAME_UFPS;
    if (op->dfps == -1)
        op->dfps = GAME_DFPS;
    if (op->speedrun == -1)
        op->speedrun = 0;
    if (op->lang == -1)
        op->lang = EN_US;
}

/**
 * Draw everything
 */
static void op_draw(struct stOptions *op) {
    GFraMe_event_draw_begin();
        int i, pl1, pl2, tab, x, y, _y;
        
        ctr_getModes(&pl1, &pl2);
        
        _op_renderLang(op, TXT_OPTS, 16, 1);
        if (op->lang == PT_BR)
            tab = 22;
        else /* if (op->lang == EN_US) */
            tab = 13;
        
/*
        "UPDATE RATE " 
        "DRAW RATE   "
        "ZOOM        "
        "MUSIC       "
        "SFX         "
        "HINT        "
        "PL1 DEVICE  "
        "PL1 MODE    "
        "PL2 DEVICE  "
        "PL2 MODE    "
        "LANGUAGE    "
        "BACK        "
        
        "TAXA DE ATUALIZACAO  "
        "TAXA DE RENDERIZACAO "
        "ZOOM                 "
        "MUSICA               "
        "SONS                 "
        "DICAS                "
        "DISPOSITIVO PL1      "
        "CONFIGURACAO PL1     "
        "DISPOSITIVO PL2      "
        "CONFIGURACAO PL2     "
        "IDIOMA               "
        "VOLTAR               "
*/
        
        x = 2;
        y = 3;
        
        _op_renderText("<", x - 1, y+op->curOpt, 1);
        _op_renderText(">", x + tab - 2, y + op->curOpt, 1);
        
        i = 0;
        while (i < OPT_MAX) {
            switch (i) {
                case OPT_UFPS: {
                    _op_renderLang(op, TXT_UPS, x, y);
                    switch (op->ufps) {
                        case 60: _op_renderValueStatic("60", x, y); break;
                        case 90: _op_renderValueStatic("90", x, y); break;
                    }
                    _op_renderTextStatic("FPS", x + tab + 3, y);
                } break;
                case OPT_DFPS: {
                    _op_renderLang(op, TXT_DPS, x, y);
                    switch (op->dfps) {
                        case 30: _op_renderValueStatic("30", x, y); break;
                        case 60: _op_renderValueStatic("60", x, y); break;
                        case 90: _op_renderValueStatic("90", x, y); break;
                    }
                    _op_renderTextStatic("FPS", x + tab + 3, y);
                } break;
                case OPT_RES: {
                    _op_renderLang(op, TXT_ZOOM, x, y);
                    switch (op->res) {
                        case 0: _op_renderValueStatic("FULLSCREEN", x, y); break;
                        case 1: _op_renderValueStatic("X1", x, y); break;
                        case 2: _op_renderValueStatic("X2", x, y); break;
                        case 3: _op_renderValueStatic("X3", x, y); break;
                        case 4: _op_renderValueStatic("X4", x, y); break; }
                } break;
                case OPT_MUSIC: {
                    float volume;
                    
                    _op_renderLang(op, TXT_MUSIC, x, y);
                    
                    volume = audio_getVolume();
                         if (volume== 0)   _op_renderLang(op, TXT_MUTED, x + tab, y);
                    else if (volume== 10)  _op_renderValueStatic("10%", x, y);
                    else if (volume== 20)  _op_renderValueStatic("20%", x, y);
                    else if (volume== 30)  _op_renderValueStatic("30%", x, y);
                    else if (volume== 40)  _op_renderValueStatic("40%", x, y);
                    else if (volume== 50)  _op_renderValueStatic("50%", x, y);
                    else if (volume== 60)  _op_renderValueStatic("60%", x, y);
                    else if (volume== 70)  _op_renderValueStatic("70%", x, y);
                    else if (volume== 80)  _op_renderValueStatic("80%", x, y);
                    else if (volume== 90)  _op_renderValueStatic("90%", x, y);
                    else if (volume== 100) _op_renderValueStatic("100%", x, y);
                } break;
                case OPT_SFX: {
                    float sfx;
                    
                    _op_renderLang(op, TXT_SFX, x, y);
                    
                    sfx = sfx_getVolume();
                         if (sfx == 0)   _op_renderLang(op, TXT_MUTED, x + tab, y);
                    else if (sfx == 10)  _op_renderValueStatic("10%", x, y);
                    else if (sfx == 20)  _op_renderValueStatic("20%", x, y);
                    else if (sfx == 30)  _op_renderValueStatic("30%", x, y);
                    else if (sfx == 40)  _op_renderValueStatic("40%", x, y);
                    else if (sfx == 50)  _op_renderValueStatic("50%", x, y);
                    else if (sfx == 60)  _op_renderValueStatic("60%", x, y);
                    else if (sfx == 70)  _op_renderValueStatic("70%", x, y);
                    else if (sfx == 80)  _op_renderValueStatic("80%", x, y);
                    else if (sfx == 90)  _op_renderValueStatic("90%", x, y);
                    else if (sfx == 100) _op_renderValueStatic("100%", x, y);
                } break;
                case OPT_HINT: {
                    _op_renderLang(op, TXT_HINT, x, y);
                    if (op->hint)
                        _op_renderLang(op, TXT_ENABLED, x + tab, y);
                    else
                        _op_renderLang(op, TXT_DISABLED, x + tab, y);
                } break;
                case OPT_SPEEDRUN: {
                    _op_renderLang(op, TXT_SPEEDRUN, x, y);
                    if (op->speedrun)
                        _op_renderLang(op, TXT_ENABLED, x + tab, y);
                    else
                        _op_renderLang(op, TXT_DISABLED, x + tab, y);
                } break;
                case OPT_P1DEV: {
                    _op_renderLang(op, TXT_PL1DEV, x, y);
                    if (pl1 < CTR_PAD1_A)
                        _op_renderLang(op, TXT_KEYBOARD, x + tab, y);
                    else if (pl1 < CTR_PAD2_A)
                        _op_renderLang(op, TXT_GAMEPAD1, x + tab, y);
                    else
                        _op_renderLang(op, TXT_GAMEPAD2, x + tab, y);
                } break;
                case OPT_P1MODE: {
                    _op_renderLang(op, TXT_PL1MODE, x, y);
                    if (pl1 % 4 == 0)
                        _op_renderLang(op, TXT_TYPEA, x + tab, y);
                    else if (pl1 % 4 == 1)
                        _op_renderLang(op, TXT_TYPEB, x + tab, y);
                    else if (pl1 % 4 == 2)
                        _op_renderLang(op, TXT_TYPEC, x + tab, y);
                    else if (pl1 % 4 == 3)
                        _op_renderLang(op, TXT_TYPED, x + tab, y);
                } break;
                case OPT_P2DEV: {
                    _op_renderLang(op, TXT_PL2DEV, x, y);
                    if (pl2 < CTR_PAD1_A)
                        _op_renderLang(op, TXT_KEYBOARD, x + tab, y);
                    else if (pl2 < CTR_PAD2_A)
                        _op_renderLang(op, TXT_GAMEPAD1, x + tab, y);
                    else
                        _op_renderLang(op, TXT_GAMEPAD2, x + tab, y);
                } break;
                case OPT_P2MODE: {
                    _op_renderLang(op, TXT_PL2MODE, x, y);
                    if (pl2 % 4 == 0)
                        _op_renderLang(op, TXT_TYPEA, x + tab, y);
                    else if (pl2 % 4 == 1)
                        _op_renderLang(op, TXT_TYPEB, x + tab, y);
                    else if (pl2 % 4 == 2)
                        _op_renderLang(op, TXT_TYPEC, x + tab, y);
                    else if (pl2 % 4 == 3)
                        _op_renderLang(op, TXT_TYPED, x + tab, y);
                } break;
                case OPT_LANG: {
                    _op_renderLang(op, TXT_LANG, x, y);
                    if (op->lang == EN_US)
                        _op_renderValueStatic("ENGLISH", x, y);
                    else if (op->lang == PT_BR)
                        _op_renderValueStatic("PORTUGUES", x, y);
                } break;
                case OPT_BACK: {
                    _op_renderLang(op, TXT_BACK, x, y);
                } break;
            }
            y++;
            i++;
        }
        
        _op_renderTextStatic("PLAYER1", x+13, y);
        _op_renderTextStatic("PLAYER2", x+24, y);
        y++;
        
        _y = y;
        _op_renderLang(op, TXT_LEFT, x, _y++);
        _op_renderLang(op, TXT_RIGHT, x, _y++);
        _op_renderLang(op, TXT_UP, x, _y++);
        _op_renderLang(op, TXT_JUMP, x, _y++);
        _op_renderLang(op, TXT_ITEM, x, _y++);
        _op_renderLang(op, TXT_SWITCH, x, _y++);
        
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
            if (GFraMe_controller_max >= 2) {
                isDown = isDown || GFraMe_controllers[1].ly > 0.5;
                isDown = isDown || GFraMe_controllers[1].down;
            }
            isUp = GFraMe_keys.up;
            isUp = isUp || GFraMe_keys.w;
            if (GFraMe_controller_max >= 1) {
                isUp = isUp || GFraMe_controllers[0].ly < -0.5;
                isUp = isUp || GFraMe_controllers[0].up;
            }
            if (GFraMe_controller_max >= 2) {
                isUp = isUp || GFraMe_controllers[1].ly < -0.5;
                isUp = isUp || GFraMe_controllers[1].up;
            }
            isLeft = GFraMe_keys.left;
            isLeft = isLeft || GFraMe_keys.a;
            if (GFraMe_controller_max >= 1) {
                isLeft = isLeft || GFraMe_controllers[0].lx < -0.5;
                isLeft = isLeft || GFraMe_controllers[0].left;
            }
            if (GFraMe_controller_max >= 2) {
                isLeft = isLeft || GFraMe_controllers[1].lx < -0.5;
                isLeft = isLeft || GFraMe_controllers[1].left;
            }
            isRight = GFraMe_keys.right;
            isRight = isRight || GFraMe_keys.d;
            if (GFraMe_controller_max >= 1) {
                isRight = isRight || GFraMe_controllers[0].lx > 0.5;
                isRight = isRight || GFraMe_controllers[0].right;
            }
            if (GFraMe_controller_max >= 2) {
                isRight = isRight || GFraMe_controllers[1].lx > 0.5;
                isRight = isRight || GFraMe_controllers[1].right;
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
            else if (op->curOpt == OPT_HINT && (isLeft || isRight)) {
                op->hint = !op->hint;
                sfx_menuMove();
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_SPEEDRUN && (isLeft || isRight)) {
                op->speedrun = !op->speedrun;
                sfx_menuMove();
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_RES && (isLeft || isRight)) {
                // Exit fullscreen
                if (op->res == 0) {
                    GFraMe_screen_setWindowed();
                }
                // Modify the current resolution
                if (isLeft)
                    op->res--;
                else if (isRight)
                    op->res++;
                // Make sure it's a valid value
                if (op->res < 0)
                    op->res = 4;
                else if (op->res > 4)
                    op->res = 0;
                // Switch the resolution... in real time! (yep, a great idea...)
                if (op->res != 0) {
                    GFraMe_ret rv;
                    
                    rv = GFraMe_screen_set_window_size(SCR_W*op->res, SCR_H*op->res);
                    if (rv == GFraMe_ret_ok)
					    GFraMe_screen_set_pixel_perfect(0, 1);
                }
                else {
                    GFraMe_ret rv;
                    
                    rv = GFraMe_screen_setFullscreen();
                    if (rv == GFraMe_ret_ok)
                        GFraMe_screen_set_pixel_perfect(0, 1);
                }
                // Set the delay on the cursor
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_MUSIC && (isLeft || isRight)) {
                float v;
                
                v = audio_getVolume();
                if (isLeft)  v -= 10;
                if (isRight) v += 10;
                audio_setVolume(v);
                
                sfx_menuMove();
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_SFX && (isLeft || isRight)) {
                float v;
                
                v = sfx_getVolume();
                if (isLeft)  v -= 10;
                if (isRight) v += 10;
                sfx_setVolume(v);
                
                sfx_menuMove();
                if (!op->firstPress)
                    op->lastPressedTime += 300;
                else
                    op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_UFPS && (isLeft || isRight)) {
                if (isLeft) op->ufps -= 30;
                else if (isRight) op->ufps += 30;
                // Clamp the value
                if (op->ufps < 60) op->ufps = 90;
                else if (op->ufps > 90) op->ufps = 60;
                // Avoid multi presses
                if (!op->firstPress) op->lastPressedTime += 300;
                else op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_DFPS && (isLeft || isRight)) {
                if (isLeft) op->dfps -= 30;
                else if (isRight) op->dfps += 30;
                // Clamp the value
                if (op->dfps < 30) op->dfps = 90;
                else if (op->dfps > 90) op->dfps = 30;
                // Avoid multi presses
                if (!op->firstPress) op->lastPressedTime += 300;
                else op->lastPressedTime += 100;
                op->firstPress = 1;
            }
            else if (op->curOpt == OPT_LANG && (isLeft || isRight)) {
                if (op->lang == EN_US) {
                    op->lang = PT_BR;
                }
                else if (op->lang == PT_BR) {
                    op->lang = EN_US;
                }
                // Avoid multi presses
                if (!op->firstPress) op->lastPressedTime += 300;
                else op->lastPressedTime += 100;
                op->firstPress = 1;
            }
        }
        isEnter = GFraMe_keys.enter;
        isEnter = isEnter || GFraMe_keys.z;
        isEnter = isEnter || GFraMe_keys.space;
        if (GFraMe_controller_max > 0) {
            isEnter = isEnter || GFraMe_controllers[0].a;
            isEnter = isEnter || GFraMe_controllers[0].start;
        }
        if (GFraMe_controller_max > 1) {
            isEnter = isEnter || GFraMe_controllers[1].a;
            isEnter = isEnter || GFraMe_controllers[1].start;
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
                (event.type == SDL_CONTROLLERAXISMOTION
                && GFraMe_controllers[event.caxis.which].ly < 0.3
                && GFraMe_controllers[event.caxis.which].ly > -0.3
                && GFraMe_controllers[event.caxis.which].lx < 0.3
                && GFraMe_controllers[event.caxis.which].lx > -0.3
               )) {
                op->firstPress = 0;
                op->lastPressedTime = 0;
            }
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
}

/**
 * Render a localized text to the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 */
static void _op_renderLang(struct stOptions *op, int text, int X, int Y) {
    char *pText;
    int len;
    
    switch (text) {
#define selectLang(textEN, textPT) \
  do { \
    if (op->lang == EN_US) { pText = textEN; len = sizeof(textEN) - 1; } \
    else if (op->lang == PT_BR) { pText = textPT; len = sizeof(textPT) - 1; } \
  } while (0)
        case TXT_OPTS:     selectLang("OPTIONS",     "OPCOES"); break;
        case TXT_UPS:      selectLang("UPDATE RATE", "TAXA DE ATUALIZACAO"); break;
        case TXT_DPS:      selectLang("DRAW RATE",   "TAXA DE RENDERIZACAO"); break;
        case TXT_ZOOM:     selectLang("ZOOM",        "ZOOM"); break;
        case TXT_MUSIC:    selectLang("MUSIC",       "MUSICA"); break;
        case TXT_SFX:      selectLang("SFX",         "SONS"); break;
        case TXT_HINT:     selectLang("HINT",        "DICAS"); break;
        case TXT_SPEEDRUN: selectLang("SPEEDRUN",    "MODO 'SPEEDRUN'"); break;
        case TXT_PL1DEV:   selectLang("PL1 DEVICE",  "DISPOSITIVO PL1"); break;
        case TXT_PL1MODE:  selectLang("PL1 MODE",    "CONFIGURACAO PL1"); break;
        case TXT_PL2DEV:   selectLang("PL2 DEVICE",  "DISPOSITIVO PL2"); break;
        case TXT_PL2MODE:  selectLang("PL2 MODE",    "CONFIGURACAO PL2"); break;
        case TXT_LANG:     selectLang("LANGUAGE",    "IDIOMA"); break;
        case TXT_BACK:     selectLang("BACK",        "VOLTAR"); break;
        case TXT_ENABLED:  selectLang("ENABLED",     "HABILITADO"); break;
        case TXT_DISABLED: selectLang("DISABLED",    "DESABILITADO"); break;
        case TXT_MUTED:    selectLang("MUTED",       "MUDO"); break;
        case TXT_KEYBOARD: selectLang("KEYBOARD",    "TECLADO"); break;
        case TXT_GAMEPAD1: selectLang("GAMEPAD 1",   "CONTROLE 1"); break;
        case TXT_GAMEPAD2: selectLang("GAMEPAD 2",   "CONTROLE 2"); break;
        case TXT_TYPEA:    selectLang("TYPE A",      "MODO A"); break;
        case TXT_TYPEB:    selectLang("TYPE B",      "MODO B"); break;
        case TXT_TYPEC:    selectLang("TYPE C",      "MODO C"); break;
        case TXT_TYPED:    selectLang("TYPE D",      "MODO D"); break;
        case TXT_LEFT:     selectLang("LEFT", "ESQUERDA"); break;
        case TXT_RIGHT:    selectLang("RIGHT", "DIREITA"); break;
        case TXT_UP:       selectLang("UP", "CIMA"); break;
        case TXT_JUMP:     selectLang("JUMP", "SALTAR"); break;
        case TXT_ITEM:     selectLang("USE ITEM", "USAR ITEM"); break;
        case TXT_SWITCH:   selectLang("SWITCH ITEM", "TROCAR ITEM"); break;
#undef selectLang
        default: return;
    }
    
    _op_renderText(pText, X, Y, len);
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
    x = X * 8;
    y = Y * 10;
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

