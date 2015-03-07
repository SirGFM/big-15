/**
 * @file src/globalVar.c
 * 
 * Module for storing global variables, accessible anywhere in the game
 */
#include "globalVar.h"
#include "types.h"

static char *_gv_names[GV_MAX+1] = {
    "pl1_hp",
    "pl1_maxhp",
    "pl1_item",
    "pl1_death",
    "pl1_x",
    "pl1_y",
    "pl2_hp",
    "pl2_maxhp",
    "pl2_item",
    "pl2_death",
    "pl2_x",
    "pl2_y",
    "sigl_x",
    "sigl_y",
    "telp_x",
    "telp_y",
    "map",
    "door_x",
    "door_y",
    "items",
    "switch_map",
    "map001_door",
    "map002_door",
    "map003_door",
    "map004_door1",
    "map004_door2",
    "gv_max"
};

/** Static array for global variables */
static int _gv_arr[GV_MAX];

/**
 * Initialize the global variables
 * 
 * TODO: Load from file
 */
void gv_init() {
    int i;
    
    // Initialize every variable to zero
    i = 0;
    while (i < GV_MAX) {
        _gv_arr[i] = 0;
        i++;
    }
    
    _gv_arr[PL1_MAXHP] = 3;
    _gv_arr[PL1_HP] = 3;
    _gv_arr[PL2_MAXHP] = 3;
    _gv_arr[PL2_HP] = 3;
    _gv_arr[SIGL_X] = -1;
    _gv_arr[SIGL_Y] = -1;
    // TODO set DOOR_X & DOOR_Y
    
//    _gv_arr[ITEMS] = ID_HIGHJUMP | ID_TELEPORT | ID_SIGNALER;
//    _gv_arr[PL1_ITEM] = ID_TELEPORT;
//    _gv_arr[PL2_ITEM] = ID_SIGNALER;
}

/**
 * Set a global variable to a value
 * 
 * @param gv The global variable
 * @param val The new value
 */
void gv_setValue(globalVar gv, int val) {
    if (gv < GV_MAX)
        _gv_arr[gv] = val;
}

/**
 * Set a single bit
 * 
 * @param gv The global variable
 * @param val The new value
 */
void gv_setBit(globalVar gv, int bit) {
    if (gv < GV_MAX)
        _gv_arr[gv] |= bit;
}

/**
 * Increase a global variable
 * 
 * @param gv The global variable
 */
void gv_inc(globalVar gv) {
    if (gv < GV_MAX)
        _gv_arr[gv]++;
}

/**
 * Decrease a global variable
 * 
 * @param gv The global variable
 */
void gv_dec(globalVar gv) {
    if (gv < GV_MAX)
        _gv_arr[gv]--;
}

/**
 * Sum a value to the global variable
 * 
 * @param gv The global variable
 * @param val The value to be added
 */
void gv_add(globalVar gv, int val) {
    if (gv < GV_MAX)
        _gv_arr[gv] += val;
}

/**
 * Subtract a value from the global variable
 * 
 * @param gv The global variable
 * @param val The value to be subtracted
 */
void gv_sub(globalVar gv, int val) {
    if (gv < GV_MAX)
        _gv_arr[gv] -= val;
}

/**
 * Get a global variable's value
 * 
 * @param gv The global variable
 * @return The variable's value
 */
int gv_getValue(globalVar gv) {
    if (gv < GV_MAX)
        return _gv_arr[gv];
    return -1;
}

/**
 * Check whether a variable is zero or not
 * 
 * @param gv The global variable
 * return 1 on true, 0 on false
 */
int gv_isZero(globalVar gv) {
    if (gv < GV_MAX)
        return _gv_arr[gv] == 0;
    return -1;
}

/**
 * Check whether a variable is non zero
 * 
 * @param gv The global variable
 * return 1 on true, 0 on false
 */
int gv_nIsZero(globalVar gv) {
    if (gv < GV_MAX)
        return _gv_arr[gv] != 0;
    return -1;
}

/**
 * Get a globalVar's name
 * 
 * @param gv The globalVar
 * @return The global variable's name or NULL
 */
char* gv_getName(globalVar gv) {
    if (gv >= GV_MAX) return 0;
    return _gv_names[gv];
}

