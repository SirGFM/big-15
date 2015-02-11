/**
 * @file src/commonEvent.h
 * 
 * Define (and implement) every callback
 */
#ifndef __COMMONEVENT_H_
#define __COMMONEVENT_H_

typedef enum {
    CE_TEST_DOOR1,
    CE_HANDLE_DOOR,
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

