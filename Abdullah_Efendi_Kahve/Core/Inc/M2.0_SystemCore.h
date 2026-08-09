/*
 * M2.0_SystemCore.h
 *
 * Sistemin ana döngü yöneticisi (Router).
 * Dinamik durum geçişlerini yönetir.
 */

#ifndef INC_M2_0_SYSTEMCORE_H_
#define INC_M2_0_SYSTEMCORE_H_

#include "main.h"
#include "M0.0_DataCenter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SYSTEM_MODE_CALIBRATION = 0,
    SYSTEM_MODE_FLYING      = 1
} SystemMode;

void SystemCore_Init(DataCenter *data);
void SystemCore_Run(DataCenter *data);
SystemMode SystemCore_GetMode(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_M2_0_SYSTEMCORE_H_ */
