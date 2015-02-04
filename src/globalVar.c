/**
 * @file src/globalVar.c
 */
#include "globalVar.h"

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

