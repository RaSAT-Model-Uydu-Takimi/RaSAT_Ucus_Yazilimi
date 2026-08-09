/*
 * M2.0_SystemCore.c
 *
 * Sistemin ana döngü yöneticisi.
 */

#include "M2.0_SystemCore.h"
#include "M2.1_CalibrationMode.h"
#include "M2.2_FlyingMode.h"

static SystemMode currentSystemMode = SYSTEM_MODE_CALIBRATION;

void SystemCore_Init(DataCenter *data) {
    // Sistem her zaman kalibrasyon ile başlar
    currentSystemMode = SYSTEM_MODE_CALIBRATION;
    data->flightMode = FLIGHT_MODE_UNCALIBRATED;
    
    CalibrationMode_Init(data);
}

void SystemCore_Run(DataCenter *data) {
    if (currentSystemMode == SYSTEM_MODE_CALIBRATION) {
        CalibrationMode_Run(data);
        
        // Eğer kalibrasyon başarıyla bittiyse otomatik olarak uçuş moduna geç
        if (CalibrationMode_GetState() == CALIB_STATE_DONE) {
            currentSystemMode = SYSTEM_MODE_FLYING;
            data->flightMode = FLIGHT_MODE_CALIBRATED;
            FlyingMode_Init(data);
        }
    } 
    else if (currentSystemMode == SYSTEM_MODE_FLYING) {
        FlyingMode_Run(data);
    }
}

SystemMode SystemCore_GetMode(void) {
    return currentSystemMode;
}
