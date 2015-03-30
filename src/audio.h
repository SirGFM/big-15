/**
 * @file src/audio.h
 */
#ifndef __AUDIO_H_
#define __AUDIO_H_

int audio_isMuted();
void audio_muteSong();
void audio_unmuteSong();
int audio_getVolume();
void audio_setVolume(int val);

int sfx_isMuted();
void sfx_Mute();
void sfx_Unmute();
int sfx_getVolume();
void sfx_setVolume(int val);

void audio_playMenu();
void audio_playIntro();
void audio_playMovingOn();
void audio_playBoss();
void audio_playVictory();
void audio_playTensionGoesUp();

void sfx_menuMove();
void sfx_menuSelect();
void sfx_text();
void sfx_plJump();
void sfx_plHighJump();
void sfx_teleport();
void sfx_plFall();
void sfx_switchItem();
void sfx_plDeath();
void sfx_plHurt();
void sfx_plStep();
void sfx_bossExpl();
void sfx_bossHit();
void sfx_jumperJump();
void sfx_jumperFall();
void sfx_shootEn();
void sfx_charger();
void sfx_shootBoss();
void sfx_bossMove();
void sfx_bombExpl();
void sfx_door();
void sfx_bulHit();
void sfx_heartUp();
void sfx_terminal();
void sfx_getItem();
void sfx_pause();
void sfx_signaler();

#endif

