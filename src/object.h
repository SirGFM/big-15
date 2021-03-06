/**
 * @file src/object.h
 * 
 * Keep track of all active objects, updating and drawing then
 */
#ifndef __OBJECTS_H_
#define __OBJECTS_H_

#include <GFraMe/GFraMe_object.h>

#include "commonEvent.h"
#include "globalVar.h"
#include "types.h"

enum {
    OBJ_VAR1 = 0,
    OBJ_VAR2,
    OBJ_VAR3,
    OBJ_VAR4,
    OBJ_VAR_MAX
};

typedef enum {
    OBJ_ANIM_DOOR_OPEN,
    OBJ_ANIM_DOOR_CLOSED,
    OBJ_ANIM_DOOR_OPENING,
    OBJ_ANIM_DOOR_CLOSING,
    OBJ_ANIM_DOOR_HOR_OPEN,
    OBJ_ANIM_DOOR_HOR_CLOSED,
    OBJ_ANIM_DOOR_HOR_OPENING,
    OBJ_ANIM_DOOR_HOR_CLOSING,
    OBJ_ANIM_MAXHP_UP_ON,
    OBJ_ANIM_MAXHP_UP_OFF,
    OBJ_ANIM_HJUMP_ON,
    OBJ_ANIM_HJUMP_OFF,
    OBJ_ANIM_TELEP_ON,
    OBJ_ANIM_TELEP_OFF,
    OBJ_ANIM_SIGNL_ON,
    OBJ_ANIM_SIGNL_OFF,
    OBJ_ANIM_TERM_ON,
    OBJ_ANIM_TERM_OFF,
    OBJ_ANIM_MAX
} objAnim;

typedef struct stObject object;

/**
 * Alloc a new object
 * 
 * @param ppObj Returned object
 * @return GFraMe error code
 */
GFraMe_ret obj_getNew(object **ppObj);

/**
 * Clean up the object
 * 
 * @param ppObj The object
 */
void obj_clean(object **ppObj);

/**
 * Make this a "empty" object
 * 
 * @param pObj The retrived object
 * @return GFraMe error code
 */
GFraMe_ret obj_setZero(object *pObj);

/**
 * Assign a object's dimension and position
 * 
 * @param pObj The object
 * @param x The object's horizontal position
 * @param y The object's vertical position
 * @param w The object's width
 * @param h The object's height
 */
void obj_setBounds(object *pObj, int x, int y, int w, int h);

/**
 * Add a flag to the object
 * 
 * @param pObj The object
 * @param f The flag
 */
void obj_addFlag(object *pObj, flag f);

/**
 * Remove a flag to the object
 * 
 * @param pObj The object
 * @param f The flag
 */
void obj_rmFlag(object *pObj, flag f);

/**
 * Assign an ID to this object
 * * @param pObj The object
 * @param ID The ID
 * @return GFraMe error code
 */
void obj_setID(object *pObj, int ID);

/**
 * Get the object's ID
 * 
 * @param pID The ID
 * @param pObj The object
 */
void obj_getID(int *pID, object *pObj);

/**
 * Set the object's current tile
 * 
 * @param pID The ID
 * @param tile The tile
 */
void obj_setTile(object *pObj, int tile);

/**
 * Set a common event to be run by this object
 * 
 * @param pObj The object
 * @param ce The common event
 */
void obj_setCommonEvent(object *pObj, commonEvent ce);

/**
 * Set an object's variable
 * 
 * @param pObj The object
 * @param index The variable index (on the object)
 * @param var The actual variable
 */
GFraMe_ret obj_setVar(object *pObj, int index, globalVar var);

/**
 * Get an object's variable
 * 
 * @param pGv The actual variable
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void obj_getVar(globalVar *pGv, object *pObj, int index);

/**
 * Update every object
 * 
 * @param pObj The object
 * @param ms Time elapsed, in milliseconds, from last frame
 */
void obj_update(object *pObj, int ms);

/**
 * Draw every object
 * 
 * @param pObj The object
 */
void obj_draw(object *pObj);

/**
 * Collide a object against this
 * 
 * @param pObj The object
 * @param pGFMobj The other object
 */
void obj_collide(object *pObj, GFraMe_object *pGFMobj);

/**
 * Set an object's current animation
 * 
 * @param pObj The object
 * @param anim The animation
 */
void obj_setAnim(object *pObj, objAnim anim);

/**
 * Get an object's animation
 * 
 * @param pObj The object
 * @return The current animation
 */
objAnim obj_getAnim(object *pObj);

/**
 * Check whether the object's animation finished
 * 
 * @param pObj The object
 * @return Whether it finished or not
 */
int obj_animFinished(object *pObj);

/**
 * Get the object's object, for collision
 * 
 * @param ppObj Object's object
 * @param pObj The object
 */
void obj_getObject(GFraMe_object **ppObj, object *pObj);

#endif

