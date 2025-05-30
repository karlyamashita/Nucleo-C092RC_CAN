/*
 * GPIO_Handler.h
 *
 *  Created on: May 19, 2025
 *      Author: karl.yamashita
 */

#ifndef INC_GPIO_HANDLER_H_
#define INC_GPIO_HANDLER_H_

typedef struct
{
	bool state;
	bool lastState;
}GPIO_Pin_t;

void ButtonPressed(void);
void LED_GreenToggle(void);
void LED_BlueToggle(void);

#endif /* INC_GPIO_HANDLER_H_ */
