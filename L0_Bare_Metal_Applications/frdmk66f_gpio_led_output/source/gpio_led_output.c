/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "gpio_led_output.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
// Blue C8
// Green C9
// Red D1

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 800000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

void toggle_Port(GPIO_Port *base, uint32_t pin)
{
	base->PTOR = pin;
}


uint32_t read_Pin(GPIO_Port *base, uint32_t pin)
{
    return (((uint32_t)(base->PDIR) >> pin) & 0x01UL);
}

void init_Pin(GPIO_Port *base, uint32_t pin, uint8_t pinDirection, uint8_t outputLogic)
{

        GPIO_PinWrite(base, pin, outputLogic);
        base->PDDR |= (1UL << pin);
}

void toggle_Pin(GPIO_Port *base, uint32_t pin){
    toggle_Port(base, 1u << pin);
}

void B_G_R_Pattern(){
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8);
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8);
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9);
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9);
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1);
        delay();
        delay();
        toggle_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1);
        delay();
        delay();
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        0,
    };


    /* Board pin, clock, debug console init */
    CLOCK_EnableClock(kCLOCK_PortD);
    CLOCK_EnableClock(kCLOCK_PortC);


    /* PORTC9 (pin D7) is configured as PTC9 */
    PORT_SetPinMux(PORTD, 1U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTC, 8U, kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTC, 9U, kPORT_MuxAsGpio);
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver example\r\n");
    PRINTF("\r\n The LED is blinking.\r\n");

    /* Init output LED GPIO. */
    init_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN8, 1, 0);
    init_Pin(BOARD_LED_GPIO_C, BOARD_LED_GPIO_PIN9, 1, 0);
    init_Pin(BOARD_LED_GPIO_D, BOARD_LED_GPIO_PIN1, 1, 0);

    while (1)
    {
        B_G_R_Pattern();
    }
}
