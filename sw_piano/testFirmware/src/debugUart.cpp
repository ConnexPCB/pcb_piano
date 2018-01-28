/*
 * debugUart.c
 *
 *  Created on: Jan 3, 2018
 *      Author: birdman
 */

#include "debugUart.h"
#include "dmaPwm.h"
/* DEFINES */

#define LEUART0_DMA_CHANNEL           2

#define BUF_MAX               100

#define WAKEUP_INTERVAL_MS    2000

#define LEUART_MAX_TABLE_SIZE 100
/* GLOBAL VARIABLES */

/* DMA control block, must be aligned to 256. */

//TODO: Does this need to be implemented twice?
//#if defined (__ICCARM__)
//#pragma data_alignment=256
//DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2];
//#elif defined (__CC_ARM)
//DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
//#elif defined (__GNUC__)
//DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
//#else
//#error Undefined toolkit, need to define alignment
//#endif

static char debugUartCharTxBuffA[LEUART_MAX_TABLE_SIZE];
static char debugUartCharTxBuffB[LEUART_MAX_TABLE_SIZE];

static bool primaryLeuartBuffActive = false;
static uint8_t leuartBufferCount = 0;
static bool leuartTxDmaInProgress = false;
static DMA_CB_TypeDef cb[DMA_CHAN_COUNT];
//static circularBuffer<volatile char> debugUartCharTxBuff(200);
static circularBuffer<volatile char> debugUartCharRxBuff(100);

void LEUART0_IRQHandler(void)
{
	//GPIO_PinOutSet(gpioPortF, 2);
//	if(LEUART_IntGet(LEUART0) & (LEUART_IF_TXC)) //If TXF flag is set
//	{
//
//		LEUART_IntClear(LEUART0, LEUART_IFC_TXC);
//		if(!debugUartCharTxBuff.empty())
//		{
//			LEUART0->TXDATA = (uint32_t)debugUartCharTxBuff.get();
//		}
//
//	}
	if(LEUART_IntGet(LEUART0) & (LEUART_IF_RXDATAV)) //If RXF flag is set
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


//	LEUART_IntEnable(LEUART0, LEUART_IEN_TXC); //Set TXC Transmit complete Interrupt
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
//	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
//	{
//		LEUART0->TXDATA = c;
//	}
//	else
//	{
//		debugUartCharTxBuff.put(c);
//	}
	if(leuartTxDmaInProgress) //add to alternate buffer
	{
		if(primaryLeuartBuffActive)
		{
			if(leuartBufferCount >= LEUART_MAX_TABLE_SIZE)
			{
				// Alt buffer is full. wait until the alternate buffer is being used
				while(!primaryLeuartBuffActive);
				//Use alternative buffer
				debugUartCharTxBuffA[leuartBufferCount] = c;
				leuartBufferCount++;
			}
			else
			{
				//There is space in current buffer
				debugUartCharTxBuffB[leuartBufferCount] = c;
				leuartBufferCount++;
			}
		}
		else
		{
			if(leuartBufferCount >= LEUART_MAX_TABLE_SIZE)
			{
				// Primary buffer is full. wait until the alternate buffer is being used
				while(primaryLeuartBuffActive);
				//Use alternative buffer
				debugUartCharTxBuffB[leuartBufferCount] = c;
				leuartBufferCount++;
			}
			else
			{
				//There is space in current buffer
				debugUartCharTxBuffA[leuartBufferCount] = c;
				leuartBufferCount++;
			}
		}
	}
	else
	{

		if(leuartBufferCount <= LEUART_MAX_TABLE_SIZE)
		{
			//There is space in current buffer
			debugUartCharTxBuffA[leuartBufferCount] = c;
			leuartBufferCount++;
		}
		else
		{
			// Primary buffer is full. wait until the alternate buffer is being used
			while(!primaryLeuartBuffActive);
			//Use alternative buffer
			debugUartCharTxBuffB[leuartBufferCount] = c;
			leuartBufferCount++;
		}
		DMA_ActivatePingPong(LEUART0_DMA_CHANNEL,		//lEUART DMA CHANNEL
		                         true,						//USE PRIMARY CHANNEL FIRST
		                         (void *)&(LEUART0->TXDATA), //COPY TO TX BUFF
		                         (void *)debugUartCharTxBuffA,			//BUFFER A IS PRIMARY
								 leuartBufferCount - 1,	//MAX BUFFER SIZE --> THIS WILL CHANGE DEPENDING ON BUFFER COUNT
		                         (void *)&(LEUART0->TXDATA), //ALWAYS TRANSMIT TO TX BUFF
		                         (void *)debugUartCharTxBuffB,			//BUFFER B IS ALTERNATE
								 LEUART_MAX_TABLE_SIZE - 1);						//INITIAL MESSAGE --> SEE ABOVE

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
//	uint8_t i = 0;
//	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
//	{
//		LEUART0->TXDATA = str[0];
//		for(i = 1; i < len; i++)
//		{
//			while(debugUartCharTxBuff.full()); //Poll for full condition
//			debugUartCharTxBuff.put(str[i]);
//		}
//	}
//	else
//	{
//		for(i = 0; i < len; i++)
//		{
//			while(debugUartCharTxBuff.full()); //Poll for full condition
//			debugUartCharTxBuff.put(str[i]);
//		}
//	}
	uint8_t i = 0;
	for(i = 0; i < len; i++)
	{
		//while(); //Poll for full condition
		//debugUartCharTxBuff.put(str[i]);
		debugUartSendChar(str[i]);
	}
}

void debugUartSendString(char str[])
{
//	uint8_t i = 0;
//	if(debugUartCharTxBuff.empty() && (LEUART0->STATUS & LEUART_STATUS_TXBL))
//	{
//		LEUART0->TXDATA = str[0];
//		for(i = 1; i < strlen(str); i++)
//		{
//			while(debugUartCharTxBuff.full()); //Poll for full condition
//			debugUartCharTxBuff.put(str[i]);
//		}
//	}
//	else
//	{
//		for(i = 0; i < strlen(str); i++)
//		{
//			while(debugUartCharTxBuff.full()); //Poll for full condition
//			debugUartCharTxBuff.put(str[i]);
//		}
//	}
	uint8_t i = 0;
	for(i = 0; i < strlen(str); i++)
	{
		//while(); //Poll for full condition
		//debugUartCharTxBuff.put(str[i]);
		debugUartSendChar(str[i]);
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

/**************************************************************************//**
 * @brief  DMA Callback function
 *
 * When the DMA transfer is completed, disables the DMA wake-up on TX in the
 * LEUART to enable the DMA to sleep even when the LEUART buffer is empty.
 *
 ******************************************************************************/
void dmaLeuartTransferDone(unsigned int channel, bool primary, void *user)
{
  //(void) channel;
  //(void) primary;
  *((bool *)user) = true; //Set clear buffer flag
  primaryLeuartBuffActive = primary;

  //

  if(leuartBufferCount > 0)
  {
	  DMA_RefreshPingPong(channel,
						  primary,
						  false,
						  NULL,
						  NULL,
						  leuartBufferCount - 1,
						  false);
	  leuartBufferCount = 0; //Reset the buffer count
	  leuartTxDmaInProgress = true;
  }
  else
  {
	  leuartTxDmaInProgress = false;
	  //Dont do anything
	  //TODO: Do I need to do anything?
  }
}

void setupLeuartDma(uint16_t * bufferA, uint16_t * bufferB, uint16_t bufferLength, volatile bool * bufferEmptyFlag)
{

	//Max_Table_Size = bufferLength;

	//Initializing the DMA
	DMA_Init_TypeDef dmaInit =
	{
		0,                    /* No descriptor protection */
		dmaControlBlock,      /* DMA control block alligned to 256 */
	};
	DMA_Init(&dmaInit);

	/* Setting call-back function */
	cb[LEUART0_DMA_CHANNEL].cbFunc  = dmaLeuartTransferDone;
	cb[LEUART0_DMA_CHANNEL].userPtr = (void *)bufferEmptyFlag;

	//Configuring the DMA Channel
	DMA_CfgChannel_TypeDef chnlCfg =
	{
		false,                   /* Normal priority */
		true,                   /* No interupt for callback function */
		DMAREQ_LEUART0_TXBL,     /* Set LEUART1 TX buffer empty, as source of DMA signals */
		&(cb[LEUART0_DMA_CHANNEL]),      /* Callback */
	};
	DMA_CfgChannel(LEUART0_DMA_CHANNEL, &chnlCfg);

	//Setting up Channel Descriptors
	DMA_CfgDescr_TypeDef descrCfg =
	{
		dmaDataIncNone,    /* Do not increment destination address */
		dmaDataInc1,       /* Increment source address by two byte */
		dmaDataSize1,      /* Data size is two byte */
		dmaArbitrate1,     /* Rearbitrate for each byte recieved */
		0,                 /* No read/write source protection */
	};

	DMA_CfgDescr(LEUART0_DMA_CHANNEL, true, &descrCfg);	//Configure channel for Primary
	DMA_CfgDescr(LEUART0_DMA_CHANNEL, false, &descrCfg); //Configure channel for alternate

//	Activate the DMA
	//debugUartCharTxBuffB = {'D', 'M', 'A', ' ', 'A', 'C', 'T', 'I', 'V', 'A', 'T', 'E', 'D', '\n', '\r'};
//	DMA_ActivatePingPong(LEUART0_DMA_CHANNEL,		//lEUART DMA CHANNEL
//                         false,						//USE ALTERNATE CHANNEL FIRST
//                         (void *)&(LEUART0->TXDATA), //COPY TO TX BUFF
//                         (void *)bufferA,			//BUFFER A IS PRIMARY
//						 LEUART_MAX_TABLE_SIZE - 1,	//MAX BUFFER SIZE --> THIS WILL CHANGE DEPENDING ON BUFFER COUNT
//                         (void *)&(LEUART0->TXDATA), //ALWAYS TRANSMIT TO TX BUFF
//                         (void *)bufferB,			//BUFFER B IS ALTERNATE
//						 14);						//INITIAL MESSAGE --> SEE ABOVE

	debugUartSendString("DMA ACTIVATED\n\r");
	//Don't activate dma quite yet as nothing is in the buffer

//	DMA_ActivateBasic(LEUART0_DMA_CHANNEL, //Use leuart0 channel for DMA
//						false, 				//ACtivate using primary descriptor
//						false,				//Not using burst mode
//						(void *)&(LEUART0->TXDATA),			//Send data to the TX buff
//						(void*)_&(debugUartCharTxBuffB), 	//Use the alternate buffer right now.
//						buffCount - 1;						//Number of things in the buffer
}

bool isPrimaryLeuartBuffActive(void)
{
	return primaryLeuartBuffActive;
}
