#include "M4_EKF_Yaw.h"
#include "Filter_Config.h"
#include <math.h>

/* -------------------------------------------------------------------------- */
/*  Açıyı -180..+180 derece aralığına sığdır                                 */
/* -------------------------------------------------------------------------- */
static float wrap_180(float angle) {
    while (angle >  180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

/* ========================================================================== */
/*  M4_Yaw_Init                                                              */
/* ========================================================================== */
void M4_Yaw_Init(M4_EKF_Yaw_t *ekf) {
    ekf->state_yaw = 0.0f;   /* derece cinsinden */
    ekf->P = 1.0f;
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
void M4_Yaw_Update(M4_EKF_Yaw_t *ekf, DataCenter *dataC, float dt_seconds) {
    if (dataC == NULL || dt_seconds <= 0.0f) return;

    /* ================================================================== */
    /*  1. TAHMİN ADIMI – 3 eksen Euler yaw hız denklemi                  */
    /*     ψ̇ = (ωy·sin(φ) + ωz·cos(φ)) / cos(θ)                         */
    /*     φ = roll, θ = pitch, ω = gyro [rad/s]                          */
    /* ================================================================== */
    float gx = dataC->gyro.x.calibratedValue;  /* rad/s */
    float gy = dataC->gyro.y.calibratedValue;
    float gz = dataC->gyro.z.calibratedValue;

    float pitch_rad = dataC->estimated.pitch.value * DEG2RAD;
    float roll_rad  = dataC->estimated.roll.value  * DEG2RAD;

    float sp = sinf(pitch_rad), cp = cosf(pitch_rad);
    float sr = sinf(roll_rad),  cr = cosf(roll_rad);

    /* cos(pitch) koruması – gimbal lock bölgesinde sınırla */
    float cp_safe = (cp > 0.01f || cp < -0.01f) ? cp : 0.01f;

    /* Euler yaw hızı (rad/s) → dps çevrimi */
    float yaw_rate_rads = (gy * sr + gz * cr) / cp_safe;
    float yaw_rate_dps  = yaw_rate_rads * RAD2DEG;

    /* State tahmini (derece cinsinden) */
    ekf->state_yaw += yaw_rate_dps * dt_seconds;
    ekf->state_yaw = wrap_180(ekf->state_yaw);

    /* Kovaryans tahmini: P = P + Q */
    ekf->P += EKF_Q_YAW_ANGLE;

    /* ================================================================== */
    /*  2. ÖLÇÜM 1 – Tilt-kompanse manyetometre heading                   */
    /*     Mag güveni varsa her zaman kullanılır (durağan + hareketli)     */
    /* ================================================================== */
    float min_mag_conf = dataC->mag.x.confidence;
    if (dataC->mag.y.confidence < min_mag_conf) min_mag_conf = dataC->mag.y.confidence;
    if (dataC->mag.z.confidence < min_mag_conf) min_mag_conf = dataC->mag.z.confidence;

    if (min_mag_conf > CONFIDENCE_MIN_VALID) {
        float mx = dataC->mag.x.calibratedValue;
        float my = dataC->mag.y.calibratedValue;
        float mz = dataC->mag.z.calibratedValue;

        /* Tilt kompanzasyonu: manyetometre vektörünü yatay düzleme çevir */
        float Mx =  mx * cp + my * sr * sp + mz * cr * sp;
        float My =  my * cr - mz * sr;

        float mag_heading_deg = atan2f(-My, Mx) * RAD2DEG;

        /* Dinamik R: güven düştükçe ölçüm gürültüsü artar */
        float dynamic_R_mag = 1.0f / (WEIGHT_YAW_MAG * min_mag_conf + 1e-6f);

        /* İnovasyon (açı farkı -180..+180) */
        float y_mag = wrap_180(mag_heading_deg - ekf->state_yaw);

        /* Kalman kazancı */
        float S_mag = ekf->P + dynamic_R_mag;
        float K_mag = ekf->P / (S_mag + 1e-12f);

        /* Düzeltme */
        ekf->state_yaw = wrap_180(ekf->state_yaw + K_mag * y_mag);
        ekf->P = (1.0f - K_mag) * ekf->P;
    }

    /* ================================================================== */
    /*  3. ÖLÇÜM 2 – GPS Course (hız > 2 m/s iken)                       */
    /*     Düşük hızda GPS course güvenilmez, yalnız yüksek hızda kullan  */
    /* ================================================================== */
    if (dataC->gps.speed.calibratedValue > 2.0f &&
        dataC->gps.course.confidence > CONFIDENCE_MIN_VALID) {

        float gps_course = dataC->gps.course.calibratedValue;

        /* Dinamik R: güven düştükçe ölçüm gürültüsü artar */
        float dynamic_R_gps = 1.0f / (WEIGHT_YAW_GPS * dataC->gps.course.confidence + 1e-6f);

        /* İnovasyon */
        float y_gps = wrap_180(gps_course - ekf->state_yaw);

        /* Kalman kazancı */
        float S_gps = ekf->P + dynamic_R_gps;
        float K_gps = ekf->P / (S_gps + 1e-12f);

        /* Düzeltme */
        ekf->state_yaw = wrap_180(ekf->state_yaw + K_gps * y_gps);
        ekf->P = (1.0f - K_gps) * ekf->P;
    }

    /* ================================================================== */
    /*  4. ÇIKTILAR                                                        */
    /* ================================================================== */
    dataC->estimated.yaw.value = ekf->state_yaw;

    /* Yaw güveni: P küçükse güven yüksek */
    dataC->estimated.yaw.confidence = 1.0f / (1.0f + ekf->P);
}
