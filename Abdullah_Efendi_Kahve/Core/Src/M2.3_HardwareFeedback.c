/*
 * M2.3_HardwareFeedback.c
 *
 * Callback (fonksiyon göstericisi) mimarisini kullanarak bloklamayan sistem geri bildirimi uygular.
 */
#include "M2.3_HardwareFeedback.h"
#include <stddef.h>

static FeedbackState currentState = FEEDBACK_IDLE;

// Her durum (state) için bir "Giriş" (onEnter) ve "Sürekli Çalışma" (onRun) fonksiyon göstericisi dizisi
static FeedbackCallback_t onEnterCallbacks[FEEDBACK_STATE_COUNT] = {NULL};
static FeedbackCallback_t onRunCallbacks[FEEDBACK_STATE_COUNT] = {NULL};

void M2_3_HardwareFeedback_RegisterCallback(FeedbackState state, FeedbackCallback_t onEnter, FeedbackCallback_t onRun) {
    if (state < FEEDBACK_STATE_COUNT) {
        onEnterCallbacks[state] = onEnter;
        onRunCallbacks[state] = onRun;
    }
}

void M2_3_HardwareFeedback_Init(void) {
    currentState = FEEDBACK_IDLE;
    
    // Not: Bu modül artık hiçbir GPIO kütüphanesine (main.h vb.) bağımlı değildir.
    // İlgili pinleri yönetecek asıl fonksiyonlar main.c veya config dosyasında 
    // yazılıp 'RegisterCallback' ile buraya bağlanacaktır.
}

void M2_3_HardwareFeedback_SetState(FeedbackState newState) {
    if (newState >= FEEDBACK_STATE_COUNT) return;
    
    if (currentState != newState) {
        currentState = newState;
        
        // State değiştiğinde, eğer yeni state için bir 'onEnter' fonksiyonu atanmışsa çalıştır.
        if (onEnterCallbacks[currentState] != NULL) {
            onEnterCallbacks[currentState]();
        }
    }
}

void M2_3_HardwareFeedback_Run(void) {
    if (currentState < FEEDBACK_STATE_COUNT) {
        // Asenkron döngüde, o anki state'in 'onRun' fonksiyonu atanmışsa sürekli çalıştır.
        // Yanıp sönme (Blink/Pulse) gibi timer tabanlı işlemler o fonksiyonun kendi içinde halledilir.
        if (onRunCallbacks[currentState] != NULL) {
            onRunCallbacks[currentState]();
        }
    }
}
