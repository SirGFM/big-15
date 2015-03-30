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
#include "timer.h"
#include "transition.h"
#include "types.h"

struct stCredits {
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

/**
 * Menustate implementation. Must initialize it, run the loop and clean it up
 */
state credits() {
    char text[] =
"               GAME OVER    \n"
"\n"
"    THANK YOU FOR PLAYING THIS GAME    \n"
"\n"
"\n"
"\n"
"             DEATH COUNT:    \n"
"\n"
"            PLAYER1: 00000    \n"
"            PLAYER2: 00000    \n"
"\n"
"\n"
"              TOTAL TIME:        \n"
"\n"
"              00:00:00.000";
    int i, j, val;
    state ret;
    struct stCredits cr;
    
    ret = -1;
    cr_init(&cr, text, (int)sizeof(text) - 1, 0, 7);
    
    i = 0;
    // Search for the first '0'
    while (text[i] != '0' && i < sizeof(text)) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < sizeof(text)) i++;
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
    while (text[i] != '0' && i < sizeof(text)) i++;
    // Go to the last '0'
    while (text[i] == '0' && i < sizeof(text)) i++;
    val = gv_getValue(PL2_DEATH);
    // Print the player 2 death count
    while (val > 0) {
        i--;
        text[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    
    i = sizeof(text) - 1 - 12;
    timer_getString(text + i);
    while (i < sizeof(text) - 1) {
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
        int x, y;
        
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

