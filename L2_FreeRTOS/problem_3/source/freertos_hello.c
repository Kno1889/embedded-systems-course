/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include <ctype.h>
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

char* str1;
char* str2;

void task_1(void *pvParameters)
	{
		PRINTF("Please Enter a String: ");
		scanf("%s", &str1);
		xSemaphoreGive(*((SemaphoreHandle_t*)pvParameters));
		vTaskDelete(NULL);
}

void task_2(void *pvParameters)
{
	//if the string is not null
	//do the while loop
	while(1)
	{
		status_t status = xSemaphoreTake(*((SemaphoreHandle_t*)pvParameters), portMAX_DELAY);

		if(&str1[0] && status == pdPASS){
			PRINTF("Your string in lowercase is: %s\n",  &str1);

			xSemaphoreGive(*((SemaphoreHandle_t*)pvParameters));

		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

//char * capitalize(char * str){
//	int i = 0;
//	while(&str[i] != "\0"){
////		&str[i] = toupper(&str[i])
//
//	}
//}

void task_3(void *pvParameters)
{
	//if the string is not null
	//do the while loop
	while(1)
	{
		status_t status = xSemaphoreTake(*((SemaphoreHandle_t*)pvParameters), portMAX_DELAY);

		if(&str1[0] && status == pdPASS){
			// find string length
//			char * ptr = str1;
//			int i = 0;
//			while(&str1[i] != '\0'){
//				printf("%d", i);
//				printf("%c", &str1[i]);
//				i++;
//			}
//
//
//			str2 = (char *) malloc(i + 1);
//
//			for(int j = 0; j < i; j++)
//				str2[j] = toupper(str1[j]);
//
//			str2[i] = "\0";

			&str1 = capitalize(&str1);

			PRINTF("Your string in uppercase is: %s\n",  &str2);
			xSemaphoreGive(*((SemaphoreHandle_t*)pvParameters));
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}


int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	SemaphoreHandle_t* semaphore = (SemaphoreHandle_t*) malloc(sizeof(SemaphoreHandle_t));
	semaphore = xSemaphoreCreateBinary(); //we only need one

	status = xTaskCreate(task_1, "Read User Input", 200, (void*)semaphore, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(task_2, "lowercase", 200, (void*)semaphore, 3, NULL);

	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	status = xTaskCreate(task_3, "uppercase", 200, (void*)semaphore, 3, NULL);

		if (status != pdPASS)
		{
			PRINTF("Task creation failed!.\r\n");
			while (1);
		}


	vTaskStartScheduler();

	for (;;);
}
