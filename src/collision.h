/**
 * @file src/collision.h
 * 
 * Handler for collisions
 */
#ifndef __COLLISION_H_
#define __COLLISION_H_

#include <GFraMe/GFraMe_object.h>

#include "bullet.h"
#include "event.h"
#include "mob.h"
#include "object.h"
#include "player.h"

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
 * Collid a player against a mob
 * 
 * @param pPl The player
 * @param pMob The mob
 */
void col_onPlMob(player *pPl, mob *pMob);

/**
 * Collide an event against an object
 * 
 * @param pEv The event
 * @param pObj The object
 */
void col_onEvObj(event *pEv, object *pObj);

/**
 * Collide an event against a mob
 * 
 * @param pEv The event
 * @param pMob The mob
 */
void col_onEvMob(event *pEv, mob *pMob);

/**
 * Collide an object against an object
 * 
 * @param pObj1 A object
 * @param pObj2 A object
 */
void col_onObject(object *pObj1, object *pObj2);

/**
 * Collide an object against a mob
 * 
 * @param pObj The object
 * @param pMob The mob
 */
void col_onObjMob(object *pObj, mob *pMob);

/**
 * Collide an object against a wall
 * 
 * @param pObj The object
 * @param pWall The wall
 */
void col_onObjWall(object *pObj, GFraMe_object *pWall);

/**
 * Collide a mob against a wall
 * 
 * @param pMob The mob
 * @param pWall The wall
 */
void col_onMobWall(mob *pMob, GFraMe_object *pWall);

/**
 * Collide two mobs
 * 
 * @param pMob1 A mob
 * @param pMob2 A mob
 */
void col_onMob(mob *pMob1, mob *pMob2);

/**
 * Collide a player against a bullet
 * 
 * @param pPl The player
 * @param pBul The bullet
 */
void col_onPlBul(player *pPl, bullet *pBul);

/**
 * Collide an event against a bullet
 * 
 * @param pEv The event
 * @param pBul The bullet
 */
void col_onEvBul(event *pEv, bullet *pBul);

/**
 * Collide an object against a bullet
 * 
 * @param pObj The object
 * @param pBul The bullet
 */
void col_onObjBul(object *pObj, bullet *pBul);

/**
 * Collide a mob against a bullet
 * 
 * @param pMob The mob
 * @param pBul The wall
 */
void col_onMobBul(mob *pMob, bullet *pBul);

/**
 * Collide a bullet against a wall
 * 
 * @param pWall The wall
 * @param pBul The bullet
 */
void col_onBulWall(bullet *pBul, GFraMe_object *pWall);

/**
 * Collide two bullets
 * 
 * @param pBul1 The bullet
 * @param pBul2 The bullet
 */
void col_onBul(bullet *pBul1, bullet *pBul2);

#endif 

