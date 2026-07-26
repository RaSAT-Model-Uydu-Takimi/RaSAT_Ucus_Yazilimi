#include "M2_Confidence_Evaluator.h" 
#include <math.h>                      // fabs() MUTLAK DEĞER fonk için gerekli
#include <stddef.h>                    // NULL makrosu için şartmış



static void EvaluateChannel(float curr_val, float *last_val, float *curr_conf, 
                            uint32_t update_time, uint32_t curr_time, float max_spike) 
{
    // 1. ZAMAN AŞIMI (TIMEOUT) KONTROLÜ
    if ((curr_time - update_time) > CONFIDENCE_TIMEOUT_US) {
        *curr_conf = 0.0f; // Veri donmuş, güven anında SIFIR!
        return;
    }

    // 2. FİZİKSEL SIÇRAMA (SPIKE) KONTROLÜ
    float delta = fabs(curr_val - (*last_val));
    if (delta > max_spike) {
        // İmkansız bir sıçrama var! Güveni cezalandır (Hızlı Düşüş)
        *curr_conf = (*curr_conf) * CONFIDENCE_SPIKE_PENALTY_FACTOR;
    } else {
        // Veri sağlıklı. Güveni YAVAŞ YAVAŞ toparla (Recovery)
        *curr_conf += 0.01f; 
        if (*curr_conf > 1.0f) {
            *curr_conf = 1.0f; // Maksimum sınır
        }
        *last_val = curr_val; // Sadece sağlıklıysa hafızayı güncelle
    }

    // 3. MİNİMUM GÜVEN EŞİĞİ
    if (*curr_conf < CONFIDENCE_MIN_VALID) {
        *curr_conf = 0.0f; // Kullanılamayacak kadar kötüyse tamamen fişini çek
    }
}

/*
 * TANIM : Değerlendirme Sisteminin Kurucu Fonksiyonu
 * İŞLEV : Tüm değerleri temelde sıfırlar 
 */
void CE_Init(Confidence_Evaluator_t *eval) {
    if (eval == NULL) return;
    
    eval->last_acc_x = 0.0f; eval->last_acc_y = 0.0f; eval->last_acc_z = 0.0f; 
    eval->last_gyro_x = 0.0f; eval->last_gyro_y = 0.0f; eval->last_gyro_z = 0.0f; 
    eval->last_baro_press = FILTER_SEA_LEVEL_PA_DEFAULT;
}

/*
 * TANIM : Sistemi çağrıldığında günceller
 * İŞLEV : DataCenter'daki tüm kanallar için güven skoru hesaplar
 */
void CE_Update(Confidence_Evaluator_t *eval, DataCenter *dataC, uint32_t curr_time) {
    if (eval == NULL || dataC == NULL) return;
    
    // --- 1. İVMEÖLÇER KANALLARI ---
    EvaluateChannel(dataC->acc.x.calibratedValue, &eval->last_acc_x, &dataC->acc.x.confidence, dataC->acc.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);
    EvaluateChannel(dataC->acc.y.calibratedValue, &eval->last_acc_y, &dataC->acc.y.confidence, dataC->acc.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);
    EvaluateChannel(dataC->acc.z.calibratedValue, &eval->last_acc_z, &dataC->acc.z.confidence, dataC->acc.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_ACC_MPS2);

    // --- 2. JİROSKOP KANALLARI ---
    EvaluateChannel(dataC->gyro.x.calibratedValue, &eval->last_gyro_x, &dataC->gyro.x.confidence, dataC->gyro.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
    EvaluateChannel(dataC->gyro.y.calibratedValue, &eval->last_gyro_y, &dataC->gyro.y.confidence, dataC->gyro.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);
    EvaluateChannel(dataC->gyro.z.calibratedValue, &eval->last_gyro_z, &dataC->gyro.z.confidence, dataC->gyro.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_GYRO_RADPS);

    // --- 3. BAROMETRE KANALI ---
    EvaluateChannel(dataC->baro.press.calibratedValue, &eval->last_baro_press, &dataC->baro.press.confidence, dataC->baro.UpdateTime, curr_time, CONFIDENCE_MAX_SPIKE_BARO_PA);

    // --- 4. MANYETOMETRE VE GPS (SADECE TIMEOUT VE FİX) ---
    if ((curr_time - dataC->mag.UpdateTime) > CONFIDENCE_TIMEOUT_US) {
        dataC->mag.x.confidence = 0.0f; dataC->mag.y.confidence = 0.0f; dataC->mag.z.confidence = 0.0f;
    } else {
        dataC->mag.x.confidence = 1.0f; dataC->mag.y.confidence = 1.0f; dataC->mag.z.confidence = 1.0f;
    }

    if ((curr_time - dataC->gps.UpdateTime) > CONFIDENCE_TIMEOUT_US || dataC->gps.fixQuality == 0) {
        dataC->gps.x.confidence = 0.0f; dataC->gps.y.confidence = 0.0f; 
        dataC->gps.z.confidence = 0.0f; dataC->gps.speed.confidence = 0.0f; dataC->gps.course.confidence = 0.0f;
    } else {
        dataC->gps.x.confidence = 1.0f; dataC->gps.y.confidence = 1.0f; 
        dataC->gps.z.confidence = 1.0f; dataC->gps.speed.confidence = 1.0f; dataC->gps.course.confidence = 1.0f;
    }
}
