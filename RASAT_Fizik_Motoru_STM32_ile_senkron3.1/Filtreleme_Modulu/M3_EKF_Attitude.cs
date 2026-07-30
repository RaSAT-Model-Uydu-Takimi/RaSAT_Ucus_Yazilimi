using System;

namespace FilterModule
{
    public static class M3_EKF_Attitude
    {
        /* -------------------------------------------------------------------------- */
        /*  Fast Inverse Square Root (gömülü sistemler için optimize)                 */
        /* -------------------------------------------------------------------------- */
        private static float invSqrt(float x)
        {
            float halfx = 0.5f * x;
            float y = x;
            
            // Fast inverse square root using BitConverter
            int i = BitConverter.SingleToInt32Bits(y);
            i = 0x5f3759df - (i >> 1);
            y = BitConverter.Int32BitsToSingle(i);
            
            y = y * (1.5f - (halfx * y * y));
            return y;
        }

        /* ========================================================================== */
        /*  M3_Attitude_Init                                                          */
        /* ========================================================================== */
        public static void M3_Attitude_Init(ref DataCenter dataC)
        {
            /* Birim kuaterniyon: uydu düz duruyor */
            dataC.estimated.q0.value = 1.0f;
            dataC.estimated.q1.value = 0.0f;
            dataC.estimated.q2.value = 0.0f;
            dataC.estimated.q3.value = 0.0f;

            dataC.estimated.pitch.value = 0.0f;
            dataC.estimated.roll.value  = 0.0f;
            dataC.estimated.yaw.value   = 0.0f;
        }

        /* ========================================================================== */
        /*  M3_Attitude_Update                                                        */
        /*                                                                            */
        /*  Mahony tarzı tamamlayıcı filtre (kuaterniyon tabanlı).                    */
        /*  - Gyro entegrasyonu ile tahmin (3 eksen, yaw dahil)                       */
        /*  - İvmeölçer düzeltmesi ile pitch/roll kararlılığı                         */
        /*  - G-Kompansasyonu: yüksek ivmede ivmeölçer devre dışı                     */
        /*                                                                            */
        /*  GİRİŞ BİRİMLERİ: calibratedValue → ivme m/s², gyro rad/s                 */
        /*  ÇIKIŞ: pitch, roll, yaw [°] ve q0-q3                                     */
        /* ========================================================================== */
        public static void M3_Attitude_Update(ref DataCenter dataC, float dt_seconds)
        {
            if (dt_seconds <= 0.0f) return;

            /* Kalibre edilmiş sensör verilerini al (SI birimleri) */
            float ax = dataC.acc.x.calibratedValue;   /* m/s² */
            float ay = dataC.acc.y.calibratedValue;
            float az = dataC.acc.z.calibratedValue;

            float gx = dataC.gyro.x.calibratedValue;  /* rad/s */
            float gy = dataC.gyro.y.calibratedValue;
            float gz = dataC.gyro.z.calibratedValue;

            float q0 = dataC.estimated.q0.value;
            float q1 = dataC.estimated.q1.value;
            float q2 = dataC.estimated.q2.value;
            float q3 = dataC.estimated.q3.value;

            /* ================================================================== */
            /*  G-KOMPANSASYONU: İvme büyüklüğü kontrol et                        */
            /*  calibratedValue m/s² olduğu için eşik de m/s² cinsindendir        */
            /* ================================================================== */
            float acc_magnitude = (float)Math.Sqrt(ax * ax + ay * ay + az * az);
            float dynamic_acc_weight = 0.0f;

            if (acc_magnitude > 1.0f && acc_magnitude < Filter_Config.EKF_G_COMP_THRESHOLD_MPS2)
            {
                /* Motor yanmıyor (aşırı G yok) → ivmeölçere güven skoru ile ağırlık ver */
                float min_acc_conf = dataC.acc.x.confidence;
                if (dataC.acc.y.confidence < min_acc_conf) min_acc_conf = dataC.acc.y.confidence;
                if (dataC.acc.z.confidence < min_acc_conf) min_acc_conf = dataC.acc.z.confidence;

                dynamic_acc_weight = Filter_Config.WEIGHT_PR_ACC * min_acc_conf;
            }

            /* ================================================================== */
            /*  DÜZELTME ADIMI (İvmeölçer → yerçekimi vektörü hatası)             */
            /* ================================================================== */
            if (dynamic_acc_weight > 0.0f)
            {
                /* İvmeölçeri normalize et */
                float norm_acc = invSqrt(ax * ax + ay * ay + az * az);
                ax *= norm_acc;
                ay *= norm_acc;
                az *= norm_acc;

                /* Kuaterniyondan tahmini yerçekimi vektörü (gövde çerçevesinde) */
                float vx = 2.0f * (q1 * q3 - q0 * q2);
                float vy = 2.0f * (q0 * q1 + q2 * q3);
                float vz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;

                /* Hata vektörü (cross product) */
                float ex = (ay * vz - az * vy);
                float ey = (az * vx - ax * vz);
                float ez = (ax * vy - ay * vx);

                /* Jiroskop değerine hatayı ORIENTATION_SYSTEM_GAIN ve ağırlıkla ekle */
                gx += Filter_Config.ORIENTATION_SYSTEM_GAIN * dynamic_acc_weight * ex;
                gy += Filter_Config.ORIENTATION_SYSTEM_GAIN * dynamic_acc_weight * ey;
                gz += Filter_Config.ORIENTATION_SYSTEM_GAIN * dynamic_acc_weight * ez;
            }

            /* ================================================================== */
            /*  TAHMİN ADIMI: Kuaterniyon entegrasyonu (tüm 3 eksen)              */
            /*  Hamilton konvansiyonu: q_dot = 0.5 * q ⊗ ω                       */
            /* ================================================================== */
            float q0_dot = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
            float q1_dot = 0.5f * ( q0 * gx + q2 * gz - q3 * gy);
            float q2_dot = 0.5f * ( q0 * gy - q1 * gz + q3 * gx);
            float q3_dot = 0.5f * ( q0 * gz + q1 * gy - q2 * gx);

            q0 += q0_dot * dt_seconds;
            q1 += q1_dot * dt_seconds;
            q2 += q2_dot * dt_seconds;
            q3 += q3_dot * dt_seconds;

            /* Normalizasyon */
            float norm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
            q0 *= norm;
            q1 *= norm;
            q2 *= norm;
            q3 *= norm;

            /* ================================================================== */
            /*  EULER AÇILARINA DÖNÜŞTÜR (Pitch, Roll, Yaw)                       */
            /*  Kuaterniyon 3 ekseni de entegre ettiği için yaw da çıkartılır.    */
            /*  Bu yaw "ham" (gyro-only) yaw'dır; M4 tarafından düzeltilecektir.  */
            /* ================================================================== */
            dataC.estimated.roll.value  = (float)Math.Atan2(2.0f * (q0 * q1 + q2 * q3),
                                                  1.0f - 2.0f * (q1 * q1 + q2 * q2)) * Filter_Config.RAD2DEG;

            /* asinf girişini [-1, 1] aralığında tut (float hassasiyet koruması) */
            float sinP = 2.0f * (q0 * q2 - q3 * q1);
            if (sinP >  1.0f) sinP =  1.0f;
            if (sinP < -1.0f) sinP = -1.0f;
            dataC.estimated.pitch.value = (float)Math.Asin(sinP) * Filter_Config.RAD2DEG;

            /* Yaw (kuaterniyondan ham değer – M4 düzeltecek) */
            dataC.estimated.yaw.value = (float)Math.Atan2(2.0f * (q0 * q3 + q1 * q2),
                                                1.0f - 2.0f * (q2 * q2 + q3 * q3)) * Filter_Config.RAD2DEG;

            /* Kuaterniyon değerlerini kaydet */
            dataC.estimated.q0.value = q0;
            dataC.estimated.q1.value = q1;
            dataC.estimated.q2.value = q2;
            dataC.estimated.q3.value = q3;

            /* Güven: Gyro güveninin minimumu (gyro her zaman kullanılıyor) */
            float min_gyro_conf = dataC.gyro.x.confidence;
            if (dataC.gyro.y.confidence < min_gyro_conf) min_gyro_conf = dataC.gyro.y.confidence;
            if (dataC.gyro.z.confidence < min_gyro_conf) min_gyro_conf = dataC.gyro.z.confidence;

            dataC.estimated.pitch.confidence = min_gyro_conf;
            dataC.estimated.roll.confidence  = min_gyro_conf;
            dataC.estimated.q0.confidence    = min_gyro_conf;
            dataC.estimated.q1.confidence    = min_gyro_conf;
            dataC.estimated.q2.confidence    = min_gyro_conf;
            dataC.estimated.q3.confidence    = min_gyro_conf;
        }
    }
}
