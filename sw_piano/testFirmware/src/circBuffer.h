/*
 * circBuffer.h
 *
 *  Created on: Jan 2, 2018
 *      Author: birdman
 */

#ifndef SRC_CIRCBUFFER_H_
#define SRC_CIRCBUFFER_H_

#include <cstdbool>
#include <cstdio>
#include <stdlib.h>

#include "debugUart.h"


template <class T>
class circularBuffer
{
public:
	circularBuffer(size_t size)
		: buf_(new T[size])
		, size_(size)
		{
			//empty constructor
		}
	~circularBuffer()
	{
		//free( buf_);
		delete[] buf_;
	}

	void put(T item)
	{
		buf_[head_] = item;
		head_ = (head_ + 1) % size_; //increment head

		//If buffer is full overwrite tail
		if(head_ == tail_)
		{
			tail_ = (tail_ + 1) % size_;
		}
	}

	T get(void)
	{
		if(empty())
		{
			return T();
		}

		//Read data and increment tail
		T val = buf_[tail_];
		tail_ = (tail_ + 1) % size_;

		return val;
	}

	void reset(void)
	{
		//set head  equal to tail
		head_ = tail_;
	}

	bool empty(void) const
	{
		//if head and tail are equal
		return (head_ == tail_);
		//return false;
	}

	bool full(void) const
	{
		//if tail is ahead of the head by 1 we are full
		return (head_ + 1) % size_ == tail_;
	}

	size_t size(void) const
	{
		return size_ - 1;
	}

private:
	T* buf_;
	uint32_t head_ = 0;
	uint32_t tail_ = 0;
	uint32_t size_ = 0;
};


//typedef struct
//{
//	uint16_t * 	buffer;
//	uint16_t 	head;
//	uint16_t	tail;
//	uint16_t	size;	//all of the buffer in bytes
//} CircBuffer_t;
//
///////////////////////////////////////////////////////
//// @brief Sets size and mallocs space for buffer
//void initCircBuff(CircBuffer_t * cbuf, uint16_t size);
//
///////////////////////////////////////////////
//// @brief reset the head and tail of buffer, returns 0 on success
//int circBufferReset(CircBuffer_t * cbuf);
//
//////////////////////////////////////////////
////@brief checks if buffer is empty
//bool circBufferEmpty(CircBuffer_t * cbuf);
//
//////////////////////////////////////////////
//// @brief checks if buffer is full
//bool circBufferFull(CircBuffer_t * cbuf);
//
/////////////////////////////////////////////
//// @brief Puts data at head of buffer
//int circBufferPut(CircBuffer_t * cbuf, uint16_t data);
//
//////////////////////////////////////////////
//// @brief Gets data from buffer at head
//int circBufferGet(CircBuffer_t * cbuf, uint16_t * data);
//
//uint16_t returnBufferLength();
//
//int testBufferInit();
//
//int testBufferFill();
//
//int testBufferGetSingle();
//
//int testBufferEmpty();
//
//int testBufferFull();

#endif /* SRC_CIRCBUFFER_H_ */
