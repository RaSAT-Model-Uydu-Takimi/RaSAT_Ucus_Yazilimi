/*
 * M2.3_HardwareFeedback.h
 *
 * Uçuş ve kalibrasyon durumlarına göre asenkron (bloklamayan) sistem geri bildirim kontrolü.
 * Donanım detaylarının koda gömülmemesi için callback (fonksiyon göstericisi) mimarisi kullanır.
 */
#ifndef INC_M2_3_HARDWAREFEEDBACK_H_
#define INC_M2_3_HARDWAREFEEDBACK_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FEEDBACK_IDLE = 0,
    FEEDBACK_CALIB_PREP,       
    FEEDBACK_CALIB_MEASURING,  
    FEEDBACK_CALIB_DONE,       
    FEEDBACK_ERROR,
    
    FEEDBACK_STATE_COUNT // Toplam durum sayısını tutmak için
} FeedbackState;

// Callback fonksiyon tipi tanımlaması
typedef void (*FeedbackCallback_t)(void);

// Fonksiyon kayıt işlemi
void M2_3_HardwareFeedback_RegisterCallback(FeedbackState state, FeedbackCallback_t onEnter, FeedbackCallback_t onRun);

// Çekirdek fonksiyonlar
void M2_3_HardwareFeedback_Init(void);
void M2_3_HardwareFeedback_SetState(FeedbackState newState);
void M2_3_HardwareFeedback_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_M2_3_HARDWAREFEEDBACK_H_ */
