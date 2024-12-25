/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_ftm.h"
#include "fsl_uart.h"

#define FTM_MOTOR FTM0
#define FTM_CHANNEL_DC_MOTOR kFTM_Chnl_0

//#define SERVO_MOTOR FTM3
#define FTM_CHANNEL_SERVO kFTM_Chnl_3

#define TARGET_UART UART4

volatile char ch;
volatile int new_char = 0;

/*******************************************************************************
 * Definitions
 ******************************************************************************/

void UART4_RX_TX_IRQHandler()
{
	UART_GetStatusFlags(TARGET_UART);
	ch = UART_ReadByte(TARGET_UART);
	new_char = 1;
}

void setupUART()
{
	uart_config_t config;

	UART_GetDefaultConfig(&config);
	config.baudRate_Bps = 57600;
	config.enableTx = true;
	config.enableRx = true;
	config.enableRxRTS = true;
	config.enableTxCTS = true;

	UART_Init(TARGET_UART, &config, CLOCK_GetFreq(kCLOCK_BusClk));
	/******** Enable Interrupts *********/
	UART_EnableInterrupts(TARGET_UART, kUART_RxDataRegFullInterruptEnable);
	EnableIRQ(UART4_RX_TX_IRQn);
}


void setupPWM_DC()
{
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
	ftmParam.chnlNumber = FTM_CHANNEL_DC_MOTOR;
	ftmParam.level = pwmLevel;
	ftmParam.dutyCyclePercent = 7;
	ftmParam.firstEdgeDelayPercent = 0U;
	ftmParam.enableComplementary = false;
	ftmParam.enableDeadtime = false;
	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_128;
	FTM_Init(FTM_MOTOR, &ftmInfo);
	FTM_SetupPwm(FTM_MOTOR, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM_MOTOR, kFTM_SystemClock);
}

void setupPWM_SERVO()
{
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam;
	ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
	ftmParam.chnlNumber = FTM_CHANNEL_SERVO;
	ftmParam.level = pwmLevel;
	ftmParam.dutyCyclePercent = 7;
	ftmParam.firstEdgeDelayPercent = 0U;
	ftmParam.enableComplementary = false;
	ftmParam.enableDeadtime = false;
	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_128;
	FTM_Init(FTM_MOTOR, &ftmInfo);
	FTM_SetupPwm(FTM_MOTOR, &ftmParam, 1U, kFTM_EdgeAlignedPwm, 50U, CLOCK_GetFreq(
	kCLOCK_BusClk));
	FTM_StartTimer(FTM_MOTOR, kFTM_SystemClock);
}

void updatePWM_dutyCycle(ftm_chnl_t channel, float dutyCycle)
{
	uint32_t cnv, cnvFirstEdge = 0, mod;
	/* The CHANNEL_COUNT macro returns -1 if it cannot match the FTM instance */
	assert(-1 != FSL_FEATURE_FTM_CHANNEL_COUNTn(FTM_MOTOR));
	mod = FTM_MOTOR->MOD;
	if (dutyCycle == 0U)
	{
	/* Signal stays low */
		cnv = 0;
	}
	else
	{
		cnv = mod * dutyCycle;
	/* For 100% duty cycle */
	if (cnv >= mod)
	{
		cnv = mod + 1U;
	}
	}
	FTM_MOTOR->CONTROLS[channel].CnV = cnv;
}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */


int main(void)
{
	char ch3[] = "0000000000";
	char received;
	char txbuff[] = "Hello World\r\n";

	int input1;
	int input2;
	float dutyCycle1;
	float dutyCycle2;

	int ch1_counter = 0;
	int ch2_counter = 0;

	/* Init board hardware*/
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	setupPWM_DC();
	setupPWM_SERVO();

	setupUART();
	/********Delay********/
	for(volatile int i = 0U; i < 10000000; i++)
		__asm("NOP");

	PRINTF("%s", txbuff);

	UART_WriteBlocking(TARGET_UART, txbuff, sizeof(txbuff) - 1);

	while(1)
	{
		//UART_ReadBlocking(TARGET_UART, &received, 1);
		//PRINTF("%c", received);


		for(int i=0;i<10;i++)
		{
			UART_ReadBlocking(TARGET_UART, &received, 1);
			PRINTF("%c", received);

			ch3[i] = received;
			if(received == '.')
			{
				PRINTF("\n");
				break;
			}
		}

		PRINTF("%s\n", ch);

		for(int i=0;i<4;i++)
		{
			if(ch3[i] == ',')
			{
				break;
			}
			else
			{
				ch1_counter++;
			}
		}
		for(int i=0;i<4;i++)
		{
			if(ch3[i + ch1_counter + 1] == '.')
			{
				break;
			}
			else
			{
				ch2_counter++;
			}
		}

		PRINTF("\n%d\n", ch1_counter);
		PRINTF("%d\n\n", ch2_counter);

		//ch1_counter--;
		//ch2_counter--;

		char ch1[ch1_counter];
		char ch2[ch2_counter];

		for(int i=0;i<ch1_counter;i++)
		{
			ch1[i] = ch3[i];
		}

//		ch2[ch2_counter + 1] = "\0";

		for(int i=0;i<ch2_counter;i++)
		{
			ch2[i] = ch3[i + ch1_counter + 1];
		}

		int num1 = atoi(ch1);
		int num2 = atoi(ch2);

		printf("%d\n", num1);
		printf("%d\n", num2);

//		PRINTF("%s\n", &ch1);
//		PRINTF("%s\n", &ch2);

		updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, num1 * 0.025f/100.0f + 0.07);
		updatePWM_dutyCycle(FTM_CHANNEL_SERVO, num2 * 0.025f/100.0f + 0.0775);

		FTM_SetSoftwareTrigger(FTM_MOTOR, true);

		ch1_counter = 0;
		ch2_counter = 0;


		//0123,5678.
		//-100,-100.
	}

}
