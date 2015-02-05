/**
 * @file src/map.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include <stdio.h>

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
 * 
 * @param str String with the map data
 * @param len String's len
 * @return GFraMe error code
 */
GFraMe_ret map_loads(char *str, int len) {
    // Should be function_not_implemented, but yeah...
    return GFraMe_ret_failed;
}

/**
 * Load a map from a file
 * 
 * @param fn Filename
 * @return GFraMe error code
 */
GFraMe_ret map_loadf(char *fn) {
    FILE *fp;
    GFraMe_ret rv;
    
    fp = NULL;
    
    GFraMe_assertRV(fn, "No filename sent!", rv = GFraMe_ret_bad_param, __ret);
    
    fp = fopen(fn, "rt");
    GFraMe_assertRV(fp, "404!", rv = GFraMe_ret_file_not_found, __ret);
    
    while (1) {
        fpos_t pos;
        int c, irv;
        
        irv = fgetpos(fp, &pos);
        GFraMe_assertRV(irv == 0, "Failed to get posision on file!",
            rv = GFraMe_ret_failed, __ret);
        // Read a character from the stream
        c = fgetc(fp);
        GFraMe_assertRV(c != EOF, "Reached EOF!", GFraMe_ret_failed, __ret);
        
        switch (c) {
            case 'e': { // event
            } break;
            case 't': {
                // May be a tilemap
                c = fgetc(fp);
                GFraMe_assertRV(c != EOF, "Reached EOF!", GFraMe_ret_failed,
                    __ret);
                
                if (c == 'm') {
                }
            } break;
        }
    }
    
    rv = GFraMe_ret_ok;
__ret:
    if (fp)
        fclose(pf);
    
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

