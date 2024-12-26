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
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "semphr.h"

#define LEFT_BIT (1 << 0)
#define RIGHT_BIT (1 << 1)
#define UP_BIT (1 << 2)
#define DOWN_BIT (1 << 3)

void producer_event(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t up_sem = semaphores[0];
	SemaphoreHandle_t down_sem = semaphores[1];
	SemaphoreHandle_t left_sem = semaphores[2];
	SemaphoreHandle_t right_sem = semaphores[3];

	BaseType_t status;
	char c;

	while(1)
	{
		scanf("%c", &c);
		switch(c)
		{
		case 'a': // give xSemaphoreGive(producer1_semaphore)
			xSemaphoreGive(left_sem);
			break;

		case 's':
			xSemaphoreGive(down_sem);
			break;

		case 'd':
			xSemaphoreGive(right_sem);
			break;

		case 'w':
			xSemaphoreGive(up_sem);
			break;
		}
	}
}

void up_event(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t up_sem = semaphores[0];

	while(1)
	{

		if(xSemaphoreTake(up_sem, portMAX_DELAY) == pdPASS)
		{
			PRINTF("Up\r\n");
		}
	}
}

void down_event(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t down_sem = semaphores[1];

	while(1)
	{
		if(xSemaphoreTake(down_sem, portMAX_DELAY) == pdPASS)
		{
			PRINTF("Down\r\n");
		}
	}
}

void left_event(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t left_sem = semaphores[2];

	while(1)
	{
		printf("");
		if(xSemaphoreTake(left_sem, portMAX_DELAY) == pdPASS)
		{
			PRINTF("Left\r\n");
		}
	}
}

void right_event(void* pvParameters)
{
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t right_sem = semaphores[3];

	while(1)
	{
		if(xSemaphoreTake(right_sem, portMAX_DELAY) == pdPASS)
		{
			PRINTF("Right\r\n");
		}
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*) malloc(4 * sizeof(SemaphoreHandle_t));
	semaphores[0] = xSemaphoreCreateBinary(); // up
	semaphores[1] = xSemaphoreCreateBinary(); // down
	semaphores[2] = xSemaphoreCreateBinary(); // left
	semaphores[3] = xSemaphoreCreateBinary(); // right

	status = xTaskCreate(producer_event, "producer", 200, (void*)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(up_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(down_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(left_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(right_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();
	while (1)
	{}
}


