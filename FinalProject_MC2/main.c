/*
 * MC2.C
 *
 *  Created on: Jul 7, 2022
 *      Author: lenovo
 */

#include "avr/io.h"
#include "external_eeprom.h"
#include "uart.h"
#include "timer0.h"
#include "buzzer.h"
#include "dcmotor.h"
#include <util/delay.h>



/*******************************************************************************
 *                                  Definitions                                *
 *******************************************************************************/

#define M1_READY 			0x10
#define M2_READY 			0x20
#define Error_pass 			0x00
#define SAVE 				0x02
#define END_SAVING 			0x03
#define CHECK_PASS 			0x04
#define CONTINUE_PASS 		0x05
#define UNLOCK_DOOR 		0x06
#define INITIALIZED_FLAG 	0x01

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint16 g_tick = 0;
uint8 g_calc_sec_flag=0;

/*******************************************************************************
 *                            FUNCTIONS DEFINITIONS                            *
 *******************************************************************************/


void Calculate_time(void)
{
	g_tick ++;

	if(g_tick == 30)
	{
		g_calc_sec_flag ++;
		g_tick =0;
	}
}

void Recieve_Password(void)
{
	while(UART_recieveByte()!= SAVE);
	uint8 pass,count_pass;
	for(count_pass=4; count_pass>0; count_pass--)
	{

		UART_sendByte(M2_READY);
		pass = UART_recieveByte();
		EEPROM_writeByte((0x0311+count_pass), pass);
		_delay_ms(70);
	}

	EEPROM_writeByte(0x0420, 0x01);
	_delay_ms(70);
	UART_sendByte(END_SAVING);
}
void Check_Password(const Timer0_ConfigType * Config_Ptr)
{
	while(UART_recieveByte()!= CHECK_PASS);

	uint8 pass=0,pass_saved=0,error,count_pass;
	for(count_pass=4; count_pass>0; count_pass--)
	{
		pass = UART_recieveByte();

		EEPROM_readByte((0x0311+count_pass), &pass_saved);
		if(pass != pass_saved)
		{
			UART_sendByte(Error_pass);
		}
		else
		{

			UART_sendByte(CONTINUE_PASS);
		}
	}
	error = UART_recieveByte();
	if(error < 3)
	{
		Check_Password(Config_Ptr);
	}
	else if(error == 3)
	{
		Timer0_init(Config_Ptr);
		g_tick =0;
		g_calc_sec_flag =0;
		while (g_calc_sec_flag != 60)
		{
			BUZZER_on();
		}
	}
}

void Open_door(const Timer0_ConfigType * Config_Ptr)
{
	while(UART_recieveByte()!= UNLOCK_DOOR);

	Timer0_init(Config_Ptr);

	g_tick =0;
	g_calc_sec_flag =0;
	while(g_calc_sec_flag != 5)
	{
		DcMotor_Rotate(DC_MOTOR_CW);
	}


	g_tick =0;
	g_calc_sec_flag =0;

	while(g_calc_sec_flag != 3)
	{
		DcMotor_Rotate(DC_MOTOR_STOP);
	}


	g_tick =0;
	g_calc_sec_flag =0;

	while(g_calc_sec_flag != 5)
	{
		DcMotor_Rotate(DC_MOTOR_ACW);
	}

	DcMotor_Rotate(DC_MOTOR_STOP);

}
void Main_Menue(const Timer0_ConfigType * Config_Ptr)
{

	Check_Password(Config_Ptr);

	if(UART_recieveByte() == 0)
	{
		if(UART_recieveByte() == '+')
		{

			Recieve_Password();
		}
		else
		{

			Open_door(Config_Ptr);
		}
	}

	else
	{
		BUZZER_off();
	}
}
int main(void)
{
	SREG|=(1<<7);
	uint8 flag_init =0;
	DcMotor_Init();
	BUZZER_init();

	Timer0_ConfigType Timer_Config = {NORMAL,CLK_1024,0,0};
	Timer0_setCallBack(Calculate_time);

	TWI_ConfigType TWI_Config = {F_CPU_CLOCK,0x02,0x01};
	EEPROM_init(& TWI_Config);


	UART_ConfigType UART_Config={9600,BIT_1,DISABLED,BIT_8};
	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_init(&UART_Config);


	while(1)
		{
			while(UART_recieveByte()!= M1_READY);

			EEPROM_readByte(0x0420, &flag_init);
			UART_sendByte(flag_init);


			if(flag_init != INITIALIZED_FLAG)
			{

				Recieve_Password();
			}

			else
			{

				Main_Menue(& Timer_Config);
			}
		}

}
