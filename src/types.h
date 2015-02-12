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
    
    ID_STATIC  = 0x01000000,
    ID_MOVABLE = 0x02000000,
    
    ID_PL1   = ID_PL  | 0x00000001,
    ID_PL2   = ID_PL  | 0x00000002,
    ID_DOOR  = ID_OBJ | 0x00000001,
} flag;

typedef enum {
    CTR_KEYS,
    CTR_BOTH,
    CTR_1CTR,
    CTR_2CTR,
    CTR_MAX
} ctr_mode;

enum {
    CLOSED  = 0,
    CLOSING = 1,
    OPENING = 2,
    OPEN    = 3,
};

//#  ifdef __PARSER_H_

#include <stdio.h>

/**
 * Parse a flag from a file pointer.
 * 
 * @param fp The file with the flags
 * @return The parsed flag or 0, on error
 */
flag t_getFlagFromFile(FILE *fp);

//#  endif

#endif

