/**
 * @file src/parser.h
 * 
 * Module for parsing every 'object', both from string and file
 */
#ifndef __PARSER_H_
#define __PARSER_H_

#include <GFraMe/GFraMe_error.h>

#include <stdio.h>

#include "commonEvent.h"
#include "event.h"

/**
 * Parse a common event from a file
 * When it's expected, it must simply be a string between double quotes, i.e.:
 * '"'[a-zA-Z]+'"'
 * 
 * @param pCe Returns the parsed common event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_commonEvent(commonEvent *pCe, FILE *fp);

/**
 * Parse a event from a file
 * 
 * @param pE Returns the parsed event
 * @param fp File pointer
 * @return GFraMe error code
 */
GFraMe_ret parsef_event(event *pE, FILE *fp);

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
    FILE *fp);

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

#endif

