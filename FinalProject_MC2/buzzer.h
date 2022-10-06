/*
 * buzzer.h
 *
 *  Created on: Jul 8, 2022
 *      Author: lenovo
 */

#ifndef BUZZER_H_
#define BUZZER_H_


#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID       	PORTB_ID
#define BUZZER_PIN_ID           PIN0_ID


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void BUZZER_init(void);
void BUZZER_on(void);
void BUZZER_off(void);



#endif /* BUZZER_H_ */
