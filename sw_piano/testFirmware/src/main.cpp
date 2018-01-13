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
//#include "stdlib.h"

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


//namespace patch
//{
//    template < typename T > std::string to_string( const T& n )
//    {
//        std::ostringstream stm ;
//        stm << n ;
//        return stm.str() ;
//    }
//}
#define LEUART_PORT gpioPortF
#define LEUART_TX_PIN 0 //PF0
#define LEUART_RX_PIN 1 //PF1

const char test[] = "\n\rHelloWord!\n\r";

int main(void)
{
  /* Chip errata */
  CHIP_Init();
  initClock();
  GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
  GPIO_PinOutClear(gpioPortF, 2);
  initDebugUart();
  initTimer0();
  //playNote(0, 10);
  fillBufferWait();
  //fillBufferZeroes()
  //debugUartSendChar('E');
//  initTimer1();
  initKeyboardSound();
//static circularBuffer<bool> alphabet(10);



  GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
//  GPIO_PinOutSet(gpioPortF, 2);
  /* Infinite loop */
  char rx_char = 0;
  uint8_t countingNote = 0;
  //debugUartSendChar('C');
  //playNote(countingNote, 255);
  //debugUartSendChar('A');
  debugUartSendString("Neat");
  debugUartSendString("\n\r");
  int seconds = 0;

  while (1) {
	  if(getTimeMs() >= 2000)
	  {
		  //seconds++;
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  		  LEUART0->TXDATA = 'A';
		  //debugUartSendString("1 Second has passed\n\r");
		  char buffer[33];
		  //itoa(seconds, &buffer, 10);
		  debugUartSendString("Playing Note:");
		  debugUartSendChar(countingNote + 48);
		  debugUartSendString("\n\r");
		  playNote(countingNote, 12000);
		  countingNote = (countingNote + 1) % 5;
		  resetTimeMs();
	  }
	  fillBufferWait();

//
//	  debugUartSendChar('C');
//	  if(getTimeMs())
//	  {
//		  debugUartSendChar('D');
//
//	  }
//	  if(getTimeMs() > 2)
//	  {
//		  debugUartSendChar('E');
//
//	  }
//	  if(getTimeMs() >= 1000)
//	  {
//		  //debugUartSendString("DangNeat");
//		  debugUartSendChar('B');
//		  resetTimeMs();
//	  }
//	  else if(!(getTimeMs() % 100))
//	  {
//		  debugUartSendChar('A');
//	  }


//	  if(returnBufferLength <= 100)
//	  {
//		  playNote(countingNote, 10);
//		  debugUartSendChar('b');
//	  }
	  //if(debugUartReceive() == 'R')
	  //{

		  //fillBufferNumSamples(127);
	  //}
  }

///	uint32_t i = 0;
//		char rx_char = 1;
//
//
//		CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);  //Enable LFRC oscillator and wait for stability
//		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
//		CMU_ClockEnable(cmuClock_LEUART0, true);
//		CMU_ClockEnable(cmuClock_CORELE, true);
//		CMU_ClockEnable(cmuClock_GPIO, true);
//
//
//		GPIO_PinModeSet(LEUART_PORT, LEUART_TX_PIN, gpioModePushPull, 1);
//		GPIO_PinModeSet(LEUART_PORT, LEUART_RX_PIN, gpioModeInput, 0);
//
//		LEUART_IntClear(LEUART0, (0xFF << 3) | 0x1); //Clear interrupt flags
//		LEUART0->ROUTE = (3 << 8) | 0x3;	//Use location #3
//
//
//		LEUART_Init_TypeDef leuart_init = {
//			.enable = leuartEnable,
//			.refFreq = 0,
//			.baudrate = 9600,
//			.databits = leuartDatabits8,
//			.parity = leuartNoParity,
//			.stopbits = leuartStopbits1,
//		};
//
//
//		LEUART_Init(LEUART0, &leuart_init);
//
//		/*print test string*/
//		for(i=0; i<sizeof(test); i++)
//		{
//			LEUART_Tx(LEUART0, test[i]);
//		}
//
//
//		while(1)
//		{
//		rx_char = LEUART_Rx(LEUART0);
//		if(rx_char)
//		{
//			LEUART_Tx(LEUART0, rx_char + 1);
//			rx_char = 0;
//		}
//		GPIO_PinOutToggle(gpioPortF, 2);
//		 for(i = 0; i < 10000; i++);
//
//		 for(i=0; i<sizeof(test); i++)
//			{
//				LEUART_Tx(LEUART0, test[i]);
//
//			}
//		}
}
