/*
 * pianoTimer.h
 *
 *  Created on: Dec 29, 2017
 *      Author: birdman
 */

#ifndef SRC_PIANOTIMER_H_
#define SRC_PIANOTIMER_H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"


#include "sinTable.h"
#include "circBuffer.h"
//#include "debugUart.h"

#define CIRC_BUFF_NUM_VALUES	255 //Number of uint16_t values are in the circular buffer
#define CIRC_BUFF_LEN			(CIRC_BUFF_NUM_VALUES * 2)	//Number of bytes for length of buffer

#define NUM_NOTES	5 //Number of notes on the keyboard

#define TOP_VAL_PWM			750	//Sets frequency to 40khz (1MHz timer clock)

//#define UPDATE_PERIOD		250		//update compare value, toggle led1 every 1/4seconds

#define PIEZO_PORT		gpioPortE
#define PIEZO_PIN		13

typedef struct
{
	uint32_t	index;		//Counts from [0,period)
	uint32_t 	period;		//Number of DAC samples per cycle
	bool 		play;		//Play note
} Note_t;





///////////////////////////////
// @brief INitiialize timer 1 for pwm output
void initTimer1(void);

void initTimer2(void);
/////////////////////////////////////
// @brief Update Duty Cycle for Timer 1
// @param newDutyCycle value between 0 and 127 for duty cycle
void updateDutyCycle();

//////////////////////////////////////
// @brief Gets next sample for sound
// @param pointer to sound struct
uint16_t getNextSample(Note_t * sound);

#endif /* SRC_PIANOTIMER_H_ */
