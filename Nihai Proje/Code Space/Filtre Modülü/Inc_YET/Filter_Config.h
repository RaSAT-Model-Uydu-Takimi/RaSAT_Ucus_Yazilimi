#ifndef INC_FILTER_CONFIG_H_
#define INC_FILTER_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * 1. SENSÖR KANALLARI STATİK SABİTLERİ (BIAS - SCALE - NOISE)
 * Her Sensor için 3 adet sabit değer girilir.
 * BIAS: Sabit hata (Sıfır kayması)
 * SCALE: Bağıl hata (Ölçek çarpanı)
 * NOISE: Sensör sabitkenki rastgele hata (Ölçüm gürültü varyansı)
 * ========================================================================= */

// 1.1 İvmeölçer (Accelerometer) Kanalları
#define CALIB_ACC_X_BIAS            0.0000f
#define CALIB_ACC_X_SCALE           1.0000f
#define CALIB_ACC_X_NOISE           0.2500f  

#define CALIB_ACC_Y_BIAS            0.0000f
#define CALIB_ACC_Y_SCALE           1.0000f
#define CALIB_ACC_Y_NOISE           0.2500f  

#define CALIB_ACC_Z_BIAS            0.0000f
#define CALIB_ACC_Z_SCALE           1.0000f
#define CALIB_ACC_Z_NOISE           0.2500f  
#define CALIB_ACC_THERMAL_DRIFT     0.0000f // IVMEÖÇLERE ÖZEL şimdilik etkisiz bırakıldı. 

// 1.2 Jiroskop (Gyroscope) Kanalları
#define CALIB_GYRO_X_BIAS           0.0000f
#define CALIB_GYRO_X_SCALE          1.0000f
#define CALIB_GYRO_X_NOISE          0.0010f  

#define CALIB_GYRO_Y_BIAS           0.0000f
#define CALIB_GYRO_Y_SCALE          1.0000f
#define CALIB_GYRO_Y_NOISE          0.0010f  

#define CALIB_GYRO_Z_BIAS           0.0000f
#define CALIB_GYRO_Z_SCALE          1.0000f
#define CALIB_GYRO_Z_NOISE          0.0010f 

// 1.3 Manyetometre (Magnetometer) Kanalları 
#define CALIB_MAG_X_BIAS            0.0000f  
#define CALIB_MAG_X_SCALE           1.0000f  
#define CALIB_MAG_X_NOISE           0.0500f  

#define CALIB_MAG_Y_BIAS            0.0000f
#define CALIB_MAG_Y_SCALE           1.0000f
#define CALIB_MAG_Y_NOISE           0.0500f

#define CALIB_MAG_Z_BIAS            0.0000f
#define CALIB_MAG_Z_SCALE           1.0000f
#define CALIB_MAG_Z_NOISE           0.0500f

// 1.4 İrtifa Sensörü (Barometer) Kanalları
#define CALIB_BARO_PRESS_BIAS       0.0000f
#define CALIB_BARO_PRESS_SCALE      1.0000f
#define CALIB_BARO_PRESS_NOISE      2.0000f

#define CALIB_BARO_TEMP_BIAS       0.0000f
#define CALIB_BARO_TEMP_SCALE      1.0000f
#define CALIB_BARO_TEMP_NOISE      0.0000f

// 1.5 GPS Kanalları
#define CALIB_GPS_X_BIAS            0.0000f
#define CALIB_GPS_X_SCALE           1.0000f
#define CALIB_GPS_X_NOISE           0.0100f

#define CALIB_GPS_Y_BIAS            0.0000f
#define CALIB_GPS_Y_SCALE           1.0000f
#define CALIB_GPS_Y_NOISE           0.0100f

#define CALIB_GPS_Z_BIAS            0.0000f
#define CALIB_GPS_Z_SCALE           1.0000f
#define CALIB_GPS_Z_NOISE           0.0100f

#define CALIB_GPS_VEL_BIAS          0.0000f
#define CALIB_GPS_VEL_SCALE         1.0000f
#define CALIB_GPS_VEL_NOISE         0.0100f

//2. ORIENTATION SYSTEM SABİTLERİ

#define ORIENTATION_SYSTEM_GAIN           0.041f  // Jiroskop sürüklenmesini ivmeölçerle düzeltme kazancı
#define ORIENTATION_SYSTEM_GPS_YAW_GAIN   0.005f  // Roket ayrılması sonrası GPS ile Yaw düzeltme kazancı


// 3. ORTAM VE RAMPA KALİBRASYON SABİTLERİ
#define FILTER_CALIB_SAMPLES_COUNT     100       // Rampada ortalaması alınacak referans örnek sayısı
#define FILTER_SEA_LEVEL_PA_DEFAULT    101325.0f // Uluslararası deniz seviyesi basıncı [Pa]
#define FILTER_TEMP_REF_DEFAULT        25.0f     // Referans kalibrasyon sıcaklığı [°C]


// 4. GÜVEN MOTORU (CONFIDENCE) SABİTLERİ VE SIÇRAMA (SPIKE) LİMİTLERİ
#define CONFIDENCE_MIN_VALID             0.20f   // EKF'ye kabul için minimum güven skoru (%20)
#define CONFIDENCE_TIMEOUT_US            500000  // Veri donmasını tespit süresi (500.000 mikrosaniye = 500 ms)
#define CONFIDENCE_SPIKE_PENALTY_FACTOR  0.5f    // Sıçrama durumunda güvenin düşürüleceği ceza çarpanı

#define CONFIDENCE_MAX_SPIKE_ACC_MPS2    15.0f   // İvmede bir adımda izin verilen en büyük değişim [m/s^2]
#define CONFIDENCE_MAX_SPIKE_GYRO_RADPS  0.8726f // Jiroskopta bir adımda izin verilen en büyük değişim [rad/s]
#define CONFIDENCE_MAX_SPIKE_BARO_PA     500.0f  // Basınçta bir adımda izin verilen en büyük değişim [Pa]

#ifdef __cplusplus
}
#endif

// 5. EKF SUREC GURULTUSU (PROCESS NOISE - Q)
// Modelin kendi fiziksel matematiksel belirsizligidir. 
// Ne kadar kucuk olursa EKF tahmine o kadar guvenir.


// M5_EKF_Z (Irtifa ve Dikey Hiz) Q Matrisi Degerleri
#define EKF_Q_Z_POS                  0.01f   // Irtifa tahminindeki model belirsizligi
#define EKF_Q_Z_VEL                  0.05f   // Hiz tahminindeki model belirsizligi
#define EKF_Q_Z_ACC_BIAS             0.001f  // Z Ivmeolcer Bias kayma hizi

// M6_EKF_XY (Yatay Konum ve Hiz) Q Matrisi Degerleri
#define EKF_Q_XY_POS                 0.01f
#define EKF_Q_XY_VEL                 0.05f

// M4_EKF_YAW (Pusula Yonu) Q Matrisi Degerleri
#define EKF_Q_YAW_ANGLE              0.01f

// EKF YONELIM (PITCH/ROLL) ICIN G-KOMPANSASYONU
#define EKF_G_COMP_THRESHOLD_MPS2    12.0f   // Bu ivmenin uzerinde EKF, ivmeolceri yok sayar (Sadece Jiro kullanir)

// 6. EKF DINAMIK AGIRLIK (WEIGHT) SABITLERI
#define WEIGHT_PR_ACC                1.0f    // Pitch/Roll icin Ivmeolcer Agirligi
#define WEIGHT_YAW_GPS               1.0f    // Yaw icin GPS Rota Agirligi
#define WEIGHT_ALT_BARO              1.0f    // Irtifa icin Barometre Agirligi
#define WEIGHT_XY_GPS                1.0f    // Yatay Konum/Hiz icin GPS Agirligi

#endif /* INC_FILTER_CONFIG_H_ */
