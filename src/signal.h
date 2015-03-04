/**
 * @file src/signal.c
 * 
 * Signal item
 */
#ifndef __SIGNAL_H_
#define __SIGNAL_H_

/**
 * Initialize this submodule
 */
void signal_init();

/**
 * Setup a new signal (and release the previous)
 * 
 * @param cx Horizontal position
 * @param cy Vertical position
 */
void signal_setPos(int cx, int cy);

/**
 * Stop the signal animation
 */
void signal_release();

/**
 * Updates the submodule
 * 
 * @param ms Time elapse since the previous frame, in milliseconds
 */
void signal_update(int ms);

/**
 * Draw the submodule
 */
void signal_draw();

#endif

