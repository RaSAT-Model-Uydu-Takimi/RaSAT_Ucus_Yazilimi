/*
 * M0.0_DataCenter.h
 *
 * DataCenter Structure
 * Stores all sensor raw/calibrated data and estimated states.
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FLIGHT_MODE_UNCALIBRATED = 0, // Makrolardaki sabit kalibrasyon değerlerini kullanır
    FLIGHT_MODE_CALIBRATED   = 1  // RAM'de dinamik hesaplanan değerleri kullanır
} FlightMode;

typedef struct {
    float raw_x;
    float raw_y;
    float raw_z;
    
    float calibrated_x;
    float calibrated_y;
    float calibrated_z;
    
    uint32_t UpdateTime;
} AccData, GyroData, MagData;

typedef struct {
    float raw_press;
    float raw_temp;
    
    float calibrated_press;
    float calibrated_temp;
    
    uint32_t UpdateTime;
} BaroData;

typedef struct {
    double raw_lat;
    double raw_lon;
    float raw_alt;
    
    double calibrated_lat;
    double calibrated_lon;
    float calibrated_alt;
    
    float HDOP;
    uint8_t satelliteCount;
    uint8_t fixQuality;
    float speed;
    uint32_t UpdateTime;
} GpsData;

typedef struct {
    float raw_volt;
    float raw_curr;
    
    float calibrated_volt;
    float calibrated_curr;
    
    uint32_t UpdateTime;
} BattData;

// Filtrelenmiş / Hesaplanmış Uçuş Verileri
typedef struct {
    // Oryantasyon (Attitude)
    float roll;
    float pitch;
    float yaw;
    float q0, q1, q2, q3; // Kuaterniyonlar
    
    // Konum ve İrtifa (Position & Altitude)
    float altitude; // Filtrelenmiş net irtifa (m)
    double latitude; 
    double longitude; 
    
    // Hız ve İvme (Velocity & Acceleration)
    float vertical_velocity; // Düşey hız (m/s)
    float horizontal_velocity; // Yatay hız (m/s)
    
    float linear_acceleration_x;
    float linear_acceleration_y;
    float linear_acceleration_z;
} EstimatedData;

// Sensör Kalibrasyon Profili (Bias, Scale, Noise)
typedef struct {
    // İvmeölçer Kalibrasyon Verileri
    float acc_bias_x;
    float acc_bias_y;
    float acc_bias_z;
    
    float acc_scale_x;
    float acc_scale_y;
    float acc_scale_z;
    
    float acc_noise_x;
    float acc_noise_y;
    float acc_noise_z;
    
    // Jiroskop Kalibrasyon Verileri
    float gyro_bias_x;
    float gyro_bias_y;
    float gyro_bias_z;
    
    float gyro_scale_x;
    float gyro_scale_y;
    float gyro_scale_z;
    
    float gyro_noise_x;
    float gyro_noise_y;
    float gyro_noise_z;
    
    // Manyetometre Kalibrasyon Verileri
    float mag_bias_x;
    float mag_bias_y;
    float mag_bias_z;
    float mag_scale_x;
    float mag_scale_y;
    float mag_scale_z;
    float mag_noise_x;
    float mag_noise_y;
    float mag_noise_z;
    
    // Barometre Kalibrasyon Verileri
    float baro_press_bias;
    float baro_press_scale;
    float baro_press_noise;
    float baro_temp_bias;
    float baro_temp_scale;
    float baro_temp_noise;
    
    // GPS Kalibrasyon Verileri
    double gps_lat_bias;
    double gps_lat_scale;
    double gps_lon_bias;
    double gps_lon_scale;
    float gps_alt_bias;
    float gps_alt_scale;
    float gps_noise;
    
    // Batarya Kalibrasyon Verileri
    float batt_volt_bias;
    float batt_volt_scale;
    float batt_volt_noise;
    float batt_curr_bias;
    float batt_curr_scale;
    float batt_curr_noise;
} SensorCalibProfile;

typedef struct {
    SensorCalibProfile calibProfile;
    EstimatedData estimated;
    FlightMode flightMode; // Uçuş modunu takip etmek için eklendi
    
    AccData acc;
    GyroData gyro;
    BaroData baro;
    MagData mag;
    GpsData gps;
    BattData batt;
} DataCenter;

#ifdef __cplusplus
}
#endif

#endif /* INC_DATA_H_ */
