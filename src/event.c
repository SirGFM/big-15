/**
 * @file src/event.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include "commonEvent.h"
#include "event.h"
#include "types.h"

struct stEvent {
    GFraMe_object obj; /** Object for collision and positioning        */
    commonEvent ce;    /** Common event called when the event triggers */
    trigger t;         /** Trigger to start the event                  */
    int active;        /** Whether the event can trigger               */
};

/**
 * Alloc a new event
 * 
 * @param ppEv Returned event
 * @return GFraMe error code
 */
GFraMe_ret event_getNew(event **ppEv) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    GFraMe_assertRV(ppEv, "No container passed!", rv = GFraMe_ret_bad_param,
        __ret);
    GFraMe_assertRV(!*ppEv, "Event already alloced!", rv = GFraMe_ret_bad_param,
        __ret);
    
    // Alloc the event
    *ppEv = (event*)malloc(sizeof(event));
    GFraMe_assertRV(*ppEv, "Failed to alloc!", rv = GFraMe_ret_memory_error,
        __ret);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

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
    commonEvent ce) {
    GFraMe_ret rv;
    
    // Check if a valid event can be created
    GFraMe_assertRV(ev, "Invalid event structure", rv = GFraMe_ret_bad_param,
        __ret);
    GFraMe_assertRV(w != 0, "Invalid event width", rv = GFraMe_ret_bad_param,
        __ret);
    GFraMe_assertRV(h != 0, "Invalid event height", rv = GFraMe_ret_bad_param,
        __ret);
    GFraMe_assertRV(t != 0 && t != KEEP_ACTIVE, "Invalid event trigger",
        rv = GFraMe_ret_bad_param, __ret);
    GFraMe_assertRV(ce < CE_MAX, "Invalid callback", rv = GFraMe_ret_bad_param,
        __ret);
    
    // Set the event's hitbox
    GFraMe_object_clear(ev->obj);
    GFraMe_object_set_x(ev->obj, x);
    GFraMe_object_set_y(ev->obj, y);
    GFraME_hitbox_set(GFraMe_object_get_hitbox(ev->obj),
        GFraMe_hitbox_upper_left, 0, 0, w, h);
    
    // Make sure that only players can start the event, if 'on_pressed' is set
    if (t & ON_PRESSED) {
        t &= ~IS_MOB;
        t &= ~IS_OBJ;
        
        t |= IS_PLAYER;
    }
    // Otherwise, make sure it can be triggered from some direction
    else if ((t & ON_ENTER) == 0) {
        t |= ON_ENTER;
    }
    ev->t = t;
    // Set the callback and activate the event
    ev->ce = ce;
    ev->active = 1;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Clean up the event
 * 
 * @param ppEv The event
 */
void event_clean(event **ppEv) {
    *ppEv->t = 0;
    *ppEv->ce = 0;
    *ppEv->active = 0;
    GFraMe_object_clear(*ppEv->obj);
    
    free(*ppEv);
    *ppEv = NULL;
}

/**
 * Check if the event was triggered and call the appropriate callback
 * 
 * @param ev The event
 * @param spr The sprite to be tested
 */
void event_check(event *ev, GFraMe_sprite *spr) {
    GFraMe_direction last_col;
    GFraMe_object *obj;
    
    // Define simpler assert
    #define ASSERT(rv) if (!(rv)) goto __ret;

    // Check if the event can even be started
    ASSERT(!(ev->t & IS_PLAYER) || (spr->id & PL_ID));
    ASSERT(!(ev->t & IS_OBJ) || (spr->id & OBJ_ID));
    ASSERT(!(ev->t & IS_MOB) || (spr->id & MOB_ID));
        
    // Store the previous state (since event doesn't count as regular col.
    obj = GFraMe_sprite_get_object(spr);
    last_col = obj->hit;
    obj->hit = 0;
    
    // Now, check if the sprite overlaps the event
    ASSERT(GFraMe_object_overlaps(ev->obj, obj, GFraMe_dont_collide)
        == GFraMe_ret_ok);
    
    // Check if the collision was valid
    if ((ev->t & ON_PRESSED) && 0/** TODO check_button */
        || (ev->t & ON_LEFT) && (obj->hit & GFraMe_direction_left)
        || (ev->t & ON_RIGHT) && (obj->hit & GFraMe_direction_right)
        || (ev->t & ON_UP) && (obj->hit & GFraMe_direction_up)
        || (ev->t & ON_DOWN) && (obj->hit & GFraMe_direction_down)) {
        // Restore the previous state (before calling the callback)
        obj->hit = last_col;
        
        // Set the caller and the target and call the callback
        ce_setParam(CE_CALLER, ev);
        ce_setParam(CE_TARGET, spr);
        ce_callEvent(t->ce);
        
        // 'Kill' the event
        if (!(ev->t & KEEP_ACTIVE))
            ev->active = 0;
    }
    else {
        // Restore the previous state
        obj->hit = last_col;
    }
    
__ret:
    return;
}

