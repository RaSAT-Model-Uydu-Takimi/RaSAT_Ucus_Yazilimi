#include "M1.0_SensorReaderCore.h"
#include "M1.1_MPU9250.h"
#include "M1.2_BMP280.h"
#include "M0.1_FilterConfig.h"
#include <string.h>

extern uint8_t discovered_i2c_addr; // main.c'den alınan global değişken (Eğer hala lazımsa)
extern uint8_t watchDog;
extern void Error_Handler(void);

void SensorReaderCore_Init(I2C_HandleTypeDef *hi2c, DataCenter *dc) {
    // 0. İlk kurulumda tüm DataCenter'ı sıfırla (Çöp verileri temizle)
    if(dc != NULL) {
        memset(dc, 0, sizeof(DataCenter));
        
        // Kalibrasyon profiline varsayılan değerleri (makroları) yükle
        dc->calibProfile.acc_bias_x = ACC_BIAS_X;
        dc->calibProfile.acc_bias_y = ACC_BIAS_Y;
        dc->calibProfile.acc_bias_z = ACC_BIAS_Z;
        dc->calibProfile.acc_scale_x = ACC_SCALE_X;
        dc->calibProfile.acc_scale_y = ACC_SCALE_Y;
        dc->calibProfile.acc_scale_z = ACC_SCALE_Z;
        dc->calibProfile.acc_noise_x = ACC_NOISE_X;
        dc->calibProfile.acc_noise_y = ACC_NOISE_Y;
        dc->calibProfile.acc_noise_z = ACC_NOISE_Z;
        
        dc->calibProfile.gyro_bias_x = GYRO_BIAS_X;
        dc->calibProfile.gyro_bias_y = GYRO_BIAS_Y;
        dc->calibProfile.gyro_bias_z = GYRO_BIAS_Z;
        dc->calibProfile.gyro_scale_x = GYRO_SCALE_X;
        dc->calibProfile.gyro_scale_y = GYRO_SCALE_Y;
        dc->calibProfile.gyro_scale_z = GYRO_SCALE_Z;
        dc->calibProfile.gyro_noise_x = GYRO_NOISE_X;
        dc->calibProfile.gyro_noise_y = GYRO_NOISE_Y;
        dc->calibProfile.gyro_noise_z = GYRO_NOISE_Z;
        
        // Yeni eklenen sensör varsayılanları
        dc->calibProfile.mag_bias_x = MAG_BIAS_X;
        dc->calibProfile.mag_bias_y = MAG_BIAS_Y;
        dc->calibProfile.mag_bias_z = MAG_BIAS_Z;
        dc->calibProfile.mag_scale_x = MAG_SCALE_X;
        dc->calibProfile.mag_scale_y = MAG_SCALE_Y;
        dc->calibProfile.mag_scale_z = MAG_SCALE_Z;
        dc->calibProfile.mag_noise_x = MAG_NOISE_X;
        dc->calibProfile.mag_noise_y = MAG_NOISE_Y;
        dc->calibProfile.mag_noise_z = MAG_NOISE_Z;
        
        dc->calibProfile.baro_press_bias = BARO_PRESS_BIAS;
        dc->calibProfile.baro_press_scale = BARO_PRESS_SCALE;
        dc->calibProfile.baro_press_noise = BARO_PRESS_NOISE;
        dc->calibProfile.baro_temp_bias = BARO_TEMP_BIAS;
        dc->calibProfile.baro_temp_scale = BARO_TEMP_SCALE;
        dc->calibProfile.baro_temp_noise = BARO_TEMP_NOISE;
        
        dc->calibProfile.gps_lat_bias = GPS_LAT_BIAS;
        dc->calibProfile.gps_lat_scale = GPS_LAT_SCALE;
        dc->calibProfile.gps_lon_bias = GPS_LON_BIAS;
        dc->calibProfile.gps_lon_scale = GPS_LON_SCALE;
        dc->calibProfile.gps_alt_bias = GPS_ALT_BIAS;
        dc->calibProfile.gps_alt_scale = GPS_ALT_SCALE;
        dc->calibProfile.gps_noise = GPS_NOISE;
        
        dc->calibProfile.batt_volt_bias = BATT_VOLT_BIAS;
        dc->calibProfile.batt_volt_scale = BATT_VOLT_SCALE;
        dc->calibProfile.batt_volt_noise = BATT_VOLT_NOISE;
        dc->calibProfile.batt_curr_bias = BATT_CURR_BIAS;
        dc->calibProfile.batt_curr_scale = BATT_CURR_SCALE;
        dc->calibProfile.batt_curr_noise = BATT_CURR_NOISE;
    }

    // 1. Önce MPU9250 Init yapılır (Bu sayede I2C Bypass modu açılır)
    if(MPU9250_Init(hi2c) == 0U){
        watchDog = 1;
        Error_Handler();
    }

    // 2. BMP280 Barometre Init (Bypass üzerinden geçerek ulaşıyoruz)
    if(BMP280_Init(hi2c) == 0U){
        // Barometre hayati olmayabilir, ama watchdog eklenebilir.
    }

    // 3. Bypass açıldıktan sonra hattaki cihazları tarıyoruz.
    uint8_t found_devices[10] = {0};
    uint8_t device_count = 0;

    for (uint16_t i = 1; i < 128; i++) {
        if (HAL_I2C_IsDeviceReady(hi2c, (i << 1), 3, 10) == HAL_OK) {
            discovered_i2c_addr = i; // Geriye dönük uyumluluk için
            if(device_count < 10) {
                found_devices[device_count] = i; 
                device_count++;
            }
        }
    }
}

void SensorReaderCore_Update(DataCenter *dc, uint32_t current_time_ms) {
    static uint32_t last_imu_time = 0; 
    static uint32_t last_baro_time = 0;
    static uint8_t first_run = 1; // İlk döngüde okumayı garanti etmek için

    // Sadece zamanı geldiyse veya ilk okumaysa IMU'yu oku
    if (first_run || (current_time_ms - last_imu_time) >= IMU_PERIOD_MS) {
        last_imu_time = current_time_ms;
        MPU9250_Read(dc);
    }

    // Sadece zamanı geldiyse veya ilk okumaysa Barometreyi oku
    if (first_run || (current_time_ms - last_baro_time) >= BARO_PERIOD_MS) {
        last_baro_time = current_time_ms;
        if (BMP280_Read(dc) == 0U) {
            // Barometre okuması başarısız oldu. Sensör modülündeki MPU9250'nin 
            // Bypass kapısı bir hata yüzünden kapanmış olabilir. Tekrar açmayı deniyoruz.
            MPU9250_EnableBypass();
            // Barometre reset yemiş ve uyku moduna geçmiş olabilir, onu da uyandır.
            BMP280_ReInit();
        }
    }
    
    first_run = 0;
}

void I2C_BusRecovery(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance != I2C1) return; // Sadece I2C1 (PB6/PB7) icin ayarli
    
    // 1. I2C donanimini kapat
    HAL_I2C_DeInit(hi2c);
    
    // 2. Pinleri gecici olarak GPIO Output yap (SCL=PB6, SDA=PB7)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // Open Drain (Pull-up direncleri devrede)
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 3. 9 Adet Clock Pulsi uret
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); // SDA High
    for (int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);   // SCL High
        HAL_Delay(1);                                         // Kucuk gecikme
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); // SCL Low
        HAL_Delay(1);
    }
    
    // STOP Condition (SCL=High, ardindan SDA=High)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); // SCL High
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); // SDA High
    HAL_Delay(1);
    
    // 4. Pinleri tekrar I2C Alternate Function moduna dondur
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // F407 icin I2C1 AF kodu AF4'tur
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 5. I2C donanimini yeniden baslat
    HAL_I2C_Init(hi2c);
}
