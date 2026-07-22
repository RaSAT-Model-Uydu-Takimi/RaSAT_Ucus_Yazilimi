/*
 * Eylem_Bloklari.h
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */

#ifndef INC_EYLEM_BLOKLARI_H_
#define INC_EYLEM_BLOKLARI_H_

#include "Veri_Structlari.h"
#include "Konfigurasyonlar.h"
#include <stdint.h>

uint8_t Ayril();
uint8_t SigmaAc();
uint8_t TelemetriFonksiyonu(uint8_t CalismaliMi);
uint8_t S2DFonksiyonu();
//uint8_t HoverKronometresi();
uint8_t PidHedefYukseklik(float hedefyukseklik);
uint8_t ApamAc();
uint8_t SigmaKapat();
uint8_t Buzzer(uint32_t BuzzerCalmaSuresi_ms);
uint8_t VideoFunction(uint8_t CalismaliMi);

#endif /* INC_EYLEM_BLOKLARI_H_ */
