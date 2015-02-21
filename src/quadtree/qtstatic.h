/**
 * @file src/quadtree/qtstatic.h
 * 
 * All the "static" variables used by the quadtree (also, functions to manage)
 */
#ifndef __QT_STATIC_H_
#define __QT_STATIC_H_

#include "quadtree.h"

#include <GFraMe/GFraMe_object.h>

#include "../event.h"
#include "../player.h"
#include "../object.h"

//============================================================================//
//                                                                            //
// Defines                                                                    //
//                                                                            //
//============================================================================//

#define NODES_MAX  6
#define MIN_WIDTH  16
#define MIN_HEIGHT 16

//============================================================================//
//                                                                            //
// Structures                                                                 //
//                                                                            //
//============================================================================//

struct stQTHitbox {
    /** Center's horizontal position (in world space) */
    int cx;
    /** Center's vertical position (in world space) */
    int cy;
    /** Half the width */
    int hw;
    /** Half the height */
    int hh;
};

struct stQTNode {
    /** This nodes hitbox (& position in space) */
    struct stQTHitbox hb;
    /** Type of the node (required to retrieve the object) */
    nodeType type;
    /** Pointer to the object */
    union {
        player *pl;
        event *ev;
        object *obj;
        GFraMe_object *wall;
    } self;
    // TODO create a list of already collided nodes?
};

struct stQTNodeLL {
    struct stQTNode *self;
    struct stQTNodeLL *next;
};

struct stQT {
    /** This tree's boundaries */
    struct stQTHitbox hb;
    /** Pointers to the next branches */
    struct stQT *children[QT_MAX];
    /** Linked list of nodes */
    struct stQTNodeLL *nodes;
    /** How many entries there are in the nodes linked list */
    int nodesCount;
};

//============================================================================//
//                                                                            //
// Functions                                                                  //
//                                                                            //
//============================================================================//

/**
 * Clean up all memory allocated
 */
void qt_staticClean();

/**
 * Get a valid quadtree structure for the root
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getNewRoot(quadtree **ppQt);

/**
 * Get the current quadtree's root
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
void qt_getRoot(quadtree **ppQt);


/**
 * Get a valid quadtree structure
 * 
 * @param ppQt The root quadtree
 * @return GFraMe error code
 */
GFraMe_ret qt_getQuadtree(quadtree **ppQt);

/**
 * Get a new node
 * 
 * @param ppNode The node
 * @return GFraMe error code
 */
GFraMe_ret qt_getNode(qtNode **ppNode);

/**
 * Reset every node and quadtree (actually, only clean the used len)
 */
void qt_resetAll();

/**
 * Get a "new" node for the linked list
 * 
 * @param ppNodeLL The linked list node
 * @return GFraMe error code
 */
GFraMe_ret qt_getNodeLL(struct stQTNodeLL **ppNodeLL);

#endif

