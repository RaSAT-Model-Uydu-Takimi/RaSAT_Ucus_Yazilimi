#include "Filter_Core.h"
#include <string.h>

void Filter_Init(Filter_System_t *sys, float initial_altitude) {
    if (sys == NULL) return;

    // 1. DataCenter Başlatma (Şimdilik her şeyi 0.0f yapar)
    memset(&sys->dataC, 0, sizeof(DataCenter));
    
    // 2. M1 Sensör Kalibratörü
    SensorCalib_Init(&sys->calib); // M1'in kendi iç statik değişkenleri varsa

    // 3. M2 Güven Doğrulayıcı
    CE_Init(&sys->conf_eval);

    // 4. M3 Yönelim Kestirici (Pitch/Roll)
    M3_Attitude_Init(&sys->dataC);

    // 5. M4 Sapma (Yaw) EKF
    M4_Yaw_Init(&sys->ekf_yaw);

    // 6. M5 İrtifa EKF
    M5_Altitude_Init(&sys->ekf_altitude, initial_altitude);

    // 7. M6 Navigasyon (X/Y) EKF
    M6_NavXY_Init(&sys->ekf_nav_xy);

    sys->last_update_time_us = 0;
}

void Filter_Update(Filter_System_t *sys, Sensor_Verileri_t *raw_data, uint32_t curr_time_us) {
    if (sys == NULL || raw_data == NULL) return;

    // Zaman deltası (Saniye cinsinden) - İlk girişte dt_seconds = 0
    float dt_seconds = 0.0f;
    if (sys->last_update_time_us != 0) {
        dt_seconds = (float)(curr_time_us - sys->last_update_time_us) / 1000000.0f;
    }
    sys->last_update_time_us = curr_time_us;

    if (dt_seconds <= 0.0f) return; // Zaman atlaması yoksa işlemi yapma

    // --- PIPELINE (BORU HATTI) İŞLETİMİ ---

    // ADIM 1: Ham Veriyi DataCenter'a Aktar
    LoadRawFromSensor_Verileri_t(raw_data, &sys->dataC);

    // ADIM 2: (M1) Ham Verileri Kalibre Et
    SensorCalibApply(&sys->calib, &sys->dataC);

    // ADIM 3: (M2) Kalibreli Verilerin Güvenini Denetle
    CE_Update(&sys->conf_eval, &sys->dataC, curr_time_us);

    // ADIM 4: (M3) Yönelim (Attitude - Pitch/Roll) Hesapla
    M3_Attitude_Update(&sys->dataC, dt_seconds);

    // ADIM 5: (M4) Sapma (Yaw) Hesapla
    M4_Yaw_Update(&sys->ekf_yaw, &sys->dataC, dt_seconds);

    // ADIM 6: (M5) İrtifa ve Dikey Hız (Z) Hesapla
    M5_Altitude_Update(&sys->ekf_altitude, &sys->dataC, &sys->calib, dt_seconds);

    // ADIM 7: (M6) Yatay Konum (X/Y) Navigasyonu Hesapla
    M6_NavXY_Update(&sys->ekf_nav_xy, &sys->dataC, dt_seconds);
}
