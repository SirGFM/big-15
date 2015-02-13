/**
 * @file src/camera.h
 * 
 * Lazy, static camera module
 */
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "player.h"

/** Current camera's horizontal position */
extern int cam_x;
/** Current camera's vertical position */
extern int cam_y;
/** Current map's width */
extern int cam_map_w;
/** Current map's height */
extern int cam_map_h;

/**
 * Try to center the camera on both players
 * It'll follow whichever player is to the right and/or above
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void cam_setPosition(player *pPl1, player *pPl2);

/**
 * Set the maximum world space for the camera
 * 
 * int w Maximum width
 * int h Maximum height
 */
void cam_setMapDimension(int w, int h);

#endif

