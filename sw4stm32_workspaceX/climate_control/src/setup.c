/*
 * setup.c
 *
 *  Created on: 8 Feb 2017
 *      Author: mark
 */

#include <stm32l0xx.h>
#include "rcc.h"
#include "spi.h"
#include "gpio.h"
#include "lcd.h"
#include "systick.h"
#include "rtc.h"
#include "sht10.h"

void setup(int firsttime)
{
	SetClock16MhzInternal();
	ENABLE_GPIO_PORT(A);
	ENABLE_GPIO_PORT(B);
	//ENABLE_GPIO_PORT(C);
	SetupSysTick(1); // note that the PWM for the peltier is driven by systick, so keep it at 1ms

	EnableLseOscillator(); // to drive RTC
	RtcInit();

	// peltier output pins
	GPIO_AS_OUTPUT(A,3);
	GPIO_AS_OUTPUT(A,1);
	GPIO_AS_OUTPUT(A,0);
	GPIO_AS_OUTPUT(B,3);

	// LIGHT
	GPIO_AS_OUTPUT(A,11);

	// VENT
	GPIO_AS_OUTPUT(B,5);

	// HUMIDITY
	GPIO_AS_OUTPUT(B,4);

	init_keypad();

	GPIO_AS_AF(A,5);
	GPIO_AS_AF(A,7);
	GPIO_SELECT_AF(A,5,0);
	GPIO_SELECT_AF(A,7,0);
	Init_LCD();

	setup_sht10();
	read_temp_rh_sensor();


}