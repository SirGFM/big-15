/**
 * @file src/textwindow.c
 * 
 * Module to write text into a window
 */
#ifndef __TEXTWINDOW_H_
#define __TEXTWINDOW_H_

/**
 * Initialize a text window
 * 
 * @param x Window's horizontal position (in tiles)
 * @param y Window's vertical position (in tiles)
 * @param w Window's width (in tiles)
 * @param h Window's height (in tiles)
 * @param text Text (the pointer will be copied)
 * @param textLen Text's length
 */
void textWnd_init(int x, int y, int w, int h, char *text, int textLen);

/**
 * Update's the text
 * 
 * @param ms Time elapsed (in ms)
 */
void textWnd_update(int ms);

/**
 * Render the text window
 */
void textWnd_draw();

/**
 * Check whether the text was completely written
 * 
 * @return 1 for true, 0 for false
 */
int textWnd_didFinish();

#endif

