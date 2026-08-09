/*
 * M2.1_CalibrationMode.h
 *
 * Kalibrasyon Modu (Asenkron State Machine)
 */

#ifndef INC_M2_1_CALIBRATIONMODE_H_
#define INC_M2_1_CALIBRATIONMODE_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// Bekleme süreleri makro olarak tanımlandı
#define CALIB_PREP_DURATION_MS 5000
#define CALIB_MEASURE_DURATION_MS 3000

typedef enum {
    CALIB_STATE_IDLE = 0,
    CALIB_STATE_PREP,
    CALIB_STATE_MEASURING,
    CALIB_STATE_DONE
} CalibrationState;

void CalibrationMode_Init(DataCenter *data);
void CalibrationMode_Run(DataCenter *data);
CalibrationState CalibrationMode_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_M2_1_CALIBRATIONMODE_H_ */
