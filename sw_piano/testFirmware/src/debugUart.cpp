/*
 * debugUart.c
 *
 *  Created on: Jan 3, 2018
 *      Author: birdman
 */

#include "debugUart.h"



static circularBuffer<char> debugUartCharTxBuff(200);
static circularBuffer<char> debugUartCharRxBuff(100);

void LEUART0_IRQHandler(void)
{
	//GPIO_PinOutSet(gpioPortF, 2);
	if(LEUART_IntGet(LEUART0) & (LEUART_IF_TXC)) //If TXF flag is set
	{

		LEUART_IntClear(LEUART0, LEUART_IFC_TXC);
		if(!debugUartCharTxBuff.empty())
		{
			LEUART0->TXDATA = (uint32_t)debugUartCharTxBuff.get();
		}

	}
	else if(LEUART_IntGet(LEUART0) & (LEUART_IF_RXDATAV)) //If RXF flag is set
	{
		//LEUART_IntClear(LEUART0, LEUART_IFC_RXD);
		debugUartCharRxBuff.put(LEUART0->RXDATA);
	}
}
//@brief Initialize debug LEUART for 9600 baud at defualt locations
void initDebugUart(void)
{
	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);		//enable LFRCO and wait for it to stabilize
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);	//Select LFRCO for LFB clock
	CMU_ClockEnable(cmuClock_LEUART0, true);			//enable LEUART0 peripheral clock
	CMU_ClockEnable(cmuClock_CORELE, true);				//enable low energy peripheral interface clock

	GPIO_PinModeSet(LEUART_PORT, LEUART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(LEUART_PORT, LEUART_RX_PIN, gpioModeInput, 0);

	LEUART_IntClear(LEUART0, (0xFF << 3) | 0x1); //Clear interrupt flags
	LEUART0->ROUTE = (3 << 8) | 0x3;	//Use location #3


	LEUART_IntEnable(LEUART0, LEUART_IEN_TXC); //Set TXC Transmit complete Interrupt
	LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV);

	NVIC_EnableIRQ(LEUART0_IRQn);

	LEUART_Init_TypeDef leuart_init =
	{
		leuartEnable,
		 0,
		9600,
		leuartDatabits8,
		leuartNoParity,
		leuartStopbits1,
	};
	LEUART_Init(LEUART0, &leuart_init);

	debugUartSendString("\n\rLEUART Is Initialized!\n\r");
}

// @brief Use LEUART_Rx to send char over LEUART
void debugUartSendChar(uint8_t c)
{
	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
	{
		LEUART0->TXDATA = c;
	}
	else
	{
		debugUartCharTxBuff.put(c);
	}
}

// @brief Add uint16_t as hex to the uart buffer
void debugUartSendUint16(uint16_t c)
{
//	uint8_t i = 0;
	char temp = 0;
	temp = (char)((c >> 12) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));
	temp = (char)((c >> 8) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));
	temp = (char)((c >> 4) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));
	temp = (char)((c >> 0) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));



}
void debugUartSendUint8(uint8_t c)
{
	char temp = 0;
	temp = (char)((c >> 4) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));
	temp = (char)((c >> 0) & 0x000F);
	debugUartSendChar(temp + (temp > 9 ? 55 : 48));
}

// @brief Send string of characters ---Note: this function is blocking
void debugUartSendString(const char str[], uint8_t len)
{
	uint8_t i = 0;
	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
	{
		LEUART0->TXDATA = str[0];
		for(i = 1; i < len; i++)
		{
			while(debugUartCharTxBuff.full()); //Poll for full condition
			debugUartCharTxBuff.put(str[i]);
		}
	}
	else
	{
		for(i = 0; i < len; i++)
		{
			while(debugUartCharTxBuff.full()); //Poll for full condition
			debugUartCharTxBuff.put(str[i]);
		}
	}

}

void debugUartSendString(char str[])
{
	uint8_t i = 0;
	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
	{
		LEUART0->TXDATA = str[0];
		for(i = 1; i < strlen(str); i++)
		{
			while(debugUartCharTxBuff.full()); //Poll for full condition
			debugUartCharTxBuff.put(str[i]);
		}
	}
	else
	{
		for(i = 0; i < strlen(str); i++)
		{
			while(debugUartCharTxBuff.full()); //Poll for full condition
			debugUartCharTxBuff.put(str[i]);
		}
	}
}

// @brief Receive uint8_t from LEUART
char debugUartReceive(void)
{
	if(!debugUartCharRxBuff.empty())
	{
		return debugUartCharRxBuff.get();
	}
	else
	{
		return 0;
	}
}
