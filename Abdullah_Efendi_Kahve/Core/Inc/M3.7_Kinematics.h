#ifndef INC_M7_KINEMATICS_H_
#define INC_M7_KINEMATICS_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Fonksiyon: Kinematics_BodyToEarth
 * Görevi: Gövde (Body) eksenindeki ivmeleri (calibratedValue, m/s²),
 *         Tahmini Pitch, Roll ve Yaw açılarını kullanarak
 *         Dünya (NED - North, East, Down) eksenine çevirir.
 *         Sonuçları DataCenter -> estimated -> earth_a_x/y/z alanlarına kaydeder.
 */
void Kinematics_BodyToEarth(DataCenter *dc);

#ifdef __cplusplus
}
#endif

#endif /* INC_M7_KINEMATICS_H_ */
