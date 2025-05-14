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
	if(hfdcan == fdcan1_msg.fdcan)
	{
		if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
		{
			if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, fdcan1_msg.rx.msgQueue[fdcan1_msg.rx.ptr.index_IN].pRxHeader, fdcan1_msg.rx.msgQueue[fdcan1_msg.rx.ptr.index_IN].data) == HAL_OK)
			{
				RingBuff_Ptr_Input(&fdcan1_msg.rx.ptr, fdcan1_msg.rx.queueSize);
			}
		}
	}
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
	if(hfdcan == fdcan1_msg.fdcan)
	{
		fdcan1_msg.tx.txPending = false;
		FDCAN_Transmit(&fdcan1_msg); // transmit if more message in queue
	}
}
