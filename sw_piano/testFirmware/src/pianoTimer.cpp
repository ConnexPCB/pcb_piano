/*
 * pianoTimer.c
 *
 *  Created on: Jan 1, 2018
 *      Author: birdman
 */

#include "pianoTimer.h"

uint16_t PWMTableA[PWM_TABLE_SIZE];
uint16_t PWMTableB[PWM_TABLE_SIZE];

static bool bufferEmptyFlag = true; //Flag to signal when buffer is empty of full

static Note_t keyboard[NUM_NOTES];

void initKeyboardSound(void)
{
	debugUartSendString("\n\rInitializing Keyboard\n\r");

	initTimer2(PWM_FREQ);	//Start Timer2 channel 2 for PWM
	uint8_t i = 0;
	double freq = 0;
	double power = 0;
	//Calculating the notes and frequencies
	for(i = 0; i <= NUM_NOTES; i++)
	{
		debugUartSendString("Note: ");
		debugUartSendChar((char)(i + 65));
		debugUartSendString("\n\r");
		////Calculate the frequency of a note based on steps above/below C
		power = (double)((i - 8.0)/ 12);
		freq = 440.0 * std::pow(2, power);

		keyboard[i].index = 0;
		keyboard[i].period = (uint16_t) (PWM_FREQ / freq); //Calculate number of pwm samples per signal period
	}
}

void initTimer2(uint16_t freq)
{
	debugUartSendString("\n\rBegin PWM Init\n\r");
	setupPwnDma((uint16_t *)&PWMTableA, (uint16_t *)&PWMTableB, PWM_TABLE_SIZE, &bufferEmptyFlag);

	uint8_t i = 0;
	for(i = 0; i < PWM_TABLE_SIZE; i++)
	{
		PWMTableA[i] = 0;
		PWMTableB[i] = 0;
	}
	uint32_t topValue = 0;
	CMU_ClockEnable(cmuClock_TIMER2, true);
	GPIO_PinModeSet(PIEZO_PORT, PIEZO_PIN, gpioModePushPull, 0); //Set output

	TIMER_InitCC_TypeDef timerCCInit =
	{
		timerEventEveryEdge,	//value ignored -- note CC
		timerEdgeBoth,		//value ignore -- not CC
		timerPRSSELCh0,		//value ignored -- not using PRS
		timerOutputActionNone,	//no action on underflow
		timerOutputActionSet,		//On overflow, output goes high
		timerOutputActionToggle, 	//On compare, output clears
		timerCCModePWM,			//Set timer to run in PWM mode
		false,				//Not using input -- no filter
		false,				//Not using PRS
		false,				//initial state for pwm is high when timer is enabled
		false,				//non-inverted output
	};
	//Configure CC channel 2
	TIMER_InitCC(TIMER2, 2, &timerCCInit);
	TIMER2->ROUTE = (3 << 16) |(1 << 2);         // connect PWM output (timer2, channel 2, Location 3) to PE13 (LED0)
	//Set Top value for Timer with given frequency
	uint32_t clock_freq = CMU_ClockFreqGet(cmuClock_HFPER);
	debugUartSendString("Sys Freq: ");
	debugUartSendUint16((uint16_t)(clock_freq >> 16));
	debugUartSendUint16((uint16_t)(clock_freq & 0x0FF));
	debugUartSendString("\r\n");
	topValue = clock_freq/freq;
	TIMER_TopSet(TIMER2, topValue);
	debugUartSendString("Freq: ");
	debugUartSendUint16((uint16_t)freq);
	debugUartSendString("\r\n");

	debugUartSendString("Top Val: ");
	debugUartSendUint16((uint16_t)topValue);
	debugUartSendString("\r\n");


	//Set compare value starting at PWMTableA
	TIMER_CompareSet(TIMER2, 2, PWMTableA[0]);

	//Setup timer configuration for PWM
	TIMER_Init_TypeDef	timerPWMInit =
	{
		true,					//Start timer upon configuration
		true,				//Run timer in debug mode
		timerPrescale1,	//set prescale to /1
		timerClkSelHFPerClk, 	//Set clock source to HFPERCLK
		false,
		false,
		timerInputActionNone, //No action from inputs
		timerInputActionNone, //No action from inputs
		timerModeUp,			//Not using up-count mode
		false,				//not using dma
		false,				//Not using quad dec. mode
		false,				//not using one shot mode
		false, 				//not synciing timer3 with other timer
	};

	TIMER_Init(TIMER2, &timerPWMInit);           // apply PWM configuration to timer2
	debugUartSendString("End Timer Init\r\n");
}

uint16_t getNextSample(Note_t * note)
{
//	//Calculate pointer in the table of sine calculations
//	// Converts sound->index  from [0, sound->period) to [0, sine_table_length)
	uint32_t ptr = note->index * returnSinSize() / note->period;
	//Calculate next index
	note->index = (note->index + 1) % note->period;
	return returnSinIndex(ptr);
}

void fillBufferWait(void)
{
	uint16_t nextDutyCycle = 0;
	uint8_t i = 0;
	uint16_t * theBuffer;
	uint16_t bufferPos = 0;
	if(bufferEmptyFlag)
	{
		//Set theb buffer pointer to the buffer that is not currently being used
		if(!isPrimaryBuffActive())
		{
			//If A is active use B
			theBuffer = &PWMTableB[0];
		}
		else
		{
			//If B is active use A
			theBuffer = &PWMTableA[0];
		}

		while (bufferPos < PWM_TABLE_SIZE)
		{
			nextDutyCycle = 0;
			//Calculate total pwm duty cycle by adding all the notes together
			for(i = 0; i < NUM_NOTES; i++)
			{
				//If samples to play calculated next duty cycle for note
				if(keyboard[i].samplesToPlay)
				{
					nextDutyCycle += getNextSample(&keyboard[i]);
					keyboard[i].samplesToPlay --;
				}

			}
			//Save duty cycle to buffer
			theBuffer[bufferPos] = nextDutyCycle;
			bufferPos++;
		}
		//Indicate the buffer is full --> This is set in the DMA callback
		bufferEmptyFlag = false;
	}
}

void fillBufferNumSamples(uint8_t numSamples)
{
//	uint16_t nextDutyCycle = 0;
//	uint8_t i = 0;
//
//	//While more samples requested and buffer isn't full --> add more samples
//	//while((numSamples) && (!circBufferFull(&dutyCycleBuffer)))
//	while(numSamples && !soundBuffer.full());
//	{
//		for(i = 0; i < NUM_NOTES; i++)
//		{
//			nextDutyCycle+= (keyboard[i].play ? 0 : getNextSample(&keyboard[i]));
//		}
//		//circBufferPut(&dutyCycleBuffer, nextDutyCycle);	//Add combined duty cycle to buffer
//		soundBuffer.put(nextDutyCycle);
//		numSamples--; //decrement number of samples
//		debugUartSendChar('p');
//		debugUartSendChar(numSamples + 65);
//	}
//

}

void playNote(uint8_t note, uint32_t num_samples)
{
	if(note >= 0 && note <= NUM_NOTES)
	keyboard[note].samplesToPlay = num_samples;
}
