#ifndef INC_M5_EKF_ALTITUDE_H_
#define INC_M5_EKF_ALTITUDE_H_

#include "M0.0_DataCenter.h"
#include "M3.1_MatrixOps.h"
#include "M3.8_StationReference.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // 3 Durumlu Durum Vektörü (State Vector x): [Z_Konum, Z_Hız, Z_İvme_Bias]
    float x[3]; 
    
    // 3x3 Hata Kovaryans Matrisi (P)
    float P[9];

    // Station referansı Station_Reference modülünden alınır
} M5_EKF_Altitude_t;

/*
 * Fonksiyon: M5_Altitude_Init
 * Görevi: EKF durum matrislerini AGL=0 olarak başlatır.
 *         İlk geçerli baro ölçümünde zemin referansı otomatik alınır.
 */
void M5_Altitude_Init(M5_EKF_Altitude_t *ekf);

/*
 * Fonksiyon: M5_Altitude_Update
 * Görevi: İvmeölçer ile tahminde bulunur, Barometre ve GPS ile düzeltme yapar.
 *         Çıktılar: estimated.pos_z, vel_z, a_z (AGL) ve confidence değerleri.
 */
void M5_Altitude_Update(M5_EKF_Altitude_t *ekf, DataCenter *dataC, const Station_Reference_t *station, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M5_EKF_ALTITUDE_H_ */
