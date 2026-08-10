/*
 * M3.2_Attitude.h
 *
 * Yönelim (Pitch ve Roll) Hesaplama Modülü
 * EKF'den gelen kuaterniyonları Euler açılarına dönüştürür.
 */

#ifndef INC_M3_2_ATTITUDE_H_
#define INC_M3_2_ATTITUDE_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// Her filtre (EKF) güncellemesinden sonra çağrılmalıdır
void M3_2_Attitude_Update(DataCenter *dc);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_2_ATTITUDE_H_ */
