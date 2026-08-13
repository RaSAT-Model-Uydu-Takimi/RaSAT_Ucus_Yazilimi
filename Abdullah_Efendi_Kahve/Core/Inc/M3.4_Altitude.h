/*
 * M3.4_Altitude.h
 *
 * İrtifa (Altitude) ve Dikey Hız (Vertical Velocity) Hesaplama Modülü
 * İvmeölçer ve Barometre verilerini 2-Durumlu Kalman Filtresi ile birleştirir.
 */

#ifndef INC_M3_4_ALTITUDE_H_
#define INC_M3_4_ALTITUDE_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

void M3_4_Altitude_Init(DataCenter *dc);
void M3_4_Altitude_Update(DataCenter *dc, float dt);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_4_ALTITUDE_H_ */
