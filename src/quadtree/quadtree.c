/**
 * @file src/quadtree/quadtree.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "qthitbox.h"
#include "qtstatic.h"
#include "quadtree.h"

#include "../commonEvent.h"
#include "../event.h"
#include "../global.h"
#include "../object.h"
#include "../player.h"

/**
 * Clean up all memory used by the quadtree
 */
void qt_clean() {
    // TODO
}

/**
 * Initializes a quadtree according to its parent'a position
 * 
 * @param pQt The quadtree
 * @param pParent The quadtree's parent
 * @param pos Quadtree position relative to its parent's center
 */
void qt_init(quadtree *pQt, quadtree *pParent, qtPosition pos) {
    // Clean up the child's children
    pQt->children[NW] = 0;
    pQt->children[NE] = 0;
    pQt->children[SW] = 0;
    pQt->children[SE] = 0;
    // Clean up its nodes
    pQt->nodes = 0;
    pQt->nodesCount = 0;
    // Copy the parent's position and halve the dimensions
    if (pParent) {
        pQt->hb.cx = pParent->hb.cx;
        pQt->hb.cy = pParent->hb.cy;
        pQt->hb.hw = pParent->hb.hw / 2;
        pQt->hb.hh = pParent->hb.hh / 2;
    }

    // Adjust the relative position
    switch (pos) {
        case NW: {
            pQt->hb.cx -= pQt->hb.hw;
            pQt->hb.cy -= pQt->hb.hh;
        } break;
        case NE: {
            pQt->hb.cx += pQt->hb.hw;
            pQt->hb.cy -= pQt->hb.hh;
        } break;
        case SW: {
            pQt->hb.cx -= pQt->hb.hw;
            pQt->hb.cy += pQt->hb.hh;
        } break;
        case SE: {
            pQt->hb.cx += pQt->hb.hw;
            pQt->hb.cy += pQt->hb.hh;
        } break;
        default: {}
    }
}

/**
 * Initialize the quadtree for collision
 * 
 * @param w World's width
 * @param h World's height
 * @return GFraMe error code
 */
GFraMe_ret qt_initCol(int w, int h) {
    GFraMe_ret rv;
    quadtree *pRoot;
    
    // Reset any "used" node/qt
    qt_resetAll();
    
    // Get the tree's root
    rv = qt_getRoot(&pRoot);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Init the root
    qt_init(pRoot, 0, QT_MAX);
    // Set its dimensions
    pRoot->hb.cx = w / 2;
    pRoot->hb.cy = h / 2;
    pRoot->hb.hw = w / 2;
    pRoot->hb.hh = h / 2;
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Adds a player and collides against everything else
 * 
 * @param pPl The player
 * @return GFraMe error code
 */
GFraMe_ret qt_addPl(player *pPl) {
    GFraMe_ret rv;
    qtNode *pNode;
    quadtree *pRoot;
    
    // Get a new node with the player
    rv = qt_getPlNode(&pNode, pPl);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Get the tree's root
    rv = qt_getRoot(&pRoot);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Add the node to the quadtree and collide against everything else
    rv = qt_addNodeCollide(pRoot, pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Adds an event and collides against everything else
 * 
 * @param pEv The event
 * @return GFraMe error code
 */
GFraMe_ret qt_addEv(event *pEv) {
    GFraMe_ret rv;
    qtNode *pNode;
    quadtree *pRoot;
    
    // Get a new node with the event
    rv = qt_getEvNode(&pNode, pEv);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Get the tree's root
    rv = qt_getRoot(&pRoot);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Add the node to the quadtree and collide against everything else
    rv = qt_addNodeCollide(pRoot, pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Adds an object and collides against everything else
 * 
 * @param pObj The object
 * @return GFraMe error code
 */
GFraMe_ret qt_addObj(object *pObj) {
    GFraMe_ret rv;
    qtNode *pNode;
    quadtree *pRoot;
    
    // Get a new node with the object
    rv = qt_getObjNode(&pNode, pObj);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Get the tree's root
    rv = qt_getRoot(&pRoot);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Add the node to the quadtree and collide against everything else
    rv = qt_addNodeCollide(pRoot, pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Adds a wall (a static, collideable object) and collides against everything
 * 
 * @param pWall The wall
 * @return GFraMe error code
 */
GFraMe_ret qt_addWall(GFraMe_object *pWall) {
    GFraMe_ret rv;
    qtNode *pNode;
    quadtree *pRoot;
    
    // Get a new node with the object
    rv = qt_getWallNode(&pNode, pWall);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Get the tree's root
    rv = qt_getRoot(&pRoot);
    ASSERT_NR(rv == GFraMe_ret_ok);
    // Add the node to the quadtree and collide against everything else
    rv = qt_addNodeCollide(pRoot, pNode);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

/**
 * Add a new node to the quadtree, subdivide it as necessary and collide against
 * every other node
 * 
 * @param pQt Tree where the collision should be done
 * @param pNode The node
 * @return GFraMe error code
 */
GFraMe_ret qt_addNodeCollide(quadtree *pQt, qtNode *pNode) {
    GFraMe_ret rv;
    
    // Check that the node intersects the tree
    ASSERT(qtHbIntersect(&pNode->hb, &pQt->hb), GFraMe_ret_ok);
    
    if (pQt->children[NW]) { // If there're any children, add into those
        qtPosition i;
        
        // Add the node into each child
        i = 0;
        while (i < QT_MAX) {
            rv = qt_addNodeCollide(pQt->children[i], pNode);
            ASSERT_NR(rv == GFraMe_ret_ok);
            i++;
        }
    }
    else if (pQt->nodesCount + 1 > NODES_MAX
             && pQt->hb.hw * 2 > MIN_WIDTH
             && pQt->hb.hh * 2 > MIN_HEIGHT) { // If the tree should subdivide
        qtPosition i;
        struct stQTNodeLL *tmp;
        
        // Subdivide the tree into each subtree
        i = 0;
        while (i < QT_MAX) {
            quadtree *tmp;
            
            // Get a new subtree
            rv = qt_getQuadtree(&tmp);
            ASSERT_NR(rv == GFraMe_ret_ok);
            // Initialize the child and add it
            qt_init(tmp, pQt, i);
            pQt->children[i] = tmp;
            
            i++;
        }
        // Add every node into the children
        tmp = pQt->nodes;
        while (tmp) {
            rv =  qt_addNodeCollide(pQt, tmp->self);
            ASSERT_NR(rv == GFraMe_ret_ok);
            
            tmp = tmp->next;
        }
    }
    else { // If the node will be added and collided
        struct stQTNodeLL *tmp;
        
        // Get a new node, to add into
        rv = qt_getNodeLL(&tmp);
        ASSERT_NR(rv == GFraMe_ret_ok);
        // Add the node into the linked list
        tmp->self = pNode;
        tmp->next = pQt->nodes;
        // Update the tree's list
        pQt->nodes = tmp;
        pQt->nodesCount++;
        
        // Collides this node against every other on the current subtree
        tmp = pQt->nodes->next;
        while (tmp) {
            // Check if this intersects the current node
            if (qtHbIntersect(&pNode->hb, &tmp->self->hb)) {
                // TODO overlap/collide
            }
            // Go to the next node
            tmp = tmp->next;
        }
    }
    
    rv = GFraMe_ret_ok;
__ret:
    return rv;
}

#ifdef QT_DEBUG_DRAW
/**
 * Draw a quadtree's (and its children's) bounding box
 * 
 * @param pQt The quadtree
 */
void qt_drawDebug(quadtree *pQt) {
    if (qt->children[NW]) { // If there're any children, draw their boxes
        qtPosition i;
        
        // Draw every child bounding box
        i = 0;
        while (i < QT_MAX) {
            qt_drawDebug(pQt->children[i]);
            i++;
        }
    }
    else { // If it's a leaf, draw it
        struct stQTNodeLL *tmp;
        
        // Render this subtree to the screen, in green
        qt_drawHitboxDebug(&pQt->hb, 0x00, 0xff, 0x00, 0xff);
        
        // Loop through every node in this one
        tmp = pQt->nodes;
        while (tmp) {
            // Render this node to the screen, in red
            qt_drawHitboxDebug(&tmp->self->hb, 0xff, 0x00, 0x00, 0xff);
            tmp = tmp->next;
        }
    }
}
#endif

