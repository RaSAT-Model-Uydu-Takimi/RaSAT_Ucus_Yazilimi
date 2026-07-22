/*
 * Konfigurasyonlar.h
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */

#ifndef INC_KONFIGURASYONLAR_H_
#define INC_KONFIGURASYONLAR_H_

#include "math.h"

//*****************************ANA AKIŞ KONFİGÜRASYON SABİTLERİ*****************************
#define HOVER_YUKSEKLIGI_m 200.0f //metre
#define YERE_INIS_YUKSEKLIGI_m 0.5f //metre

#define SISTEM_TIMERI_TIMESTEP 0.000001f //yani 1 mikrosaniye
////////////////////////////////////////////////////////////////////////////////////////////

//*****************************LOJİK MODÜL KONFİGÜRASYON SABİTLERİ**************************

//*************FallingStart(1)*********************
#define W1_1 0.35f  // Yükseklik ağırlığı
#define W2_1 0.45f  // Z Eksen Hızı ağırlığı
#define W3_1 0.20f  // Z Eksen İvmesi ağırlığı
static inline float CSF1_1(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 1000.0f)/300.0f));}
static inline float CSF2_1(float x){ return fmaxf(0.0f,fminf(1.0f,(-x - 1.0f)/4.0f));}
static inline float CSF3_1(float x){ return fmaxf(0.0f,fminf(1.0f,(-x - 2.0f)/5.0f));}
#define CT_1 0.65f
#define RFILL_1 500.0f
#define RLEAK_1 100.0f
#define TT_1 100.0f

//*************AyrilmaliMi(2)**********************
#define W1_2 0.60f  // Yükseklik ağırlığı
#define W2_2 0.40f  // S2 Faz Süresi ağırlığı
static inline float CSF1_2(float x){ return fmaxf(0.0f,fminf(1.0f,(1020.0f - x)/20.0f));}
static inline float CSF2_2(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 70.0f)/20.0f));}
#define CT_2 0.30f
#define RFILL_2 400.0f
#define RLEAK_2 400.0f // Türbülans gürültülerini önlemek için RFILL ile 1:1 dengelendi
#define TT_2 100.0f

//*************AyrildiMi(3)************************
#define W1_3 0.35f  // Z Eksen İvmesi ağırlığı
#define W2_3 0.65f  // S3 Faz Süresi ağırlığı
static inline float CSF1_3(float x){ return fmaxf(0.0f,fminf(1.0f,(-x - 2.0f)/6.0f));}
static inline float CSF2_3(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 0.5f)/1.0f));}
#define CT_3 0.45f
#define RFILL_3 600.0f
#define RLEAK_3 100.0f
#define TT_3 100.0f

//*************ApamAcilmaliMi(4)*******************
#define W1_4 0.80f  // Z Eksen Hızı ağırlığı
#define W2_4 0.20f  // Yükseklik ağırlığı
static inline float CSF1_4(float x){ return fmaxf(0.0f,fminf(1.0f,(-x - 12.0f)/4.0f));} // Eksen kuralı düzeltildi (-x)
static inline float CSF2_4(float x){ return fmaxf(0.5f,fminf(1.0f,(650.0f - x)/500.0f));}
#define CT_4 0.85f
#define RFILL_4 200.0f
#define RLEAK_4 400.0f // False-Pozitif önlemi: Motor kurtarmalarını hızlı unutması için artırıldı
#define TT_4 100.0f

//*************HoverStart(5)***********************
#define W1_5 0.65f  // Z Eksen Hızı ağırlığı
#define W2_5 0.25f  // Yükseklik ağırlığı
#define W3_5 0.10f  // Z Eksen İvmesi ağırlığı
static inline float CSF1_5(float x){ return fmaxf(0.0f,fminf(1.0f,(3.0f - fabsf(x))/2.0f));} // Kararsızlığı önlemek için Mutlak Değer (fabsf)
static inline float CSF2_5(float x){ return fmaxf(0.0f,fminf(1.0f,(50.0f - fabsf(x-200.0f))/30.0f));}
static inline float CSF3_5(float x){ return fmaxf(0.0f,fminf(1.0f,(4.0f - fabsf(x))/3.0f));}
#define CT_5 0.85f
#define RFILL_5 400.0f
#define RLEAK_5 100.0f
#define TT_5 100.0f

//*************HoverExit(6)************************
#define W1_6 0.40f  // Hover Süresi ağırlığı
#define W2_6 0.30f  // Yükseklik ağırlığı
#define W3_6 0.30f  // S4 Faz Süresi ağırlığı
static inline float CSF1_6(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 8.0f)/2.0f));}
static inline float CSF2_6(float x){ return fmaxf(0.0f,fminf(1.0f,(170.0f - x)/20.0f));}
static inline float CSF3_6(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 110.0f)/15.0f));}
#define CT_6 0.25f
#define RFILL_6 500.0f
#define RLEAK_6 100.0f
#define TT_6 100.0f

//*************FallingStop(7)**********************
#define W1_7 0.35f  // Z Eksen Hızı ağırlığı
#define W2_7 0.45f  // Yükseklik ağırlığı
#define W3_7 0.20f  // S4 Faz Süresi ağırlığı
static inline float CSF1_7(float x){ return fmaxf(0.0f,fminf(1.0f,(2.0f - fabsf(x))/1.5f));} // Yerde sıçrama anlarını korumak için fabsf eklendi
static inline float CSF2_7(float x){ return fmaxf(0.0f,fminf(1.0f,(2.0f - x)/1.5f));}
static inline float CSF3_7(float x){ return fmaxf(0.0f,fminf(1.0f,(x - 90.0f)/20.0f));}
#define CT_7 0.85f
#define RFILL_7 800.0f
#define RLEAK_7 200.0f
#define TT_7 100.0f
////////////////////////////////////////////////////////////////////////////////////////////

//*****************************EYLEM BLOKLARI KONFİGÜRASYON SABİTLERİ***********************
//*************Ayril****************************

//*************SigmaAc**************************

//*************TelemetriFonksiyonu**************

//*************S2DFonksiyonu********************

//*************HoverKronometresi****************

//*************PidHedefYükseklik****************

//*************ApamAc***************************

//*************SigmaKapat***********************

//*************BuzzerAc*************************

////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//*************************YARDIMCI FONKSİYONLAR******************************//
////////////////////////////////////////////////////////////////////////////////


#endif /* INC_KONFIGURASYONLAR_H_ */
