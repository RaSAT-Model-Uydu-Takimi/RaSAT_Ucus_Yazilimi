/*
 * motors.c
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#include "motors.h"
#include "config.h"
#include "main.h"

static float currentThrottle = 0.0f;

static uint16_t currentPWM = ESC_CUTOFF_US;

static uint8_t motorsArmed = 0;


void Motors_Init(void)
{
    currentThrottle = 0.0f;

    currentPWM = ESC_CUTOFF_US;

    motorsArmed = 0;

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    Motors_StopAll();
}

void Motors_Arm(void)
{
    motorsArmed = 1;

    currentPWM = ESC_ARM_US;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, currentPWM);
}

void Motors_Disarm(void)
{
    motorsArmed = 0;

    Motors_StopAll();
}

void Motors_StopAll(void)
{
    currentThrottle = 0.0f;

    currentPWM = ESC_CUTOFF_US;

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, currentPWM);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, currentPWM);
}


void Motors_SetThrottle(float throttle)
{
	if(throttle > PWM_MAX)
	{
	    throttle = PWM_MAX;
	}

	if(throttle < PWM_MIN)
	{
	    throttle = PWM_MIN;
	}

	if(!motorsArmed)
	{
	    Motors_StopAll();
	    return;
	}

	currentThrottle = throttle;

	currentPWM =
	ESC_MIN_US +
	(uint16_t)(throttle *
	(ESC_MAX_US - ESC_MIN_US));

	__HAL_TIM_SET_COMPARE(&htim1,
	                      TIM_CHANNEL_1,
	                      currentPWM);

	__HAL_TIM_SET_COMPARE(&htim1,
	                      TIM_CHANNEL_2,
	                      currentPWM);

	__HAL_TIM_SET_COMPARE(&htim1,
	                      TIM_CHANNEL_3,
	                      currentPWM);

	__HAL_TIM_SET_COMPARE(&htim1,
	                      TIM_CHANNEL_4,
	                      currentPWM);

}

float Motors_GetThrottle(void)
{
    return currentThrottle;
}

uint16_t Motors_GetPWM(void)
{
    return currentPWM;
}


