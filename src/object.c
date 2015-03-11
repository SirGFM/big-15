/**
 * @file src/object.h
 * 
 * Keep track of all active objects, updating and drawing then
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include <stdio.h>
#include <string.h>

#include "camera.h"
#include "commonEvent.h"
#include "global.h"
#include "globalVar.h"
#include "object.h"
#include "types.h"

struct stObject {
    GFraMe_sprite spr;            /** Event's sprite (for rendering and collision  */
    commonEvent ce;               /** Common event to be called every sprite frame */
    globalVar local[OBJ_VAR_MAX]; /** Each event has 4 local global variables      */
    objAnim anim;                 /** The object's current animation               */
    /** Every possible animation, so it won't overlap another object's */
    GFraMe_animation obj_anim[OBJ_ANIM_MAX];
};

static GFraMe_animation _obj_anim[OBJ_ANIM_MAX];
/**
 * Store the animation in the following manner:
 *   _obj_animData[i][0] = FPS
 *   _obj_animData[i][1] = data len
 *   _obj_animData[i][2] = do loop
 *   _obj_animData[i]+3  = actual data
 */
static int _obj_animData[] = {
    0, 1, 0, 200,                             /* OBJ_ANIM_DOOR_OPEN        */
    0, 1, 0, 192,                             /* OBJ_ANIM_DOOR_CLOSED      */
    8, 8, 0, 192,193,192,193,192,194,195,200, /* OBJ_ANIM_DOOR_OPENING     */
    8, 8, 0, 200,195,194,192,193,192,193,192, /* OBJ_ANIM_DOOR_CLOSING     */
    0, 1, 0, 217,                             /* OBJ_ANIM_DOOR_HOR_OPEN    */
    0, 1, 0, 185,                             /* OBJ_ANIM_DOOR_HOR_CLOSED  */
    8, 8, 0, 185,193,185,193,185,201,209,217, /* OBJ_ANIM_DOOR_HOR_OPENING */
    8, 8, 0, 217,209,201,185,193,185,193,183, /* OBJ_ANIM_DOOR_HOR_CLOSING */
   15,10, 1, 160,160,160,160,160,160,161,160,160,162, /* OBJ_ANIM_MAXHP_UP */
    0
};
static int _obj_animInit = 0;

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
    
    if (!_obj_animInit) {
        int i, *pData;
        
        // Setup every animation
        i = 0;
        pData = _obj_animData;
        while (i < OBJ_ANIM_MAX) {
            GFraMe_animation_init(&_obj_anim[i], pData[0], pData + 3, pData[1],
                pData[2]);
            pData += 3 + pData[1];
            i++;
        }
        _obj_animInit = 1;
    }
    
    // Assign the object its own animations
    memcpy((*ppObj)->obj_anim, _obj_anim, sizeof(_obj_anim));
    
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
    pObj->anim = OBJ_ANIM_MAX;
    
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
    else if (w == 32 && h == 8)
        pSset = gl_sset32x8;
    else
        pSset = NULL;
    
    GFraMe_sprite_init(&pObj->spr, x, y, w, h, pSset, 0, 0);
}

/**
 * Add a flag to the object
 * 
 * @param pObj The object
 * @param f The flag
 */
void obj_addFlag(object *pObj, flag f) {
    pObj->spr.id |= f;
}

/**
 * Remove a flag to the object
 * 
 * @param pObj The object
 * @param f The flag
 */
void obj_rmFlag(object *pObj, flag f) {
    pObj->spr.id &= ~f;
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
    
    if (ID & ID_HEARTUP) {
        obj_setAnim(pObj, OBJ_ANIM_MAXHP_UP);
    }
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
    ASSERT(index < OBJ_VAR_MAX, GFraMe_ret_bad_param);
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
 * @param pObj The object
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
 * 
 * @param pObj The object
 */
void obj_draw(object *pObj) {
    //GFraMe_sprite_draw(&pObj->spr);
    if (!(pObj->spr.id & ID_HIDDEN))
        GFraMe_sprite_draw_camera(&pObj->spr, cam_x, cam_y, SCR_W, SCR_H);
}

/**
 * Collide a object against this
 * 
 * @param pObj The object
 * @param pGFMobj The other object
 */
void obj_collide(object *pObj, GFraMe_object *pGFMobj) {
    // Collide this object, but don't move it
    if ((pObj->spr.id & ID_STATIC) == ID_STATIC) {
        GFraMe_object *pO;
        
        pO = GFraMe_sprite_get_object(&pObj->spr);
        GFraMe_object_overlap(pO, pGFMobj, GFraMe_first_fixed);
    }
    else if ((pObj->spr.id & ID_MOVABLE) == ID_MOVABLE) {
        GFraMe_object *pO;
        
        pO = GFraMe_sprite_get_object(&pObj->spr);
        GFraMe_object_overlap(pO, pGFMobj, GFraMe_second_fixed);
    }
}

/**
 * Set an object's current animation
 * 
 * @param pObj The object
 * @param anim The animation
 */
void obj_setAnim(object *pObj, objAnim anim) {
    if (pObj->anim != anim) {
        GFraMe_sprite_set_animation(&pObj->spr, &(pObj->obj_anim[anim]), 0);
        pObj->anim = anim;
    }
}

/**
 * Get an object's animation
 * 
 * @param pObj The object
 * @return The current animation
 */
objAnim obj_getAnim(object *pObj) {
    return pObj->anim;
}

/**
 * Check whether the object's animation finished
 * 
 * @param pObj The object
 * @return Whether it finished or not
 */
int obj_animFinished(object *pObj) {
    return pObj->spr.anim == NULL;
}

/**
 * Get the object's object, for collision
 * 
 * @param ppObj Object's object
 * @param pObj The object
 */
void obj_getObject(GFraMe_object **ppObj, object *pObj) {
    *ppObj = GFraMe_sprite_get_object(&pObj->spr);
}

