/**
 * @file src/object.h
 * 
 * Keep track of all active objects, updating and drawing then
 */
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include <stdio.h>

#include "commonEvent.h"
#include "global.h"
#include "globalVar.h"
#include "object.h"
#include "types.h"

struct stObject {
    GFraMe_sprite spr;            /** Event's sprite (for rendering and collision  */
    commonEvent ce;               /** Common event to be called every sprite frame */
    globalVar local[OBJ_VAR_MAX]; /** Each event has 4 local global variables */
};

/**
 * Alloc a new object
 * 
 * @param ppObj Returned object
 * @return GFraMe error code
 */
GFraMe_ret obj_getNew(object **ppObj) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    GFraMe_assertRV(ppObj, "No container passed!", rv = GFraMe_ret_bad_param,
        __ret);
    GFraMe_assertRV(!*ppObj, "Event already alloced!", rv = GFraMe_ret_bad_param,
        __ret);
    
    // Alloc the event
    *ppObj = (object*)malloc(sizeof(object));
    GFraMe_assertRV(*ppObj, "Failed to alloc!", rv = GFraMe_ret_memory_error,
        __ret);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up the object
 * 
 * @param ppObj The object
 */
void obj_clean(object **ppObj) {
    ASSERT_NR(ppObj);
    ASSERT_NR(*ppObj);
    
    free(*ppObj);
    *ppObj = NULL;
__ret:
    return;
}

/**
 * Make this a "empty" object
 * 
 * @param pObj The retrived object
 * @return GFraMe error code
 */
GFraMe_ret obj_setZero(object *pObj) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(pObj, GFraMe_ret_bad_param);
    
    // Clean up this object
    pObj->ce = CE_MAX;
    pObj->local[0] = GV_MAX;
    pObj->local[1] = GV_MAX;
    pObj->local[2] = GV_MAX;
    pObj->local[3] = GV_MAX;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Assign a object's dimension and position
 * 
 * @param pObj The object
 * @param x The object's horizontal position
 * @param y The object's vertical position
 * @param w The object's width
 * @param h The object's height
 */
void obj_setBounds(object *pObj, int x, int y, int w, int h) {
    GFraMe_spriteset *pSset;
    
    // Check which tileset to use
    if (w == 8 && h == 8)
        pSset = gl_sset8x8;
    else if (w == 8 && h == 16)
        pSset = gl_sset8x16;
    else if (w == 8 && h == 32)
        pSset = gl_sset8x32;
    else if (w == 16 && h == 16)
        pSset = gl_sset16x16;
    else
        pSset = NULL;
    
    GFraMe_sprite_init(&pObj->spr, x, y, w, h, pSset, 0, 0);
}

/**
 * Assign an ID to this object
 * 
 * @param pObj The object
 * @param ID The ID
 * @return GFraMe error code
 */
void obj_setID(object *pObj, int ID) {
    // Make sure the object is correctly flagged
    ID &= ~ID_PL;
    ID &= ~ID_MOB;
    ID |= ID_OBJ;
    
    // Set the ID
    pObj->spr.id = ID;
}

/**
 * Get the object's ID
 * 
 * @param pID The ID
 * @param pObj The object
 */
void obj_getID(int *pID, object *pObj) {
    *pID = pObj->spr.id;
}

/**
 * Set the object's current tile
 * 
 * @param pID The ID
 * @param tile The tile
 */
void obj_setTile(object *pObj, int tile) {
    pObj->spr.cur_tile = tile;
}

/**
 * Set a common event to be run by this object
 * 
 * @param pObj The object
 * @param ce The common event
 */
void obj_setCommonEvent(object *pObj, commonEvent ce) {
    pObj->ce = ce;
}

/**
 * Set an object's variable
 * 
 * @param pObj The object
 * @param index The variable index (on the object)
 * @param var The actual variable
 */
GFraMe_ret obj_setVar(object *pObj, int index, globalVar gv) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(pObj, GFraMe_ret_bad_param);
    ASSERT(index < 4, GFraMe_ret_bad_param);
    ASSERT(gv < GV_MAX, GFraMe_ret_bad_param);
    
    // Set the variable
    pObj->local[index] = gv;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get an object's variable
 * 
 * @param pGv The actual variable
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void obj_getVar(globalVar *pGv, object *pObj, int index) {
    *pGv = pObj->local[index];
}

/**
 * Update every object
 * 
 * @param ms Time elapsed, in milliseconds, from last frame
 */
void obj_update(object *pObj, int ms) {
    GFraMe_sprite_update(&pObj->spr, ms);
    
    // Call the object's event, if any
    if (pObj->ce) {
        ce_setParam(CE_CALLER, pObj);
        ce_callEvent(pObj->ce);
    }
}

/**
 * Draw every object
 */
void obj_draw(object *pObj) {
    GFraMe_sprite_draw(&pObj->spr);
}

