/**
 * @file src/audio.c
 */
#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_audio_player.h>

#include "audio.h"
#include "global.h"

typedef enum {SONG_NONE=0, SONG_MENU, SONG_INTRO, SONG_MOVINGON} song;
song curSong = SONG_NONE;
int isSongMuted = 0;

void audio_muteSong() {
    isSongMuted = 1;
    GFraMe_audio_player_play_bgm(0, 0.60f);
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

