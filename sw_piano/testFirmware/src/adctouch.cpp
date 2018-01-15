/*
 * adctouch.cpp
 *
 *  Created on: Jan 14, 2018
 *      Author: birdman
 */

#include "adctouch.h"
volatile static bool newVal = false;
volatile static uint16_t theNewCCVal = 0;
static uint16_t capTouchRawBuffer[5];
static bool isButtonPressed[5];
static uint8_t rawBufferIndex = 0;


void TIMER1_IRQHandler(void)
{
	if(TIMER1->IF & TIMER_IF_CC0)
	{
		TIMER_IntClear(TIMER1, TIMER_IFC_CC0);
		//GPIO_PinOutToggle(gpioPortF, 2);
		//newVal = true;
		//theNewCCVal = TIMER1->CC[0].CCV;
		capTouchRawBuffer[rawBufferIndex] = TIMER1->CC[0].CCV;
		rawBufferIndex = (rawBufferIndex + 1) % 5;
		//ACMP_CapsenseChannelSet(ACMP0, (acmpChannel0 + rawBufferIndex));
		ACMP0->INPUTSEL = (ACMP0->INPUTSEL & ~(0x07)) | rawBufferIndex;
		//debugUartSendUint16(theNewCCVal);
	}
//	if(TIMER1->IF & TIMER_IF_OF)
//		{
//			TIMER_IntClear(TIMER1, TIMER_IF_OF);
//			GPIO_PinOutToggle(gpioPortF, 3);
//	//		newVal = true;
//	//		theNewCCVal = TIMER1->CC[0].CCV;
//		}



}
/////NOTE: ACMP_OUT IS ON PD6 THIS IS I2C PORT !!!////
void setupCapTouch(void)
{
	debugUartSendString("Initializing Cap Touch\r\n");
	//Enable clock sources for associated peripherals
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_ACMP0, true);
	CMU_ClockEnable(cmuClock_PRS, true);
	CMU_ClockEnable(cmuClock_TIMER1, true);


	//Use default Cap Sense Init
	ACMP_CapsenseInit_TypeDef capsenseInit = ACMP_CAPSENSE_INIT_DEFAULT;
	ACMP_CapsenseInit(ACMP0, &capsenseInit);

	//Setup pc8 / ch0 for CT
	ACMP_CapsenseChannelSet(ACMP0, acmpChannel0);

	//Enable ACMP interrupt
	ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
	ACMP0->CTRL = ACMP0->CTRL | ACMP_CTRL_IRISE_ENABLED; //Set interrupt for rising edge

	debugUartSendString("ACMP Warm Up\r\n");
	//Wait for ACMP to warm up
	while(!(ACMP0->STATUS & ACMP_STATUS_ACMPACT));

	setupTimer0Ch0();
	setupTimer1Ch1();

//	//Setup PRS from ACMP0
	PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_ACMP0, PRS_CH_CTRL_SIGSEL_ACMP0OUT, prsEdgePos);

	GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 1);
	//PRS->ROUTE |= PRS_ROUTE_CH0PEN;
	/////NOTE: THIS IS I2C PORT REMOVE!!!////
	GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
	//ACMP_GPIOSetup(ACMP0, 2, true, false); //Set ACMP_Output to Location 2 (PD6)
	debugUartSendString("End Cap Touch Init\r\n");
}

void setupTimer0Ch0(void)
{

	/*******
	 * Setup CH1 of Timer 0 to be the periodic sample signal
	 * Goal: Toggle output every 100ms on PRS Ch 1 from Timer 0
	 * Ch 0. The output will be used to start a new count
	 * on Timer 2 Ch 0;
	 */
	TIMER_InitCC_TypeDef timerCCInit =
	{
		timerEventEveryEdge,	//value ignored
		timerEdgeBoth,		//value ignore
		timerPRSSELCh0,		//value ignored -- not using PRS
		timerOutputActionNone,	//no action on underflow
		timerOutputActionNone,		//On overflow, output goes high
		timerOutputActionToggle, 	//On compare, output toggles
		timerCCModeCompare,			//Set timer to run in Compare Mode
		false,				//Not using input -- no filter
		false,				//Not using PRS
		false,				//initial state for pwm is high when timer is enabled
		false,				//non-inverted output
	};

	//TIMER_CompareSet(TIMER0, 0, 32000);
	TIMER0->CTRL |= TIMER_CC_CTRL_PRSCONF_LEVEL; //Set PRS output to match CC Out (Toggle on compare)
	//TIMER0->IEN |= TIMER_IEN_CC0;
	//TIMER_IntEnable(TIMER0, TIMER_IF_CC0);
	TIMER_InitCC(TIMER0, 0, &timerCCInit);
	//Setup PRS from TIMER0
	PRS_SourceSignalSet(1, PRS_CH_CTRL_SOURCESEL_TIMER0, PRS_CH_CTRL_SIGSEL_TIMER0CC0, prsEdgePos);


}

void setupTimer1Ch1(void)
{
	/* Use TIMER1 Ch1 as the input clock to the module.
	 * Use TIMER1 Ch0 as the capture channel
	 *
	 * Goal: Set up Timer1 Ch1 to be clocked off PRS Ch0 from
	 * the ACMP0 module. This will be the clock for Timer 1 counter
	 * unit. Ch1 is not used otherwise
	 *
	 * Set up Timer1 Ch0 to have an input from PRS Ch1 from Timer0 Ch0
	 * A new Capture event is on every edge. (Eventually) Triggers DMA
	 * to a buffer of values. This counts the oscillations from ACMP0
	 * in order to measure the capacitive touch input.
	 * On every edge, the timer is reloaded and started.
	 *
	 */


	//Set up TIMER1 Ch1 to Accept PRS signal from ACMP0
	//This is the clock for Timer 1
	TIMER_InitCC_TypeDef timer1_cc1_settings = TIMER_INITCC_DEFAULT;
	timer1_cc1_settings.mode = timerCCModeCompare;
	timer1_cc1_settings.prsInput = true;	//Prs input from PRS0 -> ACMPOUT
	timer1_cc1_settings.prsSel = timerPRSSELCh0;	//Prs input from PRS0 -> ACMPOUT
	timer1_cc1_settings.eventCtrl = timerEventRising; //Controls when interrupt is sent -> This is not used
	timer1_cc1_settings.edge = timerEdgeBoth;		//Trigger on both edges from PRS
	TIMER_InitCC(TIMER1, 1, &timer1_cc1_settings);
	TIMER_CompareSet(TIMER1, 1, 500);

	//Set up TIMER1 Ch0 to Accept PRS signal from Timer0 to count
	//oscillations per period that PRS is High
	TIMER_InitCC_TypeDef timer1_cc0_settings = TIMER_INITCC_DEFAULT;
	timer1_cc0_settings.mode = timerCCModeCapture;
	timer1_cc0_settings.prsInput = true;	//PRS input from PRS1 -> Timer0
	timer1_cc0_settings.prsSel = timerPRSSELCh1;	//PRS input from PRS1 -> Timer0
	timer1_cc0_settings.eventCtrl = timerEventEveryEdge;	//Trigger when Interupt is called
	timer1_cc0_settings.edge = timerEdgeRising;	//Trigger on both edges (Timer 1 toggles on compare)
	TIMER_InitCC(TIMER1, 0, &timer1_cc0_settings);
	//TIMER_CompareSet(TIMER1, 0, 0xfff);

	TIMER_IntEnable(TIMER1, TIMER_IEN_CC0);
	TIMER_IntEnable(TIMER1, TIMER_IEN_OF);
	NVIC_EnableIRQ(TIMER1_IRQn);

	TIMER_Init_TypeDef timer_settings = TIMER_INIT_DEFAULT;
	timer_settings.clkSel = timerClkSelCC1;	//Use CC1 as the clock
	timer_settings.prescale = timerPrescale1; //TODO: Veify why this is so high
	timer_settings.riseAction = timerInputActionNone; //Reload and start timer when Ch0 has rising edge
	timer_settings.fallAction = timerInputActionReloadStart; //Reload and start timer when Ch0 has falling edge
	TIMER_Init(TIMER1, &timer_settings);
	TIMER1->TOP = 3000;

}

uint16_t getNewVal(void)
{
	if(newVal)
	{
		newVal = false;
		if(theNewCCVal)//debugUartSendUint16(theNewCCVal);

		return theNewCCVal;

	}
	else
	{
		return 0;
	}
}

void updateButtons(void)
{
	uint8_t i = 0;
	for(i = 0; i < 5; i++)
	{
		if(capTouchRawBuffer[i] < 100)
		{
			isButtonPressed[i] = true;
		}
		else
		{
			isButtonPressed[i] = false;
		}
	}
}

bool getButton(uint8_t btn)
{
	return isButtonPressed[btn];
}
