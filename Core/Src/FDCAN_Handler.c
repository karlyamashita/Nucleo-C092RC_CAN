/*
 * FDCAN_Handler.c
 *
 *  Created on: Oct 7, 2023
 *      Author: karl.yamashita
 */

#include "main.h"



/*
 * Description: Initialize FDCAN buffer.
 * 				Assign FDCAN instance to buffer instance. Set queueSize.
 */
void FDCAN_Init(FDCAN_Init_t *init)
{
	/*
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_DISABLE;
	sFilterConfig.FilterID1 = 0x101;
	sFilterConfig.FilterID2 = 0x7FF;
	*/
	if (HAL_FDCAN_ConfigFilter(init->fdcan, &init->sFilterConfig) != HAL_OK)
	{

	}

	/* Configure global filter on both FDCAN instances:
	     Filter all remote frames with STD and EXT ID
	     Reject non matching frames with STD ID and EXT ID */
	if (HAL_FDCAN_ConfigGlobalFilter(init->fdcan, init->NonMatchingStd /*FDCAN_ACCEPT_IN_RX_FIFO0*/, init->NonMatchingExt /*FDCAN_ACCEPT_IN_RX_FIFO0*/, init->RejectRemoteStd /*FDCAN_FILTER_REMOTE*/, init->RejectRemoteExt/*FDCAN_FILTER_REMOTE*/) != HAL_OK)
	{

	}

	/* Activate Rx FIFO 0 new message notification on both FDCAN instances */
	if (HAL_FDCAN_ActivateNotification(init->fdcan, init->FDCAN_Rx_Fifo0_Interrupts/*FDCAN_IT_RX_FIFO0_NEW_MESSAGE*/, 0) != HAL_OK)
	{

	}
}

/*
 * Description: Check for new message and point msgToParse to buffer index to be parsed
 * return: True if message ready, else return false
 *
 */
int FDCAN_MsgReady(FDCAN_Struct_t *msg)
{
	if(msg->rx.ptr.cnt_Handle)
	{
		msg->rx.msgToParse = &msg->rx.msgQueue[msg->rx.ptr.index_OUT];
		RingBuff_Ptr_Output(&msg->rx.ptr, msg->rx.queueSize);
		return true;
	}

	return false;
}

/*
 * Description: copy data to tx buffer and increment buffer pointer.
 *
 */
void FDCAN_AddDataToTxBuffer(FDCAN_Struct_t *msg, FDCAN_Tx *data)
{
	memcpy(&msg->tx.msgQueue[msg->tx.ptr.index_IN].pTxHeader, &data->pTxHeader, sizeof(data->pTxHeader));
	memcpy(&msg->tx.msgQueue[msg->tx.ptr.index_IN].data, &data->data, sizeof(data->data));

	RingBuff_Ptr_Input(&msg->tx.ptr, msg->tx.queueSize);

	FDCAN_Transmit(msg); // try to send
}

/*
 * Description: This will be called from FDCAN_AddDataToTxBuffer or HAL_FDCAN_TxBufferCompleteCallback
 * Input: The FDCAN_buffer
 *
 */
void FDCAN_Transmit(FDCAN_Struct_t *msg)
{
	if(msg->tx.ptr.cnt_Handle)
	{
		if(!msg->tx.txPending)
		{
			if(HAL_FDCAN_AddMessageToTxFifoQ(msg->fdcan, &msg->tx.msgQueue[msg->tx.ptr.index_OUT].pTxHeader, msg->tx.msgQueue[msg->tx.ptr.index_OUT].data) == HAL_OK)
			{
				msg->tx.txPending = true;
				RingBuff_Ptr_Output(&msg->tx.ptr, msg->tx.queueSize);
			}
		}
	}
}


/*
 * User will need to create variables
 *

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
// be sure to call function as follows
FDCAN_Init(&fdcan_init);


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


// be sure to call before while loop
HAL_FDCAN_Start(&hfdcan1);


// add callbacks to IRQ Handler
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



 */

