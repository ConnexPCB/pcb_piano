/*
 * util.c
 *
 *  Created on: Jan 1, 2018
 *      Author: birdman
 */


#include "util.h"

//TIMER ISR executes every ms
void TIMER0_IRQHandler(void)
{
	TIMER_IntClear(TIMER0, TIMER_IF_OF);
	ms_counter++;
	//debugUartSendChar('0');
	//testBufferGetSingle();
	//GPIO_PinOutToggle(gpioPortF, 2);
}

void initClock(void)
{
	CMU_HFRCOBandSet(cmuHFRCOBand_21MHz);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_TIMER0, true);

//	GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
//	CMU->CTRL &= ~(0x7 << 20); //HFRCO clock out to clock out multiplexer
//	CMU->ROUTE |= (1 << 0) | (2 << 2); //Enable clock out to CLK0 LOC2


}

void initTimer0(void)
{
	ms_counter = 0; //Clear timer counter variable

	//Setup Timer Configuration for general purpose use
	TIMER_Init_TypeDef timerGPInit =
	{
		true,                 // start timer upon configuration
		true,                 // run timer in debug mode
		timerPrescale1,       // set prescaler to /1
		timerClkSelHFPerClk,  // set clock source as HFPERCLK
		false,
		false,
		timerInputActionNone, // no action from inputs
		timerInputActionNone, // no action from inputs
		timerModeUp,          // use up-count mode
		false,                // not using DMA
		false,                // not using Quad Dec. mode
		false,                // not using one shot mode
		false                // not syncronizing timer3 with other timers
	};
//	{
//	.enable     = true,                 // start timer upon configuration
//	.debugRun   = true,                 // run timer in debug mode
//	.prescale   = timerPrescale1,       // set prescaler to /1
//	.clkSel     = timerClkSelHFPerClk,  // set clock source as HFPERCLK
//	.fallAction = timerInputActionNone, // no action from inputs
//	.riseAction = timerInputActionNone, // no action from inputs
//	.mode       = timerModeUp,          // use up-count mode
//	.dmaClrAct  = false,                // not using DMA
//	.quadModeX4 = false,                // not using Quad Dec. mode
//	.oneShot    = false,                // not using one shot mode
//	.sync       = false,                // not syncronizing timer3 with other timers
//	};

	TIMER_TopSet(TIMER0, TOP_VAL_GP_TIMER);      // GP Timer period will be 1ms = 1kHz freq
	TIMER_CounterSet(TIMER0, 0);                 // start counter at 0 (up-count mode)
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);        // enable Timer0 overflow interrupt
	NVIC_EnableIRQ(TIMER0_IRQn);                 // enable Timer0 interrupt vector in NVIC
	TIMER_Init(TIMER0, &timerGPInit);            // apply general purpose configuration to timer0

}



void initPins(void)
{


}
