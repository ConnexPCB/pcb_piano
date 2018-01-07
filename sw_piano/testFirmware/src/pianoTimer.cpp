/*
 * pianoTimer.c
 *
 *  Created on: Jan 1, 2018
 *      Author: birdman
 */

#include "pianoTimer.h"

//TIMER ISR executes every ms
void TIMER2_IRQHandler(void)
{
	//debugUartSendChar('a');
	TIMER_IntClear(TIMER2, TIMER_IF_OF);
	TIMER_IntClear(TIMER2, TIMER_IF_CC2 );
	updateDutyCycle();

}

#define PWM_FREQ  (40000) //Period for 40kHz pwm carrier frequency
////NOTE: THIS DOES NOT CHANGE ANYTHING////
#define SOUND_BUFFER_SIZE	255 //NUmber of buffer samples in the circular buffer
#define NUM_NOTES 5		//Number of notes on the keyboard. NOTE: Need to adjust the sin table as well

static circularBuffer<uint16_t> soundBuffer(10);

static Note_t keyboard[NUM_NOTES];

void initKeyboardSound(void)
{
	static circularBuffer<uint16_t> souasdndBuffer(10);

	//debugUartSendString("Begining Keyboard Init\n");
	initTimer2();	//Start Timer2 channel 2 for PWM
	//debugUartSendChar('T');
	uint8_t i = 0;
	double freq = 0;

	for(i = 0; i <= NUM_NOTES; i++)
	{
		debugUartSendChar('n');
		//freq = 440.0 * pow(2,((i-9)/12));
		initNotes(&keyboard[i], freq);
		debugUartSendChar('N');
		//TODO: Add uart print line for debug
	}

}

void initNotes(Note_t * note, double freq)
{
	note->index = 0;
	note->period = (uint16_t) (PWM_FREQ / freq); //Calculate number of pwm samples per signal period
	note->play	= false;
}

void initTimer1(void)
{
//	CMU_ClockEnable(cmuClock_TIMER1, true);
//	GPIO_PinModeSet(PIEZO_PORT, PIEZO_PIN, gpioModePushPull, 0); //Set output
//	TIMER_InitCC_TypeDef timerCCInit =
//	{
//		timerEventEveryEdge,	//value ignored -- note CC
//		timerEdgeNone,		//value ignore -- not CC
//		timerPRSSELCh0,		//value ignored -- not using PRS
//		timerOutputActionNone,	//no action on underflow
//		timerOutputActionSet,		//On overflow, output goes high
//		timerOutputActionClear, 	//On compare, output clears
//		timerCCModePWM,			//Set timer to run in PWM mode
//		false,				//Not using input -- no filter
//		false,				//Not using PRS
//		false,				//initial state for pwm is high when timer is enabled
//		false,				//non-inverted output
//	};
////	{
////			.eventCtrl	= timerEventEveryEdge,	//value ignored -- note CC
////			.edge		= timerEdgeNone,		//value ignore -- not CC
////			.prsSel		= timerPRSSELCh0,		//value ignored -- not using PRS
////			.cufoa		= timerOutputActionNone,	//no action on underflow
////			.cofoa		= timerOutputActionSet,		//On overflow, output goes high
////			.cmoa		= timerOutputActionClear, 	//On compare, output clears
////			.mode		= timerCCModePWM,			//Set timer to run in PWM mode
////			.filter		= false,				//Not using input -- no filter
////			.prsInput	= false,				//Not using PRS
////			.coist		= false,				//initial state for pwm is high when timer is enabled
////			.outInvert	= false,				//non-inverted output
////	};
//
//	//Setup timer configuration for PWM
//		TIMER_Init_TypeDef	timerPWMInit =
//		{
//				true,					//Start timer upon configuration
//				true,				//Run timer in debug mode
//				timerPrescale1,	//set prescale to /1
//				timerClkSelHFPerClk, 	//Set clock source to HFPERCLK
//				false,
//				false,
//				timerInputActionNone, //No action from inputs
//				timerInputActionNone, //No action from inputs
//				timerModeUp,			//Not using up-count mode
//				false,				//not using dma
//				false,				//Not using quad dec. mode
//				false,				//not using one shot mode
//				false, 				//not synciing timer3 with other timer
//		};
////		{
////				.enable = true,					//Start timer upon configuration
////				.debugRun	= true,				//Run timer in debug mode
////				.prescale	= timerPrescale1,	//set prescale to /1
////				.clkSel	= timerClkSelHFPerClk, 	//Set clock source to HFPERCLK
////				.fallAction	= timerInputActionNone, //No action from inputs
////				.riseAction	= timerInputActionNone, //No action from inputs
////				.mode		= timerModeUp,			//Not using up-count mode
////				.dmaClrAct	= false,				//not using dma
////				.quadModeX4	= false,				//Not using quad dec. mode
////				.oneShot	= false,				//not using one shot mode
////				.sync		= false, 				//not synciing timer3 with other timer
////		};
//
//
//
//
//	TIMER_TopSet(TIMER1, TOP_VAL_PWM);           // PWM period will be 1ms = 1kHz freq
//
//
//	TIMER_CounterSet(TIMER1, 0);                 // start counter at 0 (up-count mode)
//
//	TIMER_CompareSet(TIMER1, 0, TOP_VAL_PWM / 2);    // set CC0 compare value (0% duty)
//	TIMER_CompareBufSet(TIMER1, 0, TOP_VAL_PWM / 2); // set CC0 compare buffer value (0% duty)
//
//
//	TIMER_InitCC(TIMER1, 2, &timerCCInit);       // apply channel configuration to Timer1 channel 2 (PE13)
//	TIMER1->ROUTE = (3 << 16) |(1 << 2);         // connect PWM output (timer1, channel 2, Location 3) to PE13 (LED0). See EFM32ZG222 datasheet for details.
//
//	TIMER_IntEnable(TIMER1, TIMER_IF_CC0);        // enable Timer0 overflow interrupt
//	NVIC_EnableIRQ(TIMER1_IRQn);                 // enable Timer0 interrupt vector in NVIC
//
//	TIMER_Init(TIMER1, &timerPWMInit);           // apply PWM configuration to timer1

}

void initTimer2(void)
{
	CMU_ClockEnable(cmuClock_TIMER2, true);
	GPIO_PinModeSet(PIEZO_PORT, PIEZO_PIN, gpioModePushPull, 0); //Set output
	TIMER_InitCC_TypeDef timerCCInit =
	{
		timerEventEveryEdge,	//value ignored -- note CC
		timerEdgeNone,		//value ignore -- not CC
		timerPRSSELCh0,		//value ignored -- not using PRS
		timerOutputActionNone,	//no action on underflow
		timerOutputActionSet,		//On overflow, output goes high
		timerOutputActionClear, 	//On compare, output clears
		timerCCModePWM,			//Set timer to run in PWM mode
		false,				//Not using input -- no filter
		false,				//Not using PRS
		false,				//initial state for pwm is high when timer is enabled
		false,				//non-inverted output
	};

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



	debugUartSendChar('s');
	TIMER_TopSet(TIMER2, TOP_VAL_PWM);           // PWM period will be 1ms = 1kHz freq


	TIMER_CounterSet(TIMER2, 0);                 // start counter at 0 (up-count mode)

	TIMER_CompareSet(TIMER2, 2, TOP_VAL_PWM/2);    // set CC0 compare value (0% duty)
	TIMER_CompareBufSet(TIMER2, 2, TOP_VAL_PWM/2); // set CC0 compare buffer value (0% duty)

	debugUartSendChar('S');
	TIMER_InitCC(TIMER2, 2, &timerCCInit);       // apply channel configuration to Timer2 channel 2 (PE13)
	TIMER2->ROUTE = (3 << 16) |(1 << 2);         // connect PWM output (timer2, channel 2, Location 3) to PE13 (LED0). See  datasheet for details.

	TIMER_IntEnable(TIMER2, TIMER_IF_CC2);        // enable Timer2 overflow interrupt
	NVIC_EnableIRQ(TIMER2_IRQn);                 // enable Timer2 interrupt vector in NVIC
	debugUartSendChar('i');
	TIMER_Init(TIMER2, &timerPWMInit);           // apply PWM configuration to timer2
	debugUartSendChar('I');
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
	//if(!circBufferEmpty(&dutyCycleBuffer))
	//{
		//uint16_t newData;
		//circBufferGet(&dutyCycleBuffer, &newData);
		//if(!soundBuffer.empty())
		{
			//TIMER_CompareBufSet(TIMER2, 2, soundBuffer.get());
		}
			//	debugUartSendChar('1');
	//}
	//else
	//{
		//TIMER_CompareSet(TIMER2, 2, 0);
	//	debugUartSendChar('2');
	//}
}



uint16_t getNextSample(Note_t * note)
{
//	//Calculate pointer in the table of sine calculations
//	// Converts sound->index  from [0, sound->period) to [0, sine_table_length)
//	uint32_t ptr = note->index * sineTableLength / note->period;
//
//	//Calculate next index
//	note->index = (note->index + 1) % note->period;
//
//	return sineTable[ptr];
}

void fillBufferWait()
{
//	uint16_t nextDutyCycle = 0;
//	uint8_t i = 0;
//
//	//while(!circBufferFull(&dutyCycleBuffer))
//	while(!soundBuffer.full())
//	{
//		for(i = 0; i < NUM_NOTES; i++)
//		{
//			nextDutyCycle+= (keyboard[i].play ? getNextSample(&keyboard[i]) : 0);
//		}
//		//circBufferPut(&dutyCycleBuffer, nextDutyCycle);
//		soundBuffer.put(nextDutyCycle);
//		debugUartSendChar('p');
//	}


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

void playNote(uint8_t note, uint8_t num_samples)
{
	keyboard[note].play = true;
	fillBufferNumSamples(10);
	keyboard[note].play = false;
}
