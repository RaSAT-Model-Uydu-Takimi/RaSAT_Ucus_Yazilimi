/*
 * mpu9250.h
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_


/*
 * mpu9250.h
 *
 * MPU9250 IMU driver
 * Accelerometer + gyroscope + magnetometer okur.
 */

#include <stdint.h>
#include "main.h"


typedef struct
{
    /*
     * Accelerometer
     * Birim: g
     */
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;

    /*
     * Gyroscope
     * Birim: degree per second
     */
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;

    /*
     * Magnetometer
     * Birim: microtesla
     */
    float mag_x_uT;
    float mag_y_uT;
    float mag_z_uT;

    /*
     * valid:
     * accel + gyro verisi geçerli mi?
     *
     * mag_valid:
     * magnetometer verisi geçerli mi?
     */
    uint8_t valid;
    uint8_t mag_valid;

} MPU9250_Data;


uint8_t MPU9250_Init(I2C_HandleTypeDef *i2c_handle);

uint8_t MPU9250_Read(MPU9250_Data *data);

uint8_t MPU9250_IsReady(void);

uint8_t MPU9250_IsMagReady(void);


#endif /* INC_MPU9250_H_ */
