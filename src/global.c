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

static int gl_isInit = 0;
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
    }
}


