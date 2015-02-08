/**
 * @file src/objects.h
 * 
 * Keep track of all active objects, updating and drawing then
 */
#ifndef __OBJECTS_H_
#define __OBJECTS_H_

#include <GFraMe/GFraMe_object.h>

typedef struct stObject object;

/**
 * Initialize this submodule
 * 
 * @return GFraMe error code
 */
GFraMe_ret objs_init();

/**
 * Clean up this submodule
 */
void objs_clean();

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
 * Assign an unique ID (relative to the actives ones) to this object
 * 
 * @param pObj The object
 * @param ID The ID
 * @return GFraMe error code
 */
GFraMe_ret objs_setID(object *pObj, int ID);

/**
 * Assign a state to this object.
 * e.g ST_OPEN, ST_OPENING, ST_CLOSED, ST_CLOSING (for a door or chest)
 * 
 * @param pObj The object
 * @param state The state
 */
void objs_setState(object *pObj, int state);

/**
 * Retrieve a list with the actives objects bounds
 * 
 * @param ppObjs The list of objects
 * @param pLen How many objects there are in the list
 * @return GFraMe error code
 */
GFraMe_ret objs_getCollideList(GFraMe_object **ppObjs, int *pLen);

#endif

