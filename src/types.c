/**
 * @file src/types.c
 * 
 * Defines types used thoughout the game
 */
#include <GFraMe/GFraMe_error.h>

#include <stdio.h>
#include <string.h>

#include "types.h"

#define FLAGS_MAX 5

static char *_t_flagNames[FLAGS_MAX] = {
    "static",
    "movable",
    
    "pl1",
    "pl2",
    "door"
};

static flag _t_flags[FLAGS_MAX] = {
    ID_STATIC,
    ID_MOVABLE,
    ID_PL1,
    ID_PL2,
    ID_DOOR
};

/**
 * Parse a flag from a file pointer.
 * 
 * @param fp The file with the flags
 * @return The parsed flag or 0, on error
 */
flag t_getFlagFromFile(FILE *fp) {
    flag f;
    fpos_t pos;
    int i, irv;
    
    // Sanitize parameter
    GFraMe_assertRV(fp, "Invalid file!", f = 0, __ret);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    GFraMe_assertRV(irv == 0, "Error getting file position!", f = 0, __ret);
    
    // Check every possible flag
    i = 0;
    while (i < FLAGS_MAX) {
        int len, j;
        
        // Try to read as many character as there are in the name
        len = strlen(_t_flagNames[i]);
        j = 0;
        while (j < len) {
            int c;
            
            // Check that the characters match
            c = fgetc(fp);
            GFraMe_assertRV(c != EOF, "Reached end-of-file!", f = 0, __ret);
            if (c != _t_flagNames[i][j])
                break;
            j++;
        }
        // Stop, if a match was found
        if (j == len)
            break;

        // Return to the string's begin
        irv = fsetpos(fp, &pos);
        GFraMe_assertRV(irv == 0, "Error setting file position!", f = 0, __ret);
        
        // Go to the next flag
        i++;
    }
    
    // Check if a flag was actually found and return
    GFraMe_assertRV(i < FLAGS_MAX, "Failed to find type", f = 0, __ret);
    f = _t_flags[i];
__ret:
    return f;
}

