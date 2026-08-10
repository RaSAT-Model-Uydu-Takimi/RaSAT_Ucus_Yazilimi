/*
 * M3.3_Yaw.c
 *
 * Sapma (Yaw / Pusula) Hesaplama Modülü
 */

#include "M3.3_Yaw.h"
#include "M0.1_FilterConfig.h"
#include <math.h>

void M3_3_Yaw_Update(DataCenter *dc, float dt) {
    
#if YAW_USE_MAGNETOMETER
    // 1. Manyetometre Kalibrasyonu (Bias ve Scale uygulama)
    // Ham veri üzerinden bias çıkartılır ve scale ile çarpılır.
    float mag_x = (dc->mag.raw_x - dc->calibProfile.mag_bias_x) * dc->calibProfile.mag_scale_x;
    float mag_y = (dc->mag.raw_y - dc->calibProfile.mag_bias_y) * dc->calibProfile.mag_scale_y;
    float mag_z = (dc->mag.raw_z - dc->calibProfile.mag_bias_z) * dc->calibProfile.mag_scale_z;
    
    // Eğer manyetometre bağlantısı koptuysa veya 0 okuyorsa koruma:
    if(mag_x == 0.0f && mag_y == 0.0f && mag_z == 0.0f) {
        // Kör uçuş jiroskop entegrasyonuna geri dön
        float gz = dc->gyro.calibrated_z - dc->calibProfile.gyro_bias_z; 
        dc->estimated.yaw += gz * dt; 
        return;
    }
    
    // 2. Tilt Compensation (Eğim Düzeltmesi)
    // Roket eğik olduğunda manyetometrenin yere paralel bileşenlerini (Xh, Yh) bulmak için
    // EKF/Attitude kütüphanesinden gelen Pitch ve Roll açılarını kullanıyoruz.
    float roll_rad  = dc->estimated.roll * DEG_TO_RAD;
    float pitch_rad = dc->estimated.pitch * DEG_TO_RAD;
    
    float cos_roll  = cosf(roll_rad);
    float sin_roll  = sinf(roll_rad);
    float cos_pitch = cosf(pitch_rad);
    float sin_pitch = sinf(pitch_rad);
    
    float Xh = mag_x * cos_pitch + mag_z * sin_pitch;
    float Yh = mag_x * sin_roll * sin_pitch + mag_y * cos_roll - mag_z * sin_roll * cos_pitch;
    
    // 3. Yaw Hesaplama (Pusula yönü)
    float yaw_mag = atan2f(-Yh, Xh) * RAD_TO_DEG;
    
    // 4. Complementary Filter (Jiroskop Hızı + Manyetometre Kesinliği)
    // Sabitler ileride FilterConfig'e taşınabilir. (Örn: Alpha = 0.98)
    // Bu basit complementary filtre, karmaşık bir EKF'ye gerek kalmadan Yaw'ı stabilize eder.
    float gz = dc->gyro.calibrated_z - dc->calibProfile.gyro_bias_z;
    
    // Açının 360 derecede sarılması (wrap) problemini önlemek için fark alınır:
    float diff = yaw_mag - dc->estimated.yaw;
    if(diff > 180.0f) diff -= 360.0f;
    if(diff < -180.0f) diff += 360.0f;
    
    float yaw_new = dc->estimated.yaw + (gz * dt); // Önce jiroskopla tahmin et
    yaw_new += diff * 0.02f; // Sonra manyetometreyle %2 düzelt (Alpha = 0.98)
    
    // -180 ile +180 arasında tut
    if(yaw_new > 180.0f) yaw_new -= 360.0f;
    if(yaw_new < -180.0f) yaw_new += 360.0f;
    
    dc->estimated.yaw = yaw_new;

#else
    // MANYETOMETRE YOK - Sadece Jiroskop Entegrasyonu (Kör Uçuş)
    // Not: dc->gyro.calibrated_z zaten hardware tabanlı LPF'den geçmiş halidir.
    // Ancak bias'ı kalibrasyon modülünden (M3.1) geldiği için çıkartıyoruz.
    float gz = dc->gyro.calibrated_z - dc->calibProfile.gyro_bias_z; 
    
    dc->estimated.yaw += gz * dt; 
    
    // -180 ile +180 arasında sınırla
    if(dc->estimated.yaw > 180.0f) dc->estimated.yaw -= 360.0f;
    if(dc->estimated.yaw < -180.0f) dc->estimated.yaw += 360.0f;
#endif
}
