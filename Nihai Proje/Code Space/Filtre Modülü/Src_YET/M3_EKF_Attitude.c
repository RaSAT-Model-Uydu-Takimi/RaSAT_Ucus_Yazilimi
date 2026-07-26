#include "M3_EKF_Attitude.h"
#include "Filter_Config.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

// Fast Inverse Square Root
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

void M3_Attitude_Init(DataCenter *dataC) {
    if (dataC == NULL) return;
    
    dataC->estimated.q0.value = 1.0f;
    dataC->estimated.q1.value = 0.0f;
    dataC->estimated.q2.value = 0.0f;
    dataC->estimated.q3.value = 0.0f;

    dataC->estimated.pitch.value = 0.0f;
    dataC->estimated.roll.value  = 0.0f;
    dataC->estimated.yaw.value   = 0.0f; // Yaw burda sıfırlanır ama M4'te güncellenir.
}

void M3_Attitude_Update(DataCenter *dataC, float dt_seconds) {
    if (dataC == NULL || dt_seconds <= 0.0f) return;

    float ax = dataC->acc.x.calibratedValue;
    float ay = dataC->acc.y.calibratedValue;
    float az = dataC->acc.z.calibratedValue;
    
    float gx = dataC->gyro.x.calibratedValue;
    float gy = dataC->gyro.y.calibratedValue;
    float gz = dataC->gyro.z.calibratedValue;

    float q0 = dataC->estimated.q0.value;
    float q1 = dataC->estimated.q1.value;
    float q2 = dataC->estimated.q2.value;
    float q3 = dataC->estimated.q3.value;

    // G-Kompansasyonu: Toplam ivme sınırın üzerindeyse ivmeölçeri reddet (Ağırlık = 0)
    float acc_magnitude = sqrtf(ax * ax + ay * ay + az * az);
    float dynamic_acc_weight = 0.0f;

    if (acc_magnitude > 0.1f && acc_magnitude < EKF_G_COMP_THRESHOLD_MPS2) {
        // Motor yanmıyor (Aşırı G yok), İvmeölçere M2 Güvenine göre ağırlık ver
        // 3 Eksenin de minimum güvenini alarak en güvenli (kötü) senaryoyu koru
        float min_acc_conf = dataC->acc.x.confidence;
        if (dataC->acc.y.confidence < min_acc_conf) min_acc_conf = dataC->acc.y.confidence;
        if (dataC->acc.z.confidence < min_acc_conf) min_acc_conf = dataC->acc.z.confidence;

        dynamic_acc_weight = WEIGHT_PR_ACC * min_acc_conf;
    }

    // İvmeölçer kullanılıyorsa Düzeltme Adımı (Update Step)
    if (dynamic_acc_weight > 0.0f) {
        float norm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= norm;
        ay *= norm;
        az *= norm;

        // Tahmin edilen Yerçekimi Vektörü
        float vx = 2.0f * (q1 * q3 - q0 * q2);
        float vy = 2.0f * (q0 * q1 + q2 * q3);
        float vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

        // Hata Vektörü (Cross Product)
        float ex = (ay * vz - az * vy);
        float ey = (az * vx - ax * vz);
        float ez = (ax * vy - ay * vx);

        // Jiroskop değerine hatayı dinamik ağırlıkla ekle
        gx += dynamic_acc_weight * ex;
        gy += dynamic_acc_weight * ey;
        gz += dynamic_acc_weight * ez;
    }

    // Tahmin Adımı (Predict Step): Jiroskop verisiyle kuaterniyon entegrasyonu
    float q0_dot = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    float q1_dot = 0.5f * ( q0 * gx + q2 * gz - q3 * gy);
    float q2_dot = 0.5f * ( q0 * gy - q1 * gz + q3 * gx);
    float q3_dot = 0.5f * ( q0 * gz + q1 * gy - q2 * gx);

    q0 += q0_dot * dt_seconds;
    q1 += q1_dot * dt_seconds;
    q2 += q2_dot * dt_seconds;
    q3 += q3_dot * dt_seconds;

    // Normalizasyon
    float norm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= norm;
    q1 *= norm;
    q2 *= norm;
    q3 *= norm;

    // Euler'e Çevir (Pitch ve Roll)
    dataC->estimated.roll.value  = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2)) * (180.0f / (float)M_PI);
    dataC->estimated.pitch.value = asinf(2.0f * (q0 * q2 - q3 * q1)) * (180.0f / (float)M_PI);
    
    // Değerleri kaydet
    dataC->estimated.q0.value = q0;
    dataC->estimated.q1.value = q1;
    dataC->estimated.q2.value = q2;
    dataC->estimated.q3.value = q3;
}
