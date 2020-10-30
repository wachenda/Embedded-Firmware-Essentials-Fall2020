/*
 * main.cpp
 *
 *  Created on: Oct 29, 2020
 *      Author: DavidW
 */

#include "main.h"	// Needed to give access to the rest of the code already pre-configured
#include "LED.h"
#include <stdio.h> // NOTE:  C++ is much more strict than C about enforcing declarations and making sure that types are matched
#include <string.h>

extern UART_HandleTypeDef huart2;

void CppMain(void)
{
	LED led1(LD3_GPIO_Port, LD3_Pin);
	int value;
	char buf[80];

	while(1)
	{
		led1 = 1;		// operator= (int value) - C++ shorthand for write() using operator overloading
		HAL_Delay(500);
		led1 = !led1;	// operator= (LED &rhs)using the assignment operator which copies the state from the LED argument
		HAL_Delay(500);
		value = led1;	// operator int() - C++ shorthand for read() using operator overloading
		sprintf(buf,"led1 = %d\r\n",value);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);
		led1.write(1);
		HAL_Delay(500);
		led1 = 0;
		HAL_Delay(500);
		led1.Toggle();
		HAL_Delay(500);

	}
}



