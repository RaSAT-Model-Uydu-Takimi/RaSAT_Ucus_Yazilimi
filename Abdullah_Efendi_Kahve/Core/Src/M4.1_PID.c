/*
 * pid.c
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#include "M4.1_PID.h"


float low_pass_filter(PID_Controller *pid,
                      float new_val)
{
    pid->filtered_val =
        (pid->alpha * new_val) +
        ((1.0f - pid->alpha) * pid->filtered_val);

    return pid->filtered_val;
}


float compute_pid(PID_Controller *pid,
                  float setpoint,
                  float measured,
                  float dt)
{
    float error = setpoint - measured;

    float P = pid->Kp * error;

    pid->integral += error * dt;

    float I = pid->Ki * pid->integral;

    float D = pid->Kd *
              (error - pid->prev_error) / dt;

    pid->prev_error = error;

    float output = P + I + D;

    if(output > pid->out_max)
    {
        output = pid->out_max;
        pid->integral -= error * dt;
    }

    else if(output < pid->out_min)
    {
        output = pid->out_min;
        pid->integral -= error * dt;
    }

    return output;
}

