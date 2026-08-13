/*
 * M3.1_Calibration.c
 *
 * Sensör Kalibrasyon (Bias ve Scale hesaplama) algoritmaları
 */

#include "M3.1_Calibration.h"
#include "M0.1_FilterConfig.h" // Default config tanımlamaları için
#include <string.h> // For memset

static struct {
    double acc_x_sum, acc_y_sum, acc_z_sum;
    double acc_x_sq_sum, acc_y_sq_sum, acc_z_sq_sum; // Varyans hesaplaması için
    
    double gyro_x_sum, gyro_y_sum, gyro_z_sum;
    double gyro_x_sq_sum, gyro_y_sq_sum, gyro_z_sq_sum; // Varyans hesaplaması için
    
    double mag_x_sum, mag_y_sum, mag_z_sum;
    double mag_x_sq_sum, mag_y_sq_sum, mag_z_sq_sum; // Varyans hesaplaması için
    
    double baro_press_sum, baro_temp_sum;
    uint32_t count;
} calibData;

void M3_1_Calibration_Reset(void) {
    memset(&calibData, 0, sizeof(calibData));
}

void M3_1_Calibration_LoadDefaults(DataCenter *dc) {
    // ACC
    dc->calibProfile.acc_bias_x = ACC_BIAS_X;
    dc->calibProfile.acc_bias_y = ACC_BIAS_Y;
    dc->calibProfile.acc_bias_z = ACC_BIAS_Z;
    dc->calibProfile.acc_scale_x = ACC_SCALE_X;
    dc->calibProfile.acc_scale_y = ACC_SCALE_Y;
    dc->calibProfile.acc_scale_z = ACC_SCALE_Z;
    dc->calibProfile.acc_noise_x = ACC_NOISE_X;
    dc->calibProfile.acc_noise_y = ACC_NOISE_Y;
    dc->calibProfile.acc_noise_z = ACC_NOISE_Z;

    // GYRO
    dc->calibProfile.gyro_bias_x = GYRO_BIAS_X;
    dc->calibProfile.gyro_bias_y = GYRO_BIAS_Y;
    dc->calibProfile.gyro_bias_z = GYRO_BIAS_Z;
    dc->calibProfile.gyro_scale_x = GYRO_SCALE_X;
    dc->calibProfile.gyro_scale_y = GYRO_SCALE_Y;
    dc->calibProfile.gyro_scale_z = GYRO_SCALE_Z;
    dc->calibProfile.gyro_noise_x = GYRO_NOISE_X;
    dc->calibProfile.gyro_noise_y = GYRO_NOISE_Y;
    dc->calibProfile.gyro_noise_z = GYRO_NOISE_Z;

    // MAG
    dc->calibProfile.mag_bias_x = MAG_BIAS_X;
    dc->calibProfile.mag_bias_y = MAG_BIAS_Y;
    dc->calibProfile.mag_bias_z = MAG_BIAS_Z;
    dc->calibProfile.mag_scale_x = MAG_SCALE_X;
    dc->calibProfile.mag_scale_y = MAG_SCALE_Y;
    dc->calibProfile.mag_scale_z = MAG_SCALE_Z;
    dc->calibProfile.mag_noise_x = MAG_NOISE_X;
    dc->calibProfile.mag_noise_y = MAG_NOISE_Y;
    dc->calibProfile.mag_noise_z = MAG_NOISE_Z;

    // BARO
    dc->calibProfile.baro_press_bias = BARO_PRESS_BIAS;
    dc->calibProfile.baro_press_scale = BARO_PRESS_SCALE;
    dc->calibProfile.baro_press_noise = BARO_PRESS_NOISE;
    dc->calibProfile.baro_temp_bias = BARO_TEMP_BIAS;
    dc->calibProfile.baro_temp_scale = BARO_TEMP_SCALE;
    dc->calibProfile.baro_temp_noise = BARO_TEMP_NOISE;

    // GPS
    dc->calibProfile.gps_lat_bias = GPS_LAT_BIAS;
    dc->calibProfile.gps_lat_scale = GPS_LAT_SCALE;
    dc->calibProfile.gps_lon_bias = GPS_LON_BIAS;
    dc->calibProfile.gps_lon_scale = GPS_LON_SCALE;
    dc->calibProfile.gps_alt_bias = GPS_ALT_BIAS;
    dc->calibProfile.gps_alt_scale = GPS_ALT_SCALE;
    dc->calibProfile.gps_noise = GPS_NOISE;

    // BATT
    dc->calibProfile.batt_volt_bias = BATT_VOLT_BIAS;
    dc->calibProfile.batt_volt_scale = BATT_VOLT_SCALE;
    dc->calibProfile.batt_volt_noise = BATT_VOLT_NOISE;
    dc->calibProfile.batt_curr_bias = BATT_CURR_BIAS;
    dc->calibProfile.batt_curr_scale = BATT_CURR_SCALE;
    dc->calibProfile.batt_curr_noise = BATT_CURR_NOISE;
}

void M3_1_Calibration_Accumulate(DataCenter *dc) {
    calibData.acc_x_sum += dc->acc.raw_x;
    calibData.acc_y_sum += dc->acc.raw_y;
    calibData.acc_z_sum += dc->acc.raw_z;
    
    calibData.acc_x_sq_sum += (double)dc->acc.raw_x * dc->acc.raw_x;
    calibData.acc_y_sq_sum += (double)dc->acc.raw_y * dc->acc.raw_y;
    calibData.acc_z_sq_sum += (double)dc->acc.raw_z * dc->acc.raw_z;
    
    calibData.gyro_x_sum += dc->gyro.raw_x;
    calibData.gyro_y_sum += dc->gyro.raw_y;
    calibData.gyro_z_sum += dc->gyro.raw_z;
    
    calibData.gyro_x_sq_sum += (double)dc->gyro.raw_x * dc->gyro.raw_x;
    calibData.gyro_y_sq_sum += (double)dc->gyro.raw_y * dc->gyro.raw_y;
    calibData.gyro_z_sq_sum += (double)dc->gyro.raw_z * dc->gyro.raw_z;
    
    calibData.mag_x_sum += dc->mag.raw_x;
    calibData.mag_y_sum += dc->mag.raw_y;
    calibData.mag_z_sum += dc->mag.raw_z;
    
    calibData.mag_x_sq_sum += (double)dc->mag.raw_x * dc->mag.raw_x;
    calibData.mag_y_sq_sum += (double)dc->mag.raw_y * dc->mag.raw_y;
    calibData.mag_z_sq_sum += (double)dc->mag.raw_z * dc->mag.raw_z;
    
    calibData.baro_press_sum += dc->baro.raw_press;
    calibData.baro_temp_sum += dc->baro.raw_temp;
    
    calibData.count++;
}

void M3_1_Calibration_Calculate(DataCenter *dc) {
    if (calibData.count == 0) return;
    
    // GYRO (Hareket etmediği için bias 0 olmalı, ortalamayı direkt bias yapıyoruz)
    dc->calibProfile.gyro_bias_x = (float)(calibData.gyro_x_sum / calibData.count);
    dc->calibProfile.gyro_bias_y = (float)(calibData.gyro_y_sum / calibData.count);
    dc->calibProfile.gyro_bias_z = (float)(calibData.gyro_z_sum / calibData.count);
    
    // GYRO VARYANS (EKF Q Matrisi İçin)
    dc->calibProfile.gyro_noise_x = (float)((calibData.gyro_x_sq_sum / calibData.count) - (dc->calibProfile.gyro_bias_x * dc->calibProfile.gyro_bias_x));
    dc->calibProfile.gyro_noise_y = (float)((calibData.gyro_y_sq_sum / calibData.count) - (dc->calibProfile.gyro_bias_y * dc->calibProfile.gyro_bias_y));
    dc->calibProfile.gyro_noise_z = (float)((calibData.gyro_z_sq_sum / calibData.count) - (dc->calibProfile.gyro_bias_z * dc->calibProfile.gyro_bias_z));
    
    // ACC (Düz zeminde X=0, Y=0, Z=+1G bekliyoruz. Yönümüz NWU, yani Z yukarı. Z ekseni +1G okumalı)
    float acc_ref_x = 0.0f;
    float acc_ref_y = 0.0f;
    float acc_ref_z = 1.0f; // NWU (Z Yukarı) için ivmeölçer +1 okur.
    
    float acc_avg_x = (float)(calibData.acc_x_sum / calibData.count);
    float acc_avg_y = (float)(calibData.acc_y_sum / calibData.count);
    float acc_avg_z = (float)(calibData.acc_z_sum / calibData.count);
    
    dc->calibProfile.acc_bias_x = acc_avg_x - acc_ref_x;
    dc->calibProfile.acc_bias_y = acc_avg_y - acc_ref_y;
    dc->calibProfile.acc_bias_z = acc_avg_z - acc_ref_z;
    
    // ACC VARYANS (EKF R Matrisi İçin)
    dc->calibProfile.acc_noise_x = (float)((calibData.acc_x_sq_sum / calibData.count) - (acc_avg_x * acc_avg_x));
    dc->calibProfile.acc_noise_y = (float)((calibData.acc_y_sq_sum / calibData.count) - (acc_avg_y * acc_avg_y));
    dc->calibProfile.acc_noise_z = (float)((calibData.acc_z_sq_sum / calibData.count) - (acc_avg_z * acc_avg_z));
    
    // Minimum varyans sınırları (Sıfıra bölme hatasını önlemek için)
    if(dc->calibProfile.gyro_noise_x < 0.000001f) dc->calibProfile.gyro_noise_x = 0.000001f;
    if(dc->calibProfile.gyro_noise_y < 0.000001f) dc->calibProfile.gyro_noise_y = 0.000001f;
    if(dc->calibProfile.gyro_noise_z < 0.000001f) dc->calibProfile.gyro_noise_z = 0.000001f;
    
    if(dc->calibProfile.acc_noise_x < 0.0001f) dc->calibProfile.acc_noise_x = 0.0001f;
    if(dc->calibProfile.acc_noise_y < 0.0001f) dc->calibProfile.acc_noise_y = 0.0001f;
    if(dc->calibProfile.acc_noise_z < 0.0001f) dc->calibProfile.acc_noise_z = 0.0001f;
    
    // MAG VARYANS (EKF R Matrisi İçin) - Uçuş öncesi manyetik gürültü ölçümü
    float mag_avg_x = (float)(calibData.mag_x_sum / calibData.count);
    float mag_avg_y = (float)(calibData.mag_y_sum / calibData.count);
    float mag_avg_z = (float)(calibData.mag_z_sum / calibData.count);
    
    dc->calibProfile.mag_noise_x = (float)((calibData.mag_x_sq_sum / calibData.count) - (mag_avg_x * mag_avg_x));
    dc->calibProfile.mag_noise_y = (float)((calibData.mag_y_sq_sum / calibData.count) - (mag_avg_y * mag_avg_y));
    dc->calibProfile.mag_noise_z = (float)((calibData.mag_z_sq_sum / calibData.count) - (mag_avg_z * mag_avg_z));
    
    if(dc->calibProfile.mag_noise_x < 0.0001f) dc->calibProfile.mag_noise_x = 0.0001f;
    if(dc->calibProfile.mag_noise_y < 0.0001f) dc->calibProfile.mag_noise_y = 0.0001f;
    if(dc->calibProfile.mag_noise_z < 0.0001f) dc->calibProfile.mag_noise_z = 0.0001f;
    
    // NOT: mag_bias hesaplaması buradan kaldırıldı çünkü sabit dururken pusula sıfırlanmaz!
    
    // BARO (Mevcut basıncı yer seviyesi sıfır noktası olarak alabiliriz)
    dc->calibProfile.baro_press_bias = (float)(calibData.baro_press_sum / calibData.count);
    dc->calibProfile.baro_temp_bias = (float)(calibData.baro_temp_sum / calibData.count);
    
    // Ölçekleri varsayılan olarak 1.0'da bırakalım, gelişmiş kalibrasyonda min-max hesaplanır.
    dc->calibProfile.acc_scale_x = 1.0f;
    dc->calibProfile.acc_scale_y = 1.0f;
    dc->calibProfile.acc_scale_z = 1.0f;
    
    dc->calibProfile.gyro_scale_x = 1.0f;
    dc->calibProfile.gyro_scale_y = 1.0f;
    dc->calibProfile.gyro_scale_z = 1.0f;
    
    /*dc->calibProfile.mag_scale_x = 1.0f;
    dc->calibProfile.mag_scale_y = 1.0f;
    dc->calibProfile.mag_scale_z = 1.0f;*/
}
