/*
 * circBuffer.c
 *
 *  Created on: Jan 2, 2018
 *      Author: birdman
 */

#include "circBuffer.h"



//void initCircBuff(CircBuffer_t * cbuf, uint16_t size)
//{
//	//debugUartSendChar('c');
//	cbuf->size = size;
//	//cbuf->buffer = malloc(cbuf->size);
//	//debugUartSendChar('m');
//	circBufferReset(cbuf);
//	//debugUartSendChar('C');
//}
//
//int circBufferReset(CircBuffer_t * cbuf)
//{
//	int ret = -1;
//
//	if(cbuf)
//	{
//		cbuf->head = 0;
//		cbuf->tail = 0;
//		ret = 0;
//		bufferLength = 0;
//	}
//
//	return ret;
//}
//
//bool circBufferEmpty(CircBuffer_t * cbuf)
//{
//	//Empty is when Head == Tail
//	return (cbuf->head == cbuf->tail);
//}
//
//bool circBufferFull(CircBuffer_t * cbuf)
//{
//	//Full is when head is one behind tail
//	return ((cbuf->head + 1) % cbuf->size) == cbuf->tail;
//}
//
/////////////////////////////////////////////
//// @brief Puts data at head of buffer
//int circBufferPut(CircBuffer_t * cbuf, uint16_t data)
//{
//	int ret = -1;
//
//	if(cbuf)
//	{
//		cbuf->buffer[cbuf->head] = data;
//		cbuf->head = (cbuf->head + 1) % cbuf->size;
//		bufferLength++;
//		if(cbuf->head == cbuf->tail)
//		{
//			cbuf->tail = (cbuf->tail + 1) % cbuf->size;
//		}
//		ret = 0;
//	}
//
//	return ret;
//}
//
//int circBufferGet(CircBuffer_t * cbuf, uint16_t * data)
//{
//	int ret = -1;
//	//debugUartSendChar('F');
////	if(!circBufferEmpty(cbuf))
////	{
////		debugUartSendChar('E');
////	}
//	if(cbuf && data && !circBufferEmpty(cbuf))
//	{
//		*data = cbuf->buffer[cbuf->tail];
//		cbuf->tail = (cbuf->tail + 1) % cbuf->size;
//		//debugUartSendChar('G');
//		ret = 0;
//		bufferLength --;
//	}
//
//	return ret;
//}
//
//uint16_t returnBufferLength()
//{
//	return bufferLength;
//}
//
//int testBufferInit()
//{
//	initCircBuff(&testBuff, testBuffSize);
//}
//int testBufferFill()
//{
//	char C = 'A';
//
//	while(!circBufferFull(&testBuff))
//	{
//		circBufferPut(&testBuff,(uint16_t)C);
//		debugUartSendChar(C);
//		C++;
//
//	}
//	debugUartSendChar('*');
//}
//
//int testBufferFillNum(uint8_t num)
//{
//	char C = 'A';
//	uint8_t i = 0;
//	for(i = 0; i < num; i++)
//	{
//		circBufferPut(&testBuff, (uint16_t)C);
//		debugUartSendChar(C);
//		C++;
//	}
//	debugUartSendChar('*');
//}
//
//int testBufferGetSingle()
//{
//	uint16_t C = 'A';
//	if(!circBufferEmpty(&testBuff))
//	{
//		circBufferGet(&testBuff, &C);
//		debugUartSendChar((char)C);
//	}
//	debugUartSendChar('*');
//}
//
//int testBufferEmpty()
//{
////	uint16_t C = 'A';
////	while(!circBufferEmpty(&testBuff))
////	{
////		circBufferGet(&testBuff,&C);
////		debugUartSendChar((uint8_t)C);
////	}
////	debugUartSendChar('*');
//	return circBufferEmpty(&testBuff);
//}
//
//int testBufferFull(void)
//{
//	return circBufferFull(&testBuff);
//}
