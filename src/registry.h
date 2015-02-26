/**
 * @file src/registry.h
 * 
 * Module to hold every needed reference
 */
#ifndef __REGISTRY_H_
#define __REGISTRY_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "event.h"
#include "mob.h"
#include "object.h"

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

/**
 * Retrieve the next mob (and expand the buffer as necessary)
 * Note that the mob must be pushed later
 * 
 * @param ppE Returns the mob
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextMob(mob **ppM);

/**
 * Push the last mob (increasing its counter)
 */
void rg_pushMob();

/**
 * Update every mob
 * 
 * @param ms Time elapse from the previous frame, in milliseconds
 */
void rg_updateMobs(int ms);

/**
 * Render every mob
 */
void rg_drawMobs();

/**
 * Add every mob to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddMob();

#endif

