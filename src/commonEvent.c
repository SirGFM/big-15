/**
 * @file src/commonEvent.c
 */
#include <GFraMe/GFraMe_error.h>

#include "commonEvent.h"

static char *_ce_names[CE_MAX+1] = {
    "ce_test_door1",    /** CE_TEST_DOOR1 */
    "ce_max"            /** CE_MAX */
};

/** Object that caused the event to be called */
static void *_ce_caller = NULL;
/** Object associated with the caller */
static void *_ce_target = NULL;

/**
 * Call a common event
 * 
 * @param ce Common event to be called
 */
void ce_callEvent(commonEvent ce) {
    GFraMe_assertRet(ce < CE_MAX, "Invalid common event!", __ret);
    
    switch (ce) {
        // TODO implement every common event
        default: {}
    }
    
__ret:
    return;
}

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

