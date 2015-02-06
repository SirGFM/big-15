/**
 * @file src/map.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdio.h>

#include "event.h"
#include "map.h"

#define ASSERT(stmt, err) \
  do { \
    if (!(stmt)) { \
      rv = err; \
      goto __ret; \
    } \
  } while (0)

#define ASSERT_NR(stmt) \
  do { \
    if (!(stmt)) { \
      goto __ret; \
    } \
  } while (0)


typedef struct {
    int pos;              /** Position of the tile on the tilemap */
    int elapsed;          /** How long has this tile being displayed */
} animTile;

struct stMap {
    char *data;           /** Tilemap's data                               */
    int dataLen;          /** Size of the tilemap's buffer                 */
    int w;                /** Width of the tilemap, in tiles               */
    int h;                /** Height of the tilemap, int tiles             */
    
    event **evs;          /** List of events                               */
    int evsLen;           /** Size of the events list                      */
    int evsUsed;          /** How many events are currently active         */
    
    GFraMe_object *objs;  /** List of objects, for the walls               */
    int objsLen;          /** Size of the objects list                     */
    int objsUsed;         /** How many objects there are in the list       */
    
    animTile *animTiles;  /** List of animated tiles in the tilemap's data */
    int animTilesLen;     /** Size of the list of animated tiles           */
    int animTilesUsed;    /** Number of animated tiles on the current */
};

/**
 * Realloc the evs buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setEventsMinLength(map *pM, int len) {
    GFraMe_ret rv;
    int i;
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->evsLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    i = pM->evsLen;
    pM->evs = (event**)realloc(pM->evs, sizeof(event*) * len);
    ASSERT(pM->evs, GFraMe_ret_memory_error);
    pM->evsLen = len;
    
    // Initialize every uninitialize event
    while (i < len) {
        rv = event_getNew(&pM->evs[i]);
        ASSERT(pM->evs[i], rv);
        i++;
    }
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}
/**
 * Realloc the objs buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setObjectsMinLength(map *pM, int len) {
    GFraMe_ret rv;
    int i;
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->objLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    i = pM->objLen;
    pM->obj = (GFraMe_object*)realloc(pM->obj, sizeof(GFraMe_object) * len);
    ASSERT(pM->obj, GFraMe_ret_memory_error);
    pM->objLen = len;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}
/**
 * Realloc the animTiles buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setAnimTilesMinLength(map *pM, int len) {
    GFraMe_ret rv;
    int i;
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->animTilesLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    i = pM->objLen;
    pM->animTiles = (animTile*)realloc(pM->animTiles, sizeof(animTiles) * len);
    ASSERT(pM->animTiles, GFraMe_ret_memory_error);
    pM->animTilesLen = len;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Initialize the map module
 * 
 * @param ppM Returns the map
 * @return GFraMe error code
 */
GFraMe_ret map_init(map **ppM) {
    GFraMe_ret rv;
    map *pM;
    
    // Initialize this so it can be cleaned, on error
    pM = NULL;
    
    // Sanitize paramenters
    GFraMe_assertRV(ppM, "No container for the map sent!",
        rv = GFraMe_ret_bad_param, __ret);
    GFraMe_assertRV(!*ppM, "Map already initialized!", rv = GFraMe_ret_bad_param,
        __ret);
    
    // Create the map
    pM = (map*)malloc(sizeof(map));
    GFraMe_assertRV(pM, "Failed to alloc!", rv = GFraMe_ret_memory_error,
        __ret);
    
    // Intialize every buffer, so bad things doesn't happen on error
    pM->data = NULL;
    pM->dataLen = 0;
    pM->evs = NULL;
    pM->evsLen = 0;
    pM->objs = NULL;
    pM->objsLen = 0;
    pM->animTiles = NULL;
    pM->animTilesLen = 0;
    
    // Initialize every struture it might use
    pM->w = 40;
    pM->h = 30;
    pM->dataLen = pM->w * pM->h;
    pM->data = (char*)malloc(pM->dataLen);
    GFraMe_assertRV(pM->data, "Failed to alloc!", rv = GFraMe_ret_memory_error,
        __ret);
    
    rv = map_setEventsMinLength(pM, 4);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init events", rv=rv, __ret);
    pM->evsUsed = 0;
    
    rv = map_setObjectsMinLength(pM, 4);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init objs", rv = rv, __ret);
    pM->objsUsed = 0;
    
    rv = map_setAnimTilesMinLength(pM, 8);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init anim", rv = rv, __ret);
    pM->animTilesUsed = 0;
    
    ppM = pM;
    rv = GFraMe_ret_ok;
__ret:
    if (rv != GFraMe_ret_ok && pM)
        free(pM);
    
    return rv;
}

/**
 * Clean up memory used by the map
 * 
 * @param ppM The map
 */
void map_clean(map **ppM) {
    int i;
    
    // Sanitize parameters
    ASSERT_NR(ppM);
    ASSERT_NR(*ppM);
    
    free(*ppM->data);
    free(*ppM->objs);
    free(*ppM->animTiles);
    
    i = 0;
    while (i < *ppM->evsLen) {
        event_clean(&(*ppM->evs[i]));
        i++;
    }
    free(*ppM->evs);
    
    free(*ppM);
    *ppM = NULL;
__ret:
    return;
}

/**
 * Reset a map so it can be reused
 * 
 * @param m The map
 */
void map_reset(map *m) {
}

/**
 * Retrieve the next event on the map's list (recycled and expends as necessary)
 * Note that the event must be pushed later
 * 
 * @param ppE Returns the event
 * @param m The map
 * @return GFraMe error code
 */
GFraMe_ret map_getNextEvent(event **ppE, map *m) {
}

/**
 * Actually push the last gotten event into the map. If no map_getNextEvent was
 * previously called, this function does nothing.
 * 
 * @param m The map
 */
void map_pushEvent(map *m) {
}

/**
 * Get the current tilemap, if any
 * 
 * @param ppData Data retrieved or NULL
 * @param pLen How many bytes there are in the buffer
 * @param m The map
 * @return GFraMe error code
 */
GFraMe_ret map_getTilemapData(char **ppData, int *pLen, map *m) {
}

/**
 * Set the current tilemap
 * 
 * @param m The map
 * @param pData The tilemap
 * @param len How many bytes there are in the buffer (needn't all be in use)
 * @param w How many tiles there are horizontally
 * @param h How many tiles there are vertically
 */
void map_setTilemap(map *m, char *pData, int len, int w, int h) {
}

/**
 * Load a map from a string
 * 
 * @param m The map
 * @param str String with the map data
 * @param len String's len
 * @return GFraMe error code
 */
GFraMe_ret map_loads(map *m, char *str, int len) {
}

/**
 * Load a map from a file
 * 
 * @param m The map
 * @param fn Filename
 * @return GFraMe error code
 */
GFraMe_ret map_loadf(map *m, char *fn) {
}

/**
 * Animate the map tiles
 * 
 * @param m The map
 * @param ms Time, in milliseconds, elapsed from the last frame
 */
void map_update(map *m, int ms) {
}

/**
 * Render the current map
 * 
 * @param m The map
 */
void map_draw(map *m) {
}

/**
 * Get a list of objects for the map's collideable area
 * 
 * @param objs List of objects
 * @param len Number of valid objects on the list
 * @param m The map
 */
void map_getWalls(GFraMe_object **objs, int *len, map *m) {
}

/**
 * Check if a sprite triggered any event
 * 
 * @param m The map
 * @param spr The sprite
 */
void map_checkEvents(map *m, GFraMe_sprite *spr) {
}

