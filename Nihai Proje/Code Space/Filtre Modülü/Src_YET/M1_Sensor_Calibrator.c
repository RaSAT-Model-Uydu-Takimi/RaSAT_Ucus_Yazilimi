#include "M1_Sensor_Calibrator.h"
#include <math.h>

//****DIKKAT : gps eksikleri Sensor_Verileri_t düzelince düzelt

/*
*Adımlar:
*
*/
void SensorCalib_Init(SensorCalib_t *calib){
    if(calib == NULL) return ;
   
    // İvmeölçer Kalibrasyon Yüklemesi
    calib->acc_x_bias = CALIB_ACC_X_BIAS; calib->acc_y_bias = CALIB_ACC_Y_BIAS; calib->acc_z_bias = CALIB_ACC_Z_BIAS;
    calib->acc_x_scale = CALIB_ACC_X_SCALE; calib->acc_y_scale = CALIB_ACC_Y_SCALE; calib->acc_z_scale = CALIB_ACC_Z_SCALE;
    calib->acc_thermal_drift = CALIB_ACC_THERMAL_DRIFT;

    // Jiroskop Kalibrasyon Yüklemesi
    calib->gyro_x_bias = CALIB_GYRO_X_BIAS; calib->gyro_y_bias = CALIB_GYRO_Y_BIAS; calib->gyro_z_bias = CALIB_GYRO_Z_BIAS;
    calib->gyro_x_scale = CALIB_GYRO_X_SCALE; calib->gyro_y_scale = CALIB_GYRO_Y_SCALE; calib->gyro_z_scale = CALIB_GYRO_Z_SCALE;
    // Manyetometre Kalibrasyon Yüklemesi
    calib->mag_x_bias = CALIB_MAG_X_BIAS; calib->mag_y_bias = CALIB_MAG_Y_BIAS; calib->mag_z_bias = CALIB_MAG_Z_BIAS;
    calib->mag_x_scale = CALIB_MAG_X_SCALE; calib->mag_y_scale = CALIB_MAG_Y_SCALE; calib->mag_z_scale = CALIB_MAG_Z_SCALE;
    // Barometre Ortam Referansları
    calib->baro_press_bias = CALIB_BARO_PRESS_BIAS; calib->baro_press_scale = CALIB_BARO_PRESS_SCALE;
    calib->baro_temp_bias = CALIB_BARO_TEMP_BIAS; calib->baro_temp_scale = CALIB_BARO_TEMP_SCALE; 
    // Çevresel ve Rampa Sabitleri
    calib->sea_level_pa = FILTER_SEA_LEVEL_PA_DEFAULT;
    calib->temp_ref_c = FILTER_TEMP_REF_DEFAULT;
    calib->ref_ground_altitude_m = 0.0f; // İlk 100 örnekte (rampada) hesaplanacak
}




 
 //Görevi: DataCenter içindeki rawValue (ham) değerleri kalibrasyon 
 //denklemlerinden geçirerek calibratedValue alanlarına yazar.

void SensorCalibApply(const SensorCalib_t *calib, DataCenter *dataC) {
    if (calib == NULL || dataC == NULL) return;

    // 1. İvmeölçer Kalibrasyonu
    dataC->acc.x.calibratedValue = (dataC->acc.x.rawValue - calib->acc_x_bias) / calib->acc_x_scale;
    dataC->acc.y.calibratedValue = (dataC->acc.y.rawValue - calib->acc_y_bias) / calib->acc_y_scale;
    dataC->acc.z.calibratedValue = (dataC->acc.z.rawValue - calib->acc_z_bias) / calib->acc_z_scale;

    // 2. Jiroskop Kalibrasyonu
    dataC->gyro.x.calibratedValue = (dataC->gyro.x.rawValue - calib->gyro_x_bias) / calib->gyro_x_scale;
    dataC->gyro.y.calibratedValue = (dataC->gyro.y.rawValue - calib->gyro_y_bias) / calib->gyro_y_scale;
    dataC->gyro.z.calibratedValue = (dataC->gyro.z.rawValue - calib->gyro_z_bias) / calib->gyro_z_scale;

    // 3. Manyetometre Kalibrasyonu
    dataC->mag.x.calibratedValue = (dataC->mag.x.rawValue - calib->mag_x_bias) / calib->mag_x_scale;
    dataC->mag.y.calibratedValue = (dataC->mag.y.rawValue - calib->mag_y_bias) / calib->mag_y_scale;
    dataC->mag.z.calibratedValue = (dataC->mag.z.rawValue - calib->mag_z_bias) / calib->mag_z_scale;

    // 4. Barometre Kalibrasyonu
    dataC->baro.press.calibratedValue = (dataC->baro.press.rawValue - calib->baro_press_bias) / calib->baro_press_scale;
    dataC->baro.temp.calibratedValue = (dataC->baro.temp.rawValue - calib->baro_temp_bias) / calib->baro_temp_scale;

    // 5. GPS Verileri (GPS çipi genelde fabrika çıkışlı kendi kendini kalibre eder)
    dataC->gps.x.calibratedValue = dataC->gps.x.rawValue;
    dataC->gps.y.calibratedValue = dataC->gps.y.rawValue;
    dataC->gps.z.calibratedValue = dataC->gps.z.rawValue - calib->ref_ground_altitude_m; // AGL Çevrimi
    dataC->gps.speed.calibratedValue = dataC->gps.speed.rawValue;
    dataC->gps.course.calibratedValue = dataC->gps.course.rawValue;
}
