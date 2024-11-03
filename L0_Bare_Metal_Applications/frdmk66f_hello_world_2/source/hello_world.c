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

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

void pwm_setup()
{
	ftm_config_t ftmInfo;
	ftm_chnl_pwm_signal_param_t ftmParam[3];
	// Blue C8
	// Green C9
	// Red D1

	// D1, Red
	ftmParam[0].chnlNumber = kFTM_Chnl_1;
	ftmParam[0].level = kFTM_HighTrue;
	ftmParam[0].dutyCyclePercent = 0;
	ftmParam[0].firstEdgeDelayPercent = 0U;
	ftmParam[0].enableComplementary = false;
	ftmParam[0].enableDeadtime = false;

	// C8, Blue
	ftmParam[1].chnlNumber = kFTM_Chnl_4;
	ftmParam[1].level = kFTM_HighTrue;
	ftmParam[1].dutyCyclePercent = 0;
	ftmParam[1].firstEdgeDelayPercent = 0U;
	ftmParam[1].enableComplementary = false;
	ftmParam[1].enableDeadtime = false;

	// C9, Green
	ftmParam[2].chnlNumber = kFTM_Chnl_5;
	ftmParam[2].level = kFTM_HighTrue;
	ftmParam[2].dutyCyclePercent = 0;
	ftmParam[2].firstEdgeDelayPercent = 0U;
	ftmParam[2].enableComplementary = false;
	ftmParam[2].enableDeadtime = false;

	FTM_GetDefaultConfig(&ftmInfo);

	FTM_Init(FTM3, &ftmInfo);
	FTM_SetupPwm(FTM3, &ftmParam, 3U, kFTM_EdgeAlignedPwm, 5000U, CLOCK_GetFreq(kCLOCK_BusClk));
	FTM_StartTimer(FTM3, kFTM_SystemClock);

}

int main(void)
{
	char ch;
	int* duty_cycle;
//	int* RGB_values[3];

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	pwm_setup();

	printf("Please enter a 6-digit hex color scheme: ");

	scanf("%x", &duty_cycle);

	int val = duty_cycle;

	int red = (((val >> 16) & 0xFF) * 100) / 255;
	int green = (((val >> 8) & 0xFF) * 100) / 255;
	int blue = ((val & 0xFF) * 100) / 255;

	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_1, kFTM_EdgeAlignedPwm, red);
	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_5, kFTM_EdgeAlignedPwm, green);
	FTM_UpdatePwmDutycycle(FTM3, kFTM_Chnl_4, kFTM_EdgeAlignedPwm, blue);
	FTM_SetSoftwareTrigger(FTM3, true);


}
