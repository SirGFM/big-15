/**
 * @file src/controller.h
 * 
 * Controller abstraction so it doesn't matter if it's the player 1, 2 or if
 * they are playing on a controller or keyboard
 */
#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

typedef enum {
    CTR_KEYS_A,
    CTR_KEYS_B,
    CTR_KEYS_C,
    CTR_KEYS_D,
    CTR_PAD1_A,
    CTR_PAD1_B,
    CTR_PAD1_C,
    CTR_PAD1_D,
    CTR_PAD2_A,
    CTR_PAD2_B,
    CTR_PAD2_C,
    CTR_PAD2_D,
    CTR_MAX
} ctr_mode;

/**
 * Set default control mode for both players
 */
void ctr_setDef();

/**
 * Read both players input mode
 * 
 * @param pl1 Player1 mode
 * @param pl2 Player2 mode
 */
void ctr_getModes(int *pl1, int *pl2);

/**
 * Change the current input scheme
 * 
 * @param ID ID of the player being set
 * @param mode The new controle mode
 * @param Whether the mode was switched successfully
 */
int ctr_setMode(int ID, ctr_mode mode);

/**
 * Change the current input scheme, ignoring if it clashes with the other
 * player (but checking for controller presence)
 * 
 * @param ID ID of the player being set
 * @param mode The new controle mode
 * @param Whether the mode was switched successfully
 */
int ctr_setModeForce(int ID, ctr_mode mode);

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

/**
 * Checks if the pause button is pressed
 * 
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_pause();

#endif

