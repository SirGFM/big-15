/**
 * @file src/controller.h
 * 
 * Controller abstraction so it doesn't matter if it's the player 1, 2 or if
 * they are playing on a controller or keyboard
 */
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_keys.h>

#include "controller.h"
#include "global.h"
#include "types.h"

#if defined(EMCC)
/** Overrides the library's Numpad Enter. Manually set on the playstate. */
int emcc_numenter = 0;
#endif /* defined(EMCC) */

/** Define which control scheme is currently being used */
static ctr_mode _ctr_pl1 = CTR_PAD1_C;
static ctr_mode _ctr_pl2 = CTR_PAD1_D;

/**
 * Set default control mode for both players
 */
void ctr_setDef() {
    if (GFraMe_controller_max > 0) {
        _ctr_pl1 = CTR_PAD1_C;
        _ctr_pl2 = CTR_PAD1_D;
    }
    else {
        _ctr_pl1 = CTR_KEYS_A;
        _ctr_pl2 = CTR_KEYS_C;
    }
}

/**
 * Read both players input mode
 * 
 * @param pl1 Player1 mode
 * @param pl2 Player2 mode
 */
void ctr_getModes(int *pl1, int *pl2) {
    *pl1 = _ctr_pl1;
    *pl2 = _ctr_pl2;
}


/**
 * Change the current input scheme
 * 
 * @param ID ID of the player being set
 * @param mode The new controle mode
 * @param Whether the mode was switched successfully
 */
int ctr_setMode(int ID, ctr_mode mode) {
    int ret;
    
    ret = 0;
    // Check that the mode is valid
    ASSERT_NR(ID == ID_PL1 || ID == ID_PL2);
    ASSERT_NR(ID != ID_PL1 || _ctr_pl2 != mode);
    ASSERT_NR(ID != ID_PL2 || _ctr_pl1 != mode);
    ASSERT_NR(mode < CTR_PAD1_A || GFraMe_controller_max > 0);
    ASSERT_NR(mode < CTR_PAD2_A || GFraMe_controller_max > 1);
    
    if (ID == ID_PL1)
        _ctr_pl1 = mode;
    else
        _ctr_pl2 = mode;
    
    ret = 1;
__ret:
    return ret;
}

/**
 * Change the current input scheme, ignoring if it clashes with the other
 * player (but checking for controller presence)
 * 
 * @param ID ID of the player being set
 * @param mode The new controle mode
 * @param Whether the mode was switched successfully
 */
int ctr_setModeForce(int ID, ctr_mode mode) {
    int ret;
    
    ret = 0;
    // Check that the mode is valid
    ASSERT_NR(ID == ID_PL1 || ID == ID_PL2);
    ASSERT_NR(mode < CTR_PAD1_A || GFraMe_controller_max > 0);
    ASSERT_NR(mode < CTR_PAD2_A || GFraMe_controller_max > 1);
    
    if (ID == ID_PL1)
        _ctr_pl1 = mode;
    else
        _ctr_pl2 = mode;
    
    ret = 1;
__ret:
    return ret;
}

/**
 * Checks if the left button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_left(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.a;
        case CTR_KEYS_B: return GFraMe_keys.left;
        case CTR_KEYS_C: return GFraMe_keys.n4;
        case CTR_KEYS_D: return GFraMe_keys.h;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].left;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].lx < -0.35;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].lx < -0.35;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].rx < -0.35;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].left;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].lx < -0.35;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].lx < -0.35;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].rx < -0.35;
        default: return 0;
    }
}

/**
 * Checks if the right button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_right(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.d;
        case CTR_KEYS_B: return GFraMe_keys.right;
        case CTR_KEYS_C: return GFraMe_keys.n6;
        case CTR_KEYS_D: return GFraMe_keys.k;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].right;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].lx > 0.35;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].lx > 0.35;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].rx > 0.35;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].right;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].lx > 0.35;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].lx > 0.35;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].rx > 0.35;
        default: return 0;
    }
}


/**
 * Checks if the action button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_action(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.w;
        case CTR_KEYS_B: return GFraMe_keys.up;
        case CTR_KEYS_C: return GFraMe_keys.n8;
        case CTR_KEYS_D: return GFraMe_keys.u;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].up;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].ly < -0.5;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].ly < -0.5;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].ry < -0.5;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].up;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].ly < -0.5;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].ly < -0.5;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].ry < -0.5;
        default: return 0;
    }
}


/**
 * Checks if the jump button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_jump(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.space;
        case CTR_KEYS_B: return GFraMe_keys.x;
        case CTR_KEYS_C: return GFraMe_keys.n0;
        case CTR_KEYS_D: return GFraMe_keys.l;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].a;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].a;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].l1;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].r1;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].a;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].a;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].l1;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].r1;
        default: return 0;
    }
}


/**
 * Checks if the item button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_item(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.lshift;
        case CTR_KEYS_B: return GFraMe_keys.c;
#if defined(EMCC)
        case CTR_KEYS_C: return emcc_numenter;
#else
        case CTR_KEYS_C: return GFraMe_keys.nenter;
#endif /* defined(EMCC) */
        case CTR_KEYS_D: return GFraMe_keys.g;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].b;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].b;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].l2;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].r2;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].b;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].b;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].l2;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].r2;
        default: return 0;
    }
}


/**
 * Checks if the switchItem button is pressed
 * 
 * @param ID ID of the player checking the button
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_switchItem(int ID) {
    int mode;
    
    if (ID == ID_PL1)
        mode = _ctr_pl1;
    else if (ID == ID_PL2)
        mode = _ctr_pl2;
    else
        return 0;
    
    switch (mode) {
        case CTR_KEYS_A: return GFraMe_keys.tab;
        case CTR_KEYS_B: return GFraMe_keys.v;
        case CTR_KEYS_C: return GFraMe_keys.nperiod;
        case CTR_KEYS_D: return GFraMe_keys.i;
        case CTR_PAD1_A: return GFraMe_controller_max > 0 && GFraMe_controllers[0].x;
        case CTR_PAD1_B: return GFraMe_controller_max > 0 && GFraMe_controllers[0].x;
        case CTR_PAD1_C: return GFraMe_controller_max > 0 && GFraMe_controllers[0].left;
        case CTR_PAD1_D: return GFraMe_controller_max > 0 && GFraMe_controllers[0].x;
        case CTR_PAD2_A: return GFraMe_controller_max > 1 && GFraMe_controllers[1].x;
        case CTR_PAD2_B: return GFraMe_controller_max > 1 && GFraMe_controllers[1].x;
        case CTR_PAD2_C: return GFraMe_controller_max > 1 && GFraMe_controllers[1].left;
        case CTR_PAD2_D: return GFraMe_controller_max > 1 && GFraMe_controllers[1].x;
        default: return 0;
    }
}

/**
 * Checks if the pause button is pressed
 * 
 * @return 1 if the button is pressed, 0 otherwise
 */
int ctr_pause() {
    int ret;
    
    ret = 0;
    ret = ret || GFraMe_keys.esc;
    ret = ret || GFraMe_keys.p;
    ret = ret || GFraMe_keys.enter;
    if (GFraMe_controller_max > 0)
        ret = ret || GFraMe_controllers[0].start;
    if (GFraMe_controller_max > 1)
        ret = ret || GFraMe_controllers[1].start;
    
    return ret;
}

