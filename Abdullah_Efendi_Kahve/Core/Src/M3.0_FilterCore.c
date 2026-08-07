#include "M3.0_FilterCore.h"
#include <string.h>

/* -------------------------------------------------------------------------- */
/*  Yardımcı: DataCenter başlangıç güven skorlarını 1.0f olarak ayarla       */
/* -------------------------------------------------------------------------- */
static void InitConfidences(DataCenter *dc)
{
    dc->acc.x.confidence  = 1.0f; dc->acc.y.confidence  = 1.0f; dc->acc.z.confidence  = 1.0f;
    dc->gyro.x.confidence = 1.0f; dc->gyro.y.confidence = 1.0f; dc->gyro.z.confidence = 1.0f;
    dc->mag.x.confidence  = 1.0f; dc->mag.y.confidence  = 1.0f; dc->mag.z.confidence  = 1.0f;
    dc->baro.press.confidence = 1.0f; dc->baro.temp.confidence = 1.0f;
    dc->gps.x.confidence  = 0.0f; dc->gps.y.confidence  = 0.0f; /* GPS fix bekle */
    dc->gps.z.confidence  = 0.0f; dc->gps.speed.confidence = 0.0f;
    dc->gps.course.confidence = 0.0f;
}

/* ========================================================================== */
/*  Filter_Init                                                               */
/* ========================================================================== */
void Filter_Init(Filter_System_t *sys, DataCenter *dc)
{
    if (sys == NULL || dc == NULL) return;

    /* 1. DataCenter'ı sıfırla */
    memset(dc, 0, sizeof(DataCenter));

    /* 2. Başlangıç güven skorlarını ayarla (memset sonrası 0'dı) */
    InitConfidences(dc);

    /* 3. M1: Sensör Kalibratörü */
    SensorCalib_Init(&sys->calib);

    /* 4. M2: Güven Değerlendirici */
    CE_Init(&sys->conf_eval);

    /* 5. M3: Yönelim Kestirici (Pitch/Roll/Yaw kuaterniyon) */
    M3_Attitude_Init(&sys->ekf_attitude);

    /* 6. M4: Yaw EKF */
    M4_Yaw_Init(&sys->ekf_yaw);

    /* 7. M5: İrtifa EKF (AGL = 0 olarak başlar) */
    M5_Altitude_Init(&sys->ekf_altitude);

    /* 8. M6: Yatay Navigasyon EKF */
    M6_NavXY_Init(&sys->ekf_nav_xy);
    
    /* 9. İstasyon Referansı */
    Station_Init(&sys->station);

    sys->last_update_time_us = 0;
}

/* ========================================================================== */
/*  Filter_Update                                                             */
/*  Sensör sürücüleri rawValue ve UpdateTime alanlarını zaten doldurmuş       */
/*  olmalıdır. Bu fonksiyon kaskad EKF boru hattını çalıştırır.              */
/* ========================================================================== */
void Filter_Update(Filter_System_t *sys, DataCenter *dc, uint32_t curr_time_us)
{
    if (sys == NULL || dc == NULL) return;

    /* Zaman deltası (saniye cinsinden) */
    float dt_seconds = 0.0f;
    if (sys->last_update_time_us != 0) {
        dt_seconds = (float)(curr_time_us - sys->last_update_time_us) / 1000000.0f;
    }
    sys->last_update_time_us = curr_time_us;

    if (dt_seconds <= 0.0f || dt_seconds > 1.0f) return;  /* İlk çağrı veya zaman anomalisi */

    /* --- BATARYA FİLTRESİ (LPF ve HPF) --- */
    static float batt_lpf = 0.0f;
    static float batt_hpf = 0.0f;
    static float last_raw_batt = 0.0f;
    
    float curr_batt = dc->batt.battVolt.rawValue;
    if (batt_lpf == 0.0f && curr_batt > 0.0f) {
        batt_lpf = curr_batt;
        last_raw_batt = curr_batt;
    }
    
    if (curr_batt > 0.0f) {
        /* LPF: Sabit voltaj (Yüzde hesabı için) */
        batt_lpf = 0.9f * batt_lpf + 0.1f * curr_batt;
        /* HPF: Sadece anlık dalgalanmalar (Motor akım çekmesi) */
        batt_hpf = 0.9f * (batt_hpf + curr_batt - last_raw_batt);
        last_raw_batt = curr_batt;
        
        dc->batt.battVolt.calibratedValue = batt_lpf;
        dc->batt.battVolt.filteredValue = batt_hpf;
    }

    /* --- KASKAD BORU HATTI --- */

    /* --- KASKAD BORU HATTI --- */

    /* ADIM 1: (M1) rawValue → calibratedValue (birim çevirimi dahil) */
    SensorCalibApply(&sys->calib, dc);

    /* ADIM 2: (M2) calibratedValue üzerinde güven denetimi */
    CE_Update(&sys->conf_eval, dc, curr_time_us);

    /* ADIM 3: (M3) Yönelim – Pitch, Roll, Yaw (kuaterniyon) */
    M3_Attitude_Update(&sys->ekf_attitude, dc, dt_seconds);
    
    // Montaj/Pcb yamukluklarını çıkar (Set Level ofsetleri)
    SensorCalib_ApplyBoardAlignment(&sys->calib, &dc->estimated.pitch.value, &dc->estimated.roll.value);

    /* ADIM 4: (M4) Yaw düzeltmesi (GPS course + manyetometre) */
    M4_Yaw_Update(&sys->ekf_yaw, dc, dt_seconds);
    
    /* ADIM 5: (M7) Kinematik - Gövde ivmelerini Dünya eksenine çevir (M5 ve M6'dan önce!) */
    Kinematics_BodyToEarth(dc);

    /* ADIM 6: (M5) İrtifa ve dikey hız (AGL) */
    M5_Altitude_Update(&sys->ekf_altitude, dc, &sys->station, dt_seconds);

    /* ADIM 7: (M6) Yatay konum ve hız */
    M6_NavXY_Update(&sys->ekf_nav_xy, dc, &sys->station, dt_seconds);
}
