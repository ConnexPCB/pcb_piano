/*
 * pcbPianoMain.c
 *
 *  Created on: Jan 2, 2018
 *      Author: birdman
 */


#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_leuart.h"

#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
//#include <sstream>

#include "debugUart.h"
#include "pianoTimer.h"
#include "util.h"
#include "circBuffer.h"

#define LEUART_PORT gpioPortF
#define LEUART_TX_PIN 0 //PF0
#define LEUART_RX_PIN 1 //PF1

int main(void)
{
  /* Chip errata */
  CHIP_Init();
  initClock();

  GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
  GPIO_PinOutClear(gpioPortF, 2);
  initDebugUart();
  initTimer0();
  fillBufferWait();
  initKeyboardSound();


  debugUartSendString("Neat");
  debugUartSendString("\n\r");
  int seconds = 0;
  uint8_t countingNote = 0;

  while (1) {
	  if(getTimeMs() >= 1500)
	  {
		  seconds++;
		  debugUartSendUint16(seconds);
		  char buffer[33];
		  debugUartSendString("Playing Note:");
		  debugUartSendChar(countingNote + 48);
		  debugUartSendString("\n\r");
		  playNote(countingNote, 12000);
		  countingNote = (countingNote + 1) % 12;
		  resetTimeMs();
	  }
	  fillBufferWait();
  }
}
