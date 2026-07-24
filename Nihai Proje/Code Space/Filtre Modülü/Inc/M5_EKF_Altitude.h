#ifndef INC_M5_EKF_ALTITUDE_H_
#define INC_M5_EKF_ALTITUDE_H_

#include "Data.h"
#include "M0_Matrix_Operations.h"
#include "M1_Sensor_Calibrator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // 3 Durumlu Durum Vektörü (State Vector x): [Z_Konum, Z_Hız, Z_İvme_Bias]
    float x[3]; 
    
    // 3x3 Hata Kovaryans Matrisi (P)
    float P[9]; 
} M5_EKF_Altitude_t;

/*
 * Fonksiyon: M5_Altitude_Init
 * Görevi: EKF durum matrislerini başlangıç irtifasına göre ayarlar.
 */
void M5_Altitude_Init(M5_EKF_Altitude_t *ekf, float initial_altitude);

/*
 * Fonksiyon: M5_Altitude_Update
 * Görevi: İvmeölçer ile tahminde bulunur, Barometre ve GPS ile düzeltme yapar.
 */
void M5_Altitude_Update(M5_EKF_Altitude_t *ekf, DataCenter *dataC, const SensorCalib_t *calib, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M5_EKF_ALTITUDE_H_ */
