/**
 * @file src/menustate.c
 * 
 * Menu state
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_keys.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "audio.h"
#include "global.h"
#include "globalVar.h"
#include "state.h"
#include "timer.h"
#include "transition.h"
#include "types.h"

struct stCredits {
    struct stateHandler hnd;
    char *text;
    int maxLen;
    int len;
    int textTimer;
    int frame;
    int running;
    int textX;
    int textY;
    int iconX;
    int iconY;
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
static void _cr_renderText(char *text, int X, int Y, int l);

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static void cr_init(struct stCredits *cr, char *text, int maxLen, int X, int Y);
/**
 * Draw the current frame
 */
static void cr_draw(struct stCredits *cr);
/**
 * Update the current frame, as many times as it's accumulated
 */
static void cr_update(struct stCredits *cr);
/**
 * Handle every event
 */
static void cr_event(struct stCredits *cr);

char _textEN[] =
"               GAME OVER    \n"
"\n"
"    THANK YOU FOR PLAYING THIS GAME    \n"
"\n"
"\n"
"\n"
"             DEATH COUNT:    \n"
"\n"
"           PLAYER 1:  00000   \n"
"           PLAYER 2:  00000   \n"
"\n"
"\n"
"              TOTAL TIME:        \n"
"\n"
"              00:00:00.000";
char _textPT[] =
"              FIM DE JOGO   \n"
"\n"
"          OBRIGADO POR JOGAR!          \n"
"\n"
"\n"
"\n"
"           NUMERO DE MORTES: \n"
"\n"
"           JOGADOR 1: 00000   \n"
"           JOGADOR 2: 00000   \n"
"\n"
"\n"
"              TEMPO TOTAL:       \n"
"\n"
"              00:00:00.000";

int credits_setup(void *self) {
    struct stCredits *cr = (struct stCredits*)self;
    char *text;
    int i, j, len, val;

    if (gl_lang == PT_BR) {
        text = _textPT;
        len = (int)sizeof(_textEN);
    }
    else /* if (gl_lang == EN_US) */ {
        text = _textEN;
        len = (int)sizeof(_textEN);
    }
    cr_init(cr, text, len - 1, 0, 7);

    i = 0;
    // Search for the first '0'
    while (text[i] != '0' && i < len) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < len) i++;
    j = i;
    val = gv_getValue(PL1_DEATH);
    // Print the player 1 death count
    while (val > 0) {
        i--;
        text[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    i = j+1;
    // Search for the first '0' for player 2
    while (text[i] != '0' && i < len) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < len) i++;
    val = gv_getValue(PL2_DEATH);
    // Print the player 2 death count
    while (val > 0) {
        i--;
        text[i] = (char)('0' + (val % 10));
        val /= 10;
    }

    i = len - 1 - 12;
    timer_getString(text + i);
    while (i < len - 1) {
        text[i] += '!';
        i++;
    }

    GFraMe_event_init(GAME_UFPS, GAME_DFPS);

    return 0;
}

int credits_isRunning(void *self) {
    struct stCredits *cr = (struct stCredits*)self;

    return cr->running;
}

void credits_update(void *self) {
    struct stCredits *cr = (struct stCredits*)self;

    cr_event(cr);
    cr_update(cr);
    cr_draw(cr);
}

int credits_nextState(void *self) {
    return MENUSTATE;
}

void credits_release(void *self) {
    return;
}

static struct stCredits global_cr;
void *credits_getHnd() {
    struct stateHandler *hnd = &(global_cr.hnd);

    memset(&global_cr, 0x0, sizeof(global_cr));
    hnd->setup = &credits_setup;
    hnd->isRunning = &credits_isRunning;
    hnd->update = &credits_update;
    hnd->nextState = &credits_nextState;
    hnd->release = &credits_release;

    return &global_cr;
}


/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 */
state credits() {
    char *text;
    char textEN[] =
"               GAME OVER    \n"
"\n"
"    THANK YOU FOR PLAYING THIS GAME    \n"
"\n"
"\n"
"\n"
"             DEATH COUNT:    \n"
"\n"
"           PLAYER 1:  00000   \n"
"           PLAYER 2:  00000   \n"
"\n"
"\n"
"              TOTAL TIME:        \n"
"\n"
"              00:00:00.000";
    char textPT[] = 
"              FIM DE JOGO   \n"
"\n"
"          OBRIGADO POR JOGAR!          \n"
"\n"
"\n"
"\n"
"           NUMERO DE MORTES: \n"
"\n"
"           JOGADOR 1: 00000   \n"
"           JOGADOR 2: 00000   \n"
"\n"
"\n"
"              TEMPO TOTAL:       \n"
"\n"
"              00:00:00.000";
    int i, j, len, val;
    state ret;
    struct stCredits cr;
    
    ret = -1;
    if (gl_lang == PT_BR) {
        text = textPT;
        len = (int)sizeof(textEN);
    }
    else /* if (gl_lang == EN_US) */ {
        text = textEN;
        len = (int)sizeof(textEN);
    }
    cr_init(&cr, text, len - 1, 0, 7);
    
    i = 0;
    // Search for the first '0'
    while (text[i] != '0' && i < len) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < len) i++;
    j = i;
    val = gv_getValue(PL1_DEATH);
    // Print the player 1 death count
    while (val > 0) {
        i--;
        text[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    i = j+1;
    // Search for the first '0' for player 2
    while (text[i] != '0' && i < len) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < len) i++;
    val = gv_getValue(PL2_DEATH);
    // Print the player 2 death count
    while (val > 0) {
        i--;
        text[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    
    i = len - 1 - 12;
    timer_getString(text + i);
    while (i < len - 1) {
        text[i] += '!';
        i++;
    }
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    while (gl_running && cr.running) {
        cr_event(&cr);
        cr_update(&cr);
        cr_draw(&cr);
    }
    
    ret = MENUSTATE;
    return ret;
}

/**
 * Initialize the menustate
 * 
 * @return GFraMe error code
 */
static void cr_init(struct stCredits *cr, char *text, int maxLen, int X, int Y) {
    cr->maxLen = maxLen;
    cr->len = 0;
    cr->textTimer = 0;
    cr->frame = 34;
    cr->running = 1;
    cr->text = text;
    cr->textX = X;
    cr->textY = Y;
    cr->iconX = (40-4-1)*8;
    cr->iconY = (30-4-1)*8;
}

/**
 * Draw the current frame
 */
static void cr_draw(struct stCredits *cr) {
    GFraMe_event_draw_begin();
        // Draw the text
        _cr_renderText(cr->text, cr->textX, cr->textY, cr->len);
        // Draw the player's icon
        GFraMe_spriteset_draw(gl_sset32x32, cr->frame, cr->iconX, cr->iconY, 0);
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static void cr_update(struct stCredits *cr) {
    GFraMe_event_update_begin();
        if (cr->textTimer > 0)
            cr->textTimer -= GFraMe_event_elapsed;
        else {
            cr->len++;
            if (cr->len == cr->maxLen) {
                cr->textTimer += 3750;
                cr->frame = 39; // Set smile+wink
            }
            else if (cr->len > cr->maxLen) {
                cr->running = 0;
            }
            else {
                int c;
                
                cr->textTimer += 40;
                c = cr->text[cr->len - 1];
                if (c != ' ' && c != '\n') {
                    sfx_text();
                    if (cr->frame == 34)
                        cr->frame = 35; // Set closed mouth
                    else {
                        // Set open mouth frame
                        switch (cr->len % 4) {
                            case 0:
                            case 2:
                                cr->frame = 35; break;
                            case 1:
                                cr->frame = 36; break;
                            case 3:
                                cr->frame = 37; break;
                        }
                    }
                }
                else {
                    if (cr->frame != 35 && cr->frame != 34)
                        cr->frame = 35; // Set closed mouth
                    else
                        cr->frame = 34; // Set smile
                }
            }
        }
    GFraMe_event_update_end();
}

/**
 * Handle every event
 */
static void cr_event(struct stCredits *cr) {
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_key_down();
        GFraMe_event_on_key_up();
        GFraMe_event_on_controller();
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
static void _cr_renderText(char *text, int X, int Y, int l) {
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

