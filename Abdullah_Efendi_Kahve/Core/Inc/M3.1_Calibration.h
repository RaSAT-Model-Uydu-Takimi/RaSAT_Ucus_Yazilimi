/*
 * M3.1_Calibration.h
 *
 * Sensör Kalibrasyon (Bias ve Scale hesaplama) algoritmaları (Asenkron)
 */

#ifndef INC_M3_1_CALIBRATION_H_
#define INC_M3_1_CALIBRATION_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// Kalibrasyon sürecini sıfırlar (Yeni bir ölçüm başlatmak için)
void M3_1_Calibration_Reset(void);

// Ölçüm durumundayken her iterasyonda çağrılır ve raw değerleri toplar
void M3_1_Calibration_LoadDefaults(DataCenter *dc);
void M3_1_Calibration_Accumulate(DataCenter *dc);

// Süre dolduğunda ortalama (bias) ve ölçek (scale) hesaplamalarını yapar
void M3_1_Calibration_Calculate(DataCenter *dc);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_1_CALIBRATION_H_ */
