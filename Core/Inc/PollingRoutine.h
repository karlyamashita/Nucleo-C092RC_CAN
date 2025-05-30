/*
 * PollingRoutine.h
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template
 */

#ifndef INC_POLLINGROUTINE_H_
#define INC_POLLINGROUTINE_H_


/*

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#define Nop() asm(" NOP ")

//#include "RingBuffer.h"
//#include "TimerCallback.h"

#include "PollingRoutine.h"

*/
#ifndef __weak
#define __weak __attribute__((weak))
#endif

void PollingInit(void);
void PollingRoutine(void);

void UART_Parse(UART_DMA_Struct_t *msg);
void FDCAN_Parse(FDCAN_Struct_t *msg);

int Version(char *msg, char *retStr);
void STM32_Ready(void);
void ButtonCheck(void);
void CAN_Send_FD_Format(FDCAN_Struct_t *msg, char *data);
void CAN_Send_Classic_Format(FDCAN_Struct_t *msg, char *data);

#endif /* INC_POLLINGROUTINE_H_ */
