/*
 * bmp280.c
 *
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

/*
 * bmp280.c
 *
 * BMP280 barometric pressure sensor driver
 * Sade versiyon.
 *
 * GÃ¶rev:
 * - BMP280'i baÅŸlatmak
 * - Calibration parametrelerini okumak
 * - Pressure + temperature okumak
 */

#include "M1.2_BMP280.h"
#include "M1.0_SensorReaderCore.h"

#include <stddef.h>


/*==================================================
 * AYARLAR
 *==================================================*/

#define BMP280_I2C_TIMEOUT_MS          20U

/*
 * SDO pini GND ise adres 0x76 olur.
 * SDO pini VDDIO ise adres 0x77 olur.
 *
 * SensÃ¶r cevap vermezse Ã¶nce bunu 0x77 yapÄ±p deneyin.
 */
#define BMP280_I2C_ADDR                0x76U


/*==================================================
 * REGISTER ADRESLERÄ°
 *==================================================*/

#define BMP280_REG_ID                  0xD0U
#define BMP280_REG_CTRL_MEAS           0xF4U
#define BMP280_REG_CONFIG              0xF5U
#define BMP280_REG_PRESS_MSB           0xF7U
#define BMP280_REG_CALIB_START         0x88U


/*==================================================
 * REGISTER DEÄERLERÄ°
 *==================================================*/

#define BMP280_CHIP_ID                 0x58U

/*
 * ctrl_meas register:
 *
 * bit 7:5 -> osrs_t
 * bit 4:2 -> osrs_p
 * bit 1:0 -> mode
 */
#define BMP280_OSRS_T_X1               0x01U
#define BMP280_OSRS_P_X4               0x03U
#define BMP280_MODE_NORMAL             0x03U

/*
 * config register:
 *
 * bit 7:5 -> standby time
 * bit 4:2 -> IIR filter
 * bit 0   -> SPI 3-wire enable
 *
 * Bu driver'da filtreyi kapalÄ± bÄ±rakÄ±yoruz.
 * Filtreleme iÅŸini daha sonra sensors.c iÃ§inde yapacaÄŸÄ±z.
 */
#define BMP280_STANDBY_0_5_MS          0x00U
#define BMP280_FILTER_OFF              0x00U


/*==================================================
 * CALIBRATION STRUCT
 *==================================================*/

typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} BMP280_Calib;


/*==================================================
 * STATIC DEÄÄ°ÅKENLER
 *==================================================*/

static I2C_HandleTypeDef *bmp280_i2c = NULL;

static uint8_t bmp280_ready = 0U;

static BMP280_Calib calib;

static int32_t t_fine = 0;


/*==================================================
 * YARDIMCI FONKSÄ°YONLAR
 *==================================================*/

static uint8_t BMP280_ReadRegister(uint8_t reg_addr,
                                   uint8_t *buffer,
                                   uint16_t length)
{
    if((bmp280_i2c == NULL) || (buffer == NULL))
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Read(bmp280_i2c,
                        (uint16_t)(BMP280_I2C_ADDR << 1),
                        reg_addr,
                        I2C_MEMADD_SIZE_8BIT,
                        buffer,
                        length,
                        BMP280_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(bmp280_i2c);
        return 0U;
    }

    return 1U;
}


static uint8_t BMP280_WriteRegister(uint8_t reg_addr,
                                    uint8_t value)
{
    if(bmp280_i2c == NULL)
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Write(bmp280_i2c,
                         (uint16_t)(BMP280_I2C_ADDR << 1),
                         reg_addr,
                         I2C_MEMADD_SIZE_8BIT,
                         &value,
                         1U,
                         BMP280_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(bmp280_i2c);
        return 0U;
    }

    return 1U;
}


static uint16_t BMP280_U16_LE(uint8_t lsb, uint8_t msb)
{
    return (uint16_t)(((uint16_t)msb << 8) | lsb);
}


static int16_t BMP280_S16_LE(uint8_t lsb, uint8_t msb)
{
    return (int16_t)BMP280_U16_LE(lsb, msb);
}


static uint8_t BMP280_ReadCalibration(void)
{
    uint8_t buffer[24];

    if(BMP280_ReadRegister(BMP280_REG_CALIB_START,
                           buffer,
                           24U) == 0U)
    {
        return 0U;
    }

    calib.dig_T1 = BMP280_U16_LE(buffer[0],  buffer[1]);
    calib.dig_T2 = BMP280_S16_LE(buffer[2],  buffer[3]);
    calib.dig_T3 = BMP280_S16_LE(buffer[4],  buffer[5]);

    calib.dig_P1 = BMP280_U16_LE(buffer[6],  buffer[7]);
    calib.dig_P2 = BMP280_S16_LE(buffer[8],  buffer[9]);
    calib.dig_P3 = BMP280_S16_LE(buffer[10], buffer[11]);
    calib.dig_P4 = BMP280_S16_LE(buffer[12], buffer[13]);
    calib.dig_P5 = BMP280_S16_LE(buffer[14], buffer[15]);
    calib.dig_P6 = BMP280_S16_LE(buffer[16], buffer[17]);
    calib.dig_P7 = BMP280_S16_LE(buffer[18], buffer[19]);
    calib.dig_P8 = BMP280_S16_LE(buffer[20], buffer[21]);
    calib.dig_P9 = BMP280_S16_LE(buffer[22], buffer[23]);

    /*
     * dig_P1 sÄ±fÄ±rsa pressure compensation formÃ¼lÃ¼nde bÃ¶lme hatasÄ± oluÅŸabilir.
     */
    if(calib.dig_P1 == 0U)
    {
        return 0U;
    }

    return 1U;
}


static float BMP280_CompensateTemperature(int32_t adc_T)
{
    double var1;
    double var2;
    double temperature;

    /*
     * Datasheet floating point compensation formÃ¼lÃ¼.
     */
    var1 =
        (((double)adc_T / 16384.0) -
        ((double)calib.dig_T1 / 1024.0)) *
        ((double)calib.dig_T2);

    var2 =
        ((((double)adc_T / 131072.0) -
        ((double)calib.dig_T1 / 8192.0)) *
        (((double)adc_T / 131072.0) -
        ((double)calib.dig_T1 / 8192.0))) *
        ((double)calib.dig_T3);

    t_fine = (int32_t)(var1 + var2);

    temperature = (var1 + var2) / 5120.0;

    return (float)temperature;
}


static float BMP280_CompensatePressure(int32_t adc_P)
{
    double var1;
    double var2;
    double pressure;

    /*
     * Datasheet floating point compensation formÃ¼lÃ¼.
     * Bu fonksiyonun doÄŸru Ã§alÄ±ÅŸmasÄ± iÃ§in Ã¶nce temperature compensation
     * Ã§alÄ±ÅŸmÄ±ÅŸ ve t_fine gÃ¼ncellenmiÅŸ olmalÄ±.
     */
    var1 = ((double)t_fine / 2.0) - 64000.0;

    var2 = var1 * var1 * ((double)calib.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calib.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)calib.dig_P4) * 65536.0);

    var1 =
        (((double)calib.dig_P3) * var1 * var1 / 524288.0 +
        ((double)calib.dig_P2) * var1) / 524288.0;

    var1 = (1.0 + var1 / 32768.0) * ((double)calib.dig_P1);

    if(var1 == 0.0)
    {
        return 0.0f;
    }

    pressure = 1048576.0 - (double)adc_P;
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;

    var1 = ((double)calib.dig_P9) *
           pressure * pressure / 2147483648.0;

    var2 = pressure * ((double)calib.dig_P8) / 32768.0;

    pressure =
        pressure +
        (var1 + var2 + ((double)calib.dig_P7)) / 16.0;

    return (float)pressure;
}


/*==================================================
 * PUBLIC FONKSÄ°YONLAR
 *==================================================*/

uint8_t BMP280_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t chip_id;
    uint8_t ctrl_meas;
    uint8_t config;

    if(i2c_handle == NULL)
    {
        bmp280_ready = 0U;
        return 0U;
    }

    bmp280_i2c = i2c_handle;
    bmp280_ready = 0U;

    /*
     * Datasheet'e gÃ¶re start-up sonrasÄ± ilk haberleÅŸme iÃ§in kÄ±sa bekleme.
     */
    HAL_Delay(5U);

    /*
     * SensÃ¶r gerÃ§ekten BMP280 mi?
     */
    if(BMP280_ReadRegister(BMP280_REG_ID,
                           &chip_id,
                           1U) == 0U)
    {
        return 0U;
    }

    if(chip_id != BMP280_CHIP_ID)
    {
        return 0U;
    }

    /*
     * BMP280'de doÄŸru pressure/temperature hesabÄ± iÃ§in
     * calibration parametreleri okunmalÄ±.
     */
    if(BMP280_ReadCalibration() == 0U)
    {
        return 0U;
    }

    /*
     * Config register:
     * standby = 0.5 ms
     * IIR filter = off
     */
    config =
        (uint8_t)(BMP280_STANDBY_0_5_MS << 5) |
        (uint8_t)(BMP280_FILTER_OFF << 2);

    if(BMP280_WriteRegister(BMP280_REG_CONFIG,
                            config) == 0U)
    {
        return 0U;
    }

    /*
     * ctrl_meas register:
     * temperature oversampling = x1
     * pressure oversampling    = x4
     * mode                     = normal
     */
    ctrl_meas =
        (uint8_t)(BMP280_OSRS_T_X1 << 5) |
        (uint8_t)(BMP280_OSRS_P_X4 << 2) |
        BMP280_MODE_NORMAL;

    if(BMP280_WriteRegister(BMP280_REG_CTRL_MEAS,
                            ctrl_meas) == 0U)
    {
        return 0U;
    }

    HAL_Delay(20U);

    bmp280_ready = 1U;

    return 1U;
}


uint8_t BMP280_Read(DataCenter *data)
{
    uint8_t buffer[6];

    int32_t adc_P;
    int32_t adc_T;

    if(data == NULL)
    {
        return 0U;
    }

    if(bmp280_ready == 0U)
    {
        return 0U;
    }

    /*
     * 0xF7'den baÅŸlayarak 6 byte okunur:
     *
     * 0xF7 press_msb
     * 0xF8 press_lsb
     * 0xF9 press_xlsb
     * 0xFA temp_msb
     * 0xFB temp_lsb
     * 0xFC temp_xlsb
     */
    if(BMP280_ReadRegister(BMP280_REG_PRESS_MSB,
                           buffer,
                           6U) == 0U)
    {
        return 0U;
    }

    /*
     * BMP280 pressure ve temperature raw deÄŸerleri 20-bit formatta gelir.
     */
    adc_P =
        ((int32_t)buffer[0] << 12) |
        ((int32_t)buffer[1] << 4)  |
        ((int32_t)buffer[2] >> 4);

    adc_T =
        ((int32_t)buffer[3] << 12) |
        ((int32_t)buffer[4] << 4)  |
        ((int32_t)buffer[5] >> 4);

    /*
     * Ã–nce temperature hesaplanÄ±r.
     * Ã‡Ã¼nkÃ¼ pressure compensation iÃ§in t_fine gerekiyor.
     */
    data->baro.temp.rawValue = BMP280_CompensateTemperature(adc_T);
    data->baro.press.rawValue = BMP280_CompensatePressure(adc_P);

    if(data->baro.press.rawValue <= 0.0f)
    {
        return 0U;
    }

    data->baro.UpdateTime = HAL_GetTick() * 1000;

    return 1U;
}


uint8_t BMP280_IsReady(void)
{
    return bmp280_ready;
}

