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
	GPIO_PinOutSet(gpioPortF, 2);
	if(LEUART_IntGet(LEUART0) & (LEUART_IFC_TXC)) //If TXF flag is set
	{

		LEUART_IntClear(LEUART0, LEUART_IFC_TXC);
		if(!debugUartCharTxBuff.empty())
		{
			GPIO_PinOutClear(gpioPortF, 2);
			LEUART0->TXDATA = (uint32_t)debugUartCharTxBuff.get();
		}

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


	LEUART_IntEnable(LEUART0, (1<<0)); //Set TXC Transmit complete Interrupt
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


	uint8_t testMessageLength = 25;
	char testMessage[25] = "LEUART Init";
	//debugUartSendString("LEUART Init");
	//debugUartSendString(testMessage, testMessageLength);
	 /* LF register about to be modified require sync. busy check */
	  //LEUART_Sync(LEUART0, LEUART_SYNCBUSY_TXDATA);
	  debugUartCharTxBuff.put('A');
	while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
		  LEUART0->TXDATA = debugUartCharTxBuff.get();

	debugUartSendChar('B');
	debugUartSendChar('C');
	debugUartSendString("LEUART is Initialized!");

//
//	  if(val == 'E')
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'E';
//	  }
//	  if(val == 'F')
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'F';
//	  }
//	  if(val == 'G')
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'F';
//	  }
//	  if(val == 0)
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'0';
//	  }
//	  if(val != 0)
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'1';
//	  }
//	  if(val >= 0)
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'+';
//	  }
//	  if(val >= 65)
//	  {
//		  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  //debugUartCharTxBuff.put('B');
//		  LEUART0->TXDATA = (uint32_t)'+';
//	  }
//	  if(val >= 100)
//		  {
//			  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//			  //debugUartCharTxBuff.put('B');
//			  LEUART0->TXDATA = (uint32_t)'2';
//		  }
//
//	  uint8_t i = 0;
//	  for(i = 0; i < 127; i++)
//	  {
//		  if(val > i)
//			  {
//				  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//				  //debugUartCharTxBuff.put('B');
//				  LEUART0->TXDATA = (uint32_t)'>';
//			  }
//		  if(val == i)
//		  			  {
//		  				  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  				  //debugUartCharTxBuff.put('B');
//		  				  LEUART0->TXDATA = (uint32_t)'=';
//		  			  }
//		  if(val < i)
//		  			  {
//		  				  while(!(LEUART0->STATUS & LEUART_STATUS_TXBL));
//		  				  //debugUartCharTxBuff.put('B');
//		  				  LEUART0->TXDATA = (uint32_t)'L';
//		  			  }
//	  }
	  //val = debugUartCharTxBuff.get();
//}
}

// @brief Use LEUART_Rx to send char over LEUART
void debugUartSendChar(uint8_t c)
{
//	#ifdef DEBUG_UART
//		LEUART_Tx(LEUART0, c);
//	#endif
	//debugUartMsg_t temp = { c, 1};
	//debugUartMsgTxBuff.put(temp);
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
//	#ifdef DEBUG_UART
//		LEUART_Tx(LEUART0, c);
//	#endif
	//debugUartMsg_t temp = { c, 1};
	//debugUartMsgTxBuff.put(temp);
	uint8_t i = 0;
	char temp = 0;
	for(i = 0; i < 4; i+= 4)
	{
		temp = (char)((c >> i) & 0x00F);
		debugUartSendChar(temp + (temp > 9 ? 48 : 65));

	}
}
void debugUartSendUint8(uint8_t c)
{
//	#ifdef DEBUG_UART
//		LEUART_Tx(LEUART0, c);
//	#endif
	//debugUartMsg_t temp = { c, 1};
	//debugUartMsgTxBuff.put(temp);
	uint8_t i = 0;
	char temp = 0;
	for(i = 0; i < 2; i+= 4)
	{
		temp = (char)((c >> i) & 0x000F);
		debugUartSendChar(temp + (temp > 9 ? 48 : 65));

	}
}

// @brief Send string of characters ---Note: this function is blocking
void debugUartSendString(const char str[], uint8_t len)
{
//	#ifdef DEBUG_UART
//	uint8_t i = 0;
//	for(i = 0; i < len; i++)
//	{
//		debugUartSendChar(string[i]);
//	}
//	#endif
	//debugUartMsg_t msg = {*str, len};
	//debugUartMsgTxBuff.put(msg);
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
//	#ifdef DEBUG_UART
//	uint8_t len = std::strlen(str);
//	uint8_t i = 0;
//	for(i = 0; i < len; i++)
//	{
//		debugUartSendChar(str[i]);
//	}
//	#endif
//	debugUartMsg_t msg = {*str, std::strlen(str)};
//	debugUartMsgTxBuff.put(msg);
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
//	#ifdef DEBUG_UART
//	return LEUART_Rx(LEUART0);
//	#endif
	//return debugUartMsgRxBuff.get();
	return debugUartCharRxBuff.get();
}
