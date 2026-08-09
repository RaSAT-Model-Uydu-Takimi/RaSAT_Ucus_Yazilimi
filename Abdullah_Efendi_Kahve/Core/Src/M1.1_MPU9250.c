/*
 * mpu9250.c
 *
 *  Created on: Jul 11, 2026
 *      Author: umut
 */

/*
 * mpu9250.c
 *
 * MPU9250 IMU driver
 * Sade versiyon.
 *
 * Görev:
 * - MPU9250'yi başlatmak
 * - Accelerometer + gyroscope okumak
 * - AK8963 magnetometer okumak
 */

#include "M1.1_MPU9250.h"
#include "M0.1_FilterConfig.h"
#include "M1.0_SensorReaderCore.h"
#include <string.h>

#include <stddef.h>


/*==================================================
 * AYARLAR
 *==================================================*/

#define MPU9250_I2C_TIMEOUT_MS         20U

/*
 * AD0 pini GND ise adres 0x68 olur.
 * AD0 pini VDDIO ise adres 0x69 olur.
 *
 * Sensör cevap vermezse önce bunu 0x69 yapıp deneyin.
 */
#define MPU9250_I2C_ADDR               0x68U


/*==================================================
 * MPU9250 REGISTER ADRESLERİ
 *==================================================*/

#define MPU9250_REG_SMPLRT_DIV         0x19U
#define MPU9250_REG_CONFIG             0x1AU
#define MPU9250_REG_GYRO_CONFIG        0x1BU
#define MPU9250_REG_ACCEL_CONFIG       0x1CU
#define MPU9250_REG_ACCEL_CONFIG2      0x1DU

#define MPU9250_REG_INT_PIN_CFG        0x37U

#define MPU9250_REG_ACCEL_XOUT_H       0x3BU

#define MPU9250_REG_USER_CTRL          0x6AU
#define MPU9250_REG_PWR_MGMT_1         0x6BU
#define MPU9250_REG_PWR_MGMT_2         0x6CU

#define MPU9250_REG_WHO_AM_I           0x75U


/*==================================================
 * MPU9250 REGISTER DEĞERLERİ
 *==================================================*/

#define MPU9250_WHO_AM_I_VALUE         0x71U

#define MPU9250_DEVICE_RESET           0x80U
#define MPU9250_CLK_PLL                0x01U

#define MPU9250_SMPLRT_DIV_200HZ       4U

#define MPU9250_DLPF_CFG_41HZ          0x03U
#define MPU9250_DLPF_CFG_20HZ          0x04U

#define MPU9250_GYRO_FS_250DPS         0x00U

#define MPU9250_ACCEL_FS_2G            0x00U

#define MPU9250_ACCEL_DLPF_41HZ        0x03U
#define MPU9250_ACCEL_DLPF_20HZ        0x04U

/*
 * INT_PIN_CFG register:
 * BYPASS_EN = 1
 *
 * Böylece AK8963 magnetometer'a ana I2C hattından erişilebilir.
 */
#define MPU9250_BYPASS_EN              0x02U


/*==================================================
 * SCALE FACTOR
 *==================================================*/

/*
 * ±2g için:
 *
 * 32768 LSB / 2g = 16384 LSB/g
 */
#define MPU9250_ACCEL_SCALE_2G         16384.0f

/*
 * ±250 dps için:
 *
 * 32768 LSB / 250 dps = 131.0 LSB/dps
 */
#define MPU9250_GYRO_SCALE_250DPS      131.0f


/*==================================================
 * AK8963 MAGNETOMETER REGISTERLARI
 *==================================================*/

#define AK8963_I2C_ADDR                0x0CU

#define AK8963_REG_WIA                 0x00U
#define AK8963_REG_ST1                 0x02U
#define AK8963_REG_HXL                 0x03U
#define AK8963_REG_CNTL1               0x0AU
#define AK8963_REG_CNTL2               0x0BU
#define AK8963_REG_ASAX                0x10U

#define AK8963_WHO_AM_I_VALUE          0x48U

#define AK8963_MODE_POWER_DOWN         0x00U
#define AK8963_MODE_FUSE_ROM           0x0FU
#define AK8963_MODE_CONT_2_16BIT       0x16U

#define AK8963_SOFT_RESET              0x01U

#define AK8963_ST1_DRDY                0x01U
#define AK8963_ST2_HOFL                0x08U

/*
 * AK8963 16-bit mode için yaklaşık ölçek:
 * full scale ≈ ±4912 uT
 */
#define AK8963_MAG_SCALE_16BIT         0.149938f


/*==================================================
 * STATIC DEĞİŞKENLER
 *==================================================*/

static I2C_HandleTypeDef *mpu9250_i2c = NULL;

static uint8_t mpu9250_ready = 0U;
static uint8_t ak8963_ready = 0U;

static float ak8963_adj_x = 1.0f;
static float ak8963_adj_y = 1.0f;
static float ak8963_adj_z = 1.0f;


/*==================================================
 * YARDIMCI FONKSİYONLAR
 *==================================================*/

static uint8_t MPU9250_ReadRegister(uint8_t reg_addr,
                                    uint8_t *buffer,
                                    uint16_t length)
{
    if((mpu9250_i2c == NULL) || (buffer == NULL))
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Read(mpu9250_i2c,
                        (uint16_t)(MPU9250_I2C_ADDR << 1),
                        reg_addr,
                        I2C_MEMADD_SIZE_8BIT,
                        buffer,
                        length,
                        MPU9250_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(mpu9250_i2c);
        return 0U;
    }

    return 1U;
}


static uint8_t MPU9250_WriteRegister(uint8_t reg_addr,
                                     uint8_t value)
{
    if(mpu9250_i2c == NULL)
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Write(mpu9250_i2c,
                         (uint16_t)(MPU9250_I2C_ADDR << 1),
                         reg_addr,
                         I2C_MEMADD_SIZE_8BIT,
                         &value,
                         1U,
                         MPU9250_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(mpu9250_i2c);
        return 0U;
    }

    return 1U;
}


static uint8_t AK8963_ReadRegister(uint8_t reg_addr,
                                   uint8_t *buffer,
                                   uint16_t length)
{
    if((mpu9250_i2c == NULL) || (buffer == NULL))
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Read(mpu9250_i2c,
                        (uint16_t)(AK8963_I2C_ADDR << 1),
                        reg_addr,
                        I2C_MEMADD_SIZE_8BIT,
                        buffer,
                        length,
                        MPU9250_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(mpu9250_i2c);
        return 0U;
    }

    return 1U;
}


static uint8_t AK8963_WriteRegister(uint8_t reg_addr,
                                    uint8_t value)
{
    if(mpu9250_i2c == NULL)
    {
        return 0U;
    }

    if(HAL_I2C_Mem_Write(mpu9250_i2c,
                         (uint16_t)(AK8963_I2C_ADDR << 1),
                         reg_addr,
                         I2C_MEMADD_SIZE_8BIT,
                         &value,
                         1U,
                         MPU9250_I2C_TIMEOUT_MS) != HAL_OK)
    {
        I2C_BusRecovery(mpu9250_i2c);
        return 0U;
    }

    return 1U;
}


static int16_t MPU9250_ToInt16(uint8_t msb,
                               uint8_t lsb)
{
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}

void MPU9250_EnableBypass(void)
{
    if(mpu9250_i2c == NULL) return;
    MPU9250_WriteRegister(MPU9250_REG_INT_PIN_CFG, 0x02U);
}


static int16_t AK8963_ToInt16_LE(uint8_t lsb,
                                 uint8_t msb)
{
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}


/*==================================================
 * AK8963 MAGNETOMETER INIT
 *==================================================*/

static uint8_t AK8963_Init(void)
{
    uint8_t who_am_i;
    uint8_t asa[3];

    ak8963_ready = 0U;

    /*
     * Soft reset.
     */
    (void)AK8963_WriteRegister(AK8963_REG_CNTL2,
                               AK8963_SOFT_RESET);

    HAL_Delay(50U);

    /*
     * AK8963 cevap veriyor mu?
     */
    if(AK8963_ReadRegister(AK8963_REG_WIA,
                           &who_am_i,
                           1U) == 0U)
    {
        return 0U;
    }

    if(who_am_i != AK8963_WHO_AM_I_VALUE)
    {
        return 0U;
    }

    /*
     * Power down.
     */
    if(AK8963_WriteRegister(AK8963_REG_CNTL1,
                            AK8963_MODE_POWER_DOWN) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    /*
     * Fuse ROM mode.
     * ASAX, ASAY, ASAZ bu modda okunmalı.
     */
    if(AK8963_WriteRegister(AK8963_REG_CNTL1,
                            AK8963_MODE_FUSE_ROM) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    if(AK8963_ReadRegister(AK8963_REG_ASAX,
                           asa,
                           3U) == 0U)
    {
        return 0U;
    }

    /*
     * Sensitivity adjustment:
     * adjustment = ((ASA - 128) / 256) + 1
     */
    ak8963_adj_x = (((float)asa[0] - 128.0f) / 256.0f) + 1.0f;
    ak8963_adj_y = (((float)asa[1] - 128.0f) / 256.0f) + 1.0f;
    ak8963_adj_z = (((float)asa[2] - 128.0f) / 256.0f) + 1.0f;

    /*
     * Tekrar power down.
     */
    if(AK8963_WriteRegister(AK8963_REG_CNTL1,
                            AK8963_MODE_POWER_DOWN) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    /*
     * Continuous measurement mode 2:
     * 16-bit, 100 Hz.
     */
    if(AK8963_WriteRegister(AK8963_REG_CNTL1,
                            AK8963_MODE_CONT_2_16BIT) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    ak8963_ready = 1U;

    return 1U;
}


/*==================================================
 * AK8963 MAGNETOMETER READ
 *==================================================*/

static uint8_t AK8963_Read(DataCenter *data)
{
    uint8_t st1;
    uint8_t buffer[7];
    uint8_t st2;

    int16_t raw_mag_x;
    int16_t raw_mag_y;
    int16_t raw_mag_z;

    if(data == NULL)
    {
        return 0U;
    }

    if(ak8963_ready == 0U)
    {
        return 0U;
    }

    /*
     * Yeni magnetometer verisi hazır mı?
     */
    if(AK8963_ReadRegister(AK8963_REG_ST1,
                           &st1,
                           1U) == 0U)
    {
        return 0U;
    }

    if((st1 & AK8963_ST1_DRDY) == 0U)
    {
        return 0U;
    }



    if(AK8963_ReadRegister(AK8963_REG_HXL,
                           buffer,
                           7U) == 0U)
    {
        return 0U;
    }

    st2 = buffer[6];

    /*
     * Magnetic overflow kontrolü.
     */
    if((st2 & AK8963_ST2_HOFL) != 0U)
    {
        return 0U;
    }

    /*
     * AK8963 little-endian veri verir.
     */
    raw_mag_x = AK8963_ToInt16_LE(buffer[0], buffer[1]);
    raw_mag_y = AK8963_ToInt16_LE(buffer[2], buffer[3]);
    raw_mag_z = AK8963_ToInt16_LE(buffer[4], buffer[5]);

    data->mag.raw_x =
        (float)raw_mag_x * AK8963_MAG_SCALE_16BIT * ak8963_adj_x;

    data->mag.raw_y =
        (float)raw_mag_y * AK8963_MAG_SCALE_16BIT * ak8963_adj_y;

    data->mag.raw_z =
        (float)raw_mag_z * AK8963_MAG_SCALE_16BIT * ak8963_adj_z;

    data->mag.UpdateTime = HAL_GetTick() * 1000;

    return 1U;
}


/*==================================================
 * PUBLIC FONKSİYONLAR
 *==================================================*/

uint8_t MPU9250_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t who_am_i;
    uint8_t gyro_config;
    uint8_t accel_config;
    uint8_t accel_config2;

    if(i2c_handle == NULL)
    {
        mpu9250_ready = 0U;
        ak8963_ready = 0U;
        return 0U;
    }

    mpu9250_i2c = i2c_handle;
    mpu9250_ready = 0U;
    ak8963_ready = 0U;

    HAL_Delay(10U);

    /*
     * Sensör gerçekten MPU9250 mi?
     */
    if(MPU9250_ReadRegister(MPU9250_REG_WHO_AM_I,
                            &who_am_i,
                            1U) == 0U)
    {
        return 0U;
    }

    /* 
     * Çin malı klonlarda (MPU6500 vb.) kimlik numarası farklı olabiliyor (0x70, 0x71, 0x73 vb.).
     * Sensörün I2C adresi 0x68 olarak doğrulandığı için bu testi devre dışı bırakıyoruz.
     */
    /*
    if(who_am_i != MPU9250_WHO_AM_I_VALUE)
    {
        return 0U;
    }
    */

    /*
     * Cihazı resetle.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_PWR_MGMT_1,
                             MPU9250_DEVICE_RESET) == 0U)
    {
        return 0U;
    }

    HAL_Delay(100U);

    /*
     * Uyandır ve clock kaynağı olarak PLL seç.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_PWR_MGMT_1,
                             MPU9250_CLK_PLL) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    /*
     * Accelerometer ve gyroscope eksenlerinin hepsini aktif bırak.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_PWR_MGMT_2,
                             0x00U) == 0U)
    {
        return 0U;
    }

    /*
     * FIFO, internal I2C master gibi ekstra özellikleri kapalı tut.
     * AK8963'e doğrudan ulaşmak için internal I2C master kapalı kalmalı.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_USER_CTRL,
                             0x00U) == 0U)
    {
        return 0U;
    }

    /*
     * Gyro DLPF ayarı. (Titreşimi kesmek için 41Hz'den 20Hz'e düşürüldü)
     */
    if(MPU9250_WriteRegister(MPU9250_REG_CONFIG,
                             MPU9250_DLPF_CFG_20HZ) == 0U)
    {
        return 0U;
    }

    /*
     * Sample rate yaklaşık 200 Hz.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_SMPLRT_DIV,
                             MPU9250_SMPLRT_DIV_200HZ) == 0U)
    {
        return 0U;
    }

    /*
     * Gyroscope full-scale = ±250 dps (Default state of clones).
     *
     * GYRO_FS_SEL bitleri 4:3 konumunda.
     */
    gyro_config =
        (uint8_t)(MPU9250_GYRO_FS_250DPS << 3);

    if(MPU9250_WriteRegister(MPU9250_REG_GYRO_CONFIG,
                             gyro_config) == 0U)
    {
        return 0U;
    }

    /*
     * Accelerometer full-scale = ±2g (Default state of clones).
     *
     * ACCEL_FS_SEL bitleri 4:3 konumunda.
     */
    accel_config =
        (uint8_t)(MPU9250_ACCEL_FS_2G << 3);

    if(MPU9250_WriteRegister(MPU9250_REG_ACCEL_CONFIG,
                             accel_config) == 0U)
    {
        return 0U;
    }

    /*
     * Accelerometer DLPF ayarı. (Titreşimi kesmek için 41Hz'den 20Hz'e düşürüldü)
     */
    accel_config2 = MPU9250_ACCEL_DLPF_20HZ;

    if(MPU9250_WriteRegister(MPU9250_REG_ACCEL_CONFIG2,
                             accel_config2) == 0U)
    {
        return 0U;
    }

    /*
     * AK8963 magnetometer için bypass mode aç.
     */
    if(MPU9250_WriteRegister(MPU9250_REG_INT_PIN_CFG,
                             MPU9250_BYPASS_EN) == 0U)
    {
        return 0U;
    }

    HAL_Delay(10U);

    /*
     * Magnetometer init.
     *
     * Magnetometer başarısız olsa bile accel + gyro çalışmaya devam etsin.
     * Bu yüzden AK8963_Init() sonucuyla MPU9250_Init() fail edilmiyor.
     */
    (void)AK8963_Init();

    HAL_Delay(20U);

    mpu9250_ready = 1U;

    return 1U;
}


uint8_t MPU9250_Read(DataCenter *data)
{
    uint8_t buffer[14];

    int16_t raw_accel_x;
    int16_t raw_accel_y;
    int16_t raw_accel_z;

    int16_t raw_gyro_x;
    int16_t raw_gyro_y;
    int16_t raw_gyro_z;

    if(data == NULL)
    {
        return 0U;
    }
    if(mpu9250_ready == 0U)
    {
        return 0U;
    }

       if(MPU9250_ReadRegister(MPU9250_REG_ACCEL_XOUT_H,
                            buffer,
                            14U) == 0U)
    {
        return 0U;
    }

    raw_accel_x = MPU9250_ToInt16(buffer[0],  buffer[1]);
    raw_accel_y = MPU9250_ToInt16(buffer[2],  buffer[3]);
    raw_accel_z = MPU9250_ToInt16(buffer[4],  buffer[5]);

    /*
     * buffer[6] ve buffer[7] sıcaklık verisi.
     * Şimdilik kullanmıyoruz.
     */

    raw_gyro_x = MPU9250_ToInt16(buffer[8],  buffer[9]);
    raw_gyro_y = MPU9250_ToInt16(buffer[10], buffer[11]);
    raw_gyro_z = MPU9250_ToInt16(buffer[12], buffer[13]);

    /*
     * Raw accel -> g
     */
    data->acc.raw_x = (float)raw_accel_x / MPU9250_ACCEL_SCALE_2G;
    data->acc.raw_y = (float)raw_accel_y / MPU9250_ACCEL_SCALE_2G;
    data->acc.raw_z = (float)raw_accel_z / MPU9250_ACCEL_SCALE_2G;

    // Apply Calibration Profile (Acc) and convert to m/s^2
    data->acc.calibrated_x = (data->acc.raw_x - data->calibProfile.acc_bias_x) * data->calibProfile.acc_scale_x * GRAVITY_MSS;
    data->acc.calibrated_y = (data->acc.raw_y - data->calibProfile.acc_bias_y) * data->calibProfile.acc_scale_y * GRAVITY_MSS;
    data->acc.calibrated_z = (data->acc.raw_z - data->calibProfile.acc_bias_z) * data->calibProfile.acc_scale_z * GRAVITY_MSS;

    /*
     * Raw gyro -> dps
     */
    data->gyro.raw_x = (float)raw_gyro_x / MPU9250_GYRO_SCALE_250DPS;
    data->gyro.raw_y = (float)raw_gyro_y / MPU9250_GYRO_SCALE_250DPS;
    data->gyro.raw_z = (float)raw_gyro_z / MPU9250_GYRO_SCALE_250DPS;

    // Apply Calibration Profile (Gyro)
    data->gyro.calibrated_x = (data->gyro.raw_x - data->calibProfile.gyro_bias_x) * data->calibProfile.gyro_scale_x;
    data->gyro.calibrated_y = (data->gyro.raw_y - data->calibProfile.gyro_bias_y) * data->calibProfile.gyro_scale_y;
    data->gyro.calibrated_z = (data->gyro.raw_z - data->calibProfile.gyro_bias_z) * data->calibProfile.gyro_scale_z;

    /*
     * Magnetometer klon sensörlerde olmadığı için okuma iptal edildi.
     * SystemCore_Init içindeki memset ile 0 kalması sağlanacaktır.
     */
    // (void)AK8963_Read(data);

    data->acc.UpdateTime = HAL_GetTick() * 1000;
    data->gyro.UpdateTime = HAL_GetTick() * 1000;

    return 1U;
}


uint8_t MPU9250_IsReady(void)
{
    return mpu9250_ready;
}


uint8_t MPU9250_IsMagReady(void)
{
    return ak8963_ready;
}
