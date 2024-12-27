#include "Terminal_Component.h"

EventGroupHandle_t event_group;
QueueHandle_t uart_queue;


void setupTerminalComponent()
{
	BaseType_t status;

	setupTerminalPins();

	setupUART();

    /*************** UART Task ***************/
	uart_queue = xQueueCreate(10, sizeof(char*));
	if (uart_queue == NULL)
	{
		PRINTF("Queue creation failed!.\r\n");
		while (1);
	}
    status = xTaskCreate(uartTask, "UART task", 200, NULL, 2, NULL);
    if (status != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1);
    }

    /*************** Terminal Control Task ***************/
    //Create Event groups
    event_group = xEventGroupCreate();
	//Create Terminal Control Task
    status = status = xTaskCreate(terminalControlTask, "Terminal task", 200, NULL, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
}

void setupTerminalPins()
{
    //Configure UART pins
}

void sendMessage(const char *format, ...)
{
	va_list args;

	char* str = (char*)pvPortMalloc(250 * sizeof(char));

	va_start(args, format);
	vsprintf(str, format, args);

	if(xQueueSendToBack(uart_queue, (void *) &str, portMAX_DELAY) != pdPASS )
	{
		PRINTF("Send message to uart_queue failed!.\r\n");
		while (1);
	}

	va_end(args);
}

void setupUART()
{
	//Setup UART for sending and receiving
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 57600;
	config.enableTx = true;
	config.enableRx = true;
	config.enableRxRTS = true;
	config.enableTxCTS = true;
	UART_Init(TARGET_UART, &config, CLOCK_GetFreq(kCLOCK_BusClk));
	/******** Enable Interrupts *********/
	UART_EnableInterrupts(TARGET_UART, kUART_RxDataRegFullInterruptEnable);
	NVIC_SetPriority(UART4_RX_TX_IRQn, 2);
	EnableIRQ(UART4_RX_TX_IRQn);
}

void uartTask(void* pvParameters)
{
	char* welcome_message = "UART task started\n\r";
	char* received_str;
	BaseType_t status;

	UART_WriteBlocking(TARGET_UART, welcome_message, strlen(welcome_message));

	while(1)
	{
		status = xQueueReceive(uart_queue, (void *) &received_str, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
			while (1);
		}
		UART_WriteBlocking(TARGET_UART, received_str, strlen(received_str));
		vPortFree(received_str);
	}
}

void UART4_RX_TX_IRQHandler()
{
	//UART ISR
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	UART_GetStatusFlags(TARGET_UART);
	char ch = UART_ReadByte(TARGET_UART);
	switch(ch)
	{
	case 'a':
		xEventGroupSetBitsFromISR(event_group, LEFT_BIT, & xHigherPriorityTaskWoken);
		break;
	case 's':
		xEventGroupSetBitsFromISR(event_group, DOWN_BIT, & xHigherPriorityTaskWoken);
		break;
	case 'd':
		xEventGroupSetBitsFromISR(event_group, RIGHT_BIT, & xHigherPriorityTaskWoken);
		break;
	case 'w':
		xEventGroupSetBitsFromISR(event_group, UP_BIT, & xHigherPriorityTaskWoken);
		break;
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void terminalControlTask(void* pvParameters)
{
	//Terminal Control Task implementation
	char c;
	while(1)
	{
		scanf("%c", &c);
		switch(c)
		{
		case 'a':
			xEventGroupSetBits(event_group, LEFT_BIT);
			break;

		case 's':
			xEventGroupSetBits(event_group, DOWN_BIT);
			break;

		case 'd':
		xEventGroupSetBits(event_group, RIGHT_BIT);
		break;

		case 'w':
		xEventGroupSetBits(event_group, UP_BIT);
		break;
		}
	}
}
