#ifndef INC_M6_EKF_NAVXY_H_
#define INC_M6_EKF_NAVXY_H_

#include "Data.h"
#include "M0_Matrix_Operations.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // 4 Durumlu Durum Vektörü (State Vector x): [X_Konum, Y_Konum, X_Hız, Y_Hız]
    float x[4]; 
    
    // 4x4 Hata Kovaryans Matrisi (P)
    float P[16]; 
    
    // Referans Konumları (Derece) - İlk GPS fix'te kaydedilir
    float ref_lat;
    float ref_lon;
    uint8_t ref_set;
} M6_EKF_NavXY_t;

/*
 * Fonksiyon: M6_NavXY_Init
 * Görevi: EKF durum matrislerini yatay navigasyon için sıfırlar.
 */
void M6_NavXY_Init(M6_EKF_NavXY_t *ekf);

/*
 * Fonksiyon: M6_NavXY_Update
 * Görevi: İvmeölçer (Tahmin) ve GPS (Güncelleme) verilerini kullanarak
 * roketin yatay (Rüzgar vb.) sürüklenmesini ve konumunu yüksek frekanslı olarak hesaplar.
 */
void M6_NavXY_Update(M6_EKF_NavXY_t *ekf, DataCenter *dataC, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_M6_EKF_NAVXY_H_ */
