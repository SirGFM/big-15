/**
 * @file src/camera.h
 * 
 * Lazy, static camera module
 */
#include "camera.h"
#include "global.h"
#include "player.h"

#define CAM_DEAD_X 80
#define CAM_DEAD_Y 32
#define CAM_DEAD_W 160
#define CAM_DEAD_H 112

/** Current camera's horizontal position */
int cam_x = 0;
/** Current camera's vertical position */
int cam_y = 0;
/** Current map's width */
static int cam_map_w;
/** Current map's height */
static int cam_map_h;
/** Players' last horizontal center position */
static int last_x = 0;

/**
 * Try to center the camera on both players
 * It'll follow whichever player is to the right and/or above
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void cam_setPosition(player *pPl1, player *pPl2) {
    int cx, cy, n, x1, x2, y1, y2;
    
    // Get the point between both players
    player_getCenter(&x1, &y1, pPl1);
    player_getCenter(&x2, &y2, pPl2);
    
    n = 4;
    if (y1 == y2) {
        cx = (x1 + x2) / 2;
        cy = (y1 + y2) / 2;
    }
    else if (y1 > y2) {
        cx = (x1 + x2 * (n - 1)) / n;
        cy = (y1 + y2 * (n - 1)) / n;
    }
    else {
        cx = (x2 + x1 * (n - 1)) / n;
        cy = (y2 + y1 * (n - 1)) / n;
    }
    
    // Make sure this point is inside the horizontal deadzone
    if (cx < last_x && cx - cam_x < CAM_DEAD_X)
        cam_x = cx - CAM_DEAD_X;
    else if (cx > last_x && cx - cam_x > CAM_DEAD_X + CAM_DEAD_W)
        cam_x = cx - CAM_DEAD_X - CAM_DEAD_W;
    // Bound it to the world space
    if (cam_x < 0)
        cam_x = 0;
    else if (cam_x + SCR_W > cam_map_w)
        cam_x = cam_map_w - SCR_W;
    
    // Make sure this point is inside the vertical deadzone
    if (cy - cam_y < CAM_DEAD_Y)
        cam_y = cy - CAM_DEAD_Y;
    else if (cy > CAM_DEAD_Y + CAM_DEAD_H)
        cam_y = cy - CAM_DEAD_Y - CAM_DEAD_H;
    // Bound it to the world space
    if (cam_y < 0)
        cam_y = 0;
    else if (cam_y + SCR_H > cam_map_h)
        cam_y = cam_map_h - SCR_H;
    
    // Store the last position to detect the players' direction
    last_x = cx;
}

/**
 * Set the maximum world space for the camera
 * 
 * int w Maximum width
 * int h Maximum height
 */
void cam_setMapDimension(int w, int h) {
    cam_map_w = w;
    cam_map_h = h;
}

