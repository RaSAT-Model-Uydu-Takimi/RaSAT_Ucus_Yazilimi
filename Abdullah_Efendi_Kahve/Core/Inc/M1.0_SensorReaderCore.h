#ifndef INC_M1_0_SENSORREADERCORE_H_
#define INC_M1_0_SENSORREADERCORE_H_

#include "main.h"
#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sensör okuma periyotları (Yazılımsal Timer için)
#define IMU_PERIOD_MS    10    // 200 Hz
#define BARO_PERIOD_MS   20   // 50 Hz

/*
 * Fonksiyon: SensorReaderCore_Init
 * Görevi: Tüm sensörleri (MPU9250, BMP280 vb.) başlatır.
 *         I2C Bypass modunu açar ve hattı tarar.
 */
void SensorReaderCore_Init(I2C_HandleTypeDef *hi2c, DataCenter *dc);

/*
 * Fonksiyon: SensorReaderCore_Update
 * Görevi: Zamanlayıcı (Soft-Timer) mantığı ile sadece okuma zamanı 
 *         gelen sensörleri okur. I2C hattını gereksiz meşgul etmez.
 */
void SensorReaderCore_Update(DataCenter *dc, uint32_t current_time_ms);

/*
 * Fonksiyon: I2C_BusRecovery
 * Gorevi: I2C hatti SDA stuck-low yuzunden kilitlendiginde
 *         9 saat palsi ureterek sensoru serbest biraktirir ve hatti kurtarir.
 */
void I2C_BusRecovery(I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif /* INC_M1_0_SENSORREADERCORE_H_ */
