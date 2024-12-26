/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

char str[];

void producer_queue(void* pvParameters)
{
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;
	int i = 0;

	while(1)
	{
		status = xQueueSendToBack(queue1, &str[i], portMAX_DELAY);
		i++;
		if (status != pdPASS)
		{
			PRINTF("Queue Send failed!.\r\n");
			while (1);
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);

		if (str[i] == '\0')
		{
			vTaskDelete(NULL);
		}
	}
}

void consumer_queue(void* pvParameters)
{
	QueueHandle_t queue1 = (QueueHandle_t)pvParameters;
	BaseType_t status;
	char received_char;
	while(1)
	{
		status = xQueueReceive(queue1, &received_char, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Receive failed!.\r\n");
			while (1);
		}
		PRINTF("%c", received_char);
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	PRINTF("Please enter a string: ");
	SCANF("%s", &str);

	QueueHandle_t queue1 = xQueueCreate(1, sizeof(char));
	if (queue1 == NULL)
	{
		PRINTF("Queue creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(producer_queue, "producer", 200, (void*)queue1, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(consumer_queue, "consumer", 200, (void*)queue1, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();
	while (1)
	{}
}

