/*
 * bmp580.h
 *
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_
/*
 * bmp280.h
 *
 * BMP280 barometric pressure sensor driver
 * Görev: Basınç ve sıcaklık okumak.
 */


#include <stdint.h>
#include "main.h"


typedef struct
{
    float pressure_pa;
    float temperature_c;
    uint8_t valid;

} BMP280_Data;


uint8_t BMP280_Init(I2C_HandleTypeDef *i2c_handle);

uint8_t BMP280_Read(BMP280_Data *data);

uint8_t BMP280_IsReady(void);


#endif /* INC_BMP280_H_ */
