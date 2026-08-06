/*
 * fight_control.h
 *
 *  Created on: Aug 5, 2026
 *      Author: umut
 *
 * Sigma aktif inis sisteminin PID tabanli
   ucus kontrol islemlerini yonetir.
 */

#ifndef INC_FIGHT_CONTROL_H_
#define INC_FIGHT_CONTROL_H_


#include "Data_Structlari.h"
#include <stdint.h>


/*
 * Flight control modulunu hazirlar.
 *
 * PID durumlarini ve hedefleri sifirlar.
 * Kontrol sistemi kapali olarak baslar.
 *
 * Program baslangicinda bir kez cagrilmalidir.
 */
void FlightControl_Init(void);


/*
 * Flight control sistemini etkinlestirir.
 *
 * PID gecmislerini sifirlar.
 * Motorlari arm eder.
 * Kontrol dongusunun zaman bilgisini baslatir.
 */
void FlightControl_Enable(void);


/*
 * Flight control sistemini kapatir.
 *
 * PID gecmislerini sifirlar.
 * Motorlari durdurur ve disarm eder.
 */
void FlightControl_Disable(void);


/*
 * Flight control sisteminin aktif olup olmadigini dondurur.
 *
 * Donus:
 * 1U -> Kontrol aktif
 * 0U -> Kontrol kapali
 */
uint8_t FlightControl_IsEnabled(void);


/*
 * Hedef yuksekligi ayarlar.
 *
 * hedef_yukseklik_m:
 * Metre cinsinden hedef irtifa.
 */
void FlightControl_SetTargetAltitude(
    float hedef_yukseklik_m
);


/*
 * Hedef yonelim acilarini ayarlar.
 *
 * hedef_roll_deg:
 * Y ekseni etrafindaki hedef roll acisi [derece]
 *
 * hedef_pitch_deg:
 * X ekseni etrafindaki hedef pitch acisi [derece]
 *
 * hedef_yaw_deg:
 * Z ekseni etrafindaki hedef yaw acisi [derece]
 */
void FlightControl_SetTargetAttitude(
    float hedef_roll_deg,
    float hedef_pitch_deg,
    float hedef_yaw_deg
);


/*
 * Flight control hesabini bir kontrol cevrimi calistirir.
 *
 * veri_merkezi:
 * Kestirilmis yukseklik, dikey hiz, roll,
 * pitch ve yaw verilerini iceren DataCenter yapisi.
 *
 * mevcut_zaman_us:
 * Mikrosaniye cinsinden mevcut sistem zamani.
 *
 * Bu fonksiyon:
 * - Yukseklik PID hesabini yapar.
 * - Dikey hiz PID hesabini yapar.
 * - Roll, pitch ve yaw PID hesaplarini yapar.
 * - Motor mixer'i calistirir.
 * - Hesaplanan cikislari motorlara gonderir.
 */
void FlightControl_Update(
    const DataCenter *veri_merkezi,
    uint32_t mevcut_zaman_us
);




#endif /* INC_FIGHT_CONTROL_H_ */
