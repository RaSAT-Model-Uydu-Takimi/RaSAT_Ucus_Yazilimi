/*
 * M3.3_Yaw.h
 *
 * Sapma (Yaw / Pusula) Hesaplama Modülü
 * Manyetometre ve Jiroskop verilerini kullanarak tilt-kompanzasyonlu Yaw üretir.
 */

#ifndef INC_M3_3_YAW_H_
#define INC_M3_3_YAW_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

void M3_3_Yaw_Init(DataCenter *dc);
void M3_3_Yaw_Update(DataCenter *dc, float dt);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_3_YAW_H_ */
