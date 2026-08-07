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
#include "M0.0_DataCenter.h"

uint8_t MPU9250_Init(I2C_HandleTypeDef *i2c_handle);

uint8_t MPU9250_Read(DataCenter *data);

uint8_t MPU9250_IsReady(void);

uint8_t MPU9250_IsMagReady(void);


#endif /* INC_MPU9250_H_ */
