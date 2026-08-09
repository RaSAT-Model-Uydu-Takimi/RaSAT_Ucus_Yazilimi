/*
 * M2.1_CalibrationMode.c
 *
 * Kalibrasyon Modu implementasyonu.
 */

#include "M2.1_CalibrationMode.h"
#include "M2.3_HardwareFeedback.h"
#include "M3.1_Calibration.h"
#include "main.h" // HAL_GetTick için

static CalibrationState currentState = CALIB_STATE_IDLE;
static uint32_t stateStartTime = 0;

void CalibrationMode_Init(DataCenter *data) {
    currentState = CALIB_STATE_PREP;
    stateStartTime = HAL_GetTick();
    
    M2_3_HardwareFeedback_Init();
    M2_3_HardwareFeedback_SetState(FEEDBACK_CALIB_PREP);
    
    M3_1_Calibration_Reset();
}

void CalibrationMode_Run(DataCenter *data) {
    uint32_t now = HAL_GetTick();
    
    switch (currentState) {
        case CALIB_STATE_PREP:
            if (now - stateStartTime >= CALIB_PREP_DURATION_MS) {
                // Hazırlık bitti, ölçüme geçiliyor
                currentState = CALIB_STATE_MEASURING;
                stateStartTime = now;
                M2_3_HardwareFeedback_SetState(FEEDBACK_CALIB_MEASURING);
            }
            break;
            
        case CALIB_STATE_MEASURING:
            // Sensör verilerini topla
            M3_1_Calibration_Accumulate(data);
            
            if (now - stateStartTime >= CALIB_MEASURE_DURATION_MS) {
                // Ölçüm bitti, bitiş state'ine geç
                currentState = CALIB_STATE_DONE;
                M2_3_HardwareFeedback_SetState(FEEDBACK_CALIB_DONE);
                
                // Bias ve Scale hesapla, DataCenter'a kaydet
                M3_1_Calibration_Calculate(data);
            }
            break;
            
        case CALIB_STATE_DONE:
            // Bekleme durumu, başka mod'a geçilene kadar burada kalır
            break;
            
        default:
            break;
    }
    
    // Asenkron donanım güncellemeleri (Buzzer kesintileri vb.)
    M2_3_HardwareFeedback_Run();
}

CalibrationState CalibrationMode_GetState(void) {
    return currentState;
}
