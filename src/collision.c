/**
 * @file src/collision.h
 * 
 * Handler for collisions
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "collision.h"
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
void checkCollision(qtNode *n1, qtNode *n2) {
    player *pPl1, *pPl2;
    event *pEv1, *pEv2;
    mob *pMob1, *pMob2;
    object *pObj1, *pObj2;
    GFraMe_object *pWall1, *pWall2;
    
    // Get the actual object in both nodes
    qt_getRef(&pPl1, &pEv1, &pObj1, &pWall1, &pMob1, n1);
    qt_getRef(&pPl2, &pEv2, &pObj2, &pWall2, &pMob2, n2);
    
    // Collide it accordingly
    if      (pPl1   && pPl2  ) col_onPlayer (pPl1,  pPl2);
    else if (pPl1   && pEv2  ) col_onPlEv   (pPl1,  pEv2);
    else if (pPl1   && pObj2 ) col_onPlObj  (pPl1,  pObj2);
    else if (pPl1   && pMob2 ) col_onPlMob  (pPl1,  pMob2);
    else if (pPl1   && pWall2) col_onPlWall (pPl1,  pWall2);
    else if (pEv1   && pPl2  ) col_onPlEv   (pPl2,  pEv1);
    else if (pEv1   && pEv2  ) {}
    else if (pEv1   && pObj2 ) col_onEvObj  (pEv1,  pObj2);
    else if (pEv1   && pMob2 ) col_onEvMob  (pEv1,  pMob2);
    else if (pEv1   && pWall2) {}
    else if (pObj1  && pPl2  ) col_onPlObj  (pPl2,  pObj1);
    else if (pObj1  && pEv2  ) col_onEvObj  (pEv2,  pObj1);
    else if (pObj1  && pObj2 ) col_onObject (pObj1, pObj2);
    else if (pObj1  && pMob2 ) col_onObjMob (pObj1, pMob2);
    else if (pObj1  && pWall2) col_onObjWall(pObj1, pWall2);
    else if (pMob1  && pPl2  ) col_onPlMob  (pPl2,  pMob1);
    else if (pMob1  && pEv2  ) col_onEvMob  (pEv2,  pMob1);
    else if (pMob1  && pObj2 ) col_onObjMob (pObj2, pMob1);
    else if (pMob1  && pMob2 ) col_onMob    (pMob1, pMob2);
    else if (pMob1  && pWall2) col_onMobWall(pMob1, pWall2);
    else if (pWall1 && pPl2  ) col_onPlWall (pPl2,  pWall1);
    else if (pWall1 && pEv2  ) {}
    else if (pWall1 && pObj2 ) col_onObjWall(pObj2, pWall1);
    else if (pWall1 && pMob2 ) col_onMobWall(pMob2, pWall1);
    else if (pWall1 && pWall2) {}
    else                       GFraMe_log   ("Couldn't collide!");
}

/**
 * Collide two players
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void col_onPlayer(player *pPl1, player *pPl2) {
    GFraMe_object *pObj1, *pObj2;
    GFraMe_ret rv;
    int wasPl1Down, wasPl2Down, pl1Flags, pl2Flags;
    
    // Get both player objects
    player_getObject(&pObj1, pPl1);
    player_getObject(&pObj2, pPl2);
    
    // Get their collision flags
    pl1Flags = pObj1->hit;
    pl2Flags = pObj2->hit;
    // Clean their previous state
    pObj1->hit = 0;
    pObj2->hit = 0;
    // Check if they were already touching down
    wasPl1Down = (pl1Flags & GFraMe_direction_down);
    wasPl2Down = (pl2Flags & GFraMe_direction_down);
    // Try to collide them
    rv = GFraMe_object_overlap(pObj1, pObj2, GFraMe_dont_collide);
    if (rv == GFraMe_ret_ok) {
        // If any player wasn't touching down but is now, then it's
        // above the other one
        if (pObj1->y == pObj2->y) {} // Do nothing if they are side-by-side
        else if ((pl1Flags & GFraMe_direction_up)
            || (pl2Flags & GFraMe_direction_up)) {}
        else if (!wasPl1Down && (pObj1->hit&GFraMe_direction_down)){
            player_getCarried(pPl1, pObj2);
            pl1Flags |= GFraMe_direction_down;
        }
        else if (!wasPl2Down && (pObj2->hit&GFraMe_direction_down)){
            player_getCarried(pPl2, pObj1);
            pl2Flags |= GFraMe_direction_down;
        }
    }
    // Restore the collision flags
    pObj1->hit = pl1Flags;
    pObj2->hit = pl2Flags;
}

/**
 * Collide a player against an event
 * 
 * @param pPl The player
 * @param pEv The event
 */
void col_onPlEv(player *pPl, event *pEv) {
    GFraMe_sprite *pSpr;
    
    player_getSprite(&pSpr, pPl);
    event_check(pEv, pSpr);
}

/**
 * Collide a player against an object
 * 
 * @param pPl The player
 * @param pObj The object
 */
void col_onPlObj(player *pPl, object *pObj) {
    GFraMe_object *pGfmObj;
    
    player_getObject(&pGfmObj, pPl);
    obj_collide(pObj, pGfmObj);
}

/**
 * Collid a player against a mob
 * 
 * @param pPl The player
 * @param pMob The mob
 */
void col_onPlMob(player *pPl, mob *pMob) {
}

/**
 * Collide a player against a wall
 * 
 * @param pPl The player
 * @param pWall The wall
 */
void col_onPlWall(player *pPl, GFraMe_object *pWall) {
    GFraMe_object *pObj;
    
    player_getObject(&pObj, pPl);
    GFraMe_object_overlap(pWall, pObj, GFraMe_first_fixed);
}

/**
 * Collide an event against an object
 * 
 * @param pEv The event
 * @param pObj The object
 */
void col_onEvObj(event *pEv, object *pObj) {
    GFraMe_log("Ev X Obj collision not implemented");
}

/**
 * Collide an event against a mob
 * 
 * @param pEv The event
 * @param pMob The mob
 */
void col_onEvMob(event *pEv, mob *pMob) {
}

/**
 * Collide an object against an object
 * 
 * @param pObj1 A object
 * @param pObj2 A object
 */
void col_onObject(object *pObj1, object *pObj2) {
    GFraMe_log("Obj X Obj collision not implemented");
}

/**
 * Collide an object against a mob
 * 
 * @param pObj The object
 * @param pMob The mob
 */
void col_onObjMob(object *pObj, mob *pMob) {
}

/**
 * Collide an object against a wall
 * 
 * @param pObj The object
 * @param pWall The wall
 */
void col_onObjWall(object *pObj, GFraMe_object *pWall) {
    //GFraMe_log("Obj X Wall collision not implemented");
}

/**
 * Collide a mob against a wall
 * 
 * @param pMob The mob
 * @param pWall The wall
 */
void col_onMobWall(mob *pMob, GFraMe_object *pWall) {
    GFraMe_object *pObj;
    
    mob_getObject(&pObj, pMob);
    GFraMe_object_overlap(pWall, pObj, GFraMe_first_fixed);
}

/**
 * Collide two mobs
 * 
 * @param pMob1 A mob
 * @param pMob2 A mob
 */
void col_onMob(mob *pMob1, mob *pMob2) {
}

