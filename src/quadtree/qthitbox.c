/**
 * @file src/quadtree/qthitbox.h
 * 
 * Simple hitbox
 */
#include "qthitbox.h"
#include "qtstatic.h"

#include "../camera.h"
#include "../global.h"

/**
 * Check if two hitboxes intersects
 * 
 * @param pHb1 A hitbox
 * @param pHb2 A hitbox
 * @return 1 = True, 0 = False
 */
int qtHbIntersect(qtHitbox *pHb1, qtHitbox *pHb2) {
    int maxh, maxw, rv;
    
    // Check horizontal intersection
    maxw = pHb1->hw + pHb2->hw;
    if (pHb1->cx > pHb2->cx)
        ASSERT(pHb1->cx - pHb2->cx <= maxw, 0);
    else
        ASSERT(pHb2->cx - pHb1->cx <= maxw, 0);
    // Check vertical intersection
    maxh = pHb1->hh + pHb2->hh;
    if (pHb1->cy > pHb2->cy)
        ASSERT(pHb1->cy - pHb2->cy <= maxh, 0);
    else
        ASSERT(pHb2->cy - pHb1->cy <= maxh, 0);
    
    rv = 1;
__ret:
    return rv;
}

#ifdef QT_DEBUG_DRAW
#include <SDL2/SDL.h>
/**
 * Context where the bounding box shall be rendered; debug-mode only
 */
extern SDL_Renderer *GFraMe_renderer;

/**
 * Draw a hitbox's bounding box
 * 
 * @param pHb The hitbox
 * @param r Red color component
 * @param g Green color component
 * @param b Blue color component
 */
void qt_drawHitboxDebug(qtHitbox *pHb, int r, int g, int b) {
    // Create a SDL_Rect at its position
    SDL_Rect dbg_rect;
    dbg_rect.x = pHb->cx - pHb->hw - cam_x;
    dbg_rect.y = pHb->cy - pHb->hh - cam_y;
    dbg_rect.w = pHb->hw * 2;
    dbg_rect.h = pHb->hh * 2;
    
    // Render it to the screen, in green
    SDL_SetRenderDrawColor(GFraMe_renderer, r, g, b, 0xff);
    SDL_RenderDrawRect(GFraMe_renderer, &dbg_rect);
}

#endif

