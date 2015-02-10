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

/** Define which control scheme is currently being used */
static ctr_mode _ctr_mode = CTR_1CTR;

/**
 * Checks if the left button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_left(int ID) {
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.a || GFraMe_keys.h;
            case ID_PL2: return GFraMe_keys.left || GFraMe_keys.n4;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].left
                            || GFraMe_controllers[0].lx < -0.35;
            case ID_PL2: return GFraMe_keys.a || GFraMe_keys.left;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].left
                            || GFraMe_controllers[0].lx < -0.35;
            case ID_PL2: return GFraMe_controllers[0].x
                            || GFraMe_controllers[0].rx < -0.35;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].left
                            || GFraMe_controllers[0].lx < -0.35;
            case ID_PL2: return GFraMe_controllers[1].left
                            || GFraMe_controllers[1].lx < -0.35;
            default: return 0;
          }
      } break;
      default: return 0;
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
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.d || GFraMe_keys.k;
            case ID_PL2: return GFraMe_keys.right || GFraMe_keys.n6;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].right
                            || GFraMe_controllers[0].lx > 0.35;
            case ID_PL2: return GFraMe_keys.d || GFraMe_keys.right;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].right
                            || GFraMe_controllers[0].lx > 0.35;
            case ID_PL2: return GFraMe_controllers[0].x
                            || GFraMe_controllers[0].rx > 0.35;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].right
                            || GFraMe_controllers[0].lx > 0.35;
            case ID_PL2: return GFraMe_controllers[1].left
                            || GFraMe_controllers[1].lx > 0.35;
            default: return 0;
          }
      } break;
      default: return 0;
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
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.w || GFraMe_keys.u;
            case ID_PL2: return GFraMe_keys.up || GFraMe_keys.n8;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].up
                            || GFraMe_controllers[0].ly < -0.5;
            case ID_PL2: return GFraMe_keys.w || GFraMe_keys.up;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].up
                            || GFraMe_controllers[0].ly < -0.5;
            case ID_PL2: return GFraMe_controllers[0].y
                            || GFraMe_controllers[0].ry < -0.5;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].up
                            || GFraMe_controllers[0].ly < -0.5;
            case ID_PL2: return GFraMe_controllers[1].up
                            || GFraMe_controllers[1].ly < -0.5;
            default: return 0;
          }
      } break;
      default: return 0;
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
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.z || GFraMe_keys.b;
            case ID_PL2: return GFraMe_keys.delete || GFraMe_keys.n1;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].a;
            case ID_PL2: return GFraMe_keys.z || GFraMe_keys.h;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].l1;
            case ID_PL2: return GFraMe_controllers[0].r1;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].a;
            case ID_PL2: return GFraMe_controllers[1].a;
            default: return 0;
          }
      } break;
      default: return 0;
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
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.x || GFraMe_keys.n;
            case ID_PL2: return GFraMe_keys.end || GFraMe_keys.end;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].b;
            case ID_PL2: return GFraMe_keys.x || GFraMe_keys.j;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].l2;
            case ID_PL2: return GFraMe_controllers[0].r2;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].b;
            case ID_PL2: return GFraMe_controllers[1].b;
            default: return 0;
          }
      } break;
      default: return 0;
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
    switch (_ctr_mode) {
      case CTR_KEYS: {
          switch (ID) {
            case ID_PL1: return GFraMe_keys.c || GFraMe_keys.m;
            case ID_PL2: return GFraMe_keys.pgdown || GFraMe_keys.n3;
            default: return 0;
          }
      } break;
      case CTR_BOTH: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].x;
            case ID_PL2: return GFraMe_keys.c || GFraMe_keys.k;
            default: return 0;
          }
      } break;
      case CTR_1CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].select;
            case ID_PL2: return GFraMe_controllers[0].start;
            default: return 0;
          }
      } break;
      case CTR_2CTR: {
          switch (ID) {
            case ID_PL1: return GFraMe_controllers[0].x;
            case ID_PL2: return GFraMe_controllers[1].x;
            default: return 0;
          }
      } break;
      default: return 0;
    }
    return 0;
}

