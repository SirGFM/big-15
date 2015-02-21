/**
 * @file src/event.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>

#include "commonEvent.h"
#include "controller.h"
#include "event.h"
#include "global.h"
#include "globalVar.h"
#include "types.h"

struct stEvent {
    GFraMe_object obj;        /** Object for collision and positioning        */
    commonEvent ce;           /** Common event called when the event triggers */
    trigger t;                /** Trigger to start the event                  */
    int active;               /** Whether the event can trigger               */
    globalVar local[EV_VAR_MAX]; /** Local variables to be used on ce         */
    int iLocal[EV_VAR_MAX];   /** Local variables to be used on ce            */
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
    GFraMe_object_clear(&ev->obj);
    GFraMe_object_set_x(&ev->obj, x);
    GFraMe_object_set_y(&ev->obj, y);
    GFraMe_hitbox_set(GFraMe_object_get_hitbox(&ev->obj),
        GFraMe_hitbox_upper_left, 0, 0, w, h);
    ev->local[EV_VAR1] = GV_MAX;
    ev->local[EV_VAR2] = GV_MAX;
    ev->local[EV_VAR3] = GV_MAX;
    ev->local[EV_VAR4] = GV_MAX;
    ev->iLocal[EV_VAR1] = -1;
    ev->iLocal[EV_VAR2] = -1;
    ev->iLocal[EV_VAR3] = -1;
    ev->iLocal[EV_VAR4] = -1;
    
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
    (*ppEv)->t = 0;
    (*ppEv)->ce = 0;
    (*ppEv)->active = 0;
    
    GFraMe_object_clear(&(*ppEv)->obj);
    
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
    
    obj = 0;
    
    // Sanitize parameters
    ASSERT_NR(ev);
    ASSERT_NR(spr);
    ASSERT_NR(ev->active);

    // Check if the event can even be started
    ASSERT_NR(!(ev->t & IS_PLAYER) || (spr->id & ID_PL));
    ASSERT_NR(!(ev->t & IS_OBJ) || (spr->id & ID_OBJ));
    ASSERT_NR(!(ev->t & IS_MOB) || (spr->id & ID_MOB));
        
    // Store the previous state (since event doesn't count as regular col.
    obj = GFraMe_sprite_get_object(spr);
    last_col = obj->hit;
    obj->hit = 0;
    
    // Now, check if the sprite overlaps the event
    ASSERT_NR(GFraMe_object_overlap(&ev->obj, obj, GFraMe_dont_collide)
        == GFraMe_ret_ok);
    
    // Check if the collision was valid
    if (((ev->t & ON_PRESSED) && ctr_action(spr->id))
        || ((ev->t & ON_ENTER_LEFT) && (obj->hit & GFraMe_direction_left))
        || ((ev->t & ON_ENTER_RIGHT) && (obj->hit & GFraMe_direction_right))
        || ((ev->t & ON_ENTER_UP) && (obj->hit & GFraMe_direction_up))
        || ((ev->t & ON_ENTER_DOWN) && (obj->hit & GFraMe_direction_down))) {
        // Restore the previous state (before calling the callback)
        obj->hit = last_col;
        
        // Set the caller and the target and call the callback
        ce_setParam(CE_CALLER, ev);
        ce_setParam(CE_TARGET, spr);
        ce_callEvent(ev->ce);
        
        // 'Kill' the event
        if (!(ev->t & KEEP_ACTIVE))
            ev->active = 0;
    }
__ret:
    // Restore the previous state
    if (obj)
        obj->hit = last_col;
    
    return;
}

/**
 * Move a object to stop touching this event
 * 
 * @param ev The event
 * @param obj The object
 */
void event_separate(event *ev, GFraMe_object *obj) {
    GFraMe_object_overlap(&ev->obj, obj, GFraMe_first_fixed);
}

/**
 * Set an events's variable
 * 
 * @param pEv The event
 * @param index The variable index (on the object)
 * @param gv The actual variable
 */
GFraMe_ret event_setVar(event *pEv, int index, globalVar gv) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(pEv, GFraMe_ret_bad_param);
    ASSERT(index < EV_VAR_MAX, GFraMe_ret_bad_param);
    ASSERT(gv < GV_MAX, GFraMe_ret_bad_param);
    
    // Set the variable
    pEv->local[index] = gv;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get an events's variable
 * 
 * @param pEv The event
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void event_getVar(globalVar *pGv, event *pEv, int index) {
    *pGv = pEv->local[index];
}

/**
 * Set an events's integer variable
 * 
 * @param pEv The event
 * @param index The variable index (on the event)
 * @param val The value
 */
GFraMe_ret event_iSetVar(event *pEv, int index, int val) {
    GFraMe_ret rv;
    
    // Sanitize parameters
    ASSERT(pEv, GFraMe_ret_bad_param);
    ASSERT(index < EV_VAR_MAX, GFraMe_ret_bad_param);
    
    // Set the variable
    pEv->iLocal[index] = val;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get an events's integer variable
 * 
 * @param pVal The variable value
 * @param pObj The object
 * @param index The variable index (on the object)
 */
void event_iGetVar(int *pVal, event *pEv, int index) {
    *pVal = pEv->iLocal[index];
}

/**
 * Get the event's object, for collision
 * 
 * @param ppObj Event's object
 * @param pEv The event
 */
void event_getObject(GFraMe_object **ppObj, event *pEv) {
    *ppObj = &pEv->obj;
}

