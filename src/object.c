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
    GFraMe_sprite spr;  /** Event's sprite (for rendering and collision  */
    commonEvent ce;     /** Common event to be called every sprite frame */
    globalVar local[OBJ_VAR_MAX]; /** Each event has 4 local global variables      */
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

#if 0
/**
 * Return the next object in the list (and expand it as necessary)
 * 
 * @param ppObj The retrived object
 * @return GFraMe error code
 */
GFraMe_ret objs_getNextObj(object **ppObj) {
    GFraMe_ret rv;
    
    // Expand the list, if necessary
    if (objsUsed >= objsLen) {
        objsLen *= 2;
        
        pObjs = (object*)realloc(pObjs, sizeof(object)*objsLen);
        ASSERT(pObjs, GFraMe_ret_memory_error);
    }
    
    // Clean up this object
    pObjs[objsUsed].ce = CE_MAX;
    pObjs[objsUsed].local[0] = GV_MAX;
    pObjs[objsUsed].local[1] = GV_MAX;
    pObjs[objsUsed].local[2] = GV_MAX;
    pObjs[objsUsed].local[3] = GV_MAX;
    
    // Retrive the new object
    *ppObj = &pObjs[objsUsed];
    didGetObj = 1;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}
#endif

/**
 * Assign a object's dimension and position
 * 
 * @param pObj The object
 * @param x The object's horizontal position
 * @param y The object's vertical position
 * @param w The object's width
 * @param h The object's height
 */
void objs_setBounds(object *pObj, int x, int y, int w, int h) {
    GFraMe_spriteset *pSset;
    
    // Check which tileset to use
    if (w == 8 && h == 8)
        pSset = gl_sset8x8;
    else if (w == 8 && h == 16)
        pSset = gl_sset8x16;
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
void objs_setID(object *pObj, int ID) {
    // Make sure the object is correctly flagged
    ID &= ~ID_PL;
    ID &= ~ID_MOB;
    ID |= ID_OBJ;
    
    // Set the ID
    pObj->spr.id = ID;
}

/**
 * Set a common event to be run by this object
 * 
 * @param pObj The object
 * @param ce The common event
 */
void objs_setCommonEvent(object *pObj, commonEvent ce) {
    pObj->ce = ce;
}

/**
 * Set an object's variable
 * 
 * @param pObj The object
 * @param index The variable index (on the object)
 * @param var The actual variable
 */
GFraMe_ret objs_setVar(object *pObj, int index, globalVar gv) {
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

#if 0
/**
 * Update every object
 * 
 * @param ms Time elapsed, in milliseconds, from last frame
 */
void objs_update(int ms) {
    int i;
    
    i = 0;
    while (i < objsUsed) {
        // Update the object
        GFraMe_sprite_update(&pObjs[i].spr, ms);
        // And call its event, if any
        if (pObjs[i].ce != CE_MAX) {
            // Call the object's event, with itself as the parameter
            ce_setParam(CE_CALLER, &pObjs[i]);
            ce_callEvent(pObjs[i].ce);
        }
        
        i++;
    }
}

/**
 * Draw every object
 */
void objs_draw() {
    int i;
    
    // Draw every sprite
    i = 0;
    while (i < objsUsed) {
        GFraMe_sprite_draw(&pObjs[i].spr);
        i++;
    }
}


/**
 * Retrieve a list with the actives objects bounds
 * 
 * @param ppObjs The list of objects
 * @param pLen How many objects there are in the list
 * @return GFraMe error code
 */
GFraMe_ret objs_getCollideList(GFraMe_object **ppObjs, int *pLen);

#endif

