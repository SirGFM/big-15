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
#include "mob.h"
#include "object.h"
#include "parser.h"
#include "registry.h"

#include "quadtree/quadtree.h"

//============================================================================//
//                                                                            //
// Maps lookup table                                                          //
//                                                                            //
//============================================================================//

#define TM_MAX 10
/** Lookup table for tilemap files */
char *_map_tms[TM_MAX] = {
    "maps/map000.gfm",
    "maps/map001.gfm",
    "maps/map002.gfm",
    "maps/map003.gfm",
    "maps/map004.gfm",
    "maps/map005.gfm",
    "maps/map006.gfm",
    "maps/map007.gfm",
    "maps/map008.gfm",
    "maps/map009.gfm"
};
//============================================================================//
//                                                                            //
// Tiles definitions                                                          //
//                                                                            //
//============================================================================//

#define TILE_SHOCK_L1 96
#define TILE_SHOCK_L2 97
#define TILE_SHOCK_L3 98
#define TILE_SHOCK_L4 99
#define TILE_SHOCK_R1 128
#define TILE_SHOCK_R2 129
#define TILE_SHOCK_R3 130
#define TILE_SHOCK_R4 131
#define TILE_PC1_1 132
#define TILE_PC1_2 168
#define TILE_PC1_3 171
#define TILE_PC2_1 133
#define TILE_PC2_2 169
#define TILE_PC2_3 172
#define TILE_PC3_1 134
#define TILE_PC3_2 170
#define TILE_PC3_3 173
#define TILE_PC4_1 164
#define TILE_PC4_2 200
#define TILE_PC4_3 203
#define TILE_PC5_1 165
#define TILE_PC5_2 201
#define TILE_PC5_3 204
#define TILE_PC6_1 166
#define TILE_PC6_2 202
#define TILE_PC6_3 205

#define TILE_JB1_1 226
#define TILE_JB1_2 228
#define TILE_JB2_1 227
#define TILE_JB2_2 229
#define TILE_JB3_1 230
#define TILE_JB3_2 232
#define TILE_JB4_1 231
#define TILE_JB4_2 233

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
    int doReset;             /** Whether the walls should be reset            */
    
    animTile *animTiles;     /** List of animated tiles in the tilemap's data */
    int animTilesLen;        /** Size of the list of animated tiles           */
    int animTilesUsed;       /** Number of animated tiles on the current      */
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
 * Realloc the animTiles buffer as to have at least 'len' members
 * 
 * @param pM The map
 * @param len The new minimum length
 * @return GFraMe error code
 */
static GFraMe_ret map_setAnimTilesMinLength(map *pM, int len);

/**
 * Check whether a tile is a wall
 * 
 * @param tile The tile
 * @return GFraMe_ret_ok on sucess, GFraMe_ret_failed otherwise
 */
static GFraMe_ret map_isWall(unsigned char tile);

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
    pM->doReset = 0;
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
    // Sanitize parameters
    ASSERT_NR(ppM);
    ASSERT_NR(*ppM);
    
    if ((*ppM)->data)
        free((*ppM)->data);
    if ((*ppM)->animTiles)
        free((*ppM)->animTiles);
    
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
    pM->animTilesUsed = 0;
    
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
    
    pM->doReset = 1;
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
 * Load a indexed map
 * 
 * @param m The map
 * @param i The map's index
 * @return GFraMe error code
 */
GFraMe_ret map_loadi(map *m, int i) {
    #define MAX_NAME_LEN 128
    char name[MAX_NAME_LEN];
    GFraMe_ret rv;
    int len;
    
    // Check that the index is valid
    GFraMe_assertRV(i >=0 && i < TM_MAX, "Invalid map index", rv = GFraMe_ret_failed,
        __ret);
    
    // Retrive a valid asset filename
    len = MAX_NAME_LEN;
	rv = GFraMe_assets_clean_filename(name, _map_tms[i], &len);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load map", __ret);
    
    // Load the map
    rv = map_loadf(m, name);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to load map", __ret);
    
    rv = GFraMe_ret_ok;
__ret:
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
    
    if (pM->doReset) {
        GFraMe_ret rv;
        
        rg_resetWall();
        rv = map_genWalls(pM);
        ASSERT_NR(rv == GFraMe_ret_ok);
        // TODO return the error
        
        pM->doReset = 0;
    }
    // Update every animated tile
    i = 0;
    while (i < pM->animTilesUsed) {
        animTile *t;
        
        t = &pM->animTiles[i];
        map_animateTile(pM, t, ms);
        
        i++;
    }
    
__ret:
    return;
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
 * Get a map's dimensions (in pixels)
 * 
 * @param pM The map
 * @param pW The map's width
 * @param pH The map's height
 */
void map_getDimensions(map *pM, int *pW, int *pH) {
    *pW = pM->w * 8;
    *pH = pM->h * 8;
}

/**
 * Check if a position (in pixels) is solid or not
 * 
 * @param pM The map
 * @param x The horizontal position
 * @param y The vertical position
 * @return GFraMe_ret_ok on success
 */
GFraMe_ret map_isPixelSolid(map *pM, int x, int y) {
    GFraMe_ret rv;
    int tx, ty;
    unsigned char tile;
    
    // Get the current tile
    tx = x / 8;
    ty = y / 8;
    ASSERT(tx < pM->w, GFraMe_ret_failed);
    ASSERT(ty < pM->h, GFraMe_ret_failed);
    // Use the pixel position to account for [-7, -1] values
    ASSERT(x >= 0, GFraMe_ret_failed);
    ASSERT(y >= 0, GFraMe_ret_failed);
    
    // Get the tile and check it
    tile = pM->data[tx + ty*pM->w];
    rv = map_isWall(tile);
__ret:
    return rv;
}

/**
 * Check if a position (in tiles) is solid or not
 * 
 * @param pM The map
 * @param i The horizontal position
 * @param j The vertical position
 * @return GFraMe_ret_ok on success
 */
GFraMe_ret map_isTileSolid(map *pM, int i, int j) {
    GFraMe_ret rv;
    unsigned char tile;
    
    // Check that it's inbound
    ASSERT(i < pM->w, GFraMe_ret_failed);
    ASSERT(j < pM->h, GFraMe_ret_failed);
    
    // Get the tile and check it
    tile = pM->data[i + j*pM->w];
    rv = map_isWall(tile);
__ret:
    return rv;
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
        case TILE_PC1_1:
        case TILE_PC2_1:
        case TILE_PC3_1:
        case TILE_PC4_1:
        case TILE_PC5_1:
        case TILE_PC6_1:
        case TILE_PC1_2:
        case TILE_PC2_2:
        case TILE_PC3_2:
        case TILE_PC4_2:
        case TILE_PC5_2:
        case TILE_PC6_2:
        case TILE_JB1_1:
        case TILE_JB1_2:
        case TILE_JB2_1:
        case TILE_JB2_2:
        case TILE_JB3_1:
        case TILE_JB3_2:
        case TILE_JB4_1:
        case TILE_JB4_2:
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
        case TILE_PC1_1:
        case TILE_PC2_1:
        case TILE_PC3_1:
        case TILE_PC4_1:
        case TILE_PC5_1:
        case TILE_PC6_1: { // 6 fps
            if (pT->elapsed >= 166) {
                tile += TILE_PC1_2 - TILE_PC1_1;
                pT->elapsed -= 166;
            }
        } break;
        case TILE_PC1_2:
        case TILE_PC2_2:
        case TILE_PC3_2:
        case TILE_PC4_2:
        case TILE_PC5_2:
        case TILE_PC6_2: { // 6 fps
            if (pT->elapsed >= 166) {
                tile += 3;
                pT->elapsed -= 166;
            }
        } break;
        case TILE_PC1_3:
        case TILE_PC2_3:
        case TILE_PC3_3:
        case TILE_PC4_3:
        case TILE_PC5_3:
        case TILE_PC6_3: { // 3 fps
            if (pT->elapsed >= 333) {
                tile -= TILE_PC1_2 - TILE_PC1_1 + 3;
                pT->elapsed -= 333;
            }
        } break;
        case TILE_JB1_1:
        case TILE_JB2_1:
        case TILE_JB3_1:
        case TILE_JB4_1: { // 12 fps
            if (pT->elapsed >= 83) {
                tile += TILE_JB1_2 - TILE_JB1_1;
                pT->elapsed -= 83;
            }
        } break;
        case TILE_JB1_2:
        case TILE_JB2_2:
        case TILE_JB3_2:
        case TILE_JB4_2: { // 12 fps
            if (pT->elapsed >= 83) {
                tile -= TILE_JB1_2 - TILE_JB1_1;
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
    while (i < rg_getWallsUsed()) {
        GFraMe_object *obj;
        GFraMe_hitbox *hb;
        int iniX, iniY, endX, endY;
        
        // Get both the object and the boundings
        obj = rg_getWall(i);
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
        // ... and add it
        rv = rg_getNextWall(&obj);
        ASSERT(rv == GFraMe_ret_ok, rv);
        
        hb = GFraMe_object_get_hitbox(obj);
        
        GFraMe_object_clear(obj);
        GFraMe_object_set_x(obj, x);
        GFraMe_object_set_y(obj, y);
        GFraMe_hitbox_set(hb, GFraMe_hitbox_upper_left, 0/*x*/, 0/*y*/, w, h);
        
        // Increase the objects count
        rg_pushWall();
    }
    
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

