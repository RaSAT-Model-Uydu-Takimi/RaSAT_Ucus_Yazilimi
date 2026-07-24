#include "M5_EKF_Altitude.h"
#include "Filter_Config.h"
#include <string.h>
#include <math.h>

#define GRAVITY 9.80665f

void M5_Altitude_Init(M5_EKF_Altitude_t *ekf, float initial_altitude) {
    // Başlangıç Durumları: Z = İlk İrtifa, Hız = 0, Bias = 0
    ekf->x[0] = initial_altitude;
    ekf->x[1] = 0.0f;
    ekf->x[2] = 0.0f;

    // Başlangıç Kovaryansı (P): Konumda biraz, hız ve bias'ta düşük belirsizlik
    memset(ekf->P, 0, sizeof(ekf->P));
    ekf->P[0] = 10.0f; // P(0,0)
    ekf->P[4] = 1.0f;  // P(1,1)
    ekf->P[8] = 0.1f;  // P(2,2)
}

void M5_Altitude_Update(M5_EKF_Altitude_t *ekf, DataCenter *dataC, const SensorCalib_t *calib, float dt_seconds) {
    if (dataC == NULL || calib == NULL || dt_seconds <= 0.0f) return;

    // --- 1. GERÇEK DİKEY İVMEYİ BULMA (Dünya Eksenine Rotasyon) ---
    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;

    float q0 = dataC->estimated.q0.value;
    float q1 = dataC->estimated.q1.value;
    float q2 = dataC->estimated.q2.value;
    float q3 = dataC->estimated.q3.value;

    // Sadece Z eksenindeki net ivmeyi bulmak için Kuaterniyon Döndürme (Quaternion Rotation)
    // Dünya ekseninde Z ivmesi:
    float earth_az = 2.0f*(q1*q3 - q0*q2)*ax + 2.0f*(q0*q1 + q2*q3)*ay + (q0*q0 - q1*q1 - q2*q2 + q3*q3)*az;
    
    // Yerçekimini çıkar (Sensör yukarı yönü pozitif ölçüyorsa)
    // Roket ivmelenirken earth_az > 9.81 olacaktır, bu yüzden net ivme:
    float net_accel_z = earth_az - GRAVITY; 

    // --- 2. TAHMİN ADIMI (PREDICT) ---
    // Bias'ı ivmeden çıkar
    float a_z = net_accel_z - ekf->x[2]; 

    // x = F * x + B * u (Kinematik)
    // Z = Z + V*dt + 0.5*a*dt^2
    ekf->x[0] += ekf->x[1] * dt_seconds + 0.5f * a_z * dt_seconds * dt_seconds;
    // V = V + a*dt
    ekf->x[1] += a_z * dt_seconds;
    // Bias sabittir, değişmez (x[2] aynı kalır)

    // F Matrisi (Durum Geçiş)
    // [1, dt, -0.5*dt^2]
    // [0,  1,        -dt]
    // [0,  0,         1]
    float F[9] = {
        1.0f, dt_seconds, -0.5f * dt_seconds * dt_seconds,
        0.0f, 1.0f,       -dt_seconds,
        0.0f, 0.0f,       1.0f
    };

    // Q Matrisi (Süreç Gürültüsü - Filter_Config.h'dan)
    float Q[9] = {
        EKF_Q_Z_POS, 0, 0,
        0, EKF_Q_Z_VEL, 0,
        0, 0, EKF_Q_Z_ACC_BIAS
    };

    // P = F * P * F^T + Q
    float F_P[9];
    mat_mult(3, 3, 3, F, ekf->P, F_P); // F_P = F * P
    float F_T[9];
    mat_trans(3, 3, F, F_T);           // F_T = F^T
    float FPFT[9];
    mat_mult(3, 3, 3, F_P, F_T, FPFT); // FPFT = F_P * F_T
    mat_add(3, 3, FPFT, Q, ekf->P);    // P = FPFT + Q

    // --- 3. GÜNCELLEME ADIMI (UPDATE - Barometre) ---
    if (dataC->baro.press.confidence > CONFIDENCE_MIN_VALID) {
        // İrtifayı basınçtan anlık olarak türet
        float z_meas = 44330.0f * (1.0f - powf(dataC->baro.press.calibratedValue / calib->sea_level_pa, 0.190295f));
        
        // Dinamik Ölçüm Gürültüsü (R)
        float R_baro = 1.0f / (WEIGHT_ALT_BARO * dataC->baro.press.confidence);

        // H = [1, 0, 0] (Sadece Z konumunu ölçüyoruz)
        float H[3] = {1.0f, 0.0f, 0.0f};
        float H_T[3] = {1.0f, 0.0f, 0.0f}; // Transpozu (3x1)

        // y = z_meas - H*x
        float y = z_meas - ekf->x[0];

        // S = H * P * H^T + R
        // H*P (1x3)
        float HP[3] = {
            H[0]*ekf->P[0] + H[1]*ekf->P[3] + H[2]*ekf->P[6],
            H[0]*ekf->P[1] + H[1]*ekf->P[4] + H[2]*ekf->P[7],
            H[0]*ekf->P[2] + H[1]*ekf->P[5] + H[2]*ekf->P[8]
        };
        // HP * H^T (1x1)
        float S = HP[0]*H_T[0] + HP[1]*H_T[1] + HP[2]*H_T[2] + R_baro;

        // K = P * H^T / S  (3x1)
        float K[3];
        K[0] = (ekf->P[0]*H_T[0] + ekf->P[1]*H_T[1] + ekf->P[2]*H_T[2]) / S;
        K[1] = (ekf->P[3]*H_T[0] + ekf->P[4]*H_T[1] + ekf->P[5]*H_T[2]) / S;
        K[2] = (ekf->P[6]*H_T[0] + ekf->P[7]*H_T[1] + ekf->P[8]*H_T[2]) / S;

        // x = x + K*y
        ekf->x[0] += K[0] * y;
        ekf->x[1] += K[1] * y;
        ekf->x[2] += K[2] * y;

        // P = (I - K*H) * P
        float I_KH[9] = {
            1.0f - K[0]*H[0], 0.0f - K[0]*H[1], 0.0f - K[0]*H[2],
            0.0f - K[1]*H[0], 1.0f - K[1]*H[1], 0.0f - K[1]*H[2],
            0.0f - K[2]*H[0], 0.0f - K[2]*H[1], 1.0f - K[2]*H[2]
        };
        float P_new[9];
        mat_mult(3, 3, 3, I_KH, ekf->P, P_new);
        memcpy(ekf->P, P_new, sizeof(P_new));
    }

    // Nihai verileri kaydet
    dataC->estimated.pos_z.value = ekf->x[0];
    dataC->estimated.vel_z.value = ekf->x[1];
}
