/**
 * @file src/types.h
 * 
 * Defines types used thoughout the game
 */
#ifndef __TYPES_H_
#define __TYPES_H_

enum {
    ID_PL  = 0x10000000,
    ID_MOB = 0x20000000,
    ID_OBJ = 0x40000000
};

enum {
    ID_PL1   = ID_PL  | 0x00000001,
    ID_PL2   = ID_PL  | 0x00000002,
    
    ID_DOOR1 = ID_OBJ | 0x00000001,
    ID_DOOR2 = ID_OBJ | 0x00000002,
};

#endif

