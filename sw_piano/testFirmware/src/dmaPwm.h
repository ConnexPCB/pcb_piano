/*
 * dma.h
 *
 *  Created on: Jan 13, 2018
 *      Author: birdman
 */

#ifndef SRC_DMAPWM_H_
#define SRC_DMAPWM_H_

#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "em_dma.h"
#include "em_gpio.h"

#include "debugUart.h"

#if defined (__ICCARM__)
#pragma data_alignment=256
extern DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2];
#elif defined (__CC_ARM)
extern DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#elif defined (__GNUC__)
extern DMA_DESCRIPTOR_TypeDef dmaControlBlock[DMA_CHAN_COUNT * 2] __attribute__ ((aligned(256)));
#else
#error Undefined toolkit, need to define alignment
#endif

/**************************************************************************//**
 * @brief  DMA Callback function for Timer 2
 *
 * Sets the buffer empty flag and changes the primaryBufferFlag. Then updates the
 * DMA.
 ******************************************************************************/
void dmaTransferDone(unsigned int channel, bool primary, void *user);


/****************************************************************************//**
 * * @brief Setup PWM on Timer with DMA
 *  The TIMER2 / DMA interaction is defined and the DMA, channel and descriptor
 *  is initialized. The destination for all the DMAtransfers through this
 *  channel is set to be the TIMER2 COMP BUFFER register, and transfer complete interrupt
 *  is enabled.
 */
void setupPwnDma( uint16_t * bufferA,  uint16_t * bufferB, uint16_t bufferLength, volatile bool * bufferEmtpyFlag);

bool isPrimaryBuffActive(void);

#endif /* SRC_DMAPWM_H_ */
