/*
 * Sensor_Kodlari.h
 *
 *  Created on: Aug 10, 2026
 *      Author: abduh
 */

#ifndef INC_SENSOR_KODLARI_H_
#define INC_SENSOR_KODLARI_H_


#include "main.h"

#include "M0.0_DataCenter.h"
#include "Konfigurasyonlar.h"
#include "mpu9250.h"
#include "bmp280.h"
#include "gps.h"

#include <stdint.h>


uint8_t Sensors_Init(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *gps_uart_handle, uint16_t mpu9250_int_pin);

void SensorleriOku(DataCenter *DC, uint32_t SystemMicros);

void MPU9250_Int_Fonksiyonu(uint16_t GPIO_Pin);

void GPS_Uart_Int_Fonksiyonu(UART_HandleTypeDef *huart);








#endif /* INC_SENSOR_KODLARI_H_ */
