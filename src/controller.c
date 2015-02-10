/**
 * @file src/controller.h
 * 
 * Controller abstraction so it doesn't matter if it's the player 1, 2 or if
 * they are playing on a controller or keyboard
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_keys.h>

#include "controller.h"
#include "types.h"

/**
 * Checks if the left button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_left(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].left
                    || GFraMe_controllers[0].lx < -0.35;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].left
                    || GFraMe_controllers[0].lx < -0.35;
            else
                return GFraMe_keys.a;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].x
                    || GFraMe_controllers[0].rx < -0.35;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].left
                    || GFraMe_controllers[1].lx < -0.35;
            else
                return GFraMe_keys.left;
        } break;
        default: {}
    }
    return 0;
}

/**
 * Checks if the right button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_right(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].right
                    || GFraMe_controllers[0].lx > 0.35;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].right
                    || GFraMe_controllers[0].lx > 0.35;
            else
                return GFraMe_keys.d;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].b
                    || GFraMe_controllers[0].rx > 0.35;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].right
                    || GFraMe_controllers[1].lx > 0.35;
            else
                return GFraMe_keys.right;
        } break;
        default: {}
    }
    return 0;
}


/**
 * Checks if the action button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_action(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].up
                    || GFraMe_controllers[0].ly < -0.5;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].up
                    || GFraMe_controllers[0].ly < -0.5;
            else
                return GFraMe_keys.w;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].y
                    || GFraMe_controllers[0].ry < -0.5;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].up
                    || GFraMe_controllers[1].ly < -0.5;
            else
                return GFraMe_keys.up;
        } break;
        default: {}
    }
    return 0;
}


/**
 * Checks if the jump button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_jump(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].l1;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].a;
            else
                return GFraMe_keys.space;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].r1;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].a;
            else
                return GFraMe_keys.n3;
        } break;
        default: {}
    }
    return 0;
}


/**
 * Checks if the item button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_item(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].l2;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].b;
            else
                return GFraMe_keys.lctrl;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].r2;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].b;
            else
                return GFraMe_keys.n2;
        } break;
        default: {}
    }
    return 0;
}


/**
 * Checks if the switchItem button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_switchItem(int ID) {
    switch (ID) {
        case ID_PL1: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].select;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[0].x;
            else
                return GFraMe_keys.lshift;
        } break;
        case ID_PL2: {
            if (GFraMe_controller_max == 1)
                return GFraMe_controllers[0].start;
            else if (GFraMe_controller_max > 1)
                return GFraMe_controllers[1].x;
            else
                return GFraMe_keys.n1;
        } break;
        default: {}
    }
    return 0;
}

