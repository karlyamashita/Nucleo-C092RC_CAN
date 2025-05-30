/*
 * GPIO_Handler.c
 *
 *  Created on: May 19, 2025
 *      Author: karl.yamashita
 */


#include "main.h"


GPIO_Pin_t userButton =
{
	.lastState = 1,
	.state = 1
};


void ButtonPressed(void)
{
	Nop();
	userButton.state = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
}

void LED_GreenToggle(void)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
}

void LED_BlueToggle(void)
{
	HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
}
