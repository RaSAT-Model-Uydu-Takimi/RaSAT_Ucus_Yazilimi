#include "M6_EKF_NavXY.h"
#include "Filter_Config.h"
#include <string.h>
#include <math.h>

/* ========================================================================== */
/*  M6_NavXY_Init                                                            */
/* ========================================================================== */
void M6_NavXY_Init(M6_EKF_NavXY_t *ekf) {
    ekf->x[0] = 0.0f;   /* X Konum [m] */
    ekf->x[1] = 0.0f;   /* Y Konum [m] */
    ekf->x[2] = 0.0f;   /* X Hız [m/s] */
    ekf->x[3] = 0.0f;   /* Y Hız [m/s] */

    memset(ekf->P, 0, sizeof(ekf->P));
    ekf->P[0]  = 10.0f;  /* P(X, X)   */
    ekf->P[5]  = 10.0f;  /* P(Y, Y)   */
    ekf->P[10] = 1.0f;   /* P(Vx, Vx) */
    ekf->P[15] = 1.0f;   /* P(Vy, Vy) */

    /* Referans istasyon modülünden alınır */

/* ========================================================================== */
/*  M6_NavXY_Update                                                           */
/*                                                                            */
/*  4-Durumlu EKF: [posX, posY, velX, velY]                                  */
/*                                                                            */
/*  TAHMİN:    İvmeölçer body→NED dönüşümü (Euler açılarıyla, YAW DAHİL)    */
/*  ÖLÇÜM:     GPS konum (derece → metre çevrimi, double hassasiyetle)       */
/*                                                                            */
/*  ÖNEMLİ: M4'ün düzelttiği yaw dahil Euler açılarını kullanır.             */
/*  GPS koordinat aritmetiği double ile yapılır (float ile ~0.5m hata olur). */
/* ========================================================================== */
void M6_NavXY_Update(M6_EKF_NavXY_t *ekf, DataCenter *dataC, const Station_Reference_t *station, float dt_seconds) {
    if (dataC == NULL || station == NULL || dt_seconds <= 0.0f) return;

    /* ================================================================== */
    /*  1. YATAY İVMELERİ AL (M7 Kinematics Modülünden)                    */
    /* ================================================================== */
    float earth_ax = dataC->estimated.earth_a_x.value;
    float earth_ay = dataC->estimated.earth_a_y.value;

    /* ================================================================== */
    /*  2. TAHMİN ADIMI                                                    */
    /* ================================================================== */
    ekf->x[0] += ekf->x[2] * dt_seconds + 0.5f * earth_ax * dt_seconds * dt_seconds;
    ekf->x[1] += ekf->x[3] * dt_seconds + 0.5f * earth_ay * dt_seconds * dt_seconds;
    ekf->x[2] += earth_ax * dt_seconds;
    ekf->x[3] += earth_ay * dt_seconds;

    /* F Matrisi (4×4) */
    float F[16] = {
        1.0f, 0.0f, dt_seconds, 0.0f,
        0.0f, 1.0f, 0.0f,       dt_seconds,
        0.0f, 0.0f, 1.0f,       0.0f,
        0.0f, 0.0f, 0.0f,       1.0f
    };

    /* Q Matrisi (süreç gürültüsü) */
    float Q[16] = {
        EKF_Q_XY_POS, 0.0f, 0.0f, 0.0f,
        0.0f, EKF_Q_XY_POS, 0.0f, 0.0f,
        0.0f, 0.0f, EKF_Q_XY_VEL, 0.0f,
        0.0f, 0.0f, 0.0f, EKF_Q_XY_VEL
    };

    /* P = F * P * F^T + Q */
    float F_P[16], F_T[16], FPFT[16];
    mat_mult(4, 4, 4, F, ekf->P, F_P);
    mat_trans(4, 4, F, F_T);
    mat_mult(4, 4, 4, F_P, F_T, FPFT);
    mat_add(4, 4, FPFT, Q, ekf->P);

    /* ================================================================== */
    /*  3. ÖLÇÜM – GPS Konum (double hassasiyetle derece → metre)          */
    /* ================================================================== */
    if (dataC->gps.x.confidence > CONFIDENCE_MIN_VALID && station->gps_ref_set) {

        /* GPS koordinatları double – hassasiyeti korumak için double ile oku */
        double gps_lat = dataC->gps.x.calibratedValue;
        double gps_lon = dataC->gps.y.calibratedValue;

        /* Derece → Metre dönüşümü (double aritmetik, sonra float'a çevir) */
        float gps_x_m = (float)((gps_lat - station->ref_lat) * 111320.0);
        float gps_y_m = (float)((gps_lon - station->ref_lon) * 111320.0
                        * cos(station->ref_lat * 0.01745329251));

        /* Dinamik R */
        float R_pos = 1.0f / (WEIGHT_XY_GPS * dataC->gps.x.confidence + 1e-6f);

        /* İnovasyon: y = z - H*x */
        float y[2];
        y[0] = gps_x_m - ekf->x[0];
        y[1] = gps_y_m - ekf->x[1];

        /* S = H * P * H^T + R (2×2) */
        float S[4] = {
            ekf->P[0]  + R_pos, ekf->P[1],
            ekf->P[4],          ekf->P[5] + R_pos
        };

        /* S^-1 (2×2 ters alma) */
        float det = S[0] * S[3] - S[1] * S[2];
        if (fabs(det) > 1e-6f) {
            float S_inv[4];
            S_inv[0] =  S[3] / det;
            S_inv[1] = -S[1] / det;
            S_inv[2] = -S[2] / det;
            S_inv[3] =  S[0] / det;

            /* K = P * H^T * S^-1 (4×2) */
            float K[8];
            for (int i = 0; i < 4; i++) {
                K[i * 2 + 0] = ekf->P[i * 4 + 0] * S_inv[0] + ekf->P[i * 4 + 1] * S_inv[2];
                K[i * 2 + 1] = ekf->P[i * 4 + 0] * S_inv[1] + ekf->P[i * 4 + 1] * S_inv[3];
            }

            /* x = x + K*y */
            for (int i = 0; i < 4; i++) {
                ekf->x[i] += K[i * 2 + 0] * y[0] + K[i * 2 + 1] * y[1];
            }

            /* P = (I - K*H) * P */
            float I_KH[16];
            memset(I_KH, 0, sizeof(I_KH));
            for (int i = 0; i < 4; i++) {
                I_KH[i * 4 + i] = 1.0f;
                I_KH[i * 4 + 0] -= K[i * 2 + 0];
                I_KH[i * 4 + 1] -= K[i * 2 + 1];
            }

            float P_new[16];
            mat_mult(4, 4, 4, I_KH, ekf->P, P_new);
            memcpy(ekf->P, P_new, sizeof(P_new));
        }
    }

    /* ================================================================== */
    /*  4. ÇIKTILAR                                                        */
    /* ================================================================== */
    dataC->estimated.pos_x.value = ekf->x[0];
    dataC->estimated.pos_y.value = ekf->x[1];
    dataC->estimated.vel_x.value = ekf->x[2];
    dataC->estimated.vel_y.value = ekf->x[3];
    dataC->estimated.a_x.value   = earth_ax;
    dataC->estimated.a_y.value   = earth_ay;

    dataC->estimated.pos_x.confidence = 1.0f / (1.0f + ekf->P[0]);
    dataC->estimated.pos_y.confidence = 1.0f / (1.0f + ekf->P[5]);
    dataC->estimated.vel_x.confidence = 1.0f / (1.0f + ekf->P[10]);
    dataC->estimated.vel_y.confidence = 1.0f / (1.0f + ekf->P[15]);
    dataC->estimated.a_x.confidence   = dataC->estimated.pos_x.confidence;
    dataC->estimated.a_y.confidence   = dataC->estimated.pos_y.confidence;
}
