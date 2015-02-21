/**
 * @file src/quadtree/qtstatic.c
 * 
 * All the "static" variables used by the quadtree (also, functions to manage)
 */
#include <GFraMe/GFraMe_error.h>

#include "qtnode.h"
#include "qtstatic.h"
#include "quadtree.h"

#include "../global.h"

#include <stdlib.h>

//============================================================================//
//                                                                            //
// Static variables                                                           //
//                                                                            //
//============================================================================//

static quadtree *qts = 0;
static int qtsUsed = 0;
static int qtsLen = 0;

static qtNode *qtNodes = 0;
static int qtNodesUsed = 0;
static int qtNodesLen = 0;

static struct stQTNodeLL *qtNodeLLs = 0;
static int qtNodeLLsUsed = 0;
static int qtNodeLLsLen = 0;

//============================================================================//
//                                                                            //
// Functions                                                                  //
//                                                                            //
//============================================================================//

/**
 * Clean up all memory allocated
 */
void qt_staticClean() {
    if (qts) {
        free(qts);
        qts = 0;
        qtsUsed = 0;
        qtsLen = 0;
    }
    if (qtNodes) {
        free(qtNodes);
        qtNodes = 0;
        qtNodesUsed = 0;
        qtNodesLen = 0;
    }
    if (qtNodeLLs) {
        free(qtNodeLLs);
        qtNodeLLs = 0;
        qtNodeLLsUsed = 0;
        qtNodeLLsLen = 0;
    }
}

/**
 * Get a valid quadtree structure for the root
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getNewRoot(quadtree **ppQt) {
    GFraMe_ret rv;
    
    // Create the first few quadtrees, if necessary
    if (qtsLen == 0) {
        qts = (quadtree*)malloc(sizeof(quadtree)*5);
        ASSERT(qts, GFraMe_ret_memory_error);
        qtsLen = 5;
    }
    
    // Set the return variable
    *ppQt = &qts[0];
    qtsUsed = 1;
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
    *ppQt = &qts[0];
}


/**
 * Get a valid quadtree structure
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getQuadtree(quadtree **ppQt) {
    GFraMe_ret rv;
    
    // Check if there're enough quadtrees on the buffer
    if (qtsUsed >= qtsLen) {
        // Alloc 4 more quadtrees
        qtsLen += 4;
        qts = (quadtree*)realloc(qts, sizeof(quadtree)*qtsLen);
        ASSERT(qts, GFraMe_ret_memory_error);
    }
    
    // Set the return variable
    *ppQt = &qts[qtsUsed];
    qtsUsed++;
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
    
    // Check if there're enough nodes on the buffer
    if (qtNodesUsed >= qtNodesLen) {
        // Alloc 4 more quadtrees
        qtNodesLen += NODES_MAX;
        qtNodes = (qtNode*)realloc(qtNodes, sizeof(qtNode)*qtNodesLen);
        ASSERT(qtNodes, GFraMe_ret_memory_error);
    }
    
    // Set the return variable
    *ppNode = &qtNodes[qtNodesUsed];
    qtNodesUsed++;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Reset every node and quadtree (actually, only clean the used len)
 */
void qt_resetAll() {
    qtsUsed = 0;
    qtNodesUsed = 0;
    qtNodeLLsUsed = 0;
}

/**
 * Get a "new" node for the linked list
 * 
 * @param ppNodeLL The linked list node
 * @return GFraMe error code
 */
GFraMe_ret qt_getNodeLL(struct stQTNodeLL **ppNodeLL) {
    GFraMe_ret rv;
    
    // Check if there're enough nodes on the buffer
    if (qtNodeLLsUsed >= qtNodeLLsLen) {
        // Alloc 4 more quadtrees
        qtNodeLLsLen += NODES_MAX;
        qtNodeLLs = (struct stQTNodeLL*)realloc(qtNodeLLs, sizeof(struct stQTNodeLL)*qtNodeLLsLen);
        ASSERT(qtNodeLLs, GFraMe_ret_memory_error);
    }
    
    // Set the return variable
    *ppNodeLL = &qtNodeLLs[qtNodeLLsUsed];
    qtNodeLLsUsed++;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

