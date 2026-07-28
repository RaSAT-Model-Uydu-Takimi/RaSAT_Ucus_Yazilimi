#ifndef INC_STATION_REFERENCE_H_
#define INC_STATION_REFERENCE_H_

#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double ref_lat;
    double ref_lon;
    float ref_alt_msl;
    
    float ref_baro_msl;

    uint8_t gps_ref_set;
    uint8_t baro_ref_set;
} Station_Reference_t;

/* 
 * Fonksiyon: Station_Init
 * Görevi: İstasyon referans yapısını sıfırlar. 
 */
void Station_Init(Station_Reference_t *station);

/* 
 * Fonksiyon: Station_SetReference
 * Görevi: Uçuş öncesi (veya ilk fix anında) o anki geçerli
 *         Baro ve GPS verilerini kalkış (0,0,0) noktası olarak kaydeder.
 */
void Station_SetReference(Station_Reference_t *station, const DataCenter *dc, float sea_level_pa);

/* 
 * Fonksiyon: Station_ApplyReference
 * Görevi: Ham/Kalibre edilmiş sensör okumalarından istasyon
 *         referansını çıkartarak yerel AGL ve yerel X/Y'ye hazır hale getirir.
 */
void Station_ApplyReference(const Station_Reference_t *station, DataCenter *dc, float sea_level_pa);

#ifdef __cplusplus
}
#endif

#endif /* INC_STATION_REFERENCE_H_ */
