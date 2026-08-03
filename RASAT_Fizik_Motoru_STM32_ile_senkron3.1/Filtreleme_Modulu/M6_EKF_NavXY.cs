using System;

namespace FilterModule
{
    public struct M6_EKF_NavXY_t
    {
        // 4 Durumlu Durum Vektörü (State Vector x): [X_Konum, Y_Konum, X_Hız, Y_Hız]
        public float[] x; 
        
        // 4x4 Hata Kovaryans Matrisi (P)
        public float[] P; 
    }

    public static class M6_EKF_NavXY
    {
        /* ========================================================================== */
        /*  M6_NavXY_Init                                                            */
        /* ========================================================================== */
        public static void M6_NavXY_Init(ref M6_EKF_NavXY_t ekf)
        {
            if (ekf.x == null) ekf.x = new float[4];
            if (ekf.P == null) ekf.P = new float[16];

            ekf.x[0] = 0.0f;   /* X Konum [m] */
            ekf.x[1] = 0.0f;   /* Y Konum [m] */
            ekf.x[2] = 0.0f;   /* X Hız [m/s] */
            ekf.x[3] = 0.0f;   /* Y Hız [m/s] */

            for (int i = 0; i < 16; i++) ekf.P[i] = 0.0f;
            ekf.P[0]  = 10.0f;  /* P(X, X)   */
            ekf.P[5]  = 10.0f;  /* P(Y, Y)   */
            ekf.P[10] = 1.0f;   /* P(Vx, Vx) */
            ekf.P[15] = 1.0f;   /* P(Vy, Vy) */

            /* Referans istasyon modülünden alınır */
        }

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
        public static void M6_NavXY_Update(ref M6_EKF_NavXY_t ekf, ref DataCenter dataC, ref Station_Reference_t station, float dt_seconds)
        {
            if (dt_seconds <= 0.0f) return;

            /* ================================================================== */
            /*  1. YATAY İVMELERİ AL (M7 Kinematics Modülünden)                    */
            /* ================================================================== */
            float earth_ax = dataC.estimated.earth_a_x.value;
            float earth_ay = dataC.estimated.earth_a_y.value;

            /* ================================================================== */
            /*  2. TAHMİN ADIMI                                                    */
            /* ================================================================== */
            ekf.x[0] += ekf.x[2] * dt_seconds + 0.5f * earth_ax * dt_seconds * dt_seconds;
            ekf.x[1] += ekf.x[3] * dt_seconds + 0.5f * earth_ay * dt_seconds * dt_seconds;
            ekf.x[2] += earth_ax * dt_seconds;
            ekf.x[3] += earth_ay * dt_seconds;

            /* F Matrisi (4×4) */
            float[] F = {
                1.0f, 0.0f, dt_seconds, 0.0f,
                0.0f, 1.0f, 0.0f,       dt_seconds,
                0.0f, 0.0f, 1.0f,       0.0f,
                0.0f, 0.0f, 0.0f,       1.0f
            };

            /* Q Matrisi (süreç gürültüsü) */
            float[] Q = {
                Filter_Config.EKF_Q_XY_POS, 0.0f, 0.0f, 0.0f,
                0.0f, Filter_Config.EKF_Q_XY_POS, 0.0f, 0.0f,
                0.0f, 0.0f, Filter_Config.EKF_Q_XY_VEL, 0.0f,
                0.0f, 0.0f, 0.0f, Filter_Config.EKF_Q_XY_VEL
            };

            /* P = F * P * F^T + Q */
            float[] F_P = new float[16];
            float[] F_T = new float[16];
            float[] FPFT = new float[16];
            
            M0_Matrix_Operations.mat_mult(4, 4, 4, F, ekf.P, F_P);
            M0_Matrix_Operations.mat_trans(4, 4, F, F_T);
            M0_Matrix_Operations.mat_mult(4, 4, 4, F_P, F_T, FPFT);
            M0_Matrix_Operations.mat_add(4, 4, FPFT, Q, ekf.P);

            /* ================================================================== */
            /*  3. ÖLÇÜM – GPS Konum (double hassasiyetle derece → metre)          */
            /* ================================================================== */
            if (dataC.gps.x.confidence > Filter_Config.CONFIDENCE_MIN_VALID && station.gps_ref_set != 0)
            {
                /* GPS koordinatları double – hassasiyeti korumak için double ile oku */
                double gps_lat = dataC.gps.x.calibratedValue;
                double gps_lon = dataC.gps.y.calibratedValue;

                /* Derece → Metre dönüşümü (double aritmetik, sonra float'a çevir) */
                float gps_x_m = (float)((gps_lat - station.ref_lat) * 111320.0);
                float gps_y_m = (float)((gps_lon - station.ref_lon) * 111320.0
                                * Math.Cos(station.ref_lat * 0.01745329251));

                /* Dinamik R */
                float R_pos = 1.0f / (Filter_Config.WEIGHT_XY_GPS * dataC.gps.x.confidence + 1e-6f);

                /* İnovasyon: y = z - H*x */
                float[] y = new float[2];
                y[0] = gps_x_m - ekf.x[0];
                y[1] = gps_y_m - ekf.x[1];

                /* S = H * P * H^T + R (2×2) */
                float[] S = {
                    ekf.P[0]  + R_pos, ekf.P[1],
                    ekf.P[4],          ekf.P[5] + R_pos
                };

                /* S^-1 (2×2 ters alma) */
                float det = S[0] * S[3] - S[1] * S[2];
                if (Math.Abs(det) > 1e-6f)
                {
                    float[] S_inv = new float[4];
                    S_inv[0] =  S[3] / det;
                    S_inv[1] = -S[1] / det;
                    S_inv[2] = -S[2] / det;
                    S_inv[3] =  S[0] / det;

                    /* K = P * H^T * S^-1 (4×2) */
                    float[] K = new float[8];
                    for (int i = 0; i < 4; i++)
                    {
                        K[i * 2 + 0] = ekf.P[i * 4 + 0] * S_inv[0] + ekf.P[i * 4 + 1] * S_inv[2];
                        K[i * 2 + 1] = ekf.P[i * 4 + 0] * S_inv[1] + ekf.P[i * 4 + 1] * S_inv[3];
                    }

                    /* x = x + K*y */
                    for (int i = 0; i < 4; i++)
                    {
                        ekf.x[i] += K[i * 2 + 0] * y[0] + K[i * 2 + 1] * y[1];
                    }

                    /* P = (I - K*H) * P */
                    float[] I_KH = new float[16];
                    for (int i = 0; i < 4; i++)
                    {
                        I_KH[i * 4 + i] = 1.0f;
                        I_KH[i * 4 + 0] -= K[i * 2 + 0];
                        I_KH[i * 4 + 1] -= K[i * 2 + 1];
                    }

                    float[] P_new = new float[16];
                    M0_Matrix_Operations.mat_mult(4, 4, 4, I_KH, ekf.P, P_new);
                    Array.Copy(P_new, ekf.P, 16);
                }
            }

            /* ================================================================== */
            /*  4. ÇIKTILAR                                                        */
            /* ================================================================== */
            dataC.estimated.pos_x.value = ekf.x[0];
            dataC.estimated.pos_y.value = ekf.x[1];
            dataC.estimated.vel_x.value = ekf.x[2];
            dataC.estimated.vel_y.value = ekf.x[3];
            dataC.estimated.a_x.value   = earth_ax;
            dataC.estimated.a_y.value   = earth_ay;

            dataC.estimated.pos_x.confidence = 1.0f / (1.0f + ekf.P[0]);
            dataC.estimated.pos_y.confidence = 1.0f / (1.0f + ekf.P[5]);
            dataC.estimated.vel_x.confidence = 1.0f / (1.0f + ekf.P[10]);
            dataC.estimated.vel_y.confidence = 1.0f / (1.0f + ekf.P[15]);
            dataC.estimated.a_x.confidence   = dataC.estimated.pos_x.confidence;
            dataC.estimated.a_y.confidence   = dataC.estimated.pos_y.confidence;
        }
    }
}



