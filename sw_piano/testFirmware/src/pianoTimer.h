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
#include <cmath>

#include "sinTable.h"
#include "circBuffer.h"
#include "dmaPwm.h"
//#include "debugUart.h"

//#define CIRC_BUFF_NUM_VALUES	128 //Number of uint16_t values are in the circular buffer
//#define CIRC_BUFF_LEN			(CIRC_BUFF_NUM_VALUES * 2)	//Number of bytes for length of buffer

//#define NUM_NOTES	5 //Number of notes on the keyboard

#define TOP_VAL_PWM			1024	//Sets frequency to 40khz (1MHz timer clock)

//#define UPDATE_PERIOD		250		//update compare value, toggle led1 every 1/4seconds

#define PIEZO_PORT		gpioPortE
#define PIEZO_PIN		13

typedef struct
{
	uint32_t	index;		//Counts from [0,period)
	uint32_t 	period;		//Number of DAC samples per cycle
	uint32_t	samplesToPlay;	//Number of samples to play note -- This will be decremented as samples are added to sound buffer
	bool 		play;		//Play note
	uint16_t 	nextDutyCycle;
	bool 		countUp;
	uint16_t 	mulitplier;
} Note_t;



void initKeyboardSound(void);

///////////////////////////////
// @brief INitiialize timer 1 for pwm output
void initTimer1(void);

void initTimer2(uint16_t freq);
/////////////////////////////////////
// @brief Update Duty Cycle for Timer 1
// @param newDutyCycle value between 0 and 127 for duty cycle
void updateDutyCycle();

//////////////////////////////////////
// @brief Gets next sample for sound
// @param pointer to sound struct
uint16_t getNextSample(Note_t * sound);

void initNotes(Note_t * note, double freq);

void playNote(uint8_t note, uint32_t num_samples);

void fillBufferWait(void);

#endif /* SRC_PIANOTIMER_H_ */
