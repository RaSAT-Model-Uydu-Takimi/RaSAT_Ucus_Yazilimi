#ifndef INC_M3_EKF_ATTITUDE_H_
#define INC_M3_EKF_ATTITUDE_H_

#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Fonksiyon: M3_Attitude_Init
 * Görevi: Kuaterniyonları roketin dik (ufuk çizgisine paralel) durumuna göre sıfırlar.
 */
void M3_Attitude_Init(DataCenter *dataC);

/*
 * Fonksiyon: M3_Attitude_Update
 * Görevi: G-Kompansasyonlu Yönelim Kestiricisi (Pitch ve Roll).
 * M2'den gelen güven değerlerini ve Filter_Config'teki ağırlıkları kullanır.
 * Roketin fırlatma anındaki aşırı ivmelenmelerinde ivmeölçeri yoksayar (G-Comp).
 */
void M3_Attitude_Update(DataCenter *dataC, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_EKF_ATTITUDE_H_ */
