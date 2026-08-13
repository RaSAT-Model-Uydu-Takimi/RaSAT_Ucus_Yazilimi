/*
 * M3.0_FilterSystem.c
 *
 * 4-State Quaternion Extended Kalman Filter (EKF) Implementation
 * Dynamically adjusts R matrix based on vibration (acceleration magnitude).
 */

#include "M3.0_FilterSystem.h"
#include "M0.1_FilterConfig.h"
#include "M3.3_Yaw.h"
#include "M3.4_Altitude.h"
#include <math.h>

// EKF State Covariance Matrix (4x4)
static float P[4][4];

// EKF Process Noise Matrix (4x4)
static float Q[4][4];

// EKF Measurement Noise Matrix (3x3)
static float R_base[3][3];

// Helper: 3x3 Matrix Inversion using Cramer's Rule
static int invert3x3(float m[3][3], float minv[3][3]) {
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    if (fabsf(det) < 1e-6f) return 0; // Singular matrix

    float invdet = 1.0f / det;

    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

    return 1;
}

// Fast Inverse Square Root
static float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

void FilterSystem_Init(DataCenter *dc) {
    // Initial Quaternion
    dc->estimated.q0 = 1.0f;
    dc->estimated.q1 = 0.0f;
    dc->estimated.q2 = 0.0f;
    dc->estimated.q3 = 0.0f;
    
    // Reset Euler
    dc->estimated.roll = 0.0f;
    dc->estimated.pitch = 0.0f;
    
    // Yaw ve Kalman durumunu sıfırla (Kullanıcı isteği üzerine Filter Init içinden çağrılıyor)
    M3_3_Yaw_Init(dc);
    M3_4_Altitude_Init(dc);
    
    // Initialize P Matrix
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            P[i][j] = (i == j) ? EKF_INITIAL_P : 0.0f;
            Q[i][j] = 0.0f;
        }
    }
    
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            R_base[i][j] = 0.0f;
        }
    }
    
    // Read calibrated variances if available, otherwise use defaults
    float q_var = (dc->calibProfile.gyro_noise_x > 0) ? dc->calibProfile.gyro_noise_x : EKF_INITIAL_Q_GYRO;
    float r_var = (dc->calibProfile.acc_noise_x > 0) ? dc->calibProfile.acc_noise_x : EKF_INITIAL_R_ACCEL;
    
    // Q is 4x4 (mapping 3D gyro noise to 4D quaternion derivative noise)
    // Simplified: Just use scalar variance on diagonals
    Q[0][0] = q_var; Q[1][1] = q_var; Q[2][2] = q_var; Q[3][3] = q_var;
    
    // R is 3x3 for accelerometer
    R_base[0][0] = r_var; R_base[1][1] = r_var; R_base[2][2] = r_var;
}

void FilterSystem_Update(DataCenter *dc, float dt) {
    float q0 = dc->estimated.q0;
    float q1 = dc->estimated.q1;
    float q2 = dc->estimated.q2;
    float q3 = dc->estimated.q3;

    float gx = dc->gyro.calibrated_x * DEG_TO_RAD;
    float gy = dc->gyro.calibrated_y * DEG_TO_RAD;
    float gz = dc->gyro.calibrated_z * DEG_TO_RAD;
    
    float ax = dc->acc.calibrated_x; 
    float ay = dc->acc.calibrated_y;
    float az = dc->acc.calibrated_z;

    // ---------------------------------------------------------
    // 1. PREDICT STEP (State and Covariance Projection)
    // ---------------------------------------------------------
    
    // State Transition Matrix F = I + 0.5 * dt * Omega
    float F[4][4] = {
        { 1.0f,        -0.5f*gx*dt, -0.5f*gy*dt, -0.5f*gz*dt },
        { 0.5f*gx*dt,   1.0f,        0.5f*gz*dt, -0.5f*gy*dt },
        { 0.5f*gy*dt,  -0.5f*gz*dt,  1.0f,        0.5f*gx*dt },
        { 0.5f*gz*dt,   0.5f*gy*dt, -0.5f*gx*dt,  1.0f       }
    };
    
    // Predict State X = F * X
    float q0_pred = F[0][0]*q0 + F[0][1]*q1 + F[0][2]*q2 + F[0][3]*q3;
    float q1_pred = F[1][0]*q0 + F[1][1]*q1 + F[1][2]*q2 + F[1][3]*q3;
    float q2_pred = F[2][0]*q0 + F[2][1]*q1 + F[2][2]*q2 + F[2][3]*q3;
    float q3_pred = F[3][0]*q0 + F[3][1]*q1 + F[3][2]*q2 + F[3][3]*q3;
    
    // Normalize predicted quaternion
    float norm = invSqrt(q0_pred*q0_pred + q1_pred*q1_pred + q2_pred*q2_pred + q3_pred*q3_pred);
    q0 = q0_pred * norm;
    q1 = q1_pred * norm;
    q2 = q2_pred * norm;
    q3 = q3_pred * norm;
    
    // Predict Covariance P = F * P * F^T + Q
    float FP[4][4] = {0};
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            for(int k=0; k<4; k++) {
                FP[i][j] += F[i][k] * P[k][j];
            }
        }
    }
    
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            P[i][j] = Q[i][j];
            for(int k=0; k<4; k++) {
                P[i][j] += FP[i][k] * F[j][k]; // F[j][k] is F^T[k][j]
            }
        }
    }

    // ---------------------------------------------------------
    // 2. UPDATE STEP (Measurement Incorporation)
    // ---------------------------------------------------------
    
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Calculate acceleration magnitude for dynamic R matrix (Vibration Rejection)
        float acc_mag = sqrtf(ax*ax + ay*ay + az*az);
        
        // Normalize accelerometer measurement
        float recipNorm = invSqrt(ax*ax + ay*ay + az*az);
        float zx = ax * recipNorm;
        float zy = ay * recipNorm;
        float zz = az * recipNorm;
        
        // Expected Measurement from current quaternion (Gravity mapped to body frame)
        float hx = 2.0f * (q1*q3 - q0*q2);
        float hy = 2.0f * (q0*q1 + q2*q3);
        float hz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
        // Measurement Residual Y = Z - h(X)
        float yx = zx - hx;
        float yy = zy - hy;
        float yz = zz - hz;
        
        // Jacobian Matrix H (3x4)
        float H[3][4] = {
            { -2.0f*q2,  2.0f*q3, -2.0f*q0,  2.0f*q1 },
            {  2.0f*q1,  2.0f*q0,  2.0f*q3,  2.0f*q2 },
            {  2.0f*q0, -2.0f*q1, -2.0f*q2,  2.0f*q3 }
        };
        
        // Dynamic R matrix: Increase measurement noise if vibration detected
        float R[3][3];
        float r_scale = 1.0f;
        if (fabsf(acc_mag - GRAVITY_MSS) > EKF_VIBRATION_THRESHOLD) {
            r_scale = EKF_R_MULTIPLIER; // Heavy vibration, trust accel less
        }
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                R[i][j] = R_base[i][j] * r_scale;
            }
        }
        
        // S = H * P * H^T + R
        float HP[3][4] = {0};
        for(int i=0; i<3; i++) {
            for(int j=0; j<4; j++) {
                for(int k=0; k<4; k++) {
                    HP[i][j] += H[i][k] * P[k][j];
                }
            }
        }
        
        float S[3][3] = {0};
        for(int i=0; i<3; i++) {
            for(int j=0; j<3; j++) {
                S[i][j] = R[i][j];
                for(int k=0; k<4; k++) {
                    S[i][j] += HP[i][k] * H[j][k]; // H[j][k] is H^T[k][j]
                }
            }
        }
        
        // S_inv = S^-1
        float Sinv[3][3];
        if(invert3x3(S, Sinv)) {
            // Kalman Gain K = P * H^T * S^-1
            float P_Ht[4][3] = {0};
            for(int i=0; i<4; i++) {
                for(int j=0; j<3; j++) {
                    for(int k=0; k<4; k++) {
                        P_Ht[i][j] += P[i][k] * H[j][k]; // H[j][k] is H^T[k][j]
                    }
                }
            }
            
            float K[4][3] = {0};
            for(int i=0; i<4; i++) {
                for(int j=0; j<3; j++) {
                    for(int k=0; k<3; k++) {
                        K[i][j] += P_Ht[i][k] * Sinv[k][j];
                    }
                }
            }
            
            // State Update X = X + K * Y
            q0 += K[0][0]*yx + K[0][1]*yy + K[0][2]*yz;
            q1 += K[1][0]*yx + K[1][1]*yy + K[1][2]*yz;
            q2 += K[2][0]*yx + K[2][1]*yy + K[2][2]*yz;
            q3 += K[3][0]*yx + K[3][1]*yy + K[3][2]*yz;
            
            // Normalize updated quaternion
            norm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
            q0 *= norm;
            q1 *= norm;
            q2 *= norm;
            q3 *= norm;
            
            // Covariance Update P = (I - K * H) * P
            float KH[4][4] = {0};
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    for(int k=0; k<3; k++) {
                        KH[i][j] += K[i][k] * H[k][j];
                    }
                }
            }
            
            float P_new[4][4] = {0};
            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    float sum = 0.0f;
                    for(int k=0; k<4; k++) {
                        float I_KH = (i == k ? 1.0f : 0.0f) - KH[i][k];
                        sum += I_KH * P[k][j];
                    }
                    P_new[i][j] = sum;
                }
            }

            for(int i=0; i<4; i++) {
                for(int j=0; j<4; j++) {
                    P[i][j] = P_new[i][j];
                }
            }
        }
    }
    
    // Update State
    dc->estimated.q0 = q0;
    dc->estimated.q1 = q1;
    dc->estimated.q2 = q2;
    dc->estimated.q3 = q3;
}
