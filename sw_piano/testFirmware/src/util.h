/*
 * util.h
 *
 *  Created on: Jan 1, 2018
 *      Author: birdman
 */

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#define TOP_VAL_GP_TIMER 	14000	//Sets general purpose timer overflow freq t

#define LED_PORT		gpioPortE
#define LED0_PIN		10

volatile uint16_t ms_counter;


///////////////////////////////
//@brief initialize timer 0 millisecond counter
void initTimer0(void);

void initClock(void);

void initPins(void);


#endif /* SRC_UTIL_H_ */
