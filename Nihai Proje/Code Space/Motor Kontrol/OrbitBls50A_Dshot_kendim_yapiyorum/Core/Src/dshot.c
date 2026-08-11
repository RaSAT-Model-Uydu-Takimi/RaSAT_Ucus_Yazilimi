/*
 * dshot.c
 *
 *  Created on: Aug 9, 2026
 *      Author: abduh
 */
#include "dshot.h"

uint16_t dshot_m1_buffer[18]; // 16 bit veri + 2 bit sessizlik (LOW) süresi
uint16_t dshot_m2_buffer[18]; // 16 bit veri + 2 bit sessizlik (LOW) süresi
uint16_t dshot_m3_buffer[18]; // 16 bit veri + 2 bit sessizlik (LOW) süresi
uint16_t dshot_m4_buffer[18]; // 16 bit veri + 2 bit sessizlik (LOW) süresi
volatile uint8_t dshot_ready = 1; // Başlangıçta gönderime hazır (1)

uint32_t adc_raw_value = 0;   // ADC'den gelen ham 0-4095 arası sayı
float current_ampere = 0.0f;  // Hesaplanan gerçek Amper değeri

TIM_HandleTypeDef *signalGeneratorTimer;
TIM_HandleTypeDef *communicatorTimer;
ADC_HandleTypeDef *currentADC;

void Dshot_Init_1(TIM_HandleTypeDef *htim_signalGenerator, TIM_HandleTypeDef *htim_communicator){
	signalGeneratorTimer = htim_signalGenerator;
	communicatorTimer = htim_communicator;
}

void Dshot_Init_2(TIM_HandleTypeDef *htim_signalGenerator, TIM_HandleTypeDef *htim_communicator, ADC_HandleTypeDef *adc_current){
	signalGeneratorTimer = htim_signalGenerator;
	communicatorTimer = htim_communicator;
	currentADC = adc_current;

}

void Motors_Arm(uint32_t firstDelay_ms, uint32_t secondDelay_ms){
	HAL_Delay(firstDelay_ms);
	HAL_TIM_Base_Start_IT(communicatorTimer); // Pointer olduğu için '&' koymuyoruz
	HAL_Delay(secondDelay_ms);
}

void Motors_Disarm(uint32_t firstDelay_ms, uint32_t secondDelay_ms){
	HAL_Delay(firstDelay_ms);
	HAL_TIM_Base_Stop_IT(communicatorTimer);
	HAL_Delay(secondDelay_ms);
}

void Dshot_Frame_End_Callback(TIM_HandleTypeDef *htim){
	// Fonksiyon her bittiğinde sıfırlanmaması için sayacı 'static' tanımlıyoruz
	static uint8_t finished_channels = 0;

	if(htim == signalGeneratorTimer)
	{
		// DİKKAT: Pini ve Timer'ı KESİNLİKLE kapatmıyoruz.
		// Sadece arka plandaki "DMA İsteğini (Request)" susturuyoruz.
		// Böylece pin aktif olarak 0 Volt'a çekilmeye devam ediyor!

		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
			__HAL_TIM_DISABLE_DMA(signalGeneratorTimer, TIM_DMA_CC1);

		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
			__HAL_TIM_DISABLE_DMA(signalGeneratorTimer, TIM_DMA_CC2);

		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
			__HAL_TIM_DISABLE_DMA(signalGeneratorTimer, TIM_DMA_CC3);

		else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
			__HAL_TIM_DISABLE_DMA(signalGeneratorTimer, TIM_DMA_CC4);

		finished_channels++;

		if (finished_channels == 4)
		{
			dshot_ready = 1;       // Ana döngüdeki kilidi aç
			finished_channels = 0; // Bir sonraki tur için sayacı sıfırla

			// KÜTÜPHANEYİ SIFIRLA: HAL'a işlemlerin bittiğini ve yeni komuta hazır olduğunu bildir
			signalGeneratorTimer->State = HAL_TIM_STATE_READY;

			// Eğer CubeIDE'nin yeni sürümlerinden birini kullanıyorsan kanalları da sıfırlamalısın:
			signalGeneratorTimer->ChannelState[0] = HAL_TIM_CHANNEL_STATE_READY;
			signalGeneratorTimer->ChannelState[1] = HAL_TIM_CHANNEL_STATE_READY;
			signalGeneratorTimer->ChannelState[2] = HAL_TIM_CHANNEL_STATE_READY;
			signalGeneratorTimer->ChannelState[3] = HAL_TIM_CHANNEL_STATE_READY;
		}
	}
}




void Dshot_Communicator_Timer_Callback(TIM_HandleTypeDef *htim, uint16_t signals[4]){

	if (htim == communicatorTimer)
	{

		if (dshot_ready == 1)
		{
		  //watchdog=6;
			dshot_ready = 0; // Bayrağı indir

			// Motorlara göndermek istediğin gaz değerini (örneğin 1046) paketlere çevir.
			// NOT: DSHOT protokolünde 0 "Silahlı değil / Dur", 48 "Minimum Gaz", 2047 "Tam Gaz" demektir.
			DSHOT_Prepare_Packet(signals[0], dshot_m1_buffer);
			DSHOT_Prepare_Packet(signals[1], dshot_m2_buffer);
			DSHOT_Prepare_Packet(signals[2], dshot_m3_buffer);
			DSHOT_Prepare_Packet(signals[3], dshot_m4_buffer);

			__HAL_TIM_SET_COUNTER(signalGeneratorTimer, 0);

			// Hazırlanan paketleri DMA'ya teslim et
			HAL_TIM_PWM_Start_DMA(signalGeneratorTimer, TIM_CHANNEL_1, (uint32_t *)dshot_m1_buffer, (sizeof(dshot_m1_buffer) / sizeof(dshot_m1_buffer[0])));
			HAL_TIM_PWM_Start_DMA(signalGeneratorTimer, TIM_CHANNEL_2, (uint32_t *)dshot_m2_buffer, (sizeof(dshot_m2_buffer) / sizeof(dshot_m2_buffer[0])));
			HAL_TIM_PWM_Start_DMA(signalGeneratorTimer, TIM_CHANNEL_3, (uint32_t *)dshot_m3_buffer, (sizeof(dshot_m3_buffer) / sizeof(dshot_m3_buffer[0])));
			HAL_TIM_PWM_Start_DMA(signalGeneratorTimer, TIM_CHANNEL_4, (uint32_t *)dshot_m4_buffer, (sizeof(dshot_m4_buffer) / sizeof(dshot_m4_buffer[0])));

		}


	}


}





uint32_t Read_RawADC_Current_Value(){
	HAL_ADC_Start(currentADC); // ADC'yi uyandır ve ölçüme başla

	// Ölçümün bitmesini bekle (Maksimum 1 milisaniye timeout veriyoruz)
	if (HAL_ADC_PollForConversion(currentADC, 10) == HAL_OK)
	{
	  // Ölçüm bitti, ham değeri (0-4095) işlemcinin hafızasına al
	  adc_raw_value = HAL_ADC_GetValue(currentADC);

	  HAL_ADC_Stop(currentADC);
	  return adc_raw_value;
	}
	else{  HAL_ADC_Stop(currentADC); return 0;}

	 // Güç tasarrufu ve reset için ADC'yi durdur

}


float Get_ESC_Current(uint32_t adcrawvalue){
	current_ampere= adcrawvalue * ADCRAW_to_AMPER_FACTOR;
	return current_ampere;
}





/**
 Gaz değerini (0-2047) alıp, CRC ekler ve DMA'nın göndereceği diziye dönüştürür.
 throttle: 0 ile 2047 arasında gaz değeri (0: Dur, 48-2047: Çalış)
 buffer: Hedef DMA dizisinin pointer'ı (örn: dshot_m1_buffer)
 */
void DSHOT_Prepare_Packet(uint16_t throttle, uint16_t* buffer)
{
    // DSHOT gaz değeri maksimum 2047 olabilir (11-bit sınırı)
    if (throttle > 2047) {
        throttle = 2047;
    }

    // 1. Gaz değerini 1 bit sola kaydır ve Telemetri bitini (0) ekle
    // (throttle << 1) işlemi değeri sola kaydırır, sağda açılan boşluğa | 0 ile telemetriyi koyarız.
    uint16_t packet = (throttle << 1) | 0;

    // 2. CRC (Hata kontrol) değerini hesapla (Senin bulduğun kusursuz formül)
    uint16_t crc = (packet ^ (packet >> 4) ^ (packet >> 8)) & 0x0F;

    // 3. CRC'yi paketin en sağına (4 bit boşluk açarak) ekle. Toplam 16 bitlik veri hazır!
    uint16_t frame = (packet << 4) | crc;

    // 4. Bu 16 bitlik ham veriyi Timer'ın anlayacağı SIFIR ve BIR tiklerine çevir
    for (int i = 0; i < 16; i++)
    {
        // Veriyi en anlamlı bitten (MSB - 15. bit) LSB'ye doğru tek tek sorguluyoruz
        // (0x8000 binary olarak 1000000000000000 demektir, i arttıkça bu '1' sağa kayar)
        if (frame & (0x8000 >> i)) {
            buffer[i] = BIR_BITI_TICK;   // O bit '1' ise %75 Duty Cycle bas
        } else {
            buffer[i] = SIFIR_BITI_TICK; // O bit '0' ise %37.5 Duty Cycle bas
        }
    }

    // 5. Sessizlik süresi (Inter-Frame Gap) için son iki biti donanımsal olarak kapat
    buffer[16] = 0;
    buffer[17] = 0;
}


