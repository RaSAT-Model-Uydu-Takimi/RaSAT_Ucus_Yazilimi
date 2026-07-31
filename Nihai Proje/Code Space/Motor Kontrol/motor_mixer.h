/*
 * motor_mixer.h
 *
 * Collective, roll, pitch ve yaw komutlarini
 * dort motorun throttle degerlerine donusturur.
 */

#ifndef INC_MOTOR_MIXER_H_
#define INC_MOTOR_MIXER_H_

#include "motors.h"

void MotorMixer_Calculate(float collective,
                          float roll,
                          float pitch,
                          float yaw,
                          MotorOutputs_t *outputs);

#endif /* INC_MOTOR_MIXER_H_ */
