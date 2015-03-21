/**
 * @file src/textwindow.c
 * 
 * Module to write text into a window
 */
#include <GFraMe/GFraMe_spriteset.h>

#include "audio.h"
#include "global.h"
#include "textwindow.h"

static char *_text;
static int _textLen;
static int _len;
static int _time;
static int _x;
static int _y;
static int _w;
static int _h;
static int _finished;

/**
 * Initialize a text window
 * 
 * @param x Window's horizontal position (in tiles)
 * @param y Window's vertical position (in tiles)
 * @param w Window's width (in tiles)
 * @param h Window's height (in tiles)
 * @param text Text (the pointer will be copied)
 * @param textLen Text's length
 */
void textWnd_init(int x, int y, int w, int h, char *text, int textLen) {
    _x = x;
    _y = y;
    _w = w;
    _h = h;
    _text = text;
    _textLen = textLen;
    _len = 0;
    _time = 0;
    _finished = 0;
}

/**
 * Update's the text
 * 
 * @param ms Time elapsed (in ms)
 */
void textWnd_update(int ms) {
    if (_time <= 0) {
        if (_len < _textLen) {
            char c;
            
            _len++;
            _time += 50;
            
            c = _text[_len - 1];
            if (c != ' ' && c != '\n')
                sfx_text();
        }
        else if (!_finished) {
            _time += 1500;
            _finished = 1;
        }
    }
    else
        _time -= ms;
}

/**
 * Check whether the text was completely written
 * 
 * @return 1 for true, 0 for false
 */
int textWnd_didFinish() {
    return _finished && _time <= 0;
}

/**
 * Render the text window
 */
void textWnd_draw() {
    int i, x, y, w, h;
    
    y = _y * 8;
    w = (_w + _x) * 8;
    h = (_h + _y) * 8;
    // Render the window
    while (y < h) {
        x = _x * 8;
        while (x < w) {
            int tile;
           
            // Get the proper tile for the window
            if (x == _x*8 && y == _y*8)
                tile = 86;
            else if (x == _x*8 && y == h - 8)
                tile = 150;
            else if (x == _x*8)
                tile = 118;
            else if (x == w - 8 && y == _y*8)
                tile = 88;
            else if (x == w - 8 && y == h - 8)
                tile = 152;
            else if (x == w - 8)
                tile = 120;
            else if (y == _y*8)
                tile = 87;
            else if (y == h - 8)
                tile = 151;
            else
                tile = 119;
            
            GFraMe_spriteset_draw(gl_sset8x8, tile, x, y, 0/*flipped*/);
            
            x += 8;
        }
        y += 8;
    }
    // Render the text
    i = 0;
    x = (_x + 1) * 8;
    y = (_y + 1) * 8;
    w -= 16;
    while (i < _len) {
        char c;
        
        c = _text[i];
        if (c != ' ' && c != '\n')
            GFraMe_spriteset_draw(gl_sset8x8, c-'!', x, y, 0/*flipped*/);
        
        i++;
        x += 8;
        if (x >= w || c == '\n') {
            x = (_x + 1) * 8;
            y += 8;
        }
    }
}

