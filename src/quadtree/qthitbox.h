/**
 * @file src/quadtree/qthitbox.h
 * 
 * Simple hitbox
 */
#ifndef __QT_HITBOX_H_
#define __QT_HITBOX_H_

typedef struct stQTHitbox qtHitbox;

/**
 * Check if two hitboxes intersects
 * 
 * @param pHb1 A hitbox
 * @param pHb2 A hitbox
 * @return 1 = True, 0 = False
 */
int qtHbIntersect(qtHitbox *pHb1, qtHitbox *pHb2);

#  ifdef QT_DEBUG_DRAW
/**
 * Draw a hitbox's bounding box
 * 
 * @param pHb The hitbox
 * @param r Red color component
 * @param g Green color component
 * @param b Blue color component
 * @param a Alpha color component
 */
void qt_drawHitboxDebug(qtHitbox *pHb, int r, int g, int b, int a);
#  endif

#endif

