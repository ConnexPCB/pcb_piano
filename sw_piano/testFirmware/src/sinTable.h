/*
 * sinTable.h
 *
 *  Created on: Jan 2, 2018
 *      Author: birdman
 */

#ifndef SRC_SINTABLE_H_
#define SRC_SINTABLE_H_

#define _USE_MATH_DEFINES
#include <cstdint>

#define NUM_SINE_SAMPLES 1024
//#define AMPLITUDE	4

//extern  const uint32_t sineTableLength;
//extern  const uint16_t sineTable[NUM_SINE_SAMPLES];

uint16_t returnSinIndex(uint32_t index);
uint32_t returnSinSize(void);

//template<size_t Size, uint16_t Amplitude = std::numeric_limits<uint16_t>::max()/2>
//
//struct SineTable
//{
//	//Constant expression that is evaluated at compile
//	constexpr SineTable()
//		: table()
//		{
//			//Loop evaluated by compiler
//			for(size_t i = 0; i < Size; i++)
//			{
//				table[i] = Amplitude * std::sin(2 * i * 3.14 / Size) + std::numeric_limits<uint16_t>::max()/2;
//			}
//		}
//		//Overload bracket operator to allow indexing as array
//		constexpr uint16_t operator[](size_t index) const
//		{
//			return table[index];
//		}
//		//Size method
//		constexpr size_t size() const
//		{
//			return Size;
//		}
//
//		//the table
//		uint16_t table[Size];
//};

#endif /* SRC_SINTABLE_H_ */
