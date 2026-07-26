#ifndef INC_FILTER_CORE_H_
#define INC_FILTER_CORE_H_

#include "Data.h"
#include "Veri_Structlari.h"
#include "M1_Sensor_Calibrator.h"
#include "M2_Confidence_Evaluator.h"
#include "M3_EKF_Attitude.h"
#include "M4_EKF_Yaw.h"
#include "M5_EKF_Altitude.h"
#include "M6_EKF_NavXY.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    DataCenter dataC;
    SensorCalib_t calib;
    Confidence_Evaluator_t conf_eval;
    M4_EKF_Yaw_t ekf_yaw;
    M5_EKF_Altitude_t ekf_altitude;
    M6_EKF_NavXY_t ekf_nav_xy;
    uint32_t last_update_time_us;
} Filter_System_t;

/*
 * Fonksiyon: Filter_Init
 * Görevi: Tüm filtre sistemini, alt modülleri ve DataCenter'ı başlatır.
 * Parametreler: 
 * - sys: Filtre sistemi nesnesi
 * - initial_altitude: Rampanın deniz seviyesinden yüksekliği
 */
void Filter_Init(Filter_System_t *sys, float initial_altitude);

/*
 * Fonksiyon: Filter_Update
 * Görevi: Sensörlerden okunan ham veriyi alır ve tüm Kaskad EKF boru hattından geçirir.
 * Bu fonksiyon, RTOS veya main loop içinde saniyede mümkün olduğunca çok çağrılmalıdır.
 */
void Filter_Update(Filter_System_t *sys, Sensor_Verileri_t *raw_data, uint32_t curr_time_us);

#ifdef __cplusplus
}
#endif

#endif /* INC_FILTER_CORE_H_ */
