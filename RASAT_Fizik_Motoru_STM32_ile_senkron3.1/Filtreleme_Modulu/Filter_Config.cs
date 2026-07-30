using System;

namespace FilterModule
{
    public static class Filter_Config
    {
        /* =========================================================================
         * 0. BİRİM DÖNÜŞÜM SABİTLERİ
         * ========================================================================= */
        public const float DEG2RAD = 0.01745329251f;   /* π / 180                          */
        public const float RAD2DEG = 57.2957795131f;   /* 180 / π                          */
        public const float GRAVITY_MPS2 = 9.80665f;         /* Standart yerçekimi [m/s²]        */

        /* =========================================================================
         * 1. SENSÖR KANALLARI STATİK SABİTLERİ (BIAS - SCALE - NOISE)
         * Birimler: rawValue biriminde (g, dps, µT, Pa)
         * M1 kalibrasyon sonrası SI'ya çevrilir (m/s², rad/s)
         * ========================================================================= */

        // 1.1 İvmeölçer (Accelerometer) – rawValue birimi: g
        public const float CALIB_ACC_X_BIAS = 0.0000f;
        public const float CALIB_ACC_X_SCALE = 1.0000f;
        public const float CALIB_ACC_X_NOISE = 0.2500f;

        public const float CALIB_ACC_Y_BIAS = 0.0000f;
        public const float CALIB_ACC_Y_SCALE = 1.0000f;
        public const float CALIB_ACC_Y_NOISE = 0.2500f;

        public const float CALIB_ACC_Z_BIAS = 0.0000f;
        public const float CALIB_ACC_Z_SCALE = 1.0000f;
        public const float CALIB_ACC_Z_NOISE = 0.2500f;
        public const float CALIB_ACC_THERMAL_DRIFT = 0.0000f;

        // 1.2 Jiroskop (Gyroscope) – rawValue birimi: dps (degree per second)
        public const float CALIB_GYRO_X_BIAS = 0.0000f;
        public const float CALIB_GYRO_X_SCALE = 1.0000f;
        public const float CALIB_GYRO_X_NOISE = 0.0010f;

        public const float CALIB_GYRO_Y_BIAS = 0.0000f;
        public const float CALIB_GYRO_Y_SCALE = 1.0000f;
        public const float CALIB_GYRO_Y_NOISE = 0.0010f;

        public const float CALIB_GYRO_Z_BIAS = 0.0000f;
        public const float CALIB_GYRO_Z_SCALE = 1.0000f;
        public const float CALIB_GYRO_Z_NOISE = 0.0010f;

        // 1.3 Manyetometre (Magnetometer) – rawValue birimi: µT
        public const float CALIB_MAG_X_BIAS = 0.0000f;
        public const float CALIB_MAG_X_SCALE = 1.0000f;
        public const float CALIB_MAG_X_NOISE = 0.0500f;

        public const float CALIB_MAG_Y_BIAS = 0.0000f;
        public const float CALIB_MAG_Y_SCALE = 1.0000f;
        public const float CALIB_MAG_Y_NOISE = 0.0500f;

        public const float CALIB_MAG_Z_BIAS = 0.0000f;
        public const float CALIB_MAG_Z_SCALE = 1.0000f;
        public const float CALIB_MAG_Z_NOISE = 0.0500f;

        // 1.4 İrtifa Sensörü (Barometer) – rawValue birimi: Pa, °C
        public const float CALIB_BARO_PRESS_BIAS = 0.0000f;
        public const float CALIB_BARO_PRESS_SCALE = 1.0000f;
        public const float CALIB_BARO_PRESS_NOISE = 2.0000f;

        public const float CALIB_BARO_TEMP_BIAS = 0.0000f;
        public const float CALIB_BARO_TEMP_SCALE = 1.0000f;
        public const float CALIB_BARO_TEMP_NOISE = 0.0000f;

        // 1.5 GPS Kanalları – rawValue birimi: °, m, m/s
        public const float CALIB_GPS_X_BIAS = 0.0000f;
        public const float CALIB_GPS_X_SCALE = 1.0000f;
        public const float CALIB_GPS_X_NOISE = 0.0100f;

        public const float CALIB_GPS_Y_BIAS = 0.0000f;
        public const float CALIB_GPS_Y_SCALE = 1.0000f;
        public const float CALIB_GPS_Y_NOISE = 0.0100f;

        public const float CALIB_GPS_Z_BIAS = 0.0000f;
        public const float CALIB_GPS_Z_SCALE = 1.0000f;
        public const float CALIB_GPS_Z_NOISE = 0.0100f;

        public const float CALIB_GPS_VEL_BIAS = 0.0000f;
        public const float CALIB_GPS_VEL_SCALE = 1.0000f;
        public const float CALIB_GPS_VEL_NOISE = 0.0100f;

        /* =========================================================================
         * 2. ORIENTATION (YÖNELIM) SABİTLERİ
         * ========================================================================= */
        public const float ORIENTATION_SYSTEM_GAIN = 0.041f;  /* Mahony Kp: ivmeölçer düzeltme kazancı */
        public const float ORIENTATION_SYSTEM_GPS_YAW_GAIN = 0.005f;  /* GPS ile Yaw düzeltme kazancı          */

        /* =========================================================================
         * 3. ORTAM VE RAMPA KALİBRASYON SABİTLERİ
         * ========================================================================= */
        public const int FILTER_CALIB_SAMPLES_COUNT = 100;        /* Rampada alınacak referans örnek sayısı */
        public const float FILTER_SEA_LEVEL_PA_DEFAULT = 101325.0f;  /* Uluslararası deniz seviyesi basıncı    */
        public const float FILTER_TEMP_REF_DEFAULT = 25.0f;      /* Referans kalibrasyon sıcaklığı [°C]    */

        /* =========================================================================
         * 4. GÜVEN MOTORU (CONFIDENCE) SABİTLERİ VE SIÇRAMA (SPIKE) LİMİTLERİ
         * Spike limitleri KALİBRE EDİLMİŞ birim cinsindendir (m/s², rad/s, Pa)
         * ========================================================================= */
        public const float CONFIDENCE_MIN_VALID = 0.20f;    /* EKF'ye kabul için minimum güven (%20)  */
        public const uint CONFIDENCE_TIMEOUT_US = 500000;   /* Veri donmasını tespit süresi (500 ms)  */
        public const float CONFIDENCE_SPIKE_PENALTY_FACTOR = 0.5f;     /* Sıçramada güven ceza çarpanı           */
        public const float CONFIDENCE_RECOVERY_RATE = 0.05f;    /* Döngü başına güven toparlanma artışı   */

        public const float CONFIDENCE_MAX_SPIKE_ACC_MPS2 = 15.0f;    /* İvmede max adım değişim [m/s²]         */
        public const float CONFIDENCE_MAX_SPIKE_GYRO_RADPS = 0.8726f;  /* Jiroskopta max adım değişim [rad/s]    */
        public const float CONFIDENCE_MAX_SPIKE_BARO_PA = 500.0f;   /* Basınçta max adım değişim [Pa]         */
        public const float CONFIDENCE_MAX_SPIKE_MAG_UT = 50.0f;    /* Manyetometrede max adım değişim [µT]   */

        /* =========================================================================
         * 5. EKF SÜREÇ GÜRÜLTÜSÜ (PROCESS NOISE - Q)
         * Modelin kendi fiziksel/matematiksel belirsizliğidir.
         * Ne kadar küçük olursa EKF tahmine o kadar güvenir.
         * ========================================================================= */

        // M5_EKF_Z (İrtifa ve Dikey Hız) Q Matrisi Değerleri
        public const float EKF_Q_Z_POS = 0.01f;   /* İrtifa tahminindeki belirsizlik       */
        public const float EKF_Q_Z_VEL = 0.05f;   /* Hız tahminindeki belirsizlik           */
        public const float EKF_Q_Z_ACC_BIAS = 0.001f;  /* Z İvmeölçer Bias kayma hızı           */

        // M6_EKF_XY (Yatay Konum ve Hız) Q Matrisi Değerleri
        public const float EKF_Q_XY_POS = 0.01f;
        public const float EKF_Q_XY_VEL = 0.05f;

        // M4_EKF_YAW (Pusula Yönü) Q Matrisi Değerleri
        public const float EKF_Q_YAW_ANGLE = 0.01f;

        /* =========================================================================
         * 6. EKF G-KOMPANSASYONU VE DİNAMİK AĞIRLIK SABİTLERİ
         * ========================================================================= */

        /* calibratedValue artık m/s² olduğu için eşik de m/s² cinsindendir.         */
        /* 12g × 9.81 ≈ 117.7 m/s²: motor yanması sırasında ivmeölçer yoksayılır.   */
        public const float EKF_G_COMP_THRESHOLD_MPS2 = (12.0f * GRAVITY_MPS2);

        public const float WEIGHT_PR_ACC = 1.0f;    /* Pitch/Roll için İvmeölçer Ağırlığı   */
        public const float WEIGHT_YAW_GPS = 1.0f;    /* Yaw için GPS Rota Ağırlığı           */
        public const float WEIGHT_YAW_MAG = 0.5f;    /* Yaw için Manyetometre Ağırlığı       */
        public const float WEIGHT_ALT_BARO = 1.0f;    /* İrtifa için Barometre Ağırlığı        */
        public const float WEIGHT_XY_GPS = 1.0f;    /* Yatay Konum/Hız için GPS Ağırlığı    */
    }
}
