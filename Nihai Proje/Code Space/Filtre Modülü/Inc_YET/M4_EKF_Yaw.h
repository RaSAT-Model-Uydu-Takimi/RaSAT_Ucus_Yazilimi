#ifndef INC_M4_EKF_YAW_H_
#define INC_M4_EKF_YAW_H_

#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float state_yaw;    // yaw (derece)
    float P;            // kovaryans
} M4_EKF_Yaw_t;

void M4_Yaw_Init(M4_EKF_Yaw_t *ekf);

/*
 * Fonksiyon: M4_Yaw_Update
 * Görevi: Jiroskop Z ekseni ile GPS Rota (Course) verisini 1-Durumlu EKF ile birleştirir.
 */
void M4_Yaw_Update(M4_EKF_Yaw_t *ekf, DataCenter *dataC, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M4_EKF_YAW_H_ */
