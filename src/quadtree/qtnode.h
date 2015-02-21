/**
 * @file src/quadtree/qtnode.h
 */
#ifndef __QT_NODE_H_
#define __QT_NODE_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "../event.h"
#include "../object.h"
#include "../player.h"

typedef enum {
    QNT_PL,
    QNT_WALL,
    QNT_OBJ,
    QNT_EV,
    QNT_MOB,
    QNT_MAX
} nodeType;

typedef struct stQTNode qtNode;

/**
 * Set a object's hitbox
 * 
 * @param pNode The node
 * @param pObj The node's object
 */
void qt_setNodeDimension(qtNode *pNode, GFraMe_object *pObj);

/**
 * Get a node and assign it a player
 * 
 * @param ppNode The node
 * @param pPl The player
 * @return GFraMe error code
 */
GFraMe_ret qt_getPlNode(qtNode **ppNode, player *pPl);

/**
 * Get a node and assign it an event
 * 
 * @param ppNode The node
 * @param pEv The event
 * @return GFraMe error code
 */
GFraMe_ret qt_getEvNode(qtNode **ppNode, event *pEv);

/**
 * Get a node and assign it an object
 * 
 * @param ppNode The node
 * @param pObj The object
 * @return GFraMe error code
 */
GFraMe_ret qt_getObjNode(qtNode **ppNode, object *pObj);

/**
 * Get a node and assign it a wall (a GFraMe_object)
 * 
 * @param ppNode The node
 * @param pWall The wall
 * @return GFraMe error code
 */
GFraMe_ret qt_getWallNode(qtNode **ppNode, GFraMe_object *pWall);

#endif

