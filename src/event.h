/**
 * @file src/event.h
 * 
 * GFraMe_object wrapper. Handles event triggering and what should be called
 * on trigger.
 */
#ifndef __EVENT_H_
#define __EVENT_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_sprite.h>

#include "commonEvent.h"
#include "globalVar.h"
#include "types.h"

enum {
    EV_VAR1 = 0,
    EV_VAR2,
    EV_VAR3,
    EV_VAR4,
    EV_VAR_MAX
};

typedef struct stEvent event;

/**
 * Alloc a new event
 * 
 * @param ppEv Returned event
 * @return GFraMe error code
 */
GFraMe_ret event_getNew(event **ppEv);

/**
 * Initialize the event's fields
 * 
 * @param ev The event
 * @param x The event's horizontal position (in pixels)
 * @param y The event's vertical position (in pixels)
 * @param w The event's width (in pixels)
 * @param h The event's height (in pixels)
 * @param t The event's trigger
 * @param ce The common event to run when this is triggered
 * @return GFraMe error code
 */
GFraMe_ret event_setAll(event *ev, int x, int y, int w, int h, trigger t,
    commonEvent ce);

/**
 * Clean up the event
 * 
 * @param ppEv The event
 */
void event_clean(event **ppEv);

/**
 * Check if the event was triggered and call the appropriate callback
 * 
 * @param ev The event
 * @param spr The sprite to be tested
 */
void event_check(event *ev, GFraMe_sprite *spr);

/**
 * Move a object to stop touching this event
 * 
 * @param ev The event
 * @param obj The object
 */
void event_separate(event *ev, GFraMe_object *obj);

/**
 * Set an events's variable
 * 
 * @param pEv The event
 * @param index The variable index (on the object)
 * @param gv The actual variable
 */
GFraMe_ret event_setVar(event *pEv, int index, globalVar gv);

/**
 * Get an events's variable
 * 
 * @param pEv The event
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void event_getVar(globalVar *pGv, event *pEv, int index);

/**
 * Set an events's integer variable
 * 
 * @param pEv The event
 * @param index The variable index (on the event)
 * @param val The value
 */
GFraMe_ret event_iSetVar(event *pEv, int index, int val);

/**
 * Get an events's integer variable
 * 
 * @param pVal The variable value
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void event_iGetVar(int *pVal, event *pEv, int index);

/**
 * Get the event's object, for collision
 * 
 * @param ppObj Event's object
 * @param pEv The event
 */
void event_getObject(GFraMe_object **ppObj, event *pEv);

#endif

