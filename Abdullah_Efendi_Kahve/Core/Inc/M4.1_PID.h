/*
 * pid.h
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct {
    float Kp, Ki, Kd;
    float integral;
    float prev_error;
    float out_min, out_max;
    float alpha;
    float filtered_val;
} PID_Controller;

float low_pass_filter(PID_Controller *pid,
                      float new_val);

float compute_pid(PID_Controller *pid,
                  float setpoint,
                  float measured,
                  float dt);




#endif /* INC_PID_H_ */
