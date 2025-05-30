/*
 * IRQ_Handler.c
 *
 *  Created on: May 14, 2025
 *      Author: karl.yamashita
 */


#include "main.h"


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	int i = 0;
	uint32_t size = 0;

	if(huart == uart2_msg.huart)
	{
		size = UART_DMA_GetSize(&uart2_msg, Size);

		for(i = 0; i < size; i++)
		{
			uart2_msg.dma.circularBuffer[uart2_msg.dma.circularPtr.index_IN] = uart2_msg.dma.dma_data[uart2_msg.dma.dmaPtr.index_IN];
			RingBuff_Ptr_Input(&uart2_msg.dma.circularPtr, UART_DMA_CIRCULAR_SIZE);
			RingBuff_Ptr_Input(&uart2_msg.dma.dmaPtr, UART_DMA_BUFFER_SIZE);
		}
	}
	// repeat for other UART ports using (else if)
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart2_msg.huart)
	{
		uart2_msg.tx.txPending = false;
		UART_DMA_SendMessage(&uart2_msg);
	}
	// repeat for other UART ports using (else if)
}


/*
 * Description: Add data to queue and increment pointer
 *
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_Rx *ptr;

	if(hfdcan == fdcan1_msg.fdcan)
	{
		if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
		{
			ptr = &fdcan1_msg.rx.msgQueue[fdcan1_msg.rx.ptr.index_IN];
			if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &ptr->pRxHeader, ptr->data) == HAL_OK)
			{
				RingBuff_Ptr_Input(&fdcan1_msg.rx.ptr, fdcan1_msg.rx.queueSize);
			}
		}
	}
}

void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
	if(hfdcan == fdcan1_msg.fdcan)
	{
		FDCAN_Transmit(&fdcan1_msg); // transmit if more message in queue
	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	GPIO_EXTI_Callback(GPIO_Pin);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	GPIO_EXTI_Callback(GPIO_Pin);
}

void GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t pinStatus;

	if(GPIO_Pin == USER_BUTTON_Pin)
	{
		pinStatus = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
		if(!pinStatus)
		{
			TimerCallbackTimerStart(&timerCallback, ButtonPressed, 50, TIMER_NO_REPEAT);
		}
		else
		{
			TimerCallbackDisable(&timerCallback, ButtonPressed);
			userButton.state = GPIO_PIN_SET;
		}
	}
}
