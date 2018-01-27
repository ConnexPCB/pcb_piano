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
#include "debugUart.h"

#define PIEZO_PORT		gpioPortE
#define PIEZO_PIN		13

#define PWM_FREQ  (40000) //PWM Carrier frequency in Hz
#define NUM_NOTES 12		//Number of notes on the keyboard.

#define PWM_TABLE_SIZE    200

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

//////////////////////////////////////
// @brief Gets next sample for sound
// @param pointer to sound struct
uint16_t getNextSample(Note_t * sound);

void playNote(uint8_t note, uint32_t num_samples);

void fillBufferWait(void);

#endif /* SRC_PIANOTIMER_H_ */
