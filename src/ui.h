/**
 * @file src/ui.h
 * 
 * Simple UI component. Checks the game's state to display it.
 */
#ifndef __UI_H_
#define __UI_H_

#include <GFraMe/GFraMe_error.h>

/**
 * Initialize the ui
 * 
 * @return GFraMe error code
 */
GFraMe_ret ui_init();

/**
 * Clean up the ui
 */
void ui_clean();

/**
 * Updates the ui
 */
void ui_update(int ms);

/**
 * Draw the ui to the screen
 */
void ui_draw();

#endif

