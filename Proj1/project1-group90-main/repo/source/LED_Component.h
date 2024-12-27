#ifndef LED_COMPONENT_H
#define LED_COMPONENT_H

#include "pin_mux.h"
#include "fsl_port.h"
#include "fsl_ftm.h"
#include "fsl_debug_console.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Motor_Control_Component.h"
#include "Terminal_Component.h"

#include <stdarg.h>

#define FTM_LED 				FTM3
#define FTM_RED_CHANNEL			//Define red LED channel
#define FTM_GREEN_CHANNEL		//Define green LED channel
#define FTM_BLUE_CHANNEL		//Define blue LED channel

extern QueueHandle_t led_queue;

void setupLEDComponent();
void setupLEDs();
void setupLEDPins();

void ledTask(void* pvParameters);


typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
} GPIO_Port;

#endif /* TERMINAL_COMPONENT_H */
