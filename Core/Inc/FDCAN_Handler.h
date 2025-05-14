/*
 * FDCAN_Handler.h
 *
 *  Created on: Oct 7, 2023
 *      Author: karl.yamashita
 */

#ifndef INC_FDCAN_HANDLER_H_
#define INC_FDCAN_HANDLER_H_


#define FDCAN_RX_QUEUE_SIZE 8
#define FDCAN_TX_QUEUE_SIZE 4


typedef struct
{
	FDCAN_HandleTypeDef *fdcan; // FDCAN instance
	FDCAN_FilterTypeDef sFilterConfig;
	// HAL_FDCAN_ConfigGlobalFilter
	uint32_t NonMatchingStd; // FDCAN_ACCEPT_IN_RX_FIFO0 | FDCAN_ACCEPT_IN_RX_FIFO1 | FDCAN_REJECT
	uint32_t NonMatchingExt; // FDCAN_ACCEPT_IN_RX_FIFO0 | FDCAN_ACCEPT_IN_RX_FIFO1 | FDCAN_REJECT
	uint32_t RejectRemoteStd; // FDCAN_FILTER_REMOTE | FDCAN_REJECT_REMOTE
	uint32_t RejectRemoteExt; // FDCAN_FILTER_REMOTE | FDCAN_REJECT_REMOTE
	// HAL_FDCAN_ActivateNotification
	uint32_t FDCAN_Rx_Fifo0_Interrupts; // FDCAN_IT_RX_FIFO0_MESSAGE_LOST | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_NEW_MESSAGE
}FDCAN_Init_t;


typedef struct
{
	FDCAN_RxHeaderTypeDef *pRxHeader;
	uint8_t data[64];
}FDCAN_Rx;

typedef struct
{
	FDCAN_TxHeaderTypeDef pTxHeader;
	uint8_t data[64];
}FDCAN_Tx;


typedef struct
{
	FDCAN_HandleTypeDef *fdcan;
	struct
	{
		FDCAN_Rx *msgQueue;
		FDCAN_Rx *msgToParse;
		RING_BUFF_STRUCT ptr;
		uint32_t queueSize;
	}rx;
	struct
	{
		FDCAN_Tx *msgQueue;
		RING_BUFF_STRUCT ptr;
		uint32_t queueSize;
		bool txPending;
	}tx;
}FDCAN_Struct_t;


void FDCAN_Init(FDCAN_Init_t *init);

void FDCAN_AddToRxBuffer(FDCAN_Struct_t *fdcanBuffer, FDCAN_Rx *rxData);
int FDCAN_MsgReady(FDCAN_Struct_t *msg);

void FDCAN_AddDataToTxBuffer(FDCAN_Struct_t *fdcanBuffer, FDCAN_Tx *data);
void FDCAN_Transmit(FDCAN_Struct_t *fdcanBuffer);


#endif /* INC_FDCAN_HANDLER_H_ */
