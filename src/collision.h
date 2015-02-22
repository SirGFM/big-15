/**
 * @file src/collision.h
 * 
 * Handler for collisions
 */
#ifndef __COLLISION_H_
#define __COLLISION_H_

#include "quadtree/qtnode.h"

/**
 * Try to collide two nodes
 * 
 * @param n1 A node
 * @param n2 A node
 */
void checkCollision(qtNode *n1, qtNode *n2);

/**
 * Collide two players
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void col_onPlayer(player *pPl1, player *pPl2);

/**
 * Collide a player against an event
 * 
 * @param pPl The player
 * @param pEv The event
 */
void col_onPlEv(player *pPl, event *pEv);

/**
 * Collide a player against an object
 * 
 * @param pPl The player
 * @param pObj The object
 */
void col_onPlObj(player *pPl, object *pObj);

/**
 * Collide a player against a wall
 * 
 * @param pPl The player
 * @param pWall The wall
 */
void col_onPlWall(player *pPl, GFraMe_object *pWall);

/**
 * Collide an event against an object
 * 
 * @param pEv The event
 * @param pObj The object
 */
void col_onEvObj(event *pEv, object *pObj);

/**
 * Collide an object against an object
 * 
 * @param pObj1 A object
 * @param pObj2 A object
 */
void col_onObject(object *pObj1, object *pObj2);

/**
 * Collide an object against a wall
 * 
 * @param pObj The object
 * @param pWall The wall
 */
void col_onObjWall(object *pObj, GFraMe_object *pWall);

#endif 

