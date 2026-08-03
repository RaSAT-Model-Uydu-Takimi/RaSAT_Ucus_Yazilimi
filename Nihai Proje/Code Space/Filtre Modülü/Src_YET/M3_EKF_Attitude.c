#include "M3_EKF_Attitude.h"
#include "Filter_Config.h"
#include "M0_Matrix_Operations.h"
#include <math.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/*  Fast Inverse Square Root (gömülü sistemler için optimize)                 */
/* -------------------------------------------------------------------------- */
static float invSqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    int32_t i;
    memcpy(&i, &y, sizeof(i));
    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof(y));
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/* ========================================================================== */
/*  M3_Attitude_Init                                                          */
/* ========================================================================== */
void M3_Attitude_Init(M3_EKF_Attitude_t *ekf) {
    if (ekf == NULL) return;

    memset(ekf->x, 0, sizeof(ekf->x));
    ekf->x[0] = 1.0f; // q0
    // q1=0, q2=0, q3=0
    // bias_x=0, bias_y=0, bias_z=0

    memset(ekf->P, 0, sizeof(ekf->P));
    // Başlangıç belirsizlikleri
    for (int i = 0; i < 4; i++) ekf->P[i * 7 + i] = 1.0f;     /* Kuaterniyonlar */
    for (int i = 4; i < 7; i++) ekf->P[i * 7 + i] = 0.01f;    /* Biaslar */
}

/* ========================================================================== */
/*  M3_Attitude_Update                                                        */
/* ========================================================================== */
void M3_Attitude_Update(M3_EKF_Attitude_t *ekf, DataCenter *dataC, float dt_seconds) {
    if (ekf == NULL || dataC == NULL || dt_seconds <= 0.0f) return;

    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;

    float gx = dataC->gyro.x.calibratedValue;
    float gy = dataC->gyro.y.calibratedValue;
    float gz = dataC->gyro.z.calibratedValue;

    float q0 = ekf->x[0];
    float q1 = ekf->x[1];
    float q2 = ekf->x[2];
    float q3 = ekf->x[3];
    float bx = ekf->x[4];
    float by = ekf->x[5];
    float bz = ekf->x[6];

    /* ================================================================== */
    /*  1. TAHMİN (PREDICT) ADIMI                                         */
    /* ================================================================== */
    // Bias'ı çıkarılmış net açısal hız
    float wx = gx - bx;
    float wy = gy - by;
    float wz = gz - bz;

    // Kuaterniyon türevleri
    float q0_dot = 0.5f * (-q1 * wx - q2 * wy - q3 * wz);
    float q1_dot = 0.5f * ( q0 * wx - q3 * wy + q2 * wz);
    float q2_dot = 0.5f * ( q3 * wx + q0 * wy - q1 * wz);
    float q3_dot = 0.5f * (-q2 * wx + q1 * wy + q0 * wz);

    // Durum güncelleme (Integral)
    ekf->x[0] += q0_dot * dt_seconds;
    ekf->x[1] += q1_dot * dt_seconds;
    ekf->x[2] += q2_dot * dt_seconds;
    ekf->x[3] += q3_dot * dt_seconds;
    // Biaslar sabit kalır (ekf->x[4..6] aynı)

    // Normalizasyon
    float norm = invSqrt(ekf->x[0]*ekf->x[0] + ekf->x[1]*ekf->x[1] + ekf->x[2]*ekf->x[2] + ekf->x[3]*ekf->x[3]);
    ekf->x[0] *= norm; ekf->x[1] *= norm; ekf->x[2] *= norm; ekf->x[3] *= norm;
    q0 = ekf->x[0]; q1 = ekf->x[1]; q2 = ekf->x[2]; q3 = ekf->x[3];

    // F Matrisi (Jacobian) 7x7
    float F[49];
    memset(F, 0, sizeof(F));
    // I_4x4
    for(int i=0; i<7; i++) F[i*7 + i] = 1.0f;
    
    // Omega terms (4x4) * dt/2
    float dt2 = dt_seconds * 0.5f;
    F[0*7 + 0] = 1.0f;      F[0*7 + 1] = -wx * dt2; F[0*7 + 2] = -wy * dt2; F[0*7 + 3] = -wz * dt2;
    F[1*7 + 0] =  wx * dt2; F[1*7 + 1] = 1.0f;      F[1*7 + 2] =  wz * dt2; F[1*7 + 3] = -wy * dt2;
    F[2*7 + 0] =  wy * dt2; F[2*7 + 1] = -wz * dt2; F[2*7 + 2] = 1.0f;      F[2*7 + 3] =  wx * dt2;
    F[3*7 + 0] =  wz * dt2; F[3*7 + 1] =  wy * dt2; F[3*7 + 2] = -wx * dt2; F[3*7 + 3] = 1.0f;

    // Xi terms (4x3) * -dt/2 for biases
    F[0*7 + 4] =  q1 * dt2; F[0*7 + 5] =  q2 * dt2; F[0*7 + 6] =  q3 * dt2;
    F[1*7 + 4] = -q0 * dt2; F[1*7 + 5] =  q3 * dt2; F[1*7 + 6] = -q2 * dt2;
    F[2*7 + 4] = -q3 * dt2; F[2*7 + 5] = -q0 * dt2; F[2*7 + 6] =  q1 * dt2;
    F[3*7 + 4] =  q2 * dt2; F[3*7 + 5] = -q1 * dt2; F[3*7 + 6] = -q0 * dt2;

    // Q Matrisi 7x7
    float Q[49];
    memset(Q, 0, sizeof(Q));
    for (int i = 0; i < 4; i++) Q[i * 7 + i] = EKF_Q_ATT_Q * dt_seconds;
    for (int i = 4; i < 7; i++) Q[i * 7 + i] = EKF_Q_ATT_BIAS * dt_seconds;

    // P = F * P * F^T + Q
    float FP[49], FT[49], FPFT[49];
    mat_mult(7, 7, 7, F, ekf->P, FP);
    mat_trans(7, 7, F, FT);
    mat_mult(7, 7, 7, FP, FT, FPFT);
    mat_add(7, 7, FPFT, Q, ekf->P);

    /* ================================================================== */
    /*  2. DÜZELTME (CORRECT) ADIMI - İvmeölçer ile                       */
    /* ================================================================== */
    float acc_magnitude = sqrtf(ax*ax + ay*ay + az*az);
    float min_acc_conf = dataC->acc.x.confidence;
    if (dataC->acc.y.confidence < min_acc_conf) min_acc_conf = dataC->acc.y.confidence;
    if (dataC->acc.z.confidence < min_acc_conf) min_acc_conf = dataC->acc.z.confidence;

    // Sadece motor yanmıyorken (veya çok şiddetli sallantı yokken) düzeltme yap
    if (acc_magnitude > 1.0f && acc_magnitude < EKF_G_COMP_THRESHOLD_MPS2 && min_acc_conf > CONFIDENCE_MIN_VALID) {
        
        // İvmeölçeri birim vektöre çevir (Ölçüm z)
        float acc_norm = invSqrt(ax*ax + ay*ay + az*az);
        float z_meas[3] = {ax * acc_norm, ay * acc_norm, az * acc_norm};

        // Tahmin edilen yerçekimi vektörü h(x)
        float hx[3] = {
            2.0f * (q1 * q3 - q0 * q2),
            2.0f * (q0 * q1 + q2 * q3),
            q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3
        };

        // İnovasyon y = z - h(x)
        float y[3] = {
            z_meas[0] - hx[0],
            z_meas[1] - hx[1],
            z_meas[2] - hx[2]
        };

        // H Matrisi (Jacobian) 3x7
        float H[21];
        memset(H, 0, sizeof(H));
        H[0*7+0] = -2.0f*q2; H[0*7+1] =  2.0f*q3; H[0*7+2] = -2.0f*q0; H[0*7+3] =  2.0f*q1;
        H[1*7+0] =  2.0f*q1; H[1*7+1] =  2.0f*q0; H[1*7+2] =  2.0f*q3; H[1*7+3] =  2.0f*q2;
        H[2*7+0] =  2.0f*q0; H[2*7+1] = -2.0f*q1; H[2*7+2] = -2.0f*q2; H[2*7+3] =  2.0f*q3;
        // H[...][4..6] zaten 0

        // Dinamik R Matrisi 3x3
        float r_val = 1.0f / (WEIGHT_PR_ACC * min_acc_conf + 1e-6f);
        float R[9] = {
            r_val, 0.0f, 0.0f,
            0.0f, r_val, 0.0f,
            0.0f, 0.0f, r_val
        };

        // S = H * P * H^T + R (3x3)
        float HP[21], HT[21], HPHT[9], S[9];
        mat_mult(3, 7, 7, H, ekf->P, HP);
        mat_trans(3, 7, H, HT);
        mat_mult(3, 7, 3, HP, HT, HPHT);
        mat_add(3, 3, HPHT, R, S);

        // S matrisinin tersi
        float S_inv[9];
        if (mat_inv(3, S, S_inv)) {
            
            // -------------------------------------------------------------
            // FDI: Mahalanobis Uzaklığı Testi (İnovasyon Gate)
            // D^2 = y^T * S^-1 * y
            // -------------------------------------------------------------
            float y_Sinv[3];
            mat_mult(1, 3, 3, y, S_inv, y_Sinv);
            float mahalanobis_sq = y_Sinv[0]*y[0] + y_Sinv[1]*y[1] + y_Sinv[2]*y[2];

            if (mahalanobis_sq > EKF_INNOVATION_GATE_3SIGMA) {
                // SENSÖR YALAN SÖYLÜYOR! Güveni sıfırla, düzeltmeyi İPTAL ET.
                dataC->acc.x.confidence = 0.0f;
                dataC->acc.y.confidence = 0.0f;
                dataC->acc.z.confidence = 0.0f;
            } else {
                // Güvenli: Kalman Kazancı K = P * H^T * S^-1 (7x3)
                float PHT[21], K[21];
                mat_mult(7, 7, 3, ekf->P, HT, PHT);
                mat_mult(7, 3, 3, PHT, S_inv, K);

                // x = x + K * y
                float Ky[7];
                mat_mult(7, 3, 1, K, y, Ky);
                for (int i=0; i<7; i++) ekf->x[i] += Ky[i];

                // P = (I - K*H) * P
                float KH[49];
                mat_mult(7, 3, 7, K, H, KH);
                float I_KH[49];
                memset(I_KH, 0, sizeof(I_KH));
                for(int i=0; i<7; i++) I_KH[i*7+i] = 1.0f;
                mat_sub(7, 7, I_KH, KH, I_KH);
                float P_new[49];
                mat_mult(7, 7, 7, I_KH, ekf->P, P_new);
                memcpy(ekf->P, P_new, sizeof(P_new));

                // Güncelleme sonrası kuaterniyon normalizasyonu
                norm = invSqrt(ekf->x[0]*ekf->x[0] + ekf->x[1]*ekf->x[1] + ekf->x[2]*ekf->x[2] + ekf->x[3]*ekf->x[3]);
                ekf->x[0] *= norm; ekf->x[1] *= norm; ekf->x[2] *= norm; ekf->x[3] *= norm;
            }
        }
    }

    // --- COVARIANCE ANTI-WINDUP (Kovaryans Sınırlandırma) ---
    // YAW ve Kuaterniyon uzunluğu ivmeölçer ile gözlemlenemez (unobservable).
    // Bu yüzden bu eksenlerdeki varyans sonsuza doğru büyür ve P matrisini patlatır.
    // Bunu engellemek için P matrisinin köşegenlerini sınırlandırıyoruz (Clamp).
    for (int i = 0; i < 7; i++) {
        if (ekf->P[i*7 + i] > 0.1f) {
            float s = sqrtf(0.1f / ekf->P[i*7 + i]);
            for (int j = 0; j < 7; j++) {
                ekf->P[i*7 + j] *= s;
                if (i != j) ekf->P[j*7 + i] *= s;
            }
        }
    }

    /* ================================================================== */
    /*  3. ÇIKTILAR (DataCenter'a Kayıt)                                  */
    /* ================================================================== */
    q0 = ekf->x[0]; q1 = ekf->x[1]; q2 = ekf->x[2]; q3 = ekf->x[3];

    // Euler Açıları (Yaw dahil - fakat M4 bunu ezecek/düzeltecektir)
    dataC->estimated.roll.value  = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * RAD2DEG;
    float sinP = 2.0f * (q0 * q2 - q3 * q1);
    if (sinP >  1.0f) sinP =  1.0f;
    if (sinP < -1.0f) sinP = -1.0f;
    dataC->estimated.pitch.value = asinf(sinP) * RAD2DEG;
    dataC->estimated.yaw.value = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3)) * RAD2DEG;

    dataC->estimated.q0.value = q0;
    dataC->estimated.q1.value = q1;
    dataC->estimated.q2.value = q2;
    dataC->estimated.q3.value = q3;

    // Jiroskop Bias Kayıtları
    dataC->estimated.gyro_bias_x.value = ekf->x[4];
    dataC->estimated.gyro_bias_y.value = ekf->x[5];
    dataC->estimated.gyro_bias_z.value = ekf->x[6];

    // Tahmin güveni (Kovaryansın köşegenine ters orantılı)
    float conf = 1.0f / (1.0f + ekf->P[0]); // q0 belirsizliği
    dataC->estimated.pitch.confidence = conf;
    dataC->estimated.roll.confidence  = conf;
    dataC->estimated.q0.confidence    = conf;
    dataC->estimated.q1.confidence    = conf;
    dataC->estimated.q2.confidence    = conf;
    dataC->estimated.q3.confidence    = conf;
    
    dataC->estimated.gyro_bias_x.confidence = 1.0f / (1.0f + ekf->P[4*7+4]);
    dataC->estimated.gyro_bias_y.confidence = 1.0f / (1.0f + ekf->P[5*7+5]);
    dataC->estimated.gyro_bias_z.confidence = 1.0f / (1.0f + ekf->P[6*7+6]);
}
