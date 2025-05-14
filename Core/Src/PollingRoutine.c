/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 *      The object of this PollingRoutine.c/h files is to not have to write code in main.c which already has a lot of generated code.
 *      It is cumbersome having to scroll through all the generated code for your own code and having to find a USER CODE section so your code is not erased when CubeMX re-generates code.
 *      
 *      Direction: Call PollingInit before the main while loop. Call PollingRoutine from within the main while loop
 * 
 *      Example;
        // USER CODE BEGIN WHILE
        PollingInit();
        while (1)
        {
            PollingRoutine();
            // USER CODE END WHILE

            // USER CODE BEGIN 3
        }
        // USER CODE END 3

 */


#include "main.h"


const char fwversion[] = "1.0.1";


FDCAN_Init_t fdcan_init =
{
	.fdcan = &hfdcan1,
	.sFilterConfig.IdType = FDCAN_STANDARD_ID,
	.sFilterConfig.FilterIndex = 0,
	.sFilterConfig.FilterType = FDCAN_FILTER_MASK,
	.sFilterConfig.FilterConfig = FDCAN_FILTER_DISABLE,
	.sFilterConfig.FilterID1 = 0x101,
	.sFilterConfig.FilterID2 = 0x7FF,
	.NonMatchingStd = FDCAN_ACCEPT_IN_RX_FIFO0,
	.NonMatchingExt = FDCAN_ACCEPT_IN_RX_FIFO0,
	.RejectRemoteStd = FDCAN_FILTER_REMOTE,
	.RejectRemoteExt = FDCAN_FILTER_REMOTE,
	.FDCAN_Rx_Fifo0_Interrupts = FDCAN_IT_RX_FIFO0_NEW_MESSAGE
};


#define FDCAN1_RX_QUEUE_SIZE 8
#define FDCAN1_TX_QUEUE_SIZE 8
FDCAN_Rx fdcan_rx_queue[FDCAN1_RX_QUEUE_SIZE];
FDCAN_Tx fdcan_tx_queue[FDCAN1_TX_QUEUE_SIZE];
FDCAN_Struct_t fdcan1_msg =
{
	.fdcan = &hfdcan1,
	.rx.msgQueue = fdcan_rx_queue,
	.rx.queueSize = FDCAN1_RX_QUEUE_SIZE,
	.tx.msgQueue = fdcan_tx_queue,
	.tx.queueSize = FDCAN1_TX_QUEUE_SIZE
};


#define UART2_DMA_RX_QUEUE_SIZE 10 // queue size
#define UART2_DMA_TX_QUEUE_SIZE 4
UART_DMA_Data uart2_dmaDataRxQueue[UART2_DMA_RX_QUEUE_SIZE] = {0};
UART_DMA_Data uart2_dmaDataTxQueue[UART2_DMA_TX_QUEUE_SIZE] = {0};
UART_DMA_Struct_t uart2_msg =
{
	.huart = &huart2,
	.rx.queueSize = UART2_DMA_RX_QUEUE_SIZE,
	.rx.msgQueue = uart2_dmaDataRxQueue,
	.tx.queueSize = UART2_DMA_TX_QUEUE_SIZE,
	.tx.msgQueue = uart2_dmaDataTxQueue,
	.dma.dmaPtr.SkipOverFlow = true
};

// Add these, typically in your polling routine
void UART_CheckForNewMessage(UART_DMA_Struct_t *msg)
{
	if(UART_DMA_RxMsgRdy(msg))
	{
		// user can parse msg variable.
		if(strncmp((char*)msg->rx.msgToParse->data, "get version", strlen("get version")) == 0)
		{
			// call function to return version number
		}
		else if(strncmp((char*)msg->rx.msgToParse->data, "get status", strlen("get status")) == 0)
		{
			// call function to get status information
		}
	}
}


void PollingInit(void)
{
	FDCAN_Init(&fdcan_init);
	HAL_FDCAN_Start(&hfdcan1);
}

void PollingRoutine(void)
{

	UART_DMA_ParseCircularBuffer(&uart2_msg);

	FDCAN_Parse(&fdcan1_msg);

}

void FDCAN_Parse(FDCAN_Struct_t *msg)
{
	if(FDCAN_MsgReady(msg))
	{

	}
}

void UART_Parse(UART_DMA_Struct_t *msg)
{

	if(UART_DMA_RxMsgRdy(msg))
	{

	}
}


int Version(char *msg, char *retStr)
{
	sprintf(retStr, fwversion);

	return NO_ERROR;
}





