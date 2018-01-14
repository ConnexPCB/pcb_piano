/*
 * pianoTimer.c
 *
 *  Created on: Jan 1, 2018
 *      Author: birdman
 */

#include "pianoTimer.h"



#define PWM_FREQ  (20000) //Period for 40kHz pwm carrier frequency
////NOTE: THIS DOES NOT CHANGE ANYTHING////
#define SOUND_BUFFER_SIZE	2048 //NUmber of buffer samples in the circular buffer
#define NUM_NOTES 12		//Number of notes on the keyboard. NOTE: Need to adjust the sin table as well

static circularBuffer<uint16_t> soundBuffer(SOUND_BUFFER_SIZE);

#define PWM_TABLE_SIZE           200

uint16_t PWMTableA[PWM_TABLE_SIZE];
uint16_t PWMTableB[PWM_TABLE_SIZE];

static bool bufferEmptyFlag = true; //Flag to signal when buffer is empty of full

static Note_t keyboard[NUM_NOTES];

//TIMER ISR executes every ms
//void TIMER2_IRQHandler(void)
//{
//	//debugUartSendChar('a');
//	TIMER_IntClear(TIMER2, TIMER_IF_OF);
//	//TIMER_IntClear(TIMER2, TIMER_IF_CC2 );
//	//if(!soundBuffer.empty())
//		//if(1)
//		//{
//			//duty = soundBuffer.get();
//			//if(duty > 750)
//			//duty =
//			//if(duty < 750)
//			//{
//		//static uint16_t duty = 0;
//		//duty = soundBuffer.get();
//		//if(duty < 500)
//		//{//debugUartSendChar('+');
//	if(!soundBuffer.empty())
//	{
//			TIMER_CompareBufSet(TIMER2, 2, soundBuffer.get() );
//
//	}
//	else
//	{
//		//debugUartSendChar('/');
//		TIMER_CompareBufSet(TIMER2, 2, 10);
//	}
////		}
////		//else
////		{
////			//TIMER_CompareBufSet(TIMER2, 2, 325);
////		}
////
////	//			duty += 10;
////	//			if(duty > 150) duty = 0;
////			//}
//
////	//		else
////	//		{
////	//			TIMER_CompareBufSet(TIMER2, 2, 0);
////	//		}
////					//debugUartSendChar('1');
////		}
////		//else
////		{
////			//TIMER_CompareBufSet(TIMER2, 2, 0);
////			//debugUartSendChar('2');
////		}
//
//
//}

void initKeyboardSound(void)
{
	debugUartSendString("\n\rInitializing Keyboard\n\r");

	//static circularBuffer<uint16_t> soundBuffer(10);

	//debugUartSendString("Begining Keyboard Init\n");
	initTimer2(PWM_FREQ);	//Start Timer2 channel 2 for PWM
	//debugUartSendChar('T');
	uint8_t i = 0;
	double freq = 0;
	double power = 0;
	//Calculating the notes and frequencies
	for(i = 0; i <= NUM_NOTES; i++)
	{
		debugUartSendString("Note: ");
		debugUartSendChar((char)(i + 65));
		debugUartSendString("\n\r");
		power = (double)((i - 9.0)/ 12);
		freq = 440.0 * std::pow(2, power);
		//initNotes(&keyboard[i], freq);
		//TODO: Add uart print line for debug
		keyboard[i].index = 0;
		keyboard[i].period = (uint16_t) (PWM_FREQ / freq); //Calculate number of pwm samples per signal period
	}

//	keyboard[0].index = 0;
//	keyboard[0].period = 100;
//	keyboard[0].countUp = true;
//	keyboard[0].mulitplier = 5;
//	keyboard[0].nextDutyCycle = 75;
//	keyboard[0].samplesToPlay = 0;
//
//	keyboard[1].index = 0;
//	keyboard[1].period = 62;
//	keyboard[1].countUp = true;
//	keyboard[1].mulitplier = 8;
//	keyboard[1].nextDutyCycle = 75;
//	keyboard[1].samplesToPlay = 0;
//
//	keyboard[2].index = 0;
//	keyboard[2].period = 31;
//	keyboard[2].countUp = true;
//	keyboard[2].mulitplier = 12;
//	keyboard[2].nextDutyCycle = 75;
//	keyboard[2].samplesToPlay = 0;
//
//	keyboard[3].index = 0;
//	keyboard[3].period = 15;
//	keyboard[3].countUp = true;
//	keyboard[3].mulitplier = 15;
//	keyboard[3].nextDutyCycle = 75;
//	keyboard[3].samplesToPlay = 0;
//
//	keyboard[4].index = 0;
//	keyboard[4].period = 9;
//	keyboard[4].countUp = true;
//	keyboard[4].mulitplier = 20;
//	keyboard[4].nextDutyCycle = 75;
//	keyboard[4].samplesToPlay = 0;
}

void initNotes(Note_t * note, double freq)
{

	//note->play	= false;
	//debugUartSendUint16(note->period);
	//debugUartSendString("\n\r");
}

void initTimer2(uint16_t freq)
{
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
	topValue = CMU_ClockFreqGet(cmuClock_HFPER)/freq;
	//topValue = 750;
	//topValue = 750;
	TIMER_TopSet(TIMER2, topValue);

	debugUartSendString("Top Val: ");
	debugUartSendUint16((uint16_t)topValue);
	debugUartSendString("\r\n");


	//Set compare value starting at PWMTableA
	TIMER_CompareSet(TIMER2, 2, PWMTableA[0]);

	debugUartSendChar('s');

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

		//TIMER_Reset(TIMER2);


//	TIMER_CounterSet(TIMER2, 0);                 // start counter at 0 (up-count mode)
//
//	TIMER_CompareSet(TIMER2, 2, TOP_VAL_PWM/2);    // set CC0 compare value (0% duty)
//	TIMER_CompareBufSet(TIMER2, 2, TOP_VAL_PWM/2); // set CC0 compare buffer value (0% duty)

	//debugUartSendChar('S');


	//TIMER_IntEnable(TIMER2, TIMER_IF_OF);        // enable Timer2 overflow interrupt
	//TIMER_IntEnable(TIMER2, TIMER_IEN_ICBOF2_DEFAULT);
	//NVIC_EnableIRQ(TIMER2_IRQn);                 // enable Timer2 interrupt vector in NVIC
	//debugUartSendChar('i');
	TIMER_Init(TIMER2, &timerPWMInit);           // apply PWM configuration to timer2
	debugUartSendString("End Timer Init\r\n");
}


void updateDutyCycle()
{
//	//Set the new capture compare values to TIMER1
//	TIMER_CompareSet(TIMER1, 0, ccValues_440HZ[ccValueBufferPtr]);    // set CC0 compare value (0% duty)
//	TIMER_CompareBufSet(TIMER1, 0, ccValues_440HZ[ccValueBufferPtr]); // set CC0 compare buffer value (0% duty)
//	if(ccValueBufferPtr >= SAMPLE_LENGTH) //If we are at the end of our
//	{
//		ccValueBufferPtr = 0;
//	}
//	else
//	{
//		ccValueBufferPtr++;
//	}
	static uint16_t duty = 0;
	//static uint6_t duty2 = 0;
	if(!soundBuffer.empty())
	//if(1)
	{
		//duty = soundBuffer.get();
		//if(duty > 750)
		//duty =
		//if(duty < 750)
		//{
			//debugUartSendChar('+');
			TIMER_CompareBufSet(TIMER2, 2, soundBuffer.get());
//			duty += 10;
//			if(duty > 150) duty = 0;
		//}
//		else
//		{
//			//debugUartSendChar('/');
//			TIMER_CompareBufSet(TIMER2, 2, duty);
//		}
//		else
//		{
//			TIMER_CompareBufSet(TIMER2, 2, 0);
//		}
				//debugUartSendChar('1');
	}
	else
	{
		TIMER_CompareBufSet(TIMER2, 2, 740);
		//debugUartSendChar('2');
	}
}



uint16_t getNextSample(Note_t * note)
{
//	//Calculate pointer in the table of sine calculations
//	// Converts sound->index  from [0, sound->period) to [0, sine_table_length)
	uint32_t ptr = note->index * returnSinSize() / note->period;
	//Calculate next index
	note->index = (note->index + 1) % note->period;
	return returnSinIndex(ptr);
//	//return 375;
//	if(note->countUp)
//	{
		note->nextDutyCycle += note->mulitplier;
		if(note->nextDutyCycle > 150)
		{
			//note->countUp = false;
			note->nextDutyCycle = 0;

		}
		return note->nextDutyCycle;
		//return 350;
//		else
//		{
//			return 0;
//		}
//	}
//	else
//	{
//		note->nextDutyCycle -= note->mulitplier;
//		if(note->nextDutyCycle < 10)
//		{
//			note->countUp = true;
//			note->nextDutyCycle = 10;
//		}
//	}

}

void fillBufferWait(void)
{
	uint16_t nextDutyCycle = 0;
	uint8_t i = 0;
	uint16_t * theBuffer;
	//debugUartSendString("Filling Duty Cycle Buffer\n\r");
	//while(!circBufferFull(&dutyCycleBuffer))
//	while(!soundBuffer.full())
//	{
//		for(i = 0; i < NUM_NOTES; i++)
//		{
//			if(keyboard[i].samplesToPlay)
//			{
//				nextDutyCycle += getNextSample(&keyboard[i]);
//				keyboard[i].samplesToPlay --; //Decrement the number of samples needed
//
////				if(!keyboard[i].samplesToPlay) //if no more samples
////				{
////					keyboard[i].play = false;
////				}
//			}
//			else
//			{
//				//Do nothing
//			}
//
//
//
//		}

//		if(keyboard[1].samplesToPlay)
//		{
//			nextDutyCycle = getNextSample(&keyboard[1]);
//			//nextDutyCycle = 300;
//			keyboard[1].samplesToPlay --; //Decrement the number of samples needed
//
////						if(!keyboard[i].samplesToPlay) //if no more samples
////						{
////							keyboard[i].play = false;
////						}
//
//		}			//keyboard[i].sa
//		else{
//			nextDutyCycle = 0;
//		}
//		//nextDutyCycle = 375;
//		//circBufferPut(&dutyCycleBuffer, nextDutyCycle);
//		soundBuffer.put(nextDutyCycle); //Add next duty cycle to buffer
//		//debugUartSendString("Next Duty Cycle\n\r");
//	}

//////////////////////////DMA TESTING///////////////////////////



	uint16_t bufferPos = 0;
	if(bufferEmptyFlag)
	{
		//Set theb buffer pointer

		if(!isPrimaryBuffActive())
		{
			//debugUartSendChar('A');
			//PWMTableB[bufferPos] = 600;
			//PWMTableB[bufferPos] = nextDutyCycle;
			theBuffer = &PWMTableB[0];
		}
		else
		{
			//debugUartSendChar('B');
			//PWMTableA[bufferPos] = 100;
			//PWMTableA[bufferPos] = nextDutyCycle;
			theBuffer = &PWMTableA[0];
		}
		//debugUartSendChar('C');
//		if(keyboard[i].samplesToPlay)
//		{
//			debugUartSendChar('Y');
//		}
//		else
//		{
//			debugUartSendChar('N');
//		}

		while (bufferPos < PWM_TABLE_SIZE)
		{
			nextDutyCycle = 0;
			//Calculate total pwm duty cycle
			for(i = 0; i < NUM_NOTES; i++)
			{
				if(keyboard[i].samplesToPlay)
				{
					//debugUartSendChar('s');
					nextDutyCycle += getNextSample(&keyboard[i]);
					//nextDutyCycle = 350;
					keyboard[i].samplesToPlay --;
					//debugUartSendChar( i + 38);
				}
//				else
//				{
//					//nextDutyCycle = 0;
//				}
			}
			//Check for current active buffer -- fill the other buffer
			theBuffer[bufferPos] = nextDutyCycle;
			bufferPos++;
		}
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
	keyboard[note].play = true;
	keyboard[note].samplesToPlay = num_samples;
}
