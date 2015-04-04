/**
 * @file src/collision.h
 * 
 * Handler for collisions
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>

#include "bullet.h"
#include "collision.h"
#include "controller.h"
#include "event.h"
#include "global.h"
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
    bullet *pBul1, *pBul2;
    event *pEv1, *pEv2;
    GFraMe_object *pWall1, *pWall2;
    mob *pMob1, *pMob2;
    object *pObj1, *pObj2;
    player *pPl1, *pPl2;
    
    // Get the actual object in both nodes
    qt_getRef(&pPl1, &pEv1, &pObj1, &pWall1, &pMob1, &pBul1, n1);
    qt_getRef(&pPl2, &pEv2, &pObj2, &pWall2, &pMob2, &pBul2, n2);
    
    // Collide it accordingly
    if      (pPl1   && pPl2  ) {}
    else if (pPl1   && pEv2  ) col_onPlEv   (pPl1,  pEv2);
    else if (pPl1   && pObj2 ) col_onPlObj  (pPl1,  pObj2);
    else if (pPl1   && pMob2 ) col_onPlMob  (pPl1,  pMob2);
    else if (pPl1   && pWall2) col_onPlWall (pPl1,  pWall2);
    else if (pPl1   && pBul2)  col_onPlBul  (pPl1,  pBul2);
    else if (pEv1   && pPl2  ) col_onPlEv   (pPl2,  pEv1);
    else if (pEv1   && pEv2  ) {}
    else if (pEv1   && pObj2 ) col_onEvObj  (pEv1,  pObj2);
    else if (pEv1   && pMob2 ) col_onEvMob  (pEv1,  pMob2);
    else if (pEv1   && pWall2) {}
    else if (pEv1   && pBul2 ) col_onEvBul  (pEv1,  pBul2);
    else if (pObj1  && pPl2  ) col_onPlObj  (pPl2,  pObj1);
    else if (pObj1  && pEv2  ) col_onEvObj  (pEv2,  pObj1);
    else if (pObj1  && pObj2 ) col_onObject (pObj1, pObj2);
    else if (pObj1  && pMob2 ) col_onObjMob (pObj1, pMob2);
    else if (pObj1  && pWall2) col_onObjWall(pObj1, pWall2);
    else if (pObj1  && pBul2)  col_onObjBul (pObj1, pBul2);
    else if (pMob1  && pPl2  ) col_onPlMob  (pPl2,  pMob1);
    else if (pMob1  && pEv2  ) col_onEvMob  (pEv2,  pMob1);
    else if (pMob1  && pObj2 ) col_onObjMob (pObj2, pMob1);
    else if (pMob1  && pMob2 ) col_onMob    (pMob1, pMob2);
    else if (pMob1  && pWall2) col_onMobWall(pMob1, pWall2);
    else if (pMob1  && pBul2)  col_onMobBul (pMob1, pBul2);
    else if (pWall1 && pPl2  ) col_onPlWall (pPl2,  pWall1);
    else if (pWall1 && pEv2  ) {}
    else if (pWall1 && pObj2 ) col_onObjWall(pObj2, pWall1);
    else if (pWall1 && pMob2 ) col_onMobWall(pMob2, pWall1);
    else if (pWall1 && pWall2) {}
    else if (pWall1 && pBul2 ) col_onBulWall(pBul2, pWall1);
    else if (pBul1  && pPl2  ) col_onPlBul  (pPl2,  pBul1);
    else if (pBul1  && pEv2  ) col_onEvBul  (pEv2,  pBul1);
    else if (pBul1  && pObj2 ) col_onObjBul (pObj2, pBul1);
    else if (pBul1  && pMob2 ) col_onMobBul (pMob2, pBul1);
    else if (pBul1  && pWall2) col_onBulWall(pBul1, pWall2);
    else if (pBul1  && pBul2 ) col_onBul    (pBul1, pBul2);
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
    int wasPl1Down, pl1Flags, pl2Flags;
//    int wasPl2Down;
    
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
    // Try to collide them
    rv = GFraMe_object_overlap(pObj1, pObj2, GFraMe_dont_collide);
    if (rv == GFraMe_ret_ok) {
        // Check if they are changing itens
        player_changeItem(pPl1);
        player_changeItem(pPl2);
        // If any player wasn't touching down but is now, then it's
        // above the other one
        if (pObj1->y == pObj2->y) {} // Do nothing if they are side-by-side
        else if ((pl1Flags & GFraMe_direction_up)
            || (pl2Flags & GFraMe_direction_up)) {}
        else if (!wasPl1Down && (pObj1->hit&GFraMe_direction_down)){
            GFraMe_object_overlap(pObj1, pObj2, GFraMe_second_fixed);
            player_getCarried(pPl1, pPl2);
            pl1Flags |= GFraMe_direction_down;
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
    
    // Removed damage boost through objects
    // ASSERT_NR(!player_isHurt(pPl));
    
    player_getObject(&pGfmObj, pPl);
    obj_collide(pObj, pGfmObj);
//__ret:
//    return;
}

/**
 * Collid a player against a mob
 * 
 * @param pPl The player
 * @param pMob The mob
 */
void col_onPlMob(player *pPl, mob *pMob) {
    GFraMe_object *pObj1, *pObj2;
    GFraMe_ret rv;
    int ID;
    
    ASSERT_NR(mob_isAlive(pMob));
    ASSERT_NR(!player_isHurt(pPl));
    
    // Get both player objects
    player_getObject(&pObj1, pPl);
    mob_getObject(&pObj2, pMob);
    
    // Try to collide them
    ID = mob_getID(pMob);
    if (ID != ID_BOSS_PLAT && ID != ID_BOMB) {
        int plFlags;
        
        // Get their collision flags
        plFlags = pObj1->hit;
        // Clean their previous state
        pObj1->hit = 0;
        
        rv = GFraMe_object_overlap(pObj1, pObj2, GFraMe_dont_collide);
        if (rv == GFraMe_ret_ok) {
            player_hurt(pPl, mob_getDamage(pMob), pObj1->hit);
        }
        // Restore the collision flags
        pObj1->hit = plFlags;
    }
    else if (ID == ID_BOSS_PLAT) {
        // Make the player stand on the platform
        GFraMe_object_overlap(pObj1, pObj2, GFraMe_second_fixed);
        player_getCarriedBoss(pPl, pObj2);
    }
    else if (ID == ID_BOMB) {
        // Push the bomb
        GFraMe_object_overlap(pObj2, pObj1, GFraMe_second_fixed);
    }
    
__ret:
    return;
}

/**
 * Collide a player against a wall
 * 
 * @param pPl The player
 * @param pWall The wall
 */
void col_onPlWall(player *pPl, GFraMe_object *pWall) {
    GFraMe_object *pObj;
    
    if (player_isAlive(pPl)) {
        player_getObject(&pObj, pPl);
        GFraMe_object_overlap(pWall, pObj, GFraMe_first_fixed);
    }
}

/**
 * Collide an event against an object
 * 
 * @param pEv The event
 * @param pObj The object
 */
void col_onEvObj(event *pEv, object *pObj) {
    // GFraMe_log("Ev X Obj collision not implemented");
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
    
    ASSERT_NR(mob_getID(pMob) != ID_EYE);
    ASSERT_NR(mob_getID(pMob) != ID_EYE_LEFT);
    ASSERT_NR(mob_getID(pMob) != ID_PHANTOM);
    
    mob_getObject(&pObj, pMob);
    GFraMe_object_overlap(pWall, pObj, GFraMe_first_fixed);
    
__ret:
    return;
}

/**
 * Collide two mobs
 * 
 * @param pMob1 A mob
 * @param pMob2 A mob
 */
void col_onMob(mob *pMob1, mob *pMob2) {
    GFraMe_object *pObj1, *pObj2;
    
    ASSERT_NR(mob_isAlive(pMob1));
    ASSERT_NR(mob_isAlive(pMob2));
    
    mob_getObject(&pObj1, pMob1);
    mob_getObject(&pObj2, pMob2);
    
    if (mob_getID(pMob1) == ID_BOMB) {
//        int tmp;
        
        // Push the bomb
//        tmp = pObj2->hit;
        GFraMe_object_overlap(pObj1, pObj2, GFraMe_second_fixed);
//        pObj2->hit = tmp;
    }
    else if (mob_getID(pMob2) == ID_BOMB) {
//        int tmp;
        
        // Push the bomb
//        tmp = pObj1->hit;
        GFraMe_object_overlap(pObj2, pObj1, GFraMe_second_fixed);
//        pObj1->hit = tmp;
    }
    
__ret:
    return;
}

/**
 * Collide a player against a bullet
 * 
 * @param pPl The player
 * @param pBul The bullet
 */
void col_onPlBul(player *pPl, bullet *pBul) {
    flag id;
    GFraMe_object *pObj1, *pObj2;
    GFraMe_ret rv;
    int plFlags;
    
    ASSERT_NR(bullet_isAlive(pBul));
    bullet_getID(&id, pBul) ;
    ASSERT_NR((id & ID_PL) == 0);
    
    // Get both player objects
    player_getObject(&pObj1, pPl);
    bullet_getObject(&pObj2, pBul);
    
    // Get their collision flags
    plFlags = pObj1->hit;
    // Clean their previous state
    pObj1->hit = 0;
    // Try to collide them
    rv = GFraMe_object_overlap(pObj1, pObj2, GFraMe_dont_collide);
    if (rv == GFraMe_ret_ok) {
        player_hurt(pPl, 1, pObj1->hit);
        bullet_explode(pBul);
    }
    // Restore the collision flags
    pObj1->hit = plFlags;
    
__ret:
    return;
}

/**
 * Collide an event against a bullet
 * 
 * @param pEv The event
 * @param pBul The bullet
 */
void col_onEvBul(event *pEv, bullet *pBul) {
}

/**
 * Collide an object against a bullet
 * 
 * @param pObj The object
 * @param pBul The bullet
 */
void col_onObjBul(object *pObj, bullet *pBul) {
    GFraMe_object *pObj1, *pObj2;
    GFraMe_ret rv;
    int ID;
    objAnim anim;
    
    // Check if it's a door
    obj_getID(&ID, pObj);
    ASSERT_NR((ID & ID_DOOR) == ID_DOOR 
        || (ID & ID_DOOR_HOR) == ID_DOOR_HOR);
    // Check that the door is closed
    anim = obj_getAnim(pObj);
    ASSERT_NR((anim != OBJ_ANIM_DOOR_OPEN)
        && (anim != OBJ_ANIM_DOOR_HOR_OPEN));
    
    // Get both objects
    obj_getObject(&pObj1, pObj);
    bullet_getObject(&pObj2, pBul);
    
    // Try to collide them
    rv = GFraMe_object_overlap(pObj1, pObj2, GFraMe_dont_collide);
    if (rv == GFraMe_ret_ok) {
        bullet_explode(pBul);
    }
    
__ret:
    return;
}

/**
 * Collide a mob against a bullet
 * 
 * @param pMob The mob
 * @param pBul The wall
 */
void col_onMobBul(mob *pMob, bullet *pBul) {
    flag ID;
    
    // Check that the bullet is actually alive
    ASSERT_NR(bullet_isAlive(pBul));
    // Mob is only 'hurtable' by explosion
    bullet_getID(&ID, pBul);
    ASSERT_NR(ID == ID_EXPLPROJ);
    
    mob_hit(pMob, 1, ID);
__ret:
    return;
}

/**
 * Collide a bullet against a wall
 * 
 * @param pWall The wall
 * @param pBul The bullet
 */
void col_onBulWall(bullet *pBul, GFraMe_object *pWall) {
    flag ID;
    GFraMe_object *pObj;
    GFraMe_ret rv;
    
    ASSERT_NR(bullet_isAlive(pBul));
    
    bullet_getID(&ID, pBul);
    ASSERT_NR(ID != ID_EXPLPROJ);
    
    bullet_getObject(&pObj, pBul);
    rv = GFraMe_object_overlap(pWall, pObj, GFraMe_first_fixed);
    ASSERT_NR(rv == GFraMe_ret_ok);
    bullet_explode(pBul);
__ret:
    return;
}

/**
 * Collide two bullets
 * 
 * @param pBul1 The bullet
 * @param pBul2 The bullet
 */
void col_onBul(bullet *pBul1, bullet *pBul2) {
}

