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

#include "event.h"
#include "object.h"

typedef struct stMap map;

/**
 * Initialize the map module
 * 
 * @param ppM Returns the map
 * @return GFraMe error code
 */
GFraMe_ret map_init(map **ppM);

/**
 * Clean up memory used by the map
 * 
 * @param ppM The map
 */
void map_clean(map **ppM);

/**
 * Reset a map so it can be reused
 * 
 * @param pM The map
 */
void map_reset(map *pM);

/**
 * Get the current tilemap, if any
 * 
 * @param ppData Data retrieved or NULL
 * @param pLen How many bytes there are in the buffer
 * @param pM The map
 * @return GFraMe error code
 */
GFraMe_ret map_getTilemapData(unsigned char **ppData, int *pLen, map *pM);

/**
 * Set the current tilemap
 * 
 * @param pM The map
 * @param pData The tilemap
 * @param len How many bytes there are in the buffer (needn't all be in use)
 * @param w How many tiles there are horizontally
 * @param h How many tiles there are vertically
 */
void map_setTilemap(map *pM, unsigned char *pData, int len, int w, int h);

/**
 * Load a map from a string
 * 
 * @param m The map
 * @param str String with the map data
 * @param len String's len
 * @return GFraMe error code
 */
GFraMe_ret map_loads(map *m, char *str, int len);

/**
 * Load a map from a file
 * 
 * @param m The map
 * @param fn Filename
 * @return GFraMe error code
 */
GFraMe_ret map_loadf(map *m, char *fn);

/**
 * Load a indexed map
 * 
 * @param m The map
 * @param i The map's index
 * @return GFraMe error code
 */
GFraMe_ret map_loadi(map *m, int i);

/**
 * Animate the map tiles
 * 
 * @param pM The map
 * @param ms Time, in milliseconds, elapsed from the last frame
 */
void map_update(map *pM, int ms);

/**
 * Render the current map
 * 
 * @param pM The map
 */
void map_draw(map *pM);

/**
 * Get a list of objects for the map's collideable area
 * 
 * @param ppObjs List of objects
 * @param pLen Number of valid objects on the list
 * @param pM The map
 * @return GFraMe error code
 */
GFraMe_ret map_getWalls(GFraMe_object **ppObjs, int *pLen, map *pM);

/**
 * Get a map's dimensions (in pixels)
 * 
 * @param pM The map
 * @param pW The map's width
 * @param pH The map's height
 */
void map_getDimensions(map *pM, int *pW, int *pH);

/**
 * Add everything in the map to the quadtree
 * 
 * @param pM The map
 * @return GFraMe error code
 */
GFraMe_ret map_addQt(map *pM);

#endif

