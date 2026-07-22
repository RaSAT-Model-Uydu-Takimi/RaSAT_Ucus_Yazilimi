/*
 * Lojik_Moduller.h
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */

#ifndef INC_LOJIK_MODULLER_H_
#define INC_LOJIK_MODULLER_H_

#include "Veri_Structlari.h"
#include "Data_Structlari.h"
#include "Konfigurasyonlar.h"
#include <stdint.h>


uint8_t FallingStart(DataCenter *DC, uint32_t sistemTimeri);
uint8_t AyrilmaliMi(DataCenter *DC, uint32_t sistemTimeri);
uint8_t AyrildiMi(DataCenter *DC, uint32_t sistemTimeri);
uint8_t ApamAcilmaliMi(DataCenter *DC, uint32_t sistemTimeri);
uint8_t HoverStart(DataCenter *DC, uint32_t sistemTimeri);
uint8_t HoverExit(DataCenter *DC, uint32_t sistemTimeri);
uint8_t FallingStop(DataCenter *DC, uint32_t sistemTimeri);


#endif /* INC_LOJIK_MODULLER_H_ */
