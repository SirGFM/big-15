/**
 * @file src/camera.h
 * 
 * Lazy, static camera module
 */
#include "camera.h"
#include "player.h"

#define CAM_DEAD_X 64
#define CAM_DEAD_Y 48
#define CAM_DEAD_W 192
#define CAM_DEAD_H 144

/** Current camera's horizontal position */
int cam_x = 0;
/** Current camera's vertical position */
int cam_y = 0;
/** Current map's width */
int cam_map_w;
/** Current map's height */
int cam_map_h;

/**
 * Try to center the camera on both players
 * It'll follow whichever player is to the right and/or above
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void cam_setPosition(player *pPl1, player *pPl2) {
    int x1, x2, y1, y2;
    
    player_getCenter(&x1, &y1, pPl1);
    player_getCenter(&x2, &y2, pPl2);
    
    
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

