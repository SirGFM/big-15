    /**
 * @file src/commonEvent.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "commonEvent.h"
#include "event.h"
#include "global.h"
#include "globalVar.h"
#include "object.h"
#include "player.h"
#include "types.h"

static char *_ce_names[CE_MAX+1] = {
    "ce_open_door",
    "ce_close_door",
    "ce_switch_door",
    "ce_handle_door",
    "ce_handle_notdoor",
    "ce_switch_map",
    "ce_get_item",
    "ce_max"
};

/** Object that caused the event to be called */
static void *_ce_caller = NULL;
/** Object associated with the caller */
static void *_ce_target = NULL;

/**
 * Set a parameter
 * 
 * @param p The parameter to be set
 * @param val Value the parameter should assume
 */
void ce_setParam(ce_params p, void *val) {
    GFraMe_assertRet(p < CE_PARAM_MAX, "Invalid common event param!", __ret);
    
    switch (p) {
        case CE_CALLER: _ce_caller = val; break;
        case CE_TARGET: _ce_target = val; break;
        default: {}
    }
    
__ret:
    return;
}

/**
 * Parse a common event from a file pointer.
 * The event name must be between '"'.
 * 
 * @param fp The file with the event
 * @return The parsed common event or CE_MAX, on error
 */
commonEvent ce_getEventFromFile(FILE *fp) {
    int i;
    commonEvent ce;
    fpos_t pos;
    
    GFraMe_assertRV(fp, "Invalid file!", ce = CE_MAX, __ret);
    
    // Get the current position, to "backtrack" on error
    i = fgetpos(fp, &pos);
    GFraMe_assertRV(i == 0, "Error getting file position!", ce = CE_MAX, __ret);
    
    // Check every event (yay, dumb strats!)
    ce = 0;
    while (ce < CE_MAX) {
        int c;
        
        
        // Check that the first character is a '"'
        c = fgetc(fp);
        GFraMe_assertRV(c != EOF, "Reached end-of-file!", ce = CE_MAX, __ret);
        GFraMe_assertRV(c == '"', "Invalid event name!", ce = CE_MAX, __ret);
        
        // Try to match every other character to the current event
        i = 0;
        while (1) {
            c = fgetc(fp);
            GFraMe_assertRV(c != EOF, "Reached end-of-file!", ce = CE_MAX,
                __ret);
            
            // Stop either at the string end or on an unmatch
            if (c == '"' || c != _ce_names[ce][i])
                break;
            
            i++;
        }
        
        // If the string ended, stop
        if (c == '"' && _ce_names[ce][i + 1] == '\0')
            break;
        
        // Return to the string's begin
        i = fsetpos(fp, &pos);
        GFraMe_assertRV(i == 0, "Error setting file position!", ce = CE_MAX,
            __ret);
        
        ce++;
    }
    
__ret:
    return ce;
}

/**
 * Get a event's name
 * 
 * @param ce The common event
 * @return The common event's name or NULL
 */
char* ce_getName(commonEvent ce) {
    if (ce >= CE_MAX) return 0;
    return _ce_names[ce];
}

/**
 * Call a common event
 * 
 * @param ce Common event to be called
 */
void ce_callEvent(commonEvent ce) {
    GFraMe_assertRet(ce < CE_MAX, "Invalid common event!", __ret);
    
    switch (ce) {
        case CE_OPEN_DOOR: {
            event *pE;
            globalVar gv;
            int val;
            
            // Get the current event and the related var
            pE = (event*)_ce_caller;
            event_getVar(&gv, pE, 0);
            
            // Switch the var's state
            val = gv_getValue(gv);
            if (val == CLOSED)
                gv_setValue(gv, OPENING);
        } break;
        case CE_CLOSE_DOOR: {
            event *pE;
            globalVar gv;
            int val;
            
            // Get the current event and the related var
            pE = (event*)_ce_caller;
            event_getVar(&gv, pE, 0);
            
            // Switch the var's state
            val = gv_getValue(gv);
            if (val == OPEN)
                gv_setValue(gv, CLOSING);
        } break;
        case CE_SWITCH_DOOR: {
            event *pE;
            globalVar gv;
            int val;
            
            // Get the current event and the related var
            pE = (event*)_ce_caller;
            event_getVar(&gv, pE, 0);
            
            // Switch the var's state
            val = gv_getValue(gv);
            if (val == OPEN)
                gv_setValue(gv, CLOSING);
            else if (val == CLOSED)
                gv_setValue(gv, OPENING);
        } break;
        case CE_HANDLE_DOOR:
        case CE_HANDLE_NOTDOOR: {
            globalVar gv;
            int ID, val, not;
            object *pO;
            
            // Retrieve the reference
            ASSERT_NR(_ce_caller);
            pO = (object*)_ce_caller;
            
            // Check that the object is actually a door
            obj_getID(&ID, pO);
            ASSERT_NR((ID & ID_DOOR) == ID_DOOR);
            
            // Whether everything should be negated or not
            not = (ce == CE_HANDLE_NOTDOOR);
            
            // Get the door's state
            obj_getVar(&gv, pO, 0);
            val = gv_getValue(gv);
            
            // Handle animation, collision and state
            if ((!not && val == CLOSED) || (not && val == OPEN)) {
                obj_addFlag(pO, ID_STATIC);
                obj_setTile(pO, 192);
            }
            else if ((!not && val == OPENING) || (not && val == CLOSING)) {
                if (obj_getAnim(pO) != OBJ_ANIM_OPEN_DOOR)
                    obj_setAnim(pO, OBJ_ANIM_OPEN_DOOR);
                else if (obj_animFinished(pO))
                    gv_setValue(gv, OPEN);
            }
            else if ((!not && val == CLOSING) || (not && val == OPENING)) {
                if (obj_getAnim(pO) != OBJ_ANIM_CLOSE_DOOR) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_CLOSE_DOOR);
                }
                else if (obj_animFinished(pO))
                    gv_setValue(gv, CLOSED);
            }
            else if ((!not && val == OPEN) || (not && val == CLOSED)) {
                obj_rmFlag(pO, ID_STATIC);
                obj_setTile(pO, 196);
            }
        } break;
        case CE_SWITCH_MAP: {
            event *pE;
            GFraMe_object *pO;
            int map, x, y;
            player *pPl;
            
            // Get every parameter
            pE = (event*)_ce_caller;
            pPl = (player*)_ce_target;
            player_getObject(&pO, pPl);
            
            // Make sure the player can't leave ("physically") through the exit
            event_separate(pE, pO);
            
            // Set the player's destination
            event_iGetVar(&map, pE, 0);
            event_iGetVar(&x, pE, 1);
            event_iGetVar(&y, pE, 2);
            player_setDestMap(pPl, map, x, y);
        } break;
        case CE_GET_ITEM: {
            event *pE;
            int item;
            player *pPl;
            
            // Get every parameter
            pE = (event*)_ce_caller;
            pPl = (player*)_ce_target;
            
            // Set the item as gotten
            event_iGetVar(&item, pE, 0);
            gv_setBit(ITEMS, item);
            // And Equip the player
            if (player_getID(pPl) == ID_PL1)
                gv_setValue(PL1_ITEM, item);
            else if (player_getID(pPl) == ID_PL2)
                gv_setValue(PL2_ITEM, item);
        } break;
        // TODO implement every common event
        default: {}
    }
    
__ret:
    return;
}

