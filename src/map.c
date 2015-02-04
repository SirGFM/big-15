/**
 * @file src/map.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include "map.h"

/**
 * Initialize the map module
 * 
 * @return GFraMe error code
 */
GFraMe_ret map_init() {
    GFraMe_ret rv;
    
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up memory used by the map
 */
void map_clean() {
    
}

/**
 * Load a map from a string
 */
GFraMe_ret map_load(char *str) {
    GFraMe_ret rv;
    
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Animate the map tiles
 * 
 * @param ms Time, in milliseconds, elapsed from the last frame
 */
void map_update(int ms) {
    
}

/**
 * Render the current map
 */
void map_draw() {
    
}

/**
 * Get a list of objects for the map's collideable area
 * 
 * @param objs List of objects
 * @param len Number of valid objects on the list
 */
void map_getWalls(GFraMe_object **objs, int *len) {
    
}

/**
 * Check if a sprite triggered any event
 * 
 * @param spr The sprite
 */
void map_checkEvents(GFraMe_sprite *spr) {
    
}

