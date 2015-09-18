/**
 * @file src/demo.c
 * 
 * Play a little demo (only text on a black screen, actually)
 */
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_event.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>

#include "audio.h"
#include "demo.h"
#include "global.h"
#include "types.h"

// Initialize variables used by the event module
GFraMe_event_setup();

struct stDemo {
    char *text;
    int textLen;
    int textMaxLen;
    int textX;
    int textY;
    char *text1;
    int text1Size;
    char *text2;
    int text2Size;
    char *text3;
    int text3Size;
    int time;
};

/**
 * Draw the current frame
 */
static void dm_draw(struct stDemo *dm);
/**
 * Update the current frame, as many times as it's accumulated
 */
static int dm_update(struct stDemo *dm);
/**
 * Handle every event
 */
static int dm_event();
/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _dm_renderText(char *text, int X, int Y, int l);

/**
 * Play the demo
 * 
 * @return The next state
 */
state demo() {
    char text1[] =
"IN THE DISTANT FUTURE, A MONSTER HAS\n"
"BEEN WREAKING HAVOC THROUGH THE\n"
"GALAXY.          \n"
"NO KNOWN WEAPON SEEMS TO HURT IT.         \n"
"ALL BEINGS LIVE IN FEAR OF THE\n"
"CREATURE'S NEXT ATTACK.\n";
    char text2[] =
"BUT TWO ADVENTURERS, FEARING NOTHING\n"
"BUT BOREDOM AND THE LACK OF DANGER\n"
"DECIDED TO FIGHT BACK!\n";
    char text3[] =
"THEY WENT BACK IN TIME AND INTO THE\n"
"MONSTER'S LAIR, TO DESTROY IT BEFORE\n"
"IT COULD BECOME A THREAT!\n";
    int run;
    struct stDemo dm;
    
    dm.time = 0;
    dm.text = 0;
    dm.textLen = 0;
    dm.textMaxLen = 0;
    dm.textX = 8;
    dm.textY = 96;
    dm.text1 = text1;
    dm.text1Size = sizeof(text1);
    dm.text2 = text2;
    dm.text2Size = sizeof(text2);
    dm.text3 = text3;
    dm.text3Size = sizeof(text3);
    
    GFraMe_event_init(GAME_UFPS, GAME_DFPS);
    
    run = 1;
    while (gl_running && run) {
        run = run && !dm_event();
        run = run && !dm_update(&dm);
        dm_draw(&dm);
    }
    
    return MENUSTATE;
}

/**
 * Draw the current frame
 */
static void dm_draw(struct stDemo *dm) {
    GFraMe_event_draw_begin();
        // Draw the text
        if (dm->text)
            _dm_renderText(dm->text, dm->textX, dm->textY, dm->textLen);
    GFraMe_event_draw_end();
}

/**
 * Update the current frame, as many times as it's accumulated
 */
static int dm_update(struct stDemo *dm) {
    int ret;
    
    ret = 0;
    
    GFraMe_event_update_begin();
        if (dm->time > 0)
            dm->time -= GFraMe_event_elapsed;
        else if (dm->time <= 0) {
            // Initialize the text
            if (!dm->text) {
                dm->text = dm->text1;
                dm->textMaxLen = dm->text1Size;
                dm->textLen = 0;
            }
            // Change the text, if finished
            else if (dm->textMaxLen == dm->textLen) {
                if (dm->text == dm->text1) {
                    dm->text = dm->text2;
                    dm->textMaxLen = dm->text2Size;
                    dm->textLen = 0;
                }
                else if (dm->text == dm->text2) {
                    dm->text = dm->text3;
                    dm->textMaxLen = dm->text3Size;
                    dm->textLen = 0;
                }
                else
                    ret = 1;
            }
            // Otherwise, write another character
            else {
                dm->textLen++;
                if (dm->textLen == dm->textMaxLen)
                    dm->time += 1500;
                else {
                    int c;
                    
                    dm->time += 50;
                    c = dm->text[dm->textLen - 1];
                    if (c != ' ' && c != '\n')
                        sfx_text();
                }
            }
        }
    GFraMe_event_update_end();
    
    return ret;
}

/**
 * Handle every event
 * 
 * @return Whethe any button was pressed
 */
static int dm_event() {
    int ret;
    
    ret = 0;
    
    GFraMe_event_begin();
        GFraMe_event_on_timer();
        GFraMe_event_on_key_down();
        GFraMe_event_on_key_up();
            ret = 1;
        GFraMe_event_on_controller();
            if (event.type == SDL_CONTROLLERBUTTONUP)
                ret = 1;
        GFraMe_event_on_quit();
            gl_running = 0;
    GFraMe_event_end();
    
    return ret;
}

/**
 * Render some text into the screen
 * 
 * @param text The text
 * @param X Horizontal position
 * @param Y Vertical position
 * @param l Text length
 */
static void _dm_renderText(char *text, int X, int Y, int l) {
    int i, x, y;
    
    i = 0;
    x = X;
    y = Y;
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

