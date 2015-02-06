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

/**
 * Possible triggers
 */
typedef enum {
    /** Whether a object just touched the event from the left */
    ON_ENTER_LEFT  = 0x00000001,
    /** Whether a object just touched the event from the right */
    ON_ENTER_RIGHT = 0x00000002,
    /** Whether a object just touched the event from bellow */
    ON_ENTER_DOWN  = 0x00000004,
    /** Whether a object just touched the event from above */
    ON_ENTER_UP    = 0x00000008,
    /** Whether the triggering obj must be a player */
    IS_PLAYER     = 0x000000010,
    /** Whether the triggering obj must be a mob */
    IS_MOB        = 0x000000020,
    /** Whether the triggering obj must be a object */
    IS_OBJ        = 0x000000040,
    /** Whether a player is over it and the action button was pressed */
    ON_PRESSED     = 0x00000100,
    /** Don't deactive the event on activation */
    KEEP_ACTIVE    = 0x10000000,
    /** Trigger count */
    TRIGGER_MAX,
    /** Whether any object just touched the event */
    ON_ENTER          = ON_ENTER_LEFT | ON_ENTER_RIGHT | ON_ENTER_DOWN
                        | ON_ENTER_UP
} trigger;

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

#endif

