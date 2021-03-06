/*
 * debugUart.h
 *
 *  Created on: Jan 3, 2018
 *      Author: birdman
 */

#ifndef DEBUGUART_H_
#define DEBUGUART_H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_usart.h"
#include "em_leuart.h"
#include "em_dma.h"

#include <string>
#include <cstdint>
#include <cstdio>
#include <sstream>

#include "circBuffer.h"

#define LEUART_PORT gpioPortF
#define LEUART_TX_PIN 0 //PF0
#define LEUART_RX_PIN 1 //PF1
#define DEBUG_UART		//Enables send and receive over the LEUART0
/* DMA control block, must be aligned to 256. */

typedef struct
{
	char str[10];
	uint8_t len;
} debugUartMsg_t;

//@brief Initialize debug LEUART for 9600 baud at defualt locations
void initDebugUart(void);

// @brief Use LEUART_Rx to send char over LEUART
void debugUartSendChar(uint8_t c);

void debugUartSendUint16(uint16_t c);

void debugUartSendUint8(uint8_t c);
// @brief Send string of characters ---Note: this function is blocking
void debugUartSendString(char * string, uint8_t len);

// @brief Send string of characters without length --- Note: This function is blocking
void debugUartSendString(char str[]);

// @brief Receive uint8_t from LEUART
char debugUartReceive(void);

void setupLeuartDma(uint16_t * bufferA, uint16_t * bufferB, uint16_t bufferLength, volatile bool * bufferEmptyFlag);

bool isPrimaryLeuartBuffActive(void);

void dmaLeuartTransferDone(unsigned int channel, bool primary, void *user);
#endif
