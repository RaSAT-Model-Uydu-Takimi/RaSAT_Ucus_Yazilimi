#ifndef INC_M3_EKF_ATTITUDE_H_
#define INC_M3_EKF_ATTITUDE_H_

#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x[7];   /* State: [q0, q1, q2, q3, bias_x, bias_y, bias_z] */
    float P[49];  /* Covariance: 7x7 Matris */
} M3_EKF_Attitude_t;

/*
 * Fonksiyon: M3_Attitude_Init
 * Görevi: Kuaterniyonları roketin dik (ufuk çizgisine paralel) durumuna göre sıfırlar.
 * Bias değerlerini sıfırlar ve P matrisini başlatır.
 */
void M3_Attitude_Init(M3_EKF_Attitude_t *ekf);

/*
 * Fonksiyon: M3_Attitude_Update
 * Görevi: 7-Durumlu Genişletilmiş Kalman Filtresi (EKF).
 * Jiroskop kaymalarını (bias) dinamik olarak tahmin eder.
 * M2'den gelen güven değerlerini ve Filter_Config'teki ağırlıkları kullanır.
 * İnovasyon (FDI) testi ile hatalı ivme okumalarını reddeder.
 */
void M3_Attitude_Update(M3_EKF_Attitude_t *ekf, DataCenter *dataC, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_EKF_ATTITUDE_H_ */
