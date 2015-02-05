/**
 * @file src/parser.c
 * 
 * Module for parsing every 'object', both from string and file
 */
#include <GFraMe/GFraMe_error.h>

#include <stdio.h>

#include "commonEvent.h"
#include "event.h"
#include "parser.h"

#define ASSERT(stmt, err) \
  do { \
    if (!(stmt)) { \
      rv = err; \
      goto __ret; \
    } \
  } while (0)

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
        
        if (c == EOF || c != ' ' || c != '\t' || c != '\r' ||
            (ignoreNewline && c != '\n') || (!ignoreNewline && c == '\n'))
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
    int c, i, irv;
    
    // Sanitize parameters
    ASSERT(pI, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the first character is a digit
    c = fgetc(fp);
    ASSERT(c != EOF, GFraMe_ret_failed);
    ASSERT(c >= '0' && c <= '9', GFraMe_ret_failed);
    
    // Parse the integer
    i = 0;
    while (1) {
        i = i * 10 + (c - '0');
        
        // Read the next character and stop if it isn't a digit
        c = fgetc(fp);
        if (c == EOF || c < '0' || c > '9')
            break;
    }
    
    // Get to the next valid character
    parsef_ignoreWhitespace(fp, 1);
    
    *pI = i;
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
 * Parse a common event from a file.
 * When it's expected, it must simply be a string between double quotes, i.e.:
 * '"'string'"'
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
        int c, i;
        
        // Get the current event's name
        ceName = ce_getName(ce);
        
        // Check that the first character is a '"'
        c = fgetc(fp);
        ASSERT(c != EOF, GFraMe_ret_failed);
        ASSERT(c == '"', GFraMe_ret_failed);
        
        // Try to match every other character to the current event
        i = 0;
        while (1) {
            c = fgetc(fp);
            ASSERT(c != EOF, GFraMe_ret_failed);
            
            // Stop either at the string end or on an unmatched character
            if (c == '"' || c != ceName[i])
                break;
            
            i++;
        }
        
        // If the string ended, stop
        if (i != 0 && c == '"' && ceName[i + 1] == '\0')
            break;
        
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
 * "e:" "x:"int "y:"int "w:"int "h:"int "ce:"commonEventName "t:"int
 * All the numbers are read as tiles (i.e., multiplied by 8)
 * 
 * @param pE Returns the parsed event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_event(event *pE, FILE *fp) {
    commonEvent ce;
    event e;
    fpos_t pos;
    GFraMe_ret rv;
    int c, irv, h, w, x, y;
    trigger t;
    
    // Sanitize parameters
    ASSERT(pE, GFraMe_ret_bad_param);
    ASSERT(fp, GFraMe_ret_bad_param);
    
    // Get the current position, to "backtrack" on error
    irv = fgetpos(fp, &pos);
    ASSERT(irv == 0, GFraMe_ret_failed);
    
    // Check that the next "token" must be an event
    rv = parsef_string(fp, "e:", 2);
    ASSERT(rv == GFraMe_ret_ok, rv);
    
    // Get every parameter needed for the event, one at a time
    x = -1;
    y = -1;
    w = -1;
    h = -1;
    t = 0;
    ce = CE_MAX;
    while (x < 0 || y < 0 || w < 0 || h < 0 || t <= 0 || ce == CE_MAX) {
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
            rv = parsef_int(&t, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else if (parsef_string(fp, "ce:", 3) == GFraMe_ret_ok) {
            rv = parsef_commonEvent(&ce, fp);
            ASSERT(rv == GFraMe_ret_ok, rv);
        }
        else
            ASSERT(0, GFraMe_ret_failed);
    }
    
    // Create the event
    rv = event_init(pE, x*8, y*8, w*8, h*8, trigger t, ce);
    
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
static GFraMe_ret parse_setTile(char **pData, int *pDataLen, int i, int h,
    int w, int val) {
    GFraMe_ret rv;
    
    // If the buffer is too small, double its size
    if (i + h*w >= *pDataLen) {
        *pDataLen *= 2;
        
        *pData = (char*)realloc(*pData, *pDataLen);
        ASSERT(*pData, GFraMe_memory_error);
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
GFraMe_ret parsef_tilemap(char **ppData, int *pDataLen, int *pW, int *pH,
    FILE *fp) {
    char *data;
    fpos_t pos;
    GFraMe_ret rv;
    int c, dataLen, h, i, w;
    
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
        data = (char*)malloc(sizeof(char)*dataLen);
        ASSERT(data, GFraMe_memory_error);
    }
    else {
        dataLen = *pDataLen;
        data = *ppDataLen;
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
            
            i++;
        }
        if (i > w)
            w = i;
        
        // After reading 'i' tiles, a '\n' must be found
        c = fgetc(fp);
        ASSERT(c != EOF, GFraMe_ret_failed);
        ASSERT(c == '\n', GFraMe_ret_failed);
        
        // Go to the next column
        h++;
    }
    
    // Check that the tilemap indeed ended
    ASSERT(c == ']', GFraMe_ret_failed);
    
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
 * Parse a mob from a file
 * 
 * @param pM Returns the parsed mob
 * @param fp File pointer
 * @return GFraMe error code
 */
// GFraMe_ret parsef_mob(mob *pM, FILE *fp);

/**
 * Parse a object from a file
 * 
 * @param pO Returns the parsed object
 * @param fp File pointer
 * @return GFraMe error code
 */
// GFraMe_ret parsef_obj(obj *pO, FILE *fp);

