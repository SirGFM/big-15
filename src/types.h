/**
 * @file src/types.h
 * 
 * Defines types used thoughout the game
 */
#ifndef __TYPES_H_
#define __TYPES_H_

typedef enum {
    ID_PL      = 0x10000000,
    ID_MOB     = 0x20000000,
    ID_OBJ     = 0x40000000,
    ID_PROJ    = 0x80000000,
    
    ID_STATIC  = 0x01000000,
    ID_MOVABLE = 0x02000000,
    
    ID_PL1     = ID_PL   | 0x00000001,
    ID_PL2     = ID_PL   | 0x00000002,
    
    ID_DOOR    = ID_OBJ  | 0x00000001,
    
    ID_JUMPER  = ID_MOB  | 0x00000001,
    ID_EYE     = ID_MOB  | 0x00000002,
    
    ID_HIGHJUMP = 0x00000001,
    ID_TELEPORT = 0x00000002,
    ID_SIGNALER = 0x00000004,
    ID_LASTITEM = 0x00000008,
    ID_PL1ITEM  = 0x00000100,
    ID_PL2ITEM  = 0x00000200
} flag;

enum {
    CLOSED  = 0,
    CLOSING = 1,
    OPENING = 2,
    OPEN    = 3,
};

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


//#  ifdef __PARSER_H_

#include <stdio.h>

/**
 * Parse a flag from a file pointer.
 * 
 * @param fp The file with the flags
 * @return The parsed flag or 0, on error
 */
flag t_getFlagFromFile(FILE *fp);

/**
 * Parse a trigger from a file pointer.
 * 
 * @param fp The file with the triggers
 * @return The parsed trigger or 0, on error
 */
trigger t_getTriggerFromFile(FILE *fp);

//#  endif

#endif

