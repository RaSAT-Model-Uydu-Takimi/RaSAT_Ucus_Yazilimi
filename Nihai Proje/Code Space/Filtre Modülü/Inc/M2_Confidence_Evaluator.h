#ifndef INC_CONFIDENCE_EVALUATOR_H_
#define INC_CONFIDENCE_EVALUATOR_H_

#include "Data.h"
#include "Filter_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TANIM : Bir önceki sağlıklı ölçümü belleğe alır
 * İŞLEV : Spike tespiti için belleğe alıyoruz
 * NOT   : GPS ve Manyetometre sıçramaları için sadece timeout kontrolü yapılır
 */
typedef struct {
    float last_acc_x, last_acc_y, last_acc_z;
    float last_gyro_x, last_gyro_y, last_gyro_z;
    float last_mag_x, last_mag_y, last_mag_z;
    float last_baro_press;
} Confidence_Evaluator_t;

/*
 * TANIM : Kurulum
 * İŞLEV : Güven değerlendirici yapısını başlatır
 */
void CE_Init(Confidence_Evaluator_t *eval);

/*
 * TANIM : Güncelleme
 * İŞLEV :
 *   1. calibratedValue üzerinde spike denetimi yapar
 *   2. Zaman aşımı (timeout) kontrolü yapar
 *   3. DataCenter confidence değerlerini günceller
 */
void CE_Update(Confidence_Evaluator_t *eval, DataCenter *dataC, uint32_t curr_time);

#ifdef __cplusplus
}
#endif

#endif /* INC_CONFIDENCE_EVALUATOR_H_ */