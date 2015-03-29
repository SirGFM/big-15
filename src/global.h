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
#define NAME "JJAT"
#define FPS 60
#define GAME_UFPS 60
#define GAME_DFPS 60
#define GRAVITY 500
#define PL_VX 80
#define PL_JUMPS 180
#define PL_HIGHJUMPS 245
#define SAVEFILE "playstate.save"
#define CONFFILE "config.save"

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
extern GFraMe_spriteset *gl_sset4x4;   /** 4x4 pixels spriteset     */
extern GFraMe_spriteset *gl_sset8x8;   /** 8x8 pixels spriteset     */
extern GFraMe_spriteset *gl_sset8x16;  /** 8x16 pixels spriteset    */
extern GFraMe_spriteset *gl_sset8x32;  /** 8x16 pixels spriteset    */
extern GFraMe_spriteset *gl_sset16x16; /** 16x16 pixels spriteset   */
extern GFraMe_spriteset *gl_sset32x8;  /** 32x8 pixels spriteset    */
extern GFraMe_spriteset *gl_sset32x32; /** 32x32 pixels spriteset    */
extern GFraMe_spriteset *gl_sset64x8;  /** 64x8 pixels spriteset    */
extern GFraMe_spriteset *gl_sset64x16; /** 64x16 pixels spriteset    */
extern GFraMe_spriteset *gl_sset64x32; /** 64x32 pixels spriteset    */

#define AUD_DECLARE_EXT(AUD) \
  extern GFraMe_audio *gl_aud_##AUD

AUD_DECLARE_EXT(menuMove);
AUD_DECLARE_EXT(menuSelect);
AUD_DECLARE_EXT(text);
AUD_DECLARE_EXT(jump);
AUD_DECLARE_EXT(highjump);
AUD_DECLARE_EXT(door);
AUD_DECLARE_EXT(terminal);
AUD_DECLARE_EXT(getItem);
AUD_DECLARE_EXT(heartup);
AUD_DECLARE_EXT(switchItem);
AUD_DECLARE_EXT(shootEn);
AUD_DECLARE_EXT(blHit);
AUD_DECLARE_EXT(fall);
AUD_DECLARE_EXT(pause);
AUD_DECLARE_EXT(plHit);
AUD_DECLARE_EXT(jumperJump);
AUD_DECLARE_EXT(jumperFall);
AUD_DECLARE_EXT(charger);
AUD_DECLARE_EXT(teleport);
AUD_DECLARE_EXT(signaler);
AUD_DECLARE_EXT(shootBoss);
AUD_DECLARE_EXT(bombExpl);
AUD_DECLARE_EXT(bossExpl);
AUD_DECLARE_EXT(bossMove);
AUD_DECLARE_EXT(plDeath);
AUD_DECLARE_EXT(plStep);
AUD_DECLARE_EXT(menu);
AUD_DECLARE_EXT(intro);
AUD_DECLARE_EXT(movingOn);
AUD_DECLARE_EXT(victory);
AUD_DECLARE_EXT(bossBattle);
AUD_DECLARE_EXT(tensionGoesUp);

// Functions

GFraMe_ret gl_init();
void gl_clean();

#endif

