/**
 * @file src/mob.c
 * 
 * Mob structure and its functions
 */
#include <GFraMe/GFraMe_animation.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdio.h>

#include "global.h"
#include "mob.h"
#include "types.h"

struct stMob {
    GFraMe_sprite spr;       /** Mob's sprite (for rendering and collision)   */
    int health;              /** How many hitpoints this mob has              */
    int damage;              /** How much damage this mob does on the player  */
    flag weakness;           /** IDs that can do damage to this mob           */
    mobAnim anim;            /** The mob's current animation                  */
    /** Every possible animation, so it won't overlap another mob's */
    GFraMe_animation mob_anim[MOB_ANIM_MAX];
};

