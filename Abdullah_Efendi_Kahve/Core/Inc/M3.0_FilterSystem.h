/*
 * M3.0_FilterSystem.h
 *
 * Mahony 6-DOF (AHL) Filtre Sistemi
 */

#ifndef INC_M3_0_FILTERSYSTEM_H_
#define INC_M3_0_FILTERSYSTEM_H_

#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

// Filtre ilk kurulum
void FilterSystem_Init(DataCenter *dc);

// Filtre Güncelleme (Sensör okumasından sonra çağrılır)
void FilterSystem_Update(DataCenter *dc, float dt);

#ifdef __cplusplus
}
#endif

#endif /* INC_M3_0_FILTERSYSTEM_H_ */
