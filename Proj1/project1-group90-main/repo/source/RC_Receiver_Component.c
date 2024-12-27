#include "RC_Receiver_Component.h"
#include "Motor_Control_Component.h"
#include "LED_Component.h"

SemaphoreHandle_t rc_hold_semaphore;
TaskHandle_t rc_task_handle;

void setupRCReceiverComponent()
{
	setupRCPins();

	setupUART_RC();

    /*************** RC Task ***************/
	//Create RC Semaphore
	rc_hold_semaphore = xSemaphoreCreateBinary();
	//Create RC Task
	BaseType_t status = xTaskCreate(rcTask, "Read RC Channels", 200, (void*)rc_hold_semaphore, 2, rc_task_handle);

	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

}

void setupRCPins()
{
	//Configure RC pins
	CLOCK_EnableClock(kCLOCK_PortC);
    PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);
}

void setupUART_RC()
{
	//setup UART for RC receiver
	uart_config_t config;
	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 115200;
	config.enableTx = false;
	config.enableRx = true;
	UART_Init(UART1, &config, CLOCK_GetFreq(kCLOCK_CoreSysClk));

}

void rcTask(void* pvParameters)
{
	//RC task implementation
	while (1)
	{
		// take the motor, accel semaphores
		RC_Values rc_values;
		uint8_t* ptr = (uint8_t*) &rc_values;
		UART_ReadBlocking(UART1, ptr, 1);

		if(*ptr != 0x20)
			continue;
		UART_ReadBlocking(UART1, &ptr[1], sizeof(rc_values) - 1);

		if(rc_values.header == 0x4020)
		{

			float motor_speed;
			float motor_angle;

			int direction;
			float speed_offset = 0.065;
			float angle_offset = 0.09;

			if(rc_values.ch6 == 1000) direction = -1;
			else direction = 1;

			motor_speed = speed_offset + ((rc_values.ch3-1000.0f)/1000.0f)*(rc_values.ch5/2000.0f)*(0.025)*(direction);
			motor_angle = angle_offset - ((rc_values.ch1-1000.0f)/1000.0f)*(0.05);

			BaseType_t status = xQueueSendToBack(motor_queue, &motor_speed, portMAX_DELAY);
			if (status != pdPASS)
			{
				PRINTF("Queue Send failed!.\r\n");
				while (1);
			}

			status = xQueueSendToBack(angle_queue, &motor_angle, portMAX_DELAY);
			if (status != pdPASS)
			{
				PRINTF("Queue Send failed!.\r\n");
				while (1);
			}

			status = xQueueSendToBack(led_queue, &rc_values.ch5, portMAX_DELAY);
			if (status != pdPASS)
			{
				PRINTF("Queue Send failed!.\r\n");
				while (1);
			}
//			vTaskDelay(100 / portTICK_PERIOD_MS);

			// give rc semaphore

		}
	}
}



// channel 1: right joystick horizontal. 1000 (left), 1500 (neutral), 2000 (right)
// channel 2: right joystick vertical. 1000 (down), 1500 (neutral), 2000 (up)
// channel 3: left joystick vertical. 1000 (down), 1500 (neutral), 2000 (up)
// channel 4: left joystick horizontal. 1000(left), 1500 (neutral), 2000 (right)

// channel 5 1000 (up), 1500 (neutral), 2000 (down)
// channel 6 1000 (up), 2000 (down)


