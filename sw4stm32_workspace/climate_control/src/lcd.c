/*
 * lcd.c
 *
 *  Created on: 26/03/2014
 *      Author: mark
 */
#include <stm32l0xx.h>
#include "main.h"
#include "lcd.h"
#include <stdint.h>
#include "gpio.h"
#include "spi.h"
#include "systick.h"


#define OUTPUT 1
#define INPUT 0
#define LCD_COMMAND 0
#define LCD_DATA  1
#define LCD_SETDATA() GPIO_SET(A,6)
#define LCD_SETCONTROL() GPIO_CLR(A,6)
#define LCD_CS_SET_DIRECTION() GPIO_AS_OUTPUT(A,4)
#define LCD_DC_SET_DIRECTION() GPIO_AS_OUTPUT(A,6)
#define LCD_RST_SET_DIRECTION() GPIO_AS_OUTPUT(A,2)
#define LCD_ASSERT_RESET() GPIO_CLR(A,2)
#define LCD_RELEASE_RESET() GPIO_SET(A,2)
#define LCD_DELAY() lcd_delay()
volatile uint8_t spireadtmp;
#define WRITE_TO_SPI(x) spireadtmp = (*(__IO uint8_t *)&SPI1->DR); (*(__IO uint8_t *)&SPI1->DR = x)
#define WAIT_FOR_SPI_COMPLETION() while( SPI1->SR & SPI_SR_BSY )
#define SPI_TRANSMIT_NOT_FULL() (SPI1->SR & SPI_SR_TXE)
#define LCD_DESELECT_CHIP() GPIO_SET(A,4)// this is used for manual frame sync, define it as nothing if your SPI module self-syncs.
#define LCD_SELECT_CHIP() GPIO_CLR(A,4) // this is used for manual frame sync, define it as nothing if your SPI module self-syncs.

void lcd_delay(void)
{

	DelayMs(10);
}
#define LCD_X 84
#define LCD_Y 48

//This table contains the hex values that represent pixels
//for a font that is 5 pixels wide and 8 pixels high
static const uint8_t ASCII[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00} // 20
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c \ //make sure the slash is not at the end of the line as it will form a multi-line comment
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
  ,{0b00000110, 0b00001001, 0b00001001, 0b00000110, 0x00} // mp035: the degree symbol as 0x80.
};

#ifdef USE_SCREEN_BUFFER
uint8_t lcd_buffer[84][6]; // screen buffer for lcd
uint8_t lcd_col = 0, lcd_row = 0, buffer_written = 1, refresh_in_progress = 0, service_row = 0, service_col = 0;
#define LCDX 84
#define LCDY 6 //(bytes not pixels)

void LCDWrite(int command_or_data, int data)
{
    if(command_or_data)
    {
        while(refresh_in_progress) // don't alter data until any pending refresh is complete.
        {
            LCDService();
        }

        buffer_written = 0;
        lcd_buffer[lcd_col++][lcd_row] = data;
        if (lcd_col >= LCDX)
        {
            lcd_col = 0;
            lcd_row ++;
            if (lcd_row >=LCDY)
            {
                lcd_row = 0;
            }
        }
    }
    else
    {
        if (! buffer_written) //if the buffer has not been written when a command is sent, it is good practice to write it out so that the expected image is shown.
        {
            LCDRefresh(1);
        }
		WAIT_FOR_SPI_COMPLETION(); // because we need to sync the D/C pin, we need to wait for the SSP controller to finish the last transmission.
		LCD_DESELECT_CHIP();
		LCD_SETCONTROL();
		LCD_SELECT_CHIP();
        WRITE_TO_SPI(data);
    }
}

void LCDRefresh(int blocking)
{
    /* This initiates a screen refresh, either pass a non zero value in the 'blocking' argument to block the call until the refresh completes, or
     * repeatedly LCDService() when convenient until refresh_in_progress==0 to write the data out.  LCDService will return immediately if refresh_in_progress==0,
     * it is designed to be called regularly in a state machine when LCDRefresh is used as a non-blocking method.
     * */

    if (refresh_in_progress)
    {
        if (blocking)
        {
            while(refresh_in_progress) // if refresh is called blocking and a refresh is already in progress, clean up the last refresh and return.
            {
                LCDService();
            }
        }
        return; // but if refresh is called non-blocking and a refresh is already in progress, just ignore it, because it should be cleaned up by calls to LCDService.
    }


	WAIT_FOR_SPI_COMPLETION(); // because we need to sync the D/C pin, we need to wait for the SSP controller to finish whatever it is doing.
	LCD_DESELECT_CHIP();

	// move LCD memory position to 0,0
	LCD_SETCONTROL();
	LCD_SELECT_CHIP();
	WRITE_TO_SPI(0x80 | 0);  // Column.
	WRITE_TO_SPI(0x40 | 0);  // Row pixel rounded down to nearest 8
	WAIT_FOR_SPI_COMPLETION(); // because we need to sync the D/C pin, we need to wait for the SSP controller to finish
	LCD_DESELECT_CHIP();

	LCD_SELECT_CHIP();
	LCD_SETDATA();
	refresh_in_progress = 1;
	service_row = 0;
	service_col = 0;
	if (blocking)
	{
	    while(refresh_in_progress)
	    {
	        LCDService();
	    }
	    buffer_written = 1;
	}
}

void LCDService()
{
    /* This could be called in the SPI interrupt, or in an idle section of a state machine, it just writes out the data after a refresh call.
     * It is designed to be called regularly in a state machine.  When refresh_in_progress==0 it will return immediately.
     */
    if (refresh_in_progress == 0) return;

    // fill the transmit fifo, then return, so as not to block.
    while (SPI_TRANSMIT_NOT_FULL())
    {
        WRITE_TO_SPI(lcd_buffer[service_col++][service_row]);
        if (service_col >= LCDX)
        {
            service_col = 0;
            service_row ++;
            if (service_row >=LCDY)
            {
                service_row = 0;
                refresh_in_progress = 0;
                buffer_written = 1;
                break;
            }
        }
    }
}

void gotoXY(uint8_t x, uint8_t y)
{
    // this sends the current memory position to the given pixel.
    lcd_col = x;  // Column.
    lcd_row = (y/8);  // Row pixel rounded down to nearest 8
}

#else
void LCDWrite(int command_or_data, int data)
{
	WAIT_FOR_SPI_COMPLETION(); // because we need to sync the D/C pin, we need to wait for the SSP controller to finish the last transmission.
	if (command_or_data) LCD_SETDATA(); else LCD_SETCONTROL(); //Tell the LCD that we are writing either to data or a command
	WRITE_TO_SPI(data);

}

void gotoXY(int x, int y)
{
	// this sends the current memory position to the given pixel.
	LCDWrite(LCD_COMMAND, 0x80 | x);  // Column.
	LCDWrite(LCD_COMMAND, 0x40 | (y/8));  // Row pixel rounded down to nearest 8
}
#endif


// This takes a bitmap and draws it on the LCD starting at the specified position (0,0 at the top left)
// the first 2 bytes in the bitmap are the image width, and height
void LCDBitmap(const uint8_t bitmap[], uint8_t xpos, uint8_t ypos)
{
	uint8_t xsize = bitmap[0];
	uint8_t ysize = bitmap[1];
	char* my_array = bitmap + 2;
	int xcol;
	int yrow;
	for(yrow = 0; yrow < ysize; yrow+=8)
	{
		gotoXY(xpos, yrow+ypos);
    	for (xcol = 0; xcol < xsize; xcol++)
    	{
    		LCDWrite(LCD_DATA, my_array[yrow/8*ysize + xcol]);
    	}
	}
}

#ifdef USE_SCREEN_BUFFER

// This takes a large array of bits and sends them to the LCD, but only writes the
// white, or black pixels depending on the value of the argument pixelval (0 for white, 1 for black)
// the first 2 values in the array are the width, and height of the bitmap
void LCDOverlay(const uint8_t bitmap[], uint8_t xpos, uint8_t ypos, uint8_t pixelval)
{
	uint8_t xsize = bitmap[0];
	uint8_t ysize = bitmap[1];
	char* my_array = bitmap + 2;
	int xcol;
	int yrow;
	for(yrow = 0; yrow < ysize; yrow+=8)
	{
		gotoXY(xpos, yrow+ypos);
    	for (xcol = 0; xcol < xsize; xcol++)
    	{
    		uint8_t val = lcd_buffer[lcd_col][lcd_row];
    		if (pixelval) // only write black
    		{
    			val |= my_array[yrow/8*ysize + xcol];
    		}
    		else
    		{
    			val &= my_array[yrow/8*ysize + xcol];
    		}
    		LCDWrite(LCD_DATA, val);
    	}
	}
}

#endif

//This function takes in a character, looks it up in the font table/array
//And writes it to the screen
//Each character is 8 bits tall and 5 bits wide. We pad one blank column of
//pixels on each side of the character for readability.

void LCDCharacter(uint8_t character)
{
	LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding
	int index;
  	for (index = 0 ; index < 5 ; index++)
  		LCDWrite(LCD_DATA, ASCII[character - 0x20][index]);
    //0x20 is the ASCII character for Space (' '). The font table starts with this character

  	//LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding
}

//Given a string of characters, one by one is passed to the LCD
void LCDString(char *characters)
{
  while (*characters)
    LCDCharacter(*characters++);
}

//Clears the LCD by writing zeros to the entire screen
void LCDClear(void)
{
	int index;
	for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
		LCDWrite(LCD_DATA, 0x00);

	gotoXY(0, 0); //After we clear the display, return to the home position
}

// Blacks the LCD by writing ones to the entire screen
void LCDBlack(void)
{
	int index;
	for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
	LCDWrite(LCD_DATA, 0xFF);

	gotoXY(0, 0); //After we clear the display, return to the home position
}



void Init_LCD(void)
{
	// NOTE: GPIOInit() must be called here, if not already called in application
	Spi1Init();
	LCD_CS_SET_DIRECTION();
	LCD_DC_SET_DIRECTION();
	LCD_RST_SET_DIRECTION();
	LCD_DESELECT_CHIP();
	LCD_ASSERT_RESET();
	LCD_DELAY ();
	LCD_RELEASE_RESET();

	LCDWrite(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
	LCDWrite(LCD_COMMAND, 0xBF); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
	LCDWrite(LCD_COMMAND, 0x04); //Set Temp coefficent
	LCDWrite(LCD_COMMAND, 0x14); //LCD bias mode 1:48: Try 0x13 or 0x14

	LCDWrite(LCD_COMMAND, 0x20); //We must send 0x20 before modifying the display control mode
	LCDWrite(LCD_COMMAND, 0x0C); //Set display control, 0x0Cnormal mode. 0x0D for inverse

}


