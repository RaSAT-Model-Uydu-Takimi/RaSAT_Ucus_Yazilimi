using System;

namespace FilterModule
{
    public struct M4_EKF_Yaw_t
    {
        public float state_yaw;    // yaw (derece)
        public float P;            // kovaryans
    }

    public static class M4_EKF_Yaw
    {
        /* -------------------------------------------------------------------------- */
        /*  Açıyı -180..+180 derece aralığına sığdır                                 */
        /* -------------------------------------------------------------------------- */
        private static float wrap_180(float angle)
        {
            while (angle > 180.0f) angle -= 360.0f;
            while (angle < -180.0f) angle += 360.0f;
            return angle;
        }

        /* ========================================================================== */
        /*  M4_Yaw_Init                                                              */
        /* ========================================================================== */
        public static void M4_Yaw_Init(ref M4_EKF_Yaw_t ekf)
        {
            ekf.state_yaw = 0.0f;   /* derece cinsinden */
            ekf.P = 1.0f;
        }

        /* ========================================================================== */
        /*  M4_Yaw_Update                                                            */
        /*                                                                            */
        /*  1-Durumlu EKF: Yaw (heading) tahmini [derece]                            */
        /*                                                                            */
        /*  TAHMİN:   3-eksen Euler yaw hız denklemi (rad/s → dps çevrimi)           */
        /*  ÖLÇÜM 1:  Tilt-kompanse manyetometre heading (her zaman, güven varsa)    */
        /*  ÖLÇÜM 2:  GPS Course (hız > 2 m/s iken)                                 */
        /*                                                                            */
        /*  GİRİŞ BİRİMLERİ:                                                         */
        /*    gyro.calibratedValue = rad/s                                            */
        /*    estimated.pitch/roll = derece (M3'ten)                                  */
        /*    gps.course           = derece                                           */
        /*    mag.calibratedValue  = µT                                               */
        /* ========================================================================== */
        public static void M4_Yaw_Update(ref M4_EKF_Yaw_t ekf, ref DataCenter dataC, float dt_seconds)
        {
            if (dt_seconds <= 0.0f) return;

            /* ================================================================== */
            /*  1. TAHMİN ADIMI – 3 eksen Euler yaw hız denklemi                  */
            /*     ψ̇ = (ωy·sin(φ) + ωz·cos(φ)) / cos(θ)                         */
            /*     φ = roll, θ = pitch, ω = gyro [rad/s]                          */
            /* ================================================================== */
            float gx = dataC.gyro.x.calibratedValue;  /* rad/s */
            float gy = dataC.gyro.y.calibratedValue;
            float gz = dataC.gyro.z.calibratedValue;

            float pitch_rad = dataC.estimated.pitch.value * Filter_Config.DEG2RAD;
            float roll_rad  = dataC.estimated.roll.value  * Filter_Config.DEG2RAD;

            float sp = (float)Math.Sin(pitch_rad), cp = (float)Math.Cos(pitch_rad);
            float sr = (float)Math.Sin(roll_rad),  cr = (float)Math.Cos(roll_rad);

            /* cos(pitch) koruması – gimbal lock bölgesinde sınırla */
            float cp_safe = (cp > 0.01f || cp < -0.01f) ? cp : 0.01f;

            /* Euler yaw hızı (rad/s) → dps çevrimi */
            float yaw_rate_rads = (gy * sr + gz * cr) / cp_safe;
            float yaw_rate_dps  = yaw_rate_rads * Filter_Config.RAD2DEG;

            /* State tahmini (derece cinsinden) */
            ekf.state_yaw += yaw_rate_dps * dt_seconds;
            ekf.state_yaw = wrap_180(ekf.state_yaw);

            /* Kovaryans tahmini: P = P + Q */
            ekf.P += Filter_Config.EKF_Q_YAW_ANGLE;

            /* ================================================================== */
            /*  2. ÖLÇÜM 1 – Tilt-kompanse manyetometre heading                   */
            /*     Mag güveni varsa her zaman kullanılır (durağan + hareketli)     */
            /* ================================================================== */
            float min_mag_conf = dataC.mag.x.confidence;
            if (dataC.mag.y.confidence < min_mag_conf) min_mag_conf = dataC.mag.y.confidence;
            if (dataC.mag.z.confidence < min_mag_conf) min_mag_conf = dataC.mag.z.confidence;

            if (min_mag_conf > Filter_Config.CONFIDENCE_MIN_VALID)
            {
                float mx = dataC.mag.x.calibratedValue;
                float my = dataC.mag.y.calibratedValue;
                float mz = dataC.mag.z.calibratedValue;

                /* Tilt kompanzasyonu: manyetometre vektörünü yatay düzleme çevir */
                float Mx =  mx * cp + my * sr * sp + mz * cr * sp;
                float My =  my * cr - mz * sr;

                float mag_heading_deg = (float)Math.Atan2(-My, Mx) * Filter_Config.RAD2DEG;

                /* Dinamik R: güven düştükçe ölçüm gürültüsü artar */
                float dynamic_R_mag = 1.0f / (Filter_Config.WEIGHT_YAW_MAG * min_mag_conf + 1e-6f);

                /* İnovasyon (açı farkı -180..+180) */
                float y_mag = wrap_180(mag_heading_deg - ekf.state_yaw);

                /* Kalman kazancı */
                float S_mag = ekf.P + dynamic_R_mag;
                float K_mag = ekf.P / (S_mag + 1e-12f);

                /* Düzeltme */
                ekf.state_yaw = wrap_180(ekf.state_yaw + K_mag * y_mag);
                ekf.P = (1.0f - K_mag) * ekf.P;
            }

            /* ================================================================== */
            /*  3. ÖLÇÜM 2 – GPS Course (İPTAL EDİLDİ)                           */
            /*     Model Uydu (CanSat) paraşütle inerken rüzgara kapılıp bir      */
            /*     yöne sürüklenir (Course), ancak uydu kendi etrafında fırıl     */
            /*     fırıl dönebilir (Yaw). Bu yüzden Course = Yaw varsayımı       */
            /*     uçaklarda çalışsa da uydularda YANLIŞTIR. Sadece Mag kullanılır*/
            /* ================================================================== */
            // GPS Course ile Yaw düzeltmesi kodu Abdullah Köker'in uyarısı üzerine kaldırıldı.

            /* ================================================================== */
            /*  4. ÇIKTILAR                                                        */
            /* ================================================================== */
            dataC.estimated.yaw.value = ekf.state_yaw;

            /* Yaw güveni: P küçükse güven yüksek */
            dataC.estimated.yaw.confidence = 1.0f / (1.0f + ekf.P);
        }
    }
}
