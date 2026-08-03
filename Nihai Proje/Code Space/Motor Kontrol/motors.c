/*
 * motors.c
 *
 * Dört ESC/motorun ayrı ayrı PWM ile kontrol edilmesi.
 */

#include "motors.h"
#include "Konfigurasyonlar.h"

static TIM_HandleTypeDef *motor_timer = NULL;

static const uint32_t motor_channels[MOTOR_COUNT] =
{
    TIM_CHANNEL_1,
    TIM_CHANNEL_2,
    TIM_CHANNEL_3,
    TIM_CHANNEL_4
};

static float current_throttle[MOTOR_COUNT] = {0.0f};
static uint16_t current_pwm[MOTOR_COUNT] = {0U};
static uint8_t motors_armed = 0U;

static float ClampThrottle(float throttle)
{
    if (throttle < 0.0f)
    {
        return 0.0f;
    }

    if (throttle > 1.0f)
    {
        return 1.0f;
    }

    return throttle;
}

static uint16_t ThrottleToPWM(float throttle)
{
    throttle = ClampThrottle(throttle);

    return (uint16_t)
    (
        ESC_MIN_US +
        throttle * (float)(ESC_MAX_US - ESC_MIN_US)
    );
}

static void WriteMotorPWM(MotorId_t motor, uint16_t pwm)
{
    if ((motor_timer == NULL) || (motor >= MOTOR_COUNT))
    {
        return;
    }

    current_pwm[motor] = pwm;

    __HAL_TIM_SET_COMPARE(
        motor_timer,
        motor_channels[motor],
        pwm
    );
}

uint8_t Motors_Init(TIM_HandleTypeDef *timer)
{
    if (timer == NULL)
    {
        return 0U;
    }

    motor_timer = timer;
    motors_armed = 0U;

    for (uint8_t i = 0U; i < MOTOR_COUNT; i++)
    {
        current_throttle[i] = 0.0f;
        WriteMotorPWM((MotorId_t)i, ESC_CUTOFF_US);

        if (HAL_TIM_PWM_Start(motor_timer, motor_channels[i]) != HAL_OK)
        {
            return 0U;
        }
    }

    return 1U;
}

void Motors_Arm(void)
{
    if (motor_timer == NULL)
    {
        return;
    }

    motors_armed = 1U;

    for (uint8_t i = 0U; i < MOTOR_COUNT; i++)
    {
        current_throttle[i] = 0.0f;
        WriteMotorPWM((MotorId_t)i, ESC_ARM_US);
    }
}

void Motors_Disarm(void)
{
    motors_armed = 0U;
    Motors_StopAll();
}

uint8_t Motors_IsArmed(void)
{
    return motors_armed;
}

void Motors_StopAll(void)
{
    for (uint8_t i = 0U; i < MOTOR_COUNT; i++)
    {
        current_throttle[i] = 0.0f;
        WriteMotorPWM((MotorId_t)i, ESC_CUTOFF_US);
    }
}

void Motors_SetMotorThrottle(MotorId_t motor, float throttle)
{
    if ((motor_timer == NULL) || (motor >= MOTOR_COUNT))
    {
        return;
    }

    if (motors_armed == 0U)
    {
        current_throttle[motor] = 0.0f;
        WriteMotorPWM(motor, ESC_CUTOFF_US);
        return;
    }

    throttle = ClampThrottle(throttle);

    current_throttle[motor] = throttle;
    WriteMotorPWM(motor, ThrottleToPWM(throttle));
}

void Motors_SetThrottle(float throttle)
{
    for (uint8_t i = 0U; i < MOTOR_COUNT; i++)
    {
        Motors_SetMotorThrottle((MotorId_t)i, throttle);
    }
}

void Motors_SetOutputs(const MotorOutputs_t *outputs)
{
    if (outputs == NULL)
    {
        return;
    }

    Motors_SetMotorThrottle(MOTOR_1, outputs->m1);
    Motors_SetMotorThrottle(MOTOR_2, outputs->m2);
    Motors_SetMotorThrottle(MOTOR_3, outputs->m3);
    Motors_SetMotorThrottle(MOTOR_4, outputs->m4);
}

float Motors_GetMotorThrottle(MotorId_t motor)
{
    if (motor >= MOTOR_COUNT)
    {
        return 0.0f;
    }

    return current_throttle[motor];
}

uint16_t Motors_GetMotorPWM(MotorId_t motor)
{
    if (motor >= MOTOR_COUNT)
    {
        return 0U;
    }

    return current_pwm[motor];
}
