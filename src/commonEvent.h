/**
 * @file src/commonEvent.h
 * 
 * Define (and implement) every callback
 */
#ifndef __COMMONEVENT_H_
#define __COMMONEVENT_H_

typedef enum {
    CE_OPEN_DOOR,      /**  ev ce: open a door                                */
    CE_CLOSE_DOOR,     /**  ev ce: closes a door                              */
    CE_SWITCH_DOOR,    /**  ev ce: switch a door's state from open <-> close  */
    CE_HANDLE_DOOR,    /** obj ce: handles displaying and animating a door    */
    CE_HANDLE_NOTDOOR, /** obj ce: handles displaying and animating a ~door   */
    CE_SWITCH_MAP,     /**  ev ce: loads a new map                            */
    CE_GET_ITEM,       /**  ev ce: get an item equip it to the current player */
    CE_HIDDEN_PATH,    /**  ev ce: change a part of the map into a corridor   */
    CE_UNHIDE_HEART,   /** obj ce: removes the 'hidden' flag if the gv is true*/
    CE_INC_MAXHP,      /**  ev ce: increase the max hp                        */
    CE_SET_GV,         /**  ev ce: set a globalVar value                      */
    CE_SET_ANIM_OFF,   /** obj ce: switch to OFF on some variable             */
    CE_SPAWN_BOMB,     /**  ev ce: spawn a bomb after some time               */
    CE_NONE,
    CE_MAX
} commonEvent;

typedef enum {
    CE_CALLER,
    CE_TARGET,
    CE_PARAM_MAX
} ce_params;

/**
 * Call a common event
 * 
 * @param ce Common event to be called
 */
void ce_callEvent(commonEvent ce);

/**
 * Set a parameter
 * 
 * @param p The parameter to be set
 * @param val Value the parameter should assume
 */
void ce_setParam(ce_params p, void *val);

/**
 * Parse a common event from a file pointer.
 * The event name must be between '"'.
 * 
 * @param fp The file with the event
 * @return The parsed common event or CE_MAX, on error
 */
commonEvent ce_getEventFromFile(FILE *fp);

/**
 * Get a event's name
 * 
 * @param ce The common event
 * @return The common event's name or NULL
 */
char* ce_getName(commonEvent ce);

#endif

