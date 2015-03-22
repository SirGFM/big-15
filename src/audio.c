/**
 * @file src/audio.c
 */
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_audio_player.h>

#include "audio.h"
#include "global.h"

typedef enum {SONG_NONE=0, SONG_MENU, SONG_INTRO, SONG_MOVINGON} song;
song curSong = SONG_NONE;
static int isSongMuted = 0;

int audio_isMuted() {
	return isSongMuted;
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
    audio_playMovingOn();
}

void audio_playVictory() {
    audio_playMovingOn();
}

void sfx_menuMove() {
    GFraMe_audio_play(gl_aud_menuMove, 0.5f);
}
void sfx_menuSelect() {
    GFraMe_audio_play(gl_aud_menuSelect, 0.5f);
}
void sfx_text() {
    GFraMe_audio_play(gl_aud_text, 0.5f);
}
void sfx_plJump() {
    GFraMe_audio_play(gl_aud_jump, 0.5f);
}
void sfx_plHighJump() {
    GFraMe_audio_play(gl_aud_highjump, 0.5f);
}
void sfx_teleport() {
    GFraMe_audio_play(gl_aud_teleport, 0.4f);
}
void sfx_plFall() {
    GFraMe_audio_play(gl_aud_fall, 0.4f);
}
void sfx_switchItem() {
    GFraMe_audio_play(gl_aud_switchItem, 0.5f);
}
void sfx_plDeath() {
    GFraMe_audio_play(gl_aud_plDeath, 0.7f);
}
void sfx_plHurt() {
    GFraMe_audio_play(gl_aud_plHit, 0.75f);
}
void sfx_plStep() {
    GFraMe_audio_play(gl_aud_plStep, 0.5f);
}
void sfx_bossExpl() {
    GFraMe_audio_play(gl_aud_bossExpl, 0.36f);
}
void sfx_bossHit() {
    GFraMe_audio_play(gl_aud_plDeath, 0.4f);
}
void sfx_jumperJump() {
    GFraMe_audio_play(gl_aud_jumperJump, 0.15f);
}
void sfx_jumperFall() {
    GFraMe_audio_play(gl_aud_jumperFall, 0.35f);
}
void sfx_shootEn() {
    GFraMe_audio_play(gl_aud_shootEn, 0.4f);
}
void sfx_charger() {
    GFraMe_audio_play(gl_aud_charger, 0.4f);
}
void sfx_shootBoss() {
    GFraMe_audio_play(gl_aud_shootBoss, 0.4f);
}
void sfx_bossMove() {
    GFraMe_audio_play(gl_aud_bossMove, 0.5f);
}
void sfx_bombExpl() {
    GFraMe_audio_play(gl_aud_bombExpl, 0.35f);
}
