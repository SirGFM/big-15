/**
 * @file src/timer.c
 * 
 * Accumulate how much time has passed since the start of the game
 */
#include <GFraMe/GFraMe_spriteset.h>

#include <SDL2/SDL_timer.h>

#include "global.h"
#include "timer.h"

static int _curTime;
static int _lastTime;
static int _running;

/**
 * Initialize the timer
 * 
 * @param iniTime previously accumulated time
 */
void timer_init(int iniTime) {
    _curTime = iniTime;
    _lastTime = SDL_GetTicks();
    _running = 1;
}

/**
 * Update for how long the time has been running
 */
void timer_update() {
    if (_running) {
        int delta, t;
        
        t = SDL_GetTicks();
        delta = t - _lastTime;
        _curTime += delta;
        _lastTime = t;
        // Cap the timer at the biggest printable time (it's never gonna
        //happen... no one will play the game for 99 hours, c'mon...
        if (_curTime >= 359999999) {
            _curTime = 359999999;
            _running = 0;
        }
    }
}

/**
 * Render the timer to the screen
 */
void timer_draw() {
    char str[12];
    int i, x, y;
    
    timer_getString(str);
    
    x = 128;
    y = 8;
    i = 0;
    while (i < 12) {
        GFraMe_spriteset_draw(gl_sset8x8, str[i], x, y, 0/*flip*/);
        i++;
        x += 8;
    }
}

/**
 * Returns a 2 digits integer as a printable string
 * 
 * @param val The integer
 * @param str The returned string
 */
static void _timer_int2str(int val, char *str) {
    str[0] = (char)(val / 10) + '0' - '!';
    str[1] = (char)(val % 10) + '0' - '!';
}

/**
 * Return the time in a printable manner
 * 
 * @param str String where the time is returned; Must be at least 12 characters
 *            long
 */
void timer_getString(char *str) {
    int ms, s, min, h;
    
    ms = _curTime % 1000;
    s = (_curTime / 1000) % 60;
    min = (_curTime / 60000) % 60;
    h = (_curTime / 3600000);
    
    _timer_int2str(h  , str);     str[2] = ':' - '!';
    _timer_int2str(min, str + 3); str[5] = ':' - '!';
    _timer_int2str(s  , str + 6); str[8] = '.' - '!';
    str[9]  = (char)(ms / 100) + '0' - '!';
    str[10] = (char)((ms / 10) % 10) + '0' - '!';
    str[11] = (char)(ms % 10) + '0' - '!';
}

/**
 * Returns the current time
 * 
 * @return The current time
 */
int timer_getTime() {
    return _curTime;
}

/**
 * Make the timer stop
 */
void timer_stop() {
    _running = 0;
}

