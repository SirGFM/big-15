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

enum {
    OBJ_VAR1 = 0,
    OBJ_VAR2,
    OBJ_VAR3,
    OBJ_VAR4,
    OBJ_VAR_MAX
};

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
 * Return the next object in the list (and expand it as necessary)
 * 
 * @param ppObj The retrived object
 * @return GFraMe error code
 */
GFraMe_ret objs_getNextObj(object **ppObj);

/**
 * Update the list to account for the last object adition.
 * This function must be called after a objs_getNextObj call.
 */
void objs_pushLastObj();

/**
 * Assign a object's dimension and position
 * 
 * @param pObj The object
 * @param x The object's horizontal position
 * @param y The object's vertical position
 * @param w The object's width
 * @param h The object's height
 */
void objs_setBounds(object *pObj, int x, int y, int w, int h);

/**
 * Assign an ID to this object
 * 
 * @param pObj The object
 * @param ID The ID
 * @return GFraMe error code
 */
void objs_setID(object *pObj, int ID);

/**
 * Set a common event to be run by this object
 * 
 * @param pObj The object
 * @param ce The common event
 */
void objs_setCommonEvent(object *pObj, commonEvent ce);

/**
 * Set an object's variable
 * 
 * @param pObj The object
 * @param index The variable index (on the object)
 * @param var The actual variable
 */
GFraMe_ret objs_setVar(object *pObj, int index, globalVar var);

/**
 * Update every object
 * 
 * @param ms Time elapsed, in milliseconds, from last frame
 */
void objs_update(int ms);

/**
 * Draw every object
 */
void objs_draw();

/**
 * Retrieve a list with the actives objects bounds
 * 
 * @param ppObjs The list of objects
 * @param pLen How many objects there are in the list
 * @return GFraMe error code
 */
GFraMe_ret objs_getCollideList(GFraMe_object **ppObjs, int *pLen);

#endif

