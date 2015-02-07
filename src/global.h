/**
 * @file src/global.h
 * 
 * Global assets module
 */
#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>

// Game initialization constants
#define WND_W 640
#define WND_H 480
#define SCR_W 320
#define SCR_H 240
#define ORG "com.gfmgamecorner"
#define NAME "game"
#define FPS 60
#define GAME_UFPS 60
#define GAME_DFPS 60

#define ASSERT(stmt, err) \
  do { \
    if (!(stmt)) { \
      rv = err; \
      goto __ret; \
    } \
  } while (0)

#define ASSERT_NR(stmt) \
  do { \
    if (!(stmt)) { \
      goto __ret; \
    } \
  } while (0)


// Global variables
extern int gl_running;                 /** Flag the game as running */
extern GFraMe_spriteset *gl_sset8x8;   /** 8x8 pixels spriteset     */
extern GFraMe_spriteset *gl_sset8x16;  /** 8x16 pixels spriteset    */
extern GFraMe_spriteset *gl_sset16x16; /** 16x16 pixels spriteset   */

// Functions

GFraMe_ret gl_init();
void gl_clean();

#endif

