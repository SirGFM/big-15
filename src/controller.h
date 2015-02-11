/**
 * @file src/controller.h
 * 
 * Controller abstraction so it doesn't matter if it's the player 1, 2 or if
 * they are playing on a controller or keyboard
 */
#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

#include "types.h"

/**
 * Change the current input scheme
 * 
 * @param mode The new controle mode
 */
void ctr_setMode(ctr_mode mode);

/**
 * Checks if the left button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_left(int ID);

/**
 * Checks if the right button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_right(int ID);

/**
 * Checks if the action button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_action(int ID);

/**
 * Checks if the jump button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_jump(int ID);

/**
 * Checks if the item button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_item(int ID);

/**
 * Checks if the switchItem button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_switchItem(int ID);

#endif

