#include "M4_EKF_Yaw.h"
#include "Filter_Config.h"
#include <math.h>

// Açıları -180 ile 180 derece arasına sığdırır (Wrap to 180)
static float wrap_180(float angle) {
    while (angle > 180.0f)  angle -= 360.0f;
    while (angle <= -180.0f) angle += 360.0f;
    return angle;
}

void M4_Yaw_Init(M4_EKF_Yaw_t *ekf) {
    ekf->state_yaw = 0.0f;
    ekf->P = 1.0f; // Başlangıç belirsizliği
}

void M4_Yaw_Update(M4_EKF_Yaw_t *ekf, DataCenter *dataC, float dt_seconds) {
    if (dataC == NULL || dt_seconds <= 0.0f) return;

    // --- 1. TAHMİN ADIMI (PREDICT) ---
    // Jiroskop Z değerini al (rad/s -> deg/s çevrimi varsayımı yapılıyor)
    // Eğer Jiroskop veriniz derece/saniye ise çevrime gerek yoktur. Burada derece/saniye varsayılmıştır.
    float gyro_z_degps = dataC->gyro.z.calibratedValue * (180.0f / (float)M_PI); 
    
    // State tahmini (Kinematik model)
    ekf->state_yaw += gyro_z_degps * dt_seconds;
    ekf->state_yaw = wrap_180(ekf->state_yaw);

    // Kovaryans (Belirsizlik) tahmini: P = P + Q
    ekf->P = ekf->P + EKF_Q_YAW_ANGLE;

    // --- 2. ÖLÇÜM (GÜNCELLEME) ADIMI (UPDATE) ---
    // GPS Rotası kullanılabilir mi? Hız > 2.0 m/s ve Güven yüksek ise
    if (dataC->gps.speed.calibratedValue > 2.0f && dataC->gps.course.confidence > CONFIDENCE_MIN_VALID) {
        
        float gps_course = dataC->gps.course.calibratedValue;
        
        // Dinamik Gürültü (R) Hesabı: Nominal gürültü, güven düştükçe büyür
        float dynamic_R = (1.0f / (WEIGHT_YAW_GPS * dataC->gps.course.confidence));

        // Inovasyon (Ölçüm Hatası)
        float y = wrap_180(gps_course - ekf->state_yaw);

        // İnovasyon Kovaryansı (S) = H*P*H^T + R  (Burada H = 1)
        float S = ekf->P + dynamic_R;

        // Kalman Kazancı (K) = P * H^T * S^-1
        float K = ekf->P / S;

        // Durumu Güncelle
        ekf->state_yaw = ekf->state_yaw + (K * y);
        ekf->state_yaw = wrap_180(ekf->state_yaw);

        // Hata Kovaryansını Güncelle: P = (1 - K*H) * P
        ekf->P = (1.0f - K) * ekf->P;
    }

    // Nihai sonucu DataCenter'a aktar
    dataC->estimated.yaw.value = ekf->state_yaw;
}
