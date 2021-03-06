/**
 * @file src/globalVar.h
 * 
 * Module for storing global variables, accessible anywhere in the game
 */
#ifndef __GLOBAL_VAR_H_
#define __GLOBAL_VAR_H_

#include <GFraMe/GFraMe_error.h>

typedef enum {
    PL1_HP,       /** Player 1 current health                      */
    PL1_MAXHP,    /** Player 1 maximum health                      */
    PL1_ITEM,     /** Player 1 current item                        */
    PL1_DEATH,    /** How many times player 1 died                 */
    PL1_CX,       /** Player 1 horizontal (world) position         */
    PL1_CY,       /** Player 1 vertical (world) position           */
    PL2_HP,       /** Player 2 current health                      */
    PL2_MAXHP,    /** Player 2 maximum health                      */
    PL2_ITEM,     /** Player 2 current item                        */
    PL2_DEATH,    /** How many times player 2 died                 */
    PL2_CX,       /** Player 1 horizontal (world) position         */
    PL2_CY,       /** Player 1 vertical (world) position           */
    SIGL_X,       /** Signaler horizontal position                 */
    SIGL_Y,       /** Signaler vertical position                   */
    TELP_X,       /** Target teleporting position                  */
    TELP_Y,       /** Target teleporting position                  */
    MAP,          /** Current map                                  */
    DOOR_X,       /** Entrance horizontal point on current map     */
    DOOR_Y,       /** Entrance vertical point on current map       */
    ITEMS,        /** Flags for available items                    */
    BOSS_PHASE,   /** Which phase the boss is currently in         */
    BOSS_X,       /** Boss top-left origin, according to the wheel */
    BOSS_Y,       /** Boss top-left origin, according to the wheel */
    BOSS_DIR,
    BOSS_ISRUNNING,
    BOSS_ISDEAD,
    HEARTUP01,
    HEARTUP02,
    HEARTUP03,
    HEARTUP04,
    HEARTUP05,
    HEARTUP06,
    HEARTUP07,
    HEARTUP01_HIDDEN,
    HEARTUP02_HIDDEN,
    HEARTUP03_HIDDEN,
    HEARTUP05_HIDDEN,
    HEARTUP06_HIDDEN,
    HEARTUP07_HIDDEN,
    SWITCH_MAP,   /** Whether the map should be switched       */
    MAP001_DOOR,  /** State of the only door on map 001        */
    MAP002_DOOR1, /** State of the only door on map 002        */
    MAP002_DOOR2, /** State of the only door on map 002        */
    MAP003_DOOR,  /** State of the only door on map 003        */
    MAP005_DOOR,  /** State of a door on map 005              */
    MAP006_DOOR_A,/** State of a door on map 005              */
    MAP006_DOOR_B,/** State of a door on map 005              */
    MAP006_DOOR_C,/** State of a door on map 005              */
    MAP007_DOOR,  /** State of a door on map 005              */
    MAP008_DOOR_A,/** State of a door on map 005              */
    MAP008_DOOR_B,/** State of a door on map 005              */
    MAP008_DOOR_C,/** State of a door on map 005              */
    MAP008_DOOR_D,/** State of a door on map 005              */
    MAP008_DOOR_E,/** State of a door on map 005              */
    MAP010_DOOR,  /** State of a door on map 005              */
    MAP011_DOOR_A,/** State of a door on map 005              */
    MAP011_DOOR_B,/** State of a door on map 005              */
    MAP011_DOOR_C,/** State of a door on map 005              */
    MAP012_DOOR_A,  /** State of a door on map 005              */
    MAP012_DOOR_B,  /** State of a door on map 005              */
    MAP012_DOOR_C,  /** State of a door on map 005              */
    MAP015_DOOR_A,  /** State of a door on map 005              */
    MAP015_DOOR_B,  /** State of a door on map 005              */
    MAP016_DOOR_A,  /** State of a door on map 005              */
    MAP016_DOOR_B,  /** State of a door on map 005              */
    MAP016_DOOR_C,  /** State of a door on map 005              */
    MAP020_DOOR_A,  /** State of a door on map 005              */
    MAP020_DOOR_B,  /** State of a door on map 005              */
    MAP020_DOOR_C,  /** State of a door on map 005              */
    TERMINAL_001,
    TERMINAL_002,
    TERMINAL_003,
    TERMINAL_004,
    TERMINAL_005,
    TERMINAL_006,
    TERMINAL_007,
    TERMINAL_008,
    TERMINAL_009,
    TERMINAL_010,
    TERMINAL_011,
    TERMINAL_012,
    TERMINAL_013,
    TERMINAL_014,
    TERMINAL_015,
    TERMINAL_016,
    TERMINAL_017,
    TERMINAL_018,
    TERMINAL_019,
    TERMINAL_020,
    TERMINAL_021,
    TERMINAL_022,
    TERMINAL_023,
    TERMINAL_024,
    TERMINAL_025,
    TERMINAL_026,
    TERMINAL_027,
    HJUMP_TERM,
    TELEP_TERM,
    SIGNL_TERM,
    GAME_UPS,
    TIMER_BOMB,
    GAME_TIME,
    GV_MAX        /** Global var count                         */
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
 * Set a single bit
 * 
 * @param gv The global variable
 * @param val The new value
 */
void gv_setBit(globalVar gv, int bit);

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

/**
 * Save the current state of the global vars to a file
 * 
 * @param filename The filename
 * @return GFraMe error code
 */
GFraMe_ret gv_save(char *filename);

/**
 * Load the gv state from a file
 * 
 * @param filename The filename
 * @return GFraMe error code
 */
GFraMe_ret gv_load(char *filename);

#endif

