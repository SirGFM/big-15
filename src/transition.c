/**
 * @file src/transition.c
 * 
 * Fade out/in the screen
 */
#include <GFraMe/GFraMe_spriteset.h>

#include "global.h"
#include "transition.h"

/** How long a 'row-transition' should take */
#define DELAY 50
/** How many rows should be rendered before the next type */
#define ROW_DELAY 2
/** Data width in tiles */
#define DATA_ROW SCR_H / 8
/** Data height in tiles */
#define DATA_COL SCR_W / 8
/** Data length in tiles */
#define DATA_LEN DATA_ROW * DATA_COL
#define NUM_ROWS 6
/** Transition tilemap */
static unsigned char data[DATA_LEN];
/** stuff */
static int rows[NUM_ROWS];
/** How long has elapsed from the previous 'transition-step' */
static int time;
/** How many rows are being rendered */
static int rowsUsed;

/**
 * Clear up the transition data
 * 
 * @param val Value that should be set to the whole data
 */
static void _transition_setData(unsigned char val) {
    int i;
    
    // Set the data
    i = 0;
    while (i < DATA_LEN) {
        data[i] = val;
        i++;
    }
    
    // Clean up the helper variables
    time = 0;
    i = 0;
    while (i < NUM_ROWS)
        rows[i++] = 0;
    rowsUsed = 1;
}

/**
 * Assign a value to a complete row
 * 
 * @param row Row that should be modified
 * @param val Value to be set
 */
static void _transition_setRow(int row, unsigned char val) {
    int i, off;
    
    // Check that the row is still in bounds
    if (row >= DATA_ROW)
        return;
    
    // Just.. set it all
    off = row*DATA_COL;
    i = 0;
    while (i < DATA_COL) {
        data[off+i] = val;
        i++;
    }
}

/**
 * Setup the transition to fade in
 */
void transition_initFadeIn() {
    _transition_setData(255);
}

/**
 * Setup the transition to fade out
 */
void transition_initFadeOut() {
    _transition_setData(249);
}

/**
 * Update the fade out animation
 * 
 * @param ms Time, in milliseconds, elapsed since last frame
 * @return TR_COMPLETE or TR_INCOMPLETE
 */
tr_status transition_fadeOut(int ms) {
    // Update the time and check if a new row should be rendered
    time += ms;
    while (time >= DELAY) {
        int i;
        unsigned char val;
        
        val = 250;
        i = 0;
        while (i < rowsUsed) {
            _transition_setRow(rows[i], val);
            rows[i] += 1;
            i++;
            val++;
        }
        // Add a new type of row after ROW_DELAY of the previous one
        if (rowsUsed < NUM_ROWS && rows[i-1] >= ROW_DELAY)
            rowsUsed++;
        
        time -= DELAY;
    }
    
    // Check that the animation was complete
    if (rows[NUM_ROWS-1] == DATA_ROW)
        return TR_COMPLETE;
    return TR_INCOMPLETE;
}

/**
 * Update the fade in animation
 * 
 * @param ms Time, in milliseconds, elapsed since last frame
 * @return TR_COMPLETE or TR_INCOMPLETE
 */
tr_status transition_fadeIn(int ms) {
    // Update the time and check if a new row should be rendered
    time += ms;
    while (time >= DELAY) {
        int i;
        unsigned char val;
        
        val = 254;
        i = 0;
        while (i < rowsUsed) {
            _transition_setRow(rows[i], val);
            rows[i] += 1;
            i++;
            val--;
        }
        // Add a new type of row after ROW_DELAY of the previous one
        if (rowsUsed < NUM_ROWS && rows[i-1] >= ROW_DELAY)
            rowsUsed++;
        
        time -= DELAY;
    }
    
    // Check that the animation was complete
    if (rows[NUM_ROWS-1] == DATA_ROW)
        return TR_COMPLETE;
    return TR_INCOMPLETE;
}

/**
 * Draw the transition
 */
void transition_draw() {
    int i, x, y;
    
    // Render it tile-by-tile
    i = 0;
    x = 0;
    while (i < DATA_LEN) {
        
        GFraMe_spriteset_draw
            (
             gl_sset8x8,
             data[i],
             x,
             y,
             0 // flipped
            );
        
        x += 8;
        if (x >= SCR_W) {
            x = 0;
            y += 8;
        }
        i++;
    }
}

