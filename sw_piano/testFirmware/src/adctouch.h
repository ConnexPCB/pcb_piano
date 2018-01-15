/*
 * adctouch.h
 *
 *  Created on: Jan 14, 2018
 *      Author: birdman
 */

#ifndef ADCTOUCH_H_
#define ADCTOUCH_H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_rtc.h"
#include "em_lesense.h"
#include "em_core.h"
#include "em_prs.h"
#include <cmath>

#include "debugUart.h"

void setupCapTouch(void);
void setupTimer0Ch0(void);

void setupTimer1Ch1(void);
uint16_t getNewVal(void);
void updateButtons(void);

bool getButton(uint8_t btn);

#endif /* ADCTOUCH_H_ */
