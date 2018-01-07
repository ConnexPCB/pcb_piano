/*
 * debugUart.c
 *
 *  Created on: Jan 3, 2018
 *      Author: birdman
 */


#include "debugUart.h"
#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_leuart.h"

//@brief Initialize debug LEUART for 9600 baud at defualt locations
void initDebugUart(void)
{
	CMU_ClockEnable(cmuOsc_LFRCO, true);		//enable LFRCO and wait for it to stabilize
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);	//Select LFRCO for LFB clock
	CMU_ClockEnable(cmuClock_LEUART0, true);			//enable LEUART0 peripheral clock
	CMU_ClockEnable(cmuClock_CORELE, true);				//enable low energy peripheral interface clock

	GPIO_PinModeSet(LEUART_PORT, LEUART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(LEUART_PORT, LEUART_RX_PIN, gpioModeInput, 0);

	LEUART_IntClear(LEUART0, (0xFF << 3) | 0x1); //Clear interrupt flags
	LEUART0->ROUTE = (3 << 8) | 0x3;	//Use location #3


	LEUART_Init_TypeDef leuart_init = {
		.enable = leuartEnable,
		.refFreq = 0,
		.baudrate = 9600,
		.databits = leuartDatabits8,
		.parity = leuartNoParity,
		.stopbits = leuartStopbits1,
	};


	LEUART_Init(LEUART0, &leuart_init);

	uint8_t testMessageLength = 25;
	char testMessage[25] = "LEUART is Initialized";

	debugUartSendString(testMessage, testMessageLength);
}

// @brief Use LEUART_Rx to send char over LEUART
void debugUartSendChar(uint8_t c)
{
	#ifdef DEBUG_UART
		LEUART_Tx(LEUART0, c);
	#endif
}

// @brief Send string of characters ---Note: this function is blocking
void debugUartSendString(char string[], uint8_t len)
{
	#ifdef DEBUG_UART
	uint8_t i = 0;
	for(i = 0; i < len; i++)
	{
		debugUartSendChar(string[i]);
	}
	#endif
}

// @brief Receive uint8_t from LEUART
uint8_t debugUartReceive(void)
{
	#ifdef DEBUG_UART
	return LEUART_Rx(LEUART0);
	#endif
}
