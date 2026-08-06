/*
 * sensors.c
 *
 *  Created on: Jul 4, 2026
 *      Author: umut
 */

/*
 * sensors.c
 *
 * Sensor manager / fusion layer
 *
 * Görev:
 * - BMP280, MPU9250 ve GPS driverlarını çağırmak
 * - Basınçtan irtifa hesaplamak
 * - İrtifa değişiminden dikey hız hesaplamak
 * - Flight control için tek bir SensorData yapısı üretmek
 */

#include "sensors.h"

#include "bmp280.h"
#include "mpu9250.h"
#include "gps.h"

#include <string.h>
#include <math.h>


/*==================================================
 * AYARLAR
 *==================================================*/

/*
 * BMP280 irtifa filtresi.
 * Büyük değer: hızlı tepki, daha fazla gürültü
 * Küçük değer: yavaş tepki, daha temiz sinyal
 */
#define ALTITUDE_FILTER_ALPHA          0.25f

/*
 * Dikey hız filtresi.
 */
#define VERTICAL_SPEED_FILTER_ALPHA    0.30f

/*
 * Eğer zaman hesabı anlık bozulursa kullanılacak varsayılan dt.
 */
#define SENSORS_DEFAULT_DT_S           0.005f

/*
 * İniş algılama eşikleri.
 * Bu değerler test sonrası ayarlanabilir.
 */
#define LANDING_ALT_THRESHOLD_M        0.70f
#define LANDING_VEL_THRESHOLD_MPS      0.40f
#define LANDING_CONFIRM_MS             1000U

/*
 * Araç yerde/durağan mı kontrolü için accelerometer büyüklüğü.
 * Sabit duran sistemde accel magnitude yaklaşık 1g olur.
 */
#define ACCEL_STILL_MIN_G2             0.49f   /* 0.7g ^ 2 */
#define ACCEL_STILL_MAX_G2             1.69f   /* 1.3g ^ 2 */


/*==================================================
 * STATIC DEĞİŞKENLER
 *==================================================*/

static SensorData sensor_data;

static uint8_t pressure_reference_set = 0U;
static float pressure_reference_pa = 0.0f;

static uint8_t altitude_filter_initialized = 0U;
static uint8_t velocity_filter_initialized = 0U;

static float filtered_altitude_m = 0.0f;
static float previous_altitude_m = 0.0f;
static float filtered_vertical_velocity_mps = 0.0f;

static uint32_t last_altitude_update_ms = 0U;

static uint8_t landing_candidate_active = 0U;
static uint32_t landing_candidate_start_ms = 0U;


/*==================================================
 * YARDIMCI FONKSİYONLAR
 *==================================================*/

static float Sensors_LowPass(float previous,
                             float input,
                             float alpha)
{
    return previous + alpha * (input - previous);
}


static float Sensors_PressureToAltitude(float pressure_pa,
                                        float reference_pressure_pa)
{
    float ratio;

    if((pressure_pa <= 0.0f) || (reference_pressure_pa <= 0.0f))
    {
        return 0.0f;
    }

    ratio = pressure_pa / reference_pressure_pa;

    /*
     * Standart barometrik yaklaşık formül:
     *
     * altitude = 44330 * (1 - (P / P0)^0.19029495)
     *
     * Burada P0, sistem açıldığında alınan referans basınçtır.
     * Bu yüzden altitude_m göreli irtifadır.
     */
    return 44330.0f * (1.0f - powf(ratio, 0.19029495f));
}


static float Sensors_GetDeltaTimeSeconds(uint32_t now_ms)
{
    uint32_t dt_ms;
    float dt_s;

    if(last_altitude_update_ms == 0U)
    {
        last_altitude_update_ms = now_ms;
        return SENSORS_DEFAULT_DT_S;
    }

    dt_ms = now_ms - last_altitude_update_ms;
    last_altitude_update_ms = now_ms;

    if(dt_ms == 0U)
    {
        return SENSORS_DEFAULT_DT_S;
    }

    dt_s = (float)dt_ms / 1000.0f;

    /*
     * Debug pause / breakpoint sonrası büyük dt gelirse türev bozulmasın.
     */
    if(dt_s > 1.0f)
    {
        dt_s = SENSORS_DEFAULT_DT_S;
    }

    return dt_s;
}


static void Sensors_UpdateAltitudeAndVelocity(float pressure_pa)
{
    uint32_t now_ms;
    float dt_s;
    float raw_altitude_m;
    float raw_vertical_velocity_mps;

    now_ms = HAL_GetTick();

    /*
     * İlk geçerli basınç referans alınır.
     * Sistem yerde açılıyorsa bu yaklaşık 0 m referansı olur.
     */
    if(pressure_reference_set == 0U)
    {
        pressure_reference_pa = pressure_pa;
        pressure_reference_set = 1U;

        filtered_altitude_m = 0.0f;
        previous_altitude_m = 0.0f;
        filtered_vertical_velocity_mps = 0.0f;

        altitude_filter_initialized = 1U;
        velocity_filter_initialized = 1U;

        last_altitude_update_ms = now_ms;

        sensor_data.altitude_m = 0.0f;
        sensor_data.vertical_velocity_mps = 0.0f;

        return;
    }

    raw_altitude_m =
        Sensors_PressureToAltitude(pressure_pa,
                                   pressure_reference_pa);

    if(altitude_filter_initialized == 0U)
    {
        filtered_altitude_m = raw_altitude_m;
        previous_altitude_m = raw_altitude_m;
        altitude_filter_initialized = 1U;
    }
    else
    {
        filtered_altitude_m =
            Sensors_LowPass(filtered_altitude_m,
                            raw_altitude_m,
                            ALTITUDE_FILTER_ALPHA);
    }

    dt_s = Sensors_GetDeltaTimeSeconds(now_ms);

    raw_vertical_velocity_mps =
        (filtered_altitude_m - previous_altitude_m) / dt_s;

    previous_altitude_m = filtered_altitude_m;

    if(velocity_filter_initialized == 0U)
    {
        filtered_vertical_velocity_mps = raw_vertical_velocity_mps;
        velocity_filter_initialized = 1U;
    }
    else
    {
        filtered_vertical_velocity_mps =
            Sensors_LowPass(filtered_vertical_velocity_mps,
                            raw_vertical_velocity_mps,
                            VERTICAL_SPEED_FILTER_ALPHA);
    }

    sensor_data.altitude_m = filtered_altitude_m;
    sensor_data.vertical_velocity_mps = filtered_vertical_velocity_mps;
}


static void Sensors_UpdateLandingDetection(void)
{
    uint32_t now_ms;
    float accel_mag2;

    uint8_t altitude_low;
    uint8_t velocity_low;
    uint8_t accel_still;
    uint8_t landing_condition;

    now_ms = HAL_GetTick();

    if((sensor_data.barometer_valid == 0U) ||
       (sensor_data.imu_valid == 0U))
    {
        landing_candidate_active = 0U;
        return;
    }

    altitude_low =
        (sensor_data.altitude_m <= LANDING_ALT_THRESHOLD_M) ? 1U : 0U;

    velocity_low =
        (fabsf(sensor_data.vertical_velocity_mps) <= LANDING_VEL_THRESHOLD_MPS) ? 1U : 0U;

    accel_mag2 =
        (sensor_data.accel_x_g * sensor_data.accel_x_g) +
        (sensor_data.accel_y_g * sensor_data.accel_y_g) +
        (sensor_data.accel_z_g * sensor_data.accel_z_g);

    accel_still =
        ((accel_mag2 >= ACCEL_STILL_MIN_G2) &&
         (accel_mag2 <= ACCEL_STILL_MAX_G2)) ? 1U : 0U;

    landing_condition =
        (altitude_low && velocity_low && accel_still) ? 1U : 0U;

    if(landing_condition != 0U)
    {
        if(landing_candidate_active == 0U)
        {
            landing_candidate_active = 1U;
            landing_candidate_start_ms = now_ms;
        }

        if((now_ms - landing_candidate_start_ms) >= LANDING_CONFIRM_MS)
        {
            sensor_data.landed_detected = 1U;
        }
    }
    else
    {
        landing_candidate_active = 0U;
    }
}


/*==================================================
 * PUBLIC FONKSİYONLAR
 *==================================================*/

uint8_t Sensors_Init(I2C_HandleTypeDef *i2c_handle,
                     UART_HandleTypeDef *gps_uart_handle)
{
    uint8_t bmp_ok;
    uint8_t imu_ok;
    uint8_t gps_ok;
    uint8_t gps_rx_ok;

    memset(&sensor_data, 0, sizeof(sensor_data));

    pressure_reference_set = 0U;
    pressure_reference_pa = 0.0f;

    altitude_filter_initialized = 0U;
    velocity_filter_initialized = 0U;

    filtered_altitude_m = 0.0f;
    previous_altitude_m = 0.0f;
    filtered_vertical_velocity_mps = 0.0f;

    last_altitude_update_ms = 0U;

    landing_candidate_active = 0U;
    landing_candidate_start_ms = 0U;

    bmp_ok = BMP280_Init(i2c_handle);
    imu_ok = MPU9250_Init(i2c_handle);

    gps_ok = GPS_Init(gps_uart_handle);

    gps_rx_ok = 0U;

    if(gps_ok != 0U)
    {
        gps_rx_ok = GPS_StartReceiveIT();
    }

    sensor_data.bmp280_ready = bmp_ok;
    sensor_data.mpu9250_ready = imu_ok;
    sensor_data.mag_ready = MPU9250_IsMagReady();

    sensor_data.gps_ready = gps_ok;
    sensor_data.gps_receive_started = gps_rx_ok;

    /*
     * Flight control için minimum kritik sensörler:
     * - BMP280: altitude + vertical velocity
     * - MPU9250: hareket/eğim/landing desteği
     *
     * GPS şu aşamada data_valid için zorunlu değil.
     */
    if((bmp_ok != 0U) && (imu_ok != 0U))
    {
        return 1U;
    }

    return 0U;
}


void Sensors_Update(void)
{
    BMP280_Data bmp;
    MPU9250_Data imu;
    GPS_Data gps;

    /*
     * BMP280 update
     */
    if((BMP280_Read(&bmp) != 0U) && (bmp.valid != 0U))
    {
        sensor_data.pressure_pa = bmp.pressure_pa;
        sensor_data.temperature_c = bmp.temperature_c;
        sensor_data.barometer_valid = 1U;

        Sensors_UpdateAltitudeAndVelocity(bmp.pressure_pa);
    }
    else
    {
        sensor_data.barometer_valid = 0U;
    }

    /*
     * MPU9250 update
     */
    if((MPU9250_Read(&imu) != 0U) && (imu.valid != 0U))
    {
        sensor_data.accel_x_g = imu.accel_x_g;
        sensor_data.accel_y_g = imu.accel_y_g;
        sensor_data.accel_z_g = imu.accel_z_g;

        sensor_data.gyro_x_dps = imu.gyro_x_dps;
        sensor_data.gyro_y_dps = imu.gyro_y_dps;
        sensor_data.gyro_z_dps = imu.gyro_z_dps;

        sensor_data.mag_x_uT = imu.mag_x_uT;
        sensor_data.mag_y_uT = imu.mag_y_uT;
        sensor_data.mag_z_uT = imu.mag_z_uT;

        sensor_data.imu_valid = 1U;
        sensor_data.mag_valid = imu.mag_valid;
    }
    else
    {
        sensor_data.imu_valid = 0U;
        sensor_data.mag_valid = 0U;
    }

    sensor_data.mag_ready = MPU9250_IsMagReady();

    /*
     * GPS update
     *
     * GPS_Read() son bilinen GPS datasını kopyalar.
     * gps.new_data değeri son okumadan beri yeni NMEA cümlesi gelip gelmediğini gösterir.
     */
    if(GPS_Read(&gps) != 0U)
    {
        sensor_data.gps_latitude_deg = gps.latitude_deg;
        sensor_data.gps_longitude_deg = gps.longitude_deg;

        sensor_data.gps_altitude_m = gps.altitude_m;
        sensor_data.gps_speed_mps = gps.speed_mps;
        sensor_data.gps_course_deg = gps.course_deg;
        sensor_data.gps_hdop = gps.hdop;

        sensor_data.gps_satellites = gps.satellites;
        sensor_data.gps_fix_quality = gps.fix_quality;
        sensor_data.gps_fix_valid = gps.fix_valid;
        sensor_data.gps_new_data = gps.new_data;
    }
    else
    {
        sensor_data.gps_new_data = 0U;
    }

    /*
     * Landing detection
     */
    Sensors_UpdateLandingDetection();

    /*
     * Flight control için genel data_valid.
     * GPS burada zorunlu tutulmuyor.
     */
    if((sensor_data.barometer_valid != 0U) &&
       (sensor_data.imu_valid != 0U))
    {
        sensor_data.data_valid = 1U;
    }
    else
    {
        sensor_data.data_valid = 0U;
    }
}


SensorData Sensors_GetData(void)
{
    return sensor_data;
}


void Sensors_ToFlightControlInput(FlightControlInput *fc_input)
{
    if(fc_input == NULL)
    {
        return;
    }

    fc_input->altitude_m = sensor_data.altitude_m;
    fc_input->vertical_velocity_mps = sensor_data.vertical_velocity_mps;

    fc_input->data_valid = sensor_data.data_valid;

    fc_input->payload_separated = sensor_data.payload_separated;
    fc_input->landed_detected = sensor_data.landed_detected;
    fc_input->apam_request = sensor_data.apam_request;
}


void Sensors_UART_RxCpltCallback(UART_HandleTypeDef *uart_handle)
{
    GPS_UART_RxCpltCallback(uart_handle);
}


void Sensors_ResetAltitudeReference(void)
{
    pressure_reference_set = 0U;
    pressure_reference_pa = 0.0f;

    altitude_filter_initialized = 0U;
    velocity_filter_initialized = 0U;

    filtered_altitude_m = 0.0f;
    previous_altitude_m = 0.0f;
    filtered_vertical_velocity_mps = 0.0f;

    last_altitude_update_ms = 0U;

    sensor_data.altitude_m = 0.0f;
    sensor_data.vertical_velocity_mps = 0.0f;

    sensor_data.landed_detected = 0U;

    landing_candidate_active = 0U;
    landing_candidate_start_ms = 0U;
}


void Sensors_SetPayloadSeparated(uint8_t separated)
{
    sensor_data.payload_separated = separated ? 1U : 0U;
}


void Sensors_SetExternalApamRequest(uint8_t request)
{
    sensor_data.apam_request = request ? 1U : 0U;
}


uint8_t Sensors_IsReady(void)
{
    return sensor_data.data_valid;
}
