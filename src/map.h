/**
 * @file src/map.h
 * 
 * Module that handles displaying the current level, handling its hitbox,
 * animating it and checking its events.
 */
#ifndef __MAP_H_
#define __MAP_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

/**
 * Initialize the map module
 * 
 * @return GFraMe error code
 */
GFraMe_ret map_init();

/**
 * Clean up memory used by the map
 */
void map_clean();

/**
 * Load a map from a string
 * 
 * @param str String with the map data
 * @param len String's len
 * @return GFraMe error code
 */
GFraMe_ret map_loads(char *str, int len);

/**
 * Load a map from a file
 * 
 * @param fn Filename
 * @return GFraMe error code
 */
GFraMe_ret map_loadf(char *fn);

/**
 * Animate the map tiles
 * 
 * @param ms Time, in milliseconds, elapsed from the last frame
 */
void map_update(int ms);

/**
 * Render the current map
 */
void map_draw();

/**
 * Get a list of objects for the map's collideable area
 * 
 * @param objs List of objects
 * @param len Number of valid objects on the list
 */
void map_getWalls(GFraMe_object **objs, int *len);

/**
 * Check if a sprite triggered any event
 * 
 * @param spr The sprite
 */
void map_checkEvents(GFraMe_sprite *spr);

#endif

