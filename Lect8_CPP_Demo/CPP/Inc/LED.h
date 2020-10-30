/*
 * LED.h
 *
 *  Created on: Oct 29, 2020
 *      Author: DavidW
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"

class LED {
public:
	LED(GPIO_TypeDef* gx, uint16_t gp)
	{
		GPIOx = gx;
		GPIO_Pin = gp;
	}

	virtual ~LED()
	{
	}

	void On(void)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
	}

	void Off(void)
	{
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);

	}

	void Toggle(void)
	{
		HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);

	}

	void write(int value)
	{
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,(GPIO_PinState)value);
	}

	int read(void)
	{
		return (int)HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
	}

	/** Overridden = operator for writing **/
	LED &operator=(int value)
	{
//		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,(GPIO_PinState)value);
		write(value);
		return *this;
	}

	LED &operator= (LED &rhs);

	operator int()
	{
//		return (int)HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
		return read();
	}



protected:
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
};

#endif /* INC_LED_H_ */
