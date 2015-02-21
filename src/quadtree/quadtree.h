/**
 * @file src/quadtree/quadtree.h
 */
#ifndef __QUADTREE_H_
#define __QUADTREE_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "qtnode.h"

#include "../commonEvent.h"
#include "../event.h"
#include "../object.h"
#include "../player.h"

typedef enum {
    NW,
    NE,
    SW,
    SE,
    QT_MAX
} qtPosition;

typedef struct stQT quadtree;

/**
 * Clean up all memory used by the quadtree
 */
void qt_clean();

/**
 * Initializes a quadtree according to its parent'a position
 * 
 * @param pQt The quadtree
 * @param pParent The quadtree's parent
 * @param pos Quadtree position relative to its parent's center
 */
void qt_init(quadtree *pQt, quadtree *pParent, qtPosition pos);

/**
 * Initialize the quadtree for collision
 * 
 * @param w World's width
 * @param h World's height
 * @return GFraMe error code
 */
GFraMe_ret qt_initCol(int w, int h);

/**
 * Adds a player and collides against everything else
 * 
 * @param pPl The player
 * @return GFraMe error code
 */
GFraMe_ret qt_addPl(player *pPl);

/**
 * Adds an event and collides against everything else
 * 
 * @param pEv The event
 * @return GFraMe error code
 */
GFraMe_ret qt_addEv(event *pEv);

/**
 * Adds an object and collides against everything else
 * 
 * @param pObj The object
 * @return GFraMe error code
 */
GFraMe_ret qt_addObj(object *pObj);

/**
 * Adds a wall (a static, collideable object) and collides against everything
 * 
 * @param pWall The wall
 * @return GFraMe error code
 */
GFraMe_ret qt_addWall(GFraMe_object *pWall);

/**
 * Add a new node to the quadtree, subdivide it as necessary and collide against
 * every other node
 * 
 * @param pQt Tree where the collision should be done
 * @param pNode The node
 * @return GFraMe error code
 */
GFraMe_ret qt_addNodeCollide(quadtree *pQt, qtNode *pNode);

#  ifdef QT_DEBUG_DRAW
/**
 * Draw a quadtree's (and its children's) bounding box
 * 
 * @param pQt The quadtree
 */
void qt_drawDebug(quadtree *pQt)/
#  endif

#endif

