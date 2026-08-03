/*
 * motor_mixer.c
 *
 * Govde koordinat sistemi:
 *
 * +X: Aracin sagi
 * +Y: Aracin onu
 * +Z: Aracin alti / ekranin ici
 *
 * Roll : Y ekseni etrafinda
 * Pitch: X ekseni etrafinda
 * Yaw  : Z ekseni etrafinda
 *
 * Motor yerlesimi:
 *
 *                 -Y / Arka
 *
 *       M3 Arka-Sol CCW    M4 Arka-Sag CW
 *
 *       M2 On-Sol CW       M1 On-Sag CCW
 *
 *                 +Y / On
 *
 * Motor itkisi -Z yonundedir.
 */

#include "motor_mixer.h"
#include <stddef.h>


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


void MotorMixer_Calculate(float collective,
                          float roll,
                          float pitch,
                          float yaw,
                          MotorOutputs_t *outputs)
{
    if (outputs == NULL)
    {
        return;
    }

    outputs->m1 = collective + roll - pitch + yaw;
    outputs->m2 = collective - roll - pitch - yaw;
    outputs->m3 = collective - roll + pitch + yaw;
    outputs->m4 = collective + roll + pitch - yaw;

    outputs->m1 = ClampThrottle(outputs->m1);
    outputs->m2 = ClampThrottle(outputs->m2);
    outputs->m3 = ClampThrottle(outputs->m3);
    outputs->m4 = ClampThrottle(outputs->m4);
}