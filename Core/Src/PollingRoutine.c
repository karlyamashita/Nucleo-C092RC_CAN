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


const char version[] = "1.0.1";


uint8_t msg_copy_command[UART_DMA_QUEUE_DATA_SIZE] = {0};


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
	.ActivateNotification = FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_FLAG_TX_FIFO_EMPTY
};


#define FDCAN1_RX_QUEUE_SIZE 10
#define FDCAN1_TX_QUEUE_SIZE 10
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

void PollingInit(void)
{
	FDCAN_Init(&fdcan_init);

	UART_DMA_EnableRxInterruptIdle(&uart2_msg);

	TimerCallbackRegisterOnly(&timerCallback, LED_GreenToggle);
	TimerCallbackTimerStart(&timerCallback, LED_GreenToggle, 500, TIMER_REPEAT);

	TimerCallbackRegisterOnly(&timerCallback, ButtonPressed);

	STM32_Ready();
}

void PollingRoutine(void)
{
	TimerCallbackPoll(&timerCallback);

	UART_DMA_ParseCircularBuffer(&uart2_msg);

	FDCAN_Parse(&fdcan1_msg);

	UART_Parse(&uart2_msg);

	ButtonCheck();
}

// Add these, typically in your polling routine
void UART_Parse(UART_DMA_Struct_t *msg)
{
	int status = NO_ERROR;
	char retStr[UART_DMA_QUEUE_DATA_SIZE] = "OK";
	char *ptr;

	if(UART_DMA_RxMsgRdy(msg))
	{
		memset(&msg_copy_command, 0, sizeof(msg_copy_command)); // clear
		snprintf((char*)msg_copy_command, strlen((char*)msg->rx.msgToParse->data) - ((strlen((char*)msg->rx.msgToParse->data) > 2) ? 1:0), "%s", (char*)msg->rx.msgToParse->data);

		ptr = (char*)msg->rx.msgToParse->data;
		RemoveSpaces(ptr);

		// user can parse msg variable.
		if(strncmp((char*)ptr, "version", strlen("version")) == 0)
		{
			// call function to return version number
			status = Version(NULL, retStr);
		}
		else if(strncmp((char*)ptr, "classic:", strlen("classic:")) == 0)
		{
			ptr += strlen("classic:");
			CAN_Send_Classic_Format(&fdcan1_msg, ptr);
		}
		else if(strncmp((char*)ptr, "fd:", strlen("fd:")) == 0)
		{
			ptr += strlen("fd:");
			CAN_Send_FD_Format(&fdcan1_msg, ptr);
		}
		else
		{
			status = COMMAND_UNKNOWN;
		}

		// check return status
		if(status == NO_ACK)
		{
			return;
		}
		else if(status != 0) // other return status other than NO_ACK or NO_ERROR
		{
			UART_DMA_PrintError(msg, (char*)msg_copy_command, status);
		}
		else // NO_ERROR
		{
			UART_DMA_PrintReply(msg, (char*)msg_copy_command, retStr);
		}

		memset(ptr, 0, UART_DMA_QUEUE_DATA_SIZE); // clear current buffer index
	}
}

void FDCAN_Parse(FDCAN_Struct_t *msg)
{
	char str[UART_DMA_QUEUE_DATA_SIZE] = {0};

	if(FDCAN_MsgReady(msg))
	{
		if(msg->rx.msgToParse->pRxHeader.Identifier == 0x101)
		{
			if((msg->rx.msgToParse->data[0] % 3) == 0)
			{
				LED_BlueToggle();
			}
		}

		sprintf(str, "ID = 0x%lX, DLC = 0x%ld, data[1] = 0x%X", msg->rx.msgToParse->pRxHeader.Identifier, msg->rx.msgToParse->pRxHeader.DataLength, msg->rx.msgToParse->data[1]);
		UART_DMA_NotifyUser(&uart2_msg, str, strlen(str), true);
	}
}

int Version(char *msg, char *retStr)
{
	sprintf(retStr, version);

	return NO_ERROR;
}

void STM32_Ready(void)
{
	char str[UART_DMA_QUEUE_DATA_SIZE] = {0};

	sprintf(str, "STM32 Ready - %s", version);

	UART_DMA_NotifyUser(&uart2_msg, str, strlen(str), true);
}

void ButtonCheck(void)
{
	if(userButton.lastState != userButton.state)
	{
		userButton.lastState = userButton.state;

		if(userButton.lastState == 0) // pressed
		{

		}
	}
}


/*
 * Description: Send some data
 */
void CAN_Send_FD_Format(FDCAN_Struct_t *msg, char *data)
{
	char *token;
	char *rest = data;
	char delim[] = " ,\r";
	static uint32_t counter = 0;
	uint32_t dataValue = 0;

	FDCAN_Tx tx =
	{
		//.data[0]
		.data[1] = 0x22,
		.data[2] = 0x33,
		.data[3] = 0x44,
		.data[4] = 0x55,
		.data[5] = 0x66,
		.data[6] = 0x77,
		.data[7] = 0x88,
		.data[8] = 0x99,
		.data[9] = 0xAA,
		.data[10] = 0xBB,
		.data[63] = 0xFF,
		.pTxHeader.FDFormat = FDCAN_FD_CAN,
		.pTxHeader.Identifier = 0x101,
		.pTxHeader.IdType = FDCAN_STANDARD_ID,
		.pTxHeader.TxFrameType = FDCAN_DATA_FRAME,
		.pTxHeader.DataLength = FDCAN_DLC_BYTES_64
	};

	token = strtok_r(rest, delim, &rest); // id
	IsHex(token, &dataValue);
	tx.pTxHeader.Identifier = dataValue;

	token = strtok_r(rest, delim, &rest); // dlc
	IsHex(token, &dataValue);
	tx.pTxHeader.DataLength = dataValue;

	token = strtok_r(rest, delim, &rest); // data
	IsHex(token, &dataValue);

	tx.data[0] = counter;
	tx.data[1] = dataValue; // data
	tx.data[63] = counter++;

	FDCAN_AddDataToTxBuffer(msg, &tx);
}

void CAN_Send_Classic_Format(FDCAN_Struct_t *msg, char *data)
{
	char *token;
	char *rest = data;
	char delim[] = " ,\r";
	static uint32_t counter = 0;
	uint32_t dataValue = 0;

	FDCAN_Tx tx =
	{
		//.data[0]
		.data[1] = 0x22,
		.data[2] = 0x33,
		.data[3] = 0x44,
		.data[4] = 0x55,
		.data[5] = 0x66,
		.data[6] = 0x77,
		.data[7] = 0x88,
		.pTxHeader.FDFormat = FDCAN_CLASSIC_CAN,
		.pTxHeader.Identifier = 0x101,
		.pTxHeader.IdType = FDCAN_STANDARD_ID,
		.pTxHeader.TxFrameType = FDCAN_DATA_FRAME,
		.pTxHeader.DataLength = FDCAN_DLC_BYTES_8
	};

	token = strtok_r(rest, delim, &rest); // id
	IsHex(token, &dataValue);
	tx.pTxHeader.Identifier = dataValue;

	token = strtok_r(rest, delim, &rest); // dlc
	IsHex(token, &dataValue);
	tx.pTxHeader.DataLength = dataValue;

	token = strtok_r(rest, delim, &rest); // data
	IsHex(token, &dataValue);

	tx.data[0] = counter;
	tx.data[1] = dataValue; // data

	FDCAN_AddDataToTxBuffer(msg, &tx);
}


