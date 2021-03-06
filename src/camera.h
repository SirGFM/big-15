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

/**
 * Try to center the camera on both players
 * It'll follow whichever player is to the right and/or above
 * 
 * @param pPl1 A player
 * @param pPl2 A player
 */
void cam_setPositionSt(player *pPl1, player *pPl2);

/**
 * Try to center the camera on both players
 * It'll follow whichever player is to the right and/or above
 */
void cam_setPosition();

/**
 * Set the maximum world space for the camera
 * 
 * int w Maximum width
 * int h Maximum height
 */
void cam_setMapDimension(int w, int h);

#endif

