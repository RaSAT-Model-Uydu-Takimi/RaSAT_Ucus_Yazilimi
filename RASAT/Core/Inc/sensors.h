/*
 * sensors.h
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

/*
 * sensors.h
 *
 * Sensor manager / fusion layer
 *
 * Görev:
 * - BMP280, MPU9250 ve GPS driverlarını birlikte çalıştırmak
 * - Flight control için gerekli ortak SensorData yapısını üretmek
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include <stdint.h>
#include "main.h"
#include "flight_control.h"


typedef struct
{
    /*
     * BMP280
     */
    float pressure_pa;
    float temperature_c;
    float altitude_m;
    float vertical_velocity_mps;
    uint8_t barometer_valid;

    /*
     * MPU9250 accelerometer
     * Birim: g
     */
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;

    /*
     * MPU9250 gyroscope
     * Birim: degree per second
     */
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;

    /*
     * MPU9250 magnetometer
     * Birim: microtesla
     */
    float mag_x_uT;
    float mag_y_uT;
    float mag_z_uT;

    uint8_t imu_valid;
    uint8_t mag_valid;

    /*
     * GPS
     */
    double gps_latitude_deg;
    double gps_longitude_deg;

    float gps_altitude_m;
    float gps_speed_mps;
    float gps_course_deg;
    float gps_hdop;

    uint8_t gps_satellites;
    uint8_t gps_fix_quality;
    uint8_t gps_fix_valid;
    uint8_t gps_new_data;

    /*
     * Driver status
     */
    uint8_t bmp280_ready;
    uint8_t mpu9250_ready;
    uint8_t mag_ready;
    uint8_t gps_ready;
    uint8_t gps_receive_started;

    /*
     * Flight control için ortak flagler
     */
    uint8_t data_valid;
    uint8_t payload_separated;
    uint8_t landed_detected;
    uint8_t apam_request;

} SensorData;


uint8_t Sensors_Init(I2C_HandleTypeDef *i2c_handle,
                     UART_HandleTypeDef *gps_uart_handle);

void Sensors_Update(void);

SensorData Sensors_GetData(void);

void Sensors_ToFlightControlInput(FlightControlInput *fc_input);

void Sensors_UART_RxCpltCallback(UART_HandleTypeDef *uart_handle);

void Sensors_ResetAltitudeReference(void);

void Sensors_SetPayloadSeparated(uint8_t separated);

void Sensors_SetExternalApamRequest(uint8_t request);

uint8_t Sensors_IsReady(void);

#endif /* INC_SENSORS_H_ */
