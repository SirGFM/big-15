/**
 * @file src/quadtree/qtnode.h
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "qtnode.h"
#include "qtstatic.h"

#include "../event.h"
#include "../global.h"
#include "../object.h"
#include "../player.h"

/**
 * Set a object's hitbox
 * 
 * @param pNode The node
 * @param pObj The node's object
 */
void qt_setNodeDimension(qtNode *pNode, GFraMe_object *pObj) {
    pNode->hb.cx = pObj->x + pObj->hitbox.cx;
    pNode->hb.cy = pObj->y + pObj->hitbox.cy;
    pNode->hb.hw = pObj->hitbox.hw;
    pNode->hb.hh = pObj->hitbox.hh;
}

/**
 * Get a node and assign it a player
 * 
 * @param ppNode The node
 * @param pPl The player
 * @return GFraMe error code
 */
GFraMe_ret qt_getPlNode(qtNode **ppNode, player *pPl) {
    GFraMe_object *pObj;
    GFraMe_ret rv;
    qtNode *pNode;
    
    // Get a new node
    rv = qt_getNode(&pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Set the node reference
    pNode->self.pl = pPl;
    pNode->type = QNT_PL;
    // Get the player's object and set the node's dimension
    player_getObject(&pObj, pPl);
    qt_setNodeDimension(pNode, pObj);
    
    // Set the return variable
    *ppNode = pNode;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get a node and assign it an event
 * 
 * @param ppNode The node
 * @param pEv The event
 * @return GFraMe error code
 */
GFraMe_ret qt_getEvNode(qtNode **ppNode, event *pEv) {
    GFraMe_object *pObj;
    GFraMe_ret rv;
    qtNode *pNode;
    
    // Get a new node
    rv = qt_getNode(&pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Set the node reference
    pNode->self.ev = pEv;
    pNode->type = QNT_EV;
    // Get the event's object and set the node's dimension
    event_getObject(&pObj, pEv);
    qt_setNodeDimension(pNode, pObj);
    
    // Set the return variable
    *ppNode = pNode;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get a node and assign it an object
 * 
 * @param ppNode The node
 * @param pObj The object
 * @return GFraMe error code
 */
GFraMe_ret qt_getObjNode(qtNode **ppNode, object *pObj) {
    GFraMe_object *pGfmObj;
    GFraMe_ret rv;
    qtNode *pNode;
    
    // Get a new node
    rv = qt_getNode(&pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Set the node reference
    pNode->self.obj = pObj;
    pNode->type = QNT_OBJ;
    // Get the object's object and set the node's dimension
    obj_getObject(&pGfmObj, pObj);
    qt_setNodeDimension(pNode, pGfmObj);
    
    // Set the return variable
    *ppNode = pNode;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get a node and assign it a wall (a GFraMe_object)
 * 
 * @param ppNode The node
 * @param pWall The wall
 * @return GFraMe error code
 */
GFraMe_ret qt_getWallNode(qtNode **ppNode, GFraMe_object *pWall) {
    GFraMe_ret rv;
    qtNode *pNode;
    
    // Get a new node
    rv = qt_getNode(&pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Set the node reference
    pNode->self.wall = pWall;
    pNode->type = QNT_WALL;
    // Set the node's dimension
    qt_setNodeDimension(pNode, pWall);
    
    // Set the return variable
    *ppNode = pNode;
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Get the color a given type should be rendered
 * 
 * @param ppNode The node
 * @param r Red color component
 * @param g Green color component
 * @param b Blue color component
 */
void qt_getTypeColor(qtNode *pNode, int *pR, int *pG, int *pB) {
    switch (pNode->type) {
        case QNT_PL: {
            *pR = 0xd7;
            *pG = 0x7b;
            *pB = 0xba;
        } break;
        case QNT_WALL: {
            *pR = 0x8f;
            *pG = 0x56;
            *pB = 0x3b;
        } break;
        case QNT_OBJ: {
            *pR = 0xdf;
            *pG = 0x71;
            *pB = 0x26;
        } break;
        case QNT_EV: {
            *pR = 0xfb;
            *pG = 0xf2;
            *pB = 0x36;
        } break;
        case QNT_MOB: {
            *pR = 0xac;
            *pG = 0x32;
            *pB = 0x32;
        } break;
        default: {}
    }
}

/**
 * Get the node's referenced object
 * 
 * @param ppPl The player
 * @param ppEv The event
 * @param ppObj The object
 * @param ppWall The wall
 * @param pNode The node
 */
void qt_getRef(player **ppPl, event **ppEv, object **ppObj,
    GFraMe_object **ppWall, qtNode *pNode) {
    *ppPl = 0;
    *ppEv = 0;
    *ppObj = 0;
    *ppWall = 0;
    switch (pNode ->type) {
        case QNT_PL: {
            *ppPl = pNode->self.pl;
        }break;
        case QNT_WALL: {
            *ppWall = pNode->self.wall;
        }break;
        case QNT_OBJ: {
            *ppObj = pNode->self.obj;
        }break;
        case QNT_EV: {
            *ppEv = pNode->self.ev;
        }break;
        case QNT_MOB: {
        }break;
        default: {}
    }
}

