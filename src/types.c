/**
 * @file src/types.c
 * 
 * Defines types used thoughout the game
 */
#include <GFraMe/GFraMe_error.h>

#include <stdio.h>
#include <string.h>

#include "types.h"

#define FLAGS_MAX 11

static char *_t_flagNames[FLAGS_MAX] = {
    "static",
    "movable",
    
    "pl1",
    "pl2",
    
    "door",
    "door-hor",
    
    "jumper",
    "eye",
    "eyel",
    "charger",
    
    "eneproj"
};

static flag _t_flags[FLAGS_MAX] = {
    ID_STATIC,
    ID_MOVABLE,
    
    ID_PL1,
    ID_PL2,
    
    ID_DOOR,
    ID_DOOR_HOR,
    
    ID_JUMPER,
    ID_EYE,
    ID_EYE_LEFT,
    ID_CHARGER,
    
    ID_ENEPROJ
};

#define TRIGGERS_MAX 11

static char *_t_triggerNames[TRIGGERS_MAX] = {
    "on_enter_left",
    "on_enter_right",
    "on_enter_down",
    "on_enter_up",
    "on_enter",
    "on_pressed",
    "is_player",
    "is_mob",
    "is_obj",
    "keep_active",
    "trigger_max"
};

static trigger _t_triggers[TRIGGERS_MAX] = {
    ON_ENTER_LEFT,
    ON_ENTER_RIGHT,
    ON_ENTER_DOWN,
    ON_ENTER_UP,
    ON_ENTER,
    ON_PRESSED,
    IS_PLAYER,
    IS_MOB,
    IS_OBJ,
    KEEP_ACTIVE,
    TRIGGER_MAX
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
    int c, i, irv;
    
    // Sanitize parameter
    GFraMe_assertRV(fp, "Invalid file!", f = 0, __ret);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    GFraMe_assertRV(irv == 0, "Error getting file position!", f = 0, __ret);
    
    // Check every possible flag
    i = 0;
    while (i < FLAGS_MAX) {
        int len, j;
        
        c = fgetc(fp);
        GFraMe_assertRV(c != EOF, "Reached end-of-file!", f = 0, __ret);
        GFraMe_assertRV(c == '"', "Not a string!", f = 0, __ret);
    
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
        if (j == len) {
            int c;
            // Check that the string is finished by a '"'
            c = fgetc(fp);
            GFraMe_assertRV(c != EOF, "Reached end-of-file!", f = 0, __ret);
            if (c == '"')
                break;
        }

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

/**
 * Parse a trigger from a file pointer.
 * 
 * @param fp The file with the triggers
 * @return The parsed trigger or 0, on error
 */
trigger t_getTriggerFromFile(FILE *fp) {
    fpos_t pos;
    int c, i, irv;
    trigger t;
    
    // Sanitize parameter
    GFraMe_assertRV(fp, "Invalid file!", t = 0, __ret);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    GFraMe_assertRV(irv == 0, "Error getting file position!", t = 0, __ret);
    
    // Check every possible flag
    i = 0;
    while (i < TRIGGERS_MAX) {
        int len, j;
        
        c = fgetc(fp);
        GFraMe_assertRV(c != EOF, "Reached end-of-file!", t = 0, __ret);
        GFraMe_assertRV(c == '"', "Not a string!", t = 0, __ret);
    
        // Try to read as many character as there are in the name
        len = strlen(_t_triggerNames[i]);
        j = 0;
        while (j < len) {
            int c;
            
            // Check that the characters match
            c = fgetc(fp);
            GFraMe_assertRV(c != EOF, "Reached end-of-file!", t = 0, __ret);
            if (c != _t_triggerNames[i][j])
                break;
            j++;
        }
        // Stop, if a match was found
        if (j == len) {
            int c;
            // Check that the string is finished by a '"'
            c = fgetc(fp);
            GFraMe_assertRV(c != EOF, "Reached end-of-file!", t = 0, __ret);
            if (c == '"')
                break;
        }

        // Return to the string's begin
        irv = fsetpos(fp, &pos);
        GFraMe_assertRV(irv == 0, "Error setting file position!", t = 0, __ret);
        
        // Go to the next flag
        i++;
    }
    
    // Check if a trigger was actually found and return
    GFraMe_assertRV(i < TRIGGERS_MAX, "Failed to find type", t = 0, __ret);
    t = _t_triggers[i];
__ret:
    // Backtrack on error
    if (t == 0)
        fsetpos(fp, &pos);
    
    return t;
}

