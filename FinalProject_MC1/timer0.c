/*
 * timer0.c
 *
 *  Created on: Jul 6, 2022
 *      Author: lenovo
 */

#include "timer0.h"
#include "avr/io.h"
#include <avr/interrupt.h>/* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                      Interrupt Service Routine                               *
 *******************************************************************************/

ISR(TIMER0_COMP_vect){

	if (g_callBackPtr!=NULL_PTR){
		(*g_callBackPtr)();
	}

}


ISR(TIMER0_OVF_vect){

	if (g_callBackPtr!=NULL_PTR){
		(*g_callBackPtr)();
	}


}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer0_init(const Timer0_ConfigType *Config_Ptr){

	/*set the initialize value of the timer*/
	TCNT0=Config_Ptr->intialize_value;
	/*
	 * Active FOCO bit to specify non-PWM mode
	 * set the Wave Form Generation Mode
	 * set the COM01 COM00  that controls the OC0 behavior
	 * Clock Select
	 * */
	TCCR0=(1<<FOC0)| (0xFF & (Config_Ptr->mode <<3)) | (0xFF & (Config_Ptr->mode<<4))  | (0xFF & (Config_Ptr->mode<<5)) | (0xFF & (Config_Ptr->clock));
	if(Config_Ptr->mode==NORMAL){
		/*enable the interrupt for the overflow mode*/
		TIMSK|=(1<<TOIE0);
	}
	else if(Config_Ptr->mode==CTC){
		/*enable the interrupt for the compare mode*/
		TIMSK|=(1<<OCIE0);
		OCR0=Config_Ptr->compare_value;
	}
}

void Timer0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void Timer0_Deinit(void){
	/*Clear the value*/
	TCNT0=0;
	OCR0=0;

	/*Stop the clock*/
	TCCR0 = (TCCR0 & 0xF8);

	/*Disable the interrupt for the both mode*/
	TIMSK = (TIMSK & 0xFC);
}





