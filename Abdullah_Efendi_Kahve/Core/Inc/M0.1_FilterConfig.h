/*
 * M0.1_FilterConfig.h
 *
 * Configuration file for Filter System and Operational Modes
 */

#ifndef INC_FILTERCONFIG_H_
#define INC_FILTERCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

// --- ÇALIŞMA MODU SEÇİMİ ---
// (Bu artık dinamik olarak M2.0_SystemCore içinde yönetilir)
// --------------------------

// --- EKF (EXTENDED KALMAN FILTER) AYARLARI ---
// EKF için varsayılan gürültü varyans değerleri (Uçuş öncesi ölçülemezse kullanılır)
#define EKF_INITIAL_Q_GYRO    0.0001f  // Jiroskop gürültü varyansı (Süreç Gürültüsü Q)
#define EKF_INITIAL_R_ACCEL   0.05f    // İvmeölçer gürültü varyansı (Ölçüm Gürültüsü R)
#define EKF_INITIAL_P         1.0f     // Başlangıç hata kovaryansı (P Matrisi)

// Dinamik R matrisi (Titreşim engelleme) için ivme sınırları (m/s^2)
// Yerçekimi (9.81) değerinden bu kadar sapılırsa R matrisi büyütülür (İvmeölçere güven azalır)
#define EKF_VIBRATION_THRESHOLD  2.0f   // 9.81 ± 2.0 m/s^2 dışı sarsıntı sayılır
#define EKF_R_MULTIPLIER         100.0f // Sarsıntı anında R matrisi kaç kat büyütülecek?

#define DEG_TO_RAD (3.14159265358979323846f / 180.0f)
#define RAD_TO_DEG (180.0f / 3.14159265358979323846f)
#define GRAVITY_MSS (9.80665f) // Yerçekimi ivmesi (m/s^2)
// --- TİTREŞİM VE İVME FİLTRESİ (VIBRATION & ACCEL LPF) ---
// İvmeölçer için yazılımsal LPF katsayısı (0.0 ile 1.0 arası)
// Değer küçüldükçe filtreleme artar, tepki süresi yavaşlar. (Örn: 0.1 = %10 yeni veri, %90 eski veri)
#define ACCEL_LPF_ALPHA    0.1f

// Dinamik Kp için ivme vektörü büyüklüğü toleransı (m/s^2)
// Yerçekimi 9.81 m/s^2 civarındadır. Eğer okunan toplam ivme, 
// bu sınırların dışına çıkarsa (çok sarsıntı varsa), Mahony Kp sıfırlanır.
#define ACCEL_MAG_MIN      7.0f   // ~0.7G
#define ACCEL_MAG_MAX      13.0f  // ~1.3G
// --------------------------

// --- SENSÖR KALİBRASYON VARSAYILANLARI ---

// İvmeölçer (Accelerometer)
#define ACC_BIAS_X  0.0f
#define ACC_BIAS_Y  0.0f
#define ACC_BIAS_Z  0.0f
#define ACC_SCALE_X 1.0f
#define ACC_SCALE_Y 1.0f
#define ACC_SCALE_Z 1.0f
#define ACC_NOISE_X 0.0f
#define ACC_NOISE_Y 0.0f
#define ACC_NOISE_Z 0.0f

// Jiroskop (Gyroscope)
#define GYRO_BIAS_X 0.0f
#define GYRO_BIAS_Y 0.0f
#define GYRO_BIAS_Z 0.0f
#define GYRO_SCALE_X 1.0f
#define GYRO_SCALE_Y 1.0f
#define GYRO_SCALE_Z 1.0f
#define GYRO_NOISE_X 0.0f
#define GYRO_NOISE_Y 0.0f
#define GYRO_NOISE_Z 0.0f

// Manyetometre (Magnetometer)
#define MAG_BIAS_X  0.0f
#define MAG_BIAS_Y  0.0f
#define MAG_BIAS_Z  0.0f
#define MAG_SCALE_X 1.0f
#define MAG_SCALE_Y 1.0f
#define MAG_SCALE_Z 1.0f
#define MAG_NOISE_X 0.0f
#define MAG_NOISE_Y 0.0f
#define MAG_NOISE_Z 0.0f

// Barometre (Barometer)
#define BARO_PRESS_BIAS  0.0f
#define BARO_PRESS_SCALE 1.0f
#define BARO_PRESS_NOISE 0.0f
#define BARO_TEMP_BIAS   0.0f
#define BARO_TEMP_SCALE  1.0f
#define BARO_TEMP_NOISE  0.0f

// GPS
#define GPS_LAT_BIAS    0.0
#define GPS_LAT_SCALE   1.0
#define GPS_LON_BIAS    0.0
#define GPS_LON_SCALE   1.0
#define GPS_ALT_BIAS    0.0f
#define GPS_ALT_SCALE   1.0f
#define GPS_NOISE       0.0f

// Batarya (Battery)
#define BATT_VOLT_BIAS  0.0f
#define BATT_VOLT_SCALE 1.0f
#define BATT_VOLT_NOISE 0.0f
#define BATT_CURR_BIAS  0.0f
#define BATT_CURR_SCALE 1.0f
#define BATT_CURR_NOISE 0.0f
// --------------------------


#ifdef __cplusplus
}
#endif

#endif /* INC_FILTERCONFIG_H_ */
