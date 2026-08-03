#include "M5_EKF_Altitude.h"
#include "Filter_Config.h"
#include <string.h>
#include <math.h>
#include <stddef.h>

/* ========================================================================== */
/*  M5_Altitude_Init – AGL = 0 olarak başlat                                 */
/* ========================================================================== */
void M5_Altitude_Init(M5_EKF_Altitude_t *ekf) {
    /* Başlangıç Durumları: AGL=0, Hız=0, Bias=0 */
    ekf->x[0] = 0.0f;
    ekf->x[1] = 0.0f;
    ekf->x[2] = 0.0f;

    /* Başlangıç Kovaryansı */
    memset(ekf->P, 0, sizeof(ekf->P));
    ekf->P[0] = 10.0f;   /* P(0,0) konum belirsizliği */
    ekf->P[4] = 1.0f;    /* P(1,1) hız belirsizliği   */
    ekf->P[8] = 0.1f;    /* P(2,2) bias belirsizliği  */

    /* Station modülü zemin referansını halleder */
}
/* ========================================================================== */
/*  M5_Altitude_Update                                                        */
/*                                                                            */
/*  3-Durumlu EKF: [irtifa_AGL, dikey_hız, ivme_bias]                        */
/*                                                                            */
/*  TAHMİN:    İvmeölçerden dikey ivme (body→world, Euler açılarıyla)         */
/*  ÖLÇÜM 1:  Barometrik irtifa (AGL'ye çevrilmiş)                           */
/*  ÖLÇÜM 2:  GPS irtifa (zaten AGL – M1'de çevrildi)                        */
/*                                                                            */
/*  GİRİŞ BİRİMLERİ: ivme m/s², pitch/roll derece                            */
/*  ÇIKIŞ: pos_z [m AGL], vel_z [m/s], a_z [m/s²], confidence                */
/* ========================================================================== */
void M5_Altitude_Update(M5_EKF_Altitude_t *ekf, DataCenter *dataC,
                        const Station_Reference_t *station, float dt_seconds) {
    if (dataC == NULL || station == NULL || dt_seconds <= 0.0f) return;

    /* ================================================================== */
    /*  1. DİKEY İVMEYİ AL (M7 Kinematics modülünden gelir)              */
    /* ================================================================== */
    float earth_az = dataC->estimated.earth_a_z.value;

    /* Yerçekimini çıkar → net dikey ivme */
    float net_accel_z = earth_az - GRAVITY_MPS2;

    /* ================================================================== */
    /*  2. TAHMİN ADIMI                                                    */
    /* ================================================================== */
    /* Bias'ı ivmeden çıkar */
    float a_z = net_accel_z - ekf->x[2];

    /* Kinematik: Z = Z + V*dt + 0.5*a*dt² */
    ekf->x[0] += ekf->x[1] * dt_seconds + 0.5f * a_z * dt_seconds * dt_seconds;
    /* V = V + a*dt */
    ekf->x[1] += a_z * dt_seconds;
    /* Bias sabit kalır (x[2] değişmez) */

    /* F Matrisi (3×3 durum geçiş) */
    float F[9] = {
        1.0f, dt_seconds, -0.5f * dt_seconds * dt_seconds,
        0.0f, 1.0f,       -dt_seconds,
        0.0f, 0.0f,        1.0f
    };

    /* Q Matrisi (süreç gürültüsü) */
    float Q[9] = {
        EKF_Q_Z_POS * dt_seconds, 0.0f, 0.0f,
        0.0f, EKF_Q_Z_VEL * dt_seconds, 0.0f,
        0.0f, 0.0f, EKF_Q_Z_ACC_BIAS * dt_seconds
    };

    /* P = F * P * F^T + Q */
    float F_P[9], F_T[9], FPFT[9];
    mat_mult(3, 3, 3, F, ekf->P, F_P);
    mat_trans(3, 3, F, F_T);
    mat_mult(3, 3, 3, F_P, F_T, FPFT);
    mat_add(3, 3, FPFT, Q, ekf->P);

    /* ================================================================== */
    /*  3. ÖLÇÜM 1 – Barometrik İrtifa (AGL)                              */
    /* ================================================================== */
    if (dataC->baro.press.confidence > CONFIDENCE_MIN_VALID && station->baro_ref_set) {
        /* Barometrik MSL irtifa (Standart atmosfer, 101325 sabit alınabilir veya sea_level_pa) */
        float baro_msl = 44330.0f * (1.0f - powf(dataC->baro.press.calibratedValue / 101325.0f, 0.190295f));

        /* MSL → AGL (Station'dan gelen referans irtifa) */
        float z_meas = baro_msl - station->ref_baro_msl;

        /* Dinamik R */
        float R_baro = 1.0f / (WEIGHT_ALT_BARO * dataC->baro.press.confidence + 1e-6f);

        /* H = [1, 0, 0] */
        float y = z_meas - ekf->x[0];

        /* S = H*P*H^T + R = P[0][0] + R */
        float S = ekf->P[0] + R_baro;

        /* FDI: Mahalanobis Uzaklığı Testi */
        float mahalanobis_sq = (y * y) / (S + 1e-12f);
        if (mahalanobis_sq > EKF_INNOVATION_GATE_3SIGMA) {
            dataC->baro.press.confidence = 0.0f;
        } else {
            /* K = P*H^T / S (3×1) */
            float K[3];
            K[0] = ekf->P[0] / (S + 1e-12f);
            K[1] = ekf->P[3] / (S + 1e-12f);
            K[2] = ekf->P[6] / (S + 1e-12f);

            /* x = x + K*y */
            ekf->x[0] += K[0] * y;
            ekf->x[1] += K[1] * y;
            ekf->x[2] += K[2] * y;

            /* P = (I - K*H) * P */
            float I_KH[9] = {
                1.0f - K[0], 0.0f, 0.0f,
                0.0f - K[1], 1.0f, 0.0f,
                0.0f - K[2], 0.0f, 1.0f
            };
            float P_new[9];
            mat_mult(3, 3, 3, I_KH, ekf->P, P_new);
            memcpy(ekf->P, P_new, sizeof(P_new));
        }
    }

    /* ================================================================== */
    /*  4. ÖLÇÜM 2 – GPS İrtifa (zaten AGL – M1'de çevrildi)              */
    /* ================================================================== */
    if (dataC->gps.z.confidence > CONFIDENCE_MIN_VALID && station->gps_ref_set) {
        float z_gps = dataC->gps.z.calibratedValue - station->ref_alt_msl;  /* AGL (Station) */

        float R_gps = 1.0f / (WEIGHT_ALT_BARO * dataC->gps.z.confidence + 1e-6f);

        float y = z_gps - ekf->x[0];
        float S = ekf->P[0] + R_gps;

        /* FDI: Mahalanobis Uzaklığı Testi */
        float mahalanobis_sq = (y * y) / (S + 1e-12f);
        if (mahalanobis_sq > EKF_INNOVATION_GATE_3SIGMA) {
            dataC->gps.z.confidence = 0.0f;
        } else {
            float K[3];
            K[0] = ekf->P[0] / (S + 1e-12f);
            K[1] = ekf->P[3] / (S + 1e-12f);
            K[2] = ekf->P[6] / (S + 1e-12f);

            ekf->x[0] += K[0] * y;
            ekf->x[1] += K[1] * y;
            ekf->x[2] += K[2] * y;

            float I_KH[9] = {
                1.0f - K[0], 0.0f, 0.0f,
                0.0f - K[1], 1.0f, 0.0f,
                0.0f - K[2], 0.0f, 1.0f
            };
            float P_new[9];
            mat_mult(3, 3, 3, I_KH, ekf->P, P_new);
            memcpy(ekf->P, P_new, sizeof(P_new));
        }
    }

    /* ================================================================== */
    /*  5. ÇIKTILAR                                                        */
    /* ================================================================== */
    dataC->estimated.pos_z.value = ekf->x[0];        /* AGL irtifa [m]    */
    dataC->estimated.vel_z.value = ekf->x[1];        /* Dikey hız [m/s]   */
    dataC->estimated.a_z.value   = net_accel_z;       /* Dikey ivme [m/s²] */

    /* Güven: kovaryans köşegeninden türet */
    dataC->estimated.pos_z.confidence = 1.0f / (1.0f + ekf->P[0]);
    dataC->estimated.vel_z.confidence = 1.0f / (1.0f + ekf->P[4]);
    dataC->estimated.a_z.confidence   = 1.0f / (1.0f + ekf->P[8]);
}
