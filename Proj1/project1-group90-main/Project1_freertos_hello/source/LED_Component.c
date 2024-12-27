#include "LED_Component.h"
#include "fsl_gpio.h"
//#include "gpio_led_output.h"


QueueHandle_t led_queue;

void setupLEDComponent()
{
	setupLEDPins();

	setupLEDs();

    /*************** LED Task ***************/
	//Create LED Queue
	led_queue = xQueueCreate(1, sizeof(uint16_t));

	//Create LED Task
	BaseType_t status = xTaskCreate(ledTask, "Change led color", 200, NULL, 2, NULL);
		if (status != pdPASS)
		{
			PRINTF("Task creation failed!.\r\n");
			while (1);
		}
}

#define BOARD_LED_GPIO_D     GPIOD
#define BOARD_LED_GPIO_C     GPIOC

#define BOARD_LED_GPIO_PIN9 9
#define BOARD_LED_GPIO_PIN8 8
#define BOARD_LED_GPIO_PIN1 1

// Blue C8
// Green C9
// Red D1

void setupLEDPins()
{
	//Configure LED pins
	CLOCK_EnableClock(kCLOCK_PortD);
	CLOCK_EnableClock(kCLOCK_PortC);

	PORT_SetPinMux(PORTD, 1U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, 8U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, 9U, kPORT_MuxAsGpio);

	init_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8, 1, 0);
	init_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9, 1, 0);
	init_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1, 1, 0);
}

void setupLEDs()
{
	//Initialize PWM for the LEDs
}

void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

int lastcolor = 0;


void ledTask(void* pvParameters)
{
	uint16_t ch5;
	//LED task implementation
	while(1)
	{
		BaseType_t status = xQueueReceive(led_queue, &ch5, portMAX_DELAY);
				if (status != pdPASS)
				{
					PRINTF("Queue Receive failed!.\r\n");
					while (1);
				}
//		ch5 = ch5 - 65536000;
//		printf("channel 5: %d \n", ch5);

		if(ch5 != lastcolor)
		{
			if(lastcolor == 1000)
			{
//				delay();
				toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8);
//				delay();
			}
			else if(lastcolor == 1500)
			{
//				delay();
				toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9);
//				delay();
			}
			else if(lastcolor == 2000)
			{
//				delay();
				toggle_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1);
//				delay();
			}


			if(ch5 == 1000)
					{
//						delay();
						toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8);
//						delay();
					}
					else if(ch5 == 1500)
					{
//						delay();
						toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9);
//						delay();
					}
					else if(ch5 == 2000)
					{
//						delay();
						toggle_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1);
//						delay();
					}


		}



		lastcolor = ch5;
	}
}

void init_Pin(GPIO_Port *base, uint32_t pin, uint8_t pinDirection, uint8_t outputLogic)
{
//    assert((NULL != pinDirection) && NULL != outputLogic);

    if (pinDirection == kGPIO_DigitalInput)
    {
        base->PDDR &= GPIO_FIT_REG(~(1UL << pin));
    }
    else
    {
        GPIO_PinWrite(base, pin, outputLogic);
        base->PDDR |= GPIO_FIT_REG((1UL << pin));
    }
}

void toggle_Port(GPIO_Port *base, uint32_t pin)
{
//#if !(defined(FSL_FEATURE_GPIO_HAS_NO_INDEP_OUTPUT_CONTROL) && FSL_FEATURE_GPIO_HAS_NO_INDEP_OUTPUT_CONTROL)
	base->PTOR = GPIO_FIT_REG(pin);
//#else
//    base->PDOR ^= GPIO_FIT_REG(pin);
//#endif
}

void toggle_Pin(GPIO_Port *base, uint32_t pin){
    toggle_Port(base, 1u << pin);
}
