/**
 * @file src/parser.c
 * 
 * Module for parsing every 'object', both from string and file
 */
#include <GFraMe/GFraMe_error.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "commonEvent.h"
#include "event.h"
#include "global.h"
#include "map.h"
#include "mob.h"
#include "object.h"
#include "parser.h"
#include "registry.h"
#include "types.h"

/**
 * Simply ignore every whitespace. Since this is only called internally, no
 * verification is needed
 * 
 * @param fp File pointer
 * @param ignoreNewline Whether newline ('\n') can be ignored or not
 */
static void parsef_ignoreWhitespace(FILE *fp, int ignoreNewline) {
    int c;
    
    while (1) {
        c = fgetc(fp);
        
        if (c == EOF || (c != ' ' && c != '\t' && c != '\r' &&
            (!ignoreNewline || c != '\n')))
            break;
    }
    
    ungetc(c, fp);
}

/**
 * Parse a integer from a file
 * 
 * @param pI Returns the parsed integer
 * @param fp File pointer
 * @return GFraMe error code
 */
static GFraMe_ret parsef_int(int *pI, FILE *fp) {
    fpos_t pos;
    GFraMe_ret rv;
    int c, i, irv, signal;
    
    // Sanitize parameters
    ASSERT(pI, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the first character is a digit
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT((c >= '0' && c <= '9') || c == '-', GFraMe_ret_failed);
    
    // Set the varibles signal
    if (c == '-') {
        signal = -1;
        c = fgetc(fp);
        ASSERT(c != EOF, GFraMe_ret_failed);
        ASSERT(c >= '0' && c <= '9', GFraMe_ret_failed);
    }
    else
        signal = 1;
    
    // Parse the integer
    i = 0;
    while (1) {
        i = i * 10 + (c - '0');
        
        // Read the next character and stop if it isn't a digit
        c = fgetc(fp);
        if (c == EOF || c < '0' || c > '9')
            break;
    }
    ungetc(c, fp);
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    *pI = i*signal;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Check that a given (expected) string is read from the stream.
 * 
 * @param fp File pointer
 * @param str String to be matched
 * @param len String's length
 * @return GFraMe error code
 */
static GFraMe_ret parsef_string(FILE *fp, char *str, int len) {
    fpos_t pos;
    GFraMe_ret rv;
    int i, irv;
    
    // Sanitize parameters
    ASSERT(fp, GFraMe_ret_bad_param);
    ASSERT(str, GFraMe_ret_bad_param);
    ASSERT(len > 0, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Match each character on the string
    i = 0;
    while (i < len) {
        int c;
        
        c = fgetc(fp);
        if (c != str[i])
            break;
        i++;
    }
    
    // Check that the string was completely read (and matched)
    ASSERT(i == len, GFraMe_ret_failed);
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Parse flags from a file
 * flags - flagName ('|' flagName)*
 * 
 * @param pF Returns the parsed flags
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_flags(flag *pF, FILE *fp) {
    flag f;
    fpos_t pos;
    GFraMe_ret rv;
    int irv;
    
    // Sanitize parameters
    ASSERT(pF, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    f = 0;
    while (1) {
        flag tmp;
        
        // Get the current flag
        tmp = t_getFlagFromFile(fp);
        ASSERT(tmp != 0, GFraMe_ret_failed);
        parsef_ignoreWhitespace(fp, 1);
        
        // Add it to the current found ones
        f |= tmp;
        
        // Check if another flag is expected
        rv = parsef_string(fp, "|", 1);
        if (rv != GFraMe_ret_ok)
            break;
    }
    
    // Set return variable
    *pF = f;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Parse triggers from a file
 * triggerss - triggerName ('|' triggerName)*
 * 
 * @param pT Returns the parsed triggers
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_triggers(trigger *pT, FILE *fp) {
    fpos_t pos;
    GFraMe_ret rv;
    int irv;
    trigger t;
    
    // Sanitize parameters
    ASSERT(pT, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    t = 0;
    while (1) {
        trigger tmp;
        
        // Get the current flag
        tmp = t_getTriggerFromFile(fp);
        ASSERT(tmp != 0, GFraMe_ret_failed);
        parsef_ignoreWhitespace(fp, 1);
        
        // Add it to the current found ones
        t |= tmp;
        
        // Check if another flag is expected
        rv = parsef_string(fp, "|", 1);
        if (rv != GFraMe_ret_ok)
            break;
    }
    
    // Set return variable
    *pT = t;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Parse a global variable from a file
 * 
 * @param pGv Returns the parsed common event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_globalVar(globalVar *pGv, FILE *fp) {
    fpos_t pos;
    GFraMe_ret rv;
    globalVar gv;
    int irv;
    
    // Sanitize parameters
    ASSERT(pGv, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check every globalVar (yay, dumb strats!)
    gv = 0;
    while (gv < GV_MAX) {
        char *gvName;
        
        // Get the current globalVar's name
        gvName = gv_getName(gv);
        
        rv = parsef_string(fp, "\"", 1);
        ASSERT(rv == GFraMe_ret_ok, GFraMe_ret_failed);
        
        // Try to match every other character to the current globalVar
        rv = parsef_string(fp, gvName, strlen(gvName));
        if (rv == GFraMe_ret_ok) {
            rv = parsef_string(fp, "\"", 1);
            if (rv == GFraMe_ret_ok)
                break;
        }
        
        // Return to the string's begin
        irv = fsetpos(fp, &pos);
        ASSERT(irv == 0, GFraMe_ret_failed);
        
        gv++;
    }
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    *pGv = gv;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Parse a common event from a file.
 * 
 * @param pCe Returns the parsed common event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_commonEvent(commonEvent *pCe, FILE *fp) {
    commonEvent ce;
    fpos_t pos;
    GFraMe_ret rv;
    int irv;
    
    // Sanitize parameters
    ASSERT(pCe, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check every event (yay, dumb strats!)
    ce = 0;
    while (ce < CE_MAX) {
        char *ceName;
        
        rv = parsef_string(fp, "\"", 1);
        ASSERT(rv == GFraMe_ret_ok, GFraMe_ret_failed);
        
        // Get the current event's name
        ceName = ce_getName(ce);
        
        // Try to match every other character to the current event
        rv = parsef_string(fp, ceName, strlen(ceName));
        if (rv == GFraMe_ret_ok) {
            rv = parsef_string(fp, "\"", 1);
            if (rv == GFraMe_ret_ok)
                break;
        }
        
        // Return to the string's begin
        irv = fsetpos(fp, &pos);
        ASSERT(irv == 0, GFraMe_ret_failed);
        
        ce++;
    }
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    // Set the function's return
    *pCe = ce;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param)
        fsetpos(fp, &pos);
    
    return rv;
}

/**
 * Parse a event from a file
 * A event is described by following rule:
 * "ev:" '{' "x:"int "y:"int "w:"int "h:"int "ce:"commonEventName "t:"int 
 *          "var:"globalVarName "int:":int '}'
 * All the numbers are read as tiles (i.e., multiplied by 8)
 * 
 * @param pE Returns the parsed event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_event(event *pE, FILE *fp) {
    commonEvent ce;
    fpos_t pos;
    GFraMe_ret rv;
    globalVar gvs[EV_VAR_MAX];
    int c, irv, ivs[EV_VAR_MAX], ivsUsed, gvsUsed, h, w, x, y;
    trigger t;
    
    // Sanitize parameters
    ASSERT(pE, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the next "token" must be an event
    rv = parsef_string(fp, "ev:", 3);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Open a bracket =D
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT(c == '{', GFraMe_ret_failed);
    parsef_ignoreWhitespace(fp, 1);
    
    // Get every parameter needed for the event, one at a time
    x = -1;
    y = -1;
    w = -1;
    h = -1;
    t = 0;
    gvsUsed = 0;
    ivsUsed = 0;
    ce = CE_MAX;
    while (1) {
        if (parsef_string(fp, "x:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&x, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "y:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&y, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "w:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&w, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "h:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&h, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "t:", 2) == GFraMe_ret_ok) {
            rv = parsef_triggers(&t, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "ce:", 3) == GFraMe_ret_ok) {
            rv = parsef_commonEvent(&ce, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ASSERT(ce < CE_MAX, GFraMe_ret_failed);
        }
        else if (parsef_string(fp, "var:", 4) == GFraMe_ret_ok) {
            globalVar gv;
            
            ASSERT(gvsUsed < EV_VAR_MAX, GFraMe_ret_failed);
            rv = parsef_globalVar(&gv, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ASSERT(gv < GV_MAX, GFraMe_ret_failed);
            gvs[gvsUsed] = gv;
            gvsUsed++;
        }
        else if (parsef_string(fp, "int:", 4) == GFraMe_ret_ok) {
            int i;
            
            ASSERT(ivsUsed < EV_VAR_MAX, GFraMe_ret_failed);
            rv = parsef_int(&i, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ivs[ivsUsed] = i;
            ivsUsed++;
        }
        else {
            // If nothing was found, expect a closing bracket and stop
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            ASSERT(c == '}', GFraMe_ret_failed);
            break;
        }
    }
    ASSERT(w > 0, GFraMe_ret_failed);
    ASSERT(h > 0, GFraMe_ret_failed);
    ASSERT(t > 0, GFraMe_ret_failed);
    ASSERT(ce != CE_MAX, GFraMe_ret_failed);
    
    // Create the event
    rv = event_setAll(pE, x*8, y*8, w*8, h*8, t, ce);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Add all local variables
    while (gvsUsed > 0) {
        gvsUsed--;
        rv = event_setVar(pE, gvsUsed, gvs[gvsUsed]);
    }
    while (ivsUsed > 0) {
        ivsUsed--;
        rv = event_iSetVar(pE, ivsUsed, ivs[ivsUsed]);
    }
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}


/**
 * Parse an object from a file
 * A object is described by following rule:
 * "obj:" '{' "x:"int "y:"int "w:"int "h:"int "ce:"commonEventName "var":globalVarName '}'
 * All the numbers are read as tiles (i.e., multiplied by 8)
 * 
 * @param pO Returns the parsed object
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_object(object *pO, FILE *fp) {
    commonEvent ce;
    flag f;
    fpos_t pos;
    GFraMe_ret rv;
    int c, irv, h, w, x, y, gvsUsed;
    globalVar gvs[OBJ_VAR_MAX];
    
    // Sanitize parameters
    ASSERT(pO, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the next "token" must be an event
    rv = parsef_string(fp, "obj:", 4);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Open a bracket =D
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT(c == '{', GFraMe_ret_failed);
    parsef_ignoreWhitespace(fp, 1);
    
    // Get every parameter needed for the object, one at a time
    x = -1;
    y = -1;
    w = -1;
    h = -1;
    f = 0;
    gvsUsed = 0;
    ce = CE_MAX;
    while (1) {
        if (parsef_string(fp, "x:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&x, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "y:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&y, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "w:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&w, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "h:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&h, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "ce:", 3) == GFraMe_ret_ok) {
            rv = parsef_commonEvent(&ce, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ASSERT(ce < CE_MAX, GFraMe_ret_failed);
        }
        else if (parsef_string(fp, "var:", 4) == GFraMe_ret_ok) {
            ASSERT(gvsUsed < OBJ_VAR_MAX, GFraMe_ret_failed);
            
            rv = parsef_globalVar(&gvs[gvsUsed], fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            
            gvsUsed++;
        }
        else if (parsef_string(fp, "f:", 2) == GFraMe_ret_ok) {
            rv = parsef_flags(&f, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ASSERT(f != 0, GFraMe_ret_failed);
        }
        else {
            // If nothing was found, expect a closing bracket and stop
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            ASSERT(c == '}', GFraMe_ret_failed);
            break;
        }
    }
    ASSERT(x >= 0, GFraMe_ret_failed);
    ASSERT(y >= 0, GFraMe_ret_failed);
    ASSERT(w > 0, GFraMe_ret_failed);
    ASSERT(h > 0, GFraMe_ret_failed);
    ASSERT(f != 0, GFraMe_ret_failed);
    
    // Create the object
    obj_setZero(pO);
    obj_setBounds(pO, x*8, y*8, w*8, h*8);
    obj_setID(pO, f);
    obj_setCommonEvent(pO, ce);
    while (gvsUsed > 0) {
        gvsUsed--;
        obj_setVar(pO, gvsUsed, gvs[gvsUsed]);
    }
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Set a tile into the buffer, expanding it if necessary
 * 
 * @param ppData Buffer that will contain the tilemap
 * @param pDataLen Size of the buffer
 * @param i Horizontal position in the buffer
 * @param h Vertical position in the buffer
 * @param w Width of the buffer
 * @param val Value to be set into the buffer
 * @param GFraMe error code
 */
static GFraMe_ret parse_setTile(unsigned char **pData, int *pDataLen, int i,
    int h, int w, int val) {
    GFraMe_ret rv;
    
    // If the buffer is too small, double its size
    if (i + h*w >= *pDataLen) {
        *pDataLen *= 2;
        
        *pData = (unsigned char*)realloc(*pData, *pDataLen);
        ASSERT(*pData, GFraMe_ret_memory_error);
    }
    
    // Set the tile
    (*pData)[i + h * w] = val;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Parse a tilemap from a file, alloc'ing it's buffer (it's actually recycled!)
 * and returning the width and height in tiles.
 * A tilemap must follow the rule:
 * "tm:" '[' ((int',')+ '\n')+ ']'
 * 
 * @param ppData Buffer that will contain the tilemap; if it's being recycled,
 *               pDataLen must have the buffer current size!
 * @param pDataLen Buffer final len (and initial, if ppData isn't NULL)
 * @param pW Tilemap's width in tiles
 * @param pH Tilemap's height in tiles
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_tilemap(unsigned char **ppData, int *pDataLen, int *pW,
    int *pH, FILE *fp) {
    unsigned char *data;
    fpos_t pos;
    GFraMe_ret rv;
    int c, dataLen, h, i, irv, w;
    
    // Intialize this, so it can be cleaned
    data = NULL;
    
    // Sanitize parameters
    ASSERT(ppData, GFraMe_ret_bad_param);
    ASSERT(pDataLen, GFraMe_ret_bad_param);
    ASSERT(!*ppData || *pDataLen > 0, GFraMe_ret_bad_param);
    ASSERT(pW, GFraMe_ret_bad_param);
    ASSERT(pH, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the next "token" must be a tilemap
    rv = parsef_string(fp, "tm:", 3);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT(c == '[', GFraMe_ret_failed);
    
    // Set the working buffer
    if (!*ppData) {
        dataLen = 2;
        data = (unsigned char*)malloc(sizeof(char)*dataLen);
        ASSERT(data, GFraMe_ret_memory_error);
    }
    else {
        dataLen = *pDataLen;
        data = *ppData;
    }
    
    // Parse the tilemap
    w = 0;
    h = 0;
    while (1) {
        parsef_ignoreWhitespace(fp, 1);
        
        // Check if the end of the array was reached
        c = fgetc(fp);
        ASSERT(c != EOF, GFraMe_ret_failed);
        if (c == ']')
            break;
        ungetc(c, fp);
        
        // Get every tile in a line
        i = 0;
        while (1) {
            int n;
            
            // Read the current tile (or stop)
            rv = parsef_int(&n, fp);
            if (rv != GFraMe_ret_ok)
                break;
            
            // After a digit, a comma MUST follow
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            ASSERT(c == ',', GFraMe_ret_failed);
            
            // Set the last read tile
            parse_setTile(&data, &dataLen, i, h, w, n);
            
            // Ignore everything but '\n'
            parsef_ignoreWhitespace(fp, 0);
            
            // Go to the next column
            i++;
            
            // Check if the line ended
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            if (c == '\n') {
                parsef_ignoreWhitespace(fp, 1);
                break;
            }
            ungetc(c, fp);
        }
        if (i > w)
            w = i;
        
        // Go to the next line
        h++;
    }
    
    // Check that the tilemap indeed ended
    ASSERT(c == ']', GFraMe_ret_failed);
    parsef_ignoreWhitespace(fp, 1);
    
    // Set the camera's dimension
    cam_setMapDimension(w * 8, h * 8);
    
    // Set the function's return
    *ppData = data;
    *pDataLen = dataLen;
    *pW = w;
    *pH = h;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && rv != GFraMe_ret_bad_param) {
        fsetpos(fp, &pos);
        // If 'data' was allocated here
        if (!*ppData && data)
            free(data);
    }
    
    return rv;
}

/**
 * Parse a map from a file
 * 
 * @param ppM Returns the map
 * @param fn The file's name
 * @return GFraMe error code
 */
GFraMe_ret parsef_map(map **ppM, char *fn) {
    FILE *fp;
    GFraMe_ret rv;
    map *pM;
    
    // Intialize this, so it can be cleaned
    pM = NULL;
    
    // Sanitize parameters
    ASSERT(ppM, GFraMe_ret_bad_param);
    ASSERT(fn, GFraMe_ret_bad_param);
    
    fp = fopen(fn, "rb");
    ASSERT(fp, GFraMe_ret_file_not_found);
    
    // Get the working map
    if (*ppM)
        pM = *ppM;
    else {
        rv = map_init(&pM);
        ASSERT(rv == GFraMe_ret_ok, GFraMe_ret_memory_error);
    }
    map_reset(pM);
    rg_reset();
    
    while (1) {
        unsigned char *pData;
        event *e;
        int c, h, len, w;
        object *o;
        mob *m;
        
        // Retrieve a event from map, in case it's parsed
        rv = rg_getNextEvent(&e);
        ASSERT(rv == GFraMe_ret_ok, rv);
        // Retrieve the current map's data, to recycle it
        rv = map_getTilemapData(&pData, &len, pM);
        ASSERT(rv == GFraMe_ret_ok, rv);
        rv = rg_getNextObject(&o);
        ASSERT(rv == GFraMe_ret_ok, rv);
        rv = rg_getNextMob(&m);
        ASSERT(rv == GFraMe_ret_ok, rv);
        
        // Try to parse a event
        
        // Try to parse a event
        rv = parsef_event(e, fp);
        if (rv == GFraMe_ret_ok) {
            rg_pushEvent();
            continue;
        }
        // Try to parse a tilemap
        rv = parsef_tilemap(&pData, &len, &w, &h, fp);
        if (rv == GFraMe_ret_ok) {
            map_setTilemap(pM, pData, len, w, h);
            continue;
        }
        rv = parsef_object(o, fp);
        if (rv == GFraMe_ret_ok) {
            rg_pushObject();
            continue;
        }
        rv = parsef_mob(m, fp);
        if (rv == GFraMe_ret_ok) {
            rg_pushMob();
            continue;
        }
        // TODO parse other structures
        
        // Ignore whitespace and check that the file ended
        parsef_ignoreWhitespace(fp, 1);
        c = fgetc(fp);
        ASSERT(c == EOF, GFraMe_ret_failed);
        break;
    }
    
    *ppM = pM;
    rv = GFraMe_ret_ok;
__ret:
    // Backtrack on error
    if (rv != GFraMe_ret_ok && !*ppM && pM)
            free(pM);
    
    return rv;
}

/**
 * Parse a mob from a file
 * 
 * @param pM Returns the parsed mob
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_mob(mob *pM, FILE *fp) {
    flag f;
    fpos_t pos;
    GFraMe_ret rv;
    int c, irv, x, y;
    
    // Sanitize parameters
    ASSERT(pM, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the next "token" must be an event
    rv = parsef_string(fp, "mob:", 4);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Open a bracket =D
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT(c == '{', GFraMe_ret_failed);
    parsef_ignoreWhitespace(fp, 1);
    
    // Get every parameter needed for the object, one at a time
    x = -1;
    y = -1;
    f = 0;
    while (1) {
        if (parsef_string(fp, "x:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&x, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "y:", 2) == GFraMe_ret_ok) {
            rv = parsef_int(&y, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "f:", 2) == GFraMe_ret_ok) {
            rv = parsef_flags(&f, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
            ASSERT(f != 0, GFraMe_ret_failed);
        }
        else {
            // If nothing was found, expect a closing bracket and stop
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            ASSERT(c == '}', GFraMe_ret_failed);
            break;
        }
    }
    ASSERT(x >= 0, GFraMe_ret_failed);
    ASSERT(y >= 0, GFraMe_ret_failed);
    ASSERT(f != 0, GFraMe_ret_failed);
    
    // Create the mob
    rv = mob_init(pM, x, y, f);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}
