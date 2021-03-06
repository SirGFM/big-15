/**
 * @file src/globalVar.c
 * 
 * Module for storing global variables, accessible anywhere in the game
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_save.h>

#include "globalVar.h"
#include "save.h"
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
    "boss_phase",
    "boss_x",
    "boss_y",
    "boss_dir",
    "boss_isRunning",
    "boss_isDead",
    "heartup01",
    "heartup02",
    "heartup03",
    "heartup04",
    "heartup05",
    "heartup06",
    "heartup07",
    "heartup01-hidden",
    "heartup02-hidden",
    "heartup03-hidden",
    "heartup05-hidden",
    "heartup06-hidden",
    "heartup07-hidden",
    "switch_map",
    "map001_door",
    "map002-1_door",
    "map002-2_door",
    "map003_door",
    "map005_door",
    "map006-a_door",
    "map006-b_door",
    "map006-c_door",
    "map007_door",
    "map008-a_door",
    "map008-b_door",
    "map008-c_door",
    "map008-d_door",
    "map008-e_door",
    "map010_door",
    "map011-a_door",
    "map011-b_door",
    "map011-c_door",
    "map012-a_door",
    "map012-b_door",
    "map012-c_door",
    "map015-a_door",
    "map015-b_door",
    "map016-a_door",
    "map016-b_door",
    "map016-c_door",
    "map020-a_door",
    "map020-b_door",
    "map020-c_door",
    "terminal001",
    "terminal002",
    "terminal003",
    "terminal004",
    "terminal005",
    "terminal006",
    "terminal007",
    "terminal008",
    "terminal009",
    "terminal010",
    "terminal011",
    "terminal012",
    "terminal013",
    "terminal014",
    "terminal015",
    "terminal016",
    "terminal017",
    "terminal018",
    "terminal019",
    "terminal020",
    "terminal021",
    "terminal022",
    "terminal023",
    "terminal024",
    "terminal025",
    "terminal026",
    "terminal027",
    "hjump-term",
    "telep-term",
    "signl-term",
    "game-ups",
    "timer-bomb",
    "game-time",
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

/**
 * Save the current state of the global vars to a file
 * 
 * @param filename The filename
 * @return GFraMe error code
 */
GFraMe_ret gv_save(char *filename) {
    write_block(BLK_GAME, _gv_arr, GV_MAX);
    return flush_block(BLK_GAME);
}

/**
 * Load the gv state from a file
 * 
 * @param filename The filename
 * @return GFraMe error code
 */
GFraMe_ret gv_load(char *filename) {
    read_block(BLK_GAME, _gv_arr, GV_MAX);
    return 0;
}
