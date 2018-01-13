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

/**************************************************************************//**
 * @brief  DMA Callback function
 *
 * When the DMA transfer is completed, disables the DMA wake-up on TX in the
 * LEUART to enable the DMA to sleep even when the LEUART buffer is empty.
 *
 ******************************************************************************/
void dmaTransferDone(unsigned int channel, bool primary, void *user);

/**************************************************************************//**
 * @brief  Setup Low Energy UART with DMA operation
 *
 * The LEUART/DMA interaction is defined, and the DMA, channel and descriptor
 * is initialized. The destination for all the DMA transfers through this
 * channel is set to be the LEUART1 TXDATA register, and transfer complete
 * interrupt is enabled.
 *
 *****************************************************************************/
void setupLeuartDma(void);

/****************************************************************************//**
 * * @brief Setup PWM on Timer with DMA
 *  The TIMER 1 / DMA interaction is defined and the DMA, channel and descriptor
 *  is initialized. The destination for all the DMAtransfers through this
 *  channel is set to be the TIMER1 COMP BUFFER register, and transfer complete interrupt
 *  is enabled.
 */
void setupPwnDma( uint16_t * bufferA,  uint16_t * bufferB, uint16_t bufferLength, bool * bufferEmtpyFlag);

bool isPrimaryBuffActive(void);

bool checkBufferEmpty(bool primary);

void clearBufferEmptyFlag(bool primary);

#endif /* SRC_DMAPWM_H_ */
