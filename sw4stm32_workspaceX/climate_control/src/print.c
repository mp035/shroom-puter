#include <stdbool.h>

#include "main.h" // to get headers for the platform etc.
#include "lcd.h"
#include "print.h"
#define lcd_char_offset 48 // if you add 48 to the digit you get the char code for that digit

void null_refresh(){} // a dummy function to redirect refresh calls to


void (*lcd_func)(unsigned char data) = LCDCharacter;

void printchar(unsigned char value, unsigned char characters, unsigned char zeroleader){ // be sure to specify characters when calling this one,
	unsigned char temp;								// or else you wil get the last overloaded function (void print(unsigned char text);!!
	unsigned char divisor;
	unsigned char digitprinted = false;
	
	if (characters == 0){ //if characters is unspecified, calculate the required number 
		temp = value;
		do{
			temp/=10;
			characters++;
		}while (temp > 0);
	}

	for(divisor=1;characters>1;characters--){divisor*= 10;} // make divisor 10 to the power of characters
	while (divisor > 0){
		temp = value/divisor;
		temp = temp % 10;
		if ( (temp == 0) & ( ! zeroleader) ){
			if (digitprinted) {
				(*lcd_func)('0');
			}
			else {
				if (divisor == 1){(*lcd_func)('0');} else {(*lcd_func)(' ');}
			}
		}
		else {
			digitprinted = true;
			(*lcd_func)(lcd_char_offset + temp);
		}
		divisor /= 10; // divide divisor by 10 for the next loop
	}
	
}
	
void printshort(unsigned short value, unsigned char characters, unsigned char zeroleader){
		unsigned short temp;
	unsigned short divisor;
	unsigned char digitprinted = false;
	
	if (characters == 0){ //if characters is unspecified, calculate the required number 
		temp = value;
		do{
			temp/=10;
			characters++;
		}while (temp > 0);
	}
	
	for(divisor=1;characters>1;characters--) {divisor*= 10;} // make divisor 10 to the power of characters
	while (divisor > 0){
		temp = value/divisor;
		temp = temp % 10;
		if ( (temp == 0) & ( ! zeroleader) ){
			if (digitprinted) {
				(*lcd_func)('0');
			}
			else {
				if (divisor == 1){(*lcd_func)('0');} else {(*lcd_func)(' ');}
			}
		}
		else {
			digitprinted = true;
			(*lcd_func)(lcd_char_offset + temp);
		}
		divisor /= 10; // divide divisor by 10 for the next loop
	}
	
}	

	
void printlong(unsigned long value, unsigned char characters, unsigned char zeroleader){
	unsigned long temp;
	unsigned long divisor;
	unsigned char digitprinted = false;
	if (characters == 0){ //if characters is unspecified, calculate the required number 
		temp = value;
		do{
			temp/=10;
			characters++;
		}while (temp > 0);
	}

		
	for(divisor=1;characters>1;characters--){divisor*= 10;} // make divisor 10 to the power of characters
	while (divisor > 0){
		temp = value/divisor;
		temp = temp % 10;
		if ( (temp == 0) & ( ! zeroleader) ){
			if (digitprinted) {
				(*lcd_func)('0');
			}
			else {
				if (divisor == 1){(*lcd_func)('0');} else {(*lcd_func)(' ');}
			}
		}
		else {
			digitprinted = true;
			(*lcd_func)(lcd_char_offset + temp);
		}
	
		divisor /= 10; // divide divisor by 10 for the next loop
	}
		
}

void print_ram_string(char text[]){
	unsigned char temp = 0;
	while (text[temp] != 0){
		(*lcd_func)(text[temp]);
		temp++;
	}	
	
}

void printstring(char *text){
	unsigned char temp = 0;
	while (text[temp] != 0){
		(*lcd_func)(text[temp]);
		temp++;
	}
		
}

/*
void print(unsigned char text){
	(*lcd_func)(text);
}
*/
