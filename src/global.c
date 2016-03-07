/**
 * @file src/global.c
 * 
 * Global assets module
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>
#include <GFraMe/GFraMe_texture.h>

#include <stdlib.h>

#include "global.h"

/**
 * Declare both the external variable and the static spriteset
 */
#define DECLARE_SSET(W, H) \
  GFraMe_spriteset *gl_sset##W##x##H; \
  static GFraMe_spriteset _glSset##W##x##H

#define DECLARE_AUDIO(AUD) \
  static GFraMe_audio  _glAud_##AUD; \
  GFraMe_audio * gl_aud_##AUD

static int gl_isInit = 0;
int gl_lang;
int gl_running;

GFraMe_texture gl_tex;
DECLARE_SSET(4 , 4 );
DECLARE_SSET(8 , 8 );
DECLARE_SSET(8 , 16);
DECLARE_SSET(8 , 32);
DECLARE_SSET(16, 16);
DECLARE_SSET(32, 8 );
DECLARE_SSET(32, 32);
DECLARE_SSET(64, 8);
DECLARE_SSET(64, 16);
DECLARE_SSET(64, 32);

DECLARE_AUDIO(menuMove);
DECLARE_AUDIO(menuSelect);
DECLARE_AUDIO(text);
DECLARE_AUDIO(jump);
DECLARE_AUDIO(highjump);
DECLARE_AUDIO(door);
DECLARE_AUDIO(terminal);
DECLARE_AUDIO(getItem);
DECLARE_AUDIO(heartup);
DECLARE_AUDIO(switchItem);
DECLARE_AUDIO(shootEn);
DECLARE_AUDIO(blHit);
DECLARE_AUDIO(fall);
DECLARE_AUDIO(pause);
DECLARE_AUDIO(plHit);
DECLARE_AUDIO(jumperJump);
DECLARE_AUDIO(jumperFall);
DECLARE_AUDIO(charger);
DECLARE_AUDIO(teleport);
DECLARE_AUDIO(signaler);
DECLARE_AUDIO(shootBoss);
DECLARE_AUDIO(bombExpl);
DECLARE_AUDIO(bossExpl);
DECLARE_AUDIO(bossMove);
DECLARE_AUDIO(plDeath);
DECLARE_AUDIO(plStep);
DECLARE_AUDIO(menu);
DECLARE_AUDIO(intro);
DECLARE_AUDIO(movingOn);
DECLARE_AUDIO(victory);
DECLARE_AUDIO(bossBattle);
DECLARE_AUDIO(tensionGoesUp);


GFraMe_ret gl_init() {
    GFraMe_ret rv;
    int atlas_w, atlas_h;
    unsigned char *data;
    
    data = 0;
    atlas_w = 256;
    atlas_h = 256;
    
    // Initialize and buffer (i.e., load from a file) the texture
    GFraMe_texture_init(&gl_tex);
    rv = GFraMe_assets_buffer_image("atlas", atlas_w, atlas_h, (char**)&data);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Buffering image failed", __ret);
    
    // Load the texture
    rv = GFraMe_texture_load(&gl_tex, atlas_w, atlas_h, data);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading texture failed", __ret);
    
    /**
     * Initialize the spriteset of a given dimensions
     */
    #define INIT_SSET(W, H) \
      gl_sset##W##x##H = &_glSset##W##x##H; \
      GFraMe_spriteset_init(gl_sset##W##x##H, &gl_tex, W, H)
    
    #define INIT_AUDIO(AUD, FILEN) \
        rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 0, 0, 1); \
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
        gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG(AUD, FILEN) \
        rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 1, 0, 1); \
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
        gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG_WINTRO(AUD, FILEN, LOOPPOS) \
        rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 1, LOOPPOS, 1); \
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
        gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG_NOLOOP(AUD, FILEN) \
        rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 0, 0, 1); \
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
        gl_aud_##AUD = &_glAud_##AUD
    
    INIT_SSET(4 , 4 );
    INIT_SSET(8 , 8 );
    INIT_SSET(8 , 16);
    INIT_SSET(8 , 32);
    INIT_SSET(16, 16);
    INIT_SSET(32, 8 );
    INIT_SSET(32, 32);
    INIT_SSET(64, 8 );
    INIT_SSET(64, 16);
    INIT_SSET(64, 32);
    
    INIT_AUDIO(menuMove, "menuMove");
    INIT_AUDIO(menuSelect, "menuSelect");
    INIT_AUDIO(text, "text");
    INIT_AUDIO(jump, "jump");
    INIT_AUDIO(highjump, "highjump");
    INIT_AUDIO(door, "door");
    INIT_AUDIO(terminal, "terminal");
    INIT_AUDIO(getItem, "getItem");
    INIT_AUDIO(heartup, "heartup");
    INIT_AUDIO(switchItem, "switchItem");
    INIT_AUDIO(shootEn, "shootEn");
    INIT_AUDIO(blHit, "blHit");
    INIT_AUDIO(fall, "fall");
    INIT_AUDIO(pause, "pause");
    INIT_AUDIO(plHit, "plHit");
    INIT_AUDIO(jumperJump, "jumperJump");
    INIT_AUDIO(jumperFall, "jumperFall");
    INIT_AUDIO(charger, "charger");
    INIT_AUDIO(teleport, "teleport");
    INIT_AUDIO(signaler, "signaler");
    INIT_AUDIO(shootBoss, "shootBoss");
    INIT_AUDIO(bombExpl, "bombExpl");
    INIT_AUDIO(bossExpl, "bossExpl");
    INIT_AUDIO(bossMove, "bossMove");
    INIT_AUDIO(plDeath, "plDeath");
    INIT_AUDIO(plStep, "plStep");
    INIT_SONG(menu, "menu");
    INIT_SONG(intro, "intro");
    INIT_SONG(movingOn, "movingOn");
    INIT_SONG_NOLOOP(victory, "victory");
    INIT_SONG_WINTRO(bossBattle, "bossBattle", 58800*4);
    INIT_SONG(tensionGoesUp, "tensionGoesUp");
    
    gl_isInit = 1;
    gl_running = 1;
    rv = GFraMe_ret_ok;
__ret:
    if (data)
        free(data);
    
    return rv;
}

void gl_clean() {
    if (gl_isInit) {
        GFraMe_texture_clear(&gl_tex);
        
        #define CLEAN_AUDIO(AUD) \
            if (gl_aud_##AUD) { \
                GFraMe_audio_clear(&_glAud_##AUD); \
            }
        
        CLEAN_AUDIO(menuMove);
        CLEAN_AUDIO(menuSelect);
        CLEAN_AUDIO(text);
        CLEAN_AUDIO(jump);
        CLEAN_AUDIO(highjump);
        CLEAN_AUDIO(door);
        CLEAN_AUDIO(terminal);
        CLEAN_AUDIO(getItem);
        CLEAN_AUDIO(heartup);
        CLEAN_AUDIO(switchItem);
        CLEAN_AUDIO(shootEn);
        CLEAN_AUDIO(blHit);
        CLEAN_AUDIO(fall);
        CLEAN_AUDIO(pause);
        CLEAN_AUDIO(plHit);
        CLEAN_AUDIO(jumperJump);
        CLEAN_AUDIO(jumperFall);
        CLEAN_AUDIO(charger);
        CLEAN_AUDIO(teleport);
        CLEAN_AUDIO(signaler);
        CLEAN_AUDIO(shootBoss);
        CLEAN_AUDIO(bombExpl);
        CLEAN_AUDIO(bossExpl);
        CLEAN_AUDIO(bossMove);
        CLEAN_AUDIO(plDeath);
        CLEAN_AUDIO(menu);
        CLEAN_AUDIO(intro);
        CLEAN_AUDIO(movingOn);
    }
}


