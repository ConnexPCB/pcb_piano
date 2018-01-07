/*
 * circBuffer.h
 *
 *  Created on: Jan 2, 2018
 *      Author: birdman
 */

#ifndef SRC_CIRCBUFFER_H_
#define SRC_CIRCBUFFER_H_

#include "em_device.h"
#include "em_system.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#include "stdbool.h"
#include "stdio.h"

typedef struct
{
	uint16_t * 	buffer;
	uint16_t 	head;
	uint16_t	tail;
	uint16_t	size;	//all of the buffer in bytes
} CircBuffer_t;

/////////////////////////////////////////////////////
// @brief Sets size and mallocs space for buffer
void initCircBuff(CircBuffer_t * cbuf, uint16_t size);

/////////////////////////////////////////////
// @brief reset the head and tail of buffer, returns 0 on success
int circBufferReset(CircBuffer_t * cbuf);

////////////////////////////////////////////
//@brief checks if buffer is empty
bool circBufferEmpty(CircBuffer_t * cbuf);

////////////////////////////////////////////
// @brief checks if buffer is full
bool circBufferFull(CircBuffer_t * cbuf);

///////////////////////////////////////////
// @brief Puts data at head of buffer
int circBufferPut(CircBuffer_t * cbuf, uint16_t data);

////////////////////////////////////////////
// @brief Gets data from buffer at head
int circBufferGet(CircBuffer_t * cbuf, uint16_t * data);

uint16_t returnBufferLength();

int testBufferFill();

int testBufferGetSingle();

int testBufferEmpty();

int testBufferFull();

#endif /* SRC_CIRCBUFFER_H_ */
