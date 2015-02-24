/**
 * @file src/quadtree/qtstatic.c
 * 
 * All the "static" variables used by the quadtree (also, functions to manage)
 */
#include <GFraMe/GFraMe_error.h>

#include <stdlib.h>
#include <string.h>

#include "qtnode.h"
#include "qtstatic.h"
#include "quadtree.h"

#include "../global.h"
#include "../static_buffer.h"

//============================================================================//
//                                                                            //
// Static variables                                                           //
//                                                                            //
//============================================================================//

typedef struct stQTNodeLL qtNodeLL;

BUF_DEFINE(quadtree);
BUF_DEFINE(qtNode);
BUF_DEFINE(qtNodeLL);

//============================================================================//
//                                                                            //
// Functions                                                                  //
//                                                                            //
//============================================================================//

/**
 * Alloc a new quadtree
 * 
 * @param ppQt The alloc'ed quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_initQt(quadtree **ppQt) {
    GFraMe_ret rv;
    
    BUF_ALOC_OBJ(quadtree, ppQt, GFraMe_ret_memory_error);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Alloc a new node
 * 
 * @param ppNode The alloc'ed node
 * @return GFraMe error code
 */
GFraMe_ret qt_initNode(qtNode **ppNode) {
    GFraMe_ret rv;
    
    BUF_ALOC_OBJ(qtNode , ppNode, GFraMe_ret_memory_error);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Alloc a new LL node
 * 
 * @param ppNodeLL The alloc'ed LL node
 * @return GFraMe error code
 */
GFraMe_ret qt_initLL(struct stQTNodeLL **ppNodeLL) {
    GFraMe_ret rv;
    
    BUF_ALOC_OBJ(qtNodeLL, ppNodeLL, GFraMe_ret_memory_error);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

void qt_cleanQt(quadtree **ppQt) {
    BUF_DEALLOC_OBJ(ppQt);
}

void qt_cleanNode(qtNode **ppNode) {
    BUF_DEALLOC_OBJ(ppNode);
}

void qt_cleanLL(struct stQTNodeLL **ppNodeLL) {
    BUF_DEALLOC_OBJ(ppNodeLL);
}

/**
 * Clean up all memory allocated
 */
void qt_staticClean() {
    BUF_CLEAN(quadtree, qt_cleanQt);
    BUF_CLEAN(qtNode, qt_cleanNode);
    BUF_CLEAN(qtNodeLL, qt_cleanLL);
}

/**
 * Get a valid quadtree structure for the root
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getNewRoot(quadtree **ppQt) {
    GFraMe_ret rv;
    
    BUF_SET_MIN_SIZE(quadtree, 5, GFraMe_ret_memory_error, qt_initQt);
    *ppQt = BUF_GET_OBJECT(quadtree, 0);
    BUF_PUSH(quadtree);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get the current quadtree's root
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
void qt_getRoot(quadtree **ppQt) {
    // Set the return variable
    *ppQt = BUF_GET_OBJECT(quadtree, 0);
}


/**
 * Get a valid quadtree structure
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getQuadtree(quadtree **ppQt) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(quadtree, 4, *ppQt, GFraMe_ret_memory_error, qt_initQt);
    BUF_PUSH(quadtree);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get a new node
 * 
 * @param ppNode The node
 * @return GFraMe error code
 */
GFraMe_ret qt_getNode(qtNode **ppNode) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(qtNode, NODES_MAX, *ppNode, GFraMe_ret_memory_error, qt_initNode);
    BUF_PUSH(qtNode);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Reset every node and quadtree (actually, only clean the used len)
 */
void qt_resetAll() {
    BUF_RESET(quadtree);
    BUF_RESET(qtNode);
    BUF_RESET(qtNodeLL);
}

/**
 * Get a "new" node for the linked list
 * 
 * @param ppNodeLL The linked list node
 * @return GFraMe error code
 */
GFraMe_ret qt_getNodeLL(struct stQTNodeLL **ppNodeLL) {
    GFraMe_ret rv;
    
    BUF_GET_NEXT_REF(qtNodeLL, NODES_MAX, *ppNodeLL, GFraMe_ret_memory_error, qt_initLL);
    BUF_PUSH(qtNodeLL);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

