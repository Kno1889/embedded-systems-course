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
#include "event_groups.h"

#define prod_bit (1 << 0)
#define con1_bit (1 << 1)
#define con2_bit (1 << 2)

int counter = 0;

void producer(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;

	while(1)
	{
		counter++;
		xEventGroupSetBits(event_group, con1_bit);
		xEventGroupSetBits(event_group, con2_bit);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}


void consumer1(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	EventBits_t bits;

	while(1)
	{
		bits = xEventGroupWaitBits(event_group, con1_bit | con2_bit, pdTRUE, pdFALSE, 
		portMAX_DELAY);

		if((bits & con1_bit) == con1_bit){
			PRINTF("Value Received by Consumer 1 = %d\r\n", counter);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

void consumer2(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	EventBits_t bits;

	while(1)
	{
		bits = xEventGroupWaitBits(event_group, con1_bit | con2_bit, pdTRUE, pdFALSE, portMAX_DELAY);

		if((bits & con2_bit) == con2_bit){
			PRINTF("Value Received by Consumer 2 = %d\r\n", counter);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
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

	EventGroupHandle_t event_group = xEventGroupCreate();

	status = xTaskCreate(producer, "producer", 200, (void*)event_group, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer1, "consumer", 200, (void*)event_group, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer2, "consumer", 200, (void*)event_group, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	vTaskStartScheduler();
	while (1)
	{}
}










