/*
 * Lojik_Moduller.c
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */
#include <stdint.h>

//Bütün lojik modüller sadece bir sefer tetiklenebilir.


static uint8_t fallingstartAF = 0;
static uint8_t ayrilmalimiAF = 0;
static uint8_t ayrildimiAF = 0;
static uint8_t apamacilmalimiAF = 0;
static uint8_t hoverstartAF = 0;
static uint8_t hoverexitAF = 0;
static uint8_t fallingstopAF = 0;


uint8_t FallingStart(){
	if(fallingstartAF==1){return 0;}
}

uint8_t AyrilmaliMi(){
	if(ayrilmalimiAF==1){return 0;}
}

uint8_t AyrildiMi(){
	if(ayrildimiAF==1){return 0;}
}

uint8_t ApamAcilmaliMi(){
	if(apamacilmalimiAF==1){return 0;}
}

uint8_t HoverStart(){
	if(hoverstartAF==1){return 0;}
}

uint8_t HoverExit(){
	if(hoverexitAF==1){return 0;}
}

uint8_t FallingStop(){
	if(fallingstopAF==1){return 0;}
}


