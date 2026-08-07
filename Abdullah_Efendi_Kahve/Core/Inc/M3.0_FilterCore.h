#ifndef INC_FILTER_CORE_H_
#define INC_FILTER_CORE_H_

#include "M0.0_DataCenter.h"
#include "M2.0_FactoryCalibrator.h"
#include "M3.2_ConfidenceEvaluator.h"
#include "M3.3_EKFAttitude.h"
#include "M3.4_EKFYaw.h"
#include "M3.5_EKFAltitude.h"
#include "M3.6_EKFNavXY.h"
#include "M3.7_Kinematics.h"
#include "M3.8_StationReference.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SensorCalib_t calib;
    Confidence_Evaluator_t conf_eval;
    M3_EKF_Attitude_t ekf_attitude;
    M4_EKF_Yaw_t ekf_yaw;
    M5_EKF_Altitude_t ekf_altitude;
    M6_EKF_NavXY_t ekf_nav_xy;
    Station_Reference_t station;
    uint32_t last_update_time_us;
} Filter_System_t;

/*
 * Fonksiyon: Filter_Init
 * Görevi: Tüm filtre sistemini, alt modülleri ve DataCenter'ı başlatır.
 *         İrtifa AGL (yerden yükseklik) = 0 olarak başlar.
 *         Rampa kalibrasyonu ile otomatik referans alınır.
 */
void Filter_Init(Filter_System_t *sys, DataCenter *dc);

/*
 * Fonksiyon: Filter_Update
 * Görevi: DataCenter'daki rawValue alanlarını (sensör sürücüleri tarafından
 *         doldurulmuş) alır ve tüm Kaskad EKF boru hattından geçirir.
 *
 * NOT: Sensör sürücüleri, bu fonksiyon çağrılmadan ÖNCE rawValue ve
 *      UpdateTime alanlarını doldurmuş olmalıdır.
 */
void Filter_Update(Filter_System_t *sys, DataCenter *dc, uint32_t curr_time_us);

#ifdef __cplusplus
}
#endif

#endif /* INC_FILTER_CORE_H_ */
