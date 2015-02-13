/**
 * @file src/map.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdio.h>

#include "camera.h"
#include "commonEvent.h"
#include "event.h"
#include "global.h"
#include "map.h"
#include "object.h"
#include "parser.h"

#define TILE_SHOCK_L1 96
#define TILE_SHOCK_L2 97
#define TILE_SHOCK_L3 98
#define TILE_SHOCK_L4 99
#define TILE_SHOCK_R1 128
#define TILE_SHOCK_R2 129
#define TILE_SHOCK_R3 130
#define TILE_SHOCK_R4 131

//============================================================================//
//                                                                            //
// Structs                                                                    //
//                                                                            //
//============================================================================//

typedef struct {
    int pos;              /** Position of the tile on the tilemap */
    int elapsed;          /** How long has this tile being displayed */
} animTile;

struct stMap {
    unsigned char *data;     /** Tilemap's data                               */
    int dataLen;             /** Size of the tilemap's buffer                 */
    int w;                   /** Width of the tilemap, in tiles               */
    int h;                   /** Height of the tilemap, int tiles             */
    
    event **evs;             /** List of events                               */
    int evsLen;              /** Size of the events list                      */
    int evsUsed;             /** How many events are currently active         */
    int didGetEvent;         /** Whether map_getNextEvent has been called     */
    
    GFraMe_object *walls;    /** List of walls                                */
    int wallsLen;            /** Size of the walls list                       */
    int wallsUsed;           /** How many walls there are in the list         */
    
    animTile *animTiles;     /** List of animated tiles in the tilemap's data */
    int animTilesLen;        /** Size of the list of animated tiles           */
    int animTilesUsed;       /** Number of animated tiles on the current      */
    
    object **objs;           /** List of objects                              */
    int objsLen;             /** Size of the list of objects                  */
    int objsUsed;            /** Number of objects on the current map         */
    int didGetObject;        /** Whether map_getNextObject has been called    */
};

//============================================================================//
//                                                                            //
// Static functions forward declaration                                       //
//                                                                            //
//============================================================================//

/**
 * Check if a tile is animated or not
 * 
 * @param tile The tile to be checked
 * @return GFraMe_ret_ok if it is, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_tileIsAnimated(int tile);

/**
 * Updates and check if a given animated tile should be modified
 * 
 * @param pM The map
 * @param pT The animated tile
 * @param ms Time elapsed from the previous frame
 */
static void map_animateTile(map *pM, animTile *pT, int ms);

/**
 * Check which tiles should be animated and add to the list
 * 
 * @param pM The map
 * @return GFraMe error code
 */
static GFraMe_ret map_genAnimatedTiles(map *pM);

/**
 * Check whether a tile is already in a wall object
 * 
 * @param pM The map
 * @param pos Position of the tile to be tested
 * @return GFraMe_ret_ok if it is, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_isTileInWall(map *pM, int pos);

/**
 * Get the bounds of a wall
 * 
 * @param pX Return the horizontal position
 * @param pY Return the vertical position
 * @param pW Return the width
 * @param pH Return the height
 * @param pM The map
 * @param pos First tile in the wall
 */
static void map_getWallBounds(int *pX, int *pY, int *pW, int *pH, map *pM,
    int pos);

/**
 * Calculate where the walls should be placed
 * 
 * @param pM The map
 * @return GFraMe error code
 */
static GFraMe_ret map_genWalls(map *pM);

/**
 * Realloc the evs buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setEventsMinLength(map *pM, int len);

/**
 * Realloc the walls buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setWallsMinLength(map *pM, int len);

/**
 * Realloc the animTiles buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setAnimTilesMinLength(map *pM, int len);

/**
 * Realloc the objs buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setObjectsMinLength(map *pM, int len);

//============================================================================//
//                                                                            //
// Module implementation                                                      //
//                                                                            //
//============================================================================//

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
    pM->walls = NULL;
    pM->wallsLen = 0;
    pM->wallsUsed = 0;
    pM->animTiles = NULL;
    pM->animTilesLen = 0;
    pM->animTilesUsed = 0;
    pM->objs = NULL;
    pM->objsUsed = 0;
    pM->objsLen = 0;
    
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
    
    rv = map_setWallsMinLength(pM, 4);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init walls", rv = rv, __ret);
    pM->wallsUsed = 0;
    
    rv = map_setAnimTilesMinLength(pM, 8);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init anim", rv = rv, __ret);
    pM->animTilesUsed = 0;
    
    rv = map_setObjectsMinLength(pM, 8);
    GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to init objs", rv = rv, __ret);
    pM->objsUsed = 0;
    
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
    
    if ((*ppM)->data)
        free((*ppM)->data);
    if ((*ppM)->walls)
        free((*ppM)->walls);
    if ((*ppM)->animTiles)
        free((*ppM)->animTiles);
    
    if ((*ppM)->evs) {
        i = 0;
        while (i < (*ppM)->evsLen) {
            event_clean(&(*ppM)->evs[i]);
            i++;
        }
        free((*ppM)->evs);
    }
    
    if ((*ppM)->objs) {
        i = 0;
        while (i < (*ppM)->objsLen) {
            obj_clean(&(*ppM)->objs[i]);
            i++;
        }
        free((*ppM)->objs);
    }
    
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
    pM->wallsUsed = 0;
    pM->animTilesUsed = 0;
    pM->objsUsed = 0;
    
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
 * Retrieve the next object on the map's list (recycled and expends as
 * necessary) Note that the event must be pushed later
 * 
 * @param ppO Returns the object
 * @param pM The map
 * @return GFraMe error code
 */
GFraMe_ret map_getNextObject(object **ppO, map *pM) {
    GFraMe_ret rv;
    
    // Sanitize arguments
    ASSERT(ppO, GFraMe_ret_bad_param);
    ASSERT(pM, GFraMe_ret_bad_param);
    
    // Expand the buffer, if necessary
    if (pM->objsUsed >= pM->objsLen) {
        rv = map_setWallsMinLength(pM, pM->objsLen * 2);
        ASSERT(rv == GFraMe_ret_ok, rv);
    }
    
    // Get the next event and return
    *ppO = pM->objs[pM->objsUsed];
    pM->didGetObject = 1;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Actually push the last gotten object into the map. If no map_getNextObject
 * was previously called, this function does nothing.
 * 
 * @param pM The map
 */
void map_pushObject(map *pM) {
    // Sanitize parameters
    ASSERT_NR(pM);
    ASSERT_NR(pM->didGetObject);
    
    // Increase the objects in use
    pM->objsUsed++;
    pM->didGetObject = 0;
    
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
    
    // Animate the tilemap
    rv = map_genAnimatedTiles(pM);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // TODO return the error
    
    rv = map_genWalls(pM);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // TODO return the error
    
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
    
    // Update every object
    i = 0;
    while (i < pM->objsUsed) {
        obj_update(pM->objs[i], ms);
        i++;
    }
}

/**
 * Render the current map
 * 
 * @param pM The map
 */
void map_draw(map *pM) {
    int firstTile, dX, i, iniX, offX, x, y;
    
    // Get the first tile position on screen
    iniX = -(cam_x % 8);
    y = -(cam_y % 8);
    // Get the first tile position
    firstTile = cam_x / 8 + cam_y / 8 * pM->w;
    // Get how many tiles are skipped each row
    dX = pM->w - SCR_W / 8;
    // If the camera's pos doesn't match a tile, it will render 1 extra tile
    if (iniX != 0)
        dX--;
    
    // Loop through every tile
    i = 0;
    x = iniX;
    offX = 0;
    while (1) {
        // Check that the tile is still valid
        if (i >= pM->w * pM->h)
            break;
        
        // Render the tile to the screen
        GFraMe_spriteset_draw
            (
             gl_sset8x8,
             pM->data[firstTile + offX + i],
             x,
             y,
             0 // flipped
            );
        
        // Updates the tile position
        x += 8;
        if (x >= SCR_W) {
            x = iniX;
            y += 8;
            offX += dX;
        }
        if (y > SCR_H)
            break;
        i++;
    }
}

/**
 * Render all the objects in the map
 * 
 * @param pM The map
 */
void map_drawObjs(map *pM) {
    int i;
    
    // Draw every object
    i = 0;
    while (i < pM->objsUsed) {
        obj_draw(pM->objs[i]);
        i++;
    }
}

/**
 * Get a list of objects for the map's collideable area
 * 
 * @param ppObjs List of objects
 * @param pLen Number of valid objects on the list
 * @param pM The map
 */
GFraMe_ret map_getWalls(GFraMe_object **ppObjs, int *pLen, map *pM) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(ppObjs, GFraMe_ret_bad_param);
    ASSERT(pLen, GFraMe_ret_bad_param);
    ASSERT(pM, GFraMe_ret_bad_param);
    
    // If there's a camera, return only the visible objects?
    *ppObjs = pM->walls;
    *pLen = pM->wallsUsed;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Check if a sprite triggered any event
 * 
 * @param pM The map
 * @param pSpr The sprite
 */
void map_checkEvents(map *pM, GFraMe_sprite *pSpr) {
    int i;
    
    // Simply check every event
    i = 0;
    while (i < pM->evsUsed) {
        event_check(pM->evs[i], pSpr);
        i++;
    }
}

/**
 * Collide a object against every one in the map
 * 
 * @param pM The map
 * @param pObj The object
 */
void map_collideObjects(map *pM, GFraMe_object *pObj) {
    int i;
    
    // Simply check every event
    i = 0;
    while (i < pM->evsUsed) {
        obj_collide(pM->objs[i], pObj);
        i++;
    }
}

//============================================================================//
//                                                                            //
// Static functions implementation                                            //
//                                                                            //
//============================================================================//

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
        case TILE_SHOCK_L3: { // 12 fps
            if (pT->elapsed >= 83) {
                tile++;
                pT->elapsed -= 83;
            }
        } break;
        case TILE_SHOCK_L4: { // 12 fps
            if (pT->elapsed >= 83) {
                tile = TILE_SHOCK_L1;
                pT->elapsed -= 83;
            }
        } break;
        case TILE_SHOCK_R1:
        case TILE_SHOCK_R2:
        case TILE_SHOCK_R3: { // 12 fps
            if (pT->elapsed >= 83) {
                tile++;
                pT->elapsed -= 83;
            }
        } break;
        case TILE_SHOCK_R4: { // 12 fps
            if (pT->elapsed >= 83) {
                tile = TILE_SHOCK_R1;
                pT->elapsed -= 83;
            }
        } break;
        default: {}
    }
    
    pM->data[pT->pos] = tile;
}

/**
 * Check which tiles should be animated and add to the list
 * 
 * @param pM The map
 * @return GFraMe error code
 */
static GFraMe_ret map_genAnimatedTiles(map *pM) {
    GFraMe_ret rv;
    int i;
    
    i = 0;
    while (i < pM->w*pM->h) {
        unsigned char t;
        
        t = pM->data[i];
        if (map_tileIsAnimated(t) == GFraMe_ret_ok) {
            animTile *tile;
            
            // Expand the buffer as necessary
            if (pM->animTilesUsed >= pM->animTilesLen) {
                rv = map_setAnimTilesMinLength(pM, pM->animTilesLen*2);
                ASSERT(rv == GFraMe_ret_ok, rv);
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
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Check whether a tile is already in a wall object
 * 
 * @param pM The map
 * @param pos Position of the tile to be tested
 * @return GFraMe_ret_ok if it is, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_isTileInWall(map *pM, int pos) {
    int i, x, y;
    
    // Retrieve horizontal and vertical position of the tile
    x = pos % pM->w;
    y = pos / pM->w;
    
    // Check against every object
    i = 0;
    while (i < pM->wallsUsed) {
        GFraMe_object *obj;
        GFraMe_hitbox *hb;
        int iniX, iniY, endX, endY;
        
        // Get both the object and the boundings
        obj = &pM->walls[i];
        hb = GFraMe_object_get_hitbox(obj);
        
        iniX = obj->x / 8;
        endX = (obj->x + hb->cx + hb->hw) / 8 - 1;
        iniY = obj->y / 8;
        endY = (obj->y + hb->cy + hb->hh) / 8 - 1;
        
        // Check if the tile is inside this object
        if (x >= iniX && x <= endX && y >= iniY && y <= endY)
            return GFraMe_ret_ok;
        
        i++;
    }
    
    return GFraMe_ret_failed;
}

/**
 * Check whether a tile is a wall
 * 
 * @param tile The tile
 * @return GFraMe_ret_ok on sucess, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_isWall(unsigned char tile) {
    switch (tile) {
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 104:
        case 105:
        case 106:
        case 107:
        case 108:
        case 109:
        case 110:
        case 136:
        case 137:
        case 138:
        case 139:
        case 140:
        case 141:
        case 142:
            return GFraMe_ret_ok;
        default:
            return GFraMe_ret_failed;
    }
}

/**
 * Get the bounds of a wall. The wall will be the widest possible.
 * (i.e., first the width is calculated and then the height)
 * 
 * @param pX Return the horizontal position
 * @param pY Return the vertical position
 * @param pW Return the width
 * @param pH Return the height
 * @param pM The map
 * @param pos First tile in the wall
 */
static void map_getWallBounds(int *pX, int *pY, int *pW, int *pH, map *pM,
    int pos) {
    int i, h, w, x, y;
    
    // Get the (x, y) tile position
    x = pos % pM->w;
    y = pos / pM->w;
    
    // Search for the first 'non-wall' on the horizontal
    i = 0;
    while (x + i < pM->w) {
        unsigned char tile;
        
        tile = pM->data[pos + i];
        
        if (map_isWall(tile) != GFraMe_ret_ok)
            break;
        
        i++;
    }
    w = i;
    
    // Search for the lowest height
    i = 0;
    h = 0x7fffffff;
    while (i < w) {
        int j;
        unsigned char tile;
        
        // Check which tile isn't a wall, anymore
        j = 0;
        while (j + y < pM->h) {
            tile = pM->data[pos + i + j * pM->w];
            if (map_isWall(tile) != GFraMe_ret_ok)
                break;
            j++;
        }
        // Update the height, if necessary
        if (j < h)
            h = j;
        // Check the next tile
        i++;
    }
    
    // Set the return variables
    *pX = (pos % pM->w) * 8;
    *pY = (pos / pM->w) * 8;
    *pW = w * 8;
    *pH = h * 8;
}

/**
 * Calculate where the walls should be placed
 * 
 * @param pM The map
 * @return GFraMe error code
 */
static GFraMe_ret map_genWalls(map *pM) {
    GFraMe_ret rv;
    int i;
    
    // Traverse every tile
    i = -1;
    while (++i < pM->w*pM->h) {
        GFraMe_object *obj;
        GFraMe_hitbox *hb;
        int h, w, x, y;
        
        // Only check if the tile is a wall
        if (map_isWall(pM->data[i]) != GFraMe_ret_ok)
            continue;
    
        // Check if it already belongs to a wall object
        if (map_isTileInWall(pM, i) == GFraMe_ret_ok)
            // If it does, go to the next one
            continue;
        
        // Otherwise, find the wall bounds...
        map_getWallBounds(&x, &y, &w, &h, pM, i);
        
        // ... and add it (but, first, expand the buffer as necessary)
        if (pM->wallsUsed >= pM->wallsLen) {
            rv = map_setWallsMinLength(pM, pM->wallsLen * 2);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        
        obj = &pM->walls[pM->wallsUsed];
        hb = GFraMe_object_get_hitbox(obj);
        
        GFraMe_object_clear(obj);
        GFraMe_object_set_x(obj, x);
        GFraMe_object_set_y(obj, y);
        GFraMe_hitbox_set(hb, GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, w, h);
        
        // Increase the objects count
        pM->wallsUsed++;
    }
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
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
 * Realloc the walls buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setWallsMinLength(map *pM, int len) {
    GFraMe_ret rv;
    
    // Do nothing if the buffer is already big enough
    ASSERT(pM->wallsLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    pM->walls = (GFraMe_object*)realloc(pM->walls, sizeof(GFraMe_object) * len);
    ASSERT(pM->walls, GFraMe_ret_memory_error);
    pM->wallsLen = len;
    
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
    ASSERT(pM->objsLen < len, GFraMe_ret_ok);
    
    // Expand the buffer
    i = pM->objsLen;
    pM->objs = (object**)realloc(pM->objs, sizeof(object*) * len);
    ASSERT(pM->objs, GFraMe_ret_memory_error);
    pM->objsLen = len;
    
    // Initialize every uninitialize event
    while (i < len) {
        pM->objs[i] = NULL;
        
        rv = obj_getNew(&pM->objs[i]);
        ASSERT(pM->objs[i], rv);
        i++;
    }
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

