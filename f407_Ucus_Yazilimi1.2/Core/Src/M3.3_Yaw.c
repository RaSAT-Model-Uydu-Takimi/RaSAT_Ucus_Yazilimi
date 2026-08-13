/*
 * M3.3_Yaw.c
 *
 * Sapma (Yaw / Pusula) Hesaplama Modülü
 * Manyetometre ve Jiroskop verilerini kullanarak 1-Boyutlu Kalman Filtresi (1D KF) üretir.
 */

#include "M3.3_Yaw.h"
#include "M0.1_FilterConfig.h"
#include <math.h>

// 1D Kalman Durum Kovaryansı (P)
static float P_yaw = 1.0f;

void M3_3_Yaw_Init(DataCenter *dc) {
    dc->estimated.yaw = 0.0f;
    P_yaw = 1.0f; // Filtreyi sıfırla
}

void M3_3_Yaw_Update(DataCenter *dc, float dt) {
    
    // --- 1. TAHMİN (PREDICT) ADIMI ---
    // Jiroskopun Z ekseni verisini kalibrasyon profilindeki kayma (bias) ile düzeltiyoruz.
    float gz = dc->gyro.calibrated_z - dc->calibProfile.gyro_bias_z; 
    
    // Tahmini Yaw = Önceki Yaw + (Jiroskop Hızı * Zaman)
    float yaw_pred = dc->estimated.yaw + (gz * dt);
    
    // Tahmini Kovaryans = Önceki Kovaryans + Süreç Gürültüsü (Q)
    // Q matrisi (skaler) olarak Jiroskop Z gürültüsünü kullanıyoruz.
    float Q_yaw = (dc->calibProfile.gyro_noise_z > 0) ? dc->calibProfile.gyro_noise_z : 0.0001f;
    P_yaw = P_yaw + Q_yaw;


    // --- 2. DÜZELTME (UPDATE) ADIMI ---

    // Manyetometre Kalibrasyonu (Sadece Yer İstasyonundan Bias/Scale geldiyse uygulanır)
    // Sabit dururken bias ölçmediğimiz için, yer istasyonu girmediyse bias_x/y/z sıfırdır.
    float mag_x = (dc->mag.raw_x - dc->calibProfile.mag_bias_x) * dc->calibProfile.mag_scale_x;
    float mag_y = (dc->mag.raw_y - dc->calibProfile.mag_bias_y) * dc->calibProfile.mag_scale_y;
    float mag_z = (dc->mag.raw_z - dc->calibProfile.mag_bias_z) * dc->calibProfile.mag_scale_z;

    // Eğer manyetometre verisi sıfırsa (bağlantı kopukluğu vb.), düzeltme yapma, tahmini kullan.
    if(mag_x != 0.0f || mag_y != 0.0f || mag_z != 0.0f) {

        // Tilt Compensation (Eğim Düzeltmesi)
        float roll_rad  = dc->estimated.roll * DEG_TO_RAD;
        float pitch_rad = dc->estimated.pitch * DEG_TO_RAD;

        float cos_roll  = cosf(roll_rad);
        float sin_roll  = sinf(roll_rad);
        float cos_pitch = cosf(pitch_rad);
        float sin_pitch = sinf(pitch_rad);

        // Yatay düzleme (Earth Frame) yansıtılmış manyetik vektörler
        float Xh = mag_x * cos_pitch + mag_z * sin_pitch;
        float Yh = mag_x * sin_roll * sin_pitch + mag_y * cos_roll - mag_z * sin_roll * cos_pitch;

        // Pusula Yönü (Ölçüm - Z)
        float yaw_mag = atan2f(-Yh, Xh) * RAD_TO_DEG;

        // Ölçüm Gürültüsü (R) - Kalibrasyonda ölçülen manyetometre varyanslarının ortalamasını (veya max'ını) pusula gürültüsü olarak kabul ediyoruz.
        float mag_noise_avg = (dc->calibProfile.mag_noise_x + dc->calibProfile.mag_noise_y) * 0.5f;
        float R_yaw = (mag_noise_avg > 0) ? mag_noise_avg : 0.05f;

        // İnovasyon (Fark) = Ölçüm - Tahmin
        float Y_diff = yaw_mag - yaw_pred;

        // 360 Derece Sarılma (Wrap-around) Koruması
        // Örn: Pusula 359 derece (veya -1), Tahmin 1 derece ise aradaki fark 358 değil, -2 olmalıdır.
        if(Y_diff > 180.0f) Y_diff -= 360.0f;
        if(Y_diff < -180.0f) Y_diff += 360.0f;

        // Kalman Kazancı (K) = P / (P + R)
        float K = P_yaw / (P_yaw + R_yaw);

        // Durum (State) Güncellemesi
        yaw_pred = yaw_pred + (K * Y_diff);

        // Kovaryans (P) Güncellemesi
        P_yaw = (1.0f - K) * P_yaw;
    }


    // Sonucu -180 ile +180 arasında sınırla
    if(yaw_pred > 180.0f) yaw_pred -= 360.0f;
    if(yaw_pred < -180.0f) yaw_pred += 360.0f;
    
    // Yeni durumu kaydet
    dc->estimated.yaw = yaw_pred;
}
