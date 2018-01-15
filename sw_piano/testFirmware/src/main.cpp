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

#include <cstring>
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
#include "adctouch.h"

#define LEUART_PORT gpioPortF
#define LEUART_TX_PIN 0 //PF0
#define LEUART_RX_PIN 1 //PF1

#define DEFAULT_NOTE_LENGTH 6000
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  initClock();

  GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
  GPIO_PinOutClear(gpioPortF, 2);
  GPIO_PinModeSet(gpioPortF, 3, gpioModePushPull, 1);
  GPIO_PinOutSet(gpioPortF, 3);

  initDebugUart();
  initTimer0();
  fillBufferWait();
  initKeyboardSound();

  setupCapTouch();

  debugUartSendString("\n\rStarting The Coolest Shit Ever\n\r");
//  int seconds = 0;
//  uint8_t countingNote = 0;
  char rx_char = 0;
uint8_t i = 0;
  uint16_t count = 0; //Temp for Cap touch

  while (1) {
	  updateButtons();
//	  if(getTimeMs() >= 1500)
//	  {
//		  seconds++;
//		  debugUartSendUint16(seconds);
//		  char buffer[33];
//		  debugUartSendString("Playing Note:");
//		  debugUartSendChar(countingNote + 48);
//		  debugUartSendString("\n\r");
//		  playNote(countingNote, #define DEFAULT_NOTE_LENGTH 6000);
//		  countingNote = (countingNote + 1) % 12;
//		  resetTimeMs();
//	  }
//	  count = 0;
//	  TIMER1->CMD = TIMER_CMD_START;
//
	  if(getTimeMs() >= 1000)
	  {
//		  TIMER1->CMD = TIMER_CMD_STOP;
//		  count = TIMER1->CNT;
//		  TIMER1->CNT = 0;
		  //debugUartSendUint16(count);
		  //debugUartSendString("\n\r");
//
		  //playNote(2, DEFAULT_NOTE_LENGTH);
//		  debugUartSendString("Playing Note: ");
//		  debugUartSendChar(rx_char);
//		  debugUartSendString("\n\r");

		  resetTimeMs();

	  }
	  for(i = 0; i < 5; i++)
	  {
		  if(getButton(i))
		  {
			  debugUartSendString("Button is Pressed: ");
			  debugUartSendUint8(count);
			  debugUartSendString("\n\r");
			  playNote((3+i),3000);
		  }
	  }
	  //else if(count > 100 && count <= 200)
//	  {
//		  debugUartSendUint16(count);
//		  debugUartSendString("\n\r");
//		  playNote(1,6000);
//	  }else if(count > 200 && count <  1000)
//	  {
//		  debugUartSendUint16(count);
//		  debugUartSendString("\n\r");
//		  playNote(2,6000);
//	  }

	  fillBufferWait();
	  rx_char = debugUartReceive();
	  if(rx_char)
	  {
		  playNote((uint8_t)(rx_char - 48), DEFAULT_NOTE_LENGTH);
		  debugUartSendString("Playing Note: ");
		  debugUartSendChar(rx_char);
		  debugUartSendString("\n\r");
	  }
	  //playNote(0, 30000);
//	  switch(rx_char)
//	  {
//
//		  case '0':
//			  playNote(0, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '1':
//			  playNote(1, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '2':
//			  playNote(2, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '3':
//			  playNote(3, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '4':
//			  playNote(4, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '5':
//			  playNote(5, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '6':
//			  playNote(6, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '7':
//			  playNote(7, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '8':
//			  playNote(8, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case '9':
//			  playNote(9, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case 'A':
//			  playNote(10, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case 'B':
//			  playNote(11, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//		  case 'C':
//			  playNote(12, DEFAULT_NOTE_LENGTH);
//			  debugUartSendString("Playing Note: ");
//			  debugUartSendChar(rx_char);
//			  debugUartSendString("\n\r");
//			  break;
//
//	  }
  }
}
