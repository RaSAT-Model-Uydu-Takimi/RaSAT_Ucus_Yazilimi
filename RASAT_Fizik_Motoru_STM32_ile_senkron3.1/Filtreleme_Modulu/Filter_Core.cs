using System;

namespace FilterModule
{
    public struct Filter_System_t
    {
        public DataCenter dataC;
        public SensorCalib_t calib;
        public Confidence_Evaluator_t conf_eval;
        public M4_EKF_Yaw_t ekf_yaw;
        public M5_EKF_Altitude_t ekf_altitude;
        public M6_EKF_NavXY_t ekf_nav_xy;
        public Station_Reference_t station;
        public uint last_update_time_us;
    }

    public static class Filter_Core
    {
        /* --- BATARYA FİLTRESİ İÇİN STATİK DEĞİŞKENLER --- */
        private static float batt_lpf = 0.0f;
        private static float batt_hpf = 0.0f;
        private static float last_raw_batt = 0.0f;

        /* -------------------------------------------------------------------------- */
        /*  Yardımcı: DataCenter başlangıç güven skorlarını 1.0f olarak ayarla       */
        /* -------------------------------------------------------------------------- */
        private static void InitConfidences(ref DataCenter dc)
        {
            dc.acc.x.confidence  = 1.0f; dc.acc.y.confidence  = 1.0f; dc.acc.z.confidence  = 1.0f;
            dc.gyro.x.confidence = 1.0f; dc.gyro.y.confidence = 1.0f; dc.gyro.z.confidence = 1.0f;
            dc.mag.x.confidence  = 1.0f; dc.mag.y.confidence  = 1.0f; dc.mag.z.confidence  = 1.0f;
            dc.baro.press.confidence = 1.0f; dc.baro.temp.confidence = 1.0f;
            dc.gps.x.confidence  = 0.0f; dc.gps.y.confidence  = 0.0f; /* GPS fix bekle */
            dc.gps.z.confidence  = 0.0f; dc.gps.speed.confidence = 0.0f;
            dc.gps.course.confidence = 0.0f;
        }

        /* ========================================================================== */
        /*  Filter_Init                                                               */
        /* ========================================================================== */
        public static void Filter_Init(ref Filter_System_t sys)
        {
            /* 1. DataCenter'ı sıfırla (C#'ta struct referansı atanarak yapılabilir, ancak alanları sıfırlanmış yeni bir nesne atamak en temizidir) */
            sys.dataC = new DataCenter();

            /* 2. Başlangıç güven skorlarını ayarla */
            InitConfidences(ref sys.dataC);

            /* 3. M1: Sensör Kalibratörü */
            sys.calib = new SensorCalib_t();
            M1_Sensor_Calibrator.SensorCalib_Init(ref sys.calib);

            /* 4. M2: Güven Değerlendirici */
            sys.conf_eval = new Confidence_Evaluator_t();
            M2_Confidence_Evaluator.CE_Init(ref sys.conf_eval);

            /* 5. M3: Yönelim Kestirici (Pitch/Roll/Yaw kuaterniyon) */
            M3_EKF_Attitude.M3_Attitude_Init(ref sys.dataC);

            /* 6. M4: Yaw EKF */
            sys.ekf_yaw = new M4_EKF_Yaw_t();
            M4_EKF_Yaw.M4_Yaw_Init(ref sys.ekf_yaw);

            /* 7. M5: İrtifa EKF (AGL = 0 olarak başlar) */
            sys.ekf_altitude = new M5_EKF_Altitude_t();
            M5_EKF_Altitude.M5_Altitude_Init(ref sys.ekf_altitude);

            /* 8. M6: Yatay Navigasyon EKF */
            sys.ekf_nav_xy = new M6_EKF_NavXY_t();
            M6_EKF_NavXY.M6_NavXY_Init(ref sys.ekf_nav_xy);
            
            /* 9. İstasyon Referansı */
            sys.station = new Station_Reference_t();
            Station_Reference.Station_Init(ref sys.station);

            sys.last_update_time_us = 0;
            
            batt_lpf = 0.0f;
            batt_hpf = 0.0f;
            last_raw_batt = 0.0f;
        }

        /* ========================================================================== */
        /*  Filter_Update                                                             */
        /*  Sensör sürücüleri rawValue ve UpdateTime alanlarını zaten doldurmuş       */
        /*  olmalıdır. Bu fonksiyon kaskad EKF boru hattını çalıştırır.              */
        /* ========================================================================== */
        public static void Filter_Update(ref Filter_System_t sys, uint curr_time_us)
        {
            /* Zaman deltası (saniye cinsinden) */
            float dt_seconds = 0.0f;
            if (sys.last_update_time_us != 0)
            {
                dt_seconds = (float)(curr_time_us - sys.last_update_time_us) / 1000000.0f;
            }
            sys.last_update_time_us = curr_time_us;

            if (dt_seconds <= 0.0f || dt_seconds > 1.0f) return;  /* İlk çağrı veya zaman anomalisi */

            /* --- BATARYA FİLTRESİ (LPF ve HPF) --- */
            float curr_batt = sys.dataC.batt.battVolt.rawValue;
            if (batt_lpf == 0.0f && curr_batt > 0.0f)
            {
                batt_lpf = curr_batt;
                last_raw_batt = curr_batt;
            }
            
            if (curr_batt > 0.0f)
            {
                /* LPF: Sabit voltaj (Yüzde hesabı için) */
                batt_lpf = 0.9f * batt_lpf + 0.1f * curr_batt;
                /* HPF: Sadece anlık dalgalanmalar (Motor akım çekmesi) */
                batt_hpf = 0.9f * (batt_hpf + curr_batt - last_raw_batt);
                last_raw_batt = curr_batt;
                
                sys.dataC.batt.battVolt.calibratedValue = batt_lpf;
                sys.dataC.batt.battVolt.filteredValue = batt_hpf;
            }

            /* --- KASKAD BORU HATTI --- */

            /* ADIM 1: (M1) rawValue → calibratedValue (birim çevirimi dahil) */
            M1_Sensor_Calibrator.SensorCalibApply(ref sys.calib, ref sys.dataC);

            /* ADIM 2: (M2) calibratedValue üzerinde güven denetimi */
            M2_Confidence_Evaluator.CE_Update(ref sys.conf_eval, ref sys.dataC, curr_time_us);

            /* ADIM 3: (M3) Yönelim – Pitch, Roll, Yaw (kuaterniyon) */
            M3_EKF_Attitude.M3_Attitude_Update(ref sys.dataC, dt_seconds);

            /* ADIM 4: (M4) Yaw düzeltmesi (GPS course + manyetometre) */
            M4_EKF_Yaw.M4_Yaw_Update(ref sys.ekf_yaw, ref sys.dataC, dt_seconds);
            
            /* ADIM 5: (M7) Kinematik - Gövde ivmelerini Dünya eksenine çevir (M5 ve M6'dan önce!) */
            M7_Kinematics.Kinematics_BodyToEarth(ref sys.dataC);

            /* ADIM 6: (M5) İrtifa ve dikey hız (AGL) */
            M5_EKF_Altitude.M5_Altitude_Update(ref sys.ekf_altitude, ref sys.dataC, ref sys.station, dt_seconds);

            /* ADIM 7: (M6) Yatay konum ve hız */
            M6_EKF_NavXY.M6_NavXY_Update(ref sys.ekf_nav_xy, ref sys.dataC, ref sys.station, dt_seconds);
        }
    }
}
