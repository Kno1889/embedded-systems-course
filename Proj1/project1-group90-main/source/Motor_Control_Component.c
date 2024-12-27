#include "Motor_Control_Component.h"
#include "Accelerometer_Component.h"

QueueHandle_t motor_queue;
QueueHandle_t angle_queue;
QueueHandle_t tilt_queue;

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_ftm.h"

#define FTM_MOTOR FTM0
#define FTM_CHANNEL_DC_MOTOR kFTM_Chnl_0

//#define SERVO_MOTOR FTM3
#define FTM_CHANNEL_SERVO kFTM_Chnl_3

void setupMotorComponent()
{
	setupMotorPins();

	setupDCMotor();
	setupServo();

    /*************** Motor Task ***************/
	//Create Motor Queue
	motor_queue = xQueueCreate(1, sizeof(float));

	//Create Motor Task
	BaseType_t status = xTaskCreate(motorTask, "Set DC Motor Speed", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

    /*************** Position Task ***************/
	//Create Angle Queue
	angle_queue = xQueueCreate(1, sizeof(float));

	//Create Position Task
	status = xTaskCreate(positionTask, "Set Servo Motor Angle", 200, NULL, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	tilt_queue = xQueueCreate(1, sizeof(int16_t));


}

void setupMotorPins()
{
    //Configure PWM pins for DC and Servo motors
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortA);

	PORT_SetPinMux(PORTC, 1U, kPORT_MuxAlt4);
	PORT_SetPinMux(PORTA, 6U, kPORT_MuxAlt3);
}

void setupDCMotor()
{
	//Initialize PWM for DC motor
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

void setupServo()
{
	//Initialize PWM for Servo motor
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
	assert(-1 != FSL_FEATURE_FTM_CHANNEL_COUNTn(FTM_MOTORS));

	mod = FTM_MOTORS->MOD;
	if(dutyCycle == 0U)
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

	FTM_MOTORS->CONTROLS[channel].CnV = cnv;
}

void motorTask(void* pvParameters)
{
	//Motor task implementation
	// give motor sem
	// take rc sema
	float motor_speed;
	int16_t tilt;
	while(1){
		BaseType_t status = xQueueReceive(motor_queue, &motor_speed, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Receive failed!.\r\n");
			while (1);
		}

		status = xQueueReceive(tilt_queue, &tilt, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Receive failed!.\r\n");
			while (1);
		}

		//printf("Tilt received: %d\n", tilt);
		//.printf("Motor speed received: %f\n", motor_speed);
		//printf("Motor speed received: %f\n", (motor_speed - (((float)tilt)) / 90.0) * 0.01);
//		printf("%f",  motor_speed - (((float)tilt) / 90.0) * 0.05);
		//updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, motor_speed*(float)tilt/90.0);
		updatePWM_dutyCycle(FTM_CHANNEL_DC_MOTOR, motor_speed - (((float)tilt)) / 90.0 * 0.01);
		FTM_SetSoftwareTrigger(FTM_MOTOR, true);
	}
}

void positionTask(void* pvParameters)
{
	//Position task implementation
	float motor_angle;
	while(1){
		BaseType_t status = xQueueReceive(angle_queue, &motor_angle, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Queue Receive failed!.\r\n");
			while (1);
		}
//		printf("Motor angle received: %f\n", motor_angle);
		updatePWM_dutyCycle(FTM_CHANNEL_SERVO, motor_angle);
		FTM_SetSoftwareTrigger(FTM_MOTOR, true);
	}
}
