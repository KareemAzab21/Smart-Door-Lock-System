/*
 * dcmotor.h
 *
 *  Created on: Jul 8, 2022
 *      Author: lenovo
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* The Ports and Pins IDs for the DC-Motor two pins */
#define DC_MOTOR_PORT1_ID   PORTA_ID
#define DC_MOTOR_PORT2_ID   PORTA_ID

#define DC_MOTOR_PIN1_ID    PIN1_ID
#define DC_MOTOR_PIN2_ID    PIN0_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	DC_MOTOR_STOP,DC_MOTOR_CW,DC_MOTOR_ACW
}DcMotor_State;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * Initialize the DC Motor by:
 * 1. Setup the direction of the two motor pins as output by send the request to GPIO driver.
 * 2. Stop the motor at the beginning
 */
void DcMotor_Init(void);

/*
 * Description :
 * 1. Rotate  or Stop the motor according to the state input variable.
 * 2. Control the motor speed 0 --> 100% from its maximum speed by sending to PWM driver.
 */
void DcMotor_Rotate(DcMotor_State state);

#endif /* DCMOTOR_H_ */
