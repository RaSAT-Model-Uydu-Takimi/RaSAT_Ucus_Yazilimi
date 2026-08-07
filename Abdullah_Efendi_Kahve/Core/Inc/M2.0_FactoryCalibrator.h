#ifndef INC_FACTORY_CALIBRATOR_H_
#define INC_FACTORY_CALIBRATOR_H_

#include "M0.0_DataCenter.h"
#include "M0.1_FilterConfig.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*  6-Nokta Kalibrasyon Durum Makinesi (State Machine)                         */
/* ========================================================================== */
typedef enum {
    CALIB_STATE_IDLE = 0,       // Kalibrasyon kapalÃ„Â±, normal uÃƒÂ§uÃ…Å¸/bekleme modu
    CALIB_STATE_TRANSITION,     // 10 Saniyelik geÃƒÂ§iÃ…Å¸ / cihaza pozisyon verme sÃƒÂ¼resi
    CALIB_STATE_MEASURING,      // 20 Saniyelik ivmeÃƒÂ¶lÃƒÂ§er / jiroskop ÃƒÂ¶lÃƒÂ§ÃƒÂ¼m sÃƒÂ¼resi
    CALIB_STATE_FINISHED        // TÃƒÂ¼m 6 nokta tamamlandÃ„Â±, hesaplamalar bitti
} CalibState_e;

/* ========================================================================== */
/*  Kalibrasyon Veri YapÃ„Â±sÃ„Â± (Struct)                                           */
/* ========================================================================== */
typedef struct {
    // 1. Ana Kalibrasyon Ãƒâ€¡Ã„Â±ktÃ„Â±larÃ„Â± (Config dosyasÃ„Â±na kopyalanacaklar)
    float acc_x_bias, acc_y_bias, acc_z_bias;
    float acc_x_scale, acc_y_scale, acc_z_scale;
    float acc_thermal_drift;

    float gyro_x_bias, gyro_y_bias, gyro_z_bias;
    float gyro_x_scale, gyro_y_scale, gyro_z_scale;

    float mag_x_bias, mag_y_bias, mag_z_bias;
    float mag_x_scale, mag_y_scale, mag_z_scale;

    float baro_press_bias, baro_press_scale;
    float baro_temp_bias, baro_temp_scale;

    float sea_level_pa;
    float temp_ref_c;
    float ref_ground_altitude_m;

    // 2. Montaj YamukluÃ„Å¸u Giderici (Board Alignment / Trim)
    float board_pitch_offset;
    float board_roll_offset;

    // 3. Welford's Variance GÃƒÂ¼rÃƒÂ¼ltÃƒÂ¼ Ãƒâ€“lÃƒÂ§ÃƒÂ¼mleri (EKF/UKF iÃƒÂ§in Q ve R matrisleri)
    float acc_variance[3];   // R Matrisi iÃƒÂ§in (X, Y, Z gÃƒÂ¼rÃƒÂ¼ltÃƒÂ¼sÃƒÂ¼)
    float gyro_variance[3];  // Q Matrisi iÃƒÂ§in (X, Y, Z gÃƒÂ¼rÃƒÂ¼ltÃƒÂ¼sÃƒÂ¼)

    // 4. 6-Nokta Kalibrasyon Motoru Ã„Â°ÃƒÂ§ DeÃ„Å¸iÃ…Å¸kenleri (State Machine)
    CalibState_e state;
    uint8_t current_step;       // 0'dan 5'e (6 eksen)
    uint32_t timer_start_ms;    // GeÃƒÂ§iÃ…Å¸ ve Ãƒâ€“lÃƒÂ§ÃƒÂ¼m sÃƒÂ¼relerini tutmak iÃƒÂ§in
    uint32_t sample_count;      // O anki adÃ„Â±mdaki kÃƒÂ¼mÃƒÂ¼latif ÃƒÂ¶rnek sayÃ„Â±sÃ„Â±

    // 6-Nokta Ortalama ToplayÃ„Â±cÃ„Â±lar (Recursive Mean Ã„Â°ÃƒÂ§in)
    float step_averages[6][3];  // 6 AdÃ„Â±mÃ„Â±n herbirindeki 3 eksen ivme ortalamasÃ„Â±
    float acc_sum[3];
    float acc_sum_sq[3];
    float gyro_sum[3];
    float gyro_sum_sq[3];

    float gyro_avg[3];          // TÃƒÂ¼m adÃ„Â±mlarda jiroskop hep 0 olmalÃ„Â± (Genel ortalama)
    
    // Welford's M2 toplayÃ„Â±cÃ„Â±larÃ„Â± (Varyans hesabÃ„Â± iÃƒÂ§in)
    float acc_M2[3];
    float gyro_avg_accum[3];    // Jiroskop tÃƒÂ¼m 6 adÃ„Â±m boyunca ortalanÃ„Â±r (ÃƒÂ§ÃƒÂ¼nkÃƒÂ¼ sabit)
    float gyro_M2[3];

    // 5. Geri Bildirim FonksiyonlarÃ„Â± (Callbacks - LED, Buzzer vb.)
    void (*on_transition_start)(uint8_t step); 
    void (*on_measuring_start)(uint8_t step);
    void (*on_calibration_finished)(void);

} SensorCalib_t;

/* ========================================================================== */
/*  Fonksiyon Prototipleri                                                     */
/* ========================================================================== */

// BaÃ…Å¸langÃ„Â±ÃƒÂ§ deÃ„Å¸erlerini atar (M0.1 Config'ten ÃƒÂ§eker)
void SensorCalib_Init(SensorCalib_t *calib);

// Ham veriyi (rawValue), Scale ve Bias kullanarak kalibreli (calibratedValue) veriye ÃƒÂ§evirir
void SensorCalibApply(const SensorCalib_t *calib, DataCenter *dataC);

// Board Alignment (Montaj YamukluÃ„Å¸u) Offset'lerini geÃƒÂ§erli aÃƒÂ§Ã„Â±lardan ÃƒÂ§Ã„Â±karÃ„Â±r
void SensorCalib_ApplyBoardAlignment(const SensorCalib_t *calib, float *pitch, float *roll);

// Masaya dÃƒÂ¼z konulduÃ„Å¸unda anlÃ„Â±k aÃƒÂ§Ã„Â±larÃ„Â± okuyup Offset olarak kaydeder (Trim)
void SensorCalib_SetLevel(SensorCalib_t *calib, float current_pitch, float current_roll);

// 6-NoktalÃ„Â± GeliÃ…Å¸miÃ…Å¸ Kalibrasyon Makinesini BaÃ…Å¸latÃ„Â±r
void SensorCalib_StartAdvanced(SensorCalib_t *calib);

// Kalibrasyon modundayken zamanÄ± ve sensÃ¶rleri takip eder (Filtre dÃ¶ngÃ¼sÃ¼ne konulmalÄ±)
void SensorCalib_ProcessAdvanced(SensorCalib_t *calib, DataCenter *dataC, uint32_t current_time_ms);

// HÄ±zlÄ± Kalibrasyon (Quick Calib) - Sadece sabit dururken ivme/gyro offset Ã¶lÃ§er
void SensorCalib_QuickRun(SensorCalib_t *calib, DataCenter *data, uint32_t current_time_ms);


#ifdef __cplusplus
}
#endif

#endif /* INC_FACTORY_CALIBRATOR_H_ */


