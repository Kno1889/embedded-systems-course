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
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void periodic_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */


SemaphoreHandle_t* semaphore;


void timerCallbackFunction2(TimerHandle_t timer_handle)
{
	xSemaphoreGive(semaphore);
}

static void periodic_task(void *pvParameters)
{
	int counter = 0;
    while(1)
    {
        if(xSemaphoreTake(semaphore, portMAX_DELAY) == pdPASS){
        	PRINTF("Hello world, %d.\r\n", counter);
        	counter ++;
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

	semaphore = xSemaphoreCreateBinary();

	status = xTaskCreate(periodic_task, "Hello_task", 200, (void*) semaphore, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	TimerHandle_t timer_handle = xTimerCreate("Periodic timer",
												1000 / portTICK_PERIOD_MS,
												pdTRUE,
												NULL,
												timerCallbackFunction2);

	status = xTimerStart(timer_handle, 0);
	if(status != pdPASS)
	{
		PRINTF("Couldn’t start the timer!.\r\n");
		while (1);
	}
	vTaskStartScheduler();
	while (1)
	{}
}

