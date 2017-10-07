/*
 * lcd.h
 *
 *  Created on: 26/03/2014
 *      Author: mark
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include "main.h"

#define USE_SCREEN_BUFFER // undef this to stop using buffered functions and 518 bytes of ram will be saved.

#ifdef USE_SCREEN_BUFFER
extern uint8_t lcd_buffer[84][6]; // screen buffer for lcd
extern uint8_t lcd_col, lcd_row, buffer_written, refresh_in_progress;
void LCDRefresh(int blocking);
void LCDService();
#endif

void Init_LCD(void);
void LCDWrite(int command_or_data, int data);

void gotoXY(uint8_t x, uint8_t y);

// This takes a large array of bits and sends them to the LCD
// the first 2 values in the array are the width, and height of the bitmap
void LCDBitmap(const uint8_t bitmap[], uint8_t xpos, uint8_t ypos);

#ifdef USE_SCREEN_BUFFER

// This takes a large array of bits and sends them to the LCD, but only writes the
// white, or black pixels depending on the value of the argument pixelval (0 for white, 1 for black)
// the first 2 values in the array are the width, and height of the bitmap
void LCDOverlay(const uint8_t bitmap[], uint8_t xpos, uint8_t ypos, uint8_t pixelval);

#endif

//This function takes in a character, looks it up in the font table/array
//And writes it to the screen
//Each character is 8 bits tall and 5 bits wide. We pad one blank column of
//pixels on each side of the character for readability.
void LCDCharacter(uint8_t character);

//Given a string of characters, one by one is passed to the LCD
void LCDString(char *characters);

//Clears the LCD by writing zeros to the entire screen
void LCDClear(void);

// Blacks the LCD by writing ones to the entire screen
void LCDBlack(void);

#endif /* LCD_H_ */
