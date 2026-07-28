#include "M1_Sensor_Calibrator.h"
#include <math.h>

/* ========================================================================== */
/*  SensorCalib_Init – Kalibrasyon Sabitlerini Yükle                          */
/* ========================================================================== */
void SensorCalib_Init(SensorCalib_t *calib) {
    if (calib == NULL) return;

    /* İvmeölçer (rawValue birimi: g) */
    calib->acc_x_bias = CALIB_ACC_X_BIAS;
    calib->acc_y_bias = CALIB_ACC_Y_BIAS;
    calib->acc_z_bias = CALIB_ACC_Z_BIAS;
    calib->acc_x_scale = CALIB_ACC_X_SCALE;
    calib->acc_y_scale = CALIB_ACC_Y_SCALE;
    calib->acc_z_scale = CALIB_ACC_Z_SCALE;
    calib->acc_thermal_drift = CALIB_ACC_THERMAL_DRIFT;

    /* Jiroskop (rawValue birimi: dps) */
    calib->gyro_x_bias = CALIB_GYRO_X_BIAS;
    calib->gyro_y_bias = CALIB_GYRO_Y_BIAS;
    calib->gyro_z_bias = CALIB_GYRO_Z_BIAS;
    calib->gyro_x_scale = CALIB_GYRO_X_SCALE;
    calib->gyro_y_scale = CALIB_GYRO_Y_SCALE;
    calib->gyro_z_scale = CALIB_GYRO_Z_SCALE;

    /* Manyetometre (rawValue birimi: µT) */
    calib->mag_x_bias = CALIB_MAG_X_BIAS;
    calib->mag_y_bias = CALIB_MAG_Y_BIAS;
    calib->mag_z_bias = CALIB_MAG_Z_BIAS;
    calib->mag_x_scale = CALIB_MAG_X_SCALE;
    calib->mag_y_scale = CALIB_MAG_Y_SCALE;
    calib->mag_z_scale = CALIB_MAG_Z_SCALE;

    /* Barometre */
    calib->baro_press_bias = CALIB_BARO_PRESS_BIAS;
    calib->baro_press_scale = CALIB_BARO_PRESS_SCALE;
    calib->baro_temp_bias = CALIB_BARO_TEMP_BIAS;
    calib->baro_temp_scale = CALIB_BARO_TEMP_SCALE;

    /* Çevresel ve Rampa Sabitleri */
    calib->sea_level_pa = FILTER_SEA_LEVEL_PA_DEFAULT;
    calib->temp_ref_c = FILTER_TEMP_REF_DEFAULT;
    calib->ref_ground_altitude_m = 0.0f;
}

/* ========================================================================== */
/*  SensorCalibApply – rawValue → calibratedValue (SI birim çevirimi dahil)   */
/*                                                                            */
/*  BİRİM ZİNCİRİ:                                                           */
/*    İvme:  rawValue [g]   → calibratedValue [m/s²]                          */
/*    Gyro:  rawValue [dps] → calibratedValue [rad/s]                         */
/*    Mag:   rawValue [µT]  → calibratedValue [µT]   (değişmiyor)             */
/*    Baro:  rawValue [Pa]  → calibratedValue [Pa]   (değişmiyor)             */
/*    GPS:   rawValue [°,m] → calibratedValue [°,m]  (AGL çevrimi)            */
/* ========================================================================== */
void SensorCalibApply(const SensorCalib_t *calib, DataCenter *dataC) {
    if (calib == NULL || dataC == NULL) return;

    /* ---- 1. İvmeölçer: g → m/s² ---- */
    float ax_cal_g = (dataC->acc.x.rawValue - calib->acc_x_bias) / calib->acc_x_scale;
    float ay_cal_g = (dataC->acc.y.rawValue - calib->acc_y_bias) / calib->acc_y_scale;
    float az_cal_g = (dataC->acc.z.rawValue - calib->acc_z_bias) / calib->acc_z_scale;

    dataC->acc.x.calibratedValue = ax_cal_g * GRAVITY_MPS2;  /* g → m/s² */
    dataC->acc.y.calibratedValue = ay_cal_g * GRAVITY_MPS2;
    dataC->acc.z.calibratedValue = az_cal_g * GRAVITY_MPS2;

    /* ---- 2. Jiroskop: dps → rad/s ---- */
    float gx_cal_dps = (dataC->gyro.x.rawValue - calib->gyro_x_bias) / calib->gyro_x_scale;
    float gy_cal_dps = (dataC->gyro.y.rawValue - calib->gyro_y_bias) / calib->gyro_y_scale;
    float gz_cal_dps = (dataC->gyro.z.rawValue - calib->gyro_z_bias) / calib->gyro_z_scale;

    dataC->gyro.x.calibratedValue = gx_cal_dps * DEG2RAD;  /* dps → rad/s */
    dataC->gyro.y.calibratedValue = gy_cal_dps * DEG2RAD;
    dataC->gyro.z.calibratedValue = gz_cal_dps * DEG2RAD;

    /* ---- 3. Manyetometre: µT → µT (birim aynı) ---- */
    dataC->mag.x.calibratedValue = (dataC->mag.x.rawValue - calib->mag_x_bias) / calib->mag_x_scale;
    dataC->mag.y.calibratedValue = (dataC->mag.y.rawValue - calib->mag_y_bias) / calib->mag_y_scale;
    dataC->mag.z.calibratedValue = (dataC->mag.z.rawValue - calib->mag_z_bias) / calib->mag_z_scale;

    /* ---- 4. Barometre: Pa → Pa (birim aynı) ---- */
    dataC->baro.press.calibratedValue = (dataC->baro.press.rawValue - calib->baro_press_bias) / calib->baro_press_scale;
    dataC->baro.temp.calibratedValue  = (dataC->baro.temp.rawValue  - calib->baro_temp_bias)  / calib->baro_temp_scale;

    /* ---- 5. GPS: Fabrika kalibreli, AGL çevrimi ---- */
    dataC->gps.x.calibratedValue     = dataC->gps.x.rawValue;
    dataC->gps.y.calibratedValue     = dataC->gps.y.rawValue;
    dataC->gps.z.calibratedValue     = dataC->gps.z.rawValue - calib->ref_ground_altitude_m;  /* MSL → AGL */
    dataC->gps.speed.calibratedValue = dataC->gps.speed.rawValue;
    dataC->gps.course.calibratedValue = dataC->gps.course.rawValue;
}
