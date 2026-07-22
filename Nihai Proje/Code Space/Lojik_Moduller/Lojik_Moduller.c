/*
 * Lojik_Moduller.c
 *
 *  Created on: Jul 19, 2026
 *      Author: abduh
 */
#include <stdint.h>
#include "Veri_Structlari.h"
#include "Data_Structlari.h"
#include "Konfigurasyonlar.h"
#include <math.h>

// Bütün lojik modüller sadece bir sefer tetiklenebilir.
static uint8_t AF_1 = 0; // Eylem Bayrakları (Action Flags)
static uint8_t AF_2 = 0;
static uint8_t AF_3 = 0;
static uint8_t AF_4 = 0;
static uint8_t AF_5 = 0;
static uint8_t AF_6 = 0;
static uint8_t AF_7 = 0;

static float ACC_1 = 0.0f;  // Birikim Değerleri (Accumulator Values)
static float ACC_2 = 0.0f;
static float ACC_3 = 0.0f;
static float ACC_4 = 0.0f;
static float ACC_5 = 0.0f;
static float ACC_6 = 0.0f;
static float ACC_7 = 0.0f;

static uint32_t prev_sistemTimeri_1 = 0;
static uint32_t prev_sistemTimeri_2 = 0;
static uint32_t prev_sistemTimeri_3 = 0;
static uint32_t prev_sistemTimeri_4 = 0;
static uint32_t prev_sistemTimeri_5 = 0;
static uint32_t prev_sistemTimeri_6 = 0;
static uint32_t prev_sistemTimeri_7 = 0;

// Zaman Zırhları İçin Faz Başlangıç Zamanlayıcıları (Mikrosaniye)
static uint32_t s2_start_time = 0;
static uint32_t s3_start_time = 0;
static uint32_t s4_start_time = 0;
static uint32_t hover_start_time = 0;


uint8_t FallingStart(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_1==1){return 0;}

	// dt sıçramasını (First-Run Spike) önlemek için senkronizasyon
	if(prev_sistemTimeri_1 == 0){ prev_sistemTimeri_1 = sistemTimeri; return 0; }

	float x1 = DC->estimated.pos_z.value;
	float x2 = DC->estimated.vel_z.value;
	float x3 = DC->estimated.a_z.value;

	float CF1 = DC->estimated.pos_z.confidence;
	float CF2 = DC->estimated.vel_z.confidence;
	float CF3 = DC->estimated.a_z.confidence;

	float RIS = CSF1_1(x1)*W1_1*CF1 + CSF2_1(x2)*W2_1*CF2 + CSF3_1(x3)*W3_1*CF3;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_1))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_1) ? RFILL_1 : RLEAK_1;

	ACC_1 = fmaxf(0.0f, ACC_1 + R*dt*(RIS-CT_1));
	prev_sistemTimeri_1 = sistemTimeri;

	if(ACC_1 >= TT_1){
		AF_1 = 1;
		s2_start_time = sistemTimeri; // S2 Faz süresi kronometresi başlatıldı
		return 1;
	} else {
		return 0;
	}
}


uint8_t AyrilmaliMi(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_2==1){return 0;}
	if(prev_sistemTimeri_2 == 0){ prev_sistemTimeri_2 = sistemTimeri; return 0; }

	float x1 = DC->estimated.pos_z.value;
	float x2 = ((float)(sistemTimeri - s2_start_time)) * SISTEM_TIMERI_TIMESTEP; // Geçen süreyi saniyeye çevir

	float CF1 = DC->estimated.pos_z.confidence;
	float CF2 = 1.0f; // Zaman bilgisinin güveni her zaman %100'dür

	float RIS = CSF1_2(x1)*W1_2*CF1 + CSF2_2(x2)*W2_2*CF2;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_2))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_2) ? RFILL_2 : RLEAK_2;

	ACC_2 = fmaxf(0.0f, ACC_2 + R*dt*(RIS-CT_2));
	prev_sistemTimeri_2 = sistemTimeri;

	if(ACC_2 >= TT_2){
		AF_2 = 1;
		s3_start_time = sistemTimeri; // S3 Faz süresi kronometresi başlatıldı
		return 1;
	} else {
		return 0;
	}
}


uint8_t AyrildiMi(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_3==1){return 0;}
	if(prev_sistemTimeri_3 == 0){ prev_sistemTimeri_3 = sistemTimeri; return 0; }

	float x1 = DC->estimated.a_z.value;
	float x2 = ((float)(sistemTimeri - s3_start_time)) * SISTEM_TIMERI_TIMESTEP;

	float CF1 = DC->estimated.a_z.confidence;
	float CF2 = 1.0f;

	float RIS = CSF1_3(x1)*W1_3*CF1 + CSF2_3(x2)*W2_3*CF2;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_3))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_3) ? RFILL_3 : RLEAK_3;

	ACC_3 = fmaxf(0.0f, ACC_3 + R*dt*(RIS-CT_3));
	prev_sistemTimeri_3 = sistemTimeri;

	if(ACC_3 >= TT_3){
		AF_3 = 1;
		s4_start_time = sistemTimeri; // S4 Faz süresi kronometresi başlatıldı
		return 1;
	} else {
		return 0;
	}
}


uint8_t ApamAcilmaliMi(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_4==1){return 0;}
	if(prev_sistemTimeri_4 == 0){ prev_sistemTimeri_4 = sistemTimeri; return 0; }

	float x1 = DC->estimated.vel_z.value;
	float x2 = DC->estimated.pos_z.value;

	float CF1 = DC->estimated.vel_z.confidence;
	float CF2 = DC->estimated.pos_z.confidence;

	float RIS = CSF1_4(x1)*W1_4*CF1 + CSF2_4(x2)*W2_4*CF2;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_4))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_4) ? RFILL_4 : RLEAK_4;

	ACC_4 = fmaxf(0.0f, ACC_4 + R*dt*(RIS-CT_4));
	prev_sistemTimeri_4 = sistemTimeri;

	if(ACC_4 >= TT_4){
		AF_4 = 1;
		return 1;
	} else {
		return 0;
	}
}


uint8_t HoverStart(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_5==1){return 0;}
	if(prev_sistemTimeri_5 == 0){ prev_sistemTimeri_5 = sistemTimeri; return 0; }

	float x1 = DC->estimated.vel_z.value;
	float x2 = DC->estimated.pos_z.value;
	float x3 = DC->estimated.a_z.value;

	float CF1 = DC->estimated.vel_z.confidence;
	float CF2 = DC->estimated.pos_z.confidence;
	float CF3 = DC->estimated.a_z.confidence;

	float RIS = CSF1_5(x1)*W1_5*CF1 + CSF2_5(x2)*W2_5*CF2 + CSF3_5(x3)*W3_5*CF3;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_5))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_5) ? RFILL_5 : RLEAK_5;

	ACC_5 = fmaxf(0.0f, ACC_5 + R*dt*(RIS-CT_5));
	prev_sistemTimeri_5 = sistemTimeri;

	if(ACC_5 >= TT_5){
		AF_5 = 1;
		hover_start_time = sistemTimeri; // Hover Kronometresi başlatıldı
		return 1;
	} else {
		return 0;
	}
}


uint8_t HoverExit(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_6==1){return 0;}
	if(prev_sistemTimeri_6 == 0){ prev_sistemTimeri_6 = sistemTimeri; return 0; }

	float x1 = ((float)(sistemTimeri - hover_start_time)) * SISTEM_TIMERI_TIMESTEP;
	float x2 = DC->estimated.pos_z.value;
	float x3 = ((float)(sistemTimeri - s4_start_time)) * SISTEM_TIMERI_TIMESTEP;

	float CF1 = 1.0f;
	float CF2 = DC->estimated.pos_z.confidence;
	float CF3 = 1.0f;

	float RIS = CSF1_6(x1)*W1_6*CF1 + CSF2_6(x2)*W2_6*CF2 + CSF3_6(x3)*W3_6*CF3;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_6))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_6) ? RFILL_6 : RLEAK_6;

	ACC_6 = fmaxf(0.0f, ACC_6 + R*dt*(RIS-CT_6));
	prev_sistemTimeri_6 = sistemTimeri;

	if(ACC_6 >= TT_6){
		AF_6 = 1;
		return 1;
	} else {
		return 0;
	}
}


uint8_t FallingStop(DataCenter *DC, uint32_t sistemTimeri){
	if(AF_7==1){return 0;}
	if(prev_sistemTimeri_7 == 0){ prev_sistemTimeri_7 = sistemTimeri; return 0; }

	float x1 = DC->estimated.vel_z.value;
	float x2 = DC->estimated.pos_z.value;
	float x3 = ((float)(sistemTimeri - s4_start_time)) * SISTEM_TIMERI_TIMESTEP;

	float CF1 = DC->estimated.vel_z.confidence;
	float CF2 = DC->estimated.pos_z.confidence;
	float CF3 = 1.0f;

	float RIS = CSF1_7(x1)*W1_7*CF1 + CSF2_7(x2)*W2_7*CF2 + CSF3_7(x3)*W3_7*CF3;

	float dt = ((float)(sistemTimeri - prev_sistemTimeri_7))*SISTEM_TIMERI_TIMESTEP;
	float R = (RIS >= CT_7) ? RFILL_7 : RLEAK_7;

	ACC_7 = fmaxf(0.0f, ACC_7 + R*dt*(RIS-CT_7));
	prev_sistemTimeri_7 = sistemTimeri;

	if(ACC_7 >= TT_7){
		AF_7 = 1;
		return 1;
	} else {
		return 0;
	}
}
