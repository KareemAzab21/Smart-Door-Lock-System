/*
 * timer0.h
 *
 *  Created on: Jul 6, 2022
 *      Author: lenovo
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#include "std_types.h"



/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	NORMAL,CTC
}Timer0_Mode;

typedef enum{
	NO_CLOCK,CLK_1,CLK_8,CLK_64,CLK_256,CLK_1024
}Timer0_Clock;

typedef struct{
	Timer0_Mode mode;
	Timer0_Clock clock;
	uint8 intialize_value;
	uint8 compare_value;
}Timer0_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Initialize the timer0 with the configured mode.
 */
void Timer0_init(const Timer0_ConfigType *Config_Ptr);


/*
 * Description: Disable Timer0.
 */
void Timer0_Deinit(void);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void));



#endif /* TIMER0_H_ */
