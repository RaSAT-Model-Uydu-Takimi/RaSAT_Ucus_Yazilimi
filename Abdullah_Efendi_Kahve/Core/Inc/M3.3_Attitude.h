#ifndef INC_M3_ATTITUDE_H_
#define INC_M3_ATTITUDE_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float q[4];          /* Kuaterniyon: [q0, q1, q2, q3] */
    float integralFB[3]; /* Jiroskop bias hesabı için integral hata: [x, y, z] */
} M3_Attitude_t;

/*
 * Fonksiyon: M3_Attitude_Init
 * Görevi: Kuaterniyonları ufuk çizgisine paralel (q0=1) olarak sıfırlar.
 *         Integral hata terimlerini temizler.
 */
void M3_Attitude_Init(M3_Attitude_t *attitude);

/*
 * Fonksiyon: M3_Attitude_Update
 * Görevi: Mahony Complementary Filter (+Ki).
 *         İvmeölçer verisinden yerçekimini referans alarak Pitch/Roll tahmini yapar.
 *         Integral (Ki) terimi sayesinde jiroskop kaymalarını (bias) dinamik olarak kompanse eder.
 */
void M3_Attitude_Update(M3_Attitude_t *attitude, DataCenter *dataC, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_ATTITUDE_H_ */
