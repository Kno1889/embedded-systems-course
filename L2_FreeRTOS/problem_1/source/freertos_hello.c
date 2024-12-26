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

char* str;

void task_1(void *pvParameters)
{
	PRINTF("Please Enter a String: ");
	scanf("%s", &str);
	vTaskDelete(NULL);
}

void task_2(void *pvParameters)
{

	//if the string is not null
		//do the while loop
	while(1)
	{
		if(&str[0]){
			PRINTF("Your string is: %s\n",  &str);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

int main(void)
{
	BaseType_t status;
	TaskHandle_t task_1_handle;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	status = xTaskCreate(task_1, "Read User Input", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(task_2, "Hello_task", 200, NULL, 3, NULL);

	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}


	vTaskStartScheduler();

	for (;;);


//	status = xTaskCreate(hello_task2, "Hello_task2", 200, (void*)str, 2, NULL);
}






/*******************************************************************************
 * Definitions
 ******************************************************************************/

///* Task priorities. */
//#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
///*******************************************************************************
// * Prototypes
// ******************************************************************************/
//static void hello_task(void *pvParameters);
//
///*******************************************************************************
// * Code
// ******************************************************************************/
///*!
// * @brief Application entry point.
// */
//int main(void)
//{
//    /* Init board hardware. */
//    BOARD_InitBootPins();
//    BOARD_InitBootClocks();
//    BOARD_InitDebugConsole();
//    if (xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL) !=
//        pdPASS)
//    {
//        PRINTF("Task creation failed!.\r\n");
//        while (1)
//            ;
//    }
//    vTaskStartScheduler();
//    for (;;)
//        ;
//}
//
///*!
// * @brief Task responsible for printing of "Hello world." message.
// */
//static void hello_task(void *pvParameters)
//{
//    for (;;)
//    {
//        PRINTF("Hello world.\r\n");
//        vTaskSuspend(NULL);
//    }
//}
