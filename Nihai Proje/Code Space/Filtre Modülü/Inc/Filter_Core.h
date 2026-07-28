#ifndef INC_FILTER_CORE_H_
#define INC_FILTER_CORE_H_

#include "Data.h"
#include "M1_Sensor_Calibrator.h"
#include "M2_Confidence_Evaluator.h"
#include "M3_EKF_Attitude.h"
#include "M4_EKF_Yaw.h"
#include "M5_EKF_Altitude.h"
#include "M6_EKF_NavXY.h"
#include "M7_Kinematics.h"
#include "Station_Reference.h"
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
    Station_Reference_t station;
    uint32_t last_update_time_us;
} Filter_System_t;

/*
 * Fonksiyon: Filter_Init
 * Görevi: Tüm filtre sistemini, alt modülleri ve DataCenter'ı başlatır.
 *         İrtifa AGL (yerden yükseklik) = 0 olarak başlar.
 *         Rampa kalibrasyonu ile otomatik referans alınır.
 */
void Filter_Init(Filter_System_t *sys);

/*
 * Fonksiyon: Filter_Update
 * Görevi: DataCenter'daki rawValue alanlarını (sensör sürücüleri tarafından
 *         doldurulmuş) alır ve tüm Kaskad EKF boru hattından geçirir.
 *
 * NOT: Sensör sürücüleri, bu fonksiyon çağrılmadan ÖNCE rawValue ve
 *      UpdateTime alanlarını doldurmuş olmalıdır.
 */
void Filter_Update(Filter_System_t *sys, uint32_t curr_time_us);

#ifdef __cplusplus
}
#endif

#endif /* INC_FILTER_CORE_H_ */
