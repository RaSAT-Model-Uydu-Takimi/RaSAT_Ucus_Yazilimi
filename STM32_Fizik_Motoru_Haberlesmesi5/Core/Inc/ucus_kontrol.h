/*
 * ucus_kontrol.h
 *
 * Created on: Jul 9, 2026
 * Author: abduh
 */

#ifndef INC_UCUS_KONTROL_H_
#define INC_UCUS_KONTROL_H_

#include "fm_haberlesme.h" // Veri paketlerinin (Alim_Paketi_t, Iletim_Paketi_t) yapılarını bilmesi için






// --- FONKSİYON PROTOTİPLERİ ---

// Ana Uçuş Yazılımı Döngüsü
// Simülasyondan (veya ileride gerçek sensörden) gelen veriyi alır, itki ve bayrakları hesaplar.
void Ucus_Yazilimini_Calistir(Alim_Paketi_t *gelenler, Iletim_Paketi_t *gidecekler);




#endif /* INC_UCUS_KONTROL_H_ */
