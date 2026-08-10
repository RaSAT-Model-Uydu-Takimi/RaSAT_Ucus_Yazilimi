/*
 * M3.2_Attitude.c
 *
 * Yönelim (Pitch ve Roll) Hesaplama Modülü
 */

#include "M3.2_Attitude.h"
#include "M0.1_FilterConfig.h"
#include <math.h>

void M3_2_Attitude_Update(DataCenter *dc) {
    float q0 = dc->estimated.q0;
    float q1 = dc->estimated.q1;
    float q2 = dc->estimated.q2;
    float q3 = dc->estimated.q3;

    // Convert quaternions to Euler angles (Roll, Pitch)
    // NWU (North-West-Up) sisteminde:
    // +Roll  = Sağa Yatma (Right wing down)
    // +Pitch = Burun Aşağı (Nose down)
    
    dc->estimated.roll  = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2) * RAD_TO_DEG;
    dc->estimated.pitch = asinf(-2.0f * (q1*q3 - q0*q2)) * RAD_TO_DEG;
}
