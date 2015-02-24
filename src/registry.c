/**
 * @file src/registry.c
 * 
 * Module to hold every needed reference
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "event.h"
#include "global.h"
#include "mob.h"
#include "object.h"
#include "registry.h"
#include "static_buffer.h"

#include "quadtree/quadtree.h"

#define EVENT_INC 4
#define OBJECT_INC 4
#define MOB_INC 4

/** Define every variable buffer */
BUF_DEFINE(event);
BUF_DEFINE(object);
//BUF_DEFINE(mob);


/**
 * Initialize every buffer
 * 
 * @return GFraMe error code
 */
GFraMe_ret rg_init() {
    GFraMe_ret rv;
    
    BUF_SET_MIN_SIZE(event, 4, GFraMe_ret_memory_error, event_getNew);
    BUF_SET_MIN_SIZE(object, 8, GFraMe_ret_memory_error, obj_getNew);
//    BUF_SET_MIN_SIZE(mob, 4, GFraMe_ret_memory_error, mob_getNew);
    
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
//    BUF_CLEAN(mob, mob_clean);
}

/**
 * Reset every buffer
 */
void rg_reset() {
    BUF_RESET(event);
    BUF_RESET(object);
//    BUF_RESET(mob);
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

