#ifndef INC_FILTER_CONFIG_H_
#define INC_FILTER_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * FACTORY CALIBRATION MODE (0: Flight Mode, 1: Calibration Mode)
 * ========================================================================= */
#ifndef FACTORY_CALIBRATION_MODE
#define FACTORY_CALIBRATION_MODE    0
#endif

// Kalibrasyon Bekleme/Ölçüm Süreleri (Milisaniye Cinsinden)
#define CALIB_TRANSITION_TIME_MS    5000 // Pozisyon değiştirip cihazın titremesinin bitmesi için süre (10 sn)
#define CALIB_MEASURING_TIME_MS     5000 // O pozisyonda tamamen sabitken veri toplanacak süre (20 sn)

/* =========================================================================
 * 0. BİRİM DÖNÜŞÜM SABİTLERİ
 * ========================================================================= */
#define DEG2RAD          0.01745329251f   /* π / 180                          */
#define RAD2DEG          57.2957795131f   /* 180 / π                          */
#define GRAVITY_MPS2     9.80665f         /* Standart yerçekimi [m/s²]        */

/* =========================================================================
 * 1. SENSÖR KANALLARI STATİK SABİTLERİ (BIAS - SCALE - NOISE)
 * Birimler: rawValue biriminde (g, dps, µT, Pa)
 * M1 kalibrasyon sonrası SI'ya çevrilir (m/s², rad/s)
 * ========================================================================= */

// 1.1 İvmeölçer (Accelerometer) – rawValue birimi: g
#define CALIB_ACC_X_BIAS           -0.060000f
#define CALIB_ACC_X_SCALE           1.000766f
#define CALIB_ACC_X_NOISE           0.000007f
#define CALIB_ACC_Y_BIAS            0.008914f
#define CALIB_ACC_Y_SCALE           0.999439f
#define CALIB_ACC_Y_NOISE           0.000002f
#define CALIB_ACC_Z_BIAS           -0.002826f
#define CALIB_ACC_Z_SCALE           0.985759f
#define CALIB_ACC_Z_NOISE           0.000040f
#define CALIB_ACC_THERMAL_DRIFT     0.000000f
// 1.2 Jiroskop (Gyroscope) – rawValue birimi: dps (degree per second)
#define CALIB_GYRO_X_BIAS          -2.454215f
#define CALIB_GYRO_X_SCALE          1.000000f
#define CALIB_GYRO_X_NOISE          0.092418f
#define CALIB_GYRO_Y_BIAS           2.431932f
#define CALIB_GYRO_Y_SCALE          1.000000f
#define CALIB_GYRO_Y_NOISE          0.166621f
#define CALIB_GYRO_Z_BIAS          -0.615513f
#define CALIB_GYRO_Z_SCALE          1.000000f
#define CALIB_GYRO_Z_NOISE          0.033881f

// 1.3 Manyetometre (Magnetometer) – rawValue birimi: µT
#define CALIB_MAG_X_BIAS            0.0000f
#define CALIB_MAG_X_SCALE           1.0000f
#define CALIB_MAG_X_NOISE           0.0500f

#define CALIB_MAG_Y_BIAS            0.0000f
#define CALIB_MAG_Y_SCALE           1.0000f
#define CALIB_MAG_Y_NOISE           0.0500f

#define CALIB_MAG_Z_BIAS            0.0000f
#define CALIB_MAG_Z_SCALE           1.0000f
#define CALIB_MAG_Z_NOISE           0.0500f

// 1.4 İrtifa Sensörü (Barometer) – rawValue birimi: Pa, °C
#define CALIB_BARO_PRESS_BIAS       0.0000f
#define CALIB_BARO_PRESS_SCALE      1.0000f
#define CALIB_BARO_PRESS_NOISE      2.0000f

#define CALIB_BARO_TEMP_BIAS        0.0000f
#define CALIB_BARO_TEMP_SCALE       1.0000f
#define CALIB_BARO_TEMP_NOISE       0.0000f

// 1.5 GPS Kanalları – rawValue birimi: °, m, m/s
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

/* =========================================================================
 * 2. ORIENTATION (YÖNELIM) SABİTLERİ
 * ========================================================================= */
#define ORIENTATION_SYSTEM_GAIN           0.041f  /* Mahony Kp: ivmeölçer düzeltme kazancı */
#define ORIENTATION_SYSTEM_GPS_YAW_GAIN   0.005f  /* GPS ile Yaw düzeltme kazancı          */

/* =========================================================================
 * 3. ORTAM VE RAMPA KALİBRASYON SABİTLERİ
 * ========================================================================= */
#define FILTER_CALIB_SAMPLES_COUNT     100        /* Rampada alınacak referans örnek sayısı */
#define FILTER_SEA_LEVEL_PA_DEFAULT    101325.0f  /* Uluslararası deniz seviyesi basıncı    */
#define FILTER_TEMP_REF_DEFAULT        25.0f      /* Referans kalibrasyon sıcaklığı [°C]    */

/* =========================================================================
 * 4. GÜVEN MOTORU (CONFIDENCE) SABİTLERİ VE SIÇRAMA (SPIKE) LİMİTLERİ
 * Spike limitleri KALİBRE EDİLMİŞ birim cinsindendir (m/s², rad/s, Pa)
 * ========================================================================= */
#define CONFIDENCE_MIN_VALID             0.20f    /* EKF'ye kabul için minimum güven (%20)  */
#define CONFIDENCE_TIMEOUT_US            500000   /* Veri donmasını tespit süresi (500 ms)  */
#define CONFIDENCE_SPIKE_PENALTY_FACTOR  0.5f     /* Sıçramada güven ceza çarpanı           */
#define CONFIDENCE_RECOVERY_RATE         0.05f    /* Döngü başına güven toparlanma artışı   */

#define CONFIDENCE_MAX_SPIKE_ACC_MPS2    15.0f    /* İvmede max adım değişim [m/s²]         */
#define CONFIDENCE_MAX_SPIKE_GYRO_RADPS  0.8726f  /* Jiroskopta max adım değişim [rad/s]    */
#define CONFIDENCE_MAX_SPIKE_BARO_PA     500.0f   /* Basınçta max adım değişim [Pa]         */
#define CONFIDENCE_MAX_SPIKE_MAG_UT      50.0f    /* Manyetometrede max adım değişim [µT]   */

/* FDI (Hata Tespiti ve İzolasyon) Sabitleri */
#define EKF_INNOVATION_GATE_3SIGMA       9.0f     /* 3-Sigma kuralı (İstatistikte %99.7 sınır) */

/* =========================================================================
 * 5. EKF SÜREÇ GÜRÜLTÜSÜ (PROCESS NOISE - Q)
 * Modelin kendi fiziksel/matematiksel belirsizliğidir.
 * Ne kadar küçük olursa EKF tahmine o kadar güvenir.
 * ========================================================================= */

// M3_EKF_ATTITUDE (Yönelim - 7 Durumlu) Q Matrisi Değerleri
#define EKF_Q_ATT_Q                  0.001f  /* Kuaterniyon tahmini belirsizliği */
#define EKF_Q_ATT_BIAS               0.0001f /* Jiroskop bias (kayma) belirsizliği */

// M5_EKF_Z (İrtifa ve Dikey Hız) Q Matrisi Değerleri
#define EKF_Q_Z_POS                  0.01f   /* İrtifa tahminindeki belirsizlik       */
#define EKF_Q_Z_VEL                  0.05f   /* Hız tahminindeki belirsizlik           */
#define EKF_Q_Z_ACC_BIAS             0.001f  /* Z İvmeölçer Bias kayma hızı           */

// M6_EKF_XY (Yatay Konum ve Hız) Q Matrisi Değerleri
#define EKF_Q_XY_POS                 0.01f
#define EKF_Q_XY_VEL                 0.05f

// M4_EKF_YAW (Pusula Yönü) Q Matrisi Değerleri
#define EKF_Q_YAW_ANGLE              0.01f

/* =========================================================================
 * 6. EKF G-KOMPANSASYONU VE DİNAMİK AĞIRLIK SABİTLERİ
 * ========================================================================= */

/* calibratedValue artık m/s² olduğu için eşik de m/s² cinsindendir.         */
/* 12g × 9.81 ≈ 117.7 m/s²: motor yanması sırasında ivmeölçer yoksayılır.   */
#define EKF_G_COMP_THRESHOLD_MPS2    (12.0f * GRAVITY_MPS2)

#define WEIGHT_PR_ACC                1.0f    /* Pitch/Roll için İvmeölçer Ağırlığı   */
#define WEIGHT_YAW_GPS               1.0f    /* Yaw için GPS Rota Ağırlığı           */
#define WEIGHT_YAW_MAG               0.5f    /* Yaw için Manyetometre Ağırlığı       */
#define WEIGHT_ALT_BARO              1.0f    /* İrtifa için Barometre Ağırlığı        */
#define WEIGHT_XY_GPS                1.0f    /* Yatay Konum/Hız için GPS Ağırlığı    */

#ifdef __cplusplus
}
#endif

#endif /* INC_FILTER_CONFIG_H_ */
