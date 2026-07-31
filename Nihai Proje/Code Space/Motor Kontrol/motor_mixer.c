/*
 * motor_mixer.c
 *
 * Varsayilan motor yerlesimi:
 *
 *                 ON
 *
 *       M1 On-Sol CCW      M2 On-Sag CW
 *
 *       M4 Arka-Sol CW     M3 Arka-Sag CCW
 *
 * Gercek motor yerlesimi veya donus yonleri farkliysa
 * mixer isaretleri testlerden sonra duzeltilmeli.
 */

#include "motor_mixer.h"
#include <stddef.h>

/* Throttle degerini 0.0 - 1.0 araliginda tutar. */
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

    /*
     * collective: Dort motorun ortak temel gucu
     * roll:       Sag-sol denge duzeltmesi
     * pitch:      On-arka denge duzeltmesi
     * yaw:        CW-CCW motor cifti duzeltmesi
     */
    outputs->m1 = collective - roll + pitch + yaw;
    outputs->m2 = collective + roll + pitch - yaw;
    outputs->m3 = collective + roll - pitch + yaw;
    outputs->m4 = collective - roll - pitch - yaw;

    outputs->m1 = ClampThrottle(outputs->m1);
    outputs->m2 = ClampThrottle(outputs->m2);
    outputs->m3 = ClampThrottle(outputs->m3);
    outputs->m4 = ClampThrottle(outputs->m4);
}
