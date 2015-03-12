/**
 * @file src/transition.h
 * 
 * Fade out/in the screen
 */
#ifndef __TRANSITION_H_
#define __TRANSITION_H_

typedef enum {
    TR_COMPLETE,
    TR_INCOMPLETE
} tr_status;

/**
 * Setup the transition to fade in
 */
void transition_initFadeIn();

/**
 * Setup the transition to fade out
 */
void transition_initFadeOut();

/**
 * Update the fade out animation
 * 
 * @param ms Time, in milliseconds, elapsed since last frame
 * @return TR_COMPLETE or TR_INCOMPLETE
 */
tr_status transition_fadeOut(int ms);

/**
 * Update the fade in animation
 * 
 * @param ms Time, in milliseconds, elapsed since last frame
 * @return TR_COMPLETE or TR_INCOMPLETE
 */
tr_status transition_fadeIn(int ms);

/**
 * Draw the transition
 */
void transition_draw();

/**
 * Draw an overlay
 */
void transition_drawPause();

#endif

