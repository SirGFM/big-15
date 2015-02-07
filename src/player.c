/**
 * @file src/player.c
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_sprite.h>

#include "global.h"
#include "player.h"

#include <stdio.h>

struct stPlayer {
    GFraMe_sprite *spr;
};

/**
 * Initialize the player.
 * 
 * @param ppPl The player
 * @param firstTile Sprite's standing frame (first from its set)
 * @return GFraMe error code
 */
GFraMe_ret player_init(player **ppPl, int firstTile) {
    GFraMe_ret rv;
    player *pPl;
    
    // Initialize this so it can be cleaned on error
    pPl = NULL;
    
    // Sanitize parameters
    ASSERT(ppPl, GFraMe_ret_bad_param);
    ASSERT(!*ppPl, GFraMe_ret_bad_param);
    
    // Alloc the player
    pPl = (player*)malloc(sizeof(player));
    ASSERT(pPl, GFraMe_ret_memory_error);
    
    
    rv = GFraMe_ret_ok;
__ret:
    if (rv != GFraMe_ret_ok && pPl)
        free(pPl);
    
    return rv;
}

/**
 * Clean up the player
 * 
 * @param ppPl The player
 */
void player_clean(player **ppPl);

/**
 * Update the player. Also handle input (i.e., jump)
 * 
 * @param pPl The player
 * @param ms Time elapsed from last frame, in milliseconds
 */
void player_update(player *pPl, int ms);

/**
 * Draw the player to the screen.
 * 
 * @param pPl The player
 */
void player_draw(player *pPl);

