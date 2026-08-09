/*
 * M2.2_FlyingMode.h
 *
 * Uçuş Modu: Uçuş sırasındaki tüm kontrol (PID), filtre (EKF) ve motor işlemlerini yönetir.
 */

#ifndef INC_M2_2_FLYINGMODE_H_
#define INC_M2_2_FLYINGMODE_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

void FlyingMode_Init(DataCenter *data);
void FlyingMode_Run(DataCenter *data);

#ifdef __cplusplus
}
#endif

#endif /* INC_M2_2_FLYINGMODE_H_ */
