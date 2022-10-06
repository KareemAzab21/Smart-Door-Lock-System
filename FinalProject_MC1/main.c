/*
 * main.c
 *
 *  Created on: Jul 9, 2022
 *      Author: lenovo
 */


/*******************************************************************************
 *                                   INCLUDES                                  *
 *******************************************************************************/
#include "avr/io.h"
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer0.h"
#include <util/delay.h>

/*******************************************************************************
 *                                  Definitions                                *
 *******************************************************************************/

#define M1_READY			0x10
#define M2_READY 			0x20
#define Error_pass 			0x00
#define INITIALIZED_FLAG	0x01
#define SAVE 				0x02
#define END_SAVING 			0x03
#define CHECK_PASS 			0x04
#define UNLOCK_DOOR 		0x06

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

uint16 g_tick = 0;
uint8 g_calc_sec_flag=0;
uint8 counter_error =0;
uint8 return_flag=0;

void Calculate_time(void)
{
	g_tick ++;


	if(g_tick == 30)
	{
		g_calc_sec_flag ++;
		g_tick =0;
	}
}



void New_Password()
{

	uint8 key = 0,pass=0,count_pass;

	sint32 new_pass = 0, re_pass = 0;

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter new pass:");

	for(count_pass=0; count_pass<4; count_pass++)
	{
		key = KEYPAD_getPressedKey();

		while(!((key >= 0) && (key <= 9)))
		{
			key = KEYPAD_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*");
		new_pass= new_pass*10 + key;
		_delay_ms(500);
	}

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Renter pass:");

	for(count_pass=0; count_pass<4; count_pass++)
	{
		key = KEYPAD_getPressedKey();

		while(!((key >= 0) && (key <= 9)))
		{
			key = KEYPAD_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*");
		re_pass= re_pass*10 + key;
		_delay_ms(500);
	}


	if(re_pass == new_pass)
	{
		UART_sendByte(SAVE);
		for(count_pass=0;count_pass<4;count_pass++)
		{
			pass = re_pass % 10;
			while(UART_recieveByte()!= M2_READY);
			UART_sendByte(pass);
			re_pass /= 10;
		}
		while(UART_recieveByte()!= END_SAVING);
	}
	else
	{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Missmatch!");
		_delay_ms(500);
		New_Password();
	}
}

void Check_Password(const Timer0_ConfigType * Config_Ptr)
{
	sint32 pass=0;

	uint8 count_pass,key=0,pass_digit=0,error_pass=0;

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter pass:");

	for(count_pass=0; count_pass<4; count_pass++)
	{
		key = KEYPAD_getPressedKey();

		while(!((key >= 0) && (key <= 9)))
		{
			key = KEYPAD_getPressedKey();
		}
		LCD_displayStringRowColumn(1,count_pass,"*");
		pass= pass*10 + key;
		_delay_ms(500);
	}

	UART_sendByte(CHECK_PASS);

	for(count_pass=0; count_pass<4; count_pass++)
	{
		pass_digit = pass % 10;
		UART_sendByte(pass_digit);

		if(UART_recieveByte() == Error_pass)
		{
			error_pass ++;
		}
		pass /= 10;
	}

	if(error_pass != 0)
	{
		counter_error ++;
		UART_sendByte(counter_error);

		if(counter_error < 3)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Incorrect pass");
			_delay_ms(500);
			Check_Password(Config_Ptr);
		}
		else
		{
			LCD_clearScreen();


			Timer0_init(Config_Ptr);
			g_tick =0;
			g_calc_sec_flag =0;
			while (g_calc_sec_flag != 60)
			{
				LCD_displayStringRowColumn(0,0,"Wait 60 Sec");
			}

			counter_error = 0;
			return_flag =1;
		}
	}
	else
	{
		UART_sendByte(4);
		counter_error = 0;
	}
}
void Open_gate(const Timer0_ConfigType * Config_Ptr)
{
	UART_sendByte(UNLOCK_DOOR);

	LCD_clearScreen();
	Timer0_init(Config_Ptr);
	g_tick =0;
	g_calc_sec_flag =0;

	while(g_calc_sec_flag != 5)
	{
		LCD_displayStringRowColumn(0,0,"Unlocking door");
	}

	LCD_clearScreen();
	g_tick =0;
	g_calc_sec_flag =0;

	while(g_calc_sec_flag != 3)
	{
		LCD_displayStringRowColumn(0,0,"Door is opened");
	}

	LCD_clearScreen();
	g_tick =0;
	g_calc_sec_flag =0;

	while(g_calc_sec_flag != 5)
	{
		LCD_displayStringRowColumn(0,0,"locking door");
	}
}

void Main_Menue(const Timer0_ConfigType * Config_Ptr)
{
	uint8 key=0;

	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"#:Open door");
	LCD_displayStringRowColumn(1,0,"*:Change pass");

	while((key != '*') && (key != '#'))
	{
		key = KEYPAD_getPressedKey();
	}

	Check_Password(Config_Ptr);
	UART_sendByte(return_flag);


	if(return_flag == 0)
	{
		UART_sendByte(key);
		if (key == '*')
		{
			New_Password();

		}
		else
		{
			Open_gate(Config_Ptr);
		}
	}
	else
	{
		return_flag=0;
	}

}
int main(void)
{

	uint8 flag_init=0;

	SREG|=(1<<7);

	Timer0_ConfigType Timer_Config = {NORMAL,CLK_1024,0,0};
	Timer0_setCallBack(Calculate_time);

	UART_ConfigType UART_Config={9600,BIT_1,DISABLED,BIT_8};
	/* Initialize the UART driver with Baud-rate = 9600 bits/sec */
	UART_init(&UART_Config);

	LCD_init();
	UART_init(& UART_Config);

	while(1)
	{
		UART_sendByte(M1_READY);

		flag_init = UART_recieveByte();

		if(flag_init != INITIALIZED_FLAG)
		{

			New_Password();
		}

		else
		{

			Main_Menue(& Timer_Config);
		}
	}
	return 0;
}



