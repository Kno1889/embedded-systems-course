#ifndef RC_RECEIVER_COMPONENT_H
#define RC_RECEIVER_COMPONENT_H

#include "pin_mux.h"
#include "fsl_port.h"
#include "fsl_uart.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Motor_Control_Component.h"
#include "LED_Component.h"

#define RC_UART		//Define UART for RC receiver

extern SemaphoreHandle_t rc_hold_semaphore;
extern TaskHandle_t rc_task_handle;

void setupRCReceiverComponent();
void setupUART_RC();
void setupRCPins();

typedef struct {
	uint16_t header;
	uint16_t ch1;
	uint16_t ch2;
	uint16_t ch3;
	uint16_t ch4;
	uint16_t ch5;
	uint16_t ch6;
} RC_Values;

void rcTask(void* pvParameters);

#endif /* RC_RECEIVER_COMPONENT_H */
