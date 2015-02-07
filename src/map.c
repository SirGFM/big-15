/**
 * @file src/map.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdio.h>

#include "commonEvent.h"
#include "event.h"
#include "global.h"
#include "map.h"
#include "parser.h"

#define TILE_SHOCK_L1 96
#define TILE_SHOCK_L2 97
#define TILE_SHOCK_L3 98
#define TILE_SHOCK_L4 99
#define TILE_SHOCK_R1 128
#define TILE_SHOCK_R2 129
#define TILE_SHOCK_R3 130
#define TILE_SHOCK_R4 131

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
    unsigned char *data;  /** Tilemap's data                               */
    int dataLen;          /** Size of the tilemap's buffer                 */
    int w;                /** Width of the tilemap, in tiles               */
    int h;                /** Height of the tilemap, int tiles             */
    
    event **evs;          /** List of events                               */
    int evsLen;           /** Size of the events list                      */
    int evsUsed;          /** How many events are currently active         */
    int didGetEvent;      /** Whether map_getNextEvent has been called     */
    
    GFraMe_object *objs;  /** List of objects, for the walls               */
    int objsLen;          /** Size of the objects list                     */
    int objsUsed;         /** How many objects there are in the list       */
    
    animTile *animTiles;  /** List of animated tiles in the tilemap's data */
    int animTilesLen;     /** Size of the list of animated tiles           */
    int animTilesUsed;    /** Number of animated tiles on the current */
};

/**
 * Check if a tile is animated or not
 * 
 * @param tile The tile to be checked
 * @return GFraMe_ret_ok if it is, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_tileIsAnimated(int tile) {
    switch (tile) {
        case TILE_SHOCK_L1:
        case TILE_SHOCK_L2:
        case TILE_SHOCK_L3:
        case TILE_SHOCK_L4:
        case TILE_SHOCK_R1:
        case TILE_SHOCK_R2:
        case TILE_SHOCK_R3:
        case TILE_SHOCK_R4:
            return GFraMe_ret_ok;
        default:
            return GFraMe_ret_failed;
    }
}

/**
 * Updates and check if a given animated tile should be modified
 * 
 * @param pM The map
 * @param pT The animated tile
 * @param ms Time elapsed from the previous frame
 */
static void map_animateTile(map *pM, animTile *pT, int ms) {
    unsigned char tile;
    
    // Update the tile's running time
    pT->elapsed += ms;
    
    // Check which tile it is
    tile = pM->data[pT->pos];
    
    // Update the tile, if necessary
    switch (tile) {
        case TILE_SHOCK_L1:
        case TILE_SHOCK_L2:
        case TILE_SHOCK_L3: { // 16 fps
            if (pT->elapsed >= 62) {
                tile++;
                pT->elapsed -= 62;
            }
        } break;
        case TILE_SHOCK_L4: { // 16 fps
            if (pT->elapsed >= 62) {
                tile = TILE_SHOCK_L1;
                pT->elapsed -= 62;
            }
        } break;
        case TILE_SHOCK_R1:
        case TILE_SHOCK_R2:
        case TILE_SHOCK_R3: { // 16 fps
            if (pT->elapsed >= 62) {
                tile++;
                pT->elapsed -= 62;
            }
        } break;
        case TILE_SHOCK_R4: { // 16 fps
            if (pT->elapsed >= 62) {
                tile = TILE_SHOCK_R1;
                pT->elapsed -= 62;
            }
        } break;
        default: {}
    }
    
    pM->data[pT->pos] = tile;
}

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
        pM->evs[i] = NULL;
        
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
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->objsLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    pM->objs = (GFraMe_object*)realloc(pM->objs, sizeof(GFraMe_object) * len);
    ASSERT(pM->objs, GFraMe_ret_memory_error);
    pM->objsLen = len;
    
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
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->animTilesLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    pM->animTiles = (animTile*)realloc(pM->animTiles, sizeof(animTile) * len);
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
    pM->w = 0;
    pM->h = 0;
    pM->evs = NULL;
    pM->evsLen = 0;
    pM->evsUsed = 0;
    pM->didGetEvent = 0;
    pM->objs = NULL;
    pM->objsLen = 0;
    pM->objsUsed = 0;
    pM->animTiles = NULL;
    pM->animTilesLen = 0;
    pM->animTilesUsed = 0;
    
    // Initialize every struture it might use
    pM->w = 40;
    pM->h = 30;
    pM->dataLen = pM->w * pM->h;
    pM->data = (unsigned char*)malloc(pM->dataLen);
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
    
    *ppM = pM;
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
    
    free((*ppM)->data);
    free((*ppM)->objs);
    free((*ppM)->animTiles);
    
    i = 0;
    while (i < (*ppM)->evsLen) {
        event_clean(&(*ppM)->evs[i]);
        i++;
    }
    free((*ppM)->evs);
    
    free(*ppM);
    *ppM = NULL;
__ret:
    return;
}

/**
 * Reset a map so it can be reused
 * 
 * @param pM The map
 */
void map_reset(map *pM) {
    ASSERT_NR(pM);
    
    pM->w = 0;
    pM->h = 0;
    pM->evsUsed = 0;
    pM->objsUsed = 0;
    pM->animTilesUsed = 0;
    
__ret:
    return;
}

/**
 * Retrieve the next event on the map's list (recycled and expends as necessary)
 * Note that the event must be pushed later
 * 
 * @param ppE Returns the event
 * @param ipM The map
 * @return GFraMe error code
 */
GFraMe_ret map_getNextEvent(event **ppE, map *pM) {
    GFraMe_ret rv;
    
    // Sanitize arguments
    ASSERT(ppE, GFraMe_ret_bad_param);
    ASSERT(pM, GFraMe_ret_bad_param);
    
    // Expand the buffer, if necessary
    if (pM->evsUsed >= pM->evsLen) {
        rv = map_setEventsMinLength(pM, pM->evsLen * 2);
        ASSERT(rv == GFraMe_ret_ok, rv);
    }
    
    // Get the next event and return
    *ppE = pM->evs[pM->evsUsed];
    pM->didGetEvent = 1;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Actually push the last gotten event into the map. If no map_getNextEvent was
 * previously called, this function does nothing.
 * 
 * @param pM The map
 */
void map_pushEvent(map *pM) {
    // Sanitize parameters
    ASSERT_NR(pM);
    ASSERT_NR(pM->didGetEvent);
    
    // Increase the events in use
    pM->evsUsed++;
    pM->didGetEvent = 0;
    
__ret:
    return;
}

/**
 * Get the current tilemap, if any
 * 
 * @param ppData Data retrieved or NULL
 * @param pLen How many bytes there are in the buffer
 * @param pM The map
 * @return GFraMe error code
 */
GFraMe_ret map_getTilemapData(unsigned char **ppData, int *pLen, map *pM) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(ppData, GFraMe_ret_bad_param);
    ASSERT(pLen, GFraMe_ret_bad_param);
    ASSERT(pM, GFraMe_ret_bad_param);
    
    // Retrieve the to be returned variables
    *ppData = pM->data;
    *pLen = pM->dataLen;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Set the current tilemap
 * 
 * @param pM The map
 * @param pData The tilemap
 * @param len How many bytes there are in the buffer (needn't all be in use)
 * @param w How many tiles there are horizontally
 * @param h How many tiles there are vertically
 */
void map_setTilemap(map *pM, unsigned char *pData, int len, int w, int h) {
    GFraMe_ret rv;
    int i;
    
    // Sanitize parameters
    ASSERT_NR(pM);
    ASSERT_NR(pData);
    ASSERT_NR(len > 0);
    ASSERT_NR(w > 0);
    ASSERT_NR(h > 0);
    
    // Set the data
    pM->data = pData;
    pM->dataLen = len;
    pM->w = w;
    pM->h = h;
    
    // Get all the animated tiles
    i = 0;
    while (i < w*h) {
        unsigned char t;
        
        t = pData[i];
        if (map_tileIsAnimated(t) == GFraMe_ret_ok) {
            animTile *tile;
            
            // Expand the buffer as necessary
            if (pM->animTilesUsed >= pM->animTilesLen) {
                rv = map_setAnimTilesMinLength(pM, pM->animTilesLen*2);
                ASSERT_NR(rv == GFraMe_ret_ok);
                // TODO return the error
            }
            
            // Get the animated tile
            tile = &pM->animTiles[pM->animTilesUsed];
            pM->animTilesUsed++;
            
            // Initialize the animated tile
            tile->pos = i;
            tile->elapsed = 0;
        }
        i++;
    }
    
    // TODO create walls
    
__ret:
    return;
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
    // Wrong return... >__<
    return GFraMe_ret_failed;
}

/**
 * Load a map from a file
 * 
 * @param pM The map
 * @param fn Filename
 * @return GFraMe error code
 */
GFraMe_ret map_loadf(map *pM, char *fn) {
    GFraMe_ret rv;
    
    // Parse the map from a file
    rv = parsef_map(&pM, fn);
    
    return rv;
}

/**
 * Animate the map tiles
 * 
 * @param pM The map
 * @param ms Time, in milliseconds, elapsed from the last frame
 */
void map_update(map *pM, int ms) {
    int i;
    
    // Update every animated tile
    i = 0;
    while (i < pM->animTilesUsed) {
        animTile *t;
        
        t = &pM->animTiles[i];
        map_animateTile(pM, t, ms);
        
        i++;
    }
}

/**
 * Render the current map
 * 
 * @param pM The map
 */
void map_draw(map *pM) {
    int i, offset, x, y;
    
    // Remove this ifdef when camera is implemented (if ever)
#if 0
    // Get the first tile position on screen
    y = -(cam_y % TILE_HEIGHT);
    // Get the first tile position
    offset = cam_y / TILE_HEIGHT * TILES_PER_LINE;
#else
    int cam_y;
    
    cam_y = 0;
    y = 0;
    offset = 0;
#endif
    
    // Loop through every tile
    i = 0;
    x = 0;
    while (1) {
        // Check that the tile is still valid
        if (i >= pM->w * pM->h)
            break;
        
        // Render the tile to the screen
        GFraMe_spriteset_draw
            (
             gl_sset8x8,
             pM->data[offset + i],
             x,
             y,
             0 // flipped
            );
        
        // Updates the tile position
        x += 8;
        if (x >= SCR_W) {
            x = 0;
            y += 8;
        }
        if (y - cam_y > SCR_H)
            break;
        i++;
    }
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

