/*
 * buzzer.c
 *
 *  Created on: Jul 8, 2022
 *      Author: lenovo
 */

#include "gpio.h"
#include "buzzer.h"
#include <util/delay.h>


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void BUZZER_init(void){
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}

/*
 * Description :
 * Get the Buzzer on
 */
 void BUZZER_on(void){
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
}

/*
 * Description :
 * Get the Buzzer off
 */
 void BUZZER_off(void){

	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}


