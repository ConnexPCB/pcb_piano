/*
 * dma.cpp
 *
 *  Created on: Jan 13, 2018
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

#include "dmaPwm.h"

#define DMA_CHANNEL		0

/* GLOBAL VARIABLES */

/* DMA control block, must be aligned to 256. */
#if defined (__ICCARM__)
#pragma data_alignment=256
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2];
#elif defined (__CC_ARM)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#elif defined (__GNUC__)
DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#else
#error Undefined toolkit, need to define alignment
#endif

static uint16_t Max_Table_Size = 0;
static bool primaryBuffActive = true;

/* DMA callback structure */
DMA_CB_TypeDef cb[DMA_CHAN_COUNT];

/**************************************************************************//**
 * @brief  DMA Callback function
 *
 * When the DMA transfer is completed, disables the DMA wake-up on TX in the
 * LEUART to enable the DMA to sleep even when the LEUART buffer is empty.
 *
 ******************************************************************************/
void dmaTransferDone(unsigned int channel, bool primary, void *user)
{
  //(void) channel;
  //(void) primary;
  *((bool *)user) = true; //Set clear buffer flag
  primaryBuffActive = primary;

  DMA_RefreshPingPong(channel,
		  	  	  	  primary,
					  false,
					  NULL,
					  NULL,
					  Max_Table_Size - 1,
					  false);
}

void setupPwnDma(uint16_t * bufferA, uint16_t * bufferB, uint16_t bufferLength, bool * bufferEmptyFlag)
{

	Max_Table_Size = bufferLength;

	//Initializing the DMA
	DMA_Init_TypeDef dmaInit =
	{
		0,                    /* No descriptor protection */
		dmaControlBlock,      /* DMA control block alligned to 256 */
	};
	DMA_Init(&dmaInit);

	/* Setting call-back function */
	cb[DMA_CHANNEL].cbFunc  = dmaTransferDone;
	cb[DMA_CHANNEL].userPtr = bufferEmptyFlag;

	//Configuring the DMA Channel
	DMA_CfgChannel_TypeDef chnlCfg =
	{
		false,                   /* Normal priority */
		true,                   /* No interupt for callback function */
		DMAREQ_TIMER2_CC2,     /* Set LEUART1 TX buffer empty, as source of DMA signals */
		&(cb[DMA_CHANNEL]),      /* Callback */
	};
	DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

	//Setting up Channel Descriptors
	DMA_CfgDescr_TypeDef descrCfg =
	{
		dmaDataIncNone,    /* Do not increment destination address */
		dmaDataInc2,       /* Increment source address by two byte */
		dmaDataSize2,      /* Data size is two byte */
		dmaArbitrate1,     /* Rearbitrate for each byte recieved */
		0,                 /* No read/write source protection */
	};
	DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);	//Configure channel for Primary
	DMA_CfgDescr(DMA_CHANNEL, false, &descrCfg); //Configure channel for alternate

//Activate the DMA
	DMA_ActivatePingPong(DMA_CHANNEL,
                         false,
                         (void *)&(TIMER2->CC[2].CCVB),
                         (void *)bufferA,
                         Max_Table_Size - 1,
                         (void *)&(TIMER0->CC[0].CCVB),
                         (void *)bufferB,
                         Max_Table_Size - 1);
}

bool isPrimaryBuffActive(void)
{
	return primaryBuffActive;
}


