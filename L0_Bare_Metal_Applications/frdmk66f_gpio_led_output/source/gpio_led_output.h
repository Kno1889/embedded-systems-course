/*
 * gpio_led_output.h
 *
 *  Created on: Jan 22, 2024
 *      Author: hassak9
 */

#ifndef GPIO_LED_OUTPUT_H_
#define GPIO_LED_OUTPUT_H_

#include "core_cm4.h"                  /* Core Peripheral Access Layer */

#define GPIO_C_BASE                               (0x400FF080u)
/** Peripheral GPIOC base pointer */
#define GPIO_C                                    ((GPIO_Port *)GPIO_C_BASE)
/** Peripheral GPIOD base address */
#define GPIO_D_BASE                               (0x400FF0C0u)
/** Peripheral GPIOD base pointer */
#define GPIO_D                                    ((GPIO_Port *)GPIO_D_BASE)

#define BOARD_LED_GPIO_D     GPIO_D
#define BOARD_LED_GPIO_C     GPIO_C

#define BOARD_LED_GPIO_PIN9 9
#define BOARD_LED_GPIO_PIN8 8
#define BOARD_LED_GPIO_PIN1 1

/** GPIO - Register Layout Typedef */
typedef struct {
  __IO uint32_t PDOR;                              /**< Port Data Output Register, offset: 0x0 */
  __O  uint32_t PSOR;                              /**< Port Set Output Register, offset: 0x4 */
  __O  uint32_t PCOR;                              /**< Port Clear Output Register, offset: 0x8 */
  __O  uint32_t PTOR;                              /**< Port Toggle Output Register, offset: 0xC */
  __I  uint32_t PDIR;                              /**< Port Data Input Register, offset: 0x10 */
  __IO uint32_t PDDR;                              /**< Port Data Direction Register, offset: 0x14 */
} GPIO_Port;

void init_Pin(GPIO_Port *base, uint32_t pin, gpio_pin_direction_t pinDirection, uint8_t outputLogic);
uint32_t read_Pin(GPIO_Port *base, uint32_t pin);
void toggle_Port(GPIO_Port *base, uint32_t pin);
void B_G_R_Pattern();
void toggle_Pin(GPIO_Port *base, uint32_t pin);

#endif /* GPIO_LED_OUTPUT_H_ */
