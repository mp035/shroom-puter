/*
 * widgets.c
 *
 *  Created on: 07/04/2014
 *      Author: mark
 */
#include "main.h"
#include "lcd.h"
#include "print.h"
#include "widgets.h"

#ifndef USE_SCREEN_BUFFER
#error "You must use the LCD screen buffer for the widget library."
#endif



void draw_widget(widget_t* widget)
{

    int i; // used in several places as for loop iterator.
    int truecursorpos; // used only once in each case to calculate the displayed cursor position.

    switch(widget->widget_type)
    {
    case TIMECTRL:
        gotoXY(widget->xpos, widget->ypos);
        // print the characters
        printchar(widget->numeric_value/100,2,1);
        LCDCharacter(':');
        printshort(widget->numeric_value,2,1);

        //overlay the cursor
        // we assume an 8x5 font with a single pixel between the chars here.
        if (widget->cursor_position > 1) truecursorpos = widget->cursor_position +1; else truecursorpos = widget->cursor_position;
        for (i=0; i<7; i++)
        {
            lcd_buffer[widget->xpos + truecursorpos * 6 + i][widget->ypos / 8 + 1] |= 0x01;
        }
        // note that we don't refresh, refresh should be controlled at the application level.
        break;
    case NUMCTRL:
        gotoXY(widget->xpos, widget->ypos);
        // calculate the position of the decimal point
        int use_decimal;
        if (widget->decimal_position > 0 && widget->decimal_position < widget->digits)
        {
            use_decimal = 1;
            unsigned int multiplier = 1;
            for (i=0; i<(widget->digits - widget->decimal_position); i++)
            {
                multiplier*=10;
            }
            // print the characters
            printlong(widget->numeric_value/multiplier,widget->decimal_position,widget->zero_pad);
            LCDCharacter('.');
            printlong(widget->numeric_value,widget->digits - widget->decimal_position,1);
        }
        else
        {
            use_decimal = 0;
            printlong(widget->numeric_value,widget->digits,widget->zero_pad);
        }


        //overlay the cursor
        // we assume an 8x6 font area here.
        if (use_decimal && (widget->cursor_position >= widget->decimal_position)) truecursorpos = widget->cursor_position +1; else truecursorpos = widget->cursor_position;
        for (i=0; i<7; i++)
        {
            lcd_buffer[widget->xpos + truecursorpos * 6 + i][widget->ypos / 8 + 1] |= 0x01;
        }
        // note that we don't refresh, refresh should be controlled at the application level.
        break;
    }
}

void pass_key_to_widget(unsigned char keycode, widget_t* widget)
{
    widget->return_value = WIDGET_NOTHING; // do nothing as default

    switch(widget->widget_type)
    {
    case TIMECTRL:
        if (keycode >= '0' && keycode <= '9')
        {
            if(widget->cursor_position < 4)
            {
                int charvalue = keycode - '0';
                int replacevalue = widget->numeric_value;
                int multiplier = 1000;
                unsigned char i;
                for (i=0; i<widget->cursor_position; i++)
                {
                    multiplier/=10;
                }
                replacevalue = replacevalue - (replacevalue % multiplier); // round out values below cursor to 0
                replacevalue = replacevalue % (multiplier * 10); // remove digits above cursor
                widget->numeric_value -= replacevalue;
                widget->numeric_value += (charvalue * multiplier);
                widget->cursor_position += 1;
            }
        }
        else if (keycode == '*')
        {
            if(widget->cursor_position == 0)
            {
                widget->return_value = WIDGET_CANCEL; // cancel pending operation
            }
            else
            {
                widget->cursor_position -= 1;
            }
        }
        else if (keycode == '#')
        {
            if(widget->cursor_position > 3)
            {
                widget->return_value = WIDGET_CONFIRM; // cancel pending operation
            }
            else
            {
                widget->cursor_position += 1;
            }
        }

        if (widget->return_value == WIDGET_NOTHING)
        {
            if (widget->cursor_position > 3)
            {
                widget->return_value = WIDGET_DRAW_CONFIRM;
            }
            else if (widget->cursor_position == 0)
            {
                widget->return_value = WIDGET_DRAW_CANCEL;
            }
        }
        break;
    case NUMCTRL:
        if (keycode >= '0' && keycode <= '9')
        {
            if(widget->cursor_position < widget->digits)
            {
                int charvalue = keycode - '0';
                int replacevalue = widget->numeric_value;
                int multiplier = 1;
                unsigned char i;
                for (i=0; i<(widget->digits - widget->cursor_position - 1); i++)
                {
                    multiplier*=10;
                }
                replacevalue = replacevalue - (replacevalue % multiplier); // round out values below cursor to 0
                replacevalue = replacevalue % (multiplier * 10); // remove digits above cursor
                widget->numeric_value -= replacevalue;
                widget->numeric_value += (charvalue * multiplier);
                widget->cursor_position += 1;
            }
        }
        else if (keycode == '*')
        {
            if(widget->cursor_position == 0)
            {
                widget->return_value = WIDGET_CANCEL; // cancel pending operation
            }
            else
            {
                widget->cursor_position -= 1;
            }
        }
        else if (keycode == '#')
        {
            if(widget->cursor_position >= widget->digits)
            {
                widget->return_value = WIDGET_CONFIRM; // cancel pending operation
            }
            else
            {
                widget->cursor_position += 1;
            }
        }

        if (widget->return_value == WIDGET_NOTHING)
        {
            if (widget->cursor_position >= widget->digits)
            {
                widget->return_value = WIDGET_DRAW_CONFIRM;
            }
            else if (widget->cursor_position == 0)
            {
                widget->return_value = WIDGET_DRAW_CANCEL;
            }
        }
        break;
    }//swich widget->widget_type
    //draw_widget(widget);
}


