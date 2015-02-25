/**
 * @file src/registry.h
 * 
 * Module to hold every needed reference
 */
#ifndef __REGISTRY_H_
#define __REGISTRY_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

/**
 * Initialize every buffer
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_init();

/**
 * Clean up every buffer
 */
void rg_clean();

/**
 * Reset every buffer
 */
void rg_reset();

/**
 * Retrieve the next event (and expand the buffer as necessary)
 * Note that the event must be pushed later
 * 
 * @param ppE Returns the event
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextEvent(event **ppE);

/**
 * Push the last event (increasing its counter)
 */
void rg_pushEvent();

/**
 * Add all events to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddEvents();

/**
 * Retrieve the next valid event (expanding the buffer as necessary)
 * 
 * @param ppO Returns the object
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextObject(object **ppO);

/**
 * Push the last object (i.e, increase the counter)
 */
void rg_pushObject();

/**
 * Update every object
 * 
 * @param ms Time elapse from the previous frame, in milliseconds
 */
void rg_updateObjects(int ms);

/**
 * Render every object
 */
void rg_drawObjects();

/**
 * Add every object to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddObjects();

/**
 * Retrieve the next valid wall (expanding the buffer as necessary)
 * 
 * @param ppWall Returns the wall
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextWall(GFraMe_object **ppWall);

/**
 * Push the last wall (i.e, increase the counter)
 */
void rg_pushWall();

/**
 * Add every wall to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddWalls();

/**
 * Return how many walls there currently is
 * 
 * @return Used wall objects
 */
int rg_getWallsUsed();

/**
 * Get a wall
 * 
 * @param num The wall's index
 * @return The gotten wall
 */
GFraMe_object* rg_getWall(int num);

/**
 * Collide every wall against an object
 * 
 * @param pObj The colliding object
 */
void rg_collideObjWall(GFraMe_object *pObj);


#endif

