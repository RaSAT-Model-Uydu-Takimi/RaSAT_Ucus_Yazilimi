#include "M6_EKF_NavXY.h"
#include "Filter_Config.h"
#include <string.h>
#include <math.h>

void M6_NavXY_Init(M6_EKF_NavXY_t *ekf) {
    ekf->x[0] = 0.0f; // X Konum
    ekf->x[1] = 0.0f; // Y Konum
    ekf->x[2] = 0.0f; // X Hız
    ekf->x[3] = 0.0f; // Y Hız

    memset(ekf->P, 0, sizeof(ekf->P));
    ekf->P[0] = 10.0f;  // P(X, X)
    ekf->P[5] = 10.0f;  // P(Y, Y)
    ekf->P[10] = 1.0f;  // P(Vx, Vx)
    ekf->P[15] = 1.0f;  // P(Vy, Vy)

    ekf->ref_lat = 0.0f;
    ekf->ref_lon = 0.0f;
    ekf->ref_set = 0;
}

void M6_NavXY_Update(M6_EKF_NavXY_t *ekf, DataCenter *dataC, float dt_seconds) {
    if (dataC == NULL || dt_seconds <= 0.0f) return;

    // --- 1. GERÇEK YATAY İVMELERİ BULMA (Dünya Eksenine Rotasyon) ---
    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;

    float q0 = dataC->estimated.q0.value;
    float q1 = dataC->estimated.q1.value;
    float q2 = dataC->estimated.q2.value;
    float q3 = dataC->estimated.q3.value;

    // Kuaterniyon ile vektörü döndürme
    float earth_ax = (q0*q0 + q1*q1 - q2*q2 - q3*q3)*ax + 2.0f*(q1*q2 - q0*q3)*ay + 2.0f*(q0*q2 + q1*q3)*az;
    float earth_ay = 2.0f*(q1*q2 + q0*q3)*ax + (q0*q0 - q1*q1 + q2*q2 - q3*q3)*ay + 2.0f*(q2*q3 - q0*q1)*az;

    // --- 2. TAHMİN ADIMI (PREDICT) ---
    // x = F * x + B * u
    // X = X + Vx*dt + 0.5*ax*dt^2
    ekf->x[0] += ekf->x[2] * dt_seconds + 0.5f * earth_ax * dt_seconds * dt_seconds;
    // Y = Y + Vy*dt + 0.5*ay*dt^2
    ekf->x[1] += ekf->x[3] * dt_seconds + 0.5f * earth_ay * dt_seconds * dt_seconds;
    
    // Vx = Vx + ax*dt
    ekf->x[2] += earth_ax * dt_seconds;
    // Vy = Vy + ay*dt
    ekf->x[3] += earth_ay * dt_seconds;

    // F Matrisi (4x4)
    // [1, 0, dt, 0 ]
    // [0, 1, 0,  dt]
    // [0, 0, 1,  0 ]
    // [0, 0, 0,  1 ]
    float F[16] = {
        1.0f, 0.0f, dt_seconds, 0.0f,
        0.0f, 1.0f, 0.0f,       dt_seconds,
        0.0f, 0.0f, 1.0f,       0.0f,
        0.0f, 0.0f, 0.0f,       1.0f
    };

    // Q Matrisi (Süreç Gürültüsü)
    float Q[16] = {
        EKF_Q_XY_POS, 0, 0, 0,
        0, EKF_Q_XY_POS, 0, 0,
        0, 0, EKF_Q_XY_VEL, 0,
        0, 0, 0, EKF_Q_XY_VEL
    };

    // P = F * P * F^T + Q
    float F_P[16];
    mat_mult(4, 4, 4, F, ekf->P, F_P);
    float F_T[16];
    mat_trans(4, 4, F, F_T);
    float FPFT[16];
    mat_mult(4, 4, 4, F_P, F_T, FPFT);
    mat_add(4, 4, FPFT, Q, ekf->P);

    // --- 3. GÜNCELLEME ADIMI (UPDATE - GPS) ---
    // GPS geçerliyse ve yatayda veri akışı varsa güncelle
    if (dataC->gps.x.confidence > CONFIDENCE_MIN_VALID) {
        
        float gps_x = dataC->gps.x.calibratedValue;
        float gps_y = dataC->gps.y.calibratedValue;

        if (ekf->ref_set == 0) {
            ekf->ref_lat = gps_x;
            ekf->ref_lon = gps_y;
            ekf->ref_set = 1;
        }

        // Derece to Metre dönüşümü
        float gps_x_m = (gps_x - ekf->ref_lat) * 111320.0f;
        float gps_y_m = (gps_y - ekf->ref_lon) * 111320.0f * cosf(ekf->ref_lat * M_PI / 180.0f);
        
        // Dinamik Ölçüm Gürültüsü (R) - Sadece Konum
        float R_pos = 1.0f / (WEIGHT_XY_GPS * dataC->gps.x.confidence);
        
        // Z (Ölçüm Vektörü 2x1) - Sadece X ve Y
        float z_meas[2] = {gps_x_m, gps_y_m};
        
        // y = Z - H*x (H = [1 0 0 0; 0 1 0 0])
        float y[2];
        y[0] = z_meas[0] - ekf->x[0];
        y[1] = z_meas[1] - ekf->x[1];

        // S = H * P * H^T + R (2x2 Matris)
        float S[4] = {
            ekf->P[0] + R_pos, ekf->P[1],
            ekf->P[4],         ekf->P[5] + R_pos
        };

        // S_inv = S^-1 (2x2 Ters Alma)
        float S_inv[4];
        float det = S[0]*S[3] - S[1]*S[2];
        if (fabs(det) > 1e-6f) {
            S_inv[0] = S[3] / det;
            S_inv[1] = -S[1] / det;
            S_inv[2] = -S[2] / det;
            S_inv[3] = S[0] / det;

            // K = P * H^T * S_inv (4x2 Matris)
            float K[8];
            for (int i = 0; i < 4; i++) {
                K[i*2 + 0] = ekf->P[i*4 + 0] * S_inv[0] + ekf->P[i*4 + 1] * S_inv[2];
                K[i*2 + 1] = ekf->P[i*4 + 0] * S_inv[1] + ekf->P[i*4 + 1] * S_inv[3];
            }

            // x = x + K*y
            for (int i = 0; i < 4; i++) {
                ekf->x[i] += K[i*2 + 0] * y[0] + K[i*2 + 1] * y[1];
            }

            // P = (I - K*H) * P
            float I_KH[16];
            memset(I_KH, 0, sizeof(I_KH));
            for (int i = 0; i < 4; i++) {
                I_KH[i*4 + i] = 1.0f;           // Identity matris köşegeni
                I_KH[i*4 + 0] -= K[i*2 + 0];    // K*H'nin 0. sütunu
                I_KH[i*4 + 1] -= K[i*2 + 1];    // K*H'nin 1. sütunu
            }
            
            float P_new[16];
            mat_mult(4, 4, 4, I_KH, ekf->P, P_new);
            memcpy(ekf->P, P_new, sizeof(P_new));
        }
    }
}
