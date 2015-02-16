/**
 * @file src/transition.c
 * 
 * Fade out/in the screen
 */
#include <GFraMe/GFraMe_spriteset.h>

#include "global.h"
#include "transition.h"

#define DATA_ROW SCR_H / 8
#define DATA_COL SCR_W / 8
#define DATA_LEN DATA_ROW * DATA_COL
static unsigned char data[DATA_LEN];
static int row0;
static int row1;
static int row2;
static int row3;
static int row4;
static int time;

static void _transition_setData(unsigned char val) {
    int i;
    
    i = 0;
    while (i < DATA_LEN) {
        data[i] = val;
        i++;
    }
}

static void _transition_setRow(int row, unsigned char val) {
    int i, off;
    
    if (row >= DATA_ROW)
        return;
    
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
    
    time = 0;
    
    row0 = 0;
    row1 = 0;
    row2 = 0;
    row3 = 0;
    row4 = 0;
}

/**
 * Setup the transition to fade out
 */
void transition_initFadeOut() {
    _transition_setData(250);
    
    time = 0;
    
    row0 = 0;
    row1 = 0;
    row2 = 0;
    row3 = 0;
    row4 = 0;
}

/**
 * Update the fade out animation
 * 
 * @param ms Time, in milliseconds, elapsed since last frame
 * @return TR_COMPLETE or TR_INCOMPLETE
 */
tr_status transition_fadeOut(int ms) {
    time += ms;
    if (time >= 100) {
        if (row3 != 0)
            _transition_setRow(row4++, 255);
        if (row2 != 0)
            _transition_setRow(row3++, 254);
        if (row1 != 0)
            _transition_setRow(row2++, 253);
        if (row0 != 0)
            _transition_setRow(row1++, 252);
        _transition_setRow(row0++, 251);
        
        time -= 100;
    }
    
    if (row4 == DATA_ROW)
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
    time += ms;
    if (time >= 100) {
        if (row3 != 0)
            _transition_setRow(row4++, 250);
        if (row2 != 0)
            _transition_setRow(row3++, 251);
        if (row1 != 0)
            _transition_setRow(row2++, 252);
        if (row0 != 0)
            _transition_setRow(row1++, 253);
        _transition_setRow(row0++, 254);
        
        time -= 100;
    }
    
    if (row4 == DATA_ROW)
        return TR_COMPLETE;
    return TR_INCOMPLETE;
}

/**
 * Draw the transition
 */
void transition_draw() {
    int i, x, y;
    
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

