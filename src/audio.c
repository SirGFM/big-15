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

int audio_isMuted() {
	return isSongMuted;
}
int sfx_isMuted() {
    return isSfxMuted;
}

void sfx_Mute() {
    isSfxMuted = 1;
}
void sfx_Unmute() {
    isSfxMuted = 0;
}

void audio_muteSong() {
    if (!isSongMuted) {
        isSongMuted = 1;
        GFraMe_audio_player_play_bgm(0, 0.60f);
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
        GFraMe_audio_player_play_bgm(gl_aud_menu, 0.60f);
    }
    curSong = SONG_MENU;
}

void audio_playIntro() {
    if (curSong != SONG_INTRO && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_intro, 0.60f);
    }
    curSong = SONG_INTRO;
}

void audio_playMovingOn() {
    if (curSong != SONG_MOVINGON && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_movingOn, 0.60f);
    }
    curSong = SONG_MOVINGON;
}

void audio_playBoss() {
    if (curSong != SONG_BOSSBATTLE && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_bossBattle, 0.60f);
    }
    curSong = SONG_BOSSBATTLE;
}

void audio_playVictory() {
    if (curSong != SONG_VICTORY && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_victory, 0.60f);
    }
    curSong = SONG_VICTORY;
}

void audio_playTensionGoesUp() {
    if (curSong != SONG_TENSIONGOESUP && !isSongMuted) {
        GFraMe_audio_player_play_bgm(gl_aud_tensionGoesUp, 0.60f);
    }
    curSong = SONG_TENSIONGOESUP;
}

void sfx_menuMove() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_menuMove, 0.5f);
}
void sfx_menuSelect() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_menuSelect, 0.5f);
}
void sfx_text() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_text, 0.5f);
}
void sfx_plJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jump, 0.5f);
}
void sfx_plHighJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_highjump, 0.5f);
}
void sfx_teleport() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_teleport, 0.4f);
}
void sfx_plFall() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_fall, 0.4f);
}
void sfx_switchItem() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_switchItem, 0.5f);
}
void sfx_plDeath() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plDeath, 0.7f);
}
void sfx_plHurt() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plHit, 0.75f);
}
void sfx_plStep() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plStep, 0.5f);
}
void sfx_bossExpl() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bossExpl, 0.36f);
}
void sfx_bossHit() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_plDeath, 0.4f);
}
void sfx_jumperJump() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jumperJump, 0.15f);
}
void sfx_jumperFall() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_jumperFall, 0.35f);
}
void sfx_shootEn() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_shootEn, 0.4f);
}
void sfx_charger() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_charger, 0.4f);
}
void sfx_shootBoss() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_shootBoss, 0.4f);
}
void sfx_bossMove() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bossMove, 0.5f);
}
void sfx_bombExpl() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_bombExpl, 0.35f);
}
void sfx_door() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_door, 0.3f);
}
void sfx_bulHit() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_blHit, 0.5f);
}
void sfx_heartUp() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_heartup, 0.55f);
}
void sfx_terminal() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_terminal, 0.3f);
}
void sfx_getItem() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_getItem, 0.75f);
}
void sfx_pause() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_pause, 0.4f);
}
void sfx_signaler() {
    if (!isSfxMuted) GFraMe_audio_play(gl_aud_signaler, 0.43f);
}
