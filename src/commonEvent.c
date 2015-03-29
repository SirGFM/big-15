    /**
 * @file src/commonEvent.c
 */
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include <stdio.h>

#include "audio.h"
#include "commonEvent.h"
#include "event.h"
#include "global.h"
#include "globalVar.h"
#include "object.h"
#include "player.h"
#include "playstate.h"
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
    "ce_unhide_on_gv", // actualy CE_UNHIDE_HEART
    "ce_inc_maxhp",
    "ce_set_gv",
    "ce_set_anim_off",
    "ce_spawn_bomb",
    "ce_none",
    "ce_max"
};

static char _ce_hjboots_text[] = 
"YOU GOT THE HIGH JUMP BOOTS!\n"
"     \n"
"JUST EQUIP IT TO JUMP HIGHER.\n"
"\n"
"PRESS ANY KEY/BUTTON TO CONTINUE...";

static char _ce_telp_text[] = 
"YOU GOT THE TELEPORTER!\n"
"     \n"
"USE IT TO TELEPORT TO A PLACED SIGNAL\n"
"OR THE SIGNALER HOLDER.\n"
"\n"
"PRESS ANY KEY/BUTTON TO CONTINUE...";

static char _ce_sign_text[] = 
"YOU GOT THE SIGNALER!\n"
"     \n"
"USE IT TO PLACE A SIGNAL FOR THE\n"
"TELEPORTER OR SIMPLY HOLD IT, SO\n"
"YOU'LL ACT AS THE TELEPORTER TARGET.\n"
"\n"
"PRESS ANY KEY/BUTTON TO CONTINUE...";

/** Object that caused the event to be called */
static void *_ce_caller = NULL;
/** Object associated with the caller */
static void *_ce_target = NULL;

/**
 * Select which tile to put, according to all its neighbours
 * 
 * @param i The tiles x position
 * @param j The tiles y position
 * @return The tile
 */
static unsigned char _ce_setTile(int i, int j);

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
            if (val == CLOSED) {
                gv_setValue(gv, OPENING);
                sfx_door();
            }
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
            if (val == OPEN) {
                gv_setValue(gv, CLOSING);
                sfx_door();
            }
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
            if (val == OPEN) {
                gv_setValue(gv, CLOSING);
                sfx_door();
            }
            else if (val == CLOSED) {
                gv_setValue(gv, OPENING);
                sfx_door();
            }
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
                
                if (item == ID_HIGHJUMP) {
                    ps_showText(_ce_hjboots_text, sizeof(_ce_hjboots_text), 0, 0, 40, 8);
                }
                else if (item == ID_TELEPORT) {
                    ps_showText(_ce_telp_text, sizeof(_ce_telp_text), 0, 0, 40, 9);
                }
                else if (item == ID_SIGNALER) {
                    ps_showText(_ce_sign_text, sizeof(_ce_sign_text), 0, 0, 40, 9);
                }
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
            // Make the hidden path empty
            j = 0;
            while (j < ly) {
                i = 0;
                while (i < lx) {
                    pData[ini + i + j*w] = 64;
                    i++;
                }
                j++;
            }
            // Set its left wall
            j = 0;
            while (j < ly) {
                if (map_isTileSolid(m, x-1, y + j) == GFraMe_ret_ok) {
                    pData[ini + j*w] = 106;
                }
                j++;
            }
            // Set its right wall
            j = 0;
            while (j < ly) {
                if (map_isTileSolid(m, x+lx, y + j) == GFraMe_ret_ok) {
                    pData[ini + lx - 1 + j*w] = 104;
                }
                j++;
            }
            // Set its upper wall
            i = 0;
            while (i < lx) {
                if (map_isTileSolid(m, x + i, y - 1) == GFraMe_ret_ok) {
                    pData[ini + i] = 137;
                }
                i++;
            }
            // Set its lower wall
            i = 0;
            while (i < lx) {
                if (map_isTileSolid(m, x + i, y + ly) == GFraMe_ret_ok) {
                    pData[ini + i + (ly - 1)*w] = 73;
                }
                i++;
            }
            // Set the upper-left corner
            pData[     x + y*w       ] = _ce_setTile(     x, y     );
            // Set the upper-right corner
            pData[x+lx-1 + y*w       ] = _ce_setTile(x+lx-1, y     );
            // Set the lower-left corner
            pData[     x + (y+ly-1)*w] = _ce_setTile(     x, y+ly-1);
            // Set the lower-right corner
            pData[x+lx-1 + (y+ly-1)*w] = _ce_setTile(x+lx-1, y+ly-1);
            // Reset the tilemap bounds and animations
            map_setTilemap(m, pData, len, w, h);
        } break;
        case CE_UNHIDE_HEART: {
            globalVar gv;
            int ID;
            object *pO;
            
            // Retrieve the reference
            ASSERT_NR(_ce_caller);
            pO = (object*)_ce_caller;
            
            obj_getID(&ID, pO);
            
            // Check if the object is still hidden
            if (ID & ID_HIDDEN) {
                // Unhide the event, on true
                obj_getVar(&gv, pO, 0);
                if (gv_nIsZero(gv))
                    obj_rmFlag(pO, ID_HIDDEN);
            }
            // Check if the graphics should be updated
            obj_getVar(&gv, pO, 1);
            if (gv_nIsZero(gv))
                obj_setAnim(pO, OBJ_ANIM_MAXHP_UP_OFF);
        } break;
        case CE_INC_MAXHP: {
            event *pE;
            globalVar gv;
            int val;
            
            // Check whether this power up was previously gotten
            pE = (event*)_ce_caller;
            event_getVar(&gv, pE, 0);
            ASSERT_NR(gv_isZero(gv));
            // Increase the max hp and recover health
            gv_inc(PL1_MAXHP);
            gv_inc(PL2_MAXHP);
            val = gv_getValue(PL1_MAXHP);
            gv_setValue(PL1_HP, val);
            gv_setValue(PL2_HP, val);
            // Mark this as gotten
            gv_inc(gv);
            
            sfx_heartUp();
        } break;
        case CE_SET_GV: {
            event *pE;
            globalVar gv;
            int val;
            
            // Get the globalVar value
            pE = (event*)_ce_caller;
            event_getVar(&gv, pE, 0);
            event_iGetVar(&val, pE, 0);
            // Set it!
            gv_setValue(gv, val);
            if (gv >= TERMINAL_001 && gv <= TERMINAL_027) {
                sfx_terminal();
            }
            else if (gv >= HJUMP_TERM && gv <= SIGNL_TERM) {
                sfx_getItem();
            }
        } break;
        case CE_SET_ANIM_OFF: {
            globalVar gv;
            int ID;
            object *pO;
            
            // Retrieve the reference
            ASSERT_NR(_ce_caller);
            pO = (object*)_ce_caller;
            // Change the animation, on true
            obj_getVar(&gv, pO, 0);
            if (gv_nIsZero(gv)) {
                // Get the object's ID
                obj_getID(&ID, pO);
                // Set the animation accordingly
                if ((ID & ID_HEARTUP) == ID_HEARTUP)
                    obj_setAnim(pO, OBJ_ANIM_MAXHP_UP_OFF);
                else if ((ID & ID_HJUMP_TERM) == ID_HJUMP_TERM)
                    obj_setAnim(pO, OBJ_ANIM_HJUMP_OFF);
                else if ((ID & ID_TELEP_TERM) == ID_TELEP_TERM)
                    obj_setAnim(pO, OBJ_ANIM_TELEP_OFF);
                else if ((ID & ID_SIGNL_TERM) == ID_SIGNL_TERM)
                    obj_setAnim(pO, OBJ_ANIM_SIGNL_OFF);
                else if ((ID & ID_TERM) == ID_TERM)
                    obj_setAnim(pO, OBJ_ANIM_TERM_OFF);
            }
        } break;
        case CE_SPAWN_BOMB: {
            if (gv_getValue(TIMER_BOMB) >= 8500) {
                GFraMe_ret rv;
                mob *pM;
                
                // Recycle a mob
                rv = rg_recycleMob(&pM);
                ASSERT_NR(rv == GFraMe_ret_ok);
                rv = mob_init(pM, 143, 32, ID_BOMB);
                ASSERT_NR(rv == GFraMe_ret_ok);
                
                // Reset the timer
                gv_sub(TIMER_BOMB, 8500);
            }
            else
                gv_add(TIMER_BOMB, gv_getValue(GAME_UPS));
        } break;
        // TODO implement every common event
        case CE_NONE: {}
        default: {}
    }
    
__ret:
    return;
}

/**
 * Select which tile to put, according to all its neighbours
 * 
 * @param i The tiles x position
 * @param j The tiles y position
 * @return The tile
 */
static unsigned char _ce_setTile(int i, int j) {
    int a, b, c, d, e, f, g, h;
    // a | b | c
    // d |   | e
    // f | g | h
    a = map_isTileSolid(m, i-1, j-1) == GFraMe_ret_ok;
    b = map_isTileSolid(m, i  , j-1) == GFraMe_ret_ok;
    c = map_isTileSolid(m, i+1, j-1) == GFraMe_ret_ok;
    d = map_isTileSolid(m, i-1, j  ) == GFraMe_ret_ok;
    e = map_isTileSolid(m, i+1, j  ) == GFraMe_ret_ok;
    f = map_isTileSolid(m, i-1, j+1) == GFraMe_ret_ok;
    g = map_isTileSolid(m, i  , j+1) == GFraMe_ret_ok;
    h = map_isTileSolid(m, i+1, j+1) == GFraMe_ret_ok;
    
    // xxx
    // xxx
    // xx#
    if ( a &&  b &&  c &&
         d &&        e &&
         f &&  g && !h   )
        return 140;
    else
    // xx#
    // xxx
    // xxx
    if ( a &&  b && !c &&
         d &&        e &&
         f &&  g &&  h   )
        return 108;
    else
    // #xx
    // xxx
    // xxx
    if (!a &&  b &&  c &&
         d &&        e &&
         f &&  g &&  h   )
        return 107;
    else
    // xxx
    // xxx
    // #xx
    if ( a &&  b &&  c &&
         d &&        e &&
        !f &&  g &&  h   )
        return 139;
    else
    // xx#
    // xxx
    // xx#
    if ( a &&  b && !c &&
         d &&        e &&
         f &&  g && !h   )
        return 141;
    else
    // #x#
    // xxx
    // xxx
    if (!a &&  b && !c &&
         d &&        e &&
         f &&  g &&  h   )
        return 142;
    else
    // #xx
    // xxx
    // #xx
    if (!a &&  b &&  c &&
         d &&        e &&
        !f &&  g &&  h   )
        return 110;
    else
    // xxx
    // xxx
    // #x#
    if ( a &&  b &&  c &&
         d &&        e &&
        !f &&  g && !h   )
        return 109;
    else
    // ###
    // xxx
    // xx#
    if (      !b &&      
         d &&        e &&
         f &&  g && !h   )
        return 174;
    else
    // xx#
    // xxx
    // ###
    if ( a &&  b && !c &&
         d &&        e &&
              !g         )
        return 206;
    else
    // ###
    // xxx
    // xxx
    if (      !b &&      
         d &&        e &&
         f &&  g &&  h   )
        return 73;
    else
    // #xx
    // #xx
    // #xx
    if (       b &&  c &&
        !d &&        e &&
               g &&  h   )
        return 104;
    else
    // xxx
    // xxx
    // ###
    if ( a &&  b &&  c &&
         d &&        e &&
              !g         )
        return 137;
    else
    // xx#
    // xx#
    // xx#
    if ( a &&  b &&      
         d &&       !e &&
         f &&  g         )
        return 106;

    else
    // #xx
    // #xx
    // ###
    if (       b &&  c &&
        !d &&        e &&
        !f && !g         )
        return 136;
    else
    // xx#
    // xx#
    // ###
    if ( a &&  b &&      
         d &&       !e &&
              !g && !h   )
        return 138;
    else
    // ###
    // xx#
    // xx#
    if (      !b && !c &&
         d &&       !e &&
         f &&  g         )
        return 74;
    else
    // ###
    // #xx
    // #xx
    if (!a && !b &&      
        !d &&        e &&
               g &&  h   )
        return 72;
    else
    // xxx
    // xxx
    // xxx
        return 64;
}

