/**
 * @file src/audio.c
 */
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_audio_player.h>

#include "audio.h"
#include "global.h"

typedef enum {SONG_NONE=0, SONG_MENU, SONG_INTRO, SONG_MOVINGON,
              SONG_VICTORY, SONG_BOSSBATTLE, SONG_TENSIONGOESUP} song;
song curSong = SONG_NONE;
static int isSongMuted = 0;
static int isSfxMuted = 0;

static int song_volume;
static int sfx_volume;
static float sfx_volumef;

int audio_getVolume() {
    return song_volume;
}
int audio_isMuted() {
	return isSongMuted;
}

int sfx_getVolume() {
    return sfx_volume;
}
int sfx_isMuted() {
    return isSfxMuted;
}

void audio_setVolume(int val) {
    if (val < 0)
        val = 0;
    else if (val > 100)
        val = 100;
    
    if (song_volume != 0 && val == 0)
        audio_muteSong();
    else if (song_volume == 0 && val != 0)
        audio_unmuteSong();
    
    song_volume = val;
    GFraMe_audio_player_set_bgm_volume(song_volume / 100.0);
    
    isSongMuted = song_volume == 0;
}

void sfx_setVolume(int val) {
    if (val < 0)
        val = 0;
    else if (val > 100)
        val = 100;
    
    sfx_volume = val;
    
    isSfxMuted = sfx_volume == 0;
    sfx_volumef = sfx_volume / 100.0f;
}


void audio_muteSong() {
    if (!isSongMuted) {
        isSongMuted = 1;
#if defined(EMCC)
        GFraMe_audio_player_play_bgm(gl_aud_menu, 0.f);
#else
        GFraMe_audio_player_play_bgm(0, 0.60f);
#endif
    }
}

void audio_unmuteSong() {
    isSongMuted = 0;
    switch (curSong) {
        case SONG_MENU: {
            curSong = SONG_NONE;
            audio_playMenu();
        } break;
        case SONG_INTRO: {
            curSong = SONG_NONE;
            audio_playIntro();
        } break;
        case SONG_MOVINGON: {
            curSong = SONG_NONE;
            audio_playMovingOn();
        } break;
        case SONG_VICTORY: {
            curSong = SONG_NONE;
            audio_playVictory();
        } break;
        case SONG_BOSSBATTLE: {
            curSong = SONG_NONE;
            audio_playVictory();
        } break;
        case SONG_TENSIONGOESUP: {
            curSong = SONG_NONE;
            audio_playTensionGoesUp();
        } break;
        default: {}
    }
}

void audio_playMenu() {
    if (curSong != SONG_MENU && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_menu, song_volume / 100.0f);
    }
    curSong = SONG_MENU;
}

void audio_playIntro() {
    if (curSong != SONG_INTRO && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_intro, song_volume / 100.0f);
    }
    curSong = SONG_INTRO;
}

void audio_playMovingOn() {
    if (curSong != SONG_MOVINGON && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_movingOn, song_volume / 100.0f);
    }
    curSong = SONG_MOVINGON;
}

void audio_playBoss() {
    if (curSong != SONG_BOSSBATTLE && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_bossBattle, song_volume / 100.0f);
    }
    curSong = SONG_BOSSBATTLE;
}

void audio_playVictory() {
    if (curSong != SONG_VICTORY && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_victory, song_volume / 100.0f);
    }
    curSong = SONG_VICTORY;
}

void audio_playTensionGoesUp() {
    if (curSong != SONG_TENSIONGOESUP && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_tensionGoesUp, song_volume / 100.0f);
    }
    curSong = SONG_TENSIONGOESUP;
}

void sfx_menuMove() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_menuMove, sfx_volumef);
}
void sfx_menuSelect() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_menuSelect, sfx_volumef);
}
void sfx_text() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_text, sfx_volumef);
}
void sfx_plJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jump, sfx_volumef);
}
void sfx_plHighJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_highjump, sfx_volumef);
}
void sfx_teleport() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_teleport, 0.8f * sfx_volumef);
}
void sfx_plFall() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_fall, 0.8f * sfx_volumef);
}
void sfx_switchItem() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_switchItem, sfx_volumef);
}
void sfx_plDeath() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plDeath, 1.4f * sfx_volumef);
}
void sfx_plHurt() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plHit, 1.5f * sfx_volumef);
}
void sfx_plStep() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plStep, sfx_volumef);
}
void sfx_bossExpl() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bossExpl, 0.72f * sfx_volumef);
}
void sfx_bossHit() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plDeath, 0.8f * sfx_volumef);
}
void sfx_jumperJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jumperJump, 0.3f * sfx_volumef);
}
void sfx_jumperFall() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jumperFall, 0.7f * sfx_volumef);
}
void sfx_shootEn() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_shootEn, 0.8f * sfx_volumef);
}
void sfx_charger() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_charger, 0.8f * sfx_volumef);
}
void sfx_shootBoss() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_shootBoss, 0.8f * sfx_volumef);
}
void sfx_bossMove() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bossMove, sfx_volumef);
}
void sfx_bombExpl() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bombExpl, 0.7f * sfx_volumef);
}
void sfx_door() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_door, 0.6f * sfx_volumef);
}
void sfx_bulHit() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_blHit, sfx_volumef);
}
void sfx_heartUp() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_heartup, sfx_volumef);
}
void sfx_terminal() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_terminal, 0.6f * sfx_volumef);
}
void sfx_getItem() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_getItem, 1.5f * sfx_volumef);
}
void sfx_pause() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_pause, 0.8f * sfx_volumef);
}
void sfx_signaler() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_signaler, 0.86f * sfx_volumef);
}
