/**
 * @file src/globalVar.h
 * 
 * Module for storing global variables, accessible anywhere in the game
 */
#ifndef __GLOBAL_VAR_H_
#define __GLOBAL_VAR_H_

typedef enum {
    PL1_HP,    /** Player 1 current health                  */
    PL1_MAXHP, /** Player 1 maximum health                  */
    PL1_ITEM,  /** Player 1 current item                    */
    PL1_DEATH, /** How many times player 1 died             */
    PL2_HP,    /** Player 2 current health                  */
    PL2_MAXHP, /** Player 2 maximum health                  */
    PL2_ITEM,  /** Player 2 current item                    */
    PL2_DEATH, /** How many times player 2 died             */
    MAP,       /** Current map                              */
    DOOR_X,    /** Entrance horizontal point on current map */
    DOOR_Y,    /** Entrance vertical point on current map   */
    ITEMS,     /** Flags for available items                */
    TEST_DOOR, /** State of a test door                     */
    GV_MAX     /** Global var count                         */
} globalVar;

/**
 * Initialize the global variables
 * 
 * TODO: Load from file
 */
void gv_init();

/**
 * Set a global variable to a value
 * 
 * @param gv The global variable
 * @param val The new value
 */
void gv_setValue(globalVar gv, int val);

/**
 * Increase a global variable
 * 
 * @param gv The global variable
 */
void gv_inc(globalVar gv);

/**
 * Decrease a global variable
 * 
 * @param gv The global variable
 */
void gv_dec(globalVar gv);

/**
 * Sum a value to the global variable
 * 
 * @param gv The global variable
 * @param val The value to be added
 */
void gv_add(globalVar gv, int val);

/**
 * Subtract a value from the global variable
 * 
 * @param gv The global variable
 * @param val The value to be subtracted
 */
void gv_sub(globalVar gv, int val);

/**
 * Get a global variable's value
 * 
 * @param gv The global variable
 * @return The variable's value
 */
int gv_getValue(globalVar gv);

/**
 * Check whether a variable is zero or not
 * 
 * @param gv The global variable
 * return 1 on true, 0 on false
 */
int gv_isZero(globalVar gv);

/**
 * Check whether a variable is non zero
 * 
 * @param gv The global variable
 * return 1 on true, 0 on false
 */
int gv_nIsZero(globalVar gv);

/**
 * Get a globalVar's name
 * 
 * @param gv The globalVar
 * @return The global variable's name or NULL
 */
char* gv_getName(globalVar gv);

#endif

