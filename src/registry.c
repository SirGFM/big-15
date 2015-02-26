/**
 * @file src/registry.c
 * 
 * Module to hold every needed reference
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include <stdlib.h>

#include "event.h"
#include "global.h"
#include "mob.h"
#include "object.h"
#include "player.h"
#include "registry.h"
#include "static_buffer.h"

#include "quadtree/quadtree.h"

#define EVENT_INC 4
#define OBJECT_INC 4
#define WALL_INC 8
#define MOB_INC 4

typedef GFraMe_object wall;
/**
 * Alloc a new GFraMe_object
 * 
 * @param ppObj The new object
 * @return GFraMe error code
 */
static GFraMe_ret rg_getNewGfmObj(GFraMe_object **ppObj);
/**
 * Free a GFraMe_object
 * 
 * @param ppObj The new object
 */
static void rg_cleanGfmObj(GFraMe_object **ppObj);

/** Define every variable buffer */
BUF_DEFINE(event);
BUF_DEFINE(object);
BUF_DEFINE(mob);
BUF_DEFINE(wall);

/**
 * Initialize every buffer
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_init() {
    GFraMe_ret rv;
    
    BUF_SET_MIN_SIZE(event, 4, GFraMe_ret_memory_error, event_getNew);
    BUF_SET_MIN_SIZE(object, 8, GFraMe_ret_memory_error, obj_getNew);
    BUF_SET_MIN_SIZE(wall, 8, GFraMe_ret_memory_error, rg_getNewGfmObj);
    BUF_SET_MIN_SIZE(mob, 4, GFraMe_ret_memory_error, mob_getNew);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up every buffer
 */
void rg_clean() {
    BUF_CLEAN(event, event_clean);
    BUF_CLEAN(object, obj_clean);
    BUF_CLEAN(wall, rg_cleanGfmObj);
    BUF_CLEAN(mob, mob_clean);
}

/**
 * Reset every buffer
 */
void rg_reset() {
    BUF_RESET(event);
    BUF_RESET(object);
    BUF_RESET(wall);
    BUF_RESET(mob);
}

/**
 * Retrieve the next event (and expand the buffer as necessary)
 * Note that the event must be pushed later
 * 
 * @param ppE Returns the event
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextEvent(event **ppE) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(event, EVENT_INC, *ppE, GFraMe_ret_memory_error, event_getNew);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Push the last event (increasing its counter)
 */
void rg_pushEvent() {
    BUF_PUSH(event);
}

/**
 * Add all events to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddEvents() {
    GFraMe_ret rv;
    
    BUF_CALL_ALL_RET(event, rv, qt_addEv);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Retrieve the next valid event (expanding the buffer as necessary)
 * 
 * @param ppO Returns the object
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextObject(object **ppO) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(object, OBJECT_INC, *ppO, GFraMe_ret_memory_error, obj_getNew);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Push the last object (i.e, increase the counter)
 */
void rg_pushObject() {
    BUF_PUSH(object);
}

/**
 * Update every object
 * 
 * @param ms Time elapse from the previous frame, in milliseconds
 */
void rg_updateObjects(int ms) {
    BUF_CALL_ALL(object, obj_update, ms);
}

/**
 * Render every object
 */
void rg_drawObjects() {
    BUF_CALL_ALL(object, obj_draw);
}

/**
 * Add every object to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddObjects() {
    GFraMe_ret rv;
    
    BUF_CALL_ALL_RET(object, rv, qt_addObj);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Alloc a new GFraMe_object
 * 
 * @param ppObj The new object
 * @return GFraMe error code
 */
static GFraMe_ret rg_getNewGfmObj(GFraMe_object **ppObj) {
    GFraMe_ret rv;

    BUF_ALLOC_OBJ(GFraMe_object, ppObj, GFraMe_ret_memory_error);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Free a GFraMe_object
 * 
 * @param ppObj The new object
 */
static void rg_cleanGfmObj(GFraMe_object **ppObj) {
    BUF_DEALLOC_OBJ(ppObj);
}

/**
 * Retrieve the next valid wall (expanding the buffer as necessary)
 * 
 * @param ppWall Returns the wall
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextWall(GFraMe_object **ppWall) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(wall, WALL_INC, *ppWall, GFraMe_ret_memory_error, rg_getNewGfmObj);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Push the last wall (i.e, increase the counter)
 */
void rg_pushWall() {
    BUF_PUSH(wall);
}

/**
 * Add every wall to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddWalls() {
    GFraMe_ret rv;
    
    BUF_CALL_ALL_RET(wall, rv, qt_addWall);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Return how many walls there currently is
 * 
 * @return Used wall objects
 */
int rg_getWallsUsed() {
    return BUF_GET_USED(wall);
}

/**
 * Get a wall
 * 
 * @param num The wall's index
 * @return The gotten wall
 */
GFraMe_object* rg_getWall(int num) {
    return BUF_GET_OBJECT(wall, num);
}

/**
 * Collide every wall against an object
 * 
 * @param pObj The colliding object
 */
void rg_collideObjWall(GFraMe_object *pObj) {
    BUF_CALL_ALL(wall, GFraMe_object_overlap, pObj, GFraMe_first_fixed);
}

/**
 * Retrieve the next mob (and expand the buffer as necessary)
 * Note that the mob must be pushed later
 * 
 * @param ppE Returns the mob
 * @return GFraMe error code
 */
GFraMe_ret rg_getNextMob(mob **ppM) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(mob, MOB_INC, *ppM, GFraMe_ret_memory_error, mob_getNew);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Push the last mob (increasing its counter)
 */
void rg_pushMob() {
    BUF_PUSH(mob);
}

/**
 * Update every mob
 * 
 * @param ms Time elapse from the previous frame, in milliseconds
 */
void rg_updateMobs(int ms) {
    BUF_CALL_ALL(mob, mob_update, ms);
}

/**
 * Render every mob
 */
void rg_drawMobs() {
    BUF_CALL_ALL(mob, mob_draw);
}

/**
 * Add every mob to the quadtree
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_qtAddMob() {
    GFraMe_ret rv;
    
    BUF_CALL_ALL_RET(mob, rv, qt_addMob);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

