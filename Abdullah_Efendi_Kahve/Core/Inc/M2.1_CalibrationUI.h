#ifndef INC_M2_1_CALIBRATION_UI_H_
#define INC_M2_1_CALIBRATION_UI_H_

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "M2.0_FactoryCalibrator.h"

typedef enum {
    UI_STATE_IDLE,
    UI_STATE_TRANSITION,
    UI_STATE_MEASURING,
    UI_STATE_FINISHED
} Calibration_UI_State_e;

/*
 * Fonksiyon: CalibrationUI_Init
 * Görevi: STM32F407 üzerindeki D portunu aktif eder ve varsayılan LED pinlerini (12, 13, 14, 15) ayarlar.
 */
void CalibrationUI_Init(void);

/*
 * Fonksiyon: CalibrationUI_SetState
 * Görevi: İlgili aşamaya göre LED'leri yakar veya söndürür.
 * 
 * Kurallar:
 * UI_STATE_IDLE       -> Tüm LED'ler Söner
 * UI_STATE_TRANSITION -> Turuncu (PD13) Yanar, Diğerleri Söner
 * UI_STATE_MEASURING  -> Kırmızı (PD14) Yanar, Diğerleri Söner
 * UI_STATE_FINISHED   -> Yeşil (PD12) Yanar, Diğerleri Söner
 */
void CalibrationUI_SetState(Calibration_UI_State_e state);

/*
 * Fonksiyon: CalibrationUI_PrintInstruction
 * Görevi: Serial/Terminal üzerinden kullanıcıya bir sonraki adımı (Ters Z, Düz Z vb.) yazdırır.
 */
void CalibrationUI_PrintInstruction(uint8_t current_step);

/*
 * Fonksiyon: CalibrationUI_PrintResults
 * Görevi: Kalibrasyon bitiminde hesaplanan Bias, Scale ve Noise değerlerini UART üzerinden yazdırır.
 */
void CalibrationUI_PrintResults(SensorCalib_t *calib);

#endif /* INC_M2_1_CALIBRATION_UI_H_ */
