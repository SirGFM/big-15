/**
 * @file src/timer.c
 * 
 * Accumulate how much time has passed since the start of the game
 */
#ifndef __TIMER__H_
#define __TIMER__H_

/**
 * Initialize the timer
 * 
 * @param iniTime previously accumulated time
 */
void timer_init(int iniTime);

/**
 * Update for how long the time has been running
 */
void timer_update();

/**
 * Render the timer to the screen
 */
void timer_draw();

/**
 * Return the time in a printable manner
 * 
 * @param str String where the time is returned; Must be at least 12 characters
 *            long
 */
void timer_getString(char *str);

/**
 * Returns the current time
 * 
 * @return The current time
 */
int timer_getTime();

/**
 * Make the timer stop
 */
void timer_stop();

#endif

