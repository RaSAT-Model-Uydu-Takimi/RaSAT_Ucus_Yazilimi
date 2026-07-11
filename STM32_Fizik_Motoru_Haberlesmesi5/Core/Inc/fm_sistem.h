/*
 * fm_sistem.h
 *
 * Created on: Jul 9, 2026
 * Author: abduh
 */

#ifndef INC_FM_SISTEM_H_
#define INC_FM_SISTEM_H_

#include "fm_haberlesme.h" // İletişim veri yapılarını (Alim_Veri_t, Iletim_Veri_t) tanıyabilmesi için

// Alarm_Paketi_Olustur fonksiyonunu sildik, yerine sabit paketimizi dışa açtık:
extern const Iletim_Veri_t ALARM_PAKETI_SABIT;


// --- FONKSİYON PROTOTİPLERİ ---
// 1. Fizik Motorundan Alarm geldiğinde bir önceki paketi tekrar gönderime hazırlar
void En_Son_Paketi_Bir_Daha_Olustur(Iletim_Veri_t *iletilecek_paket);

// 2. Ana uçuş yazılımı (Sensör verilerini alıp motor itkilerini ve bayrakları belirler)
void Sistem_Davranisini_Hesapla(Alim_Veri_t *gelen_veri, Iletim_Veri_t *iletilecek_paket);

// 3. Paketin index'ini artırıp, Checksum (CHK) hesaplayarak paketi mühürler
void Iletim_Paketini_Olustur(Iletim_Veri_t *iletilecek_paket);

#endif /* INC_FM_SISTEM_H_ */
