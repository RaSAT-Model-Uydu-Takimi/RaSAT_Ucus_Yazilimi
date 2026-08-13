/*
 * M2.2_FlyingMode.c
 *
 * Uçuş Modu implementasyonu.
 * Sensör verilerini düzeltir ve filtrelere/PID kontrolcülerine dağıtır.
 */

#include "M2.2_FlyingMode.h"
#include "M3.0_FilterSystem.h"
#include "M3.2_Attitude.h"
#include "M3.3_Yaw.h"
#include "M3.4_Altitude.h"
#include "main.h" // HAL_GetTick için

static uint32_t last_run_time = 0;

void FlyingMode_Init(DataCenter *data) {
    // Filtreleri (EKF) ilk değerleriyle başlat
    FilterSystem_Init(data);
    
    last_run_time = HAL_GetTick();
}

void FlyingMode_Run(DataCenter *data) {
    // 1. Delta Time (dt) Hesapla (Saniye cinsinden)
    uint32_t current_time = HAL_GetTick();
    float dt = (current_time - last_run_time) / 1000.0f;
    
    // Eğer süre çok kısaysa veya taşma olduysa koruma sağla
    if (dt <= 0.0f || dt > 1.0f) {
        dt = 0.005f; // Varsayılan bir değer (örneğin 200Hz için 0.005s)
    }
    last_run_time = current_time;

    // 2. Ham (Raw) Verilere Kalibrasyon Uygula (Calibrated hesapla)
    
    // İvmeölçer
    data->acc.calibrated_x = (data->acc.raw_x - data->calibProfile.acc_bias_x) * data->calibProfile.acc_scale_x;
    data->acc.calibrated_y = (data->acc.raw_y - data->calibProfile.acc_bias_y) * data->calibProfile.acc_scale_y;
    data->acc.calibrated_z = (data->acc.raw_z - data->calibProfile.acc_bias_z) * data->calibProfile.acc_scale_z;
    
    // Jiroskop
    data->gyro.calibrated_x = (data->gyro.raw_x - data->calibProfile.gyro_bias_x) * data->calibProfile.gyro_scale_x;
    data->gyro.calibrated_y = (data->gyro.raw_y - data->calibProfile.gyro_bias_y) * data->calibProfile.gyro_scale_y;
    data->gyro.calibrated_z = (data->gyro.raw_z - data->calibProfile.gyro_bias_z) * data->calibProfile.gyro_scale_z;
    
    // Manyetometre
    data->mag.calibrated_x = (data->mag.raw_x - data->calibProfile.mag_bias_x) * data->calibProfile.mag_scale_x;
    data->mag.calibrated_y = (data->mag.raw_y - data->calibProfile.mag_bias_y) * data->calibProfile.mag_scale_y;
    data->mag.calibrated_z = (data->mag.raw_z - data->calibProfile.mag_bias_z) * data->calibProfile.mag_scale_z;

    // Barometre
    data->baro.calibrated_press = (data->baro.raw_press - data->calibProfile.baro_press_bias) * data->calibProfile.baro_press_scale + data->calibProfile.baro_press_bias;
    data->baro.calibrated_temp  = (data->baro.raw_temp  - data->calibProfile.baro_temp_bias)  * data->calibProfile.baro_temp_scale  + data->calibProfile.baro_temp_bias;

    // 3. Yönelim (Attitude) Filtresini (EKF) Çalıştır
    // EKF, düzeltilmiş ivme ve jiroskop verilerini kullanarak sadece kuaterniyonları (q0-q3) hesaplar.
    FilterSystem_Update(data, dt);
    
    // 4. Kuaterniyonlardan Pitch ve Roll (Yunuslama/Yuvarlanma) açılarını bul
    M3_2_Attitude_Update(data);
    
    // 5. Yaw (Pusula) açısını bul (Pitch ve Roll verisine ihtiyaç duyar)
    M3_3_Yaw_Update(data, dt);
    
    // 6. Yükseklik Filtresi (2-Durumlu Kalman) ve Düşey Hız hesaplaması
    M3_4_Altitude_Update(data, dt);
    
    // 7. (Gelecekte) PID Kontrolcüsü burada çalışacak ve motor PWM'leri güncellenecek
}
