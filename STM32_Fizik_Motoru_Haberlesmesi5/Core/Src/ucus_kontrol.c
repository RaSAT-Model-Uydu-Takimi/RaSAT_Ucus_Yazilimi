/*
 * ucus_kontrol.c
 *
 * Created on: Jul 9, 2026
 * Author: abduh
 */

#include "ucus_kontrol.h"

// --- SABİTLER ---
#define TIME_STEP 0.0025f // Fizik motorundan gelen her paketin temsil ettiği sanal süre (400Hz)
// Durum Bayrakları Maskeleri (Bit 0 - 3)
#define BAYRAK_APAM  (1U << 0) // Bit 0
#define BAYRAK_SEP   (1U << 1) // Bit 1: Ayrılma
#define BAYRAK_SGM   (1U << 2) // Bit 2: SİGMA Kanatları
#define BAYRAK_BUZZ  (1U << 3) // Bit 3: Buzzer
//bayrakları kaldırmak için durumbayraklari kısmı ile veyalama işlemi yapacağız.
//bayrağı indirmek için ise bayrağın değilini alıp veleyeceğiz.
#define PERIOD_400HZ (1.0f / 400.0f) // 0.0025f
#define PERIOD_100HZ (1.0f / 100.0f) // 0.01f
#define PERIOD_10HZ  (1.0f / 10.0f)  // 0.1f
#define PERIOD_1HZ   (1.0f / 1.0f)   // 1.0f




// --- KÜTÜPHANE İÇİ GİZLİ (STATIC) DEĞİŞKENLER ---
static float zaman = 0.0f;     // Sanal zaman sayacı
static uint32_t previndex = 0;
static float dt = 0.0025f;



///////////UÇUŞ YAZILIMI DEĞİŞKENLERİ/////////////////////
static uint8_t yukari = 0;     // Motor açık/kapalı durum bayrağı
///////////////////////////////////////////////////////////





// ---------------------------------------------------------
// ANA UÇUŞ DÖNGÜSÜ
// ---------------------------------------------------------
void Ucus_Yazilimini_Calistir(Alim_Paketi_t *gelenler, Iletim_Paketi_t *gidecekler) {

	// 1. SANAL ZAMANI İLERLET
	// Bu fonksiyon her çağrıldığında sanal simülasyon zamanı ilerler.
	if (gelenler->index < previndex) { previndex = gelenler->index; }
	zaman = (float)(gelenler->index) * TIME_STEP;
	dt = (float)(gelenler->index - previndex) * TIME_STEP;
	static uint32_t task_400Hz_ticks = 0;
	static uint32_t task_100Hz_ticks = 0;
	static uint32_t task_10Hz_ticks  = 0;
	static uint32_t task_1Hz_ticks   = 0;
	uint32_t delta_ticks = gelenler->index - previndex;
	task_400Hz_ticks += delta_ticks;
	task_100Hz_ticks += delta_ticks;
	task_10Hz_ticks  += delta_ticks;
	task_1Hz_ticks   += delta_ticks;





	if (task_400Hz_ticks >= 1) {
	  task_400Hz_ticks = 0;

	}


	if (task_100Hz_ticks >= 4) {
	  task_100Hz_ticks %= 4;

	}


	if (task_10Hz_ticks >= 40) {
	  task_10Hz_ticks %= 40;

	}


	if (task_1Hz_ticks >= 400) {
	  task_1Hz_ticks %= 400;
	  yukari ^= (1U);
	  if(yukari == 1){//şimdilik test etmek için burayı doldurdum
		  gidecekler->mi1 = gidecekler->mi2 = gidecekler->mi3 = gidecekler->mi4 = 20000;
	  }
	  else if (yukari == 0){
		  gidecekler->mi1 = gidecekler->mi2 = gidecekler->mi3 = gidecekler->mi4 = 0;
	  }


	}

    gidecekler->durum_bayraklari = 0x00;

    /* Örnek Bayrak Atamaları:
    if (mevcut_durum == DURUM_AYRILMA) {
        hesaplanan_ciktilar->durum_bayraklari |= BAYRAK_SEP;  // Ayrılma bayrağını dik
    }
    if (mevcut_durum == DURUM_GOREV) {
        hesaplanan_ciktilar->durum_bayraklari |= BAYRAK_SGM;  // SİGMA bayrağını dik
    }
    */

    previndex = gelenler->index;
}
