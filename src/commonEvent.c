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
#include "registry.h"
#include "types.h"

static char *_ce_names[CE_MAX+1] = {
    "ce_open_door",
    "ce_close_door",
    "ce_switch_door",
    "ce_handle_door",
    "ce_handle_notdoor",
    "ce_switch_map",
    "ce_get_item",
    "ce_hidden_path",
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
            ID &= ID_DOOR | ID_DOOR_HOR;
            ASSERT_NR(ID == ID_DOOR || ID == ID_DOOR_HOR);
            //ASSERT_NR((ID & ID_DOOR) == ID_DOOR);
            
            // Whether everything should be negated or not
            not = (ce == CE_HANDLE_NOTDOOR);
            
            // Get the door's state
            obj_getVar(&gv, pO, 0);
            val = gv_getValue(gv);
            
            // OPEN
            if ((not && val == CLOSED) || (!not && val == OPEN)) {
                if (ID == ID_DOOR) {
                    obj_rmFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_OPEN);
                }
                else if (ID == ID_DOOR_HOR) {
                    obj_rmFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_HOR_OPEN);
                }
            }
            // OPENING
            else if ((!not && val == OPENING) || (not && val == CLOSING)) {
                // Vertical door
                if (ID == ID_DOOR && obj_getAnim(pO) != OBJ_ANIM_DOOR_OPENING) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_OPENING);
                }
                // Horizontal door
                else if (ID == ID_DOOR_HOR && obj_getAnim(pO) != OBJ_ANIM_DOOR_HOR_OPENING) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_HOR_OPENING);
                }
                // Both
                else if (obj_animFinished(pO) && !not)
                    gv_setValue(gv, OPEN);
                else if (obj_animFinished(pO) && not)
                    gv_setValue(gv, CLOSED);
            }
            // CLOSING
            else if ((!not && val == CLOSING) || (not && val == OPENING)) {
                // Vertical door
                if (ID == ID_DOOR && obj_getAnim(pO) != OBJ_ANIM_DOOR_CLOSING) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_CLOSING);
                }
                // Horizontal door
                else if (ID == ID_DOOR_HOR && obj_getAnim(pO) != OBJ_ANIM_DOOR_HOR_CLOSING) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_HOR_CLOSING);
                }
                // Both
                else if (obj_animFinished(pO) && !not)
                    gv_setValue(gv, CLOSED);
                else if (obj_animFinished(pO) && not)
                    gv_setValue(gv, OPEN);
            }
            // CLOSED
            else if ((not && val == OPEN) || (!not && val == CLOSED)) {
                if (ID == ID_DOOR) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_CLOSED);
                }
                else if (ID == ID_DOOR_HOR) {
                    obj_addFlag(pO, ID_STATIC);
                    obj_setAnim(pO, OBJ_ANIM_DOOR_HOR_CLOSED);
                }
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
            int item, prevItems;
            player *pPl;
            
            // Get every parameter
            pE = (event*)_ce_caller;
            pPl = (player*)_ce_target;
            
            prevItems = gv_getValue(ITEMS);
            // Set the item as gotten
            event_iGetVar(&item, pE, 0);
            gv_setBit(ITEMS, item);
            // And Equip the player
            if ((prevItems & item) == 0) {
                if (player_getID(pPl) == ID_PL1)
                    gv_setValue(PL1_ITEM, item);
                else if (player_getID(pPl) == ID_PL2)
                    gv_setValue(PL2_ITEM, item);
            }
        } break;
        case CE_HIDDEN_PATH: {
            event *pE;
            GFraMe_object *pObj;
            GFraMe_ret rv;
            int i, ini, j, len, lx, ly, x, y, w, h;
            unsigned char *pData;
            
            map_getDimensions(m, &w, &h);
            w /= 8;
            h /= 8;
            // Get every parameter
            pE = (event*)_ce_caller;
            event_getObject(&pObj, pE);
            // Get the initial tile and dimensions
            x = pObj->x / 8;
            y = pObj->y / 8;
            ly = pObj->hitbox.hh * 2 / 8;
            lx = pObj->hitbox.hw * 2 / 8;
            ini = x + y * w;
            // Get the map's data
            rv = map_getTilemapData(&pData, &len, m);
            ASSERT_NR(rv == GFraMe_ret_ok);
            // Check that the event is inbounds
            ASSERT_NR(x + y*w < len);
            ASSERT_NR(x + lx + y*w < len);
            ASSERT_NR(x + (y + ly)*w < len);
            // Actually set the data
            j = 0;
            // Set both walls and the empty space
            while (++j < ly) {
                i = ini;
                // Set the left wall
                if (map_isTileSolid(m, x - 1, y + j) == GFraMe_ret_ok)
                    pData[i + j*w] = 106;
                else
                    pData[i + j*w] = 64;
                // Set the walkable space
                while (++i < ini + lx - 1)
                    pData[i + j*w] = 64;
                // Set the right wall
                if (map_isTileSolid(m, x + lx, y + j) == GFraMe_ret_ok)
                    pData[i + j*w] = 104;
                else
                    pData[i + j*w] = 64;
            }
            // Set both ceiling and floor
            i = ini;
            while (++i < ini + lx) {
                if (map_isTileSolid(m, x, y-1) == GFraMe_ret_ok)
                    pData[i] = 137;
                if (map_isTileSolid(m, x, y+ly) == GFraMe_ret_ok)
                    pData[i + (ly-1)*w] = 73;
            }
            // Set all corners
            //if (map_isTileSolid(m, x, y) == GFraMe_ret_ok
            //    && map_isTileSolid(m, x-1, y) == GFraMe_ret_ok
              if (map_isTileSolid(m, x-1, y) == GFraMe_ret_ok
                && map_isTileSolid(m, x, y-1) == GFraMe_ret_ok)
                pData[x + y * w] = 140;
            else
                pData[x + y * w] = 136;
            
            if (map_isTileSolid(m, x+lx-1, y) == GFraMe_ret_ok
                && map_isTileSolid(m, x+lx, y) == GFraMe_ret_ok
                && map_isTileSolid(m, x+lx-1, y-1) == GFraMe_ret_ok)
                pData[x+lx-1 + y * w] = 139;
            else
                pData[x+lx-1 + y * w] = 138;
            
            if (map_isTileSolid(m, x, y+ly-1) == GFraMe_ret_ok
                && map_isTileSolid(m, x-1, y+ly-1) == GFraMe_ret_ok
                && map_isTileSolid(m, x, y+ly) == GFraMe_ret_ok)
                pData[x + (y+ly-1) * w] = 108;
            else
                pData[x + (y+ly-1) * w] = 72;
            
            if (map_isTileSolid(m, x+lx-1, y+ly-1) == GFraMe_ret_ok
                && map_isTileSolid(m, x+lx, y+ly-1) == GFraMe_ret_ok
                && map_isTileSolid(m, x+lx-1, y+ly) == GFraMe_ret_ok)
                pData[x+lx-1 + (y+ly-1) * w] = 107;
            else
                pData[x+lx-1 + (y+ly-1) * w] = 74;
            
            map_setTilemap(m, pData, len, w, h);
        } break;
        // TODO implement every common event
        default: {}
    }
    
__ret:
    return;
}

